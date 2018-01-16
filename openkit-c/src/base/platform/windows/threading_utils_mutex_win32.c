/**
* Copyright 2018 Dynatrace LLC
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "threading_utils_mutex.h"

#include <windows.h>

#include "memory.h"

threading_mutex* init_mutex()
{
	CRITICAL_SECTION* critical_section = (CRITICAL_SECTION*)memory_malloc(sizeof(CRITICAL_SECTION));
	if (critical_section != NULL)
	{
		InitializeCriticalSectionAndSpinCount(critical_section, 0);

		threading_mutex* mutex = (threading_mutex*)memory_malloc(sizeof(threading_mutex));
		if (mutex != NULL)
		{
			mutex->platform_mutex = critical_section;
			return mutex;
		}
	}
	memory_free(critical_section);

	return NULL;
}

int32_t destroy_mutex(threading_mutex* mutex)
{
	if (mutex != NULL)
	{
		DeleteCriticalSection(mutex->platform_mutex);
		memory_free(mutex->platform_mutex);
		memory_free(mutex);
		mutex = NULL;
		return 0;
	}
	return EINVAL;
}

int32_t threading_mutex_lock(threading_mutex* mutex)
{
	if (mutex != NULL)
	{
		EnterCriticalSection(mutex->platform_mutex);
		return 0;
	}
	return EINVAL;
}

int32_t threading_mutex_unlock(threading_mutex* mutex)
{
	if (mutex != NULL)
	{
		LeaveCriticalSection(mutex->platform_mutex);
		return 0;
	}
	return EINVAL;
}