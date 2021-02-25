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

#include "httprequest.h"

const std::string HTTPRequest::GetURL() const
{
	return url;
}

int HTTPRequest::GetConnectTimeout() const
{
	return connectTimeout;
}

void HTTPRequest::SetConnectTimeout(int connectTimeout)
{
	this->connectTimeout = connectTimeout;
}

bool HTTPRequest::GetFollowLocation() const
{
	return followLocation;
}

void HTTPRequest::SetFollowLocation(bool followLocation)
{
	this->followLocation = followLocation;
}

int HTTPRequest::GetMaxRecvSpeed() const
{
	return maxRecvSpeed;
}

void HTTPRequest::SetMaxRecvSpeed(int maxSpeed)
{
	this->maxRecvSpeed = maxSpeed;
}

int HTTPRequest::GetMaxSendSpeed() const
{
	return maxSendSpeed;
}

void HTTPRequest::SetMaxSendSpeed(int maxSpeed)
{
	this->maxSendSpeed = maxSpeed;
}

int HTTPRequest::GetTimeout() const
{
	return timeout;
}

void HTTPRequest::SetTimeout(int timeout)
{
	this->timeout = timeout;
}
