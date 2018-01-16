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

#include <pthread.h>
#include <errno.h>

#include "memory.h"

threading_rw_lock* init_rw_lock()
{
	pthread_rwlock_t* platform_lock = (pthread_rwlock_t*)memory_malloc(sizeof(pthread_rwlock_t));
	if (platform_lock != NULL)
	{
		//default attributes(NULL) are fine, we do not need to share locks between different processes
		int32_t result = pthread_rwlock_init(platform_lock, NULL);

		if (result == 0)
		{
			threading_rw_lock* lock = (threading_rw_lock*)memory_malloc(sizeof(threading_rw_lock));

			if (lock != NULL)
			{
				lock->platform_rw_lock = platform_lock;
				return lock;
			}
		}
		memory_free(platform_lock);
	}

	return NULL;
}

int32_t destroy_rw_lock(threading_rw_lock* rw_lock)
{
	if (rw_lock != NULL)
	{
		int32_t result = pthread_rwlock_destroy(rw_lock->platform_rw_lock);

		memory_free(rw_lock->platform_rw_lock);
		memory_free(rw_lock);
		rw_lock = NULL;
		return result;
	}
	return EINVAL;
}

void threading_rw_lock_lock_read(threading_rw_lock* rw_lock)
{
	if (rw_lock != NULL)
	{
		pthread_rwlock_rdlock(rw_lock->platform_rw_lock);
	}
}

void threading_rw_lock_lock_write(threading_rw_lock* rw_lock)
{
	if (rw_lock != NULL)
	{
		pthread_rwlock_wrlock(rw_lock->platform_rw_lock);
	}
}

void threading_rw_lock_unlock_read(threading_rw_lock* rw_lock)
{
	if (rw_lock != NULL)
	{
		pthread_rwlock_unlock(rw_lock->platform_rw_lock);
	}
}

void threading_rw_lock_unlock_write(threading_rw_lock* rw_lock)
{
	if (rw_lock != NULL)
	{
		pthread_rwlock_unlock(rw_lock->platform_rw_lock);
	}
}
