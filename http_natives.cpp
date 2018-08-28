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

static cell_t CreateClient(IPluginContext *pContext, const cell_t *params)
{
	char *baseURL;
	pContext->LocalToString(params[1], &baseURL);

	HTTPClient *client = new HTTPClient(baseURL);

	Handle_t hndl = handlesys->CreateHandle(htHTTPClientObject, client, pContext->GetIdentity(), myself->GetIdentity(), NULL);
	if (hndl == BAD_HANDLE)
	{
		pContext->ThrowNativeError("Could not create HTTP client handle.");
		return BAD_HANDLE;
	}

	return hndl;
}

static cell_t SetClientHeader(IPluginContext *pContext, const cell_t *params)
{
	HandleError err;
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());

	HTTPClient *client;
	Handle_t hndlClient = static_cast<Handle_t>(params[1]);
	if ((err=handlesys->ReadHandle(hndlClient, htHTTPClientObject, &sec, (void **)&client)) != HandleError_None)
	{
		return pContext->ThrowNativeError("Invalid HTTP client handle %x (error %d)", hndlClient, err);
	}

	char *name;
	pContext->LocalToString(params[2], &name);

	char *value;
	pContext->LocalToString(params[3], &value);

	client->SetHeader(name, value);

	return 1;
}

static cell_t GetRequest(IPluginContext *pContext, const cell_t *params)
{
	HandleError err;
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());

	HTTPClient *client;
	Handle_t hndlClient = static_cast<Handle_t>(params[1]);
	if ((err=handlesys->ReadHandle(hndlClient, htHTTPClientObject, &sec, (void **)&client)) != HandleError_None)
	{
		return pContext->ThrowNativeError("Invalid HTTP client handle %x (error %d)", hndlClient, err);
	}

	char *endpoint;
	pContext->LocalToString(params[2], &endpoint);

	IPluginFunction *callback = pContext->GetFunctionById(params[3]);
	cell_t value = params[4];

	struct HTTPRequest request("GET", endpoint);

	client->Request(request, callback, value);

	return 1;
}

static cell_t PostRequest(IPluginContext *pContext, const cell_t *params)
{
	HandleError err;
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());

	HTTPClient *client;
	Handle_t hndlClient = static_cast<Handle_t>(params[1]);
	if ((err=handlesys->ReadHandle(hndlClient, htHTTPClientObject, &sec, (void **)&client)) != HandleError_None)
	{
		return pContext->ThrowNativeError("Invalid HTTP client handle %x (error %d)", hndlClient, err);
	}

	char *endpoint;
	pContext->LocalToString(params[2], &endpoint);

	json_t *data;
	Handle_t hndlData = static_cast<Handle_t>(params[3]);
	if ((err=handlesys->ReadHandle(hndlData, htJSONObject, &sec, (void **)&data)) != HandleError_None)
	{
		return pContext->ThrowNativeError("Invalid data handle %x (error %d)", hndlData, err);
	}

	IPluginFunction *callback = pContext->GetFunctionById(params[4]);
	cell_t value = params[5];

	struct HTTPRequest request("POST", endpoint, data);

	client->Request(request, callback, value);

	return 1;
}

static cell_t PutRequest(IPluginContext *pContext, const cell_t *params)
{
	HandleError err;
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());

	HTTPClient *client;
	Handle_t hndlClient = static_cast<Handle_t>(params[1]);
	if ((err=handlesys->ReadHandle(hndlClient, htHTTPClientObject, &sec, (void **)&client)) != HandleError_None)
	{
		return pContext->ThrowNativeError("Invalid HTTP client handle %x (error %d)", hndlClient, err);
	}

	char *endpoint;
	pContext->LocalToString(params[2], &endpoint);

	json_t *data;
	Handle_t hndlData = static_cast<Handle_t>(params[3]);
	if ((err=handlesys->ReadHandle(hndlData, htJSONObject, &sec, (void **)&data)) != HandleError_None)
	{
		return pContext->ThrowNativeError("Invalid data handle %x (error %d)", hndlData, err);
	}

	IPluginFunction *callback = pContext->GetFunctionById(params[4]);
	cell_t value = params[5];

	struct HTTPRequest request("PUT", endpoint, data);

	client->Request(request, callback, value);

	return 1;
}

static cell_t PatchRequest(IPluginContext *pContext, const cell_t *params)
{
	HandleError err;
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());

	HTTPClient *client;
	Handle_t hndlClient = static_cast<Handle_t>(params[1]);
	if ((err=handlesys->ReadHandle(hndlClient, htHTTPClientObject, &sec, (void **)&client)) != HandleError_None)
	{
		return pContext->ThrowNativeError("Invalid HTTP client handle %x (error %d)", hndlClient, err);
	}

	char *endpoint;
	pContext->LocalToString(params[2], &endpoint);

	json_t *data;
	Handle_t hndlData = static_cast<Handle_t>(params[3]);
	if ((err=handlesys->ReadHandle(hndlData, htJSONObject, &sec, (void **)&data)) != HandleError_None)
	{
		return pContext->ThrowNativeError("Invalid data handle %x (error %d)", hndlData, err);
	}

	IPluginFunction *callback = pContext->GetFunctionById(params[4]);
	cell_t value = params[5];

	struct HTTPRequest request("PATCH", endpoint, data);

	client->Request(request, callback, value);

	return 1;
}

static cell_t DeleteRequest(IPluginContext *pContext, const cell_t *params)
{
	HandleError err;
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());

	HTTPClient *client;
	Handle_t hndlClient = static_cast<Handle_t>(params[1]);
	if ((err=handlesys->ReadHandle(hndlClient, htHTTPClientObject, &sec, (void **)&client)) != HandleError_None)
	{
		return pContext->ThrowNativeError("Invalid HTTP client handle %x (error %d)", hndlClient, err);
	}

	char *endpoint;
	pContext->LocalToString(params[2], &endpoint);

	IPluginFunction *callback = pContext->GetFunctionById(params[3]);
	cell_t value = params[4];

	struct HTTPRequest request("DELETE", endpoint);

	client->Request(request, callback, value);

	return 1;
}

static cell_t GetClientConnectTimeout(IPluginContext *pContext, const cell_t *params)
{
	HandleError err;
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());

	HTTPClient *client;
	Handle_t hndlClient = static_cast<Handle_t>(params[1]);
	if ((err=handlesys->ReadHandle(hndlClient, htHTTPClientObject, &sec, (void **)&client)) != HandleError_None)
	{
		return pContext->ThrowNativeError("Invalid HTTP client handle %x (error %d)", hndlClient, err);
	}

	return client->GetConnectTimeout();
}

static cell_t SetClientConnectTimeout(IPluginContext *pContext, const cell_t *params)
{
	HandleError err;
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());

	HTTPClient *client;
	Handle_t hndlClient = static_cast<Handle_t>(params[1]);
	if ((err=handlesys->ReadHandle(hndlClient, htHTTPClientObject, &sec, (void **)&client)) != HandleError_None)
	{
		return pContext->ThrowNativeError("Invalid HTTP client handle %x (error %d)", hndlClient, err);
	}

	client->SetConnectTimeout(params[2]);

	return 1;
}

static cell_t GetClientFollowLocation(IPluginContext *pContext, const cell_t *params)
{
	HandleError err;
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());

	HTTPClient *client;
	Handle_t hndlClient = static_cast<Handle_t>(params[1]);
	if ((err=handlesys->ReadHandle(hndlClient, htHTTPClientObject, &sec, (void **)&client)) != HandleError_None)
	{
		return pContext->ThrowNativeError("Invalid HTTP client handle %x (error %d)", hndlClient, err);
	}

	return client->GetFollowLocation();
}

static cell_t SetClientFollowLocation(IPluginContext *pContext, const cell_t *params)
{
	HandleError err;
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());

	HTTPClient *client;
	Handle_t hndlClient = static_cast<Handle_t>(params[1]);
	if ((err=handlesys->ReadHandle(hndlClient, htHTTPClientObject, &sec, (void **)&client)) != HandleError_None)
	{
		return pContext->ThrowNativeError("Invalid HTTP client handle %x (error %d)", hndlClient, err);
	}

	client->SetFollowLocation(params[2]);

	return 1;
}

static cell_t GetClientTimeout(IPluginContext *pContext, const cell_t *params)
{
	HandleError err;
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());

	HTTPClient *client;
	Handle_t hndlClient = static_cast<Handle_t>(params[1]);
	if ((err=handlesys->ReadHandle(hndlClient, htHTTPClientObject, &sec, (void **)&client)) != HandleError_None)
	{
		return pContext->ThrowNativeError("Invalid HTTP client handle %x (error %d)", hndlClient, err);
	}

	return client->GetTimeout();
}

static cell_t SetClientTimeout(IPluginContext *pContext, const cell_t *params)
{
	HandleError err;
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());

	HTTPClient *client;
	Handle_t hndlClient = static_cast<Handle_t>(params[1]);
	if ((err=handlesys->ReadHandle(hndlClient, htHTTPClientObject, &sec, (void **)&client)) != HandleError_None)
	{
		return pContext->ThrowNativeError("Invalid HTTP client handle %x (error %d)", hndlClient, err);
	}

	client->SetTimeout(params[2]);

	return 1;
}

static cell_t GetResponseData(IPluginContext *pContext, const cell_t *params)
{
	HandleError err;
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());

	struct HTTPResponse *response;
	Handle_t hndlResponse = static_cast<Handle_t>(params[1]);
	if ((err=handlesys->ReadHandle(hndlResponse, htHTTPResponseObject, &sec, (void **)&response)) != HandleError_None)
	{
		pContext->ThrowNativeError("Invalid HTTP response handle %x (error %d)", hndlResponse, err);
		return BAD_HANDLE;
	}

	if (response->data == NULL)
	{
		return BAD_HANDLE;
	}

	/* Return the same handle every time we get the HTTP response data */
	if (response->hndlData == BAD_HANDLE)
	{
		response->hndlData = handlesys->CreateHandleEx(htJSONObject, response->data, &sec, NULL, NULL);
		if (response->hndlData == BAD_HANDLE)
		{
			pContext->ThrowNativeError("Could not create data handle.");
			return BAD_HANDLE;
		}
	}

	return response->hndlData;
}

static cell_t GetResponseStatus(IPluginContext *pContext, const cell_t *params)
{
	HandleError err;
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());

	struct HTTPResponse *response;
	Handle_t hndlResponse = static_cast<Handle_t>(params[1]);
	if ((err=handlesys->ReadHandle(hndlResponse, htHTTPResponseObject, &sec, (void **)&response)) != HandleError_None)
	{
		return pContext->ThrowNativeError("Invalid HTTP response handle %x (error %d)", hndlResponse, err);
	}

	return response->status;
}


const sp_nativeinfo_t http_natives[] =
{
	{"HTTPClient.HTTPClient",			CreateClient},
	{"HTTPClient.SetHeader",			SetClientHeader},
	{"HTTPClient.Get",					GetRequest},
	{"HTTPClient.Post",					PostRequest},
	{"HTTPClient.Put",					PutRequest},
	{"HTTPClient.Patch",				PatchRequest},
	{"HTTPClient.Delete",				DeleteRequest},
	{"HTTPClient.ConnectTimeout.get",	GetClientConnectTimeout},
	{"HTTPClient.ConnectTimeout.set",	SetClientConnectTimeout},
	{"HTTPClient.FollowLocation.get",	GetClientFollowLocation},
	{"HTTPClient.FollowLocation.set",	SetClientFollowLocation},
	{"HTTPClient.Timeout.get",			GetClientTimeout},
	{"HTTPClient.Timeout.set",			SetClientTimeout},
	{"HTTPResponse.Data.get",			GetResponseData},
	{"HTTPResponse.Status.get",			GetResponseStatus},

	{NULL,								NULL}
};
