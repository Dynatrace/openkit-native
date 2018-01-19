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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "memory.h"

static SRWLOCK* create_slim_rw_lock()
{
	SRWLOCK* platform_lock = (SRWLOCK*)memory_malloc(sizeof(SRWLOCK));
	if (platform_lock == NULL)
	{
		return NULL;
	}

	InitializeSRWLock(platform_lock);

	return platform_lock;
}

threading_rw_lock* init_rw_lock()
{
	SRWLOCK* platform_lock = create_slim_rw_lock();
	if (platform_lock == NULL)
	{
		return NULL;
	}


	threading_rw_lock* lock = (threading_rw_lock*)memory_malloc(sizeof(threading_rw_lock));
	if (lock == NULL)
	{
		// failed to allocate wrapper structure
		// destroy so far allocated memory
		// NOTE Slim RW locks do not have an explicit destroy function
		//see https://msdn.microsoft.com/en-us/library/windows/desktop/aa904937(v=vs.85).aspx
		memory_free(platform_lock);
		return NULL;
	}

	lock->platform_rw_lock = platform_lock;
	return lock;
}

int32_t destroy_rw_lock(threading_rw_lock* rw_lock)
{
	if (rw_lock != NULL)
	{
		//windows api doesn't require to destroy rw_locks
		//see https://msdn.microsoft.com/en-us/library/windows/desktop/aa904937(v=vs.85).aspx
		if (rw_lock->platform_rw_lock != NULL)
		{
			memory_free(rw_lock->platform_rw_lock);
		}
		memory_free(rw_lock);
		return 0;
	}
	return EINVAL;
}

int32_t threading_rw_lock_lock_read(threading_rw_lock* rw_lock)
{
	if (rw_lock != NULL && rw_lock->platform_rw_lock != NULL)
	{
		AcquireSRWLockShared(rw_lock->platform_rw_lock);
		return 0;
	}
	return EINVAL;
}

int32_t threading_rw_lock_lock_write(threading_rw_lock* rw_lock)
{
	if (rw_lock != NULL && rw_lock->platform_rw_lock != NULL)
	{
		AcquireSRWLockExclusive(rw_lock->platform_rw_lock);
		return 0;
	}
	return EINVAL;
}

int32_t threading_rw_lock_unlock_read(threading_rw_lock* rw_lock)
{
	if (rw_lock != NULL && rw_lock->platform_rw_lock != NULL)
	{
		ReleaseSRWLockShared(rw_lock->platform_rw_lock);
		return 0;
	}
	return EINVAL;
}

int32_t threading_rw_lock_unlock_write(threading_rw_lock* rw_lock)
{
	if (rw_lock != NULL && rw_lock->platform_rw_lock != NULL)
	{
		ReleaseSRWLockExclusive(rw_lock->platform_rw_lock);
		return 0;
	}
	return EINVAL;
}
