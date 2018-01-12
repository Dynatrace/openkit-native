#include <stdio.h>
#include <stdint.h>

#include "threading_utils_atomic.h"
#include "threading_utils_thread.h"

atomic* atomicInteger = NULL;

struct thread_info
{
	uint32_t thread_id;
};

void* incrementThread(void* arg)
{
	int32_t i;
	for (i = 0; i < 20; i++)
	{
		atomic_increment(atomicInteger);

		threading_sleep(1);
	}

	return NULL;
}

int32_t main(int32_t argc, char** argv)
{
	(void)argc;
	(void)argv;

	atomicInteger = init_atomic(0);

	if (atomicInteger != NULL)
	{
		threading_thread* t1 = create_thread(&incrementThread, NULL);
		threading_thread* t2 = create_thread(&incrementThread, NULL);
		threading_thread* t3 = create_thread(&incrementThread, NULL);
		threading_thread* t4 = create_thread(&incrementThread, NULL);
		threading_thread* t5 = create_thread(&incrementThread, NULL);

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

		printf("final value is %d\n", atomicInteger->integer_value);
		destroy_atomic(atomicInteger);
	}
	else
	{
		printf("Error: Was unable to initialize rwlock\n");
	}
	return 0;

}
