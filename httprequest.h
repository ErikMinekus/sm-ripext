/**
 * vim: set ts=4 :
 * =============================================================================
 * SourceMod REST in Pawn Extension
 * Copyright 2017-2021 Erik Minekus
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

#ifndef SM_RIPEXT_HTTPREQUEST_H_
#define SM_RIPEXT_HTTPREQUEST_H_

#include "extension.h"

class HTTPRequest
{
public:
	HTTPRequest(const std::string &url) : url(url) {}

	void Perform(const char *method, json_t *data, IChangeableForward *forward, cell_t value);
	void DownloadFile(const char *path, IChangeableForward *forward, cell_t value);
	void UploadFile(const char *path, IChangeableForward *forward, cell_t value);

	const std::string BuildURL() const;
	void AppendQueryParam(const char *name, const char *value);

	struct curl_slist *BuildHeaders(struct curl_slist *headers);
	void SetHeader(const char *name, const char *value);

	bool UseBasicAuth() const;
	const std::string GetUsername() const;
	const std::string GetPassword() const;
	void SetBasicAuth(const char *username, const char *password);

	int GetConnectTimeout() const;
	void SetConnectTimeout(int connectTimeout);

	bool GetFollowLocation() const;
	void SetFollowLocation(bool followLocation);

	int GetMaxRecvSpeed() const;
	void SetMaxRecvSpeed(int maxSpeed);

	int GetMaxSendSpeed() const;
	void SetMaxSendSpeed(int maxSpeed);

	int GetTimeout() const;
	void SetTimeout(int timeout);

private:
	const std::string url;
	std::string query;
	HTTPHeaderMap headers;
	bool useBasicAuth = false;
	std::string username;
	std::string password;
	int connectTimeout = 10;
	bool followLocation = true;
	int maxRecvSpeed = 0;
	int maxSendSpeed = 0;
	int timeout = 30;
};

#endif // SM_RIPEXT_HTTPREQUEST_H_
