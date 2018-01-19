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

#ifndef _THREADING_UTILS_ATOMIC
#define _THREADING_UTILS_ATOMIC

#include "threading_utils_mutex.h"

#include <stdint.h>

#ifdef  __cplusplus
extern "C" {
#endif
	///
	/// struct encapsulating an atomic 32 bit integer
	/// an atomic integer provides thread-safe access from multiple threads
	///
	typedef struct _atomic_int32
	{
		volatile int32_t integer_value;
		threading_mutex* platform_mutex;
	} atomic_int32;

	///
	/// Initialize an atomic integer
	/// @param[in] init_value initial value of the atomic integer
	/// @return a new atomic integer or NULL if errors occured
	///
	atomic_int32* init_atomic(int32_t init_value);

	///
	/// Destroy an atomic integer and free all associated resources
	/// @param[in] atomic_int the atomic integer to destruct
	/// @return 0 on succes, in all other cases an error number indicating the reason for the failure
	///
	int32_t destroy_atomic(atomic_int32* atomic_int);

	///
	/// Add the given value @c val to the current value of the atomic integer and returns the addition result.
	/// @param[in] atomic_int the atomic integer where to add the value to
	/// @param[in] val the value to add to the current value of the atomic integer
	/// @return The value after adding @c val or if @c atomic_int is @c null, then INT_MAX is returned.
	///
	int32_t atomic_add_and_get(atomic_int32* atomic_int, int32_t val);

	///
	/// Increment the current value of the atomic integer by 1 and return the resulting value.
	/// @param[in] atomic_int the atomic integer to increment
	/// @return Returns the resuling value after incrementing the atomic integer by 1 
	///         or if @c atomic_int is @c null, then INT_MAX is returned.
	///
	int32_t atomic_increment_and_get(atomic_int32* atomic_int);

	///
	/// Decrement the current value of the atomic integer by 1 and return the resulting value.
	/// @param[in] atomic_int the atomic integer to decrement
	/// @return Returns the resuling value after decrementing the atomic integer by 1
	///         or if @c atomic_int is @c null, then INT_MAX is returned.
	///
	int32_t atomic_decrement_and_get(atomic_int32* atomic_int);

	///
	/// Perform an atomic compare and set operation on the atomic integer
	/// If the current value is equal to a comparison value the new target_value is
	/// used as new value of the atomic integer.
	/// @param[in] the atomic integer to perform the compare-and-set operation on
	/// @param[in] cmp the integer value compared to the current value of the atomic integer
	/// @param[on] target_value the value to apply on the atomic integer when the comparison
	///            was sucessful
	/// @return Returns the previous value of atomic integer or if @c atomic_int is @c null, then INT_MAX is returned..
	///
	int32_t atomic_compare_and_set(atomic_int32* atomic_int, int32_t cmp, int32_t target_value);

	///
	/// Set the current value of the atomic integer
	/// @param[in] the atomic integer to perform the operation on
	/// @param val the value to apply on the atomic integer
	/// @return Returns the previous value of atomic integer or if @c atomic_int is @c null, then INT_MAX is returned..
	///
	int32_t atomic_set(atomic_int32* atomic_int, int32_t val);
#ifdef  __cplusplus
}
#endif
#endif