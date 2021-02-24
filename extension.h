/**
 * vim: set ts=4 :
 * =============================================================================
 * SourceMod REST in Pawn Extension
 * Copyright 2017-2020 Erik Minekus
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

#ifndef _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_
#define _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_

#include <string>
#include <curl/curl.h>
#include <jansson.h>
#include <sm_stringhashmap.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>
#include "smsdk_ext.h"

#define SM_RIPEXT_CA_BUNDLE_PATH "configs/ripext/ca-bundle.crt"
#define SM_RIPEXT_USER_AGENT "sm-ripext/" SMEXT_CONF_VERSION

extern uv_loop_t *g_Loop;

typedef StringHashMap<std::string> HTTPHeaderMap;

class IHTTPContext
{
public:
	virtual bool InitCurl() = 0;
	virtual void OnCompleted() = 0;
	virtual ~IHTTPContext() {}

	CURL *curl;
};

struct CurlContext {
	CurlContext(curl_socket_t socket) : socket(socket)
	{
		uv_poll_init_socket(g_Loop, &poll_handle, socket);
		poll_handle.data = this;
	}

	void Destroy()
	{
		uv_poll_stop(&poll_handle);
		uv_close((uv_handle_t *)&poll_handle, OnClosed);
	}

	static void OnClosed(uv_handle_t *handle)
	{
		delete (CurlContext *)handle->data;
	}

	curl_socket_t socket;
	uv_poll_t poll_handle;
};

struct HTTPResponse {
	long status = 0;
	json_t *data = NULL;
	Handle_t hndlData = BAD_HANDLE;
	HTTPHeaderMap headers;

	char *body = NULL;
	size_t size = 0;
};

struct JSONObjectKeys {
	JSONObjectKeys(json_t *object) : object(object), iter(json_object_iter(object)) {}

	const char *GetKey()
	{
		return json_object_iter_key(iter);
	}

	void Next()
	{
		iter = json_object_iter_next(object, iter);
	}

private:
	json_t *object;
	void *iter;
};


/**
 * @brief Implementation of the REST in Pawn Extension.
 * Note: Uncomment one of the pre-defined virtual functions in order to use it.
 */
class RipExt : public SDKExtension
{
public:
	/**
	 * @brief This is called after the initial loading sequence has been processed.
	 *
	 * @param error		Error message buffer.
	 * @param maxlength	Size of error message buffer.
	 * @param late		Whether or not the module was loaded after map load.
	 * @return			True to succeed loading, false to fail.
	 */
	virtual bool SDK_OnLoad(char *error, size_t maxlength, bool late);

	/**
	 * @brief This is called right before the extension is unloaded.
	 */
	virtual void SDK_OnUnload();

	/**
	 * @brief This is called once all known extensions have been loaded.
	 * Note: It is is a good idea to add natives here, if any are provided.
	 */
	//virtual void SDK_OnAllLoaded();

	/**
	 * @brief Called when the pause state is changed.
	 */
	//virtual void SDK_OnPauseChange(bool paused);

	/**
	 * @brief this is called when Core wants to know if your extension is working.
	 *
	 * @param error		Error message buffer.
	 * @param maxlength	Size of error message buffer.
	 * @return			True if working, false otherwise.
	 */
	//virtual bool QueryRunning(char *error, size_t maxlength);
public:
#if defined SMEXT_CONF_METAMOD
	/**
	 * @brief Called when Metamod is attached, before the extension version is called.
	 *
	 * @param error			Error buffer.
	 * @param maxlength		Maximum size of error buffer.
	 * @param late			Whether or not Metamod considers this a late load.
	 * @return				True to succeed, false to fail.
	 */
	//virtual bool SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlength, bool late);

	/**
	 * @brief Called when Metamod is detaching, after the extension version is called.
	 * NOTE: By default this is blocked unless sent from SourceMod.
	 *
	 * @param error			Error buffer.
	 * @param maxlength		Maximum size of error buffer.
	 * @return				True to succeed, false to fail.
	 */
	//virtual bool SDK_OnMetamodUnload(char *error, size_t maxlength);

	/**
	 * @brief Called when Metamod's pause state is changing.
	 * NOTE: By default this is blocked unless sent from SourceMod.
	 *
	 * @param paused		Pause state being set.
	 * @param error			Error buffer.
	 * @param maxlength		Maximum size of error buffer.
	 * @return				True to succeed, false to fail.
	 */
	//virtual bool SDK_OnMetamodPauseChange(bool paused, char *error, size_t maxlength);
#endif
public:
	void AddRequestToQueue(IHTTPContext *context);

	char caBundlePath[PLATFORM_MAX_PATH];
};

class HTTPClientHandler : public IHandleTypeDispatch
{
public:
	void OnHandleDestroy(HandleType_t type, void *object);
};

class HTTPRequestHandler : public IHandleTypeDispatch
{
public:
	void OnHandleDestroy(HandleType_t type, void *object);
};

class HTTPResponseHandler : public IHandleTypeDispatch
{
public:
	void OnHandleDestroy(HandleType_t type, void *object);
};

class JSONHandler : public IHandleTypeDispatch
{
public:
	void OnHandleDestroy(HandleType_t type, void *object);
};

class JSONObjectKeysHandler : public IHandleTypeDispatch
{
public:
	void OnHandleDestroy(HandleType_t type, void *object);
};

extern RipExt g_RipExt;

extern HTTPClientHandler	g_HTTPClientHandler;
extern HandleType_t			htHTTPClient;

extern HTTPRequestHandler	g_HTTPRequestHandler;
extern HandleType_t			htHTTPRequest;

extern HTTPResponseHandler	g_HTTPResponseHandler;
extern HandleType_t				htHTTPResponse;

extern JSONHandler	g_JSONHandler;
extern HandleType_t		htJSON;

extern JSONObjectKeysHandler	g_JSONObjectKeysHandler;
extern HandleType_t				htJSONObjectKeys;

extern const sp_nativeinfo_t http_natives[];
extern const sp_nativeinfo_t json_natives[];

#endif // _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_
