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

#ifndef _CORE_CACHING_BEACONCACHEEVICTOR_H
#define _CORE_CACHING_BEACONCACHEEVICTOR_H

#include "OpenKit/ILogger.h"
#include "IObserver.h"
#include "IBeaconCache.h"
#include "IBeaconCacheEvictor.h"
#include "IBeaconCacheEvictionStrategy.h"
#include "core/configuration/IBeaconCacheConfiguration.h"
#include "core/util/ThreadSurrogate.h"
#include "providers/ITimingProvider.h"

#include <memory>
#include <vector>
#include <mutex>
#include <condition_variable>

namespace core
{
	namespace caching
	{
		///
		/// Class responsible for handling an eviction thread, to ensure @ref BeaconCache stays in configured boundaries.
		///
		class BeaconCacheEvictor
			: public IBeaconCacheEvictor
			, IObserver
		{
		public:

			///
			/// Public constructor, initializing the eviction thread with the default @ref TimeEvictionStrategy and @ref SpaceEvictionStrategy strategies.
			/// @param[in] logger to write traces to
			/// @param[in] beaconCache    The Beacon cache to check if entries need to be evicted
			/// @param[in] configuration  Beacon cache configuration
			/// @param[in] timingProvider Timing provider required for time retrieval
			///
			BeaconCacheEvictor
			(
				std::shared_ptr<openkit::ILogger> logger,
				std::shared_ptr<IBeaconCache> beaconCache,
				std::shared_ptr<configuration::IBeaconCacheConfiguration> configuration,
				std::shared_ptr<providers::ITimingProvider> timingProvider
			);

			///
			/// Internal testing constructor.
			/// @param[in] logger to write traces to
			/// @param[in] beaconCache The Beacon cache to check if entries need to be evicted
			/// @param[in] strategies  Strategies passed to the actual Runnable.
			///
			BeaconCacheEvictor
			(
				std::shared_ptr<openkit::ILogger> logger,
				std::shared_ptr<IBeaconCache> beaconCache,
				std::vector<std::shared_ptr<IBeaconCacheEvictionStrategy>> strategies
			);

			///
			/// Starts the eviction thread.
			/// @return @c true if the eviction thread was started, @c false if the thread was already running.
			///
			bool start() override;

			///
			/// Stops the eviction thread with the default timeout of @ref EVICTION_THREAD_JOIN_TIMEOUT.
			/// See also @ref stop(std::chrono::milliseconds)
			///
			bool stop() override;

			///
			/// Stops the eviction thread with the given @c timeout. If the timeout is reached the thread is terminated forcefully.
			/// @param[in] timeout The number of milliseconds to join the thread.
			/// @return @c true if stopping was successful, @c false if eviction thread is not running or could not be stopped in time.
			///
			bool stop(std::chrono::milliseconds timeout) override;

			///
			/// Checks if BeaconCacheEvictor stop was requested.
			/// 
			/// @return @c true if stop was requested, @c false otherwise.
			///
			bool isStopRequested();

			///
			/// Checks if the eviction thread is running or not.
			/// @return @c true if running, @c false otherwise
			///
			bool isAlive() override;

			///
			/// Update function to be notified about a new record being added.
			///
			void update() override;

			///
			/// The thread function
			////
			void cacheEvictionLoopFunc();

		private:
			/// Logger to write traces to
			std::shared_ptr<openkit::ILogger> mLogger;

			/// The Beacon cache to check if entries need to be evicted
			std::shared_ptr<IBeaconCache> mBeaconCache;

			/// Eviction strategies executed in an eviction run
			std::vector<std::shared_ptr<IBeaconCacheEvictionStrategy>> mStrategies;

			/// Thread being responsible for evicting records from the cache, based on an eviction strategy
			std::unique_ptr<core::util::ThreadSurrogate> mEvictionThread;

			/// Flag to stop the eviction thread
			bool mStop;

			/// Flag, which indicates that a new record was added to the cache, thus we need to execute the eviction strategies
			bool mRecordAdded;

			/// Mutex for condition variable
			std::mutex mMutex;

			/// To trigger thread operation
			std::condition_variable mConditionVariable;
		};

	}
}

#endif
