#include "threading_utils_condition_variable.h"

#include <windows.h>

#include "threading_utils_mutex.h"
#include "memory.h"

threading_condition_variable* init_condition_variable()
{
	CONDITION_VARIABLE* platform_condition_variable = (CONDITION_VARIABLE*)memory_malloc(sizeof(CONDITION_VARIABLE));
	InitializeConditionVariable(platform_condition_variable);

	threading_condition_variable* condition_variable = (threading_condition_variable*)memory_malloc(sizeof(threading_condition_variable));
	condition_variable->platform_condvar = platform_condition_variable;
	return condition_variable;
}

void destroy_condition_variable(threading_condition_variable* condvar)
{
	if (condvar != NULL)
	{
		//no windows API call to destroy platform condition variable
		free(condvar->platform_condvar);
		free(condvar);
		condvar = NULL;
	}
}

void threading_condition_variable_block(threading_condition_variable* condvar, threading_mutex* mutex)
{
	if (condvar != NULL)
	{
		SleepConditionVariableCS(condvar->platform_condvar, mutex->platform_mutex, INFINITE);
	}
}

void threading_condition_variable_unblock_single(threading_condition_variable* condvar)
{
	if (condvar != NULL)
	{
		WakeConditionVariable(condvar->platform_condvar);
	}
}

void threading_condition_variable_unblock_all(threading_condition_variable* condvar)
{
	if (condvar != NULL)
	{
		WakeAllConditionVariable(condvar->platform_condvar);
	}
}