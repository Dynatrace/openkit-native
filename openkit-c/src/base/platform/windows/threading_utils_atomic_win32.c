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
	atomic* atomicInt = (atomic*)memory_malloc(sizeof(atomic));
	InterlockedExchange(&atomicInt->integer_value, init_value);
	return atomicInt;
}

void destroy_atomic(atomic* atomicInt)
{
	free(atomicInt);
	atomicInt = NULL;
}

void atomic_add(atomic* atomicInt, int32_t val)
{
	if (atomicInt != NULL)
	{
		InterlockedExchangeAdd(&atomicInt->integer_value, val);
	}
}

void atomic_increment(atomic* atomicInt)
{
	if (atomicInt != NULL)
	{
		InterlockedIncrement(&atomicInt->integer_value);
	}
}

void atomic_decrement(atomic* atomicInt)
{
	if (atomicInt != NULL)
	{
		InterlockedDecrement(&atomicInt->integer_value);
	}
}

void atomic_compare_and_set(atomic* atomicInt, int32_t cmp, int32_t target_value)
{
	if (atomicInt != NULL)
	{
		InterlockedCompareExchange(&atomicInt->integer_value, target_value, cmp);
	}
}

void atomic_set(atomic* atomicInt, int32_t val)
{
	if (atomicInt != NULL)
	{
		InterlockedExchange(&atomicInt->integer_value, val);
	}
}