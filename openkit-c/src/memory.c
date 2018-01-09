#include "memory.h"

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