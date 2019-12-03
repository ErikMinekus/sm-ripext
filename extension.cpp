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
#include "httpcontext.h"
#include "queue.h"

RipExt g_RipExt;		/**< Global singleton for extension's main interface */

SMEXT_LINK(&g_RipExt);

LockedQueue<HTTPContext *> g_RequestQueue;
LockedQueue<HTTPContext *> g_CompletedRequestQueue;

CURLM *g_Curl;
uv_loop_t *g_Loop;
uv_thread_t g_Thread;
uv_timer_t g_Timeout;

uv_async_t g_AsyncPerformRequests;
uv_async_t g_AsyncStopLoop;

HTTPClientObjectHandler	g_HTTPClientObjectHandler;
HandleType_t			htHTTPClientObject;

HTTPResponseObjectHandler	g_HTTPResponseObjectHandler;
HandleType_t				htHTTPResponseObject;

JSONObjectHandler		g_JSONObjectHandler;
HandleType_t			htJSONObject;

static void CheckCompletedRequests()
{
	CURLMsg *message;
	int pending;

	while ((message = curl_multi_info_read(g_Curl, &pending)))
	{
		if (message->msg != CURLMSG_DONE)
		{
			continue;
		}

		CURL *curl = message->easy_handle;
		CURLcode res = message->data.result;
		curl_multi_remove_handle(g_Curl, curl);

		HTTPContext *context;
		curl_easy_getinfo(curl, CURLINFO_PRIVATE, &context);

		if (res == CURLE_OK)
		{
			context->response.data = json_loads(context->response.body, 0, NULL);
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &context->response.status);
		}

		g_CompletedRequestQueue.Lock();
		g_CompletedRequestQueue.Push(context);
		g_CompletedRequestQueue.Unlock();
	}
}

static void PerformRequests(uv_timer_t *handle)
{
	int running;
	curl_multi_socket_action(g_Curl, CURL_SOCKET_TIMEOUT, 0, &running);

	CheckCompletedRequests();
}

static void CurlSocketActivity(uv_poll_t *handle, int status, int events)
{
	CurlContext *context = (CurlContext *)handle->data;
	int flags = 0;

	if (events & UV_READABLE)
	{
		flags |= CURL_CSELECT_IN;
	}
	if (events & UV_WRITABLE)
	{
		flags |= CURL_CSELECT_OUT;
	}

	int running;
	curl_multi_socket_action(g_Curl, context->socket, flags, &running);

	CheckCompletedRequests();
}

static int CurlSocketCallback(CURL *curl, curl_socket_t socket, int action, void *userdata, void *socketdata)
{
	CurlContext *context;
	int events = 0;

	switch (action)
	{
		case CURL_POLL_IN:
		case CURL_POLL_OUT:
		case CURL_POLL_INOUT:
			context = socketdata ? (CurlContext *)socketdata : new CurlContext(socket);
			curl_multi_assign(g_Curl, socket, context);

			if (action != CURL_POLL_IN)
			{
				events |= UV_WRITABLE;
			}
			if (action != CURL_POLL_OUT)
			{
				events |= UV_READABLE;
			}

			uv_poll_start(&context->poll_handle, events, &CurlSocketActivity);
			break;
		case CURL_POLL_REMOVE:
			if (socketdata)
			{
				context = (CurlContext *)socketdata;
				context->Destroy();

				curl_multi_assign(g_Curl, socket, NULL);
			}
			break;
	}

	return 0;
}

static int CurlTimeoutCallback(CURLM *multi, long timeout_ms, void *userdata)
{
	if (timeout_ms == -1)
	{
		uv_timer_stop(&g_Timeout);
		return 0;
	}

	uv_timer_start(&g_Timeout, &PerformRequests, timeout_ms, 0);
	return 0;
}

static void EventLoop(void *data)
{
	uv_run(g_Loop, UV_RUN_DEFAULT);
}

static void AsyncPerformRequests(uv_async_t *handle)
{
	g_RequestQueue.Lock();
	HTTPContext *context;

	while (!g_RequestQueue.Empty())
	{
		context = g_RequestQueue.Pop();

		curl_multi_add_handle(g_Curl, context->curl);
	}

	g_RequestQueue.Unlock();
}

static void AsyncStopLoop(uv_async_t *handle)
{
	uv_stop(g_Loop);
}

static void FrameHook(bool simulating)
{
	if (!g_RequestQueue.Empty())
	{
		uv_async_send(&g_AsyncPerformRequests);
	}

	if (!g_CompletedRequestQueue.Empty())
	{
		g_CompletedRequestQueue.Lock();
		HTTPContext *context;

		while (!g_CompletedRequestQueue.Empty())
		{
			context = g_CompletedRequestQueue.Pop();
			context->OnCompleted();

			delete context;
		}

		g_CompletedRequestQueue.Unlock();
	}
}

bool RipExt::SDK_OnLoad(char *error, size_t maxlength, bool late)
{
	sharesys->AddNatives(myself, http_natives);
	sharesys->AddNatives(myself, json_natives);
	sharesys->RegisterLibrary(myself, "ripext");

	/* Initialize cURL */
	CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
	if (res != CURLE_OK)
	{
		smutils->Format(error, maxlength, "%s", curl_easy_strerror(res));
		return false;
	}

	g_Curl = curl_multi_init();
	curl_multi_setopt(g_Curl, CURLMOPT_SOCKETFUNCTION, &CurlSocketCallback);
	curl_multi_setopt(g_Curl, CURLMOPT_TIMERFUNCTION, &CurlTimeoutCallback);

	/* Initialize libuv */
	g_Loop = uv_default_loop();
	uv_timer_init(g_Loop, &g_Timeout);
	uv_async_init(g_Loop, &g_AsyncPerformRequests, &AsyncPerformRequests);
	uv_async_init(g_Loop, &g_AsyncStopLoop, &AsyncStopLoop);
	uv_thread_create(&g_Thread, &EventLoop, NULL);

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

	smutils->AddGameFrameHook(&FrameHook);

	return true;
}

void RipExt::SDK_OnUnload()
{
	uv_async_send(&g_AsyncStopLoop);
	uv_thread_join(&g_Thread);
	uv_loop_close(g_Loop);

	curl_multi_cleanup(&g_Curl);
	curl_global_cleanup();

	handlesys->RemoveType(htHTTPClientObject, myself->GetIdentity());
	handlesys->RemoveType(htHTTPResponseObject, myself->GetIdentity());
	handlesys->RemoveType(htJSONObject, myself->GetIdentity());

	smutils->RemoveGameFrameHook(&FrameHook);
}

void RipExt::AddRequestToQueue(HTTPContext *context)
{
	g_RequestQueue.Lock();
	g_RequestQueue.Push(context);
	g_RequestQueue.Unlock();
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
