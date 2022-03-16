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

#ifndef SM_RIPEXT_HTTPFILECONTEXT_H_
#define SM_RIPEXT_HTTPFILECONTEXT_H_

#include <stdio.h>
#include "extension.h"

class HTTPFileContext : public IHTTPContext
{
public:
	HTTPFileContext(bool isUpload, const std::string &url, const std::string &path,
		struct curl_slist *headers, IChangeableForward *forward, cell_t value,
		long connectTimeout, long maxRedirects, long timeout, curl_off_t maxSendSpeed, curl_off_t maxRecvSpeed,
		bool useBasicAuth, const std::string &username, const std::string &password);
	~HTTPFileContext();

public: // IHTTPContext
	bool InitCurl();
	void OnCompleted();

private:
	FILE *file = NULL;

	bool isUpload;
	const std::string url;
	const std::string path;
	struct curl_slist *headers;
	IChangeableForward *forward;
	cell_t value;
	char error[CURL_ERROR_SIZE] = {'\0'};
	long connectTimeout;
	long maxRedirects;
	long timeout;
	curl_off_t maxSendSpeed;
	curl_off_t maxRecvSpeed;
	bool useBasicAuth;
	const std::string username;
	const std::string password;
};

off_t FileSize(FILE *fd);

#endif // SM_RIPEXT_HTTPFILECONTEXT_H_
