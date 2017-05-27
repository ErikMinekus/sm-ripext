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

#ifndef SM_RIPEXT_CURLAPI_H_
#define SM_RIPEXT_CURLAPI_H_

#include <stdlib.h>
#include <string.h>
#include "extension.h"

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

class HTTPClient
{
public:
	HTTPClient(const char *baseURL) : baseURL(baseURL) {}

	char *BuildURL(const char *endpoint);

	struct curl_slist *GetHeaders(struct HTTPRequest request);

	void Request(struct HTTPRequest request, IPluginFunction *function, cell_t value);

private:
	const char *baseURL;
};

#endif // SM_RIPEXT_CURLAPI_H_
