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
		delete client;

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

	client->Request("GET", endpoint, NULL, callback, value);

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

	client->Request("POST", endpoint, data, callback, value);

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

	client->Request("PUT", endpoint, data, callback, value);

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

	client->Request("PATCH", endpoint, data, callback, value);

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

	client->Request("DELETE", endpoint, NULL, callback, value);

	return 1;
}

static cell_t DownloadFile(IPluginContext *pContext, const cell_t *params)
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

	char *path;
	pContext->LocalToString(params[3], &path);

	IPluginFunction *callback = pContext->GetFunctionById(params[4]);
	cell_t value = params[5];

	client->DownloadFile(endpoint, path, callback, value);

	return 1;
}

static cell_t UploadFile(IPluginContext *pContext, const cell_t *params)
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

	char *path;
	pContext->LocalToString(params[3], &path);

	IPluginFunction *callback = pContext->GetFunctionById(params[4]);
	cell_t value = params[5];

	client->UploadFile(endpoint, path, callback, value);

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

	/* Return the same handle every time we get the HTTP response data */
	if (response->hndlData == BAD_HANDLE)
	{
		json_error_t error;
		response->data = json_loads(response->body, 0, &error);
		if (response->data == NULL)
		{
			pContext->ThrowNativeError("Invalid JSON in line %d, column %d: %s", error.line, error.column, error.text);
			return BAD_HANDLE;
		}

		response->hndlData = handlesys->CreateHandleEx(htJSONObject, response->data, &sec, NULL, NULL);
		if (response->hndlData == BAD_HANDLE)
		{
			json_decref(response->data);

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

static cell_t GetResponseHeader(IPluginContext *pContext, const cell_t *params)
{
	HandleError err;
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());

	struct HTTPResponse *response;
	Handle_t hndlResponse = static_cast<Handle_t>(params[1]);
	if ((err=handlesys->ReadHandle(hndlResponse, htHTTPResponseObject, &sec, (void **)&response)) != HandleError_None)
	{
		return pContext->ThrowNativeError("Invalid HTTP response handle %x (error %d)", hndlResponse, err);
	}

	char *name;
	pContext->LocalToString(params[2], &name);
	const ke::AString lowercaseName = ke::AString(name).lowercase();

	HTTPHeaderMap::Result header = response->headers.find(lowercaseName.chars());
	if (!header.found())
	{
		return 0;
	}

	pContext->StringToLocalUTF8(params[3], params[4], header->value.chars(), NULL);

	return 1;
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
	{"HTTPClient.DownloadFile",			DownloadFile},
	{"HTTPClient.UploadFile",			UploadFile},
	{"HTTPClient.ConnectTimeout.get",	GetClientConnectTimeout},
	{"HTTPClient.ConnectTimeout.set",	SetClientConnectTimeout},
	{"HTTPClient.FollowLocation.get",	GetClientFollowLocation},
	{"HTTPClient.FollowLocation.set",	SetClientFollowLocation},
	{"HTTPClient.Timeout.get",			GetClientTimeout},
	{"HTTPClient.Timeout.set",			SetClientTimeout},
	{"HTTPResponse.Data.get",			GetResponseData},
	{"HTTPResponse.Status.get",			GetResponseStatus},
	{"HTTPResponse.GetHeader",			GetResponseHeader},

	{NULL,								NULL}
};
