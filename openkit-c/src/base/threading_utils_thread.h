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

#ifndef _THREADING_UTILS_THREAD
#define _THREADING_UTILS_THREAD
#include <stdint.h>

typedef struct _threading_thread
{
	void* platform_thread;
	int32_t platform_thread_info;
} threading_thread;

threading_thread* create_thread(void*(*function)(void*), void* thread_data);
void destroy_thread(threading_thread* thread);
void* threading_thread_join(threading_thread* thread);
void threading_thread_cancel(threading_thread* thread);
void threading_thread_detach(threading_thread* thread);

void threading_sleep(uint32_t time_in_ms);

#endif