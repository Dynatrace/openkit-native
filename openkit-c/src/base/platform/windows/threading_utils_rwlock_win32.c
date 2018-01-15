/**
* Copyright 2018 Dynatrace LLC
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "threading_utils_rwlock.h"

#include <windows.h>

#include "memory.h"

threading_rw_lock* init_rw_lock()
{
	SRWLOCK* platform_lock = (SRWLOCK*)memory_malloc(sizeof(SRWLOCK));
	InitializeSRWLock(platform_lock);

	threading_rw_lock* lock = (threading_rw_lock*)memory_malloc(sizeof(threading_rw_lock));
	lock->platform_rw_lock = (void*)platform_lock;

	return lock;
}

void destroy_rw_lock(threading_rw_lock* rw_lock)
{
	if (rw_lock != NULL)
	{
		//windows api doesn't require to destroy rw_locks
		//see https://msdn.microsoft.com/en-us/library/windows/desktop/aa904937(v=vs.85).aspx

		free((SRWLOCK*)(rw_lock->platform_rw_lock));
		free(rw_lock);
		rw_lock = NULL;
	}
}

void threading_rw_lock_lock_read(threading_rw_lock* rw_lock)
{
	if (rw_lock != NULL)
	{
		AcquireSRWLockShared(rw_lock->platform_rw_lock);
	}
}

void threading_rw_lock_lock_write(threading_rw_lock* rw_lock)
{
	if (rw_lock != NULL)
	{
		AcquireSRWLockExclusive(rw_lock->platform_rw_lock);
	}
}

void threading_rw_lock_unlock_read(threading_rw_lock* rw_lock)
{
	if (rw_lock != NULL)
	{
		ReleaseSRWLockShared(rw_lock->platform_rw_lock);
	}
}

void threading_rw_lock_unlock_write(threading_rw_lock* rw_lock)
{
	if (rw_lock != NULL)
	{
		ReleaseSRWLockExclusive(rw_lock->platform_rw_lock);
	}
}
