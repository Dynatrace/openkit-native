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

static void delete_posix_rw_lock(pthread_rwlock_t* posix_rw_lock)
{
	if (posix_rw_lock == NULL)
	{
		// do nothing if a NULL ptr was given
		return;
	}

	// don't care about errors
	pthread_rwlock_destroy(posix_rw_lock);
	memory_free(posix_rw_lock);
}

threading_rw_lock* init_rw_lock()
{
	pthread_rwlock_t* platform_lock = (pthread_rwlock_t*)memory_malloc(sizeof(pthread_rwlock_t));
	if (platform_lock == NULL)
	{
		return NULL;
	}

	// the second parameter is attributes
	// in our case, since we don't need to share our RW lock accross process boundaries,
	// the default attributes are sufficient.
	int32_t result = pthread_rwlock_init(platform_lock, NULL);
	if (result != 0)
	{
		// error happened - release memory and return
		memory_free(platform_lock);
		return result;
	}

	threading_rw_lock* lock = (threading_rw_lock*)memory_malloc(sizeof(threading_rw_lock));
	if (lock == NULL)
	{
		// allocating our wrapper structure failed
		delete_posix_rw_lock(platform_lock);
		return NULL;
	}

	lock->platform_rw_lock = platform_lock;
	return lock;
}

int32_t destroy_rw_lock(threading_rw_lock* rw_lock)
{
	if (rw_lock != NULL)
	{
		delete_posix_rw_lock(rw_lock->platform_rw_lock);
		memory_free(rw_lock);
		return 0;
	}
	return EINVAL;
}

int32_t threading_rw_lock_lock_read(threading_rw_lock* rw_lock)
{
	if (rw_lock != NULL && rw_lock->platform_rw_lock != NULL)
	{
		return pthread_rwlock_rdlock(rw_lock->platform_rw_lock);
	}
	return EINVAL;
}

int32_t threading_rw_lock_lock_write(threading_rw_lock* rw_lock)
{
	if (rw_lock != NULL && rw_lock->platform_rw_lock != NULL)
	{
		return pthread_rwlock_wrlock(rw_lock->platform_rw_lock);
	}
	return EINVAL;
}

int32_t threading_rw_lock_unlock_read(threading_rw_lock* rw_lock)
{
	if (rw_lock != NULL && rw_lock->platform_rw_lock != NULL)
	{
		return pthread_rwlock_unlock(rw_lock->platform_rw_lock);
	}
	return EINVAL;
}

int32_t threading_rw_lock_unlock_write(threading_rw_lock* rw_lock)
{
	if (rw_lock != NULL && rw_lock->platform_rw_lock != NULL)
	{
		return pthread_rwlock_unlock(rw_lock->platform_rw_lock);
	}
	return EINVAL;
}
