#include "threading_utils_atomic.h"

#include "memory.h"
#include "threading_utils_mutex.h"

atomic* init_atomic(int32_t init_value)
{
	atomic* atomicInt = (atomic*)memory_malloc(sizeof(atomic));
	threading_mutex* mutex = init_mutex();
	atomicInt->platform_mutex = mutex;
	atomic_set(atomicInt, init_value);

	return atomicInt;
}

void destroy_atomic(atomic* atomicInt)
{
	if (atomicInt != NULL)
	{
		free(atomicInt->platform_mutex);
		free(atomicInt);
		atomicInt = NULL;
	}
}

void atomic_add(atomic* atomicInt, int32_t val)
{
	if (atomicInt != NULL)
	{
		threading_mutex_lock(atomicInt->platform_mutex);
		atomicInt->integer_value += val;
		threading_mutex_unlock(atomicInt->platform_mutex);
	}
}

void atomic_increment(atomic* atomicInt)
{
	if (atomicInt != NULL)
	{
		threading_mutex_lock(atomicInt->platform_mutex);
		++(atomicInt->integer_value);
		threading_mutex_unlock(atomicInt->platform_mutex);
	}
}

void atomic_decrement(atomic* atomicInt)
{
	if (atomicInt != NULL)
	{
		threading_mutex_lock(atomicInt->platform_mutex);
		--(atomicInt->integer_value);
		threading_mutex_unlock(atomicInt->platform_mutex);
	}
}

void atomic_compare_and_set(atomic* atomicInt, int32_t cmp, int32_t target_value)
{
	if (atomicInt != NULL)
	{
		threading_mutex_lock(atomicInt->platform_mutex);
		if (atomicInt->integer_value == cmp)
		{
			atomicInt->integer_value = target_value;
		}
		threading_mutex_unlock(atomicInt->platform_mutex);
	}
}

void atomic_set(atomic* atomicInt, int32_t val)
{
	if (atomicInt != NULL)
	{
		threading_mutex_lock(atomicInt->platform_mutex);
		atomicInt->integer_value = val;
		threading_mutex_unlock(atomicInt->platform_mutex);
	}
}