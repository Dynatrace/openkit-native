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

#ifndef _THREADING_UTILS_MUTEX
#define _THREADING_UTILS_MUTEX
#include <stdint.h>

typedef struct _threading_mutex
{
	void* platform_mutex;
	void* mutex_attributes;
} threading_mutex;

threading_mutex* init_mutex();
void destroy_mutex(threading_mutex* mutex);
void threading_mutex_lock(threading_mutex* mutex);
void threading_mutex_unlock(threading_mutex* mutex);
#endif