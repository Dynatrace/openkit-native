#include "stddef.h"
#include "stdlib.h"
#include "string.h"

void* memory_malloc(size_t size);

void* memory_realloc(void* ptr, size_t size);

void* memory_calloc(size_t num, size_t size);

void memory_free(void* ptr);