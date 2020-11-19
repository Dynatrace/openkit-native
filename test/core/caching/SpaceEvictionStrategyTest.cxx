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

#include "core/caching/SpaceEvictionStrategy.h"
#include "core/caching/BeaconKey.h"

#include "mock/MockIBeaconCache.h"
#include "../configuration/mock/MockIBeaconCacheConfiguration.h"
#include "../../api/mock/MockILogger.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <memory>

using namespace test;

using MockNiceIBeaconCache_sp = std::shared_ptr<testing::NiceMock<MockIBeaconCache>>;
using MockNiceIBeaconCacheConfiguration_sp = std::shared_ptr<testing::NiceMock<MockIBeaconCacheConfiguration>>;
using MockNiceILogger_sp = std::shared_ptr<testing::NiceMock<MockILogger>>;
using MockStrictILogger_sp = std::shared_ptr<testing::StrictMock<MockILogger>>;
using SpaceEvictionStrategy_t = core::caching::SpaceEvictionStrategy;
using BeaconKey_t = core::caching::BeaconKey;
using BeaconKeySet_t = std::unordered_set<BeaconKey_t, BeaconKey_t::Hash>;

class SpaceEvictionStrategyTest : public testing::Test
{
protected:

	MockNiceILogger_sp mockLoggerNice;
	MockNiceIBeaconCache_sp mockBeaconCache;

	MockStrictILogger_sp mockLoggerStrict;

	void SetUp() override
	{
		mockLoggerNice = MockILogger::createNice();
		mockBeaconCache = MockIBeaconCache::createNice();

		mockLoggerStrict = MockILogger::createStrict();
	}

	MockNiceIBeaconCacheConfiguration_sp createBeaconCacheConfig(int64_t maxAge, int64_t lowerBound, int64_t upperBound)
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

TEST_F(SpaceEvictionStrategyTest, theStrategyIsDisabledIfCacheSizeLowerBoundIsEqualToZero)
{
	// given
	auto configuration = createBeaconCacheConfig(1000L, 0L, 2000L);
	SpaceEvictionStrategy_t target(
		mockLoggerNice,
		mockBeaconCache,
		configuration,
		std::bind(&SpaceEvictionStrategyTest::mockedIsStopRequestedFunctionAlwaysFalse, this)
	);

	// then
	ASSERT_THAT(target.isStrategyDisabled(), testing::Eq(true));
}

TEST_F(SpaceEvictionStrategyTest, theStrategyIsDisabledIfCacheSizeLowerBoundIsLessThanZero)
{
	// given
	auto configuration = createBeaconCacheConfig(1000L, -1L, 2000L);
	SpaceEvictionStrategy_t target(
		mockLoggerNice,
		mockBeaconCache,
		configuration,
		std::bind(&SpaceEvictionStrategyTest::mockedIsStopRequestedFunctionAlwaysFalse, this)
	);

	// then
	ASSERT_THAT(target.isStrategyDisabled(), testing::Eq(true));
}

TEST_F(SpaceEvictionStrategyTest, theStrategyIsDisabledIfCacheSizeUpperBoundIsEqualToZero)
{
	// given
	auto configuration = createBeaconCacheConfig(1000L, 1000L, 0L);
	SpaceEvictionStrategy_t target(
		mockLoggerNice,
		mockBeaconCache,
		configuration,
		std::bind(&SpaceEvictionStrategyTest::mockedIsStopRequestedFunctionAlwaysFalse, this)
	);

	// then
	ASSERT_THAT(target.isStrategyDisabled(), testing::Eq(true));
}

TEST_F(SpaceEvictionStrategyTest, theStrategyIsDisabledIfCacheSizeUpperBoundIsLessThanZero)
{
	// given
	auto configuration = createBeaconCacheConfig(1000L, 1000L, -1L);
	SpaceEvictionStrategy_t target(
		mockLoggerNice,
		mockBeaconCache,
		configuration,
		std::bind(&SpaceEvictionStrategyTest::mockedIsStopRequestedFunctionAlwaysFalse, this)
	);

	// then
	ASSERT_THAT(target.isStrategyDisabled(), testing::Eq(true));
}

TEST_F(SpaceEvictionStrategyTest, theStrategyIsDisabledIfCacheSizeUpperBoundIsLessThanLowerBound)
{
	// given
	auto configuration = createBeaconCacheConfig(1000L, 1000L, 999L);
	SpaceEvictionStrategy_t target(
		mockLoggerNice,
		mockBeaconCache,
		configuration,
		std::bind(&SpaceEvictionStrategyTest::mockedIsStopRequestedFunctionAlwaysFalse, this)
	);

	// then
	ASSERT_THAT(target.isStrategyDisabled(), testing::Eq(true));
}

TEST_F(SpaceEvictionStrategyTest, shouldRunGivesTrueIfNumBytesInCacheIsGreaterThanUpperBoundLimit)
{
	// given
	auto configuration = createBeaconCacheConfig(1000L, 1000L, 2000L);
	SpaceEvictionStrategy_t target(
		mockLoggerNice,
		mockBeaconCache,
		configuration,
		std::bind(&SpaceEvictionStrategyTest::mockedIsStopRequestedFunctionAlwaysFalse, this)
	);

	//when
	ON_CALL(*mockBeaconCache, getNumBytesInCache())
		.WillByDefault(testing::Return(configuration->getCacheSizeUpperBound() + 1));

	// then
	ASSERT_THAT(target.shouldRun(), testing::Eq(true));
}

TEST_F(SpaceEvictionStrategyTest, shouldRunGivesFalseIfNumBytesInCacheIsEqualToUpperBoundLimit)
{
	// given
	auto configuration = createBeaconCacheConfig(1000L, 1000L, 2000L);
	SpaceEvictionStrategy_t target(
		mockLoggerNice,
		mockBeaconCache,
		configuration,
		std::bind(&SpaceEvictionStrategyTest::mockedIsStopRequestedFunctionAlwaysFalse, this)
	);

	//when
	ON_CALL(*mockBeaconCache, getNumBytesInCache())
		.WillByDefault(testing::Return(configuration->getCacheSizeUpperBound()));

	// then
	ASSERT_THAT(target.shouldRun(), testing::Eq(false));
}

TEST_F(SpaceEvictionStrategyTest, shouldRunGivesFalseIfNumBytesInCacheIsLessThanUpperBoundLimit)
{
	// given
	auto configuration = createBeaconCacheConfig(1000L, 1000L, 2000L);
	SpaceEvictionStrategy_t target(
		mockLoggerNice,
		mockBeaconCache,
		configuration,
		std::bind(&SpaceEvictionStrategyTest::mockedIsStopRequestedFunctionAlwaysFalse, this)
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
	auto configuration = createBeaconCacheConfig(1000L, 1000L, -1L);
	SpaceEvictionStrategy_t target(
		mockLoggerStrict,
		mockBeaconCache,
		configuration, std::bind(&SpaceEvictionStrategyTest::mockedIsStopRequestedFunctionAlwaysFalse, this)
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

	// expect
	EXPECT_CALL(*mockLoggerStrict, isInfoEnabled())
		.Times(1);

	// given
	auto configuration = createBeaconCacheConfig(1000L, 1000L, -1L);
	SpaceEvictionStrategy_t target(
		mockLoggerStrict,
		mockBeaconCache,
		configuration,
		std::bind(&SpaceEvictionStrategyTest::mockedIsStopRequestedFunctionAlwaysFalse, this)
	);

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
	// with
	BeaconKey_t keyOne(1, 0);
	BeaconKey_t keyTwo(42, 0);

	// expect
	EXPECT_CALL(*mockBeaconCache, getNumBytesInCache())
		.WillOnce(testing::Return(2001L))		// 2001 for SpaceEvictionStrategy::shouldRun()
		.WillOnce(testing::Return(2001L))		// 2001 for outer while loop in SpaceEvictionStrategy::doExecute()
		.WillOnce(testing::Return(2001L))		// 2001 for inner while loop in SpaceEvictionStrategy::doExecute() which evicts beaconID 1
		.WillOnce(testing::Return(2001L))		// 2001 for inner while loop in SpaceEvictionStrategy::doExecute() which evicts beaconID 42
		.WillOnce(testing::Return(0L));			// 0 for outer while loop in SpaceEvictionStrategy::doExecute() (to exit the while loop)
	EXPECT_CALL(*mockBeaconCache, evictRecordsByNumber(keyOne, 1))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockBeaconCache, evictRecordsByNumber(keyTwo, 1))
		.Times(testing::Exactly(1));

	// given
	auto configuration = createBeaconCacheConfig(1000L, 1000L, 2000L);
	SpaceEvictionStrategy_t target(
		mockLoggerNice,
		mockBeaconCache,
		configuration,
		std::bind(&SpaceEvictionStrategyTest::mockedIsStopRequestedFunctionAlwaysFalse, this)
	);
	ON_CALL(*mockBeaconCache, getBeaconKeys())
		.WillByDefault(testing::Return(BeaconKeySet_t({ keyOne, keyTwo })));

	// when
	target.execute();
}

TEST_F(SpaceEvictionStrategyTest, executeEvictionLogsEvictionResultIfDebugIsEnabled)
{
	// with
	BeaconKey_t keyOne(1, 0);
	BeaconKey_t keyTwo(42, 14);

	// expect
	EXPECT_CALL(*mockLoggerStrict, isDebugEnabled())
		.Times(3);
	EXPECT_CALL(*mockLoggerStrict, mockDebug("SpaceEvictionStrategy doExecute() - Removed 5 records from Beacon with key [sn=1, seq=0]"))
		.Times(1);
	EXPECT_CALL(*mockLoggerStrict, mockDebug("SpaceEvictionStrategy doExecute() - Removed 1 records from Beacon with key [sn=42, seq=14]"))
		.Times(1);
	EXPECT_CALL(*mockBeaconCache, getNumBytesInCache())
		.WillOnce(testing::Return(2001L))		// 2001 for SpaceEvictionStrategy::shouldRun()
		.WillOnce(testing::Return(2001L))		// 2001 for outer while loop in SpaceEvictionStrategy::doExecute()
		.WillOnce(testing::Return(2001L))		// 2001 for inner while loop in SpaceEvictionStrategy::doExecute() which evicts beaconID 1
		.WillOnce(testing::Return(2001L))		// 2001 for inner while loop in SpaceEvictionStrategy::doExecute() which evicts beaconID 42
		.WillOnce(testing::Return(0L));			// 0 for outer while loop in SpaceEvictionStrategy::doExecute() (to exit the while loop)

	// given
	auto configuration = createBeaconCacheConfig(1000L, 1000L, 2000L);
	SpaceEvictionStrategy_t target(
		mockLoggerStrict,
		mockBeaconCache,
		configuration,
		std::bind(&SpaceEvictionStrategyTest::mockedIsStopRequestedFunctionAlwaysFalse, this)
	);

	ON_CALL(*mockBeaconCache, getBeaconKeys())
		.WillByDefault(testing::Return(BeaconKeySet_t({ keyOne, keyTwo })));
	ON_CALL(*mockBeaconCache, evictRecordsByNumber(keyOne, testing::_))
		.WillByDefault(testing::Return(5));
	ON_CALL(*mockBeaconCache, evictRecordsByNumber(keyTwo, testing::_))
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
	EXPECT_CALL(*mockBeaconCache, getNumBytesInCache())
		.WillOnce(testing::Return(2001L))		// 2001 for SpaceEvictionStrategy::shouldRun()
		.WillOnce(testing::Return(2001L))		// 2001 for outer while loop in SpaceEvictionStrategy::doExecute()
		.WillOnce(testing::Return(2001L))		// 2001 for inner while loop in SpaceEvictionStrategy::doExecute() which evicts beaconID 1
		.WillOnce(testing::Return(2001L))		// 2001 for inner while loop in SpaceEvictionStrategy::doExecute() which evicts beaconID 42
		.WillOnce(testing::Return(0L));			// 0 for outer while loop in SpaceEvictionStrategy::doExecute() (to exit the while loop)

	// given
	auto configuration = createBeaconCacheConfig(1000L, 1000L, 2000L);
	SpaceEvictionStrategy_t target(
		mockLoggerStrict,
		mockBeaconCache,
		configuration,
		std::bind(&SpaceEvictionStrategyTest::mockedIsStopRequestedFunctionAlwaysFalse, this)
	);

	BeaconKey_t keyOne(1, 0);
	BeaconKey_t keyTwo(42, 0);

	ON_CALL(*mockBeaconCache, getBeaconKeys())
		.WillByDefault(testing::Return(BeaconKeySet_t({ keyOne, keyTwo })));
	ON_CALL(*mockBeaconCache, evictRecordsByNumber(keyOne, testing::_))
		.WillByDefault(testing::Return(5));
	ON_CALL(*mockBeaconCache, evictRecordsByNumber(keyTwo, testing::_))
		.WillByDefault(testing::Return(1));

	// when executing
	target.execute();
}

TEST_F(SpaceEvictionStrategyTest, executeEvictionRunsUntilTheCacheSizeIsLessThanOrEqualToLowerBound)
{
	// with
	BeaconKey_t keyOne(1, 0);
	BeaconKey_t keyTwo(42, 0);

	// expect
	EXPECT_CALL(*mockBeaconCache, evictRecordsByNumber(keyOne, 1))
		.Times(testing::Exactly(2));
	EXPECT_CALL(*mockBeaconCache, evictRecordsByNumber(keyTwo, 1))
		.Times(testing::Exactly(2));

	// given
	auto configuration = createBeaconCacheConfig(1000L, 1000L, 2000L);
	SpaceEvictionStrategy_t target(
		mockLoggerNice,
		mockBeaconCache,
		configuration,
		std::bind(&SpaceEvictionStrategyTest::mockedIsStopRequestedFunctionAlwaysFalse, this)
	);

	ON_CALL(*mockBeaconCache, getBeaconKeys())
		.WillByDefault(testing::Return(BeaconKeySet_t({ keyOne, keyTwo })));

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

	// when
	target.execute();
}

TEST_F(SpaceEvictionStrategyTest, executeEvictionStopsIfThreadGetsInterruptedBetweenTwoBeacons)
{
	// expect
	EXPECT_CALL(*mockBeaconCache, getNumBytesInCache())
		.WillOnce(testing::Return(2001L))		// 2001 for SpaceEvictionStrategy::shouldRun()
		.WillOnce(testing::Return(2000L))		// 2000 for outer while loop in SpaceEvictionStrategy::doExecute()
		.WillOnce(testing::Return(2000L))		// 2000 for inner while loop in SpaceEvictionStrategy::doExecute() which evicts beaconID 1
		.WillRepeatedly(testing::Return(0L));	// just for safety
	EXPECT_CALL(*mockBeaconCache, evictRecordsByNumber(testing::_, 1))
		.Times(testing::Exactly(1));

	// given
	auto configuration = createBeaconCacheConfig(1000L, 1000L, 2000L);
	uint32_t callCountIsStopRequested = 0;
	auto isStopRequested = [&callCountIsStopRequested]() -> bool {
		// isStopRequested shall return "true" after the 2nd call
		return ++callCountIsStopRequested > 2;
	};
	SpaceEvictionStrategy_t target(
		mockLoggerNice,
		mockBeaconCache,
		configuration,
		isStopRequested
	);

	BeaconKey_t keyOne(1, 0);
	BeaconKey_t keyTwo(42, 0);

	ON_CALL(*mockBeaconCache, getBeaconKeys())
		.WillByDefault(testing::Return(BeaconKeySet_t({ keyOne, keyTwo })));

	// when
	target.execute();
}

TEST_F(SpaceEvictionStrategyTest, executeEvictionStopsIfNumBytesInCacheFallsBelowLowerBoundBetweenTwoBeacons)
{
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
	EXPECT_CALL(*mockBeaconCache, evictRecordsByNumber(testing::An<const BeaconKey_t&>(), 1))
		.Times(testing::Exactly(3));

	// given
	auto configuration = createBeaconCacheConfig(1000L, 1000L, 2000L);
	SpaceEvictionStrategy_t target(
		mockLoggerNice,
		mockBeaconCache,
		configuration,
		std::bind(&SpaceEvictionStrategyTest::mockedIsStopRequestedFunctionAlwaysFalse, this)
	);

	BeaconKey_t keyOne(1, 0);
	BeaconKey_t keyTwo(42, 0);

	ON_CALL(*mockBeaconCache, getBeaconKeys())
		.WillByDefault(testing::Return(BeaconKeySet_t({ keyOne, keyTwo })));

	// when
	target.execute();
}