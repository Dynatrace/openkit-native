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

#ifndef _CACHING_TIMEEVICTIONSTRATEGY_H
#define _CACHING_TIMEEVICTIONSTRATEGY_H

#include "caching/IBeaconCache.h"
#include "caching/IBeaconCacheEvictionStrategy.h"
#include "configuration/BeaconCacheConfiguration.h"
#include "providers/ITimingProvider.h"

#include <cstdint>
#include <memory>

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
		/// @param[in] beaconCache The beacon cache to evict if necessary.
		/// @param[in] configuration The configuration providing the boundary settings for this strategy.
		/// @param[in] timingProvider Timing provider required for time retrieval
		///
		TimeEvictionStrategy(std::shared_ptr<IBeaconCache> beaconCache, std::shared_ptr<configuration::BeaconCacheConfiguration> configuration, std::shared_ptr<providers::ITimingProvider> timingProvider);

		///
		/// Default Destructor
		///
		virtual ~TimeEvictionStrategy() {}

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
		/// The Beacon cache to check if entries need to be evicted
		std::shared_ptr<IBeaconCache> mBeaconCache;

		/// The configuration providing the boundary settings for this strategy.
		std::shared_ptr<configuration::BeaconCacheConfiguration> mConfiguration;

		/// Timing provider to get timestamps
		std::shared_ptr<providers::ITimingProvider> mTimingProvider;

		/// Timestamp of the last eviction strategy execution
		int64_t mLastRunTimestamp;
	};

}

#endif
