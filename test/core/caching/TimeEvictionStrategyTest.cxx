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

#include "mock/MockIBeaconCache.h"
#include "../configuration/mock/MockIBeaconCacheConfiguration.h"
#include "../../api/mock/MockILogger.h"
#include "../../providers/mock/MockITimingProvider.h"

#include "core/caching/TimeEvictionStrategy.h"
#include "core/configuration/BeaconConfiguration.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <memory>

using namespace test;

using MockNiceIBeaconCache_sp = std::shared_ptr<testing::NiceMock<MockIBeaconCache>>;
using MockNiceIBeaconCacheConfiguration_sp = std::shared_ptr<testing::NiceMock<MockIBeaconCacheConfiguration>>;
using MockNiceILogger_sp = std::shared_ptr<testing::NiceMock<MockILogger>>;
using MockNiceITimingProvider_sp = std::shared_ptr<testing::NiceMock<MockITimingProvider>>;
using MockStrictIBeaconCache_sp = std::shared_ptr<testing::StrictMock<MockIBeaconCache>>;
using MockStrictILogger_sp = std::shared_ptr<testing::StrictMock<MockILogger>>;
using MockStrictITimingProvider_sp = std::shared_ptr<testing::StrictMock<MockITimingProvider>>;
using TimeEvictionStrategy_t = core::caching::TimeEvictionStrategy;

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

class TimeEvictionStrategyTest : public testing::Test
{
protected:

	MockNiceILogger_sp mockLoggerNice;
	MockNiceIBeaconCache_sp mockBeaconCacheNice;
	MockNiceITimingProvider_sp mockTimingProviderNice;

	MockStrictILogger_sp mockLoggerStrict;
	MockStrictIBeaconCache_sp mockBeaconCacheStrict;
	MockStrictITimingProvider_sp mockTimingProviderStrict;

	void SetUp() override
	{
		mockLoggerNice = MockILogger::createNice();
		mockBeaconCacheNice = MockIBeaconCache::createNice();
		mockTimingProviderNice = MockITimingProvider::createNice();

		mockLoggerStrict = MockILogger::createStrict();
		mockBeaconCacheStrict = MockIBeaconCache::createStrict();
		mockTimingProviderStrict = MockITimingProvider::createStrict();
	}

	MockNiceIBeaconCacheConfiguration_sp createBeaconCacheConfig(int32_t maxAge, int32_t lowerBound, int32_t upperBound)
	{
		auto config = MockIBeaconCacheConfiguration::createNice();
		ON_CALL(*config, getMaxRecordAge())
			.WillByDefault(testing::Return(maxAge));
		ON_CALL(*config, getCacheSizeLowerBound())
			.WillByDefault(testing::Return(lowerBound));
		ON_CALL(*config, getCacheSizeUpperBound())
			.WillByDefault(testing::Return(upperBound));

		return config;
	}

public:

	bool mockedIsAliveFunctionAlwaysTrue()
	{
		return true;
	}

};

TEST_F(TimeEvictionStrategyTest, theInitialLastRunTimestampIsMinusOne)
{
	// given
	auto configuration = createBeaconCacheConfig(-1L, 1000L, 2000L);
	TimeEvictionStrategy_t target(
		mockLoggerNice,
		mockBeaconCacheNice,
		configuration,
		mockTimingProviderNice,
		std::bind(&TimeEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this)
	);

	// then
	ASSERT_EQ(target.getLastRunTimestamp(), -1L);
}

TEST_F(TimeEvictionStrategyTest, theStrategyIsDisabledIfBeaconMaxAgeIsSetToLessThanZero)
{
	// given
	auto configuration = createBeaconCacheConfig(-1L, 1000L, 2000L);
	TimeEvictionStrategy_t target(
		mockLoggerStrict,
		mockBeaconCacheStrict,
		configuration,
		mockTimingProviderStrict,
		std::bind(&TimeEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this)
	);

	// then
	ASSERT_TRUE(target.isStrategyDisabled());
}

TEST_F(TimeEvictionStrategyTest, theStrategyIsDisabledIfBeaconMaxAgeIsSetToZero)
{
	// given (use StrictMocks to verify that no mock interactions were made)
	auto configuration = createBeaconCacheConfig(0L, 1000L, 2000L);
	TimeEvictionStrategy_t target(
		mockLoggerStrict,
		mockBeaconCacheStrict,
		configuration,
		mockTimingProviderStrict,
		std::bind(&TimeEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this)
	);

	// then
	ASSERT_TRUE(target.isStrategyDisabled());
}

TEST_F(TimeEvictionStrategyTest, theStrategyIsNotDisabledIFMaxRecordAgeIsGreaterThanZero)
{
	// given
	auto configuration = createBeaconCacheConfig(1L, 1000L, 2000L);
	TimeEvictionStrategy_t target(
		mockLoggerStrict,
		mockBeaconCacheStrict,
		configuration,
		mockTimingProviderStrict,
		std::bind(&TimeEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this)
	);

	// then
	ASSERT_FALSE(target.isStrategyDisabled());
}

TEST_F(TimeEvictionStrategyTest, shouldRunGivesFalseIfLastRunIsLessThanMaxAgeMillisecondsAgo)
{
	// given
	auto configuration = createBeaconCacheConfig(1000L, 1000L, 2000L);
	TimeEvictionStrategy_t target(
		mockLoggerNice,
		mockBeaconCacheNice,
		configuration,
		mockTimingProviderNice,
		std::bind(&TimeEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this)
	);

	target.setLastRunTimestamp(1000);
	ON_CALL(*mockTimingProviderNice, provideTimestampInMilliseconds())
		.WillByDefault(testing::Return(target.getLastRunTimestamp() + configuration->getMaxRecordAge() - 1));

	// then
	ASSERT_FALSE(target.shouldRun());
}

TEST_F(TimeEvictionStrategyTest, shouldRunGivesTrueIfLastRunIsExactlyMaxAgeMillisecondsAgo)
{
	// given
	auto configuration = createBeaconCacheConfig(1000L, 1000L, 2000L);
	TimeEvictionStrategy_t target(
		mockLoggerNice,
		mockBeaconCacheNice,
		configuration,
		mockTimingProviderNice,
		std::bind(&TimeEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this)
	);

	target.setLastRunTimestamp(1000);
	ON_CALL(*mockTimingProviderNice, provideTimestampInMilliseconds())
		.WillByDefault(testing::Return(target.getLastRunTimestamp() + configuration->getMaxRecordAge()));

	// then
	ASSERT_TRUE(target.shouldRun());
}

TEST_F(TimeEvictionStrategyTest, shouldRunGivesTrueIfLastRunIsMoreThanMaxAgeMillisecondsAgo)
{
	// given
	auto configuration = createBeaconCacheConfig(1000L, 1000L, 2000L);
	TimeEvictionStrategy_t target(
		mockLoggerNice,
		mockBeaconCacheNice,
		configuration,
		mockTimingProviderNice,
		std::bind(&TimeEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this)
	);

	target.setLastRunTimestamp(1000);
	ON_CALL(*mockTimingProviderNice, provideTimestampInMilliseconds())
		.WillByDefault(testing::Return(target.getLastRunTimestamp() + configuration->getMaxRecordAge() + 1));

	// then
	ASSERT_TRUE(target.shouldRun());
}

TEST_F(TimeEvictionStrategyTest, executeEvictionLogsAMessageOnceAndReturnsIfStrategyIsDisabled)
{
	// expect
	EXPECT_CALL(*mockLoggerStrict, isInfoEnabled())
		.Times(1);
	EXPECT_CALL(*mockLoggerStrict, mockInfo("TimeEvictionStrategy execute() - strategy is disabled"))
		.Times(1);

	// given
	auto configuration = createBeaconCacheConfig(0L, 1000L, 2000L);
	TimeEvictionStrategy_t target(
		mockLoggerStrict,
		mockBeaconCacheStrict,
		configuration,
		mockTimingProviderStrict,
		std::bind(&TimeEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this)
	);

	// when executing 2 times
	target.execute();
	target.execute();
}

TEST_F(TimeEvictionStrategyTest, executeEvictionDoesNotLogIfStrategyIsDisabledAndInfoIsDisabledInLogger)
{
	// with
	ON_CALL(*mockLoggerStrict, isInfoEnabled())
		.WillByDefault(testing::Return(false));

	// given
	auto configuration = createBeaconCacheConfig(0L, 1000L, 2000L);
	TimeEvictionStrategy_t target(
		mockLoggerStrict,
		mockBeaconCacheStrict,
		configuration,
		mockTimingProviderStrict,
		std::bind(&TimeEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this)
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

TEST_F(TimeEvictionStrategyTest, lastRuntimeStampIsAdjustedDuringFirstExecution)
{
	// given
	auto configuration = createBeaconCacheConfig(1000L, 1000L, 2000L);
	TimeEvictionStrategy_t target(
		mockLoggerNice,
		mockBeaconCacheNice,
		configuration,
		mockTimingProviderNice,
		std::bind(&TimeEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this)
	);

	// expect
	EXPECT_CALL(*mockTimingProviderNice, provideTimestampInMilliseconds())
		.WillOnce(testing::Return(1000L))		// 1000 for TimeEvictionStrategy::execute() (first time execution)
		.WillOnce(testing::Return(1001L));		// 1001 for TimeEvictionStrategy::shouldRun()
	target.execute();

	// then
	ASSERT_EQ(target.getLastRunTimestamp(), 1000L);

	// when executing the second time
	EXPECT_CALL(*mockTimingProviderNice, provideTimestampInMilliseconds())
		.WillOnce(testing::Return(1001L));		// 1001 for TimeEvictionStrategy::doExecute() with no beacons
	target.execute();

	// then
	ASSERT_EQ(target.getLastRunTimestamp(), 1000L);
}

TEST_F(TimeEvictionStrategyTest, executeEvictionStopsIfNoBeaconIdsAreAvailableInCache)
{
	// given
	auto configuration = createBeaconCacheConfig(1000L, 1000L, 2000L);
	TimeEvictionStrategy_t target(
		mockLoggerNice,
		mockBeaconCacheStrict,
		configuration,
		mockTimingProviderStrict,
		std::bind(&TimeEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this)
	);
	ON_CALL(*mockBeaconCacheStrict, getBeaconIDs())
		.WillByDefault(testing::Return(std::unordered_set<int32_t>()));

	// when executing the first time
	EXPECT_CALL(*mockBeaconCacheStrict, getBeaconIDs())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockTimingProviderStrict, provideTimestampInMilliseconds())
		.WillOnce(testing::Return(1000L))		// 1000 for TimeEvictionStrategy::execute() (first time execution)
		.WillOnce(testing::Return(2000L))		// 2000 for TimeEvictionStrategy::shouldRun()
		.WillOnce(testing::Return(2000L));		// 2000 for inner while loop in SpaceEvictionStrategy::doExecute() which evicts beaconID 1
	target.execute();

	// also ensure that the last run timestamp was updated
	ASSERT_EQ(target.getLastRunTimestamp(), 2000L);
}

TEST_F(TimeEvictionStrategyTest, executeEvictionCallsEvictionForEachBeaconSeparately)
{
	// given
	auto configuration = createBeaconCacheConfig(1000L, 1000L, 2000L);
	TimeEvictionStrategy_t target(
		mockLoggerNice,
		mockBeaconCacheStrict,
		configuration,
		mockTimingProviderStrict,
		std::bind(&TimeEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this)
	);
	ON_CALL(*mockBeaconCacheStrict, getBeaconIDs())
		.WillByDefault(testing::Return(std::unordered_set<int32_t>({ 1, 42 } )));

	// then verify interactions
	EXPECT_CALL(*mockBeaconCacheStrict, getBeaconIDs())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockBeaconCacheStrict, evictRecordsByAge(1, 2099L - configuration->getMaxRecordAge()))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockBeaconCacheStrict, evictRecordsByAge(42, 2099L - configuration->getMaxRecordAge()))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockTimingProviderStrict, provideTimestampInMilliseconds())
		.WillOnce(testing::Return(1000L))		// 1000 for TimeEvictionStrategy::execute() (first time execution)
		.WillOnce(testing::Return(2099L))		// 2099 for TimeEvictionStrategy::shouldRun()
		.WillOnce(testing::Return(2099L));		// 2099 for TimeEvictionStrategy::doExecute() with no beacons

	// when
	target.execute();

	// also ensure that the last run timestamp was updated
	ASSERT_EQ(target.getLastRunTimestamp(), 2099L);
}

TEST_F(TimeEvictionStrategyTest, executeEvictionLogsTheNumberOfRecordsRemoved)
{
	// expect
	EXPECT_CALL(*mockLoggerStrict, isDebugEnabled())
		.Times(2);
	EXPECT_CALL(*mockLoggerStrict, mockDebug("TimeEvictionStrategy doExecute() - Removed 2 records from Beacon with ID 1"))
		.Times(1);
	EXPECT_CALL(*mockLoggerStrict, mockDebug("TimeEvictionStrategy doExecute() - Removed 5 records from Beacon with ID 42"))
		.Times(1);

	// given
	auto configuration = createBeaconCacheConfig(1000L, 1000L, 2000L);
	TimeEvictionStrategy_t target(
		mockLoggerStrict,
		mockBeaconCacheNice,
		configuration,
		mockTimingProviderNice,
		std::bind(&TimeEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this)
	);

	EXPECT_CALL(*mockTimingProviderNice, provideTimestampInMilliseconds())
		.WillOnce(testing::Return(1000L))		// 1000 for TimeEvictionStrategy::execute() (first time execution)
		.WillOnce(testing::Return(2099L))		// 2099 for TimeEvictionStrategy::shouldRun()
		.WillOnce(testing::Return(2099L));		// 2099 for TimeEvictionStrategy::doExecute() with no beacons
	ON_CALL(*mockBeaconCacheNice, getBeaconIDs())
		.WillByDefault(testing::Return(std::unordered_set<int32_t>({ 1, 42 })));
	ON_CALL(*mockBeaconCacheNice, evictRecordsByAge(1, testing::_))
		.WillByDefault(testing::Return(2));
	ON_CALL(*mockBeaconCacheNice, evictRecordsByAge(42, testing::_))
		.WillByDefault(testing::Return(5));

	// when executing
	target.execute();
}

TEST_F(TimeEvictionStrategyTest, executeEvictionIsStoppedIfThreadGetsInterrupted)
{
	// given
	auto configuration = createBeaconCacheConfig(1000L, 1000L, 2000L);
	auto mockIsAlive = std::make_shared<testing::NiceMock<MockIsAlive>>();
	TimeEvictionStrategy_t target(
		mockLoggerNice,
		mockBeaconCacheStrict,
		configuration,
		mockTimingProviderStrict,
		std::bind(&MockIsAlive::isAlive, mockIsAlive)
	);

	uint32_t callCountIsAlive = 0;
	ON_CALL(*mockIsAlive, isAlive())
		.WillByDefault(testing::Invoke(
			[&callCountIsAlive]() -> bool {
				// isAlive shall return "false" after the 1st call
				callCountIsAlive++;
				return callCountIsAlive == 1;
			}
		));
	ON_CALL(*mockBeaconCacheStrict, getBeaconIDs())
		.WillByDefault(testing::Return(std::unordered_set<int32_t>({ 1, 42 })));

	// then verify interactions
	EXPECT_CALL(*mockBeaconCacheStrict, getBeaconIDs())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockBeaconCacheStrict, evictRecordsByAge(testing::_, 2099L - configuration->getMaxRecordAge()))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockTimingProviderStrict, provideTimestampInMilliseconds())
		.WillOnce(testing::Return(1000L))		// 1000 for TimeEvictionStrategy::execute() (first time execution)
		.WillOnce(testing::Return(2099L))		// 2099 for TimeEvictionStrategy::shouldRun()
		.WillOnce(testing::Return(2099L));		// 2099 for TimeEvictionStrategy::doExecute() with no beacons
	// when
	target.execute();
}
