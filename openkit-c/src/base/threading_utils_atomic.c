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
#include "threading_utils_mutex.h"

#include <errno.h>
#include <limits.h>

#include "memory.h"



atomic_int32* init_atomic(int32_t initial_value)
{
	atomic_int32* atomic_int = (atomic_int32*)memory_malloc(sizeof(atomic_int32));
	if (atomic_int == NULL)
	{
		return NULL;
	}

	threading_mutex* mutex = init_mutex();
	if (mutex == NULL)
	{
		memory_free(atomic_int);
		return NULL;
	}
	atomic_int->platform_mutex = mutex;
	atomic_int->integer_value = initial_value;

	return atomic_int;
}

int32_t destroy_atomic(atomic_int32* atomic_int)
{
	if (atomic_int == NULL)
	{
		return NULL;
	}

	destroy_mutex(atomic_int->platform_mutex);
	memory_free(atomic_int);
	
	return 0;
}

int32_t atomic_add_and_get(atomic_int32* atomic_int, int32_t val)
{
	if (atomic_int != NULL)
	{
		threading_mutex_lock(atomic_int->platform_mutex);
		atomic_int->integer_value += val;
		int result = atomic_int->integer_value;
		threading_mutex_unlock(atomic_int->platform_mutex);

		return result;
	}

	return INT_MAX;
}

int32_t atomic_increment_and_get(atomic_int32* atomic_int)
{
	return atomic_add_and_get(atomic_int, 1);
}

int32_t atomic_decrement_and_get(atomic_int32* atomic_int)
{
	return atomic_add_and_get(atomic_int, -1);
}

int32_t atomic_compare_and_set(atomic_int32* atomic_int, int32_t cmp, int32_t target_value)
{
	if (atomic_int != NULL)
	{
		threading_mutex_lock(atomic_int->platform_mutex);
		int32_t initial = atomic_int->integer_value;
		if (atomic_int->integer_value == cmp)
		{
			atomic_int->integer_value = target_value;
		}
		threading_mutex_unlock(atomic_int->platform_mutex);

		return initial;
	}

	return INT_MAX;
}

int32_t atomic_set(atomic_int32* atomic_int, int32_t val)
{
	if (atomic_int != NULL)
	{
		threading_mutex_lock(atomic_int->platform_mutex);
		int32_t initial = atomic_int->integer_value;
		atomic_int->integer_value = val;
		threading_mutex_unlock(atomic_int->platform_mutex);

		return initial;
	}

	return INT_MAX;
}