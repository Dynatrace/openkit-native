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

#include <windows.h>

#include "memory.h"

atomic* init_atomic(int32_t init_value)
{
	atomic* atomic_int = (atomic*)memory_malloc(sizeof(atomic));

	if (atomic_int != NULL)
	{
		long value = InterlockedExchange(&atomic_int->integer_value, init_value);
		if (value == init_value)
		{
			return atomic_int;
		}
		memory_free(atomic_int);
	}

	return NULL;
}

int32_t destroy_atomic(atomic* atomic_int)
{
	if (atomic_int != NULL)
	{
		memory_free(atomic_int);
		atomic_int = NULL;
	}
}

void atomic_add(atomic* atomic_int, int32_t val)
{
	if (atomic_int != NULL)
	{
		InterlockedExchangeAdd(&atomic_int->integer_value, val);
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
		InterlockedCompareExchange(&atomic_int->integer_value, target_value, cmp);
	}
}

void atomic_set(atomic* atomic_int, int32_t val)
{
	if (atomic_int != NULL)
	{
		InterlockedExchange(&atomic_int->integer_value, val);
	}
}