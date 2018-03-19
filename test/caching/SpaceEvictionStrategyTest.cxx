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

#include "configuration/BeaconCacheConfiguration.h"
#include "caching/BeaconCache.h"
#include "caching/SpaceEvictionStrategy.h"
#include "../caching/MockBeaconCache.h"

#include <memory>

using namespace configuration;
using namespace caching;

class SpaceEvictionStrategyTest : public testing::Test
{
public:
	SpaceEvictionStrategyTest()
		: mockBeaconCache()
	{
	}

	void SetUp()
	{
		mockBeaconCache = std::shared_ptr<testing::NiceMock<test::MockBeaconCache>>(new testing::NiceMock<test::MockBeaconCache>());
	}

	void TearDown()
	{
	}

	std::shared_ptr<testing::NiceMock<test::MockBeaconCache>> mockBeaconCache;
};

TEST_F(SpaceEvictionStrategyTest, theStrategyIsDisabledIfCacheSizeLowerBoundIsEqualToZero)
{
	// given
	auto configuration = std::make_shared<BeaconCacheConfiguration>(1000L, 0L, 2000L);
	auto target = new SpaceEvictionStrategy(mockBeaconCache, configuration);

	// then
	ASSERT_TRUE(target->isStrategyDisabled());
}

TEST_F(SpaceEvictionStrategyTest, theStrategyIsDisabledIfCacheSizeLowerBoundIsLessThanZero)
{
	// given
	auto configuration = std::make_shared<BeaconCacheConfiguration>(1000L, -1L, 2000L);
	auto target = new SpaceEvictionStrategy(mockBeaconCache, configuration);

	// then
	ASSERT_TRUE(target->isStrategyDisabled());
}

TEST_F(SpaceEvictionStrategyTest, theStrategyIsDisabledIfCacheSizeUpperBoundIsEqualToZero)
{
	// given
	auto configuration = std::make_shared<BeaconCacheConfiguration>(1000L, 1000L, 0L);
	auto target = new SpaceEvictionStrategy(mockBeaconCache, configuration);

	// then
	ASSERT_TRUE(target->isStrategyDisabled());
}

TEST_F(SpaceEvictionStrategyTest, theStrategyIsDisabledIfCacheSizeUpperBoundIsLessThanLowerBound)
{
	// given
	auto configuration = std::make_shared<BeaconCacheConfiguration>(1000L, 1000L, 999L);
	auto target = new SpaceEvictionStrategy(mockBeaconCache, configuration);

	// then
	ASSERT_TRUE(target->isStrategyDisabled());
}

TEST_F(SpaceEvictionStrategyTest, shouldRunGivesTrueIfNumBytesInCacheIsGreaterThanUpperBoundLimit)
{
	// given
	auto configuration = std::make_shared<BeaconCacheConfiguration>(1000L, 1000L, 2000L);
	auto target = new SpaceEvictionStrategy(mockBeaconCache, configuration);
	

	//when
	ON_CALL(*mockBeaconCache, getNumBytesInCache())
		.WillByDefault(testing::Return(configuration->getCacheSizeUpperBound() + 1));

	// then
	ASSERT_TRUE(target->shouldRun());
}