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

///
/// struct encapsulating a platform mutex
///
typedef struct _threading_mutex
{
	void* platform_mutex;
	void* mutex_attributes;
} threading_mutex;

///
/// Initializes a new mutex object
/// @return a new mutex or NULL if errors occured
///
threading_mutex* init_mutex();

///
/// Destroy a mutex object and free associated resources
/// @param[in] the mutex to destroy
/// @return 0 in case of succcess, EINVAL if null was passed and error codes in case of failure
///
int32_t destroy_mutex(threading_mutex* mutex);

///
/// Perform a lock operation on the passed mutex
/// @param[in] the mutex to lock
/// @return 0 in case of succcess, EINVAL if null was passed and error codes in case of failure
///
int32_t threading_mutex_lock(threading_mutex* mutex);

///
/// Perform an unlock operation on the passed mutex
/// @param[in] the mutex to unlock
/// @return 0 in case of succcess, EINVAL if null was passed and error codes in case of failure
///
int32_t threading_mutex_unlock(threading_mutex* mutex);
#endif