/**
 * Copyright 2018-2021 Dynatrace LLC
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

#ifndef _CORE_CACHING_IBEACONCACHEEVICTOR_H
#define _CORE_CACHING_IBEACONCACHEEVICTOR_H

#include <thread>
#include <chrono>

namespace core
{
	namespace caching
	{
		///
		/// Interface responsible for handling an eviction thread, to ensure @ref IBeaconCache stays in configured boundaries.
		///
		class IBeaconCacheEvictor
		{
		public:

			///
			/// Destructor
			///
			virtual ~IBeaconCacheEvictor() = default;

			///
			/// Checks if the eviction thread is running or not.
			/// @return @c true if running, @c false otherwise
			///
			virtual bool isAlive() = 0;

			///
			/// Starts the eviction thread.
			/// @return @c true if the eviction thread was started, @c false if the thread was already running.
			///
			virtual bool start() = 0;

			///
			/// Stops the eviction thread with the default timeout of @ref EVICTION_THREAD_JOIN_TIMEOUT.
			/// See also @ref stop(std::chrono::milliseconds)
			///
			virtual bool stop() = 0;

			///
			/// Stops the eviction thread with the given @c timeout. If the timeout is reached the thread is terminated forcefully.
			/// @param[in] timeout The number of milliseconds to join the thread.
			/// @return @c true if stopping was successful, @c false if eviction thread is not running or could not be stopped in time.
			///
			virtual bool stop(std::chrono::milliseconds timeout) = 0;
		};
	}
}

#endif
