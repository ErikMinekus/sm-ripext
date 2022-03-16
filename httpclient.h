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

#ifndef SM_RIPEXT_HTTPCLIENT_H_
#define SM_RIPEXT_HTTPCLIENT_H_

#include "extension.h"

class HTTPClient
{
public:
	HTTPClient(const char *baseURL) : baseURL(baseURL) {}

	const std::string BuildURL(const std::string &endpoint) const;

	struct curl_slist *BuildHeaders(const char *acceptTypes, const char *contentType);

	void Request(const char *method, const char *endpoint, json_t *data, IPluginFunction *callback, cell_t value);

	void DownloadFile(const char *endpoint, const char *path, IPluginFunction *callback, cell_t value);
	void UploadFile(const char *endpoint, const char *path, IPluginFunction *callback, cell_t value);

	void SetHeader(const char *name, const char *value);

	int GetConnectTimeout() const;
	void SetConnectTimeout(int connectTimeout);

	bool GetFollowLocation() const;
	void SetFollowLocation(bool followLocation);

	int GetTimeout() const;
	void SetTimeout(int timeout);

	int GetMaxSendSpeed() const;
	void SetMaxSendSpeed(int speed);

	int GetMaxRecvSpeed() const;
	void SetMaxRecvSpeed(int speed);

private:
	const std::string baseURL;
	HTTPHeaderMap headers;
	int connectTimeout = 10;
	bool followLocation = true;
	int timeout = 30;
	int maxRecvSpeed = 0;
	int maxSendSpeed = 0;
};

#endif // SM_RIPEXT_HTTPCLIENT_H_
