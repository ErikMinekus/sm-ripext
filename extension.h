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

#ifndef _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_
#define _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_

#include <curl/curl.h>
#include <jansson.h>
#include <queue>
#include <sm_stringhashmap.h>
#include <stdlib.h>
#include <string.h>
#include "smsdk_ext.h"

typedef StringHashMap<ke::AString> HTTPHeaderMap;

struct HTTPRequest {
	HTTPRequest(const char *method, const char *endpoint, json_t *data = NULL)
		: method(method), endpoint(endpoint), data(data), body(NULL), pos(0), size(0)
	{
		if (data != NULL)
		{
			body = json_dumps(data, 0);
			size = (body == NULL) ? 0 : strlen(body);
		}
	}

	const char *method;
	const char *endpoint;
	json_t *data;

	char *body;
	size_t pos;
	size_t size;
};

struct HTTPResponse {
	HTTPResponse() : status(0), data(NULL), hndlData(BAD_HANDLE), body((char *)malloc(1)), size(0) {}

	long status;
	json_t *data;
	Handle_t hndlData;

	char *body;
	size_t size;
};

struct HTTPRequestCallback {
	HTTPRequestCallback(IChangeableForward *forward, struct HTTPResponse response, cell_t value)
		: forward(forward), response(response), value(value) {}

	IChangeableForward *forward;
	struct HTTPResponse response;
	cell_t value;
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
	void AddCallbackToQueue(const struct HTTPRequestCallback &callback);
	void RunFrame();
private:
	IMutex *callbackMutex;
	std::queue<struct HTTPRequestCallback> callbackQueue;
};

class HTTPClientObjectHandler : public IHandleTypeDispatch
{
public:
	void OnHandleDestroy(HandleType_t type, void *object);
};

class HTTPResponseObjectHandler : public IHandleTypeDispatch
{
public:
	void OnHandleDestroy(HandleType_t type, void *object);
};

class JSONObjectHandler : public IHandleTypeDispatch
{
public:
	void OnHandleDestroy(HandleType_t type, void *object);
};

extern RipExt g_RipExt;

extern HTTPClientObjectHandler	g_HTTPClientObjectHandler;
extern HandleType_t				htHTTPClientObject;

extern HTTPResponseObjectHandler	g_HTTPResponseObjectHandler;
extern HandleType_t					htHTTPResponseObject;

extern JSONObjectHandler	g_JSONObjectHandler;
extern HandleType_t			htJSONObject;

extern const sp_nativeinfo_t curl_natives[];
extern const sp_nativeinfo_t json_natives[];

#endif // _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_
