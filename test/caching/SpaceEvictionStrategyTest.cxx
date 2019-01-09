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

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "configuration/BeaconCacheConfiguration.h"
#include "caching/BeaconCache.h"
#include "caching/SpaceEvictionStrategy.h"
#include "core/util/DefaultLogger.h"
#include "../caching/MockBeaconCache.h"

#include <memory>

using namespace configuration;
using namespace caching;

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
public:
	SpaceEvictionStrategyTest()
		: mLogger(nullptr)
		, mMockBeaconCache()
	{
	}

	void SetUp()
	{
		mLogger = std::shared_ptr<openkit::ILogger>(new core::util::DefaultLogger(devNull, true));
		mMockBeaconCache = std::shared_ptr<testing::NiceMock<test::MockBeaconCache>>(new testing::NiceMock<test::MockBeaconCache>());
	}

	void TearDown()
	{
		mLogger = nullptr;
		mMockBeaconCache = nullptr;
	}

	std::ostringstream devNull;
	std::shared_ptr<openkit::ILogger> mLogger;
	std::shared_ptr<testing::NiceMock<test::MockBeaconCache>> mMockBeaconCache;

	bool mockedIsAliveFunctionAlwaysTrue()
	{
		return true;
	}
};

TEST_F(SpaceEvictionStrategyTest, theStrategyIsDisabledIfCacheSizeLowerBoundIsEqualToZero)
{
	// given
	auto configuration = std::make_shared<BeaconCacheConfiguration>(1000L, 0L, 2000L);
	SpaceEvictionStrategy target(mLogger, mMockBeaconCache, configuration, std::bind(&SpaceEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this));

	// then
	ASSERT_TRUE(target.isStrategyDisabled());
}

TEST_F(SpaceEvictionStrategyTest, theStrategyIsDisabledIfCacheSizeLowerBoundIsLessThanZero)
{
	// given
	auto configuration = std::make_shared<BeaconCacheConfiguration>(1000L, -1L, 2000L);
	SpaceEvictionStrategy target(mLogger, mMockBeaconCache, configuration, std::bind(&SpaceEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this));

	// then
	ASSERT_TRUE(target.isStrategyDisabled());
}

TEST_F(SpaceEvictionStrategyTest, theStrategyIsDisabledIfCacheSizeUpperBoundIsEqualToZero)
{
	// given
	auto configuration = std::make_shared<BeaconCacheConfiguration>(1000L, 1000L, 0L);
	SpaceEvictionStrategy target(mLogger, mMockBeaconCache, configuration, std::bind(&SpaceEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this));

	// then
	ASSERT_TRUE(target.isStrategyDisabled());
}

TEST_F(SpaceEvictionStrategyTest, theStrategyIsDisabledIfCacheSizeUpperBoundIsLessThanLowerBound)
{
	// given
	auto configuration = std::make_shared<BeaconCacheConfiguration>(1000L, 1000L, 999L);
	SpaceEvictionStrategy target(mLogger, mMockBeaconCache, configuration, std::bind(&SpaceEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this));

	// then
	ASSERT_TRUE(target.isStrategyDisabled());
}

TEST_F(SpaceEvictionStrategyTest, shouldRunGivesTrueIfNumBytesInCacheIsGreaterThanUpperBoundLimit)
{
	// given
	auto configuration = std::make_shared<BeaconCacheConfiguration>(1000L, 1000L, 2000L);
	SpaceEvictionStrategy target(mLogger, mMockBeaconCache, configuration, std::bind(&SpaceEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this));

	//when
	ON_CALL(*mMockBeaconCache, getNumBytesInCache())
		.WillByDefault(testing::Return(configuration->getCacheSizeUpperBound() + 1));

	// then
	ASSERT_TRUE(target.shouldRun());
}

TEST_F(SpaceEvictionStrategyTest, shouldRunGivesFalseIfNumBytesInCacheIsEqualToUpperBoundLimit)
{
	// given
	auto configuration = std::make_shared<BeaconCacheConfiguration>(1000L, 1000L, 2000L);
	SpaceEvictionStrategy target(mLogger, mMockBeaconCache, configuration, std::bind(&SpaceEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this));

	//when
	ON_CALL(*mMockBeaconCache, getNumBytesInCache())
		.WillByDefault(testing::Return(configuration->getCacheSizeUpperBound()));

	// then
	ASSERT_FALSE(target.shouldRun());
}

TEST_F(SpaceEvictionStrategyTest, shouldRunGivesFalseIfNumBytesInCacheIsLessThanUpperBoundLimit)
{
	// given
	auto configuration = std::make_shared<BeaconCacheConfiguration>(1000L, 1000L, 2000L);
	SpaceEvictionStrategy target(mLogger, mMockBeaconCache, configuration, std::bind(&SpaceEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this));

	//when
	ON_CALL(*mMockBeaconCache, getNumBytesInCache())
		.WillByDefault(testing::Return(configuration->getCacheSizeUpperBound() - 1));

	// then
	ASSERT_FALSE(target.shouldRun());
}

TEST_F(SpaceEvictionStrategyTest, executeEvictionLogsAMessageOnceAndReturnsIfStrategyIsDisabled)
{
	// given
	std::ostringstream oss;
	auto logger = std::shared_ptr<openkit::ILogger>(new core::util::DefaultLogger(oss, true));
	auto configuration = std::make_shared<BeaconCacheConfiguration>(1000L, 1000L, -1L);
	SpaceEvictionStrategy target(logger, mMockBeaconCache, configuration, std::bind(&SpaceEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this));

	// when executing the first time
	target.execute();

	// then
	auto found = oss.str().find("SpaceEvictionStrategy execute() - strategy is disabled\n");
	ASSERT_TRUE(found != std::string::npos) << "Unexpected log statement: " << oss.str() << std::endl;
	oss.str("");
	oss.clear();

	// and when executing a second time
	target.execute();

	// then
	ASSERT_TRUE(oss.str().empty());
}

TEST_F(SpaceEvictionStrategyTest, executeEvictionDoesNotLogIfStrategyIsDisabledAndInfoIsDisabledInLogger)
{
	// given
	std::ostringstream oss;
	auto logger = std::shared_ptr<openkit::ILogger>(new core::util::DefaultLogger(oss, false));
	auto configuration = std::make_shared<BeaconCacheConfiguration>(1000L, 1000L, -1L);
	SpaceEvictionStrategy target(logger, mMockBeaconCache, configuration, std::bind(&SpaceEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this));

	// when executing the first time
	target.execute();

	// then
	ASSERT_TRUE(oss.str().empty());

	// and when executing a second time
	target.execute();

	// then
	ASSERT_TRUE(oss.str().empty());
}

TEST_F(SpaceEvictionStrategyTest, executeEvictionCallsCacheMethodForEachBeacon)
{
	// given
	auto configuration = std::make_shared<BeaconCacheConfiguration>(1000L, 1000L, 2000L);
	SpaceEvictionStrategy target(mLogger, mMockBeaconCache, configuration, std::bind(&SpaceEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this));
	ON_CALL(*mMockBeaconCache, getBeaconIDs())
		.WillByDefault(testing::Return(std::unordered_set<int32_t>({ 1, 42 })));

	// then
	EXPECT_CALL(*mMockBeaconCache, getNumBytesInCache())
		.WillOnce(testing::Return(2001L))		// 2001 for SpaceEvictionStrategy::shouldRun()
		.WillOnce(testing::Return(2001L))		// 2001 for outer while loop in SpaceEvictionStrategy::doExecute()
		.WillOnce(testing::Return(2001L))		// 2001 for inner while loop in SpaceEvictionStrategy::doExecute() which evicts beaconID 1
		.WillOnce(testing::Return(2001L))		// 2001 for inner while loop in SpaceEvictionStrategy::doExecute() which evicts beaconID 42
		.WillOnce(testing::Return(0L));			// 0 for outer while loop in SpaceEvictionStrategy::doExecute() (to exit the while loop)
	EXPECT_CALL(*mMockBeaconCache, evictRecordsByNumber(1, 1))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockBeaconCache, evictRecordsByNumber(42, 1))
		.Times(testing::Exactly(1));

	// when
	target.execute();
}

TEST_F(SpaceEvictionStrategyTest, executeEvictionLogsEvictionResultIfDebugIsEnabled)
{
	// given
	std::ostringstream oss;
	auto logger = std::shared_ptr<openkit::ILogger>(new core::util::DefaultLogger(oss, true));
	auto configuration = std::make_shared<BeaconCacheConfiguration>(1000L, 1000L, 2000L);
	SpaceEvictionStrategy target(logger, mMockBeaconCache, configuration, std::bind(&SpaceEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this));

	EXPECT_CALL(*mMockBeaconCache, getNumBytesInCache())
		.WillOnce(testing::Return(2001L))		// 2001 for SpaceEvictionStrategy::shouldRun()
		.WillOnce(testing::Return(2001L))		// 2001 for outer while loop in SpaceEvictionStrategy::doExecute()
		.WillOnce(testing::Return(2001L))		// 2001 for inner while loop in SpaceEvictionStrategy::doExecute() which evicts beaconID 1
		.WillOnce(testing::Return(2001L))		// 2001 for inner while loop in SpaceEvictionStrategy::doExecute() which evicts beaconID 42
		.WillOnce(testing::Return(0L));			// 0 for outer while loop in SpaceEvictionStrategy::doExecute() (to exit the while loop)
	ON_CALL(*mMockBeaconCache, getBeaconIDs())
		.WillByDefault(testing::Return(std::unordered_set<int32_t>({ 1, 42 })));
	ON_CALL(*mMockBeaconCache, evictRecordsByNumber(1, testing::_))
		.WillByDefault(testing::Return(5));
	ON_CALL(*mMockBeaconCache, evictRecordsByNumber(42, testing::_))
		.WillByDefault(testing::Return(1));

	// when executing
	target.execute();

	// then
	auto found = oss.str().find("SpaceEvictionStrategy doExecute() - Removed 5 records from Beacon with ID 1\n");
	ASSERT_TRUE(found != std::string::npos) << "Unexpected log statement: " << oss.str() << std::endl;
	found = oss.str().find("SpaceEvictionStrategy doExecute() - Removed 1 records from Beacon with ID 42\n");
	ASSERT_TRUE(found != std::string::npos) << "Unexpected log statement: " << oss.str() << std::endl;
}

TEST_F(SpaceEvictionStrategyTest, executeEvictionDoesNotLogEvictionResultIfDebugIsDisabled)
{
	// given
	std::ostringstream oss;
	auto logger = std::shared_ptr<openkit::ILogger>(new core::util::DefaultLogger(oss, false));
	auto configuration = std::make_shared<BeaconCacheConfiguration>(1000L, 1000L, 2000L);
	SpaceEvictionStrategy target(logger, mMockBeaconCache, configuration, std::bind(&SpaceEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this));

	EXPECT_CALL(*mMockBeaconCache, getNumBytesInCache())
		.WillOnce(testing::Return(2001L))		// 2001 for SpaceEvictionStrategy::shouldRun()
		.WillOnce(testing::Return(2001L))		// 2001 for outer while loop in SpaceEvictionStrategy::doExecute()
		.WillOnce(testing::Return(2001L))		// 2001 for inner while loop in SpaceEvictionStrategy::doExecute() which evicts beaconID 1
		.WillOnce(testing::Return(2001L))		// 2001 for inner while loop in SpaceEvictionStrategy::doExecute() which evicts beaconID 42
		.WillOnce(testing::Return(0L));			// 0 for outer while loop in SpaceEvictionStrategy::doExecute() (to exit the while loop)
	ON_CALL(*mMockBeaconCache, getBeaconIDs())
		.WillByDefault(testing::Return(std::unordered_set<int32_t>({ 1, 42 })));
	ON_CALL(*mMockBeaconCache, evictRecordsByNumber(1, testing::_))
		.WillByDefault(testing::Return(5));
	ON_CALL(*mMockBeaconCache, evictRecordsByNumber(42, testing::_))
		.WillByDefault(testing::Return(1));

	// when executing
	target.execute();

	// then
	ASSERT_TRUE(oss.str().empty());
}

TEST_F(SpaceEvictionStrategyTest, executeEvictionRunsUntilTheCacheSizeIsLessThanOrEqualToLowerBound)
{
	// given
	auto configuration = std::make_shared<BeaconCacheConfiguration>(1000L, 1000L, 2000L);
	SpaceEvictionStrategy target(mLogger, mMockBeaconCache, configuration, std::bind(&SpaceEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this));

	ON_CALL(*mMockBeaconCache, getBeaconIDs())
		.WillByDefault(testing::Return(std::unordered_set<int32_t>({ 1, 42 })));

	// then
	EXPECT_CALL(*mMockBeaconCache, getNumBytesInCache())
		.WillOnce(testing::Return(2001L))		// 2001 for SpaceEvictionStrategy::shouldRun()
		.WillOnce(testing::Return(2000L))		// 2000 for outer while loop in SpaceEvictionStrategy::doExecute()
		.WillOnce(testing::Return(2000L))		// 2000 for inner while loop in SpaceEvictionStrategy::doExecute() which evicts beaconID 1
		.WillOnce(testing::Return(2000L))		// 2000 for inner while loop in SpaceEvictionStrategy::doExecute() which evicts beaconID 42
		.WillOnce(testing::Return(1500L))		// 1500 for outer while loop (second iteration) in SpaceEvictionStrategy::doExecute()
		.WillOnce(testing::Return(1500L))		// 1500 for inner while loop in SpaceEvictionStrategy::doExecute() which evicts beaconID 1
		.WillOnce(testing::Return(1500L))		// 1500 for inner while loop in SpaceEvictionStrategy::doExecute() which evicts beaconID 42
		.WillOnce(testing::Return(1000L))		// 1000 for outer while loop in SpaceEvictionStrategy::doExecute() (to exit the while loop)
		.WillRepeatedly(testing::Return(0L));	// just for safety
	EXPECT_CALL(*mMockBeaconCache, evictRecordsByNumber(1, 1))
		.Times(testing::Exactly(2));
	EXPECT_CALL(*mMockBeaconCache, evictRecordsByNumber(42, 1))
		.Times(testing::Exactly(2));

	// when
	target.execute();
}

TEST_F(SpaceEvictionStrategyTest, executeEvictionStopsIfThreadGetsInterruptedBetweenTwoBeacons)
{
	// given
	auto configuration = std::make_shared<BeaconCacheConfiguration>(1000L, 1000L, 2000L);
	auto mockIsAlive = std::shared_ptr<testing::NiceMock<MockIsAlive>>(new testing::NiceMock<MockIsAlive>());
	SpaceEvictionStrategy target(mLogger, mMockBeaconCache, configuration, std::bind(&MockIsAlive::isAlive, mockIsAlive));

	uint32_t callCountIsAlive = 0;
	ON_CALL(*mockIsAlive, isAlive())
		.WillByDefault(testing::Invoke(
			[&callCountIsAlive]() -> bool {
		// isAlive shall return "false" after the 2nd call
		callCountIsAlive++;
		return callCountIsAlive <= 2;
	}
	));
	ON_CALL(*mMockBeaconCache, getBeaconIDs())
		.WillByDefault(testing::Return(std::unordered_set<int32_t>({ 1, 42 })));

	// then
	EXPECT_CALL(*mMockBeaconCache, getNumBytesInCache())
		.WillOnce(testing::Return(2001L))		// 2001 for SpaceEvictionStrategy::shouldRun()
		.WillOnce(testing::Return(2000L))		// 2000 for outer while loop in SpaceEvictionStrategy::doExecute()
		.WillOnce(testing::Return(2000L))		// 2000 for inner while loop in SpaceEvictionStrategy::doExecute() which evicts beaconID 1
		.WillRepeatedly(testing::Return(0L));	// just for safety
	EXPECT_CALL(*mMockBeaconCache, evictRecordsByNumber(testing::_, 1))
		.Times(testing::Exactly(1));
	
	// when
	target.execute();
}

TEST_F(SpaceEvictionStrategyTest, executeEvictionStopsIfNumBytesInCacheFallsBelowLowerBoundBetweenTwoBeacons)
{
	// given
	auto configuration = std::make_shared<BeaconCacheConfiguration>(1000L, 1000L, 2000L);
	SpaceEvictionStrategy target(mLogger, mMockBeaconCache, configuration, std::bind(&SpaceEvictionStrategyTest::mockedIsAliveFunctionAlwaysTrue, this));
	ON_CALL(*mMockBeaconCache, getBeaconIDs())
		.WillByDefault(testing::Return(std::unordered_set<int32_t>({ 1, 42 })));

	// then
	EXPECT_CALL(*mMockBeaconCache, getNumBytesInCache())
		.WillOnce(testing::Return(2001L))		// 2001 for SpaceEvictionStrategy::shouldRun()
		.WillOnce(testing::Return(2000L))		// 2000 for outer while loop in SpaceEvictionStrategy::doExecute()
		.WillOnce(testing::Return(2000L))		// 2000 for inner while loop in SpaceEvictionStrategy::doExecute() which evicts beaconID 1
		.WillOnce(testing::Return(2000L))		// 2000 for inner while loop in SpaceEvictionStrategy::doExecute() which evicts beaconID 42
		.WillOnce(testing::Return(1500L))		// 1500 for outer while loop (second iteration) in SpaceEvictionStrategy::doExecute()
		.WillOnce(testing::Return(1500L))		// 1500 for inner while loop in SpaceEvictionStrategy::doExecute() which evicts beaconID 1
		.WillOnce(testing::Return(1000L))		// 1000 for outer while loop in SpaceEvictionStrategy::doExecute() (to exit the while loop)
		.WillRepeatedly(testing::Return(0L));	// just for safety
	EXPECT_CALL(*mMockBeaconCache, evictRecordsByNumber(testing::An<int32_t>(), 1))
		.Times(testing::Exactly(3));
	
	// when
	target.execute();
}