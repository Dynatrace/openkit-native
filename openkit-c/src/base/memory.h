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


#ifndef _MEMORY_H
#define _MEMORY_H

#include <stddef.h>

///
/// Allocate a block of memory of given size
/// (this function is a wrapper for the standard malloc implementation)
/// @param[in] size number of bytes to be allocated
/// @return upon sucess a non-NULL typeless pointer to the allocated memory is returned
///
void* memory_malloc(size_t size);

///
/// Re-allocate a block of memory of given size. If ptr is NULL, realloc behaves
/// just like malloc. In case ptr contains a previously allocated block the new block
/// is returned. If necessary realloc will create a new block at a different
/// address which will be returned.
/// (this function is a wrapper for the standard realloc implementation)
/// @param[in] ptr address of existing block or NULL
/// @param[in] size number of bytes to be allocated
/// @return upon sucess a non-NULL typeless pointer to the allocated memory is returned
void* memory_realloc(void* ptr, size_t size);

///
/// Allocate a block of memory and initialize all bytes in the block 
/// with 0. The size is calculated as number of elements multplied by
/// size of an element.
/// (this function is a wrapper for the standard calloc implementation)
/// @param[in] num number of elements to allocate
/// @param[in] size of each element
/// @return upon sucess a non-NULL typeless pointer to the allocated memory is returned
void* memory_calloc(size_t num, size_t size);

///
/// Free memory that has been allocated before with memory_malloc, memory_realloc or memory_calloc.
/// If the passed pointer is a NULL pointer no further operation is performed.
/// @param[in] ptr pointer to the memory to free
/// 
void memory_free(void* ptr);
#endif