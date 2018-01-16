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

#include <windows.h>

#include "memory.h"


typedef struct _thread_context
{
	void*(*thread_function)(void*);
	void* thread_data;
} thread_context;

DWORD WINAPI ThreadFunc(thread_context* context)
{
	if (context != NULL)
	{
		void*(*thread_function)(void*) = context->thread_function;
		thread_function(context->thread_data);
		return 0;
	}
	return -1;
}

threading_thread* create_thread(void*(*function)(void*), void* thread_data)
{
	if (function != NULL)
	{
		thread_context* context = (thread_context*)memory_malloc(sizeof(thread_context));

		if (context != NULL)
		{
			context->thread_function = function;
			context->thread_data = thread_data;

			threading_thread* thread = (threading_thread*)memory_malloc(sizeof(threading_thread));

			if (thread != NULL)
			{
				HANDLE platform_thread = CreateThread(
					NULL,
					0,
					&ThreadFunc,
					context,
					0,
					&thread->platform_thread_info
				);

				if (platform_thread != NULL)
				{
					thread->platform_thread = platform_thread;
					return thread;
				}
				memory_free(thread);
			}
			memory_free(context);
		}
	}

	return NULL;
}

int32_t destroy_thread(threading_thread* thread)
{
	if (thread != NULL)
	{
		memory_free(thread);
		thread = NULL;
		return 0;
	}
	return EINVAL;
}

int32_t threading_thread_join(threading_thread* thread)
{
	if (thread != NULL)
	{
		WaitForSingleObjectEx(thread->platform_thread, INFINITE, FALSE);
		return 0;
	}
	return EINVAL;
}

int32_t threading_sleep(uint32_t time_in_ms)
{
	HANDLE hTimer = NULL;
	LARGE_INTEGER liDueTime;
	liDueTime.QuadPart = -10000 * time_in_ms; // intervals of 100ns : 1000 x 10 (ms -> 100ns)
												// -negative declares relative time
	hTimer = CreateWaitableTimer(NULL, TRUE, "WaitableTimer");
	if (!hTimer)
	{
		return -1;
	}
	// Set a timer to wait for the requested time. 
	if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0))
	{
		return -2;
	}
	// Wait for the timer. 
	WaitForSingleObject(hTimer, INFINITE);
	return 0;
}