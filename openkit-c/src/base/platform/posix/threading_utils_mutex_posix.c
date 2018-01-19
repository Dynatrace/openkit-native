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

#include "threading_utils_mutex.h"

#include <pthread.h>
#include <errno.h>

#include "memory.h"

static int32_t init_mutex_attributes(pthread_mutexattr_t* mutex_attributes)
{
	int32_t result = pthread_mutexattr_init(mutex_attributes);
	if (result == 0)
	{
		result = pthread_mutexattr_settype(mutex_attributes, PTHREAD_MUTEX_RECURSIVE);
		if (result != 0)
		{
			// error case, we need to undo pthread_mutexattr_init
			pthread_mutexattr_destroy(mutex_attributes);
		}
	}

	return result;
}

static void delete_posix_mutex(pthread_mutex_t* posix_mutex)
{
	if (posix_mutex == NULL)
	{
		// do nothing if a NULL ptr was given
		return;
	}

	pthread_mutex_destroy(posix_mutex);
	memory_free(posix_mutex);
}

threading_mutex* init_mutex()
{
	// create and initialize mutex attributes
	pthread_mutexattr_t mutex_attributes;
	int32_t result = init_mutex_attributes(&mutex_attributes);
	if (result != 0)
	{
		return NULL;
	}

	pthread_mutex_t* platform_mutex = (pthread_mutex_t*)memory_malloc(sizeof(pthread_mutex_t));
	if (platform_mutex == NULL)
	{
		// memory allocation failed, destroy mutex attributes
		pthread_mutexattr_destroy(&mutex_attributes);
		return NULL;
	}

	// initialize mutex
	result = pthread_mutex_init(platform_mutex, &mutex_attributes);
	if (result != 0)
	{
		// initialization failed
		memory_free(platform_mutex);
		pthread_mutexattr_destroy(&mutex_attributes);
		return NULL;
	}

	// mutex has been initialized
	// mutex attributes are no longer needed
	pthread_mutexattr_destroy(&mutex_attributes);

	// create our wrapper struct
	threading_mutex* mutex = (threading_mutex*)memory_malloc(sizeof(threading_mutex));
	if (mutex != NULL)
	{
		mutex->platform_mutex = platform_mutex;
		return mutex;
	}

	// failed to allocate our wrapper structure
	// cleanup posix mutex
	delete_posix_mutex(platform_mutex);
	return NULL;
}

int32_t destroy_mutex(threading_mutex* mutex)
{
	if (mutex != NULL)
	{
		delete_posix_mutex(mutex->platform_mutex);
		memory_free(mutex);
		return 0;
	}
	return EINVAL;
}

int32_t threading_mutex_lock(threading_mutex* mutex)
{
	if (mutex != NULL && mutex->platform_mutex != NULL)
	{
		return pthread_mutex_lock(mutex->platform_mutex);
	}
	return EINVAL;
}

int32_t threading_mutex_unlock(threading_mutex* mutex)
{
	if (mutex != NULL && mutex->platform_mutex != NULL)
	{
		return pthread_mutex_unlock(mutex->platform_mutex);
	}
	return EINVAL;
}
