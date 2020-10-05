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

#ifndef _CORE_CACHING_TIMEEVICTIONSTRATEGY_H
#define _CORE_CACHING_TIMEEVICTIONSTRATEGY_H

#include "OpenKit/ILogger.h"
#include "IBeaconCache.h"
#include "IBeaconCacheEvictionStrategy.h"
#include "core/configuration/IBeaconCacheConfiguration.h"
#include "providers/ITimingProvider.h"

#include <cstdint>
#include <memory>
#include <functional>

namespace core
{
	namespace caching
	{
		///
		/// Time based eviction strategy for the beacon cache.
		///
		/// This strategy deletes all records from @ref BeaconCache exceeding a certain age.
		///
		class TimeEvictionStrategy : public IBeaconCacheEvictionStrategy
		{
		public:
			///
			/// Constructor.
			/// @param[in] logger to write traces to
			/// @param[in] beaconCache The beacon cache to evict if necessary.
			/// @param[in] configuration The configuration providing the boundary settings for this strategy.
			/// @param[in] timingProvider Timing provider required for time retrieval
			/// @param[in] isStopRequested function to check whether the eviction thread stop was requested or not
			///
			TimeEvictionStrategy(
				std::shared_ptr<openkit::ILogger> logger,
				std::shared_ptr<IBeaconCache> beaconCache,
				std::shared_ptr<configuration::IBeaconCacheConfiguration> configuration,
				std::shared_ptr<providers::ITimingProvider> timingProvider,
				std::function<bool()> isStopRequested
			);

			///
			/// Destructor
			///
			~TimeEvictionStrategy() override = default;

			///
			/// Delete the copy constructor
			///
			TimeEvictionStrategy(const TimeEvictionStrategy&) = delete;

			///
			/// Delete the assignment operator
			///
			TimeEvictionStrategy& operator = (const TimeEvictionStrategy &) = delete;

			///
			/// Called when this strategy is executed.
			///
			void execute() override;

			///
			/// Checks if the strategy is disabled.
			///
			/// The strategy might be disabled on purpose, if the maximum record age is less than or equal to zero.
			///
			/// @return @c true if strategy is disabled, @c false otherwise.
			///
			bool isStrategyDisabled() const;

			///
			/// Get a boolean flag indicating whether the strategy shall be executed or not.
			///
			/// @return @c true if the strategy shall be executed, @c false otherwise.
			///
			bool shouldRun() const;

			///
			/// Get the timestamp when this strategy was executed last.
			///
			/// @return A timestamp (the number of milliseconds elapsed, since 1970-01-01) when this strategy was last time executed.
			///
			int64_t getLastRunTimestamp() const;

			///
			/// Set the timestamp when this strategy was executed last.
			///
			/// @param[in] lastRunTimestamp A timestamp (the number of milliseconds elapsed, since 1970-01-01) when this strategy was last time executed.
			///
			void setLastRunTimestamp(int64_t lastRunTimestamp);

		private:
			///
			/// Real strategy execution.
			///
			void doExecute();

		private:
			/// Logger to write traces to
			std::shared_ptr<openkit::ILogger> mLogger;

			/// The Beacon cache to check if entries need to be evicted
			std::shared_ptr<IBeaconCache> mBeaconCache;

			/// The configuration providing the boundary settings for this strategy.
			std::shared_ptr<configuration::IBeaconCacheConfiguration> mConfiguration;

			/// Timing provider to get timestamps
			std::shared_ptr<providers::ITimingProvider> mTimingProvider;

			/// Timestamp of the last eviction strategy execution
			int64_t mLastRunTimestamp;

			/// Function to check whether the eviction thread should be stopped or not
			std::function<bool()> mIsStopRequested;

			/// Flag to suppress cyclic log output
			bool mInfoShown;
		};
	}
}

#endif
