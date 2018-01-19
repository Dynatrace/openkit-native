#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "threading_utils_thread.h"
#include "threading_utils_mutex.h"
#include "threading_utils_condition_variable.h"


#define BSIZE 10

struct thread_info
{
	threading_mutex* mutex;
	threading_condition_variable* condvar;
	uint32_t thread_id;
};

typedef struct {
	char buf[BSIZE];
	int32_t occupied;
	int32_t nextin;
	int32_t nextout;
	threading_mutex* mutex;
	threading_condition_variable* more;
	threading_condition_variable* less;
} buffer_t;

buffer_t buffer;

void* producerThread(void* arg)
{
	struct thread_info* info = (struct thread_info*)arg;
	int32_t threadId = info->thread_id;

	int32_t i;
	for (i = 0; i < 150; i++)
	{
		char item = '0' + i%10;
		threading_mutex_lock(buffer.mutex);

		while (buffer.occupied >= BSIZE)
		{
			threading_condition_variable_block(buffer.less, buffer.mutex);
		}

		assert(buffer.occupied < BSIZE);

		buffer.buf[buffer.nextin++] = item;

		buffer.nextin %= BSIZE;
		buffer.occupied++;

		printf("[thread %d] producing value '%c'\n", threadId, item);

		/* now: either b->occupied < BSIZE and b->nextin is the index
		of the next empty slot in the buffer, or
		b->occupied == BSIZE and b->nextin is the index of the
		next (occupied) slot that will be emptied by a consumer
		(such as b->nextin == b->nextout) */

		threading_condition_variable_unblock_all(buffer.more);

		threading_mutex_unlock(buffer.mutex);
	}
	printf("finished producer thread %d\n", threadId);
	return NULL;
}

void* consumerThread(void* arg)
{
	struct thread_info* info = (struct thread_info*)arg;
	int32_t threadId = info->thread_id;

	int32_t i;
	for (i = 0; i < 100; i++)
	{
		char item;
		threading_mutex_lock(buffer.mutex);
		while (buffer.occupied <= 0)
		{
			threading_condition_variable_block(buffer.more, buffer.mutex);
		}

		assert(buffer.occupied > 0);

		item = buffer.buf[buffer.nextout++];

		buffer.nextout %= BSIZE;
		buffer.occupied--;
		printf("[thread %d] consuming value '%c'\n", threadId, item);

		/* now: either b->occupied > 0 and b->nextout is the index
		of the next occupied slot in the buffer, or
		b->occupied == 0 and b->nextout is the index of the next
		(empty) slot that will be filled by a producer (such as
		b->nextout == b->nextin) */

		threading_condition_variable_unblock_all(buffer.less);
		threading_mutex_unlock(buffer.mutex);

		threading_sleep(20);
	}
	printf("finished consumer thread %d\n", threadId);
	return NULL;
}

void cleanup()
{
	destroy_mutex(buffer.mutex);
	destroy_condition_variable(buffer.more);
	destroy_condition_variable(buffer.less);
}

int32_t main(int32_t argc, char** argv)
{
	(void)argc;
	(void)argv;

	buffer.mutex = init_mutex();
	buffer.more = init_condition_variable();
	buffer.less = init_condition_variable();

	if (buffer.mutex != NULL && buffer.less != NULL && buffer.more != 0)
	{
		struct thread_info infoT1;
		infoT1.thread_id = 1;

		struct thread_info infoT2;
		infoT2.thread_id = 2;

		struct thread_info infoT3;
		infoT3.thread_id = 3;

		struct thread_info infoT4;
		infoT4.thread_id = 4;

		struct thread_info infoT5;
		infoT5.thread_id = 5;

		threading_thread* t1 = create_thread(&producerThread, (void*)&infoT1);
		threading_thread* t2 = create_thread(&producerThread, (void*)&infoT2);
		threading_thread* t3 = create_thread(&consumerThread, (void*)&infoT3);
		threading_thread* t4 = create_thread(&consumerThread, (void*)&infoT4);
		threading_thread* t5 = create_thread(&consumerThread, (void*)&infoT5);

		if (t1 != NULL && t2 != NULL && t3 != NULL && t4 != NULL && t5 != NULL)
		{
			threading_thread_join(t1);
			threading_thread_join(t2);
			threading_thread_join(t3);
			threading_thread_join(t4);
			threading_thread_join(t5);

			destroy_thread(t1);
			destroy_thread(t2);
			destroy_thread(t3);
			destroy_thread(t4);
			destroy_thread(t5);

			cleanup();
			return 0;
		}

		cleanup();
	}
	else
	{
		printf("Error: Was unable to initialize rwlock\n");
	}
	return -1;

}
