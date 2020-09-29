/**
 * Copyright 2018-2020 Dynatrace LLC
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

#ifndef _UTIL_CYCLICBARRIER_H
#define _UTIL_CYCLICBARRIER_H

#include <mutex>
#include <condition_variable>

namespace core
{
	namespace util
	{
		///
		/// The CyclicBarrier allows a number of threads to synchronize.
		///
		class CyclicBarrier
		{
		public:
			///
			/// Constructor
			/// @param[in] numThreads the number of threads required for the synchronization barrier to notify
			///
			CyclicBarrier(uint32_t numThreads);

			///
			/// Wait for the required number of threads upon the barrier object.
			/// If the number is reached, the waiting threads are unlocked and
			/// the cyclic barrier resets itself to become reusable.
			///
			void await();

		private:
			/// number of threads
			uint32_t mNumThreads;

			// number of waiting threads
			uint32_t mWaits;

			/// mutex used for synchronisation
			std::mutex mMutex;

			/// condition variable used as the waiting spot
			std::condition_variable mConditionVariable;
		};
	}
}
#endif
