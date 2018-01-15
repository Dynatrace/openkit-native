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
#include <stdint.h>

#include "threading_utils_mutex.h"

typedef struct _condition_variable
{
	void* platform_condvar;
} threading_condition_variable;

threading_condition_variable* init_condition_variable();
void destroy_condition_variable(threading_condition_variable* condvar);
void threading_condition_variable_block(threading_condition_variable* condvar, threading_mutex* mutex);
void threading_condition_variable_unblock_single(threading_condition_variable* condvar);
void threading_condition_variable_unblock_all(threading_condition_variable* condvar);
#endif