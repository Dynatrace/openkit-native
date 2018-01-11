#include "threading_utils_mutex.h"

#include <pthread.h>

#include "memory.h"

threading_mutex* init_mutex()
{
	pthread_mutexattr_t* mutex_attributes = (pthread_mutexattr_t*)memory_malloc(sizeof(pthread_mutexattr_t));
	pthread_mutexattr_init(mutex_attributes);

	pthread_mutexattr_settype(mutex_attributes, PTHREAD_MUTEX_RECURSIVE);

	pthread_mutex_t* platform_mutex = (pthread_mutex_t*)memory_malloc(sizeof(pthread_mutex_t));
	int32_t result = pthread_mutex_init(platform_mutex, mutex_attributes);

	if (result == 0)
	{
		threading_mutex* mutex = (threading_mutex*)memory_malloc(sizeof(threading_mutex));
		mutex->platform_mutex = platform_mutex;
		mutex->mutex_attributes = mutex_attributes;
		return mutex;
	}

	return NULL;
}

void destroy_mutex(threading_mutex* mutex)
{
	pthread_mutex_destroy(mutex->platform_mutex);
	pthread_mutexattr_destroy(mutex->mutex_attributes);
	free(mutex->platform_mutex);
	free(mutex->mutex_attributes);
	free(mutex);
	mutex = NULL;
}

void threading_mutex_lock(threading_mutex* mutex)
{
	pthread_mutex_lock(mutex->platform_mutex);
}

void threading_mutex_unlock(threading_mutex* mutex)
{
	pthread_mutex_unlock(mutex->platform_mutex);
}