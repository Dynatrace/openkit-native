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

#include <stdint.h>

#ifdef  __cplusplus
extern "C" {
#endif
	///
	/// struct encapsulating a platform read-write lock
	///
	typedef struct _threading_rw_lock
	{
		void* platform_rw_lock;
	} threading_rw_lock;

	///
	/// Initialize a new read-write lock
	/// @return a new read-write lock or NULL if errors occured
	///
	threading_rw_lock* init_rw_lock();

	///
	/// Destroy a rwlock object and free associated resources
	/// @param rw_lock the read-write lock to destroy
	/// @return 0 in case of succcess, EINVAL if null was passed and error codes in case of failure
	///
	int32_t destroy_rw_lock(threading_rw_lock* rw_lock);

	///
	/// Perform a read-lock on the read-write lock
	/// @param[in] rwlock the read-write lock to lock for reading
	/// @return 0 in case of succcess, EINVAL if null was passed and error codes in case of a failure
	///
	int32_t threading_rw_lock_lock_read(threading_rw_lock* rw_lock);

	///
	/// Perform a write-lock on the read-write lock
	/// @param[in] rwlock the read-write lock to lock for writing
	/// @return 0 in case of succcess, EINVAL if null was passed and error codes in case of a failure
	///
	int32_t threading_rw_lock_lock_write(threading_rw_lock* rw_lock);

	///
	/// Perform a read-unlock on the read-write lock
	/// @param[in] rwlock the read-write lock to unlock for reading
	/// @return 0 in case of succcess, EINVAL if null was passed and error codes in case of a failure
	///
	int32_t threading_rw_lock_unlock_read(threading_rw_lock* rw_lock);

	///
	/// Perform a write-unlock on the read-write lock
	/// @param[in] rwlock the read-write lock to unlock for writing
	/// @return 0 in case of succcess, EINVAL if null was passed and error codes in case of a failure
	///
	int32_t threading_rw_lock_unlock_write(threading_rw_lock* rw_lock);
#ifdef  __cplusplus
}
#endif
#endif