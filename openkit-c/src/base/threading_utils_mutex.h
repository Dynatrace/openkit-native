#ifndef _THREADING_UTILS_MUTEX
#define _THREADING_UTILS_MUTEX
#include <stdint.h>

typedef struct _threading_mutex
{
	void* platform_mutex;
	void* mutex_attributes;
} threading_mutex;

threading_mutex* init_mutex();
void destroy_mutex(threading_mutex* mutex);
void threading_mutex_lock(threading_mutex* mutex);
void threading_mutex_unlock(threading_mutex* mutex);
#endif