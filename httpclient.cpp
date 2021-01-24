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

#include "httpclient.h"
#include "httprequestcontext.h"
#include "httpfilecontext.h"

const std::string HTTPClient::BuildURL(const std::string &endpoint) const
{
	std::string url(this->baseURL);
	url.append("/");
	url.append(endpoint);

	return url;
}

struct curl_slist *HTTPClient::BuildHeaders(const char *acceptTypes, const char *contentType)
{
	struct curl_slist *headers = NULL;
	char header[8192];

	snprintf(header, sizeof(header), "Accept: %s", acceptTypes);
	headers = curl_slist_append(headers, header);

	snprintf(header, sizeof(header), "Content-Type: %s", contentType);
	headers = curl_slist_append(headers, header);

	for (HTTPHeaderMap::iterator iter = this->headers.iter(); !iter.empty(); iter.next())
	{
		snprintf(header, sizeof(header), "%s: %s", iter->key.chars(), iter->value.c_str());
		headers = curl_slist_append(headers, header);
	}

	return headers;
}

void HTTPClient::Request(const char *method, const char *endpoint, json_t *data, IPluginFunction *callback, cell_t value)
{
	IChangeableForward *forward = forwards->CreateForwardEx(NULL, ET_Ignore, 3, NULL, Param_Cell, Param_Cell, Param_String);
	if (forward == NULL || !forward->AddFunction(callback))
	{
		smutils->LogError(myself, "Could not create forward.");
		return;
	}

	const std::string url = this->BuildURL(std::string(endpoint));
	struct curl_slist *headers = this->BuildHeaders("application/json", "application/json");
	HTTPRequestContext *context = new HTTPRequestContext(std::string(method), url, data, headers, forward, value,
		this->connectTimeout, this->followLocation, this->timeout, this->maxSendSpeed, this->maxRecvSpeed);

	g_RipExt.AddRequestToQueue(context);
}

void HTTPClient::DownloadFile(const char *endpoint, const char *path, IPluginFunction *callback, cell_t value)
{
	IChangeableForward *forward = forwards->CreateForwardEx(NULL, ET_Ignore, 3, NULL, Param_Cell, Param_Cell, Param_String);
	if (forward == NULL || !forward->AddFunction(callback))
	{
		smutils->LogError(myself, "Could not create forward.");
		return;
	}

	const std::string url = this->BuildURL(std::string(endpoint));
	struct curl_slist *headers = this->BuildHeaders("*/*", "application/octet-stream");
	HTTPFileContext *context = new HTTPFileContext(false, url, std::string(path), headers, forward, value,
		this->connectTimeout, this->followLocation, this->timeout, this->maxSendSpeed, this->maxRecvSpeed);

	g_RipExt.AddRequestToQueue(context);
}

void HTTPClient::UploadFile(const char *endpoint, const char *path, IPluginFunction *callback, cell_t value)
{
	IChangeableForward *forward = forwards->CreateForwardEx(NULL, ET_Ignore, 3, NULL, Param_Cell, Param_Cell, Param_String);
	if (forward == NULL || !forward->AddFunction(callback))
	{
		smutils->LogError(myself, "Could not create forward.");
		return;
	}

	const std::string url = this->BuildURL(std::string(endpoint));
	struct curl_slist *headers = this->BuildHeaders("*/*", "application/octet-stream");
	HTTPFileContext *context = new HTTPFileContext(true, url, std::string(path), headers, forward, value,
		this->connectTimeout, this->followLocation, this->timeout, this->maxSendSpeed, this->maxRecvSpeed);

	g_RipExt.AddRequestToQueue(context);
}

void HTTPClient::SetHeader(const char *name, const char *value)
{
	std::string vstr(value);
	this->headers.replace(name, std::move(vstr));
}

int HTTPClient::GetConnectTimeout() const
{
	return this->connectTimeout;
}

void HTTPClient::SetConnectTimeout(int connectTimeout)
{
	this->connectTimeout = connectTimeout;
}

bool HTTPClient::GetFollowLocation() const
{
	return this->followLocation;
}

void HTTPClient::SetFollowLocation(bool followLocation)
{
	this->followLocation = followLocation;
}

int HTTPClient::GetTimeout() const
{
	return this->timeout;
}

void HTTPClient::SetTimeout(int timeout)
{
	this->timeout = timeout;
}

int HTTPClient::GetMaxSendSpeed() const
{
	return this->maxSendSpeed;
}

void HTTPClient::SetMaxSendSpeed(int speed)
{
	this->maxSendSpeed = speed;
}

int HTTPClient::GetMaxRecvSpeed() const
{
	return this->maxRecvSpeed;
}

void HTTPClient::SetMaxRecvSpeed(int speed)
{
	this->maxRecvSpeed = speed;
}
