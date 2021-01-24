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

#include "httpfilecontext.h"

HTTPFileContext::HTTPFileContext(bool isUpload, const std::string &url, const std::string &path,
	struct curl_slist *headers, IChangeableForward *forward, cell_t value,
	long connectTimeout, long followLocation, long timeout, curl_off_t maxSendSpeed, curl_off_t maxRecvSpeed)
	: isUpload(isUpload), url(url), path(path), headers(headers), forward(forward), value(value),
	connectTimeout(connectTimeout), followLocation(followLocation), timeout(timeout),
	maxSendSpeed(maxSendSpeed), maxRecvSpeed(maxRecvSpeed)
{}

HTTPFileContext::~HTTPFileContext()
{
	forwards->ReleaseForward(forward);

	curl_easy_cleanup(curl);
	curl_slist_free_all(headers);

	if (file)
	{
		fclose(file);
	}
}

void HTTPFileContext::InitCurl()
{
	curl = curl_easy_init();
	if (curl == NULL)
	{
		smutils->LogError(myself, "Could not initialize cURL session.");
		return;
	}

	char realpath[PLATFORM_MAX_PATH];
	smutils->BuildPath(Path_Game, realpath, sizeof(realpath), "%s", path.c_str());

	file = fopen(realpath, isUpload ? "rb" : "wb");
	if (file == NULL)
	{
		smutils->LogError(myself, "Could not open file %s.", path.c_str());
		return;
	}

	if (isUpload)
	{
		curl_easy_setopt(curl, CURLOPT_READDATA, file);
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, fread);
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
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
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, followLocation);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
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
}

void HTTPFileContext::OnCompleted()
{
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
