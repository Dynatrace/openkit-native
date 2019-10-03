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

#include "TimeEvictionStrategy.h"

#include <map>

using namespace core::caching;

TimeEvictionStrategy::TimeEvictionStrategy
(
	std::shared_ptr<openkit::ILogger> logger,
	std::shared_ptr<IBeaconCache> beaconCache,
	std::shared_ptr<core::configuration::BeaconCacheConfiguration> configuration,
	std::shared_ptr<providers::ITimingProvider> timingProvider,
	std::function<bool()> isAlive
)
	: mLogger(logger)
	, mBeaconCache(beaconCache)
	, mConfiguration(configuration)
	, mTimingProvider(timingProvider)
	, mLastRunTimestamp(-1)
	, mIsAliveFunction(isAlive)
	, mInfoShown(false)
{
}

void TimeEvictionStrategy::execute()
{
	if (isStrategyDisabled())
	{
		// immediately return if this strategy is disabled
		if (!mInfoShown && mLogger->isInfoEnabled())
		{
			mLogger->info("TimeEvictionStrategy execute() - strategy is disabled");
			// suppress any further log output
			mInfoShown = true;
		}
		return;
	}

	if (mLastRunTimestamp < 0)
	{
		// first time execution
		mLastRunTimestamp = mTimingProvider->provideTimestampInMilliseconds();
	}

	if (shouldRun())
	{
		doExecute();
	}
}

bool TimeEvictionStrategy::isStrategyDisabled() const
{
	return mConfiguration->getMaxRecordAge() <= 0;
}

bool TimeEvictionStrategy::shouldRun() const
{
	// if delta since we last ran is >= the maximum age, we should run, otherwise this run can be skipped
	int64_t currentTimestamp = mTimingProvider->provideTimestampInMilliseconds();
	return (currentTimestamp - mLastRunTimestamp) >= mConfiguration->getMaxRecordAge();
}

int64_t TimeEvictionStrategy::getLastRunTimestamp() const
{
	return mLastRunTimestamp;
}

void TimeEvictionStrategy::setLastRunTimestamp(int64_t lastRunTimestamp)
{
	mLastRunTimestamp = lastRunTimestamp;
}

void TimeEvictionStrategy::doExecute()
{
	auto beaconIDs = mBeaconCache->getBeaconIDs();
	if (beaconIDs.empty())
	{
		// no beacons - set last run timestamp and return immediately
		setLastRunTimestamp(mTimingProvider->provideTimestampInMilliseconds());
		return;
	}

	// retrieve the timestamp when we start with execution
	int64_t currentTimestamp = mTimingProvider->provideTimestampInMilliseconds();
	int64_t smallestAllowedBeaconTimestamp = currentTimestamp - mConfiguration->getMaxRecordAge();

	// iterate over the previously obtained set and evict for each beacon
	auto it = beaconIDs.begin();
	while (mIsAliveFunction() && it != beaconIDs.end())
	{
		auto beaconID = *it;

		uint32_t numRecordsRemoved = mBeaconCache->evictRecordsByAge(beaconID, smallestAllowedBeaconTimestamp);

		if (numRecordsRemoved > 0 && mLogger->isDebugEnabled())
		{
			mLogger->debug("TimeEvictionStrategy doExecute() - Removed %u records from Beacon with ID %d", numRecordsRemoved, beaconID);
		}

		it++;
	}



	// last but not least update the last runtime
	setLastRunTimestamp(currentTimestamp);
}