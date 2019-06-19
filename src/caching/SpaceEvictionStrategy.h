/**
* Copyright 2018-2019 Dynatrace LLC
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

#ifndef _CACHING_SPACEEVICTIONSTRATEGY_H
#define _CACHING_SPACEEVICTIONSTRATEGY_H

#include "OpenKit/ILogger.h"
#include "caching/IBeaconCache.h"
#include "caching/IBeaconCacheEvictionStrategy.h"
#include "configuration/BeaconCacheConfiguration.h"

#include <memory>
#include <functional>

namespace caching
{
	///
	/// Space based eviction strategy for the beacon cache.
	///
	/// This strategy checks if the number of cached bytes is greater than @ref configuration::BeaconCacheConfiguration::getCacheSizeLowerBound()
	/// and in this case runs the strategy.
	///
	class SpaceEvictionStrategy : public IBeaconCacheEvictionStrategy
	{
	public:
		///
		/// Constructor.
		/// @param[in] logger to write traces to
		/// @param[in] beaconCache The beacon cache to evict if necessary.
		/// @param[in] configuration The configuration providing the boundary settings for this strategy.
		/// @param[in] isAlive function to check whether the eviction thread is running or not
		///
		SpaceEvictionStrategy(std::shared_ptr<openkit::ILogger> logger, std::shared_ptr<IBeaconCache> beaconCache, std::shared_ptr<configuration::BeaconCacheConfiguration> configuration, std::function<bool()> isAlive);

		///
		/// Destructor
		///
		virtual ~SpaceEvictionStrategy() {}

		///
		/// Delete the copy constructor
		///
		SpaceEvictionStrategy(const SpaceEvictionStrategy&) = delete;

		///
		/// Delete the assignment operator
		///
		SpaceEvictionStrategy& operator = (const SpaceEvictionStrategy &) = delete;

		///
		/// Called when this strategy is executed.
		///
		void execute() override;

		///
		/// Checks if the strategy is disabled.
		///
		/// The strategy might be disabled on purpose, if upper and lower boundary are less than or equal to 0
		/// or accidentally, when either lower or upper boundary is less than or equal to zero or upper boundary is less
		/// than lower boundary.
		///
		/// @return @c true if strategy is disabled, @c false otherwise.
		///
		bool isStrategyDisabled() const;

		///
		/// Checks if the strategy should run.
		///
		/// The strategy should run, if the currently stored number of bytes in the Beacon cache exceeds the configured upper limit.
		///
		/// @return @c true if the strategy should run, @c false otherwise.
		///
		bool shouldRun() const;

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
		std::shared_ptr<configuration::BeaconCacheConfiguration> mConfiguration;

		/// Function to check whether the eviction thread is running or not
		std::function<bool()> mIsAliveFunction;

		/// Flag to suppress cyclic log output
		bool mInfoShown;
	};

}

#endif
