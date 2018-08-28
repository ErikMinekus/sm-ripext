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

#ifndef SM_RIPEXT_HTTPTHREAD_H_
#define SM_RIPEXT_HTTPTHREAD_H_

#include "extension.h"

class HTTPRequestThread : public IThread
{
public:
	HTTPRequestThread(HTTPClient *client, struct HTTPRequest request, IChangeableForward *forward, cell_t value)
		: client(client), request(request), forward(forward), value(value) {}

	void RunThread(IThreadHandle *pThread);
	void OnTerminate(IThreadHandle *pThread, bool cancel) {}

private:
	HTTPClient *client;
	struct HTTPRequest request;
	IChangeableForward *forward;
	cell_t value;
};

#endif // SM_RIPEXT_HTTPTHREAD_H_
