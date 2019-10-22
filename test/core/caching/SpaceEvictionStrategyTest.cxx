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
#include "../../api/mock/MockILogger.h"

#include "core/caching/SpaceEvictionStrategy.h"
#include "core/configuration/BeaconConfiguration.h"


#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <memory>

using namespace test;

using BeaconCacheConfiguration_t = core::configuration::BeaconCacheConfiguration;
using MockNiceIBeaconCache_sp = std::shared_ptr<testing::NiceMock<MockIBeaconCache>>;
using MockNiceILogger_sp = std::shared_ptr<testing::NiceMock<MockILogger>>;
using MockStrictILogger_sp = std::shared_ptr<testing::StrictMock<MockILogger>>;
using SpaceEvictionStrategy_t = core::caching::SpaceEvictionStrategy;

// To mock a change in the isAlive() method we need to create a dummy mock class holding the mocked isAlive method.
class MockIsAlive
{
public:
	MockIsAlive()
	{
	}

	virtual ~MockIsAlive() {}

	MOCK_METHOD0(isAlive, bool());
};

class SpaceEvictionStrategyTest : public testing::Test
{
protected:

	MockNiceILogger_sp mockLoggerNice;
	MockNiceIBeaconCache_sp mockBeaconCache;

	MockStrictILogger_sp mockLoggerStrict;

	void SetUp()
	{
		mockLoggerNice = MockILogger::createNice();
		mockBeaconCache = MockIBeaconCache::createNice();

		mockLoggerStrict = MockILogger::createStrict();
	}

public:

	bool mockedIsAliveFunctionAlwaysTrue()
	{
		return true;
	}
};

TEST_F(SpaceEvictionStrategyTest, theStrategyIsDisabledIfCacheSizeLowerBoundIsEqualToZero)
{
	// given
	auto configuration = std::make_shared<BeaconCacheConfiguration_t>(1000L, 0L, 2000L);
	SpaceEvictionStrategy_t target(
		mockLoggerNice,
		mockBeaconCache,
		configuration,
		std::bind(&SpaceEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this)
	);

	// then
	ASSERT_TRUE(target.isStrategyDisabled());
}

TEST_F(SpaceEvictionStrategyTest, theStrategyIsDisabledIfCacheSizeLowerBoundIsLessThanZero)
{
	// given
	auto configuration = std::make_shared<BeaconCacheConfiguration_t>(1000L, -1L, 2000L);
	SpaceEvictionStrategy_t target(
		mockLoggerNice,
		mockBeaconCache,
		configuration,
		std::bind(&SpaceEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this)
	);

	// then
	ASSERT_TRUE(target.isStrategyDisabled());
}

TEST_F(SpaceEvictionStrategyTest, theStrategyIsDisabledIfCacheSizeUpperBoundIsEqualToZero)
{
	// given
	auto configuration = std::make_shared<BeaconCacheConfiguration_t>(1000L, 1000L, 0L);
	SpaceEvictionStrategy_t target(
		mockLoggerNice,
		mockBeaconCache,
		configuration,
		std::bind(&SpaceEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this)
	);

	// then
	ASSERT_TRUE(target.isStrategyDisabled());
}

TEST_F(SpaceEvictionStrategyTest, theStrategyIsDisabledIfCacheSizeUpperBoundIsLessThanLowerBound)
{
	// given
	auto configuration = std::make_shared<BeaconCacheConfiguration_t>(1000L, 1000L, 999L);
	SpaceEvictionStrategy_t target(
		mockLoggerNice,
		mockBeaconCache,
		configuration,
		std::bind(&SpaceEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this)
	);

	// then
	ASSERT_TRUE(target.isStrategyDisabled());
}

TEST_F(SpaceEvictionStrategyTest, shouldRunGivesTrueIfNumBytesInCacheIsGreaterThanUpperBoundLimit)
{
	// given
	auto configuration = std::make_shared<BeaconCacheConfiguration_t>(1000L, 1000L, 2000L);
	SpaceEvictionStrategy_t target(
		mockLoggerNice,
		mockBeaconCache,
		configuration,
		std::bind(&SpaceEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this)
	);

	//when
	ON_CALL(*mockBeaconCache, getNumBytesInCache())
		.WillByDefault(testing::Return(configuration->getCacheSizeUpperBound() + 1));

	// then
	ASSERT_TRUE(target.shouldRun());
}

TEST_F(SpaceEvictionStrategyTest, shouldRunGivesFalseIfNumBytesInCacheIsEqualToUpperBoundLimit)
{
	// given
	auto configuration = std::make_shared<BeaconCacheConfiguration_t>(1000L, 1000L, 2000L);
	SpaceEvictionStrategy_t target(
		mockLoggerNice,
		mockBeaconCache,
		configuration,
		std::bind(&SpaceEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this)
	);

	//when
	ON_CALL(*mockBeaconCache, getNumBytesInCache())
		.WillByDefault(testing::Return(configuration->getCacheSizeUpperBound()));

	// then
	ASSERT_FALSE(target.shouldRun());
}

TEST_F(SpaceEvictionStrategyTest, shouldRunGivesFalseIfNumBytesInCacheIsLessThanUpperBoundLimit)
{
	// given
	auto configuration = std::make_shared<BeaconCacheConfiguration_t>(1000L, 1000L, 2000L);
	SpaceEvictionStrategy_t target(
		mockLoggerNice,
		mockBeaconCache,
		configuration,
		std::bind(&SpaceEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this)
	);

	//when
	ON_CALL(*mockBeaconCache, getNumBytesInCache())
		.WillByDefault(testing::Return(configuration->getCacheSizeUpperBound() - 1));

	// then
	ASSERT_FALSE(target.shouldRun());
}

TEST_F(SpaceEvictionStrategyTest, executeEvictionLogsAMessageOnceAndReturnsIfStrategyIsDisabled)
{
	// expect
	EXPECT_CALL(*mockLoggerStrict, isInfoEnabled())
		.Times(1);
	EXPECT_CALL(*mockLoggerStrict, mockInfo("SpaceEvictionStrategy execute() - strategy is disabled"))
		.Times(1);

	// given
	auto configuration = std::make_shared<BeaconCacheConfiguration_t>(1000L, 1000L, -1L);
	SpaceEvictionStrategy_t target(
		mockLoggerStrict,
		mockBeaconCache,
		configuration, std::bind(&SpaceEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this)
	);

	// when executing 2 times
	target.execute();
	target.execute();
}

TEST_F(SpaceEvictionStrategyTest, executeEvictionDoesNotLogIfStrategyIsDisabledAndInfoIsDisabledInLogger)
{
	// with
	ON_CALL(*mockLoggerStrict, isInfoEnabled())
		.WillByDefault(testing::Return(false));

	// given
	auto configuration = std::make_shared<BeaconCacheConfiguration_t>(1000L, 1000L, -1L);
	SpaceEvictionStrategy_t target(
		mockLoggerStrict,
		mockBeaconCache,
		configuration,
		std::bind(&SpaceEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this)
	);

	// expect
	EXPECT_CALL(*mockLoggerStrict, isInfoEnabled())
		.Times(1);

	// when
	target.execute();

	// and expect
	EXPECT_CALL(*mockLoggerStrict, isInfoEnabled())
			.Times(1);

	// when
	target.execute();
}

TEST_F(SpaceEvictionStrategyTest, executeEvictionCallsCacheMethodForEachBeacon)
{
	// given
	auto configuration = std::make_shared<BeaconCacheConfiguration_t>(1000L, 1000L, 2000L);
	SpaceEvictionStrategy_t target(
		mockLoggerNice,
		mockBeaconCache,
		configuration,
		std::bind(&SpaceEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this)
	);
	ON_CALL(*mockBeaconCache, getBeaconIDs())
		.WillByDefault(testing::Return(std::unordered_set<int32_t>({ 1, 42 })));

	// then
	EXPECT_CALL(*mockBeaconCache, getNumBytesInCache())
		.WillOnce(testing::Return(2001L))		// 2001 for SpaceEvictionStrategy::shouldRun()
		.WillOnce(testing::Return(2001L))		// 2001 for outer while loop in SpaceEvictionStrategy::doExecute()
		.WillOnce(testing::Return(2001L))		// 2001 for inner while loop in SpaceEvictionStrategy::doExecute() which evicts beaconID 1
		.WillOnce(testing::Return(2001L))		// 2001 for inner while loop in SpaceEvictionStrategy::doExecute() which evicts beaconID 42
		.WillOnce(testing::Return(0L));			// 0 for outer while loop in SpaceEvictionStrategy::doExecute() (to exit the while loop)
	EXPECT_CALL(*mockBeaconCache, evictRecordsByNumber(1, 1))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockBeaconCache, evictRecordsByNumber(42, 1))
		.Times(testing::Exactly(1));

	// when
	target.execute();
}

TEST_F(SpaceEvictionStrategyTest, executeEvictionLogsEvictionResultIfDebugIsEnabled)
{
	// expect
	EXPECT_CALL(*mockLoggerStrict, isDebugEnabled())
		.Times(3);
	EXPECT_CALL(*mockLoggerStrict, mockDebug("SpaceEvictionStrategy doExecute() - Removed 5 records from Beacon with ID 1"))
		.Times(1);
	EXPECT_CALL(*mockLoggerStrict, mockDebug("SpaceEvictionStrategy doExecute() - Removed 1 records from Beacon with ID 42"))
		.Times(1);

	// given
	auto configuration = std::make_shared<BeaconCacheConfiguration_t>(1000L, 1000L, 2000L);
	SpaceEvictionStrategy_t target(
		mockLoggerStrict,
		mockBeaconCache,
		configuration,
		std::bind(&SpaceEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this)
	);

	EXPECT_CALL(*mockBeaconCache, getNumBytesInCache())
		.WillOnce(testing::Return(2001L))		// 2001 for SpaceEvictionStrategy::shouldRun()
		.WillOnce(testing::Return(2001L))		// 2001 for outer while loop in SpaceEvictionStrategy::doExecute()
		.WillOnce(testing::Return(2001L))		// 2001 for inner while loop in SpaceEvictionStrategy::doExecute() which evicts beaconID 1
		.WillOnce(testing::Return(2001L))		// 2001 for inner while loop in SpaceEvictionStrategy::doExecute() which evicts beaconID 42
		.WillOnce(testing::Return(0L));			// 0 for outer while loop in SpaceEvictionStrategy::doExecute() (to exit the while loop)
	ON_CALL(*mockBeaconCache, getBeaconIDs())
		.WillByDefault(testing::Return(std::unordered_set<int32_t>({ 1, 42 })));
	ON_CALL(*mockBeaconCache, evictRecordsByNumber(1, testing::_))
		.WillByDefault(testing::Return(5));
	ON_CALL(*mockBeaconCache, evictRecordsByNumber(42, testing::_))
		.WillByDefault(testing::Return(1));

	// when executing
	target.execute();
}

TEST_F(SpaceEvictionStrategyTest, executeEvictionDoesNotLogEvictionResultIfDebugIsDisabled)
{
	// with
	ON_CALL(*mockLoggerStrict, isDebugEnabled())
		.WillByDefault(testing::Return(false));

	// expect
	EXPECT_CALL(*mockLoggerStrict, isDebugEnabled())
		.Times(3);

	// given
	auto configuration = std::make_shared<BeaconCacheConfiguration_t>(1000L, 1000L, 2000L);
	SpaceEvictionStrategy_t target(
		mockLoggerStrict,
		mockBeaconCache,
		configuration,
		std::bind(&SpaceEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this)
	);

	EXPECT_CALL(*mockBeaconCache, getNumBytesInCache())
		.WillOnce(testing::Return(2001L))		// 2001 for SpaceEvictionStrategy::shouldRun()
		.WillOnce(testing::Return(2001L))		// 2001 for outer while loop in SpaceEvictionStrategy::doExecute()
		.WillOnce(testing::Return(2001L))		// 2001 for inner while loop in SpaceEvictionStrategy::doExecute() which evicts beaconID 1
		.WillOnce(testing::Return(2001L))		// 2001 for inner while loop in SpaceEvictionStrategy::doExecute() which evicts beaconID 42
		.WillOnce(testing::Return(0L));			// 0 for outer while loop in SpaceEvictionStrategy::doExecute() (to exit the while loop)
	ON_CALL(*mockBeaconCache, getBeaconIDs())
		.WillByDefault(testing::Return(std::unordered_set<int32_t>({ 1, 42 })));
	ON_CALL(*mockBeaconCache, evictRecordsByNumber(1, testing::_))
		.WillByDefault(testing::Return(5));
	ON_CALL(*mockBeaconCache, evictRecordsByNumber(42, testing::_))
		.WillByDefault(testing::Return(1));

	// when executing
	target.execute();
}

TEST_F(SpaceEvictionStrategyTest, executeEvictionRunsUntilTheCacheSizeIsLessThanOrEqualToLowerBound)
{
	// given
	auto configuration = std::make_shared<BeaconCacheConfiguration_t>(1000L, 1000L, 2000L);
	SpaceEvictionStrategy_t target(
		mockLoggerNice,
		mockBeaconCache,
		configuration,
		std::bind(&SpaceEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this)
	);

	ON_CALL(*mockBeaconCache, getBeaconIDs())
		.WillByDefault(testing::Return(std::unordered_set<int32_t>({ 1, 42 })));

	// then
	EXPECT_CALL(*mockBeaconCache, getNumBytesInCache())
		.WillOnce(testing::Return(2001L))		// 2001 for SpaceEvictionStrategy::shouldRun()
		.WillOnce(testing::Return(2000L))		// 2000 for outer while loop in SpaceEvictionStrategy::doExecute()
		.WillOnce(testing::Return(2000L))		// 2000 for inner while loop in SpaceEvictionStrategy::doExecute() which evicts beaconID 1
		.WillOnce(testing::Return(2000L))		// 2000 for inner while loop in SpaceEvictionStrategy::doExecute() which evicts beaconID 42
		.WillOnce(testing::Return(1500L))		// 1500 for outer while loop (second iteration) in SpaceEvictionStrategy::doExecute()
		.WillOnce(testing::Return(1500L))		// 1500 for inner while loop in SpaceEvictionStrategy::doExecute() which evicts beaconID 1
		.WillOnce(testing::Return(1500L))		// 1500 for inner while loop in SpaceEvictionStrategy::doExecute() which evicts beaconID 42
		.WillOnce(testing::Return(1000L))		// 1000 for outer while loop in SpaceEvictionStrategy::doExecute() (to exit the while loop)
		.WillRepeatedly(testing::Return(0L));	// just for safety
	EXPECT_CALL(*mockBeaconCache, evictRecordsByNumber(1, 1))
		.Times(testing::Exactly(2));
	EXPECT_CALL(*mockBeaconCache, evictRecordsByNumber(42, 1))
		.Times(testing::Exactly(2));

	// when
	target.execute();
}

TEST_F(SpaceEvictionStrategyTest, executeEvictionStopsIfThreadGetsInterruptedBetweenTwoBeacons)
{
	// given
	auto configuration = std::make_shared<BeaconCacheConfiguration_t>(1000L, 1000L, 2000L);
	auto mockIsAlive = std::make_shared<testing::NiceMock<MockIsAlive>>();
	SpaceEvictionStrategy_t target(
		mockLoggerNice,
		mockBeaconCache,
		configuration,
		std::bind(&MockIsAlive::isAlive, mockIsAlive)
	);

	uint32_t callCountIsAlive = 0;
	ON_CALL(*mockIsAlive, isAlive())
		.WillByDefault(testing::Invoke(
			[&callCountIsAlive]() -> bool {
		// isAlive shall return "false" after the 2nd call
		callCountIsAlive++;
		return callCountIsAlive <= 2;
	}
	));
	ON_CALL(*mockBeaconCache, getBeaconIDs())
		.WillByDefault(testing::Return(std::unordered_set<int32_t>({ 1, 42 })));

	// then
	EXPECT_CALL(*mockBeaconCache, getNumBytesInCache())
		.WillOnce(testing::Return(2001L))		// 2001 for SpaceEvictionStrategy::shouldRun()
		.WillOnce(testing::Return(2000L))		// 2000 for outer while loop in SpaceEvictionStrategy::doExecute()
		.WillOnce(testing::Return(2000L))		// 2000 for inner while loop in SpaceEvictionStrategy::doExecute() which evicts beaconID 1
		.WillRepeatedly(testing::Return(0L));	// just for safety
	EXPECT_CALL(*mockBeaconCache, evictRecordsByNumber(testing::_, 1))
		.Times(testing::Exactly(1));

	// when
	target.execute();
}

TEST_F(SpaceEvictionStrategyTest, executeEvictionStopsIfNumBytesInCacheFallsBelowLowerBoundBetweenTwoBeacons)
{
	// given
	auto configuration = std::make_shared<BeaconCacheConfiguration_t>(1000L, 1000L, 2000L);
	SpaceEvictionStrategy_t target(
		mockLoggerNice,
		mockBeaconCache,
		configuration,
		std::bind(&SpaceEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this)
	);
	ON_CALL(*mockBeaconCache, getBeaconIDs())
		.WillByDefault(testing::Return(std::unordered_set<int32_t>({ 1, 42 })));

	// then
	EXPECT_CALL(*mockBeaconCache, getNumBytesInCache())
		.WillOnce(testing::Return(2001L))		// 2001 for SpaceEvictionStrategy::shouldRun()
		.WillOnce(testing::Return(2000L))		// 2000 for outer while loop in SpaceEvictionStrategy::doExecute()
		.WillOnce(testing::Return(2000L))		// 2000 for inner while loop in SpaceEvictionStrategy::doExecute() which evicts beaconID 1
		.WillOnce(testing::Return(2000L))		// 2000 for inner while loop in SpaceEvictionStrategy::doExecute() which evicts beaconID 42
		.WillOnce(testing::Return(1500L))		// 1500 for outer while loop (second iteration) in SpaceEvictionStrategy::doExecute()
		.WillOnce(testing::Return(1500L))		// 1500 for inner while loop in SpaceEvictionStrategy::doExecute() which evicts beaconID 1
		.WillOnce(testing::Return(1000L))		// 1000 for outer while loop in SpaceEvictionStrategy::doExecute() (to exit the while loop)
		.WillRepeatedly(testing::Return(0L));	// just for safety
	EXPECT_CALL(*mockBeaconCache, evictRecordsByNumber(testing::An<int32_t>(), 1))
		.Times(testing::Exactly(3));

	// when
	target.execute();
}