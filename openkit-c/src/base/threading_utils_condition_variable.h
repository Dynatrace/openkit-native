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

#ifndef _THREADING_UTILS_CONDITION_VAR
#define _THREADING_UTILS_CONDITION_VAR

#include "threading_utils_mutex.h"

#include <stdint.h>

///
/// struct for the system-independent storage of a condition variable
///
typedef struct _condition_variable
{
	void* platform_condvar;
} threading_condition_variable;

///
/// Initialize a condition variable
/// @return new instance of a condition variable  or NULL if errors occured
///
threading_condition_variable* init_condition_variable();

///
/// Destroy a condition variable and free associated ressources
/// @param[in] condvar the condition variable to destroy
/// @return 0 in case of succcess, EINVAL if null was passed and error codes in case of failure
///
int32_t destroy_condition_variable(threading_condition_variable* condvar);

///
/// Block the current thread on this condition variable.
/// @param[in] condvar the condition variable used for blocking
/// @param[in] mutex the mutex used for handling exclusive access to this condition variable
/// @return 0 in case of succcess, EINVAL if null was passed and error codes in case of failure
///
int32_t threading_condition_variable_block(threading_condition_variable* condvar, threading_mutex* mutex);

///
/// Wake a single thread that is blocked by this condition variable
/// @param[in] condvar the condition variable used for blocking
/// @return 0 in case of succcess, EINVAL if null was passed and error codes in case of failure
///
int32_t threading_condition_variable_unblock_single(threading_condition_variable* condvar);

///
/// Wake all threads that are blocked by this condition variable
/// @param[in] condvar the condition variable used for blocking
/// @return 0 in case of succcess, EINVAL if null was passed and error codes in case of failure
///
int32_t threading_condition_variable_unblock_all(threading_condition_variable* condvar);
#endif