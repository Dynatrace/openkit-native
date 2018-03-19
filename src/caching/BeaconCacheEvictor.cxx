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

#include "caching/BeaconCacheEvictor.h"
#include "caching/TimeEvictionStrategy.h"
#include "caching/SpaceEvictionStrategy.h"

#include <chrono>

using namespace caching;

constexpr std::chrono::milliseconds EVICTION_THREAD_JOIN_TIMEOUT = std::chrono::seconds(2);

BeaconCacheEvictor::BeaconCacheEvictor(std::shared_ptr<IBeaconCache> beaconCache, std::shared_ptr<configuration::BeaconCacheConfiguration> configuration, std::shared_ptr<providers::ITimingProvider> timingProvider)
	: BeaconCacheEvictor(beaconCache, std::initializer_list<std::shared_ptr<IBeaconCacheEvictionStrategy>>{std::make_shared<TimeEvictionStrategy>(beaconCache, configuration, timingProvider), std::make_shared<SpaceEvictionStrategy>(beaconCache, configuration)})
{

}

BeaconCacheEvictor::BeaconCacheEvictor(std::shared_ptr<IBeaconCache> beaconCache, std::vector<std::shared_ptr<IBeaconCacheEvictionStrategy>> strategies)
	: mBeaconCache(beaconCache)
	, mStrategies(strategies)
	, mEvictionThread(nullptr)
	, mStop(false)
	, mRecordAdded(false)
	, mMutex()
	, mConditionVariable()
{
}

bool BeaconCacheEvictor::start()
{
	if (isAlive())
	{
		// eviction thread already running
		return false;
	}

	mEvictionThread = std::unique_ptr<std::thread>(new std::thread(&BeaconCacheEvictor::cacheEvictionLoopFunc, this));
	//mEvictionThread->join(); // TODO: Ask Stefan why would I need this.

	return true;
}

bool BeaconCacheEvictor::stop()
{
	std::unique_lock<std::mutex> lock(mMutex);

	if (!isAlive())
	{
		// eviction thread not running, nothing to stop
		return false;
	}

	mStop = true;
	mConditionVariable.notify_all();

	mEvictionThread->join();
	mEvictionThread = nullptr;

	return true;
}

#if 0 // TODO: Discuss with Stefan if it is OK to to skip this function
bool BeaconCacheEvictor::stop(int64_t timeout)
{
	return stop();
}
#endif

bool BeaconCacheEvictor::isAlive() const
{
	return mEvictionThread == nullptr || mStop == true;
}

void BeaconCacheEvictor::update()
{
	std::unique_lock<std::mutex> lock(mMutex);

	mRecordAdded = true;
	mConditionVariable.notify_all();
}


void BeaconCacheEvictor::cacheEvictionLoopFunc()
{
	std::unique_lock<std::mutex> lock(mMutex);

	// first register ourselves
	mBeaconCache->addObserver(this);

	while (!mStop)
	{
		mConditionVariable.wait(lock);

		// reset the added flag
		mRecordAdded = false;

		// a new record has been added to the cache
		// run all eviction strategies, to perform cache cleanup
		for (auto it = mStrategies.begin(); it != mStrategies.end(); ++it)
		{
			it->get()->execute();
		}
	}
}