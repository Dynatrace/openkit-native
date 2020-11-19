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

#include "SpaceEvictionStrategy.h"

#include <unordered_map>

using namespace core::caching;

SpaceEvictionStrategy::SpaceEvictionStrategy
(
	std::shared_ptr<openkit::ILogger> logger,
	std::shared_ptr<IBeaconCache> beaconCache,
	std::shared_ptr<core::configuration::IBeaconCacheConfiguration> configuration,
	std::function<bool()> isStopRequested
)
	: mLogger(logger)
	, mBeaconCache(beaconCache)
	, mConfiguration(configuration)
	, mIsStopRequested(isStopRequested)
	, mInfoShown(false)
{
}

void SpaceEvictionStrategy::execute()
{
	if (isStrategyDisabled())
	{
		// immediately return if this strategy is disabled
		if (!mInfoShown && mLogger->isInfoEnabled())
		{
			mLogger->info("SpaceEvictionStrategy execute() - strategy is disabled");
			// suppress any further log output
			mInfoShown = true;
		}

		return;
	}

	if (shouldRun())
	{
		doExecute();
	}
}

bool SpaceEvictionStrategy::isStrategyDisabled() const
{
	return mConfiguration->getCacheSizeLowerBound() <= 0
		|| mConfiguration->getCacheSizeUpperBound() <= 0
		|| mConfiguration->getCacheSizeUpperBound() < mConfiguration->getCacheSizeLowerBound();
}

bool SpaceEvictionStrategy::shouldRun() const
{
	return mBeaconCache->getNumBytesInCache() > mConfiguration->getCacheSizeUpperBound();
}

void SpaceEvictionStrategy::doExecute()
{
	std::unordered_map<BeaconKey, uint32_t, BeaconKey::Hash> removedRecordsPerBeacon;
	while (!mIsStopRequested() && mBeaconCache->getNumBytesInCache() > mConfiguration->getCacheSizeLowerBound())
	{
		auto beaconKeys = mBeaconCache->getBeaconKeys();
		auto it = beaconKeys.begin();
		while (!mIsStopRequested() && it != beaconKeys.end() && mBeaconCache->getNumBytesInCache() > mConfiguration->getCacheSizeLowerBound())
		{
			// remove 1 record from Beacon cache for given beaconID
			// the result is the number of records removed, which might be in range [0, numRecords=1]
			uint32_t numRecordsRemoved = mBeaconCache->evictRecordsByNumber(*it, 1);

			if (mLogger->isDebugEnabled())
			{
				auto itr = removedRecordsPerBeacon.find(*it);
				if (itr == removedRecordsPerBeacon.end())
				{
					removedRecordsPerBeacon.insert(std::make_pair(*it, numRecordsRemoved));
				}
				else
				{
					itr->second += numRecordsRemoved;
				}
			}

			it++;
		}
	}

	if (mLogger->isDebugEnabled())
	{
		for (auto itr = removedRecordsPerBeacon.begin(); itr != removedRecordsPerBeacon.end(); itr++)
		{
			mLogger->debug("SpaceEvictionStrategy doExecute() - Removed %u records from Beacon with key [sn=%d, seq=%d]",
				itr->second, itr->first.getBeaconId(), itr->first.getBeaconSequenceNumber());
		}
	}
}