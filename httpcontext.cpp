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

#include "httpcontext.h"

static size_t ReadRequestBody(void *body, size_t size, size_t nmemb, void *userdata)
{
	size_t total = size * nmemb;
	struct HTTPRequest *request = (struct HTTPRequest *)userdata;
	size_t to_copy = (request->size - request->pos < total) ? request->size - request->pos : total;

	memcpy(body, &(request->body[request->pos]), to_copy);
	request->pos += to_copy;

	return to_copy;
}

static size_t WriteResponseBody(void *body, size_t size, size_t nmemb, void *userdata)
{
	size_t total = size * nmemb;
	struct HTTPResponse *response = (struct HTTPResponse *)userdata;

	char *temp = (char *)realloc(response->body, response->size + total + 1);
	if (temp == NULL)
	{
		return 0;
	}

	response->body = temp;
	memcpy(&(response->body[response->size]), body, total);
	response->size += total;
	response->body[response->size] = '\0';

	return total;
}

static size_t ReceiveResponseHeader(char *buffer, size_t size, size_t nmemb, void *userdata)
{
	size_t total = size * nmemb;
	struct HTTPResponse *response = (struct HTTPResponse *)userdata;

	char header[CURL_MAX_HTTP_HEADER] = {'\0'};
	strncat(header, buffer, total - 2); // Strip CRLF

	const char *match = strstr(header, ": ");
	if (match == NULL)
	{
		return total;
	}

	ke::AString name(header, match - header);
	ke::AString value(match + 2);

	response->headers.replace(name.lowercase().chars(), ke::Move(value));

	return total;
}

HTTPContext::HTTPContext(const ke::AString &method, const ke::AString &url, json_t *data,
	struct curl_slist *headers, IChangeableForward *forward, cell_t value,
	long connectTimeout, long followLocation, long timeout)
	: request(method, url, data), headers(headers), forward(forward), value(value)
{
	curl = curl_easy_init();
	if (curl == NULL)
	{
		forwards->ReleaseForward(forward);

		smutils->LogError(myself, "Could not initialize cURL session.");
		return;
	}

	char caBundlePath[PLATFORM_MAX_PATH];
	smutils->BuildPath(Path_SM, caBundlePath, sizeof(caBundlePath), SM_RIPEXT_CA_BUNDLE_PATH);

	if (request.method.compare("POST") == 0)
	{
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
	}
	else if (request.method.compare("PUT") == 0)
	{
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
	}
	else if (request.method.compare("PATCH") == 0)
	{
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, request.method.chars());
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
	}
	else if (request.method.compare("DELETE") == 0)
	{
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, request.method.chars());
	}

	curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
	curl_easy_setopt(curl, CURLOPT_CAINFO, caBundlePath);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, connectTimeout);
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, followLocation);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, &ReceiveResponseHeader);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
	curl_easy_setopt(curl, CURLOPT_PRIVATE, this);
	curl_easy_setopt(curl, CURLOPT_READDATA, &request);
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, &ReadRequestBody);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
	curl_easy_setopt(curl, CURLOPT_URL, request.url.chars());
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &WriteResponseBody);
}

HTTPContext::~HTTPContext()
{
	curl_easy_cleanup(curl);
	curl_slist_free_all(headers);
	free(request.body);
}

void HTTPContext::OnCompleted()
{
	/* Return early if the plugin was unloaded while the thread was running */
	if (forward->GetFunctionCount() == 0)
	{
		free(response.body);
		json_decref(response.data);

		forwards->ReleaseForward(forward);
		return;
	}

	HandleSecurity sec(NULL, myself->GetIdentity());
	Handle_t hndlResponse = handlesys->CreateHandleEx(htHTTPResponseObject, &response, &sec, NULL, NULL);
	if (hndlResponse == BAD_HANDLE)
	{
		free(response.body);
		json_decref(response.data);

		forwards->ReleaseForward(forward);

		smutils->LogError(myself, "Could not create HTTP response handle.");
		return;
	}

	forward->PushCell(hndlResponse);
	forward->PushCell(value);
	forward->PushString(error);
	forward->Execute(NULL);

	handlesys->FreeHandle(hndlResponse, &sec);
	handlesys->FreeHandle(response.hndlData, &sec);

	forwards->ReleaseForward(forward);
}
