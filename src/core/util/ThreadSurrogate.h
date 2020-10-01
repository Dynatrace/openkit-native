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

#ifndef _CORE_UTIL_THREAD_SURROGATE_H
#define _CORE_UTIL_THREAD_SURROGATE_H

#include <condition_variable>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

#ifdef _MSC_VER
# include <sal.h>
#endif 

namespace core
{
	namespace util
	{
		class ThreadSurrogate
		{
		public:

			using ThreadFunction = std::function<void()>;

			///
			/// Default constructor
			///
			ThreadSurrogate();

			///
			/// Spawns a new thread, if not called yet.
			/// 
			/// @param threadFunction the function being invoked from the newly spawned thread.
			///
			/// @return @c false if this method was already called before, @c true otherwise.
			///
			bool start(ThreadFunction threadFunction);

			///
			/// Joins the thread for a given amount of time.
			///
			/// @param waitTimeInMillis number of milliseconds to wait for the thread to complete.
			/// 
			/// @return @c true if thread was successfully terminated in time, @c false on timeout.
			///
			bool join(int64_t waitTimeInMillis);

			///
			/// Gets @c true if ThreadSurrogate::start(ThreadFunction) was called already, @c false otherwise.
			///
			bool isStarted() const;

			///
			/// Gets @c true if this thread is alive, @c false otherwise.
			///
			/// @remarks
			/// If ThreadSurrogate::start(ThreadFunction) was not called before, this method returns @c true.
			///
			bool isAlive() const;

			///
			/// Copy constructor is not allowed
			///
			ThreadSurrogate(const ThreadSurrogate&) = delete;

			///
			/// Assignment operator is not allowed
			///
			ThreadSurrogate& operator = (const ThreadSurrogate) = delete;

		private:

#ifdef _MSC_VER
			// Make MS C++ compiler happy - https://docs.microsoft.com/en-us/cpp/code-quality/c26115
			_Acquires_lock_(this->mMutex)
#endif
			void threadWrapperFunction(ThreadFunction threadFunction);

			mutable std::mutex mMutex;
			std::condition_variable mStopConditionVariable;
			bool mIsThreadStopped;
			std::unique_ptr<std::thread> mThread;
		};
	}
}

#endif
