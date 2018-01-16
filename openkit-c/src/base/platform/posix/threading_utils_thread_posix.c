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

#include "threading_utils_thread.h"

#include "memory.h"

#include <pthread.h>
#include <errno.h>



threading_thread* create_thread(void*(*function)(void*), void* thread_data)
{
	if (function != NULL)
	{
		pthread_t* platform_thread = (pthread_t*)malloc(sizeof(pthread_t));
		if (platform_thread != NULL)
		{
			int result = pthread_create(platform_thread, NULL, function, thread_data);

			if (result == 0)
			{
				threading_thread* thread = (threading_thread*)malloc(sizeof(threading_thread));
				if (thread != NULL)
				{
					thread->platform_thread = platform_thread;
					return thread;
				}
			}
			pthread_mutex_destroy(platform_thread);
			memory_free(platform_thread);
		}
	}

	return NULL;
}

int32_t destroy_thread(threading_thread* thread)
{
	if (thread != NULL)
	{
		int32_t result = pthread_mutex_destroy(thread->platform_thread);
		memory_free(thread->platform_thread);
		memory_free(thread);
		thread = NULL;
		return result;
	}
	return EINVAL;
}

int32_t threading_thread_join(threading_thread* thread)
{
	if (thread != NULL)
	{
		void* return_value;
		int32_t result = pthread_join(*(pthread_t*)thread->platform_thread, &return_value);
		return result;
	}
	return EINVAL;
}

int32_t threading_sleep(uint32_t time_in_ms)
{
	struct timespec wait_time;
	wait_time.tv_sec = time_in_ms / 1000;
	wait_time.tv_nsec = (time_in_ms % 1000) * 1000;
	return nanosleep(&wait_time, NULL);
}