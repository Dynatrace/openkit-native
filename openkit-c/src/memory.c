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

#include "memory.h"

#include <stdlib.h>
#include <string.h>

void* memory_malloc(size_t size)
{
	return malloc(size);
}

void* memory_realloc(void* ptr, size_t size)
{
	return realloc(ptr, size);
}

void* memory_calloc(size_t num, size_t size)
{
	return calloc(num, size);
}

void memory_free(void* ptr)
{
	free(ptr);
}

void memory_copy(void* dst, const void* src, size_t number_of_bytes)
{
	memcpy(dst, src, number_of_bytes);
}

void memory_move(void* dst, const void* src, size_t number_of_bytes)
{
	memmove(dst, src, number_of_bytes);
}

void memory_set(void* dst, int32_t value, size_t number_of_bytes)
{
	memset(dst, value, number_of_bytes);
}