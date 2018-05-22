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

#include "SpaceEvictionStrategy.h"

#include <map>

using namespace caching;

SpaceEvictionStrategy::SpaceEvictionStrategy(std::shared_ptr<openkit::ILogger> logger, std::shared_ptr<IBeaconCache> beaconCache, std::shared_ptr<configuration::BeaconCacheConfiguration> configuration, std::function<bool()> isAlive)
	: mLogger(logger)
	, mBeaconCache(beaconCache)
	, mConfiguration(configuration)
	, mIsAliveFunction(isAlive)
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
			mLogger->info("SpaceEvictionStrategy execute()  - strategy is disabled");
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
	std::map<int32_t, uint32_t> removedRecordsPerBeacon;
	while (mIsAliveFunction() && mBeaconCache->getNumBytesInCache() > mConfiguration->getCacheSizeLowerBound())
	{
		auto beaconIDs = mBeaconCache->getBeaconIDs();
		auto it = beaconIDs.begin();
		while (mIsAliveFunction() && it != beaconIDs.end() && mBeaconCache->getNumBytesInCache() > mConfiguration->getCacheSizeLowerBound())
		{
			auto beaconID = *it;

			// remove 1 record from Beacon cache for given beaconID
			// the result is the number of records removed, which might be in range [0, numRecords=1]
			uint32_t numRecordsRemoved = mBeaconCache->evictRecordsByNumber(beaconID, 1);

			if (mLogger->isDebugEnabled())
			{
				auto itr = removedRecordsPerBeacon.find(beaconID);
				if (itr == removedRecordsPerBeacon.end())
				{
					removedRecordsPerBeacon.insert(std::make_pair(beaconID, numRecordsRemoved));
				}
				else
				{
					removedRecordsPerBeacon.insert(std::make_pair(beaconID, itr->second + numRecordsRemoved));
				}
			}

			it++;
		}
	}

	if (mLogger->isDebugEnabled())
	{
		for (auto itr = removedRecordsPerBeacon.begin(); itr != removedRecordsPerBeacon.end(); itr++)
		{
			mLogger->debug("SpaceEvictionStrategy doExecute() - Removed %u records from Beacon with ID %d", itr->second, itr->first);
		}
	}
}