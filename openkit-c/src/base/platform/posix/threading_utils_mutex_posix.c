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

#include "memory.h"

threading_mutex* init_mutex()
{
	pthread_mutexattr_t* mutex_attributes = (pthread_mutexattr_t*)memory_malloc(sizeof(pthread_mutexattr_t));

	if (mutex_attributes != NULL)
	{
		pthread_mutexattr_init(mutex_attributes);

		pthread_mutexattr_settype(mutex_attributes, PTHREAD_MUTEX_RECURSIVE);

		pthread_mutex_t* platform_mutex = (pthread_mutex_t*)memory_malloc(sizeof(pthread_mutex_t));

		if (platform_mutex != NULL)
		{
			int32_t result = pthread_mutex_init(platform_mutex, mutex_attributes);
			if (result == 0)
			{
				threading_mutex* mutex = (threading_mutex*)memory_malloc(sizeof(threading_mutex));
				if (mutex != NULL)
				{
					mutex->platform_mutex = platform_mutex;
					mutex->mutex_attributes = mutex_attributes;
					return mutex;
				}
			}
			memory_free(platform_mutex);
		}
		memory_free(mutex_attributes);
	}


	return NULL;

}

void destroy_mutex(threading_mutex* mutex)
{
	if (mutex != NULL)
	{
		pthread_mutex_destroy(mutex->platform_mutex);
		pthread_mutexattr_destroy(mutex->mutex_attributes);
		memory_free(mutex->platform_mutex);
		memory_free(mutex->mutex_attributes);
		memory_free(mutex);
		mutex = NULL;
	}
}

void threading_mutex_lock(threading_mutex* mutex)
{
	if (mutex != NULL)
	{
		pthread_mutex_lock(mutex->platform_mutex);
	}
}

void threading_mutex_unlock(threading_mutex* mutex)
{
	if (mutex != NULL)
	{
		pthread_mutex_unlock(mutex->platform_mutex);
	}
}