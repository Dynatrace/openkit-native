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
		//mEvictor = std::shared_ptr<BeaconCacheEvictor>(new BeaconCacheEvictor(mMockBeaconCache, {}));
	}

	void TearDown()
	{
		//if (mEvictor != nullptr)
		//{
		//	mEvictor->stop();
		//}
		mMockBeaconCache = nullptr;
	}

	//std::shared_ptr<BeaconCacheEvictor> mEvictor;
	testing::NiceMock<test::MockBeaconCacheEvictionStrategy> mockStrategyOne;
	testing::NiceMock<test::MockBeaconCacheEvictionStrategy> mockStrategyTwo;
	std::shared_ptr<testing::NiceMock<test::MockBeaconCache>> mMockBeaconCache;
};

TEST_F(BeaconCacheEvictorTest, afterStartingABeaconCacheEvictorItIsAlive)
{
	// given
	BeaconCacheEvictor evictor(mMockBeaconCache, {});

	// then
	ASSERT_FALSE(evictor.isAlive());
}

TEST_F(BeaconCacheEvictorTest, aDefaultConstructedBeaconCacheEvictorIsNotAlive)
{
	// given
	BeaconCacheEvictor evictor(mMockBeaconCache, {});
	auto obtained = evictor.start();

	// then
	ASSERT_TRUE(obtained);
	ASSERT_TRUE(evictor.isAlive());

	evictor.stop();
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

	evictor.stop();
}

TEST_F(BeaconCacheEvictorTest, stoppingABeaconCacheEvictorWithAHugeTimeout)
{
	// given
	BeaconCacheEvictor evictor(mMockBeaconCache, {});
	auto obtained = evictor.start();

	// when
	obtained = evictor.stop(10000); // 10s

	// then
	ASSERT_TRUE(obtained);
}

TEST_F(BeaconCacheEvictorTest, stoppingABeaconCacheEvictorWithAZeroTimeout)
{
	// given
	BeaconCacheEvictor evictor(mMockBeaconCache, {});
	auto obtained = evictor.start();

	// when
	obtained = evictor.stop(0);

	// then
	ASSERT_FALSE(obtained);
}
