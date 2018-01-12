#ifndef _THREADING_UTILS_ATOMIC
#define _THREADING_UTILS_ATOMIC
#include <stdint.h>
#include "threading_utils_mutex.h"

typedef struct _atomic
{
	volatile long integer_value;
	threading_mutex* platform_mutex;
} atomic;

atomic* init_atomic(int32_t init_value);
void destroy_atomic(atomic* atomicInt);
void atomic_add(atomic* atomicInt, int32_t val);
void atomic_increment(atomic* atomicInt);
void atomic_decrement(atomic* atomicInt);
void atomic_compare_and_set(atomic* atomicInt, int32_t cmp, int32_t target_value);
void atomic_set(atomic* atomicInt, int32_t val);
#endif