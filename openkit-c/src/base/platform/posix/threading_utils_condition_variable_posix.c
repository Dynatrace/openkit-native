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

#include <pthread.h>
#include <errno.h>

#include "threading_utils_mutex.h"
#include "memory.h"

threading_condition_variable* init_condition_variable()
{
	pthread_cond_t* platform_condition_variable = (pthread_cond_t*)memory_malloc(sizeof(pthread_cond_t));

	if (platform_condition_variable != NULL)
	{
		int result = pthread_cond_init(platform_condition_variable, NULL);
		if (result == 0)
		{
			threading_condition_variable* condition_variable = (threading_condition_variable*)memory_malloc(sizeof(threading_condition_variable));

			if (condition_variable != NULL)
			{
				condition_variable->platform_condvar = platform_condition_variable;
				return condition_variable;
			}
			pthread_cond_destroy(condition_variable->platform_condvar);
			memory_free(platform_condition_variable);
		}
	}

	return NULL;
}

int32_t destroy_condition_variable(threading_condition_variable* condition_variable)
{
	if (condition_variable != NULL)
	{
		int32_t result = pthread_cond_destroy(condition_variable->platform_condvar);
		memory_free(condition_variable->platform_condvar);
		memory_free(condition_variable);
		condition_variable = NULL;
		return result;
	}
	return EINVAL;
}

int32_t threading_condition_variable_block(threading_condition_variable* condvar, threading_mutex* mutex)
{
	if (condvar != NULL)
	{
		return pthread_cond_wait(condvar->platform_condvar, mutex->platform_mutex);
	}
	return EINVAL;
}

int32_t threading_condition_variable_unblock_single(threading_condition_variable* condvar)
{
	if (condvar != NULL)
	{
		return pthread_cond_signal(condvar->platform_condvar);
	}
	return EINVAL;
}

int32_t threading_condition_variable_unblock_all(threading_condition_variable* condvar)
{
	if (condvar != NULL)
	{
		return pthread_cond_broadcast(condvar->platform_condvar);
	}
	return EINVAL;
}