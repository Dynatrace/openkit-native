#include "threading_utils_condition_variable.h"

#include <pthread.h>

#include "threading_utils_mutex.h"
#include "memory.h"

threading_condition_variable* init_condition_variable()
{
	pthread_cond_t* platform_condition_variable = (pthread_cond_t*)memory_malloc(sizeof(pthread_cond_t));
	pthread_cond_init(platform_condition_variable, NULL);

	threading_condition_variable* condition_variable = (threading_condition_variable*)memory_malloc(sizeof(threading_condition_variable));
	condition_variable->platform_condvar = platform_condition_variable;

	return condition_variable;
}

void destroy_condition_variable(threading_condition_variable* condvar)
{
	if (condvar != NULL)
	{
		pthread_cond_destroy(condvar->platform_condvar);
		free(condvar->platform_condvar);
		free(condvar);
		condvar = NULL;
	}
}

void threading_condition_variable_block(threading_condition_variable* condvar, threading_mutex* mutex)
{
	if (condvar != NULL)
	{
		pthread_cond_wait(condvar->platform_condvar, mutex->platform_mutex);
	}
}

void threading_condition_variable_unblock_single(threading_condition_variable* condvar)
{
	if (condvar != NULL)
	{
		pthread_cond_signal(condvar->platform_condvar);
	}
}

void threading_condition_variable_unblock_all(threading_condition_variable* condvar)
{
	if (condvar != NULL)
	{
		pthread_cond_broadcast(condvar->platform_condvar);
	}
}