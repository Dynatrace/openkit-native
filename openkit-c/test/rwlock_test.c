#include <stdio.h>
#include <stdint.h>

#include "threading_utils_thread.h"
#include "threading_utils_rwlock.h"

extern int64_t g_longValue = 0;

struct thread_info
{
	threading_rw_lock* lock;
	uint32_t thread_id;
};

void* readerThread(void* arg)
{
	struct thread_info* info = (struct thread_info*)arg;
	threading_rw_lock* lock = info->lock;
	int32_t threadId = info->thread_id;

	int32_t i;
	for (i = 0; i < 10; i++)
	{
		threading_rw_lock_lock_read(lock);
		printf("[thread %d] value is %lli\n", threadId, g_longValue);
		threading_rw_lock_unlock_read(lock);
		threading_sleep(750);
	}
	return NULL;
}

void* writerThread(void* arg)
{
	struct thread_info* info = (struct thread_info*)arg;
	threading_rw_lock* lock = info->lock;
	int32_t threadId = info->thread_id;

	int32_t i;
	for (i = 0; i < 10; i++)
	{
		threading_rw_lock_lock_write(lock);
		g_longValue++;
		printf("[thread %d] incrementing counter\n", threadId);
		threading_rw_lock_unlock_write(lock);
		threading_sleep(512);
	}
	return NULL;
}

int32_t main(int32_t argc, char** argv)
{
	(void)argc;
	(void)argv;

	threading_rw_lock* lock = init_rw_lock();
	if (lock != NULL)
	{
		struct thread_info infoT1;
		infoT1.lock = lock;
		infoT1.thread_id = 1;

		struct thread_info infoT2;
		infoT2.lock = lock;
		infoT2.thread_id = 2;

		struct thread_info infoT3;
		infoT3.lock = lock;
		infoT3.thread_id = 3;

		threading_thread* t1 = create_thread(&writerThread, (void*)&infoT1);
		threading_thread* t2 = create_thread(&readerThread, (void*)&infoT2);
		threading_thread* t3 = create_thread(&readerThread, (void*)&infoT3);

		threading_thread_join(t1);
		threading_thread_join(t2);
		threading_thread_join(t3);

		destroy_thread(t1);
		destroy_thread(t2);
		destroy_thread(t3);

		destroy_rw_lock(lock);
	}
	else
	{
		printf("Error: Was unable to initialize rwlock\n");
	}
	return 0;

}
