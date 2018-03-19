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

SpaceEvictionStrategy::SpaceEvictionStrategy(std::shared_ptr<IBeaconCache> beaconCache, std::shared_ptr<configuration::BeaconCacheConfiguration> configuration)
	: mBeaconCache(beaconCache)
	, mConfiguration(configuration)
{
}

void SpaceEvictionStrategy::execute()
{
	if (isStrategyDisabled())
	{
		// immediately return if this strategy is disabled
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
	while (mBeaconCache->getNumBytesInCache() > mConfiguration->getCacheSizeLowerBound())
	{
		auto beaconIDs = mBeaconCache->getBeaconIDs();
		auto it = beaconIDs.begin();
		while (it != beaconIDs.end() && mBeaconCache->getNumBytesInCache() > mConfiguration->getCacheSizeLowerBound())
		{
			auto beaconID = *it;

			// remove 1 record from Beacon cache for given beaconID
			// the result is the number of records removed, which might be in range [0, numRecords=1]
			/*uint32_t numRecordsRemoved =*/ mBeaconCache->evictRecordsByNumber(beaconID, 1);

			it++;
		}
	}
}