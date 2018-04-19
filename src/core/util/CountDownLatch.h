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

#ifndef _CORE_UTIL_COUNTDOWNLATCH_H
#define _CORE_UTIL_COUNTDOWNLATCH_H

#include <mutex>
#include <condition_variable>

namespace core
{
	namespace util
	{
		///
		/// CountDownLatch is a synchronisation mechanism where threads will wait in the @c await() method
		/// unit the count reaches 0. If 1 is used as initial value this is a simple on/off latch.
		/// This implementation is a pseudo countdown-latch using a condition variable
		///
		class CountDownLatch
		{
		public:

			///
			/// Constructor taking an initial count
			/// @param[in] count initial count of the CountDownLatch
			///
			CountDownLatch(uint32_t count);

			///
			/// Count down by one. If the result is 0 fire all threads currently stuck in await
			///
			void countDown();

			///
			/// Wait until the CountDownLatch is down to 0
			/// NOTE: This is a blocking operation
			///
			void await();

			///
			/// Wait until the CountDownLatch is down to 0 or timeout expired
			/// NOTE: This is a blocking operation
			/// @param[in] milliseconds The maximum number of milliseconds to wait for initialization being completed.
			///
			void await(int64_t milliseconds);

		private:
			/// the count
			uint32_t mCount;

			/// mutex used for sychronisation
			std::mutex mMutex;

			/// condition variable used as countdown latch
			std::condition_variable mConditionVariable;
		};
	}
}


#endif