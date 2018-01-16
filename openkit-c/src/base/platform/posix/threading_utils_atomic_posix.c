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

#include "threading_utils_atomic.h"

#include "memory.h"
#include "threading_utils_mutex.h"

atomic* init_atomic(int32_t init_value)
{
	atomic* atomic_int = (atomic*)memory_malloc(sizeof(atomic));
	if (atomic_int != NULL)
	{
		threading_mutex* mutex = init_mutex();

		if (mutex != NULL)
		{
			atomic_int->platform_mutex = mutex;
			atomic_set(atomic_int, init_value);
			return atomic_int;
		}
		destroy_mutex(mutex->platform_mutex);
	}
	memory_free(atomic_int);

	return NULL;
}

void destroy_atomic(atomic* atomic_int)
{
	if (atomic_int != NULL)
	{
		destroy_mutex(atomic_int->platform_mutex);
		memory_free(atomic_int);
		atomic_int = NULL;
	}
}

void atomic_add(atomic* atomic_int, int32_t val)
{
	if (atomic_int != NULL)
	{
		threading_mutex_lock(atomic_int->platform_mutex);
		atomic_int->integer_value += val;
		threading_mutex_unlock(atomic_int->platform_mutex);
	}
}

void atomic_increment(atomic* atomic_int)
{
	atomic_add(atomic_int, 1);
}

void atomic_decrement(atomic* atomic_int)
{
	atomic_add(atomic_int, -1);
}

void atomic_compare_and_set(atomic* atomic_int, int32_t cmp, int32_t target_value)
{
	if (atomic_int != NULL)
	{
		threading_mutex_lock(atomic_int->platform_mutex);
		if (atomic_int->integer_value == cmp)
		{
			atomic_int->integer_value = target_value;
		}
		threading_mutex_unlock(atomic_int->platform_mutex);
	}
}

void atomic_set(atomic* atomic_int, int32_t val)
{
	if (atomic_int != NULL)
	{
		threading_mutex_lock(atomic_int->platform_mutex);
		atomic_int->integer_value = val;
		threading_mutex_unlock(atomic_int->platform_mutex);
	}
}