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
#include "caching/TimeEvictionStrategy.h"
#include "../caching/MockBeaconCache.h"
#include "../providers/MockTimingProvider.h"

#include <memory>

using namespace configuration;
using namespace caching;

class TimeEvictionStrategyTest : public testing::Test
{
public:
	TimeEvictionStrategyTest()
		: mMockBeaconCache()
	{
	}

	void SetUp()
	{
		mMockBeaconCache = std::shared_ptr<testing::NiceMock<test::MockBeaconCache>>(new testing::NiceMock<test::MockBeaconCache>());
		mMockTimingProvider = std::shared_ptr<testing::NiceMock<test::MockTimingProvider>>(new testing::NiceMock<test::MockTimingProvider>());
	}

	void TearDown()
	{
		mMockTimingProvider = nullptr;
		mMockBeaconCache = nullptr;
	}

	std::shared_ptr<testing::NiceMock<test::MockBeaconCache>> mMockBeaconCache;
	std::shared_ptr<testing::NiceMock<test::MockTimingProvider>> mMockTimingProvider;
};

TEST_F(TimeEvictionStrategyTest, theInitialLastRunTimestampIsMinusOne)
{
	// given (use StrictMocks to verify that no mock interactions were made)
	auto mockBeaconCache = std::shared_ptr<testing::NiceMock<test::MockBeaconCache>>(new testing::NiceMock<test::MockBeaconCache>());
	auto mockTimingProvider = std::shared_ptr<testing::NiceMock<test::MockTimingProvider>>(new testing::NiceMock<test::MockTimingProvider>());
	auto configuration = std::make_shared<BeaconCacheConfiguration>(-1L, 1000L, 2000L);
	TimeEvictionStrategy target(mockBeaconCache, configuration, mockTimingProvider);

	// then
	ASSERT_EQ(target.getLastRunTimestamp(), -1L);
}

TEST_F(TimeEvictionStrategyTest, theStrategyIsDisabledIfBeaconMaxAgeIsSetToLessThanZero)
{
	// given (use StrictMocks to verify that no mock interactions were made)
	auto mockBeaconCache = std::shared_ptr<testing::StrictMock<test::MockBeaconCache>>(new testing::StrictMock<test::MockBeaconCache>());
	auto mockTimingProvider = std::shared_ptr<testing::StrictMock<test::MockTimingProvider>>(new testing::StrictMock<test::MockTimingProvider>());
	auto configuration = std::make_shared<BeaconCacheConfiguration>(-1L, 1000L, 2000L);
	TimeEvictionStrategy target(mockBeaconCache, configuration, mockTimingProvider);

	// then
	ASSERT_TRUE(target.isStrategyDisabled());
}

TEST_F(TimeEvictionStrategyTest, theStrategyIsDisabledIfBeaconMaxAgeIsSetToZero)
{
	// given (use StrictMocks to verify that no mock interactions were made)
	auto mockBeaconCache = std::shared_ptr<testing::StrictMock<test::MockBeaconCache>>(new testing::StrictMock<test::MockBeaconCache>());
	auto mockTimingProvider = std::shared_ptr<testing::StrictMock<test::MockTimingProvider>>(new testing::StrictMock<test::MockTimingProvider>());
	auto configuration = std::make_shared<BeaconCacheConfiguration>(0L, 1000L, 2000L);
	TimeEvictionStrategy target(mockBeaconCache, configuration, mockTimingProvider);

	// then
	ASSERT_TRUE(target.isStrategyDisabled());
}

TEST_F(TimeEvictionStrategyTest, theStrategyIsNotDisabledIFMaxRecordAgeIsGreaterThanZero)
{
	// given (use StrictMocks to verify that no mock interactions were made)
	auto mockBeaconCache = std::shared_ptr<testing::StrictMock<test::MockBeaconCache>>(new testing::StrictMock<test::MockBeaconCache>());
	auto mockTimingProvider = std::shared_ptr<testing::StrictMock<test::MockTimingProvider>>(new testing::StrictMock<test::MockTimingProvider>());
	auto configuration = std::make_shared<BeaconCacheConfiguration>(1L, 1000L, 2000L);
	TimeEvictionStrategy target(mockBeaconCache, configuration, mockTimingProvider);

	// then
	ASSERT_FALSE(target.isStrategyDisabled());
}

TEST_F(TimeEvictionStrategyTest, shouldRunGivesFalseIfLastRunIsLessThanMaxAgeMillisecondsAgo)
{
	// given
	auto configuration = std::make_shared<BeaconCacheConfiguration>(1000L, 1000L, 2000L);
	TimeEvictionStrategy target(mMockBeaconCache, configuration, mMockTimingProvider);

	target.setLastRunTimestamp(1000);
	ON_CALL(*mMockTimingProvider, provideTimestampInMilliseconds())
		.WillByDefault(testing::Return(target.getLastRunTimestamp() + configuration->getMaxRecordAge() - 1));

	// then
	ASSERT_FALSE(target.shouldRun());
}

TEST_F(TimeEvictionStrategyTest, shouldRunGivesTrueIfLastRunIsExactlyMaxAgeMillisecondsAgo)
{
	// given
	auto configuration = std::make_shared<BeaconCacheConfiguration>(1000L, 1000L, 2000L);
	TimeEvictionStrategy target(mMockBeaconCache, configuration, mMockTimingProvider);

	target.setLastRunTimestamp(1000);
	ON_CALL(*mMockTimingProvider, provideTimestampInMilliseconds())
		.WillByDefault(testing::Return(target.getLastRunTimestamp() + configuration->getMaxRecordAge()));

	// then
	ASSERT_TRUE(target.shouldRun());
}

TEST_F(TimeEvictionStrategyTest, shouldRunGivesTrueIfLastRunIsMoreThanMaxAgeMillisecondsAgo)
{
	// given
	auto configuration = std::make_shared<BeaconCacheConfiguration>(1000L, 1000L, 2000L);
	TimeEvictionStrategy target(mMockBeaconCache, configuration, mMockTimingProvider);

	target.setLastRunTimestamp(1000);
	ON_CALL(*mMockTimingProvider, provideTimestampInMilliseconds())
		.WillByDefault(testing::Return(target.getLastRunTimestamp() + configuration->getMaxRecordAge() + 1));

	// then
	ASSERT_TRUE(target.shouldRun());
}

TEST_F(TimeEvictionStrategyTest, executeEvictionLogsAMessageOnceAndReturnsIfStrategyIsDisabled)
{
	// TODO: Test logger interactions once the logger is implemented
}

TEST_F(TimeEvictionStrategyTest, executeEvictionDoesNotLogIfStrategyIsDisabledAndInfoIsDisabledInLogger)
{
	// TODO: Test logger interactions once the logger is implemented
}

TEST_F(TimeEvictionStrategyTest, lastRuntimeStampIsAdjustedDuringFirstExecution)
{
	// given
	auto configuration = std::make_shared<BeaconCacheConfiguration>(1000L, 1000L, 2000L);
	TimeEvictionStrategy target(mMockBeaconCache, configuration, mMockTimingProvider);

	uint32_t callCount = 0;
	ON_CALL(*mMockTimingProvider, provideTimestampInMilliseconds())
		.WillByDefault(testing::Invoke(
			[&callCount]() -> int64_t {
				// callCount 1 => 1000 for TimeEvictionStrategy::execute() (first time execution)
				// callCount 2 => 1001 for TimeEvictionStrategy::shouldRun()
				// callCount 3 => 1001 for TimeEvictionStrategy::doExecute() with no beacons
				callCount++;
				return callCount == 1 ? 1000L : 1001L;
			}
		));

	// when executing the first time
	EXPECT_CALL(*mMockTimingProvider, provideTimestampInMilliseconds())
		.Times(testing::Exactly(2));
	target.execute();

	// then
	ASSERT_EQ(target.getLastRunTimestamp(), 1000L);

	// when executing the second time
	EXPECT_CALL(*mMockTimingProvider, provideTimestampInMilliseconds())
		.Times(testing::Exactly(1));
	target.execute();

	// then
	ASSERT_EQ(target.getLastRunTimestamp(), 1000L);
}

TEST_F(TimeEvictionStrategyTest, executeEvictionStopsIfNoBeaconIdsAreAvailableInCache)
{
	// given (use StrictMocks to verify that no mock interactions were made)
	auto mockBeaconCache = std::shared_ptr<testing::StrictMock<test::MockBeaconCache>>(new testing::StrictMock<test::MockBeaconCache>());
	auto mockTimingProvider = std::shared_ptr<testing::StrictMock<test::MockTimingProvider>>(new testing::StrictMock<test::MockTimingProvider>());
	auto configuration = std::make_shared<BeaconCacheConfiguration>(1000L, 1000L, 2000L);
	TimeEvictionStrategy target(mockBeaconCache, configuration, mockTimingProvider);

	uint32_t callCount = 0;
	ON_CALL(*mockTimingProvider, provideTimestampInMilliseconds())
		.WillByDefault(testing::Invoke(
			[&callCount]() -> int64_t {
				// callCount 1 => 1000 for TimeEvictionStrategy::execute() (first time execution)
				// callCount 2 => 2000 for TimeEvictionStrategy::shouldRun()
				// callCount 3 => 2000 for TimeEvictionStrategy::doExecute() with no beacons
				callCount++;
				return callCount == 1 ? 1000L : 2000L;
			}
		));
	ON_CALL(*mockBeaconCache, getBeaconIDs())
		.WillByDefault(testing::Return(std::unordered_set<int32_t>()));

	// when executing the first time
	EXPECT_CALL(*mockBeaconCache, getBeaconIDs())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockTimingProvider, provideTimestampInMilliseconds())
		.Times(testing::Exactly(3));
	target.execute();

	// also ensure that the last run timestamp was updated
	ASSERT_EQ(target.getLastRunTimestamp(), 2000L);
}

TEST_F(TimeEvictionStrategyTest, executeEvictionCallsEvictionForEachBeaconSeparately)
{
	// given (use StrictMocks to verify that no mock interactions were made)
	auto mockBeaconCache = std::shared_ptr<testing::StrictMock<test::MockBeaconCache>>(new testing::StrictMock<test::MockBeaconCache>());
	auto mockTimingProvider = std::shared_ptr<testing::StrictMock<test::MockTimingProvider>>(new testing::StrictMock<test::MockTimingProvider>());
	auto configuration = std::make_shared<BeaconCacheConfiguration>(1000L, 1000L, 2000L);
	TimeEvictionStrategy target(mockBeaconCache, configuration, mockTimingProvider);

	uint32_t callCount = 0;
	ON_CALL(*mockTimingProvider, provideTimestampInMilliseconds())
		.WillByDefault(testing::Invoke(
			[&callCount]() -> int64_t {
				// callCount 1 => 1000 for TimeEvictionStrategy::execute() (first time execution)
				// callCount 2 => 2099 for TimeEvictionStrategy::shouldRun()
				// callCount 3 => 2099 for TimeEvictionStrategy::doExecute() with no beacons
				callCount++;
				return callCount == 1 ? 1000L : 2099L;
			}
		));
	ON_CALL(*mockBeaconCache, getBeaconIDs())
		.WillByDefault(testing::Return(std::unordered_set<int32_t>({ 1, 42 } )));

	// then verify interactions
	EXPECT_CALL(*mockBeaconCache, getBeaconIDs())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockBeaconCache, evictRecordsByAge(1, 2099L - configuration->getMaxRecordAge()))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockBeaconCache, evictRecordsByAge(42, 2099L - configuration->getMaxRecordAge()))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockTimingProvider, provideTimestampInMilliseconds())
		.Times(testing::Exactly(3));

	// when 
	target.execute();

	// also ensure that the last run timestamp was updated
	ASSERT_EQ(target.getLastRunTimestamp(), 2099L);
}

TEST_F(TimeEvictionStrategyTest, executeEvictionLogsTheNumberOfRecordsRemoved)
{
	// TODO: Test logger interactions once the logger is implemented
}

#if 0 // The code currently does not support thread isInterrupted() checking
TEST_F(TimeEvictionStrategyTest, executeEvictionIsStoppedIfThreadGetsInterrupted)
{
	// given (use StrictMocks to verify that no mock interactions were made)
	auto mockBeaconCache = std::shared_ptr<testing::StrictMock<test::MockBeaconCache>>(new testing::StrictMock<test::MockBeaconCache>());
	auto mockTimingProvider = std::shared_ptr<testing::StrictMock<test::MockTimingProvider>>(new testing::StrictMock<test::MockTimingProvider>());
	auto configuration = std::make_shared<BeaconCacheConfiguration>(1000L, 1000L, 2000L);
	TimeEvictionStrategy target(mockBeaconCache, configuration, mockTimingProvider);

	uint32_t callCount = 0;
	ON_CALL(*mockTimingProvider, provideTimestampInMilliseconds())
		.WillByDefault(testing::Invoke(
			[&callCount]() -> int64_t {
		// callCount 1 => 1000 for TimeEvictionStrategy::execute() (first time execution)
		// callCount 2 => 2099 for TimeEvictionStrategy::shouldRun()
		// callCount 3 => 2099 for TimeEvictionStrategy::doExecute() with no beacons
		callCount++;
		return callCount == 1 ? 1000L : 2099L;
	}
	));
	ON_CALL(*mockBeaconCache, getBeaconIDs())
		.WillByDefault(testing::Return(std::unordered_set<int32_t>({ 1, 42 })));

	// then verify interactions
	EXPECT_CALL(*mockBeaconCache, getBeaconIDs())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockBeaconCache, evictRecordsByAge(1, 2099L - configuration->getMaxRecordAge()))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockBeaconCache, evictRecordsByAge(42, 2099L - configuration->getMaxRecordAge()))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockTimingProvider, provideTimestampInMilliseconds())
		.Times(testing::Exactly(3));

	// when 
	target.execute();

	// also ensure that the last run timestamp was updated
	ASSERT_EQ(target.getLastRunTimestamp(), 2099L);
}
#endif