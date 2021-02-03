/**
 * Copyright 2018-2021 Dynatrace LLC
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

#include "core/caching/TimeEvictionStrategy.h"
#include "core/caching/BeaconKey.h"
#include "core/configuration/BeaconConfiguration.h"

#include "mock/MockIBeaconCache.h"
#include "../configuration/mock/MockIBeaconCacheConfiguration.h"
#include "../../api/mock/MockILogger.h"
#include "../../providers/mock/MockITimingProvider.h"

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
using BeaconKey_t = core::caching::BeaconKey;
using BeaconKeySet_t = std::unordered_set<BeaconKey_t, BeaconKey_t::Hash>;

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

	bool mockedIsStopRequestedFunctionAlwaysFalse()
	{
		return false;
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
		std::bind(&TimeEvictionStrategyTest::mockedIsStopRequestedFunctionAlwaysFalse, this)
	);

	// then
	ASSERT_THAT(target.getLastRunTimestamp(), testing::Eq(-1L));
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
		std::bind(&TimeEvictionStrategyTest::mockedIsStopRequestedFunctionAlwaysFalse, this)
	);

	// then
	ASSERT_THAT(target.isStrategyDisabled(), testing::Eq(true));
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
		std::bind(&TimeEvictionStrategyTest::mockedIsStopRequestedFunctionAlwaysFalse, this)
	);

	// then
	ASSERT_THAT(target.isStrategyDisabled(), testing::Eq(true));
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
		std::bind(&TimeEvictionStrategyTest::mockedIsStopRequestedFunctionAlwaysFalse, this)
	);

	// then
	ASSERT_THAT(target.isStrategyDisabled(), testing::Eq(false));
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
		std::bind(&TimeEvictionStrategyTest::mockedIsStopRequestedFunctionAlwaysFalse, this)
	);

	target.setLastRunTimestamp(1000);
	ON_CALL(*mockTimingProviderNice, provideTimestampInMilliseconds())
		.WillByDefault(testing::Return(target.getLastRunTimestamp() + configuration->getMaxRecordAge() - 1));

	// then
	ASSERT_THAT(target.shouldRun(), testing::Eq(false));
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
		std::bind(&TimeEvictionStrategyTest::mockedIsStopRequestedFunctionAlwaysFalse, this)
	);

	target.setLastRunTimestamp(1000);
	ON_CALL(*mockTimingProviderNice, provideTimestampInMilliseconds())
		.WillByDefault(testing::Return(target.getLastRunTimestamp() + configuration->getMaxRecordAge()));

	// then
	ASSERT_THAT(target.shouldRun(), testing::Eq(true));
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
		std::bind(&TimeEvictionStrategyTest::mockedIsStopRequestedFunctionAlwaysFalse, this)
	);

	target.setLastRunTimestamp(1000);
	ON_CALL(*mockTimingProviderNice, provideTimestampInMilliseconds())
		.WillByDefault(testing::Return(target.getLastRunTimestamp() + configuration->getMaxRecordAge() + 1));

	// then
	ASSERT_THAT(target.shouldRun(), testing::Eq(true));
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
		std::bind(&TimeEvictionStrategyTest::mockedIsStopRequestedFunctionAlwaysFalse, this)
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

	// expect
	EXPECT_CALL(*mockLoggerStrict, isInfoEnabled())
		.Times(1);

	// given
	auto configuration = createBeaconCacheConfig(0L, 1000L, 2000L);
	TimeEvictionStrategy_t target(
		mockLoggerStrict,
		mockBeaconCacheStrict,
		configuration,
		mockTimingProviderStrict,
		std::bind(&TimeEvictionStrategyTest::mockedIsStopRequestedFunctionAlwaysFalse, this)
	);

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
		std::bind(&TimeEvictionStrategyTest::mockedIsStopRequestedFunctionAlwaysFalse, this)
	);

	// expect
	EXPECT_CALL(*mockTimingProviderNice, provideTimestampInMilliseconds())
		.WillOnce(testing::Return(1000L))		// 1000 for TimeEvictionStrategy::execute() (first time execution)
		.WillOnce(testing::Return(1001L));		// 1001 for TimeEvictionStrategy::shouldRun()
	target.execute();

	// then
	ASSERT_THAT(target.getLastRunTimestamp(), testing::Eq(1000L));

	// when executing the second time
	EXPECT_CALL(*mockTimingProviderNice, provideTimestampInMilliseconds())
		.WillOnce(testing::Return(1001L));		// 1001 for TimeEvictionStrategy::doExecute() with no beacons
	target.execute();

	// then
	ASSERT_THAT(target.getLastRunTimestamp(), testing::Eq(1000L));
}

TEST_F(TimeEvictionStrategyTest, executeEvictionStopsIfNoBeaconIdsAreAvailableInCache)
{
	// expect
	EXPECT_CALL(*mockBeaconCacheStrict, getBeaconKeys())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockTimingProviderStrict, provideTimestampInMilliseconds())
		.WillOnce(testing::Return(1000L))		// 1000 for TimeEvictionStrategy::execute() (first time execution)
		.WillOnce(testing::Return(2000L))		// 2000 for TimeEvictionStrategy::shouldRun()
		.WillOnce(testing::Return(2000L));		// 2000 for inner while loop in SpaceEvictionStrategy::doExecute() which evicts beaconID 1

	// given
	auto configuration = createBeaconCacheConfig(1000L, 1000L, 2000L);
	TimeEvictionStrategy_t target(
		mockLoggerNice,
		mockBeaconCacheStrict,
		configuration,
		mockTimingProviderStrict,
		std::bind(&TimeEvictionStrategyTest::mockedIsStopRequestedFunctionAlwaysFalse, this)
	);
	ON_CALL(*mockBeaconCacheStrict, getBeaconKeys())
		.WillByDefault(testing::Return(BeaconKeySet_t()));

	// when executing the first time
	target.execute();

	// also ensure that the last run timestamp was updated
	ASSERT_THAT(target.getLastRunTimestamp(), testing::Eq(2000L));
}

TEST_F(TimeEvictionStrategyTest, executeEvictionCallsEvictionForEachBeaconSeparately)
{
	// with
	BeaconKey_t keyOne(1, 0);
	BeaconKey_t keyTwo(42, 0);
	auto configuration = createBeaconCacheConfig(1000L, 1000L, 2000L);

	// expect
	EXPECT_CALL(*mockBeaconCacheStrict, getBeaconKeys())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockBeaconCacheStrict, evictRecordsByAge(keyOne, 2099L - configuration->getMaxRecordAge()))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockBeaconCacheStrict, evictRecordsByAge(keyTwo, 2099L - configuration->getMaxRecordAge()))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockTimingProviderStrict, provideTimestampInMilliseconds())
		.WillOnce(testing::Return(1000L))		// 1000 for TimeEvictionStrategy::execute() (first time execution)
		.WillOnce(testing::Return(2099L))		// 2099 for TimeEvictionStrategy::shouldRun()
		.WillOnce(testing::Return(2099L));		// 2099 for TimeEvictionStrategy::doExecute() with no beacons

	// given
	TimeEvictionStrategy_t target(
		mockLoggerNice,
		mockBeaconCacheStrict,
		configuration,
		mockTimingProviderStrict,
		std::bind(&TimeEvictionStrategyTest::mockedIsStopRequestedFunctionAlwaysFalse, this)
	);
	ON_CALL(*mockBeaconCacheStrict, getBeaconKeys())
		.WillByDefault(testing::Return(BeaconKeySet_t({ keyOne, keyTwo } )));

	// when
	target.execute();

	// also ensure that the last run timestamp was updated
	ASSERT_THAT(target.getLastRunTimestamp(), testing::Eq(2099L));
}

TEST_F(TimeEvictionStrategyTest, executeEvictionLogsTheNumberOfRecordsRemoved)
{
	// with
	BeaconKey_t keyOne(1, 0);
	BeaconKey_t keyTwo(42, 13);

	// expect
	EXPECT_CALL(*mockLoggerStrict, isDebugEnabled())
		.Times(2);
	EXPECT_CALL(*mockLoggerStrict, mockDebug("TimeEvictionStrategy doExecute() - Removed 2 records from Beacon with key [sn=1, seq=0]"))
		.Times(1);
	EXPECT_CALL(*mockLoggerStrict, mockDebug("TimeEvictionStrategy doExecute() - Removed 5 records from Beacon with key [sn=42, seq=13]"))
		.Times(1);
	EXPECT_CALL(*mockTimingProviderNice, provideTimestampInMilliseconds())
		.WillOnce(testing::Return(1000L))		// 1000 for TimeEvictionStrategy::execute() (first time execution)
		.WillOnce(testing::Return(2099L))		// 2099 for TimeEvictionStrategy::shouldRun()
		.WillOnce(testing::Return(2099L));		// 2099 for TimeEvictionStrategy::doExecute() with no beacons

	// given
	auto configuration = createBeaconCacheConfig(1000L, 1000L, 2000L);
	TimeEvictionStrategy_t target(
		mockLoggerStrict,
		mockBeaconCacheNice,
		configuration,
		mockTimingProviderNice,
		std::bind(&TimeEvictionStrategyTest::mockedIsStopRequestedFunctionAlwaysFalse, this)
	);
	
	ON_CALL(*mockBeaconCacheNice, getBeaconKeys())
		.WillByDefault(testing::Return(BeaconKeySet_t({ keyOne, keyTwo })));
	ON_CALL(*mockBeaconCacheNice, evictRecordsByAge(keyOne, testing::_))
		.WillByDefault(testing::Return(2));
	ON_CALL(*mockBeaconCacheNice, evictRecordsByAge(keyTwo, testing::_))
		.WillByDefault(testing::Return(5));

	// when executing
	target.execute();
}

TEST_F(TimeEvictionStrategyTest, executeEvictionIsStoppedIfStopIsRequested)
{
	// with
	auto configuration = createBeaconCacheConfig(1000L, 1000L, 2000L);

	// expect
	EXPECT_CALL(*mockBeaconCacheStrict, getBeaconKeys())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockBeaconCacheStrict, evictRecordsByAge(testing::_, 2099L - configuration->getMaxRecordAge()))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockTimingProviderStrict, provideTimestampInMilliseconds())
		.WillOnce(testing::Return(1000L))		// 1000 for TimeEvictionStrategy::execute() (first time execution)
		.WillOnce(testing::Return(2099L))		// 2099 for TimeEvictionStrategy::shouldRun()
		.WillOnce(testing::Return(2099L));		// 2099 for TimeEvictionStrategy::doExecute() with no beacons

	// given
	uint32_t callCountIsStopRequested = 0;
	auto isStopRequested = [&callCountIsStopRequested]() -> bool {
		// isStopRequested shall return "true" after the 1st call
		return ++callCountIsStopRequested != 1;
	};
	TimeEvictionStrategy_t target(
		mockLoggerNice,
		mockBeaconCacheStrict,
		configuration,
		mockTimingProviderStrict,
		isStopRequested
	);

	BeaconKey_t keyOne(1, 0);
	BeaconKey_t keyTwo(42, 0);

	ON_CALL(*mockBeaconCacheStrict, getBeaconKeys())
		.WillByDefault(testing::Return(BeaconKeySet_t({ keyOne, keyTwo })));

	// when
	target.execute();
}
