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

#include "curlapi.h"
#include "curlthread.h"

const ke::AString HTTPClient::BuildURL(const ke::AString &endpoint) const
{
	char *url = new char[this->baseURL.length() + endpoint.length() + 2];
	strcpy(url, this->baseURL.chars());
	strcat(url, "/");
	strcat(url, endpoint.chars());

	ke::AString ret(url);
	delete[] url;
	return ret;
}

struct curl_slist *HTTPClient::BuildHeaders(struct HTTPRequest request)
{
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");

	char header[8192];
	snprintf(header, sizeof(header), "Content-Length: %d", request.size);
	headers = curl_slist_append(headers, header);

	for (HTTPHeaderMap::iterator iter = this->headers.iter(); !iter.empty(); iter.next())
	{
		snprintf(header, sizeof(header), "%s: %s", iter->key.chars(), iter->value.chars());
		headers = curl_slist_append(headers, header);
	}

	return headers;
}

void HTTPClient::Request(struct HTTPRequest request, IPluginFunction *function, cell_t value)
{
	IChangeableForward *forward = forwards->CreateForwardEx(NULL, ET_Ignore, 3, NULL, Param_Cell, Param_Cell, Param_String);
	if (forward == NULL || !forward->AddFunction(function))
	{
		smutils->LogError(myself, "Could not create forward.");
		return;
	}

	HTTPRequestThread *thread = new HTTPRequestThread(this, request, forward, value);
	threader->MakeThread(thread);
}

void HTTPClient::SetHeader(const char *name, const char *value)
{
	ke::AString vstr(value);
	this->headers.replace(name, ke::Move(vstr));
}
