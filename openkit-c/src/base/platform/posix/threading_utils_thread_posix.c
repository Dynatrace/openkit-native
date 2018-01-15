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

#include <pthread.h>

#include "memory.h"

threading_thread* create_thread(void*(*function)(void*), void* thread_data)
{
	threading_thread* thread = (threading_thread*)malloc(sizeof(threading_thread));
	thread->platform_thread = (pthread_t*)malloc(sizeof(pthread_t));
	int result = pthread_create(thread->platform_thread, NULL, function, thread_data);

	if (result != 0)
	{
		free(thread->platform_thread);
		free(thread);
		thread = NULL;
	}
	return thread;
}

void destroy_thread(threading_thread* thread)
{
	if (thread != NULL)
	{
		free(thread->platform_thread);
		free(thread);
		thread = NULL;
	}
}

void* threading_thread_join(threading_thread* thread)
{
	if (thread != NULL)
	{
		void* return_value;
		pthread_join(*(pthread_t*)thread->platform_thread, &return_value);
		return return_value;
	}
}

void threading_sleep(uint32_t time_in_ms)
{
	struct timespec wait_time;
	wait_time.tv_sec = time_in_ms / 1000;
	wait_time.tv_nsec = (time_in_ms % 1000) * 1000;
	nanosleep(&wait_time, NULL);
}