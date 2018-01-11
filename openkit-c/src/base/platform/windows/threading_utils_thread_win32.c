#include "threading_utils_thread.h"

#include <windows.h>

#include "memory.h"


typedef struct _thread_context
{
	void*(**thread_function)(void*);
	void* thread_data;
} thread_context;

DWORD WINAPI ThreadFunc(thread_context* context)
{
	void*(*thread_function)(void*) = context->thread_function;
	thread_function(context->thread_data);
}

threading_thread* create_thread(void*(*function)(void*), void* thread_data)
{
	thread_context* context = memory_malloc((thread_context*)sizeof(thread_context));
	context->thread_function = function;
	context->thread_data = thread_data;

	threading_thread* thread = (threading_thread*)memory_malloc(sizeof(threading_thread));

	HANDLE threadHandle = CreateThread(
		NULL,
		0,
		&ThreadFunc,
		context,
		0,
		&thread->platform_thread_info
	);

	thread->platform_thread = threadHandle;

	if (thread->platform_thread == NULL)
	{
		free(thread);
		thread = NULL;
	}
}

void destroy_thread(threading_thread* thread)
{
	free(thread);
	thread = NULL;
}

void* threading_thread_join(threading_thread* thread)
{
	if (thread == NULL)
	{
		return NULL;
	}
	
	WaitForSingleObjectEx(thread->platform_thread, INFINITE, FALSE);
}

void threading_thread_cancel(threading_thread* thread)
{
	if (thread == NULL)
	{
		return;
	}
}

void threading_thread_detach(threading_thread* thread)
{
	if (thread == NULL)
	{
		return;
	}
}

void threading_sleep(uint32_t time_in_ms)
{
	Sleep(time_in_ms);
}