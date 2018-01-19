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

#include "threading_utils_condition_variable.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "threading_utils_mutex.h"
#include "memory.h"

static void delete_conditiction_variable(CONDITION_VARIABLE* condition_variable)
{
	if (condition_variable == NULL)
	{
		// do nothing if a NULL ptr was given
		return;
	}

	// NOTE there is no special function to destroy a CV
	// just free up the memory
	memory_free(condition_variable);
}

threading_condition_variable* init_condition_variable()
{
	CONDITION_VARIABLE* platform_condition_variable = (CONDITION_VARIABLE*)memory_malloc(sizeof(CONDITION_VARIABLE));
	if (platform_condition_variable == NULL)
	{
		return NULL;
	}

	InitializeConditionVariable(platform_condition_variable);

	threading_condition_variable* condition_variable = (threading_condition_variable*)memory_malloc(sizeof(threading_condition_variable));
	if (condition_variable == NULL)
	{
		// allocation of wrapper structure failed
		// destroy previously created things
		destroy_condition_variable(platform_condition_variable);
		return NULL;
	}

	condition_variable->platform_condvar = platform_condition_variable;
	return condition_variable;
}

int32_t destroy_condition_variable(threading_condition_variable* condvar)
{
	if (condvar != NULL)
	{
		//no windows API call to destroy platform condition variable
		delete_conditiction_variable(condvar->platform_condvar);
		memory_free(condvar);
		return 0;
	}
	return EINVAL;
}

int32_t threading_condition_variable_block(threading_condition_variable* condvar, threading_mutex* mutex)
{
	if ((condvar != NULL && condvar->platform_condvar != NULL)
		&& (mutex != NULL && mutex->platform_mutex != NULL))
	{
		if (SleepConditionVariableCS(condvar->platform_condvar, mutex->platform_mutex, INFINITE) != FALSE)
		{
			// function succeeded, since return value is non-zero
			return 0;
		}
		// function failed, last error contains details
		return GetLastError();
	}
	return EINVAL;
}

int32_t threading_condition_variable_unblock_single(threading_condition_variable* condvar)
{
	if (condvar != NULL && condvar->platform_condvar != NULL)
	{
		WakeConditionVariable(condvar->platform_condvar);
		return 0;
	}
	return EINVAL;
}

int32_t threading_condition_variable_unblock_all(threading_condition_variable* condvar)
{
	if (condvar != NULL && condvar->platform_condvar != NULL)
	{
		WakeAllConditionVariable(condvar->platform_condvar);
		return 0;
	}
	return EINVAL;
}