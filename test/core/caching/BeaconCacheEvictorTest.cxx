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

#include "mock/MockIBeaconCache.h"
#include "mock/MockIBeaconCacheEvictionStrategy.h"
#include "../../api/mock/MockILogger.h"

#include "core/caching/BeaconCacheEvictor.h"
#include "core/caching/IObserver.h"
#include "core/util/CountDownLatch.h"
#include "core/util/CyclicBarrier.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <memory>
#include <vector>

using namespace test;

using BeaconCacheEvictor_t = core::caching::BeaconCacheEvictor;
using CountDownLatch_t = core::util::CountDownLatch;
using CyclicBarrier_t = core::util::CyclicBarrier;
using IObserver_t = core::caching::IObserver;
using MockNiceIBeaconCache_sp = std::shared_ptr<testing::NiceMock<MockIBeaconCache>>;
using MockNiceILogger_sp = std::shared_ptr<testing::NiceMock<MockILogger>>;
using MockNiceIBeaconCacheEvictionStrategy_sp = std::shared_ptr<testing::NiceMock<MockIBeaconCacheEvictionStrategy>>;

class BeaconCacheEvictorTest : public testing::Test
{
protected:

	MockNiceILogger_sp mockLogger;
	MockNiceIBeaconCache_sp mockBeaconCache;
	MockNiceIBeaconCacheEvictionStrategy_sp mockStrategyOne;
	MockNiceIBeaconCacheEvictionStrategy_sp mockStrategyTwo;

	void SetUp() override
	{
		mockLogger = MockILogger::createNice();
		mockBeaconCache = MockIBeaconCache::createNice();
		mockStrategyOne = MockIBeaconCacheEvictionStrategy::createNice();
		mockStrategyTwo = MockIBeaconCacheEvictionStrategy::createNice();
	}
};

TEST_F(BeaconCacheEvictorTest, aDefaultConstructedBeaconCacheEvictorIsNotAlive)
{
	// given
	BeaconCacheEvictor_t evictor(mockLogger, mockBeaconCache, {});

	// then
	ASSERT_FALSE(evictor.isAlive());
}

TEST_F(BeaconCacheEvictorTest, afterStartingABeaconCacheEvictorItIsAlive)
{
	// given
	BeaconCacheEvictor_t evictor(mockLogger, mockBeaconCache, {});
	auto obtained = evictor.start();

	// then
	ASSERT_TRUE(obtained);
	ASSERT_TRUE(evictor.isAlive());

	evictor.stopAndJoin();
}



TEST_F(BeaconCacheEvictorTest, startingAnAlreadyAliveBeaconCacheEvictorDoesNothing)
{
	// given
	BeaconCacheEvictor_t evictor(mockLogger, mockBeaconCache, {});
	evictor.start();

	// when trying to start the evictor again
	auto obtained = evictor.start();

	// then
	ASSERT_FALSE(obtained);
	ASSERT_TRUE(evictor.isAlive());

	evictor.stop();
}

TEST_F(BeaconCacheEvictorTest, stoppingABeaconCacheEvictorWhichIsNotAliveDoesNothing)
{
	// given
	BeaconCacheEvictor_t evictor(mockLogger, mockBeaconCache, {});

	// when
	auto obtained = evictor.stop();

	// then
	ASSERT_FALSE(obtained);
	ASSERT_FALSE(evictor.isAlive());
}

TEST_F(BeaconCacheEvictorTest, stoppingAnAliveBeaconCacheEvictor)
{
	// given
	BeaconCacheEvictor_t evictor(mockLogger, mockBeaconCache, {});
	evictor.start();

	// when
	auto obtained = evictor.stop();

	// then
	ASSERT_TRUE(obtained);
	ASSERT_FALSE(evictor.isAlive());
}

TEST_F(BeaconCacheEvictorTest, stoppingABeaconCacheEvictorWithASmallTimeout)
{
	// given
	ON_CALL(*mockStrategyOne, execute())
		.WillByDefault(testing::Invoke(
			[]() -> void
	{
		// simulate the strategy execution to take 1s
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	));

	BeaconCacheEvictor_t evictor(mockLogger, mockBeaconCache, { mockStrategyOne });
	evictor.start();

	// give the eviction thread enough time to at least execute the strategy before stopping it
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	// when
	auto stopped = evictor.stop(std::chrono::seconds(10));

	// then
	ASSERT_TRUE(stopped);
}

TEST_F(BeaconCacheEvictorTest, stoppingABeaconCacheEvictorWithAHugeTimeout)
{
	// given
	ON_CALL(*mockStrategyOne, execute())
		.WillByDefault(testing::Invoke(
			[]() -> void
	{
		// simulate the strategy execution to take 10s
		std::this_thread::sleep_for(std::chrono::seconds(10));
	}
	));

	BeaconCacheEvictor_t evictor(mockLogger, mockBeaconCache, { mockStrategyOne });
	evictor.start();

	// give the eviction thread enough time to at least execute the strategy before stopping it
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	// when
	auto stopped = evictor.stop(std::chrono::seconds(1));

	// then
	ASSERT_TRUE(stopped);
}

TEST_F(BeaconCacheEvictorTest, triggeringEvictionStrategiesInThread)
{
	// given
	std::vector<IObserver_t*> observers;
	CountDownLatch_t addObserverLatch(1);
	CyclicBarrier_t strategyInvokedBarrier(2);

	ON_CALL(*mockBeaconCache, addObserver(testing::_))
		.WillByDefault(testing::Invoke(
			[&observers, &addObserverLatch](IObserver_t* observer) -> void
			{
				observers.push_back(observer);
				addObserverLatch.countDown();
			}
		));

	ON_CALL(*mockStrategyTwo, execute())
		.WillByDefault(testing::Invoke(
			[&strategyInvokedBarrier]() -> void
			{
				strategyInvokedBarrier.await();
			}
		));

	// first step start the eviction thread
	BeaconCacheEvictor_t evictor(mockLogger, mockBeaconCache, { mockStrategyOne, mockStrategyTwo });
	evictor.start();

	// wait until the eviction thread registered itself as observer
	addObserverLatch.await();

	// verify the observer was set
	ASSERT_FALSE(observers.empty());

	// update() shall (indirectly) trigger execute() on the eviction strategy
	EXPECT_CALL(*mockStrategyOne, execute())
		.Times(testing::Exactly(10));
	EXPECT_CALL(*mockStrategyTwo, execute())
		.Times(testing::Exactly(10));

	// do some updates
	for (int i = 0; i < 10; i++)
	{
		auto it = observers.begin();
		(*it)->update();
		strategyInvokedBarrier.await();
	}

	// stop the stuff and ensure it's invoked
	auto stopped = evictor.stop();

	// then
	ASSERT_TRUE(stopped);
	ASSERT_FALSE(evictor.isAlive());
}
