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

#include "../../api/mock/MockILogger.h"
#include "mock/MockIObserver.h"

#include "core/UTF8String.h"
#include "core/caching/BeaconCache.h"
#include "core/caching/BeaconCacheRecord.h"
#include "core/caching/BeaconKey.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <algorithm>

using namespace test;

using BeaconCache_t = core::caching::BeaconCache;
using BeaconCacheRecord_t = core::caching::BeaconCacheRecord;
using BeaconKey_t = core::caching::BeaconKey;
using BeaconKeySet_t = std::unordered_set<BeaconKey_t, BeaconKey_t::Hash>;
using MockNiceILogger_sp = std::shared_ptr<testing::NiceMock<MockILogger>>;
using MockNiceIObserver_t = testing::NiceMock<MockIObserver>;
using MockStrictIObserver_t = testing::StrictMock<MockIObserver>;
using Utf8String_t = core::UTF8String;

class BeaconCacheTest : public testing::Test
{
protected:

	MockNiceILogger_sp mockLogger;

	void SetUp() override
	{
		mockLogger = MockILogger::createNice();
	}
};

TEST_F(BeaconCacheTest, aDefaultConstructedCacheDoesNotContainBeacons)
{
	// given
	BeaconCache_t target(mockLogger);

	// then
	ASSERT_THAT(target.getBeaconKeys(), testing::IsEmpty());
	ASSERT_THAT(target.getNumBytesInCache(), testing::Eq(0));
}

TEST_F(BeaconCacheTest, addEventDataAddsBeaconKeyToCache)
{
	// given
	BeaconKey_t keyOne(1, 0);
	BeaconKey_t keyTwo(2, 0);

	 BeaconCache_t target(mockLogger);

	// when adding beacon with key 1
	target.addEventData(keyOne, 1000L, "a");

	// then
	ASSERT_THAT(target.getBeaconKeys(), testing::ContainerEq(BeaconKeySet_t{keyOne}));
	ASSERT_THAT(target.getEvents(keyOne), testing::ContainerEq(std::vector<core::UTF8String>{"a"}));

	// and when adding beacon with key 2
	target.addEventData(keyTwo, 1100L, "b");

	// then
	ASSERT_THAT(target.getBeaconKeys(), testing::ContainerEq(BeaconKeySet_t{keyOne, keyTwo}));
	ASSERT_THAT(target.getEvents(keyOne), testing::ContainerEq(std::vector<core::UTF8String>{"a"}));
	ASSERT_THAT(target.getEvents(keyTwo), testing::ContainerEq(std::vector<core::UTF8String>{"b"}));
}

TEST_F(BeaconCacheTest, addEventDataAddsDataToAlreadyExistingBeaconKey)
{
	// given
	BeaconKey_t key(1, 0);

	 BeaconCache_t target(mockLogger);

	// when adding beacon with key
	target.addEventData(key, 1000L, "a");

	// then
	ASSERT_THAT(target.getBeaconKeys(), testing::ContainerEq(BeaconKeySet_t{key}));
	ASSERT_THAT(target.getEvents(key), testing::ContainerEq(std::vector<core::UTF8String>{"a"}));

	// and when adding other data with beacon key
	target.addEventData(key, 1100L, "bc");

	// then
	ASSERT_THAT(target.getBeaconKeys(), testing::ContainerEq(BeaconKeySet_t{key}));
	ASSERT_THAT(target.getEvents(key), testing::ContainerEq(std::vector<core::UTF8String>{"a", "bc"}));
}

TEST_F(BeaconCacheTest, addEventDataIncreasesCacheSize)
{
	// given
	BeaconKey_t keyOne(1, 0);
	BeaconKey_t keyTwo(42, 0);
	BeaconCache_t target(mockLogger);

	// when adding some data
	target.addEventData(keyOne, 1000L, "a");
	target.addEventData(keyTwo, 1000L, "z");
	target.addEventData(keyOne, 1000L, "iii");

	// then
	ASSERT_THAT(target.getNumBytesInCache(), 
		testing::Eq(BeaconCacheRecord_t(1000L, "a").getDataSizeInBytes() +	BeaconCacheRecord_t(1000L, "z").getDataSizeInBytes() + BeaconCacheRecord_t(1000L, "iii").getDataSizeInBytes()));
}

TEST_F(BeaconCacheTest, addEventDataNotifiesObserver)
{
	// given
	BeaconKey_t keyOne(1, 0);
	BeaconKey_t keyTwo(666, 0);
	BeaconCache_t target(mockLogger);
	MockNiceIObserver_t observer;

	target.addObserver(&observer);

	// when adding some data (verify observer got notified)
	EXPECT_CALL(observer, update())
		.Times(testing::Exactly(1));
	target.addEventData(keyOne, 1000L, "a");

	// when adding some more data (verify observer got notified another two times)
	EXPECT_CALL(observer, update())
		.Times(testing::Exactly(2));

	target.addEventData(keyOne, 1100L, "b");
	target.addEventData(keyTwo, 1200L, "xyz");
}

TEST_F(BeaconCacheTest, addActionDataAddsBeaconKeyToCache)
{
	// given
	BeaconKey_t keyOne(1, 0);
	BeaconKey_t keyTwo(2, 0);
	BeaconCache_t target(mockLogger);

	// when adding beacon with key 1
	target.addActionData(keyOne, 1000L, "a");

	// then
	ASSERT_THAT(target.getBeaconKeys(), testing::ContainerEq(BeaconKeySet_t{keyOne}));
	ASSERT_THAT(target.getActions(keyOne), testing::ContainerEq(std::vector<core::UTF8String>{"a"}));

	// and when adding beacon with key 2
	target.addActionData(keyTwo, 1100L, "b");

	// then
	ASSERT_THAT(target.getBeaconKeys(), testing::ContainerEq(BeaconKeySet_t{keyOne, keyTwo}));
	ASSERT_THAT(target.getActions(keyOne), testing::ContainerEq(std::vector<core::UTF8String>{"a"}));
	ASSERT_THAT(target.getActions(keyTwo), testing::ContainerEq(std::vector<core::UTF8String>{"b"}));
}

TEST_F(BeaconCacheTest, addActionDataAddsDataToAlreadyExistingBeaconKey)
{
	// given
	BeaconKey_t key(1, 0);
	BeaconCache_t target(mockLogger);

	// when adding beacon with key
	target.addActionData(key, 1000L, "a");

	// then
	ASSERT_THAT(target.getBeaconKeys(), testing::ContainerEq(BeaconKeySet_t{key}));
	ASSERT_THAT(target.getActions(key), testing::ContainerEq(std::vector<core::UTF8String>{"a"}));

	// and when adding other data with beacon key
	target.addActionData(key, 1100L, "bc");

	// then
	ASSERT_THAT(target.getBeaconKeys(), testing::ContainerEq(BeaconKeySet_t{key}));
	ASSERT_THAT(target.getActions(key), testing::ContainerEq(std::vector<core::UTF8String>{"a", "bc"}));
}

TEST_F(BeaconCacheTest, addActionDataIncreasesCacheSize)
{
	// given
	BeaconKey_t keyOne(1, 0);
	BeaconKey_t keyTwo(42, 0);
	BeaconCache_t target(mockLogger);

	// when adding some data
	target.addActionData(keyOne, 1000L, "a");
	target.addActionData(keyTwo, 1000L, "z");
	target.addActionData(keyOne, 1000L, "iii");

	// then
	ASSERT_THAT(target.getNumBytesInCache(), 
		testing::Eq(BeaconCacheRecord_t(1000L, "a").getDataSizeInBytes() + BeaconCacheRecord_t(1000L, "z").getDataSizeInBytes() + BeaconCacheRecord_t(1000L, "iii").getDataSizeInBytes()));
}

TEST_F(BeaconCacheTest, addActionDataNotifiesObserver)
{
	// given
	BeaconKey_t keyOne(1, 0);
	BeaconKey_t keyTwo(666, 0);
	BeaconCache_t target(mockLogger);
	MockNiceIObserver_t observer;

	target.addObserver(&observer);

	// when adding some data (verify observer got notified)
	EXPECT_CALL(observer, update())
		.Times(testing::Exactly(1));
	target.addActionData(keyOne, 1000L, "a");

	// when adding some more data (verify observer got notified another two times)
	EXPECT_CALL(observer, update())
		.Times(testing::Exactly(2));
	target.addActionData(keyOne, 1100L, "b");
	target.addActionData(keyTwo, 1200L, "xyz");
}

TEST_F(BeaconCacheTest, deleteCacheEntryRemovesTheGivenBeacon)
{
	// given
	BeaconKey_t keyOne(1, 0);
	BeaconKey_t keyTwo(42, 0);
	BeaconCache_t target(mockLogger);
	target.addActionData(keyOne, 1000L, "a");
	target.addActionData(keyTwo, 1000L, "z");
	target.addEventData(keyOne, 1000L, "iii");

	// when removing beacon with id 1
	target.deleteCacheEntry(keyOne);

	// then
	ASSERT_THAT(target.getBeaconKeys(), testing::ContainerEq(BeaconKeySet_t{keyTwo}));

	// and when removing beacon with id 42
	target.deleteCacheEntry(keyTwo);

	// then
	ASSERT_THAT(target.getBeaconKeys(), testing::IsEmpty());
}

TEST_F(BeaconCacheTest, deleteCacheEntryDecrementsCacheSize)
{
	// given
	BeaconKey_t keyOne(1, 0);
	BeaconKey_t keyTwo(42, 0);
	BeaconCache_t target(mockLogger);
	target.addActionData(keyOne, 1000L, "a");
	target.addActionData(keyTwo, 1000L, "z");
	target.addEventData(keyOne, 1000L, "iii");

	// when removing beacon with id 42
	target.deleteCacheEntry(keyTwo);

	// then
	ASSERT_THAT(target.getNumBytesInCache(),
		testing::Eq(BeaconCacheRecord_t(1000L, "a").getDataSizeInBytes() + BeaconCacheRecord_t(1000L, "iii").getDataSizeInBytes()));
}

TEST_F(BeaconCacheTest, deleteCacheEntryDoesNotNotifyObservers)
{
	// given
	BeaconKey_t keyOne(1, 0);
	BeaconKey_t keyTwo(42, 0);
	BeaconCache_t target(mockLogger);
	target.addActionData(keyOne, 1000L, "a");
	target.addActionData(keyTwo, 1000L, "z");
	target.addEventData(keyOne, 1000L, "iii");

	MockStrictIObserver_t observer;
	EXPECT_CALL(observer, update())
		.Times(testing::Exactly(0));

	target.addObserver(&observer);

	// when deleting both entries
	target.deleteCacheEntry(keyOne);
	target.deleteCacheEntry(keyTwo);
}

TEST_F(BeaconCacheTest, deleteCacheEntriesDoesNothingIfGivenBeaconKeyIsNotInCache)
{
	// given
	BeaconKey_t keyOne(1, 0);
	BeaconKey_t keyTwo(42, 0);
	BeaconCache_t target(mockLogger);
	target.addActionData(keyOne, 1000L, "a");
	target.addActionData(keyTwo, 1000L, "z");
	target.addEventData(keyOne, 1000L, "iii");
	MockStrictIObserver_t observer;

	target.addObserver(&observer);

	auto cachedSize = target.getNumBytesInCache();

	// when
	target.deleteCacheEntry(BeaconKey_t(666, 0));

	// then
	ASSERT_THAT(target.getBeaconKeys(), testing::ContainerEq(BeaconKeySet_t{keyOne, keyTwo}));
	ASSERT_THAT(target.getNumBytesInCache(), testing::Eq(cachedSize));
}

TEST_F(BeaconCacheTest, getNextBeaconChunkReturnsNullIfGivenBeaconKeyDoesNotExist)
{
	// given
	BeaconKey_t keyOne(1, 0);
	BeaconKey_t keyTwo(42, 0);
	BeaconCache_t target(mockLogger);
	target.addActionData(keyOne, 1000L, "a");
	target.addActionData(keyTwo, 1000L, "z");
	target.addEventData(keyOne, 1000L, "iii");

	// when
	auto obtained = target.getNextBeaconChunk(BeaconKey_t(666, 0), "", 1024, "&");

	// then
	ASSERT_THAT(obtained.empty(), testing::Eq(true));
}

TEST_F(BeaconCacheTest, getNextBeaconChunkCopiesDataForSending)
{
	// given
	BeaconKey_t keyOne(1, 0);
	BeaconKey_t keyTwo(42, 0);
	BeaconCache_t target(mockLogger);
	target.addActionData(keyOne, 1000L, "a");
	target.addActionData(keyOne, 1001L, "iii");
	target.addActionData(keyTwo, 2000L, "z");
	target.addEventData(keyOne, 1000L, "b");
	target.addEventData(keyOne, 1001L, "jjj");

	// when
	auto obtained = target.getNextBeaconChunk(keyOne, "prefix", 0, "&");

	// then
	ASSERT_THAT(obtained, testing::Eq(Utf8String_t("prefix")));

	ASSERT_THAT(target.getActions(keyOne), testing::IsEmpty());
	ASSERT_THAT(target.getEvents(keyOne), testing::IsEmpty());
	ASSERT_THAT(target.getActionsBeingSent(keyOne), testing::ContainerEq(
		std::list<BeaconCacheRecord_t>{
			BeaconCacheRecord_t(1000L, "a"),
			BeaconCacheRecord_t(1001L, "iii")
		}
	));
	ASSERT_THAT(target.getEventsBeingSent(keyOne), testing::ContainerEq(
		std::list<BeaconCacheRecord_t>{
			BeaconCacheRecord_t(1000L, "b"),
			BeaconCacheRecord_t(1001L, "jjj")
		}
	));
}

TEST_F(BeaconCacheTest, getNextBeaconChunkDecreasesBeaconCacheSize)
{
	// given
	BeaconKey_t keyOne(1, 0);
	BeaconKey_t keyTwo(42, 0);
	BeaconCache_t target(mockLogger);
	target.addActionData(keyOne, 1000L, "a");
	target.addActionData(keyOne, 1001L, "iii");
	target.addActionData(keyTwo, 2000L, "z");
	target.addEventData(keyOne, 1000L, "b");
	target.addEventData(keyOne, 1001L, "jjj");

	// when
	target.getNextBeaconChunk(keyOne, "prefix", 0, "&");

	// cache stats are also adjusted
	ASSERT_THAT(target.getNumBytesInCache(),
		testing::Eq(BeaconCacheRecord_t(2000L, "z").getDataSizeInBytes()));
}

TEST_F(BeaconCacheTest, getNextBeaconChunkRetrievesNextChunk)
{
	// given
	BeaconKey_t keyOne(1, 0);
	BeaconKey_t keyTwo(42, 0);
	BeaconCache_t target(mockLogger);
	target.addActionData(keyOne, 1000L, "a");
	target.addActionData(keyOne, 1001L, "iii");
	target.addActionData(keyTwo, 2000L, "z");
	target.addEventData(keyOne, 1000L, "b");
	target.addEventData(keyOne, 1001L, "jjj");

	// when
	auto obtained = target.getNextBeaconChunk(keyOne, "prefix", 10, "&");

	// then
	ASSERT_THAT(obtained, testing::Eq(Utf8String_t("prefix&b&jjj")));

	// then
	ASSERT_THAT(target.getActionsBeingSent(keyOne), testing::ContainerEq(
		std::list<BeaconCacheRecord_t>{
			BeaconCacheRecord_t(1000L, "a"),
			BeaconCacheRecord_t(1001L, "iii")
		}
	));

	BeaconCacheRecord_t expectedOne(1000L, "b");
	BeaconCacheRecord_t expectedTwo(1001L, "jjj");
	expectedOne.markForSending();
	expectedTwo.markForSending();
	ASSERT_THAT(target.getEventsBeingSent(keyOne), testing::ContainerEq(
		std::list<BeaconCacheRecord_t>{
			expectedOne, expectedTwo
		}
	));
}

TEST_F(BeaconCacheTest, removeChunkedDataClearsAlreadyRetrievedChunks)
{
	// given
	BeaconKey_t keyOne(1, 0);
	BeaconKey_t keyTwo(42, 0);
	BeaconCache_t target(mockLogger);
	target.addActionData(keyOne, 1000L, "a");
	target.addActionData(keyOne, 1001L, "iii");
	target.addActionData(keyTwo, 2000L, "z");
	target.addEventData(keyOne, 1000L, "b");
	target.addEventData(keyOne, 1001L, "jjj");

	// when retrieving the first chunk and removing retrieved chunks
	auto obtained = target.getNextBeaconChunk(keyOne, "prefix", 10, "&");
	target.removeChunkedData(keyOne);

	// then
	ASSERT_THAT(obtained, testing::Eq(Utf8String_t("prefix&b&jjj")));

	// then
		// then
	ASSERT_THAT(target.getActionsBeingSent(keyOne), testing::ContainerEq(
		std::list<BeaconCacheRecord_t>{
			BeaconCacheRecord_t(1000L, "a"),
			BeaconCacheRecord_t(1001L, "iii")
		}
	));
	ASSERT_THAT(target.getEventsBeingSent(keyOne), testing::IsEmpty());

	// when retrieving the second chunk and removing retrieved chunks
	obtained = target.getNextBeaconChunk(keyOne, "prefix", 10, "&");
	target.removeChunkedData(keyOne);

	// then
	ASSERT_THAT(obtained, testing::Eq(Utf8String_t("prefix&a&iii")));

	ASSERT_THAT(target.getActionsBeingSent(keyOne), testing::IsEmpty());
	ASSERT_THAT(target.getEventsBeingSent(keyOne), testing::IsEmpty());
}

TEST_F(BeaconCacheTest, removeChunkedDataDoesNothingIfCalledWithNonExistingBeaconKey)
{
	// given
	BeaconKey_t keyOne(1, 0);
	BeaconKey_t keyTwo(42, 0);
	BeaconCache_t target(mockLogger);
	target.addActionData(keyOne, 1000L, "a");
	target.addActionData(keyOne, 1001L, "iii");
	target.addActionData(keyTwo, 2000L, "z");
	target.addEventData(keyOne, 1000L, "b");
	target.addEventData(keyOne, 1001L, "jjj");

	// when retrieving the first chunk and removing retrieved chunks
	auto obtained = target.getNextBeaconChunk(keyOne, "prefix", 10, "&");
	target.removeChunkedData(keyTwo);

	// then
	ASSERT_THAT(target.getActionsBeingSent(keyOne), testing::ContainerEq(
		std::list<BeaconCacheRecord_t>{
			BeaconCacheRecord_t(1000L, "a"),
			BeaconCacheRecord_t(1001L, "iii")
		}
	));

	BeaconCacheRecord_t expectedOne(1000L, "b");
	BeaconCacheRecord_t expectedTwo(1001L, "jjj");
	expectedOne.markForSending();
	expectedTwo.markForSending();
	ASSERT_THAT(target.getEventsBeingSent(keyOne), testing::ContainerEq(
		std::list<BeaconCacheRecord_t>{
			expectedOne, expectedTwo
		}
	));
}

TEST_F(BeaconCacheTest, resetChunkedRestoresData)
{
	// given
	BeaconKey_t key(1, 0);
	BeaconCache_t target(mockLogger);
	target.addActionData(key, 1000L, "a");
	target.addActionData(key, 1001L, "iii");
	target.addEventData(key, 1000L, "b");
	target.addEventData(key, 1001L, "jjj");

	// do same step we'd do when we send the
	target.getNextBeaconChunk(key, "prefix", 10, "&");

	// data has been copied, but still add some new event & action data
	target.addActionData(key, 6666L, "123");
	target.addEventData(key, 6666L, "987");

	// and when resetting the previously copied data
	target.resetChunkedData(key);

	// then
	ASSERT_THAT(target.getActionsBeingSent(key), testing::IsEmpty());
	ASSERT_THAT(target.getEventsBeingSent(key), testing::IsEmpty());
	ASSERT_THAT(target.getActions(key), testing::ContainerEq(
		std::vector<Utf8String_t>{ "a", "iii", "123" }));
	ASSERT_THAT(target.getEvents(key), testing::ContainerEq(
		std::vector<Utf8String_t>{ "b", "jjj", "987" }));
}

TEST_F(BeaconCacheTest, resetChunkedRestoresCacheSize)
{
	// given
	BeaconKey_t key(1, 0);
	BeaconCache_t target(mockLogger);
	target.addActionData(key, 1000L, "a");
	target.addActionData(key, 1001L, "iii");
	target.addEventData(key, 1000L, "b");
	target.addEventData(key, 1001L, "jjj");

	// do same step we'd do when we send the
	target.getNextBeaconChunk(key, "prefix", 10, "&");

	// data has been copied, but still add some new event & action data
	target.addActionData(key, 6666L, "123");
	target.addEventData(key, 6666L, "987");

	// and when resetting the previously copied data
	target.resetChunkedData(key);

	// then
	ASSERT_THAT(target.getNumBytesInCache(), testing::Eq(14L)); // aiiibjjj123987
}

TEST_F(BeaconCacheTest, resetChunkedNotifiesObservers)
{
	// given
	BeaconKey_t key(1, 0);
	BeaconCache_t target(mockLogger);
	target.addActionData(key, 1000L, "a");
	target.addActionData(key, 1001L, "iii");
	target.addEventData(key, 1000L, "b");
	target.addEventData(key, 1001L, "jjj");

	// do same step we'd do when we send the
	target.getNextBeaconChunk(key, "prefix", 10, "&");

	// data has been copied, but still add some new event & action data
	target.addActionData(key, 6666L, "123");
	target.addEventData(key, 6666L, "987");

	MockNiceIObserver_t observer;
	target.addObserver(&observer);

	// and when resetting the previously copied data
	EXPECT_CALL(observer, update())
		.Times(testing::Exactly(1));
	target.resetChunkedData(key);
}

TEST_F(BeaconCacheTest, resetChunkedDoesNothingIfEntryDoesNotExist)
{
	// given
	BeaconKey_t key(1, 0);
	BeaconCache_t target(mockLogger);
	target.addActionData(key, 1000L, "a");
	target.addActionData(key, 1001L, "iii");
	target.addEventData(key, 1000L, "b");
	target.addEventData(key, 1001L, "jjj");

	// do same step we'd do when we send the
	target.getNextBeaconChunk(key, "prefix", 10, "&");

	// data has been copied, but still add some new event & action data
	target.addActionData(key, 6666L, "123");
	target.addEventData(key, 6666L, "987");

	MockStrictIObserver_t observer;
	target.addObserver(&observer);

	// resetting not existing data
	target.resetChunkedData(BeaconKey_t(666, 0));

	// then (StrictMock usage verifies that no methods of the mocked object are called)
	ASSERT_THAT(target.getNumBytesInCache(), testing::Eq(6L));	// 123987
}

TEST_F(BeaconCacheTest, evictRecordsByAgeDoesNothingAndReturnsZeroIfBeaconIDDoesNotExist)
{
	// given
	BeaconKey_t key(1, 0);
	BeaconCache_t target(mockLogger);
	target.addActionData(key, 1000L, "a");
	target.addActionData(key, 1001L, "iii");
	target.addEventData(key, 1000L, "b");
	target.addEventData(key, 1001L, "jjj");

	// when
	auto obtained = target.evictRecordsByAge(BeaconKey_t(666, 0), 0);

	// then
	ASSERT_THAT(obtained, testing::Eq(static_cast<uint32_t>(0)));
}

TEST_F(BeaconCacheTest, evictRecordsByAge)
{
	// given
	BeaconKey_t key(1, 0);
	BeaconCache_t target(mockLogger);
	target.addActionData(key, 1000L, "a");
	target.addActionData(key, 1001L, "iii");
	target.addEventData(key, 1000L, "b");
	target.addEventData(key, 1001L, "jjj");

	// when
	auto obtained = target.evictRecordsByAge(key, 1001);

	// then
	ASSERT_THAT(obtained, testing::Eq(static_cast<uint32_t>(2)));
}

TEST_F(BeaconCacheTest, evictRecordsByNumberDoesNothingAndReturnsZeroIfBeaconIDDoesNotExist)
{
	// given
	BeaconKey_t key(1, 0);
	BeaconCache_t target(mockLogger);
	target.addActionData(key, 1000L, "a");
	target.addActionData(key, 1001L, "iii");
	target.addEventData(key, 1000L, "b");
	target.addEventData(key, 1001L, "jjj");

	// when
	auto obtained = target.evictRecordsByNumber(BeaconKey_t(666, 0), 100);

	// then
	ASSERT_THAT(obtained, testing::Eq(static_cast<uint32_t>(0)));
}

TEST_F(BeaconCacheTest, evictRecordsByNumber)
{
	// given
	BeaconKey_t key(1, 0);
	BeaconCache_t target(mockLogger);
	target.addActionData(key, 1000L, "a");
	target.addActionData(key, 1001L, "iii");
	target.addEventData(key, 1000L, "b");
	target.addEventData(key, 1001L, "jjj");

	// when
	auto obtained = target.evictRecordsByNumber(key, 2);

	// then
	ASSERT_THAT(obtained, testing::Eq(static_cast<uint32_t>(2)));
}

TEST_F(BeaconCacheTest, isEmptyGivesTrueIfBeaconDoesNotExistInCache)
{
	// given
	BeaconKey_t key(1, 0);
	BeaconCache_t target(mockLogger);
	target.addActionData(key, 1000L, "a");
	target.addActionData(key, 1001L, "iii");
	target.addEventData(key, 1000L, "b");
	target.addEventData(key, 1001L, "jjj");

	// then
	ASSERT_THAT(target.isEmpty(BeaconKey_t(666, 0)), testing::Eq(true));
}

TEST_F(BeaconCacheTest, isEmptyGivesFalseIfBeaconDataSizeIsNotEqualToZero)
{
	// given
	BeaconKey_t key(1, 0);
	BeaconCache_t target(mockLogger);
	target.addActionData(key, 1000L, "a");
	target.addEventData(key, 1000L, "b");

	// then
	ASSERT_THAT(target.isEmpty(key), testing::Eq(false));
}

TEST_F(BeaconCacheTest, isEmptyGivesTrueIfBeaconDoesNotContainActiveData)
{
	// given
	BeaconKey_t key(1, 0);
	BeaconCache_t target(mockLogger);
	target.addActionData(key, 1000L, "a");
	target.addEventData(key, 1000L, "b");

	target.getNextBeaconChunk(key, "prefix", 0, "&");

	// then
	ASSERT_THAT(target.isEmpty(key), testing::Eq(true));
}