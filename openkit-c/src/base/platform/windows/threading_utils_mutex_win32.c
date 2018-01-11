#include "threading_utils_mutex.h"

#include <windows.h>

#include "memory.h"

threading_mutex* init_mutex()
{
	CRITICAL_SECTION* critical_section = (CRITICAL_SECTION*)memory_malloc(sizeof(CRITICAL_SECTION));
	InitializeCriticalSectionAndSpinCount(critical_section, 0);

	threading_mutex* mutex = (threading_mutex*)memory_malloc(sizeof(threading_mutex));
	mutex->platform_mutex = critical_section;
	return mutex;
}

void destroy_mutex(threading_mutex* mutex)
{
	DeleteCriticalSection(mutex->platform_mutex);
	free(mutex->platform_mutex);
	free(mutex);
	mutex = NULL;
}

void threading_mutex_lock(threading_mutex* mutex)
{
	EnterCriticalSection(mutex->platform_mutex);
}

void threading_mutex_unlock(threading_mutex* mutex)
{
	LeaveCriticalSection(mutex->platform_mutex);
}