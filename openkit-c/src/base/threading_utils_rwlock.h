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

#ifndef _THREADING_UTILS_RWLOCK
#define _THREADING_UTILS_RWLOCK

typedef struct _threading_rw_lock
{
	void* platform_rw_lock;
} threading_rw_lock;

threading_rw_lock* init_rw_lock();
void destroy_rw_lock(threading_rw_lock* rw_lock);
void threading_rw_lock_lock_read(threading_rw_lock* rw_lock);
void threading_rw_lock_lock_write(threading_rw_lock* rw_lock);
void threading_rw_lock_unlock_read(threading_rw_lock* rw_lock);
void threading_rw_lock_unlock_write(threading_rw_lock* rw_lock);

#endif