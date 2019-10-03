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

#include "Types.h"
#include "MockTypes.h"
#include "../../api/Types.h"
#include "../configuration/Types.h"
#include "../providers/MockTypes.h"
#include "../util/Types.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <memory>

using namespace test::types;

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
	TimeEvictionStrategyTest()
		: mLogger(nullptr)
		, mMockBeaconCache()
		, mMockTimingProvider()
	{
	}

	void SetUp()
	{
		mLogger = std::make_shared<cDefaultLogger_t>(devNull, LogLevel_t::LOG_LEVEL_DEBUG);
		mMockBeaconCache = std::make_shared<MockNiceBeaconCache_t>();
		mMockTimingProvider = std::make_shared()<MockNiceTimingProvider_t>>();
	}

	void TearDown()
	{
		mLogger = nullptr;
		mMockTimingProvider = nullptr;
		mMockBeaconCache = nullptr;
	}

	std::ostringstream devNull;
	ILogger_t mLogger;
	MockNiceBeaconCache_sp mMockBeaconCache;
	MockNiceTimingProvider_sp mMockTimingProvider;

public:

	bool mockedIsAliveFunctionAlwaysTrue()
	{
		return true;
	}
};

TEST_F(TimeEvictionStrategyTest, theInitialLastRunTimestampIsMinusOne)
{
	// given (use StrictMocks to verify that no mock interactions were made)
	auto mockBeaconCache = std::shared_ptr<testing::NiceMock<test::MockBeaconCache>>(new testing::NiceMock<test::MockBeaconCache>());
	auto mockTimingProvider = std::shared_ptr<testing::NiceMock<test::MockTimingProvider>>(new testing::NiceMock<test::MockTimingProvider>());
	auto configuration = std::make_shared<BeaconCacheConfiguration>(-1L, 1000L, 2000L);
	TimeEvictionStrategy target(mLogger, mockBeaconCache, configuration, mockTimingProvider, std::bind(&TimeEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this));

	// then
	ASSERT_EQ(target.getLastRunTimestamp(), -1L);
}

TEST_F(TimeEvictionStrategyTest, theStrategyIsDisabledIfBeaconMaxAgeIsSetToLessThanZero)
{
	// given (use StrictMocks to verify that no mock interactions were made)
	auto mockBeaconCache = std::make_shared<MockStrictBeaconCache_t>();
	auto mockTimingProvider = std::make_shared<MockStrictTimingProvider_t>();
	auto configuration = std::make_shared<BeaconCacheConfiguration_t>(-1L, 1000L, 2000L);
	TimeEvictionStrategy_t target(mLogger, mockBeaconCache, configuration, mockTimingProvider, std::bind(&TimeEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this));

	// then
	ASSERT_TRUE(target.isStrategyDisabled());
}

TEST_F(TimeEvictionStrategyTest, theStrategyIsDisabledIfBeaconMaxAgeIsSetToZero)
{
	// given (use StrictMocks to verify that no mock interactions were made)
	auto mockBeaconCache = std::make_shared<MockStrictBeaconCache_t>();
	auto mockTimingProvider = std::make_shared<MockStrictTimingProvider_t>();
	auto configuration = std::make_shared<BeaconCacheConfiguration>(0L, 1000L, 2000L);
	TimeEvictionStrategy_t target(mLogger, mockBeaconCache, configuration, mockTimingProvider, std::bind(&TimeEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this));

	// then
	ASSERT_TRUE(target.isStrategyDisabled());
}

TEST_F(TimeEvictionStrategyTest, theStrategyIsNotDisabledIFMaxRecordAgeIsGreaterThanZero)
{
	// given (use StrictMocks to verify that no mock interactions were made)
	auto mockBeaconCache = std::make_shared<MockStrictBeaconCache_t>();
	auto mockTimingProvider = std::make_shared<MockStrictTimingProvider_t>();
	auto configuration = std::make_shared<BeaconCacheConfiguration_t>(1L, 1000L, 2000L);
	TimeEvictionStrategy_t target(mLogger, mockBeaconCache, configuration, mockTimingProvider, std::bind(&TimeEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this));

	// then
	ASSERT_FALSE(target.isStrategyDisabled());
}

TEST_F(TimeEvictionStrategyTest, shouldRunGivesFalseIfLastRunIsLessThanMaxAgeMillisecondsAgo)
{
	// given
	auto configuration = std::make_shared<BeaconCacheConfiguration_t>(1000L, 1000L, 2000L);
	TimeEvictionStrategy_t target(mLogger, mMockBeaconCache, configuration, mMockTimingProvider, std::bind(&TimeEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this));

	target.setLastRunTimestamp(1000);
	ON_CALL(*mMockTimingProvider, provideTimestampInMilliseconds())
		.WillByDefault(testing::Return(target.getLastRunTimestamp() + configuration->getMaxRecordAge() - 1));

	// then
	ASSERT_FALSE(target.shouldRun());
}

TEST_F(TimeEvictionStrategyTest, shouldRunGivesTrueIfLastRunIsExactlyMaxAgeMillisecondsAgo)
{
	// given
	auto configuration = std::make_shared<BeaconCacheConfiguration_t>(1000L, 1000L, 2000L);
	TimeEvictionStrategy_t target(mLogger, mMockBeaconCache, configuration, mMockTimingProvider, std::bind(&TimeEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this));

	target.setLastRunTimestamp(1000);
	ON_CALL(*mMockTimingProvider, provideTimestampInMilliseconds())
		.WillByDefault(testing::Return(target.getLastRunTimestamp() + configuration->getMaxRecordAge()));

	// then
	ASSERT_TRUE(target.shouldRun());
}

TEST_F(TimeEvictionStrategyTest, shouldRunGivesTrueIfLastRunIsMoreThanMaxAgeMillisecondsAgo)
{
	// given
	auto configuration = std::make_shared<BeaconCacheConfiguration_t>(1000L, 1000L, 2000L);
	TimeEvictionStrategy_t target(mLogger, mMockBeaconCache, configuration, mMockTimingProvider, std::bind(&TimeEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this));

	target.setLastRunTimestamp(1000);
	ON_CALL(*mMockTimingProvider, provideTimestampInMilliseconds())
		.WillByDefault(testing::Return(target.getLastRunTimestamp() + configuration->getMaxRecordAge() + 1));

	// then
	ASSERT_TRUE(target.shouldRun());
}

TEST_F(TimeEvictionStrategyTest, executeEvictionLogsAMessageOnceAndReturnsIfStrategyIsDisabled)
{
	// given
	std::ostringstream oss;
	auto logger = std::make_shared<DefaultLogger_t>(oss, LogLevel_t::LOG_LEVEL_DEBUG);
	auto configuration = std::make_shared<BeaconCacheConfiguration_t>(0L, 1000L, 2000L);
	TimeEvictionStrategy_t target(logger, mMockBeaconCache, configuration, mMockTimingProvider, std::bind(&TimeEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this));

	// when executing the first time
	target.execute();

	// then
	auto found = oss.str().find("TimeEvictionStrategy execute() - strategy is disabled\n");
	ASSERT_TRUE(found != std::string::npos) << "Unexpected log statement: " << oss.str() << std::endl;
	oss.str("");
	oss.clear();

	// and when executing a second time
	target.execute();

	// then
	ASSERT_TRUE(oss.str().empty());
}

TEST_F(TimeEvictionStrategyTest, executeEvictionDoesNotLogIfStrategyIsDisabledAndInfoIsDisabledInLogger)
{
	// given
	std::ostringstream oss;
	auto logger = std::make_shared<DefaultLogger_t>(oss, LogLevel_t::LOG_LEVEL_WARN);
	auto configuration = std::make_shared<BeaconCacheConfiguration_t>(0L, 1000L, 2000L);
	TimeEvictionStrategy_t target(logger, mMockBeaconCache, configuration, mMockTimingProvider, std::bind(&TimeEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this));

	// when executing the first time
	target.execute();

	// then
	ASSERT_TRUE(oss.str().empty());

	// and when executing a second time
	target.execute();

	// then
	ASSERT_TRUE(oss.str().empty());
}

TEST_F(TimeEvictionStrategyTest, lastRuntimeStampIsAdjustedDuringFirstExecution)
{
	// given
	auto configuration = std::make_shared<BeaconCacheConfiguration_t>(1000L, 1000L, 2000L);
	TimeEvictionStrategy_t target(mLogger, mMockBeaconCache, configuration, mMockTimingProvider, std::bind(&TimeEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this));

	// when executing the first time
	EXPECT_CALL(*mMockTimingProvider, provideTimestampInMilliseconds())
		.WillOnce(testing::Return(1000L))		// 1000 for TimeEvictionStrategy::execute() (first time execution)
		.WillOnce(testing::Return(1001L));		// 1001 for TimeEvictionStrategy::shouldRun()
	target.execute();

	// then
	ASSERT_EQ(target.getLastRunTimestamp(), 1000L);

	// when executing the second time
	EXPECT_CALL(*mMockTimingProvider, provideTimestampInMilliseconds())
		.WillOnce(testing::Return(1001L));		// 1001 for TimeEvictionStrategy::doExecute() with no beacons
	target.execute();

	// then
	ASSERT_EQ(target.getLastRunTimestamp(), 1000L);
}

TEST_F(TimeEvictionStrategyTest, executeEvictionStopsIfNoBeaconIdsAreAvailableInCache)
{
	// given (use StrictMocks to verify that no mock interactions were made)
	auto mockBeaconCache = std::make_shared<MockStrictBeaconCache_t>();
	auto mockTimingProvider = std::make_shared<MockStrictTimingProvider_t();
	auto configuration = std::make_shared<BeaconCacheConfiguration>(1000L, 1000L, 2000L);
	TimeEvictionStrategy target(mLogger, mockBeaconCache, configuration, mockTimingProvider, std::bind(&TimeEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this));
	ON_CALL(*mockBeaconCache, getBeaconIDs())
		.WillByDefault(testing::Return(std::unordered_set<int32_t>()));

	// when executing the first time
	EXPECT_CALL(*mockBeaconCache, getBeaconIDs())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockTimingProvider, provideTimestampInMilliseconds())
		.WillOnce(testing::Return(1000L))		// 1000 for TimeEvictionStrategy::execute() (first time execution)
		.WillOnce(testing::Return(2000L))		// 2000 for TimeEvictionStrategy::shouldRun()
		.WillOnce(testing::Return(2000L));		// 2000 for inner while loop in SpaceEvictionStrategy::doExecute() which evicts beaconID 1
	target.execute();

	// also ensure that the last run timestamp was updated
	ASSERT_EQ(target.getLastRunTimestamp(), 2000L);
}

TEST_F(TimeEvictionStrategyTest, executeEvictionCallsEvictionForEachBeaconSeparately)
{
	// given (use StrictMocks to verify that no mock interactions were made)
	auto mockBeaconCache = std::make_shared<MockStrictBeaconCache_t>();
	auto mockTimingProvider = std::make_shared<MockStrictTimingProvider_t>();
	auto configuration = std::make_shared<BeaconCacheConfiguration>(1000L, 1000L, 2000L);
	TimeEvictionStrategy target(mLogger, mockBeaconCache, configuration, mockTimingProvider, std::bind(&TimeEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this));
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
	// given
	std::ostringstream oss;
	auto logger = std::make_shared<DefaultLogger_t(oss, LogLevel_t::LOG_LEVEL_DEBUG);
	auto mockBeaconCache = std::make_shared<MockNiceBeaconCache_t>();
	auto mockTimingProvider = std::make_shared<std::MockNiceTimingProvider>();
	auto configuration = std::make_shared<BeaconCacheConfiguration_t>(1000L, 1000L, 2000L);
	TimeEvictionStrategy_t target(logger, mockBeaconCache, configuration, mockTimingProvider, std::bind(&TimeEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this));

	EXPECT_CALL(*mockTimingProvider, provideTimestampInMilliseconds())
		.WillOnce(testing::Return(1000L))		// 1000 for TimeEvictionStrategy::execute() (first time execution)
		.WillOnce(testing::Return(2099L))		// 2099 for TimeEvictionStrategy::shouldRun()
		.WillOnce(testing::Return(2099L));		// 2099 for TimeEvictionStrategy::doExecute() with no beacons
	ON_CALL(*mockBeaconCache, getBeaconIDs())
		.WillByDefault(testing::Return(std::unordered_set<int32_t>({ 1, 42 })));
	ON_CALL(*mockBeaconCache, evictRecordsByAge(1, testing::_))
		.WillByDefault(testing::Return(2));
	ON_CALL(*mockBeaconCache, evictRecordsByAge(42, testing::_))
		.WillByDefault(testing::Return(5));

	// when executing
	target.execute();

	// then
	auto found = oss.str().find("TimeEvictionStrategy doExecute() - Removed 2 records from Beacon with ID 1\n");
	ASSERT_TRUE(found != std::string::npos) << "Unexpected log statement: " << oss.str() << std::endl;
	found = oss.str().find("TimeEvictionStrategy doExecute() - Removed 5 records from Beacon with ID 42\n");
	ASSERT_TRUE(found != std::string::npos) << "Unexpected log statement: " << oss.str() << std::endl;
}

TEST_F(TimeEvictionStrategyTest, executeEvictionIsStoppedIfThreadGetsInterrupted)
{
	// given (use StrictMocks to verify that no mock interactions were made)
	auto mockBeaconCache = std::make_shared<MockStrictBeaconCache_t>();
	auto mockTimingProvider = std::make_shared<MockStrictTimingProvider>();
	auto configuration = std::make_shared<BeaconCacheConfiguration_t>(1000L, 1000L, 2000L);
	auto mockIsAlive = std::make_shared<testing::NiceMock<MockIsAlive>>();
	TimeEvictionStrategy_t target(mLogger, mockBeaconCache, configuration, mockTimingProvider, std::bind(&MockIsAlive::isAlive, mockIsAlive));
	uint32_t callCountIsAlive = 0;
	ON_CALL(*mockIsAlive, isAlive())
		.WillByDefault(testing::Invoke(
			[&callCountIsAlive]() -> bool {
		// isAlive shall return "false" after the 1st call
		callCountIsAlive++;
		return callCountIsAlive == 1;
	}
	));
	ON_CALL(*mockBeaconCache, getBeaconIDs())
		.WillByDefault(testing::Return(std::unordered_set<int32_t>({ 1, 42 })));

	// then verify interactions
	EXPECT_CALL(*mockBeaconCache, getBeaconIDs())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockBeaconCache, evictRecordsByAge(testing::_, 2099L - configuration->getMaxRecordAge()))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockTimingProvider, provideTimestampInMilliseconds())
		.WillOnce(testing::Return(1000L))		// 1000 for TimeEvictionStrategy::execute() (first time execution)
		.WillOnce(testing::Return(2099L))		// 2099 for TimeEvictionStrategy::shouldRun()
		.WillOnce(testing::Return(2099L));		// 2099 for TimeEvictionStrategy::doExecute() with no beacons
	// when
	target.execute();
}
