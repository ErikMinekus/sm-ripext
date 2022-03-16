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

#include "httprequestcontext.h"

static size_t ReadRequestBody(void *body, size_t size, size_t nmemb, void *userdata)
{
	size_t total = size * nmemb;
	HTTPRequestContext *context = (HTTPRequestContext *)userdata;
	size_t to_copy = (context->size - context->pos < total) ? context->size - context->pos : total;

	memcpy(body, &(context->body[context->pos]), to_copy);
	context->pos += to_copy;

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

	std::string name(header, match - header);
	std::string value(match + 2);

	for (size_t i = 0; i < name.size(); i++)
	{
		name[i] = tolower(name[i]);
	}

	response->headers.replace(name.c_str(), std::move(value));

	return total;
}

HTTPRequestContext::HTTPRequestContext(const std::string &method, const std::string &url, json_t *data,
	struct curl_slist *headers, IChangeableForward *forward, cell_t value,
	long connectTimeout, long maxRedirects, long timeout, curl_off_t maxSendSpeed, curl_off_t maxRecvSpeed,
	bool useBasicAuth, const std::string &username, const std::string &password)
	: method(method), url(url), headers(headers), forward(forward), value(value),
	connectTimeout(connectTimeout), maxRedirects(maxRedirects), timeout(timeout), maxSendSpeed(maxSendSpeed),
	maxRecvSpeed(maxRecvSpeed), useBasicAuth(useBasicAuth), username(username), password(password)
{
	if (data != NULL)
	{
		body = json_dumps(data, 0);
		size = (body == NULL) ? 0 : strlen(body);
	}
}

HTTPRequestContext::~HTTPRequestContext()
{
	forwards->ReleaseForward(forward);

	curl_easy_cleanup(curl);
	curl_slist_free_all(headers);
	free(body);
	free(response.body);
}

bool HTTPRequestContext::InitCurl()
{
	curl = curl_easy_init();
	if (curl == NULL)
	{
		smutils->LogError(myself, "Could not initialize cURL session.");
		return false;
	}

	if (method.compare("POST") == 0)
	{
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, (curl_off_t) size);
	}
	else if (method.compare("PUT") == 0)
	{
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
		curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t) size);
	}
	else if (method.compare("PATCH") == 0)
	{
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str());
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, (curl_off_t) size);
	}
	else if (method.compare("DELETE") == 0)
	{
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str());
	}

	curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
	curl_easy_setopt(curl, CURLOPT_CAINFO, g_RipExt.caBundlePath);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, connectTimeout);
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, &ReceiveResponseHeader);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_MAXREDIRS, maxRedirects);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
	curl_easy_setopt(curl, CURLOPT_PIPEWAIT, 1L);
	curl_easy_setopt(curl, CURLOPT_PRIVATE, this);
	curl_easy_setopt(curl, CURLOPT_READDATA, this);
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, &ReadRequestBody);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_USERAGENT, SM_RIPEXT_USER_AGENT);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &WriteResponseBody);

	if (maxRecvSpeed > 0)
	{
		curl_easy_setopt(curl, CURLOPT_MAX_RECV_SPEED_LARGE, maxRecvSpeed);
	}
	if (maxSendSpeed > 0)
	{
		curl_easy_setopt(curl, CURLOPT_MAX_SEND_SPEED_LARGE, maxSendSpeed);
	}
	if (useBasicAuth)
	{
		curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());
		curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());
	}

#ifdef DEBUG
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif

	return true;
}

void HTTPRequestContext::OnCompleted()
{
	/* Return early if the plugin was unloaded while the thread was running */
	if (forward->GetFunctionCount() == 0)
	{
		return;
	}

	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status);

	HandleError err;
	HandleSecurity sec(NULL, myself->GetIdentity());
	Handle_t hndlResponse = handlesys->CreateHandleEx(htHTTPResponse, &response, &sec, NULL, &err);
	if (hndlResponse == BAD_HANDLE)
	{
		smutils->LogError(myself, "Could not create HTTP response handle (error %d)", err);
		return;
	}

	forward->PushCell(hndlResponse);
	forward->PushCell(value);
	forward->PushString(error);
	forward->Execute(NULL);

	handlesys->FreeHandle(hndlResponse, &sec);
	handlesys->FreeHandle(response.hndlData, &sec);
}
