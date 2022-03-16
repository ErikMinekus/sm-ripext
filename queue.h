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

#ifndef SM_RIPEXT_QUEUE_H_
#define SM_RIPEXT_QUEUE_H_

#include <queue>
#include <uv.h>

template <class T>
class LockedQueue
{
public:
	LockedQueue()
	{
		uv_mutex_init(&mutex);
	}

	~LockedQueue()
	{
		uv_mutex_destroy(&mutex);
	}

	void Lock()
	{
		uv_mutex_lock(&mutex);
	}

	void Unlock()
	{
		uv_mutex_unlock(&mutex);
	}

	T Pop()
	{
		T item = queue.front();
		queue.pop();

		return item;
	}

	void Push(T item)
	{
		queue.push(item);
	}

	bool Empty()
	{
		return queue.empty();
	}

private:
	uv_mutex_t mutex;
	std::queue<T> queue;
};

#endif // SM_RIPEXT_QUEUE_H_
