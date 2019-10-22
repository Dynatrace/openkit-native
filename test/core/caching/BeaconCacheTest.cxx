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

#include "../../api/MockILogger.h"
#include "mock/MockIObserver.h"

#include "core/UTF8String.h"
#include "core/caching/BeaconCache.h"
#include "core/caching/BeaconCacheRecord.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <algorithm>

using namespace test;

using BeaconCache_t = core::caching::BeaconCache;
using BeaconCacheRecord_t = core::caching::BeaconCacheRecord;
using MockNiceILogger_sp = std::shared_ptr<testing::NiceMock<MockILogger>>;
using MockNiceIObserver_t = testing::NiceMock<MockIObserver>;
using MockStrictIObserver_t = testing::StrictMock<MockIObserver>;
using Utf8String_t = core::UTF8String;

class BeaconCacheTest : public testing::Test
{
protected:

	MockNiceILogger_sp mockLogger;

	void SetUp()
	{
		mockLogger = MockILogger::createNice();
	}
};

TEST_F(BeaconCacheTest, aDefaultConstructedCacheDoesNotContainBeacons)
{
	// given
	BeaconCache_t target(mockLogger);

	// then
	ASSERT_TRUE(target.getBeaconIDs().empty());
	ASSERT_EQ(target.getNumBytesInCache(), 0L);
}

TEST_F(BeaconCacheTest, addEventDataAddsBeaconIdToCache)
{
	// given
	 BeaconCache_t target(mockLogger);

	// when adding beacon with id 1
	target.addEventData(1, 1000L, "a");

	// then
	ASSERT_EQ(target.getBeaconIDs().size(), 1);
	ASSERT_EQ(target.getBeaconIDs().count(1), 1);
	ASSERT_EQ(target.getEvents(1).size(), 1);
	ASSERT_TRUE(target.getEvents(1).begin()->equals("a"));

	// and when adding beacon with id 2
	target.addEventData(2, 1100L, "b");

	// then
	ASSERT_EQ(target.getBeaconIDs().size(), 2);
	ASSERT_EQ(target.getBeaconIDs().count(1), 1);
	ASSERT_EQ(target.getBeaconIDs().count(2), 1);
	ASSERT_EQ(target.getEvents(1).size(), 1);
	ASSERT_TRUE(target.getEvents(1).begin()->equals("a"));
	ASSERT_EQ(target.getEvents(2).size(), 1);
	ASSERT_TRUE(target.getEvents(2).begin()->equals("b"));
}

TEST_F(BeaconCacheTest, addEventDataAddsDataToAlreadyExistingBeaconId)
{
	// given
	 BeaconCache_t target(mockLogger);

	// when adding beacon with id 1
	target.addEventData(1, 1000L, "a");

	// then
	ASSERT_EQ(target.getBeaconIDs().size(), 1);
	ASSERT_EQ(target.getBeaconIDs().count(1), 1);
	ASSERT_EQ(target.getEvents(1).size(), 1);
	ASSERT_TRUE(target.getEvents(1).begin()->equals("a"));

	// and when adding other data with beacon id 1
	target.addEventData(1, 1100L, "bc");

	// then
	ASSERT_EQ(target.getBeaconIDs().size(), 1);
	ASSERT_EQ(target.getBeaconIDs().count(1), 1);
	ASSERT_EQ(target.getEvents(1).size(), 2);
	auto v = target.getEvents(1);
	ASSERT_TRUE(std::find(v.begin(), v.end(), Utf8String_t("a")) != v.end());
	ASSERT_TRUE(std::find(v.begin(), v.end(), Utf8String_t("bc")) != v.end());
}

TEST_F(BeaconCacheTest, addEventDataIncreasesCacheSize)
{
	// given
	 BeaconCache_t target(mockLogger);

	// when adding some data
	target.addEventData(1, 1000L, "a");
	target.addEventData(42, 1000L, "z");
	target.addEventData(1, 1000L, "iii");

	// then
	ASSERT_EQ(target.getNumBytesInCache(), BeaconCacheRecord_t(1000L, "a").getDataSizeInBytes() +	BeaconCacheRecord_t(1000L, "z").getDataSizeInBytes() + BeaconCacheRecord_t(1000L, "iii").getDataSizeInBytes());
}

TEST_F(BeaconCacheTest, addEventDataNotifiesObserver)
{
	// given
	BeaconCache_t target(mockLogger);
	MockNiceIObserver_t observer;

	target.addObserver(&observer);

	// when adding some data (verify observer got notified)
	EXPECT_CALL(observer, update())
		.Times(testing::Exactly(1));
	target.addEventData(1, 1000L, "a");

	// when adding some more data (verify observer got notified another two times)
	EXPECT_CALL(observer, update())
		.Times(testing::Exactly(2));
	target.addEventData(1, 1100L, "b");
	target.addEventData(666, 1200L, "xyz");
}

TEST_F(BeaconCacheTest, addActionDataAddsBeaconIdToCache)
{
	// given
	BeaconCache_t target(mockLogger);

	// when adding beacon with id 1
	target.addActionData(1, 1000L, "a");

	// then
	ASSERT_EQ(target.getBeaconIDs().size(), 1);
	ASSERT_EQ(target.getBeaconIDs().count(1), 1);
	ASSERT_EQ(target.getActions(1).size(), 1);
	ASSERT_TRUE(target.getActions(1).begin()->equals("a"));

	// and when adding beacon with id 2
	target.addActionData(2, 1100L, "b");

	// then
	ASSERT_EQ(target.getBeaconIDs().size(), 2);
	ASSERT_EQ(target.getBeaconIDs().count(1), 1);
	ASSERT_EQ(target.getBeaconIDs().count(2), 1);
	ASSERT_EQ(target.getActions(1).size(), 1);
	ASSERT_TRUE(target.getActions(1).begin()->equals("a"));
	ASSERT_EQ(target.getActions(2).size(), 1);
	ASSERT_TRUE(target.getActions(2).begin()->equals("b"));
}

TEST_F(BeaconCacheTest, addActionDataAddsDataToAlreadyExistingBeaconId)
{
	// given
	BeaconCache_t target(mockLogger);

	// when adding beacon with id 1
	target.addActionData(1, 1000L, "a");

	// then
	ASSERT_EQ(target.getBeaconIDs().size(), 1);
	ASSERT_EQ(target.getBeaconIDs().count(1), 1);
	ASSERT_EQ(target.getActions(1).size(), 1);
	ASSERT_TRUE(target.getActions(1).begin()->equals("a"));

	// and when adding other data with beacon id 1
	target.addActionData(1, 1100L, "bc");

	// then
	ASSERT_EQ(target.getBeaconIDs().size(), 1);
	ASSERT_EQ(target.getBeaconIDs().count(1), 1);
	ASSERT_EQ(target.getActions(1).size(), 2);
	auto v = target.getActions(1);
	ASSERT_TRUE(std::find(v.begin(), v.end(), Utf8String_t("a")) != v.end());
	ASSERT_TRUE(std::find(v.begin(), v.end(), Utf8String_t("bc")) != v.end());
}

TEST_F(BeaconCacheTest, addActionDataIncreasesCacheSize)
{
	// given
	BeaconCache_t target(mockLogger);

	// when adding some data
	target.addActionData(1, 1000L, "a");
	target.addActionData(42, 1000L, "z");
	target.addActionData(1, 1000L, "iii");

	// then
	ASSERT_EQ(target.getNumBytesInCache(), BeaconCacheRecord_t(1000L, "a").getDataSizeInBytes() + BeaconCacheRecord_t(1000L, "z").getDataSizeInBytes() + BeaconCacheRecord_t(1000L, "iii").getDataSizeInBytes());
}

TEST_F(BeaconCacheTest, addActionDataNotifiesObserver)
{
	// given
	BeaconCache_t target(mockLogger);
	MockNiceIObserver_t observer;

	target.addObserver(&observer);

	// when adding some data (verify observer got notified)
	EXPECT_CALL(observer, update())
		.Times(testing::Exactly(1));
	target.addActionData(1, 1000L, "a");

	// when adding some more data (verify observer got notified another two times)
	EXPECT_CALL(observer, update())
		.Times(testing::Exactly(2));
	target.addActionData(1, 1100L, "b");
	target.addActionData(666, 1200L, "xyz");
}

TEST_F(BeaconCacheTest, deleteCacheEntryRemovesTheGivenBeacon)
{
	// given
	BeaconCache_t target(mockLogger);
	target.addActionData(1, 1000L, "a");
	target.addActionData(42, 1000L, "z");
	target.addEventData(1, 1000L, "iii");

	// when removing beacon with id 1
	target.deleteCacheEntry(1);

	// then
	ASSERT_EQ(target.getBeaconIDs().size(), 1);
	ASSERT_EQ(target.getBeaconIDs().count(42), 1);

	// and when removing beacon with id 42
	target.deleteCacheEntry(42);

	// then
	ASSERT_TRUE(target.getBeaconIDs().empty());
}

TEST_F(BeaconCacheTest, deleteCacheEntryDecrementsCacheSize)
{
	// given
	BeaconCache_t target(mockLogger);
	target.addActionData(1, 1000L, "a");
	target.addActionData(42, 1000L, "z");
	target.addEventData(1, 1000L, "iii");

	// when removing beacon with id 42
	target.deleteCacheEntry(42);

	// then
	ASSERT_EQ(target.getNumBytesInCache(), BeaconCacheRecord_t(1000L, "a").getDataSizeInBytes() + BeaconCacheRecord_t(1000L, "iii").getDataSizeInBytes());
}

TEST_F(BeaconCacheTest, deleteCacheEntryDoesNotNotifyObservers)
{
	// given
	BeaconCache_t target(mockLogger);
	target.addActionData(1, 1000L, "a");
	target.addActionData(42, 1000L, "z");
	target.addEventData(1, 1000L, "iii");
	MockStrictIObserver_t observer;

	target.addObserver(&observer);

	// when deleting both entries
	target.deleteCacheEntry(1);
	target.deleteCacheEntry(42);

	// then (StrictMock usage verifies that no methods of the mocked object are called)
}

TEST_F(BeaconCacheTest, deleteCacheEntriesDoesNothingIfGivenBeaconIDIsNotInCache)
{
	// given
	BeaconCache_t target(mockLogger);
	target.addActionData(1, 1000L, "a");
	target.addActionData(42, 1000L, "z");
	target.addEventData(1, 1000L, "iii");
	MockStrictIObserver_t observer;

	target.addObserver(&observer);

	auto cachedSize = target.getNumBytesInCache();

	// when
	target.deleteCacheEntry(666);

	// then
	ASSERT_EQ(target.getBeaconIDs().size(), 2);
	ASSERT_EQ(target.getBeaconIDs().count(1), 1);
	ASSERT_EQ(target.getBeaconIDs().count(42), 1);
	ASSERT_EQ(target.getNumBytesInCache(), cachedSize);
}

TEST_F(BeaconCacheTest, getNextBeaconChunkReturnsNullIfGivenBeaconIDDoesNotExist)
{
	// given
	BeaconCache_t target(mockLogger);
	target.addActionData(1, 1000L, "a");
	target.addActionData(42, 1000L, "z");
	target.addEventData(1, 1000L, "iii");

	// when
	auto obtained = target.getNextBeaconChunk(666, "", 1024, "&");

	// then
	ASSERT_TRUE(obtained.empty());
}

TEST_F(BeaconCacheTest, getNextBeaconChunkCopiesDataForSending)
{
	// given
	BeaconCache_t target(mockLogger);
	target.addActionData(1, 1000L, "a");
	target.addActionData(1, 1001L, "iii");
	target.addActionData(42, 2000L, "z");
	target.addEventData(1, 1000L, "b");
	target.addEventData(1, 1001L, "jjj");

	// when
	auto obtained = target.getNextBeaconChunk(1, "prefix", 0, "&");

	// then
	ASSERT_TRUE(obtained.equals("prefix"));

	ASSERT_TRUE(target.getActions(1).empty());
	ASSERT_TRUE(target.getEvents(1).empty());
	auto v = target.getActionsBeingSent(1);
	ASSERT_EQ(v.size(), 2);
	auto it = v.begin();
	ASSERT_TRUE(it->getData().equals("a"));
	it++;
	ASSERT_TRUE(it->getData().equals("iii"));

	auto v2 = target.getEventsBeingSent(1);
	ASSERT_EQ(v2.size(), 2);
	auto it2 = v2.begin();
	ASSERT_TRUE(it2->getData().equals("b"));
	it2++;
	ASSERT_TRUE(it2->getData().equals("jjj"));
}

TEST_F(BeaconCacheTest, getNextBeaconChunkDecreasesBeaconCacheSize)
{
	// given
	BeaconCache_t target(mockLogger);
	target.addActionData(1, 1000L, "a");
	target.addActionData(1, 1001L, "iii");
	target.addActionData(42, 2000L, "z");
	target.addEventData(1, 1000L, "b");
	target.addEventData(1, 1001L, "jjj");

	// when
	target.getNextBeaconChunk(1, "prefix", 0, "&");

	// cache stats are also adjusted
	ASSERT_EQ(target.getNumBytesInCache(), BeaconCacheRecord_t(2000L, "z").getDataSizeInBytes());
}

TEST_F(BeaconCacheTest, getNextBeaconChunkRetrievesNextChunk)
{
	// given
	BeaconCache_t target(mockLogger);
	target.addActionData(1, 1000L, "a");
	target.addActionData(1, 1001L, "iii");
	target.addActionData(42, 2000L, "z");
	target.addEventData(1, 1000L, "b");
	target.addEventData(1, 1001L, "jjj");

	// when
	auto obtained = target.getNextBeaconChunk(1, "prefix", 10, "&");

	// then
	ASSERT_TRUE(obtained.equals("prefix&b&jjj"));

	// then
	auto v = target.getActionsBeingSent(1);
	ASSERT_EQ(v.size(), 2);
	auto it = v.begin();
	ASSERT_TRUE(it->getData().equals("a"));
	it++;
	ASSERT_TRUE(it->getData().equals("iii"));
	ASSERT_EQ(target.getEventsBeingSent(1).size(), 2);

	auto v2 = target.getEventsBeingSent(1);
	auto it2 = v2.begin();
	ASSERT_TRUE(it2->getData().equals("b"));
	ASSERT_TRUE(it2->isMarkedForSending());
	it2++;
	ASSERT_TRUE(it2->getData().equals("jjj"));
	ASSERT_TRUE(it2->isMarkedForSending());
}

TEST_F(BeaconCacheTest, removeChunkedDataClearsAlreadyRetrievedChunks)
{
	// given
	BeaconCache_t target(mockLogger);
	target.addActionData(1, 1000L, "a");
	target.addActionData(1, 1001L, "iii");
	target.addActionData(42, 2000L, "z");
	target.addEventData(1, 1000L, "b");
	target.addEventData(1, 1001L, "jjj");

	// when retrieving the first chunk and removing retrieved chunks
	auto obtained = target.getNextBeaconChunk(1, "prefix", 10, "&");
	target.removeChunkedData(1);

	// then
	ASSERT_TRUE(obtained.equals("prefix&b&jjj"));

	// then
	auto v = target.getActionsBeingSent(1);
	ASSERT_EQ(v.size(), 2);
	auto it = v.begin();
	ASSERT_TRUE(it->getData().equals("a"));
	it++;
	ASSERT_TRUE(it->getData().equals("iii"));
	ASSERT_TRUE(target.getEventsBeingSent(1).empty());

	// when retrieving the second chunk and removing retrieved chunks
	obtained = target.getNextBeaconChunk(1, "prefix", 10, "&");
	target.removeChunkedData(1);

	// then
	ASSERT_TRUE(obtained.equals("prefix&a&iii"));

	ASSERT_TRUE(target.getActionsBeingSent(1).empty());
	ASSERT_TRUE(target.getEventsBeingSent(1).empty());
}

TEST_F(BeaconCacheTest, removeChunkedDataDoesNothingIfCalledWithNonExistingBeaconID)
{
	// given
	BeaconCache_t target(mockLogger);
	target.addActionData(1, 1000L, "a");
	target.addActionData(1, 1001L, "iii");
	target.addActionData(42, 2000L, "z");
	target.addEventData(1, 1000L, "b");
	target.addEventData(1, 1001L, "jjj");

	// when retrieving the first chunk and removing retrieved chunks
	auto obtained = target.getNextBeaconChunk(1, "prefix", 10, "&");
	target.removeChunkedData(2);

	// then
	auto v = target.getActionsBeingSent(1);
	ASSERT_EQ(v.size(), 2);
	auto it = v.begin();
	ASSERT_TRUE(it->getData().equals("a"));
	it++;
	ASSERT_TRUE(it->getData().equals("iii"));
	ASSERT_EQ(target.getEventsBeingSent(1).size(), 2);

	auto v2 = target.getEventsBeingSent(1);
	auto it2 = v2.begin();
	ASSERT_TRUE(it2->getData().equals("b"));
	ASSERT_TRUE(it2->isMarkedForSending());
	it2++;
	ASSERT_TRUE(it2->getData().equals("jjj"));
	ASSERT_TRUE(it2->isMarkedForSending());
}

TEST_F(BeaconCacheTest, resetChunkedRestoresData)
{
	// given
	BeaconCache_t target(mockLogger);
	target.addActionData(1, 1000L, "a");
	target.addActionData(1, 1001L, "iii");
	target.addEventData(1, 1000L, "b");
	target.addEventData(1, 1001L, "jjj");

	// do same step we'd do when we send the
	target.getNextBeaconChunk(1, "prefix", 10, "&");

	// data has been copied, but still add some new event & action data
	target.addActionData(1, 6666L, "123");
	target.addEventData(1, 6666L, "987");

	// and when resetting the previously copied data
	target.resetChunkedData(1);

	// then
	ASSERT_TRUE(target.getActionsBeingSent(1).empty());
	ASSERT_TRUE(target.getEventsBeingSent(1).empty());
	auto v = target.getActions(1);
	ASSERT_EQ(v.size(), 3);
	auto it = v.begin();
	ASSERT_TRUE(it->equals("a"));
	it++;
	ASSERT_TRUE(it->equals("iii"));
	it++;
	ASSERT_TRUE(it->equals("123"));

	auto v2 = target.getEvents(1);
	ASSERT_EQ(v2.size(), 3);
	auto it2 = v2.begin();
	ASSERT_TRUE(it2->equals("b"));
	it2++;
	ASSERT_TRUE(it2->equals("jjj"));
	it2++;
	ASSERT_TRUE(it2->equals("987"));
}

TEST_F(BeaconCacheTest, resetChunkedRestoresCacheSize)
{
	// given
	BeaconCache_t target(mockLogger);
	target.addActionData(1, 1000L, "a");
	target.addActionData(1, 1001L, "iii");
	target.addEventData(1, 1000L, "b");
	target.addEventData(1, 1001L, "jjj");

	// do same step we'd do when we send the
	target.getNextBeaconChunk(1, "prefix", 10, "&");

	// data has been copied, but still add some new event & action data
	target.addActionData(1, 6666L, "123");
	target.addEventData(1, 6666L, "987");

	// and when resetting the previously copied data
	target.resetChunkedData(1);

	// then
	ASSERT_EQ(target.getNumBytesInCache(), 14L); // aiiibjjj123456
}

TEST_F(BeaconCacheTest, resetChunkedNotifiesObservers)
{
	// given
	BeaconCache_t target(mockLogger);
	target.addActionData(1, 1000L, "a");
	target.addActionData(1, 1001L, "iii");
	target.addEventData(1, 1000L, "b");
	target.addEventData(1, 1001L, "jjj");

	// do same step we'd do when we send the
	target.getNextBeaconChunk(1, "prefix", 10, "&");

	// data has been copied, but still add some new event & action data
	target.addActionData(1, 6666L, "123");
	target.addEventData(1, 6666L, "987");

	MockNiceIObserver_t observer;
	target.addObserver(&observer);

	// and when resetting the previously copied data
	EXPECT_CALL(observer, update())
		.Times(testing::Exactly(1));
	target.resetChunkedData(1);
}

TEST_F(BeaconCacheTest, resetChunkedDoesNothingIfEntryDoesNotExist)
{
	// given
	BeaconCache_t target(mockLogger);
	target.addActionData(1, 1000L, "a");
	target.addActionData(1, 1001L, "iii");
	target.addEventData(1, 1000L, "b");
	target.addEventData(1, 1001L, "jjj");

	// do same step we'd do when we send the
	target.getNextBeaconChunk(1, "prefix", 10, "&");

	// data has been copied, but still add some new event & action data
	target.addActionData(1, 6666L, "123");
	target.addEventData(1, 6666L, "987");

	MockStrictIObserver_t observer;
	target.addObserver(&observer);

	// resetting not existing data
	target.resetChunkedData(666);

	// then (StrictMock usage verifies that no methods of the mocked object are called)
	ASSERT_EQ(target.getNumBytesInCache(), 6L);	// 123987
}

TEST_F(BeaconCacheTest, evictRecordsByAgeDoesNothingAndReturnsZeroIfBeaconIDDoesNotExist)
{
	// given
	BeaconCache_t target(mockLogger);
	target.addActionData(1, 1000L, "a");
	target.addActionData(1, 1001L, "iii");
	target.addEventData(1, 1000L, "b");
	target.addEventData(1, 1001L, "jjj");

	// when
	auto obtained = target.evictRecordsByAge(666, 0);

	// then
	ASSERT_EQ(obtained, 0);
}

TEST_F(BeaconCacheTest, evictRecordsByAge)
{
	// given
	BeaconCache_t target(mockLogger);
	target.addActionData(1, 1000L, "a");
	target.addActionData(1, 1001L, "iii");
	target.addEventData(1, 1000L, "b");
	target.addEventData(1, 1001L, "jjj");

	// when
	auto obtained = target.evictRecordsByAge(1, 1001);

	// then
	ASSERT_EQ(obtained, 2);
}

TEST_F(BeaconCacheTest, evictRecordsByNumberDoesNothingAndReturnsZeroIfBeaconIDDoesNotExist)
{
	// given
	BeaconCache_t target(mockLogger);
	target.addActionData(1, 1000L, "a");
	target.addActionData(1, 1001L, "iii");
	target.addEventData(1, 1000L, "b");
	target.addEventData(1, 1001L, "jjj");

	// when
	auto obtained = target.evictRecordsByNumber(666, 100);

	// then
	ASSERT_EQ(obtained, 0);
}

TEST_F(BeaconCacheTest, evictRecordsByNumber)
{
	// given
	BeaconCache_t target(mockLogger);
	target.addActionData(1, 1000L, "a");
	target.addActionData(1, 1001L, "iii");
	target.addEventData(1, 1000L, "b");
	target.addEventData(1, 1001L, "jjj");

	// when
	auto obtained = target.evictRecordsByNumber(1, 2);

	// then
	ASSERT_EQ(obtained, 2);
}

TEST_F(BeaconCacheTest, isEmptyGivesTrueIfBeaconDoesNotExistInCache)
{
	// given
	BeaconCache_t target(mockLogger);
	target.addActionData(1, 1000L, "a");
	target.addActionData(1, 1001L, "iii");
	target.addEventData(1, 1000L, "b");
	target.addEventData(1, 1001L, "jjj");

	// then
	ASSERT_TRUE(target.isEmpty(666));
}

TEST_F(BeaconCacheTest, isEmptyGivesFalseIfBeaconDataSizeIsNotEqualToZero)
{
	// given
	BeaconCache_t target(mockLogger);
	target.addActionData(1, 1000L, "a");
	target.addEventData(1, 1000L, "b");

	// then
	ASSERT_FALSE(target.isEmpty(1));
}

TEST_F(BeaconCacheTest, isEmptyGivesTrueIfBeaconDoesNotContainActiveData)
{
	// given
	BeaconCache_t target(mockLogger);
	target.addActionData(1, 1000L, "a");
	target.addEventData(1, 1000L, "b");

	target.getNextBeaconChunk(1, "prefix", 0, "&");

	// then
	ASSERT_TRUE(target.isEmpty(1));
}

