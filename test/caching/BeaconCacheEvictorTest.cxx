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

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "caching/BeaconCache.h"
#include "caching/BeaconCacheEvictor.h"
#include "core/util/CountDownLatch.h"
#include "core/util/CyclicBarrier.h"
#include "../caching/MockBeaconCache.h"
#include "../caching/MockBeaconCacheEvictionStrategy.h"

#include <vector>

using namespace caching;

class BeaconCacheEvictorTest : public testing::Test
{
public:
	void SetUp()
	{
		mMockBeaconCache = std::shared_ptr<testing::NiceMock<test::MockBeaconCache>>(new testing::NiceMock<test::MockBeaconCache>());
		mMockStrategyOne = std::shared_ptr<testing::NiceMock<test::MockBeaconCacheEvictionStrategy>>(new testing::NiceMock<test::MockBeaconCacheEvictionStrategy>());
		mMockStrategyTwo = std::shared_ptr<testing::NiceMock<test::MockBeaconCacheEvictionStrategy>>(new testing::NiceMock<test::MockBeaconCacheEvictionStrategy>());
	}

	void TearDown()
	{
		mMockBeaconCache = nullptr;
		mMockStrategyOne = nullptr;
		mMockStrategyTwo = nullptr;
	}

	//std::shared_ptr<BeaconCacheEvictor> mEvictor;
	std::shared_ptr<testing::NiceMock<test::MockBeaconCacheEvictionStrategy>> mMockStrategyOne;
	std::shared_ptr<testing::NiceMock<test::MockBeaconCacheEvictionStrategy>> mMockStrategyTwo;
	std::shared_ptr<testing::NiceMock<test::MockBeaconCache>> mMockBeaconCache;
};

TEST_F(BeaconCacheEvictorTest, aDefaultConstructedBeaconCacheEvictorIsNotAlive)
{
	// given
	BeaconCacheEvictor evictor(mMockBeaconCache, {});

	// then
	ASSERT_FALSE(evictor.isAlive());
}

TEST_F(BeaconCacheEvictorTest, afterStartingABeaconCacheEvictorItIsAlive)
{
	// given
	BeaconCacheEvictor evictor(mMockBeaconCache, {});
	auto obtained = evictor.start();

	// then
	ASSERT_TRUE(obtained);
	ASSERT_TRUE(evictor.isAlive());

	evictor.stopAndJoin();
}



TEST_F(BeaconCacheEvictorTest, startingAnAlreadyAliveBeaconCacheEvictorDoesNothing)
{
	// given
	BeaconCacheEvictor evictor(mMockBeaconCache, {});
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
	BeaconCacheEvictor evictor(mMockBeaconCache, {});
	
	// when
	auto obtained = evictor.stop();

	// then
	ASSERT_FALSE(obtained);
	ASSERT_FALSE(evictor.isAlive());
}

TEST_F(BeaconCacheEvictorTest, stoppingAnAliveBeaconCacheEvictor)
{
	// given
	BeaconCacheEvictor evictor(mMockBeaconCache, {});
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
	ON_CALL(*mMockStrategyOne, execute())
		.WillByDefault(testing::Invoke(
			[]() -> void
	{
		// simulate the strategy execution to take 1s
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	));

	BeaconCacheEvictor evictor(mMockBeaconCache, { mMockStrategyOne });
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
	ON_CALL(*mMockStrategyOne, execute())
		.WillByDefault(testing::Invoke(
			[]() -> void
	{
		// simulate the strategy execution to take 10s
		std::this_thread::sleep_for(std::chrono::seconds(10));
	}
	));

	BeaconCacheEvictor evictor(mMockBeaconCache, { mMockStrategyOne });
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
	std::vector<IObserver*> observers;
	core::util::CountDownLatch addObserverLatch(1);
	core::util::CyclicBarrier strategyInvokedBarrier(2);

	ON_CALL(*mMockBeaconCache, addObserver(testing::_))
		.WillByDefault(testing::Invoke(
			[&observers, &addObserverLatch](IObserver* observer) -> void
			{
				observers.push_back(observer);
				addObserverLatch.countDown();
			}
		));

	ON_CALL(*mMockStrategyTwo, execute())
		.WillByDefault(testing::Invoke(
			[&strategyInvokedBarrier]() -> void
			{
				strategyInvokedBarrier.await();
			}
		));

	// first step start the eviction thread
	BeaconCacheEvictor evictor(mMockBeaconCache, { mMockStrategyOne, mMockStrategyTwo });
	evictor.start();

	// wait until the eviction thread registered itself as observer
	addObserverLatch.await();

	// verify the observer was set
	ASSERT_FALSE(observers.empty());

	// update() shall (indirectly) trigger execute() on the eviction strategy
	EXPECT_CALL(*mMockStrategyOne, execute())
		.Times(testing::Exactly(10));
	EXPECT_CALL(*mMockStrategyTwo, execute())
		.Times(testing::Exactly(10));

	// do some updates
	for (int i = 0; i < 10; i++)
	{
		auto it = observers.begin();
		(*it)->update();
		strategyInvokedBarrier.await();
		strategyInvokedBarrier.reset();
	}

	// stop the stuff and ensure it's invoked
	auto stopped = evictor.stop();

	// then
	ASSERT_TRUE(stopped);
	ASSERT_FALSE(evictor.isAlive());
}
