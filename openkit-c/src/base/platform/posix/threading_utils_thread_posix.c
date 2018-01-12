#include "threading_utils_thread.h"

#include <pthread.h>

#include "memory.h"

threading_thread* create_thread(void*(*function)(void*), void* thread_data)
{
	threading_thread* thread = (threading_thread*)malloc(sizeof(threading_thread));
	thread->platform_thread = (pthread_t*)malloc(sizeof(pthread_t));
	int result = pthread_create(thread->platform_thread, NULL, function, thread_data);

	if (result != 0)
	{
		free(thread->platform_thread);
		free(thread);
		thread = NULL;
	}
	return thread;
}

void destroy_thread(threading_thread* thread)
{
	if (thread != NULL)
	{
		free(thread->platform_thread);
		free(thread);
		thread = NULL;
	}
}

void* threading_thread_join(threading_thread* thread)
{
	if (thread != NULL)
	{
		void* return_value;
		pthread_join(*(pthread_t*)thread->platform_thread, &return_value);
		return return_value;
	}
}

void threading_sleep(uint32_t time_in_ms)
{
	struct timespec wait_time;
	wait_time.tv_sec = time_in_ms / 1000;
	wait_time.tv_nsec = (time_in_ms % 1000) * 1000;
	nanosleep(&wait_time, NULL);
}