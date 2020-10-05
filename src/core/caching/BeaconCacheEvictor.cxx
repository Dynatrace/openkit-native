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

#include "BeaconCacheEvictor.h"
#include "TimeEvictionStrategy.h"
#include "SpaceEvictionStrategy.h"

#include <chrono>

using namespace core::caching;

constexpr std::chrono::milliseconds EVICTION_THREAD_JOIN_TIMEOUT = std::chrono::seconds(2);

BeaconCacheEvictor::BeaconCacheEvictor
(
	std::shared_ptr<openkit::ILogger> logger,
	std::shared_ptr<IBeaconCache> beaconCache,
	std::shared_ptr<core::configuration::IBeaconCacheConfiguration> configuration,
	std::shared_ptr<providers::ITimingProvider> timingProvider
)
: BeaconCacheEvictor
(
	logger,
	beaconCache,
	{
		std::make_shared<TimeEvictionStrategy>
		(
			logger,
			beaconCache,
			configuration,
			timingProvider,
			std::bind(&BeaconCacheEvictor::isStopRequested, this)
		),
		std::make_shared<SpaceEvictionStrategy>
		(
			logger,
			beaconCache,
			configuration,
			std::bind(&BeaconCacheEvictor::isStopRequested, this)
		)
	}
)
{
}

BeaconCacheEvictor::BeaconCacheEvictor
(
	std::shared_ptr<openkit::ILogger> logger,
	std::shared_ptr<IBeaconCache> beaconCache,
	std::vector<std::shared_ptr<IBeaconCacheEvictionStrategy>> strategies
)
	: mLogger(logger)
	, mBeaconCache(beaconCache)
	, mStrategies(strategies)
	, mEvictionThread(new core::util::ThreadSurrogate())
	, mStop(false)
	, mRecordAdded(false)
	, mMutex()
	, mConditionVariable()
{
}

bool BeaconCacheEvictor::start()
{
	if (mEvictionThread->isStarted())
	{
		// eviction thread already started
		if (mLogger->isDebugEnabled())
		{
			mLogger->debug("BeaconCacheEvictor start() - Not starting BeaconCacheEviction thread, since it's already started");
		}

		return false;
	}

	std::unique_lock<std::mutex> lock(mMutex);
	mEvictionThread->start(std::bind(&BeaconCacheEvictor::cacheEvictionLoopFunc, this));

	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("BeaconCacheEvictior start() - BeaconCacheEviction thread started.");
	}

	return true;
}

bool BeaconCacheEvictor::stop()
{
	return stop(EVICTION_THREAD_JOIN_TIMEOUT);
}

bool BeaconCacheEvictor::stop(std::chrono::milliseconds timeout)
{
	if (!isAlive())
	{
		// eviction thread not running, nothing to stop
		if (mLogger->isDebugEnabled())
		{
			mLogger->debug("BeaconCacheEvictior stop() - Not stopping BeaconCacheEviction thread, since it's not alive");
		}

		return false;
	}

	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("BeaconCacheEvictior stop() - Stopping BeaconCacheEviction thread.");
	}

	{
		std::unique_lock<std::mutex> lock(mMutex);
		mStop = true;
		mConditionVariable.notify_all();
	}

	return mEvictionThread->join(timeout.count());
}

bool BeaconCacheEvictor::isStopRequested()
{
	std::unique_lock<std::mutex> lock(mMutex);

	return mStop;
}

bool BeaconCacheEvictor::isAlive()
{
	return mEvictionThread->isAlive();
}

void BeaconCacheEvictor::update()
{
	std::unique_lock<std::mutex> lock(mMutex);

	mRecordAdded = true;
	mConditionVariable.notify_all();
}

void BeaconCacheEvictor::cacheEvictionLoopFunc()
{
	{
		std::unique_lock<std::mutex> lock(mMutex);

		// we've started up -> let the start() function return
		mConditionVariable.notify_all();
	}

	// first register ourselves
	mBeaconCache->addObserver(this);

	while (true)
	{
		{
			std::unique_lock<std::mutex> lock(mMutex);
			while (!mRecordAdded && !mStop)
			{
				mConditionVariable.wait(lock);
			}

			if (mStop)
			{
				// exit the thread
				break;
			}

			// reset the added flag
			mRecordAdded = false;
		}

		// a new record has been added to the cache
		// run all eviction strategies, to perform cache cleanup
		for (auto it = mStrategies.begin(); it != mStrategies.end(); ++it)
		{
			it->get()->execute();
		}
	}

	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("BeaconCacheEvictor cacheEvictionLoopFunc() - BeaconCacheEviction thread is stopped.");
	}
}