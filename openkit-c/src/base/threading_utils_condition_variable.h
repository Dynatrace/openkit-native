#ifndef _THREADING_UTILS_CONDITION_VAR
#define _THREADING_UTILS_CONDITION_VAR
#include <stdint.h>

#include "threading_utils_mutex.h"

typedef struct _condition_variable
{
	void* platform_condvar;
} threading_condition_variable;

threading_condition_variable* init_condition_variable();
void destroy_condition_variable(threading_condition_variable* condvar);
void threading_condition_variable_block(threading_condition_variable* condvar, threading_mutex* mutex);
void threading_condition_variable_unblock_single(threading_condition_variable* condvar);
void threading_condition_variable_unblock_all(threading_condition_variable* condvar);
#endif