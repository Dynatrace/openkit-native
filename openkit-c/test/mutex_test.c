#include <stdio.h>
#include <stdint.h>

#include "threading_utils_thread.h"
#include "threading_utils_mutex.h"

extern int64_t g_longValue = 0;

struct thread_info
{
	threading_mutex* mutex;
	uint32_t thread_id;
};

void* writerThread(void* arg)
{
	struct thread_info* info = (struct thread_info*)arg;
	threading_mutex* mutex = info->mutex;
	int32_t threadId = info->thread_id;

	int32_t i;
	for (i = 0; i < 25; i++)
	{
		threading_mutex_lock(mutex);
		g_longValue++;
		printf("[thread %d] incrementing counter to %lli\n", threadId, g_longValue);
		threading_mutex_unlock(mutex);
		threading_sleep(250);
	}
	return NULL;
}

int32_t main(int32_t argc, char** argv)
{
	(void)argc;
	(void)argv;

	threading_mutex* mutex = init_mutex();
	if (mutex != NULL)
	{
		struct thread_info infoT1;
		infoT1.mutex = mutex;
		infoT1.thread_id = 1;

		struct thread_info infoT2;
		infoT2.mutex = mutex;
		infoT2.thread_id = 2;

		threading_thread* t1 = create_thread(&writerThread, (void*)&infoT1);
		threading_thread* t2 = create_thread(&writerThread, (void*)&infoT2);

		threading_thread_join(t1);
		threading_thread_join(t2);

		destroy_thread(t1);
		destroy_thread(t2);

		destroy_mutex(mutex);
	}
	else
	{
		printf("Error: Was unable to initialize rwlock\n");
	}
	return 0;

}
