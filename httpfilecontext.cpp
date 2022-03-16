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

#include "httpfilecontext.h"
#include <sys/stat.h>

static size_t IgnoreResponseBody(void *body, size_t size, size_t nmemb, void *userdata)
{
	return size * nmemb;
}

HTTPFileContext::HTTPFileContext(bool isUpload, const std::string &url, const std::string &path,
	struct curl_slist *headers, IChangeableForward *forward, cell_t value,
	long connectTimeout, long maxRedirects, long timeout, curl_off_t maxSendSpeed, curl_off_t maxRecvSpeed,
	bool useBasicAuth, const std::string &username, const std::string &password)
	: isUpload(isUpload), url(url), path(path), headers(headers), forward(forward), value(value),
	connectTimeout(connectTimeout), maxRedirects(maxRedirects), timeout(timeout), maxSendSpeed(maxSendSpeed),
	maxRecvSpeed(maxRecvSpeed), useBasicAuth(useBasicAuth), username(username), password(password)
{}

HTTPFileContext::~HTTPFileContext()
{
	forwards->ReleaseForward(forward);

	curl_easy_cleanup(curl);
	curl_slist_free_all(headers);
}

bool HTTPFileContext::InitCurl()
{
	curl = curl_easy_init();
	if (curl == NULL)
	{
		smutils->LogError(myself, "Could not initialize cURL session.");
		return false;
	}

	char realpath[PLATFORM_MAX_PATH];
	smutils->BuildPath(Path_Game, realpath, sizeof(realpath), "%s", path.c_str());

	file = fopen(realpath, isUpload ? "rb" : "wb");
	if (file == NULL)
	{
		smutils->LogError(myself, "Could not open file %s.", path.c_str());
		return false;
	}

	if (isUpload)
	{
		curl_easy_setopt(curl, CURLOPT_READDATA, file);
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, fread);
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &IgnoreResponseBody);
		curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t) FileSize(file));
	}
	else
	{
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite);
	}

	curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
	curl_easy_setopt(curl, CURLOPT_CAINFO, g_RipExt.caBundlePath);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, connectTimeout);
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_MAXREDIRS, maxRedirects);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
	curl_easy_setopt(curl, CURLOPT_PIPEWAIT, 1L);
	curl_easy_setopt(curl, CURLOPT_PRIVATE, this);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_USERAGENT, SM_RIPEXT_USER_AGENT);

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

#ifdef WIN32
	curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
#endif

	return true;
}

void HTTPFileContext::OnCompleted()
{
	fclose(file);

	/* Return early if the plugin was unloaded while the thread was running */
	if (forward->GetFunctionCount() == 0)
	{
		return;
	}

	long status;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);

	forward->PushCell(status);
	forward->PushCell(value);
	forward->PushString(error);
	forward->Execute(NULL);
}

off_t FileSize(FILE *fd)
{
#ifdef WIN32
	struct _stat file_info;
	int stat_res = _fstat(fileno(fd), &file_info);
#else
	struct stat file_info;
	int stat_res = fstat(fileno(fd), &file_info);
#endif // WIN32

	if (stat_res != 0)
	{
		return -1;
	}

	return file_info.st_size;
}
