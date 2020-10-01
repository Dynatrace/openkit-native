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

#ifndef _CORE_UTIL_INTERRUPTIBLETHREADSUSPENDER_H
#define _CORE_UTIL_INTERRUPTIBLETHREADSUSPENDER_H

#include "IInterruptibleThreadSuspender.h"

#include <mutex>
#include <condition_variable>

namespace core
{
	namespace util
	{
		class InterruptibleThreadSuspender
			: public IInterruptibleThreadSuspender
		{
		public:

			InterruptibleThreadSuspender();

			~InterruptibleThreadSuspender() override = default;

			void sleep(int64_t millis) override;

			void wakeup() override;

		private:

			bool mSignaled;
			std::mutex mMutex;
			std::condition_variable mConditionVariable;
		};
	}
}

#endif
