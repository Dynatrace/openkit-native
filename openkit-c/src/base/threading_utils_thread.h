#ifndef _THREADING_UTILS_THREAD
#define _THREADING_UTILS_THREAD
#include <stdint.h>

typedef struct _threading_thread
{
	void* platform_thread;
	int32_t platform_thread_info;
} threading_thread;

threading_thread* create_thread(void*(*function)(void*), void* thread_data);
void destroy_thread(threading_thread* thread);
void* threading_thread_join(threading_thread* thread);
void threading_thread_cancel(threading_thread* thread);
void threading_thread_detach(threading_thread* thread);

void threading_sleep(uint32_t time_in_ms);

#endif