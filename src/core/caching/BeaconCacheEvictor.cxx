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
			std::bind(&BeaconCacheEvictor::isAlive, this)
		),
		std::make_shared<SpaceEvictionStrategy>
		(
			logger,
			beaconCache,
			configuration,
			std::bind(&BeaconCacheEvictor::isAlive, this)
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
	, mEvictionThread(nullptr)
	, mRunning(false)
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
		if (mLogger->isDebugEnabled())
		{
			mLogger->debug("BeaconCacheEvictor start() - Not starting BeaconCacheEviction thread, since it's already running");
		}
		return false;
	}

	std::unique_lock<std::mutex> lock(mMutex);
	mEvictionThread = std::unique_ptr<std::thread>(new std::thread(&BeaconCacheEvictor::cacheEvictionLoopFunc, this));
	while (!mRunning)
	{
		mConditionVariable.wait(lock);
	}

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

	// Instead of join() (=waiting for the thread to terminate) we detach the thread and wait up to the timeout time
	mEvictionThread->detach();
	std::chrono::steady_clock clock;
	auto t0 = clock.now();
	auto t1 = t0 + timeout;

	while (mRunning && clock.now() < t1)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	mEvictionThread = nullptr;

	if (isAlive())
	{
		// not stopped in time
		mLogger->warning("BeaconCacheEvictor stop() - BeaconCacheEviction thread was not stopped in time.");
		return false;
	}
	// stopped in time
	return true;
}

bool BeaconCacheEvictor::stopAndJoin()
{
	if (!isAlive())
	{
		// eviction thread not running, nothing to stop
		return false;
	}

	{
		std::unique_lock<std::mutex> lock(mMutex);
		mStop = true;
		mConditionVariable.notify_all();
	}

	mEvictionThread->join();
	mEvictionThread = nullptr;

	return true;
}

bool BeaconCacheEvictor::isAlive()
{
	std::unique_lock<std::mutex> lock(mMutex);
	return mRunning;
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
		mRunning = true;

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

	std::unique_lock<std::mutex> lock(mMutex);
	mRunning = false;

	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("BeaconCacheEvictor cacheEvictionLoopFunc() - BeaconCacheEviction thread is stopped.");
	}
}