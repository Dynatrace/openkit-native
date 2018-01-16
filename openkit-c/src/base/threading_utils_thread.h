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

///
/// struct encapsulating a platform thread
///
typedef struct _threading_thread
{
	void* platform_thread;
	int32_t platform_thread_info;
} threading_thread;

///
/// Initialize a new thread
/// @param[in] function a function pointer declaring the threads execution context
/// @param[in] thread_data a void pointer to the thread's arguments
/// @return a new thread or NULL if errors occured
///
threading_thread* create_thread(void*(*function)(void*), void* thread_data);

///
/// Destroy a thread and free associated resources
/// @param[in] the thread to destroy
/// @return 0 in case of succcess, EINVAL if null was passed and error codes in case of failure
///
int32_t destroy_thread(threading_thread* thread);

///
/// Wait until the thread reaches the end of its execution, the calling thread
/// is blocked until the other thread returns.
/// @param[in] thread the thread for whose end a blocking wait is performed
/// @return 0 in case of succcess, EINVAL if null was passed and error codes in case of failure
///
int32_t threading_thread_join(threading_thread* thread);

///
/// Sleep for the specified amount of milliseconds. For being able to specify wait operations
/// in the millisecond resolution high-precision timers are used.
/// @param[in] time_in_ms amount of milliseconds to wait in the callers thread.
///
int32_t threading_sleep(uint32_t time_in_ms);

#endif