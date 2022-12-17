/**
 * vim: set ts=4 :
 * =============================================================================
 * SourceMod REST in Pawn Extension
 * Copyright 2017-2022 Erik Minekus
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
#include "httprequest.h"
#include "queue.h"

// Limit the max processing request per tick
#define MAX_PROCESS 10

RipExt g_RipExt;		/**< Global singleton for extension's main interface */

SMEXT_LINK(&g_RipExt);

LockedQueue<IHTTPContext *> g_RequestQueue;
LockedQueue<IHTTPContext *> g_RequestQueueImmediately;
LockedQueue<IHTTPContext *> g_CompletedRequestQueue;

CURLM *g_Curl;
uv_loop_t *g_Loop;
uv_thread_t g_Thread;
uv_timer_t g_Timeout;

uv_async_t g_AsyncPerformRequests;
uv_async_t g_AsyncPerformRequestsImmediately;
uv_async_t g_AsyncStopLoop;

HTTPClientHandler	g_HTTPClientHandler;
HandleType_t		htHTTPClient;

HTTPRequestHandler	g_HTTPRequestHandler;
HandleType_t		htHTTPRequest;

HTTPResponseHandler	g_HTTPResponseHandler;
HandleType_t			htHTTPResponse;

JSONHandler		g_JSONHandler;
HandleType_t		htJSON;

JSONObjectKeysHandler	g_JSONObjectKeysHandler;
HandleType_t			htJSONObjectKeys;

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
		curl_multi_remove_handle(g_Curl, curl);

		IHTTPContext *context;
		curl_easy_getinfo(curl, CURLINFO_PRIVATE, &context);

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
	IHTTPContext *context;
	// Limiter
	int count = 0;

	while (!g_RequestQueue.Empty() && count < MAX_PROCESS)
	{
		context = g_RequestQueue.Pop();

		if (!context->InitCurl())
		{
			delete context;
			continue;
		}

		curl_multi_add_handle(g_Curl, context->curl);
		count++;
	}

	g_RequestQueue.Unlock();
}

static void AsyncPerformRequestsImmediately(uv_async_t *handle)
{
	g_RequestQueueImmediately.Lock();
	IHTTPContext *context;

	while (!g_RequestQueueImmediately.Empty())
	{
		context = g_RequestQueueImmediately.Pop();

		if (!context->InitCurl())
		{
			delete context;
			continue;
		}
		
		curl_multi_add_handle(g_Curl, context->curl);
	}

	g_RequestQueueImmediately.Unlock();
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
		IHTTPContext *context = g_CompletedRequestQueue.Pop();

		context->OnCompleted();
		delete context;

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
	uv_async_init(g_Loop, &g_AsyncPerformRequestsImmediately, &AsyncPerformRequestsImmediately);
	uv_async_init(g_Loop, &g_AsyncStopLoop, &AsyncStopLoop);
	uv_thread_create(&g_Thread, &EventLoop, NULL);

	/* Set up access rights for the 'HTTPRequest' handle type */
	HandleAccess haHTTPRequest;
	handlesys->InitAccessDefaults(NULL, &haHTTPRequest);
	haHTTPRequest.access[HandleAccess_Delete] = 0;

	/* Set up access rights for the 'HTTPResponse' handle type */
	HandleAccess haHTTPResponse;
	handlesys->InitAccessDefaults(NULL, &haHTTPResponse);
	haHTTPResponse.access[HandleAccess_Clone] = HANDLE_RESTRICT_IDENTITY;

	/* Set up access rights for the 'JSON' handle type */
	HandleAccess haJSON;
	handlesys->InitAccessDefaults(NULL, &haJSON);
	haJSON.access[HandleAccess_Delete] = 0;

	htHTTPClient = handlesys->CreateType("HTTPClient", &g_HTTPClientHandler, 0, NULL, NULL, myself->GetIdentity(), NULL);
	htHTTPRequest = handlesys->CreateType("HTTPRequest", &g_HTTPRequestHandler, 0, NULL, &haHTTPRequest, myself->GetIdentity(), NULL);
	htHTTPResponse = handlesys->CreateType("HTTPResponse", &g_HTTPResponseHandler, 0, NULL, &haHTTPResponse, myself->GetIdentity(), NULL);
	htJSON = handlesys->CreateType("JSON", &g_JSONHandler, 0, NULL, &haJSON, myself->GetIdentity(), NULL);
	htJSONObjectKeys = handlesys->CreateType("JSONObjectKeys", &g_JSONObjectKeysHandler, 0, NULL, NULL, myself->GetIdentity(), NULL);

	smutils->AddGameFrameHook(&FrameHook);
	smutils->BuildPath(Path_SM, caBundlePath, sizeof(caBundlePath), SM_RIPEXT_CA_BUNDLE_PATH);

	return true;
}

void RipExt::SDK_OnUnload()
{
	uv_async_send(&g_AsyncStopLoop);
	uv_thread_join(&g_Thread);
	uv_loop_close(g_Loop);

	curl_multi_cleanup(&g_Curl);
	curl_global_cleanup();

	handlesys->RemoveType(htHTTPClient, myself->GetIdentity());
	handlesys->RemoveType(htHTTPRequest, myself->GetIdentity());
	handlesys->RemoveType(htHTTPResponse, myself->GetIdentity());
	handlesys->RemoveType(htJSON, myself->GetIdentity());
	handlesys->RemoveType(htJSONObjectKeys, myself->GetIdentity());

	smutils->RemoveGameFrameHook(&FrameHook);
}

void RipExt::AddRequestToQueue(IHTTPContext *context)
{
	g_RequestQueue.Lock();
	g_RequestQueue.Push(context);
	g_RequestQueue.Unlock();
}

void RipExt::PerformRequest(IHTTPContext *context)
{
	g_RequestQueueImmediately.Lock();
	g_RequestQueueImmediately.Push(context);
	g_RequestQueueImmediately.Unlock();
	uv_async_send(&g_AsyncPerformRequestsImmediately);
}

void HTTPClientHandler::OnHandleDestroy(HandleType_t type, void *object)
{
	delete (HTTPClient *)object;
}

void HTTPRequestHandler::OnHandleDestroy(HandleType_t type, void *object)
{
	delete (HTTPRequest *)object;
}

void HTTPResponseHandler::OnHandleDestroy(HandleType_t type, void *object)
{
	/* Response objects are automatically cleaned up */
}

void JSONHandler::OnHandleDestroy(HandleType_t type, void *object)
{
	json_decref((json_t *)object);
}

void JSONObjectKeysHandler::OnHandleDestroy(HandleType_t type, void *object)
{
	delete (struct JSONObjectKeys *)object;
}
