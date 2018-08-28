/**
 * vim: set ts=4 :
 * =============================================================================
 * SourceMod REST in Pawn Extension
 * Copyright 2017 Erik Minekus
 * =============================================================================
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "extension.h"
#include "httpclient.h"

RipExt g_RipExt;		/**< Global singleton for extension's main interface */

SMEXT_LINK(&g_RipExt);

HTTPClientObjectHandler	g_HTTPClientObjectHandler;
HandleType_t			htHTTPClientObject;

HTTPResponseObjectHandler	g_HTTPResponseObjectHandler;
HandleType_t				htHTTPResponseObject;

JSONObjectHandler		g_JSONObjectHandler;
HandleType_t			htJSONObject;

static void FrameHook(bool simulating)
{
	g_RipExt.RunFrame();
}

bool RipExt::SDK_OnLoad(char *error, size_t maxlength, bool late)
{
	sharesys->AddNatives(myself, http_natives);
	sharesys->AddNatives(myself, json_natives);
	sharesys->RegisterLibrary(myself, "ripext");

	smutils->AddGameFrameHook(&FrameHook);

	this->callbackMutex = threader->MakeMutex();

	/* Initialize cURL */
	CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
	if (res != CURLE_OK)
	{
		smutils->Format(error, maxlength, "%s", curl_easy_strerror(res));
		return false;
	}

	/* Set up access rights for the 'HTTPResponseObject' handle type */
	HandleAccess haHTTPResponseObject;
	haHTTPResponseObject.access[HandleAccess_Clone] = HANDLE_RESTRICT_IDENTITY|HANDLE_RESTRICT_OWNER;
	haHTTPResponseObject.access[HandleAccess_Delete] = HANDLE_RESTRICT_IDENTITY;
	haHTTPResponseObject.access[HandleAccess_Read] = HANDLE_RESTRICT_IDENTITY;

	/* Set up access rights for the 'JSONObject' handle type */
	HandleAccess haJSONObject;
	haJSONObject.access[HandleAccess_Clone] = 0;
	haJSONObject.access[HandleAccess_Delete] = 0;
	haJSONObject.access[HandleAccess_Read] = 0;

	htHTTPClientObject = handlesys->CreateType("HTTPClientObject", &g_HTTPClientObjectHandler, 0, NULL, NULL, myself->GetIdentity(), NULL);
	htHTTPResponseObject = handlesys->CreateType("HTTPResponseObject", &g_HTTPResponseObjectHandler, 0, NULL, &haHTTPResponseObject, myself->GetIdentity(), NULL);
	htJSONObject = handlesys->CreateType("JSONObject", &g_JSONObjectHandler, 0, NULL, &haJSONObject, myself->GetIdentity(), NULL);

	return true;
}

void RipExt::SDK_OnUnload()
{
	handlesys->RemoveType(htHTTPClientObject, myself->GetIdentity());
	handlesys->RemoveType(htHTTPResponseObject, myself->GetIdentity());
	handlesys->RemoveType(htJSONObject, myself->GetIdentity());

	smutils->RemoveGameFrameHook(&FrameHook);

	this->callbackMutex->DestroyThis();

	curl_global_cleanup();
}

void RipExt::AddCallbackToQueue(const struct HTTPRequestCallback &callback)
{
	this->callbackMutex->Lock();
	this->callbackQueue.push(callback);
	this->callbackMutex->Unlock();
}

void RipExt::RunFrame()
{
	if (this->callbackQueue.empty() || !this->callbackMutex->TryLock())
	{
		return;
	}

	struct HTTPRequestCallback &callback = this->callbackQueue.front();
	IChangeableForward *forward = callback.forward;
	struct HTTPResponse response = callback.response;
	cell_t value = callback.value;
	const ke::AString error = callback.error;

	this->callbackQueue.pop();

	/* Return early if the plugin was unloaded while the thread was running */
	if (forward->GetFunctionCount() == 0)
	{
		free(response.body);
		json_decref(response.data);

		forwards->ReleaseForward(forward);
		this->callbackMutex->Unlock();

		return;
	}

	HandleSecurity sec(NULL, myself->GetIdentity());
	Handle_t hndlResponse = handlesys->CreateHandleEx(htHTTPResponseObject, &response, &sec, NULL, NULL);
	if (hndlResponse == BAD_HANDLE)
	{
		free(response.body);
		json_decref(response.data);

		forwards->ReleaseForward(forward);
		this->callbackMutex->Unlock();

		smutils->LogError(myself, "Could not create HTTP response handle.");
		return;
	}

	forward->PushCell(hndlResponse);
	forward->PushCell(value);
	forward->PushString(error.chars());
	forward->Execute(NULL);

	handlesys->FreeHandle(hndlResponse, &sec);
	handlesys->FreeHandle(response.hndlData, &sec);

	forwards->ReleaseForward(forward);
	this->callbackMutex->Unlock();
}

void HTTPClientObjectHandler::OnHandleDestroy(HandleType_t type, void *object)
{
	delete (HTTPClient *)object;
}

void HTTPResponseObjectHandler::OnHandleDestroy(HandleType_t type, void *object)
{
	struct HTTPResponse *response = (struct HTTPResponse *)object;

	free(response->body);
}

void JSONObjectHandler::OnHandleDestroy(HandleType_t type, void *object)
{
	json_decref((json_t *)object);
}
