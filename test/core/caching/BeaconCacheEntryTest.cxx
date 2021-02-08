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

#include "core/UTF8String.h"
#include "core/caching/BeaconCacheEntry.h"

#include <cstring>
#include <list>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using BeaconCacheEntry_t = core::caching::BeaconCacheEntry;
using BeaconCacheRecord_t = core::caching::BeaconCacheRecord;
using Utf8String_t = core::UTF8String;

class BeaconCacheEntryTest : public testing::Test
{
};

TEST_F(BeaconCacheEntryTest, aDefaultConstructedInstanceHasNoData)
{
	// given
	BeaconCacheEntry_t target;

	// then
	ASSERT_THAT(target.getEventData(), testing::IsEmpty());
	ASSERT_THAT(target.getActionData(), testing::IsEmpty());
	ASSERT_THAT(target.getEventDataBeingSent(), testing::IsEmpty());
	ASSERT_THAT(target.getActionDataBeingSent(), testing::IsEmpty());
}

TEST_F(BeaconCacheEntryTest, addingActionData)
{
	// given
	BeaconCacheRecord_t dataOne(0L, "foo");
	BeaconCacheRecord_t dataTwo(1L, "bar");

	BeaconCacheEntry_t target;

	// when adding first record
	target.addActionData(dataOne);

	// then
	auto expectedActionData = std::list<BeaconCacheRecord_t>{ dataOne };
	ASSERT_THAT(target.getActionData(), testing::Eq(expectedActionData));
	ASSERT_THAT(target.getEventData(), testing::IsEmpty());
	ASSERT_THAT(target.getEventDataBeingSent(), testing::IsEmpty());
	ASSERT_THAT(target.getActionDataBeingSent(), testing::IsEmpty());


	// and when adding second record
	target.addActionData(dataTwo);

	// then
	expectedActionData = std::list<BeaconCacheRecord_t>{ dataOne, dataTwo };
	ASSERT_THAT(target.getActionData(), testing::Eq(expectedActionData));
	ASSERT_THAT(target.getEventData(), testing::IsEmpty());
	ASSERT_THAT(target.getEventDataBeingSent(), testing::IsEmpty());
	ASSERT_THAT(target.getActionDataBeingSent(), testing::IsEmpty());
}

TEST_F(BeaconCacheEntryTest, addingEventData)
{
	// given
	BeaconCacheRecord_t dataOne(0L, "foo");
	BeaconCacheRecord_t dataTwo(1L, "bar");

	BeaconCacheEntry_t target;

	// when adding first record
	target.addEventData(dataOne);

	// then
	auto expectedEventData = std::list<BeaconCacheRecord_t>{ dataOne };
	ASSERT_THAT(target.getActionData(), testing::IsEmpty());
	ASSERT_THAT(target.getEventData(), testing::Eq(expectedEventData));
	ASSERT_THAT(target.getEventDataBeingSent(), testing::IsEmpty());
	ASSERT_THAT(target.getActionDataBeingSent(), testing::IsEmpty());

	// and when adding second record
	target.addEventData(dataTwo);

	// then
	expectedEventData = std::list<BeaconCacheRecord_t>{ dataOne, dataTwo };
	ASSERT_THAT(target.getActionData(), testing::IsEmpty());
	ASSERT_THAT(target.getEventData(), testing::Eq(expectedEventData));
	ASSERT_THAT(target.getEventDataBeingSent(), testing::IsEmpty());
	ASSERT_THAT(target.getActionDataBeingSent(), testing::IsEmpty());
}

TEST_F(BeaconCacheEntryTest, copyDataForSendingMovesData)
{
	// given
	BeaconCacheRecord_t dataOne(0L, "One");
	BeaconCacheRecord_t dataTwo(0L, "Two");
	BeaconCacheRecord_t dataThree(1L, "Three");
	BeaconCacheRecord_t dataFour(1L, "Four");

	BeaconCacheEntry_t target;
	target.addEventData(dataOne);
	target.addEventData(dataFour);
	target.addActionData(dataTwo);
	target.addActionData(dataThree);

	// when copying data for later chunking
	target.copyDataForSending();

	// then the data was moved
	auto expectedActionDataBeingSent = std::list<BeaconCacheRecord_t>{ dataTwo, dataThree };
	auto expectedEventDataBeingSent = std::list<BeaconCacheRecord_t>{ dataOne, dataFour };
	ASSERT_THAT(target.getActionData(), testing::IsEmpty());
	ASSERT_THAT(target.getEventData(), testing::IsEmpty());
	ASSERT_THAT(target.getEventDataBeingSent(), testing::Eq(expectedEventDataBeingSent));
	ASSERT_THAT(target.getActionDataBeingSent(), testing::Eq(expectedActionDataBeingSent));
}

TEST_F(BeaconCacheEntryTest, needsDataCopyBeforeSendingGivesTrueBeforeDataIsCopied)
{
	// given
	BeaconCacheRecord_t dataOne(0L, "One");
	BeaconCacheRecord_t dataTwo(0L, "Two");
	BeaconCacheRecord_t dataThree(1L, "Three");
	BeaconCacheRecord_t dataFour(1L, "Four");

	BeaconCacheEntry_t target;
	target.addEventData(dataOne);
	target.addEventData(dataFour);
	target.addActionData(dataTwo);
	target.addActionData(dataThree);

	// when, then
	ASSERT_THAT(target.needsDataCopyBeforeSending(), testing::Eq(true));
}

TEST_F(BeaconCacheEntryTest, needsDataCopyBeforeSendingGivesFalseAfterDataHasBeenCopied)
{
	// given
	BeaconCacheRecord_t dataOne(0L, "One");
	BeaconCacheRecord_t dataTwo(0L, "Two");
	BeaconCacheRecord_t dataThree(1L, "Three");
	BeaconCacheRecord_t dataFour(1L, "Four");

	BeaconCacheEntry_t target;
	target.addEventData(dataOne);
	target.addEventData(dataFour);
	target.addActionData(dataTwo);
	target.addActionData(dataThree);

	target.copyDataForSending();

	// when, then
	ASSERT_THAT(target.needsDataCopyBeforeSending(), testing::Eq(false));
}

TEST_F(BeaconCacheEntryTest, needsDataCopyBeforeSendingGivesFalseEvenIfListsAreEmpty)
{
	// given
	BeaconCacheEntry_t target;

	target.copyDataForSending();

	// when, then
	ASSERT_THAT(target.needsDataCopyBeforeSending(), testing::Eq(false));

	// and all the lists are empty
	ASSERT_THAT(target.getEventData(), testing::IsEmpty());
	ASSERT_THAT(target.getActionData(), testing::IsEmpty());
	ASSERT_THAT(target.getEventDataBeingSent(), testing::IsEmpty());
	ASSERT_THAT(target.getActionDataBeingSent(), testing::IsEmpty());
}

TEST_F(BeaconCacheEntryTest, getChunkMarksRetrievedData)
{
	// given
	BeaconCacheRecord_t dataOne(0L, "One");
	BeaconCacheRecord_t dataTwo(0L, "Two");
	BeaconCacheRecord_t dataThree(1L, "Three");
	BeaconCacheRecord_t dataFour(1L, "Four");

	BeaconCacheEntry_t target;
	target.addEventData(dataOne);
	target.addEventData(dataFour);
	target.addActionData(dataTwo);
	target.addActionData(dataThree);

	// when copying data for later chunking
	target.copyDataForSending();

	// when retrieving data
	auto obtained = target.getChunk("prefix", 1024, "&");

	// then
	ASSERT_THAT(obtained, testing::Eq(Utf8String_t("prefix&One&Four&Two&Three")));

	// and all of them are marked
	auto expectedDataOne = BeaconCacheRecord_t(dataOne);
	expectedDataOne.markForSending();
	auto expectedDataFour = BeaconCacheRecord_t(dataFour);
	expectedDataFour.markForSending();
	auto expectedEventDataBeingSent = std::list<BeaconCacheRecord_t>{ expectedDataOne, expectedDataFour };

	ASSERT_THAT(target.getEventDataBeingSent(), testing::Eq(expectedEventDataBeingSent));

	auto expectedDataTwo = BeaconCacheRecord_t(dataTwo);
	expectedDataTwo.markForSending();
	auto expectedDataThree = BeaconCacheRecord_t(dataThree);
	expectedDataThree.markForSending();
	auto expectedActionDataBeingSent = std::list<BeaconCacheRecord_t>{ expectedDataTwo, expectedDataThree };

	ASSERT_THAT(target.getActionDataBeingSent(), testing::Eq(expectedActionDataBeingSent));
}

TEST_F(BeaconCacheEntryTest, getChunkGetsChunksFromEventDataBeforeActionData)
{
	// given
	BeaconCacheRecord_t dataOne(0L, "One");
	BeaconCacheRecord_t dataTwo(0L, "Two");
	BeaconCacheRecord_t dataThree(1L, "Three");
	BeaconCacheRecord_t dataFour(1L, "Four");

	BeaconCacheEntry_t target;
	target.addEventData(dataOne);
	target.addEventData(dataFour);
	target.addActionData(dataTwo);
	target.addActionData(dataThree);

	// when copying data for later chunking
	target.copyDataForSending();

	// when retrieving data
	auto obtained = target.getChunk("a", 2, "&");

	// then it's the first event data
	ASSERT_THAT(obtained, testing::Eq(Utf8String_t("a&One")));

	// and when removing already sent data and getting next chunk
	target.removeDataMarkedForSending();
	auto obtained2 = target.getChunk("a", 2, "&");

	// then it's second event data
	ASSERT_THAT(obtained2, testing::Eq(Utf8String_t("a&Four")));

	// and when removing already sent data and getting next chunk
	target.removeDataMarkedForSending();
	auto obtained3 = target.getChunk("a", 2, "&");

	// then it's the first action data
	ASSERT_THAT(obtained3, testing::Eq(Utf8String_t("a&Two")));

	// and when removing already sent data and getting next chunk
	target.removeDataMarkedForSending();
	auto obtained4 = target.getChunk("a", 2, "&");

	// then it's the second action data
	ASSERT_THAT(obtained4, testing::Eq(Utf8String_t("a&Three")));

	// and when removing already sent data and getting next chunk
	target.removeDataMarkedForSending();
	auto obtained5 = target.getChunk("a", 2, "&");

	// then we get an empty string, since all chunks were sent & deleted
	ASSERT_THAT(obtained5, testing::Eq(Utf8String_t("")));
}

TEST_F(BeaconCacheEntryTest, getChunkGetsAlreadyMarkedData)
{
	// given
	BeaconCacheRecord_t dataOne(0L, "One");
	BeaconCacheRecord_t dataTwo(0L, "Two");
	BeaconCacheRecord_t dataThree(1L, "Three");
	BeaconCacheRecord_t dataFour(1L, "Four");

	BeaconCacheEntry_t target;
	target.addEventData(dataOne);
	target.addEventData(dataFour);
	target.addActionData(dataTwo);
	target.addActionData(dataThree);

	target.copyDataForSending();

	// when getting data to send
	auto obtained = target.getChunk("a", 100, "&");

	// then
	ASSERT_THAT(obtained, testing::Eq(Utf8String_t("a&One&Four&Two&Three")));

	// and all of them are marked
	auto expectedDataOne = BeaconCacheRecord_t(dataOne);
	expectedDataOne.markForSending();
	auto expectedDataFour = BeaconCacheRecord_t(dataFour);
	expectedDataFour.markForSending();
	auto expectedEventDataBeingSent = std::list<BeaconCacheRecord_t>{ expectedDataOne, expectedDataFour };

	auto expectedDataTwo = BeaconCacheRecord_t(dataTwo);
	expectedDataTwo.markForSending();
	auto expectedDataThree = BeaconCacheRecord_t(dataThree);
	expectedDataThree.markForSending();
	auto expectedActionDataBeingSent = std::list<BeaconCacheRecord_t>{ expectedDataTwo, expectedDataThree };

	ASSERT_THAT(target.getEventDataBeingSent(), testing::Eq(expectedEventDataBeingSent));
	ASSERT_THAT(target.getActionDataBeingSent(), testing::Eq(expectedActionDataBeingSent));

	// when getting data to send once more
	auto obtained2 = target.getChunk("a", 100, "&");

	// then
	ASSERT_THAT(obtained, testing::Eq(Utf8String_t("a&One&Four&Two&Three")));

	ASSERT_THAT(target.getEventDataBeingSent(), testing::Eq(expectedEventDataBeingSent));
	ASSERT_THAT(target.getActionDataBeingSent(), testing::Eq(expectedActionDataBeingSent));
}

TEST_F(BeaconCacheEntryTest, getChunksTakesSizeIntoAccount)
{
	// given
	BeaconCacheRecord_t dataOne(0L, "One");
	BeaconCacheRecord_t dataTwo(0L, "Two");
	BeaconCacheRecord_t dataThree(1L, "Three");
	BeaconCacheRecord_t dataFour(1L, "Four");

	BeaconCacheEntry_t target;
	target.addEventData(dataOne);
	target.addEventData(dataFour);
	target.addActionData(dataTwo);
	target.addActionData(dataThree);

	target.copyDataForSending();

	// when requesting first chunk
	auto obtained = target.getChunk("prefix", 1, "&");

	// then only prefix is returned, since "prefix".length > maxSize (=1)
	ASSERT_THAT(obtained, testing::Eq(Utf8String_t("prefix")));

	// and when retrieving something which is one character longer than "prefix"
	auto obtained2 = target.getChunk("prefix", std::strlen("prefix"), "&");

	// then only prefix is returned, since "prefix".length > maxSize (=1)
	ASSERT_THAT(obtained2, testing::Eq(Utf8String_t("prefix&One")));

	// and when retrieving another chunk
	auto obtained3 = target.getChunk("prefix", std::strlen("prefix&One"), "&");

	// then
	ASSERT_THAT(obtained3, testing::Eq(Utf8String_t("prefix&One&Four")));
}

TEST_F(BeaconCacheEntryTest, removeDataMarkedForSendingReturnsIfDataHasNotBeenCopied)
{
	// given
	BeaconCacheRecord_t dataOne(0L, "One");
	BeaconCacheRecord_t dataTwo(0L, "Two");
	BeaconCacheRecord_t dataThree(1L, "Three");
	BeaconCacheRecord_t dataFour(1L, "Four");

	BeaconCacheEntry_t target;
	target.addEventData(dataOne);
	target.addEventData(dataFour);
	target.addActionData(dataTwo);
	target.addActionData(dataThree);

	// when
	target.removeDataMarkedForSending();

	// then
	auto expectedEventData = std::list<BeaconCacheRecord_t>{ dataOne, dataFour };
	auto expectedActionData = std::list<BeaconCacheRecord_t>{ dataTwo, dataThree };

	ASSERT_THAT(target.getEventData(), testing::Eq(expectedEventData));
	ASSERT_THAT(target.getActionData(), testing::Eq(expectedActionData));
	ASSERT_THAT(target.getEventDataBeingSent(), testing::IsEmpty());
	ASSERT_THAT(target.getActionDataBeingSent(), testing::IsEmpty());
}

TEST_F(BeaconCacheEntryTest, resetDataMarkedForSendingReturnsIfDataHasNotBeenCopied)
{
	// given
	BeaconCacheRecord_t dataOne(0L, "One");
	BeaconCacheRecord_t dataTwo(0L, "Two");
	BeaconCacheRecord_t dataThree(1L, "Three");
	BeaconCacheRecord_t dataFour(1L, "Four");

	BeaconCacheEntry_t target;
	target.addEventData(dataOne);
	target.addEventData(dataFour);
	target.addActionData(dataTwo);
	target.addActionData(dataThree);

	// when
	target.resetDataMarkedForSending();

	// then
	auto expectedEventData = std::list<BeaconCacheRecord_t>{ dataOne, dataFour };
	auto expectedActionData = std::list<BeaconCacheRecord_t>{ dataTwo, dataThree };

	ASSERT_THAT(target.getEventData(), testing::Eq(expectedEventData));
	ASSERT_THAT(target.getActionData(), testing::Eq(expectedActionData));
	ASSERT_THAT(target.getEventDataBeingSent(), testing::IsEmpty());
	ASSERT_THAT(target.getActionDataBeingSent(), testing::IsEmpty());
}

TEST_F(BeaconCacheEntryTest, resetDataMarkedForSendingMovesPreviouslyCopiedDataBack)
{
	// given
	BeaconCacheRecord_t dataOne(0L, "One");
	BeaconCacheRecord_t dataTwo(0L, "Two");
	BeaconCacheRecord_t dataThree(1L, "Three");
	BeaconCacheRecord_t dataFour(1L, "Four");

	BeaconCacheEntry_t target;
	target.addEventData(dataOne);
	target.addEventData(dataFour);
	target.addActionData(dataTwo);
	target.addActionData(dataThree);

	target.copyDataForSending();

	// when data is reset
	target.resetDataMarkedForSending();

	// then
	auto expectedEventData = std::list<BeaconCacheRecord_t>{ dataOne, dataFour };
	auto expectedActionData = std::list<BeaconCacheRecord_t>{ dataTwo, dataThree };

	ASSERT_THAT(target.getEventData(), testing::Eq(expectedEventData));
	ASSERT_THAT(target.getActionData(), testing::Eq(expectedActionData));
	ASSERT_THAT(target.getEventDataBeingSent(), testing::IsEmpty());
	ASSERT_THAT(target.getActionDataBeingSent(), testing::IsEmpty());
}

TEST_F(BeaconCacheEntryTest, resetDataMarkedForSendingResetsMarkedForSendingFlag)
{
	// given
	BeaconCacheRecord_t dataOne(0L, "One");
	BeaconCacheRecord_t dataTwo(0L, "Two");
	BeaconCacheRecord_t dataThree(1L, "Three");
	BeaconCacheRecord_t dataFour(1L, "Four");

	BeaconCacheEntry_t target;
	target.addEventData(dataOne);
	target.addEventData(dataFour);
	target.addActionData(dataTwo);
	target.addActionData(dataThree);

	target.copyDataForSending();

	// when data is retrieved
	target.getChunk("", 1024, "&");

	// then all records are marked for sending
	auto expectedDataOne = BeaconCacheRecord_t(dataOne);
	expectedDataOne.markForSending();
	auto expectedDataFour = BeaconCacheRecord_t(dataFour);
	expectedDataFour.markForSending();
	auto expectedEventDataBeingSent = std::list<BeaconCacheRecord_t>{ expectedDataOne, expectedDataFour };

	auto expectedDataTwo = BeaconCacheRecord_t(dataTwo);
	expectedDataTwo.markForSending();
	auto expectedDataThree = BeaconCacheRecord_t(dataThree);
	expectedDataThree.markForSending();
	auto expectedActionDataBeingSent = std::list<BeaconCacheRecord_t>{ expectedDataTwo, expectedDataThree };

	ASSERT_THAT(target.getEventDataBeingSent(), testing::Eq(expectedEventDataBeingSent));
	ASSERT_THAT(target.getActionDataBeingSent(), testing::Eq(expectedActionDataBeingSent));

	// and when
	target.resetDataMarkedForSending();

	// then
	auto expectedEventData = std::list<BeaconCacheRecord_t>{ dataOne, dataFour };
	auto expectedActionData = std::list<BeaconCacheRecord_t>{ dataTwo, dataThree };

	ASSERT_THAT(target.getEventData(), testing::Eq(expectedEventData));
	ASSERT_THAT(target.getActionData(), testing::Eq(expectedActionData));
	ASSERT_THAT(target.getEventDataBeingSent(), testing::IsEmpty());
	ASSERT_THAT(target.getActionDataBeingSent(), testing::IsEmpty());
}

TEST_F(BeaconCacheEntryTest, getTotalNumberOfBytesCountsAddedRecordBytes)
{
	// given
	BeaconCacheRecord_t dataOne(0L, "One");
	BeaconCacheRecord_t dataTwo(0L, "Two");
	BeaconCacheRecord_t dataThree(1L, "Three");
	BeaconCacheRecord_t dataFour(1L, "Four");

	BeaconCacheEntry_t target;

	// when getting total number of bytes on an empty entry, then
	ASSERT_THAT(target.getTotalNumberOfBytes(), testing::Eq(int64_t(0)));

	// and when adding first entry
	target.addActionData(dataOne);

	// then
	ASSERT_THAT(target.getTotalNumberOfBytes(), testing::Eq(dataOne.getDataSizeInBytes()));

	// and when adding next entry
	target.addEventData(dataTwo);

	// then
	ASSERT_THAT(target.getTotalNumberOfBytes(),
		testing::Eq(dataOne.getDataSizeInBytes() + dataTwo.getDataSizeInBytes()));

	// and when adding next entry
	target.addEventData(dataThree);

	// then
	ASSERT_THAT(target.getTotalNumberOfBytes(),
		testing::Eq(dataOne.getDataSizeInBytes() + dataTwo.getDataSizeInBytes() + dataThree.getDataSizeInBytes()));

	// and when adding next entry
	target.addActionData(dataFour);

	// then
	ASSERT_THAT(target.getTotalNumberOfBytes(),
		testing::Eq(dataOne.getDataSizeInBytes() + dataTwo.getDataSizeInBytes() + dataThree.getDataSizeInBytes() + dataFour.getDataSizeInBytes()));
}

TEST_F(BeaconCacheEntryTest, removeRecordsOlderThanRemovesNothingIfNoActionOrEventDataExists)
{
	// given
	BeaconCacheEntry_t target;

	// when
	auto obtained = target.removeRecordsOlderThan(0);

	// then
	ASSERT_THAT(obtained, testing::Eq(0));
}

TEST_F(BeaconCacheEntryTest, removeRecordsOlderThanRemovesRecordsFromActionData)
{
	// given
	BeaconCacheRecord_t dataOne(4000L, "One");
	BeaconCacheRecord_t dataTwo(3000L, "Two");
	BeaconCacheRecord_t dataThree(2000L, "Three");
	BeaconCacheRecord_t dataFour(1000L, "Four");

	BeaconCacheEntry_t target;
	target.addActionData(dataOne);
	target.addActionData(dataTwo);
	target.addActionData(dataThree);
	target.addActionData(dataFour);

	// when removing everything older than 3000
	auto obtained = target.removeRecordsOlderThan(dataTwo.getTimestamp());

	// then
	ASSERT_THAT(obtained, testing::Eq(2)); // two were removed
	ASSERT_THAT(target.getActionData(), testing::Eq(std::list<BeaconCacheRecord_t>{ dataOne, dataTwo }));
}

TEST_F(BeaconCacheEntryTest, removeRecordsOlderThanRemovesRecordsFromEventData)
{
	// given
	BeaconCacheRecord_t dataOne(4000L, "One");
	BeaconCacheRecord_t dataTwo(3000L, "Two");
	BeaconCacheRecord_t dataThree(2000L, "Three");
	BeaconCacheRecord_t dataFour(1000L, "Four");

	BeaconCacheEntry_t target;
	target.addEventData(dataOne);
	target.addEventData(dataTwo);
	target.addEventData(dataThree);
	target.addEventData(dataFour);

	// when removing everything older than 3000
	auto obtained = target.removeRecordsOlderThan(dataTwo.getTimestamp());

	// then
	ASSERT_THAT(obtained, testing::Eq(2)); // two were removed
	ASSERT_THAT(target.getEventData(), testing::Eq(std::list<BeaconCacheRecord_t>{ dataOne, dataTwo }));
}

TEST_F(BeaconCacheEntryTest, removeOldestRecordsRemovesNothingIfEntryIsEmpty)
{
	// given
	BeaconCacheEntry_t target;

	// when
	auto obtained = target.removeRecordsOlderThan(1);

	// then
	ASSERT_THAT(obtained, testing::Eq(0));
}

TEST_F(BeaconCacheEntryTest, removeOldestRecordsRemovesActionDataIfEventDataIsEmpty)
{
	// given
	BeaconCacheRecord_t dataOne(4000L, "One");
	BeaconCacheRecord_t dataTwo(3000L, "Two");
	BeaconCacheRecord_t dataThree(2000L, "Three");
	BeaconCacheRecord_t dataFour(1000L, "Four");

	BeaconCacheEntry_t target;
	target.addActionData(dataOne);
	target.addActionData(dataTwo);
	target.addActionData(dataThree);
	target.addActionData(dataFour);

	// when
	auto obtained = target.removeOldestRecords(2);

	// then
	ASSERT_THAT(obtained, testing::Eq(2)); // two were removed
	ASSERT_THAT(target.getActionData(), testing::Eq(std::list<BeaconCacheRecord_t>{ dataThree, dataFour }));
}

TEST_F(BeaconCacheEntryTest, removeOldestRecordsRemovesEventDataIfActionDataIsEmpty)
{
	// given
	BeaconCacheRecord_t dataOne(4000L, "One");
	BeaconCacheRecord_t dataTwo(3000L, "Two");
	BeaconCacheRecord_t dataThree(2000L, "Three");
	BeaconCacheRecord_t dataFour(1000L, "Four");

	BeaconCacheEntry_t target;
	target.addEventData(dataOne);
	target.addEventData(dataTwo);
	target.addEventData(dataThree);
	target.addEventData(dataFour);

	// when
	auto obtained = target.removeOldestRecords(2);

	// then
	ASSERT_THAT(obtained, testing::Eq(2)); // two were removed
	ASSERT_THAT(target.getEventData(), testing::Eq(std::list<BeaconCacheRecord_t>{ dataThree, dataFour }));
}

TEST_F(BeaconCacheEntryTest, removeOldestRecordsComparesTopActionAndEventDataAndRemovesOldest)
{
	// given
	BeaconCacheRecord_t dataOne(1000L, "One");
	BeaconCacheRecord_t dataTwo(1100L, "Two");
	BeaconCacheRecord_t dataThree(950L, "Three");
	BeaconCacheRecord_t dataFour(1200L, "Four");

	BeaconCacheEntry_t target;
	target.addEventData(dataOne);
	target.addActionData(dataTwo);
	target.addActionData(dataThree);
	target.addEventData(dataFour);

	// when
	auto obtained = target.removeOldestRecords(1);

	// then
	ASSERT_THAT(obtained, testing::Eq(1));
	ASSERT_THAT(target.getActionData(), testing::Eq(std::list<BeaconCacheRecord_t>{ dataTwo, dataThree }));
	ASSERT_THAT(target.getEventData(), testing::Eq(std::list<BeaconCacheRecord_t>{ dataFour }));
}

TEST_F(BeaconCacheEntryTest, removeOldestRecordsRemovesEventDataIfTopEventDataAndActionDataHaveSameTimestamp)
{
	// given
	BeaconCacheRecord_t dataOne(1000L, "One");
	BeaconCacheRecord_t dataTwo(1100L, "Two");
	BeaconCacheRecord_t dataThree(dataOne.getTimestamp(), "Three");
	BeaconCacheRecord_t dataFour(dataTwo.getTimestamp(), "Four");

	BeaconCacheEntry_t target;
	target.addEventData(dataOne);
	target.addEventData(dataTwo);
	target.addActionData(dataThree);
	target.addActionData(dataFour);

	// when
	auto obtained = target.removeOldestRecords(1);

	// then
	ASSERT_THAT(obtained, testing::Eq(1));
	ASSERT_THAT(target.getActionData(), testing::Eq(std::list<BeaconCacheRecord_t>{ dataThree, dataFour }));
	ASSERT_THAT(target.getEventData(), testing::Eq(std::list<BeaconCacheRecord_t>{ dataTwo }));
}

TEST_F(BeaconCacheEntryTest, removeOldestRecordsStopsIfListsAreEmpty)
{
	// given
	BeaconCacheRecord_t dataOne(4000L, "One");
	BeaconCacheRecord_t dataTwo(3000L, "Two");
	BeaconCacheRecord_t dataThree(2000L, "Three");
	BeaconCacheRecord_t dataFour(1000L, "Four");

	BeaconCacheEntry_t target;
	target.addEventData(dataOne);
	target.addEventData(dataTwo);
	target.addEventData(dataThree);
	target.addEventData(dataFour);

	// when
	auto obtained = target.removeOldestRecords(100);

	// then
	ASSERT_THAT(obtained, testing::Eq(4));
	ASSERT_THAT(target.getActionData(), testing::IsEmpty());
	ASSERT_THAT(target.getEventData(), testing::IsEmpty());
}

TEST_F(BeaconCacheEntryTest, removeRecordsOlderThanDoesNotRemoveAnythingFromEventAndActionsBeingSent)
{
	// given
	BeaconCacheRecord_t dataOne(1000L, "One");
	BeaconCacheRecord_t dataTwo(1500L, "Two");
	BeaconCacheRecord_t dataThree(2000L, "Three");
	BeaconCacheRecord_t dataFour(2500L, "Four");

	BeaconCacheEntry_t target;
	target.addEventData(dataOne);
	target.addEventData(dataFour);
	target.addActionData(dataTwo);
	target.addActionData(dataThree);

	target.copyDataForSending();

	// when
	auto obtained = target.removeRecordsOlderThan(10000);

	// then
	ASSERT_THAT(obtained, testing::Eq(0));
	ASSERT_THAT(target.getActionDataBeingSent(), testing::Eq(std::list<BeaconCacheRecord_t>{ dataTwo, dataThree }));
	ASSERT_THAT(target.getEventDataBeingSent(), testing::Eq(std::list<BeaconCacheRecord_t>{ dataOne, dataFour }));
}

TEST_F(BeaconCacheEntryTest, removeOldestRecordsDoesNotRemoveAnythingFromEventAndActionsBeingSent)
{
	// given
	BeaconCacheRecord_t dataOne(1000L, "One");
	BeaconCacheRecord_t dataTwo(1500L, "Two");
	BeaconCacheRecord_t dataThree(2000L, "Three");
	BeaconCacheRecord_t dataFour(2500L, "Four");

	BeaconCacheEntry_t target;
	target.addEventData(dataOne);
	target.addEventData(dataFour);
	target.addActionData(dataTwo);
	target.addActionData(dataThree);

	target.copyDataForSending();

	// when
	auto obtained = target.removeOldestRecords(10000);

	// then
	ASSERT_THAT(obtained, testing::Eq(0));
	ASSERT_THAT(target.getActionDataBeingSent(), testing::Eq(std::list<BeaconCacheRecord_t>{ dataTwo, dataThree }));
	ASSERT_THAT(target.getEventDataBeingSent(), testing::Eq(std::list<BeaconCacheRecord_t>{ dataOne, dataFour }));
}

TEST_F(BeaconCacheEntryTest, hasDataForSendingReturnsFalseIfDataWasNotCopied)
{
	// given
	BeaconCacheRecord_t dataOne(1000L, "One");
	BeaconCacheRecord_t dataTwo(1500L, "Two");

	BeaconCacheEntry_t target;
	target.addEventData(dataOne);
	target.addEventData(dataTwo);

	// when
	auto obtained = target.hasDataToSend();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(BeaconCacheEntryTest, hasDataForSendingReturnsFalseIfNoDataWasAddedBeforeCopying)
{
	// given
	BeaconCacheEntry_t target;
	target.copyDataForSending();

	// when
	auto obtained = target.hasDataToSend();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(BeaconCacheEntryTest, hasDataForSendingReturnsTrueIfEventDataWasAddedBeforeCopying)
{
	// given
	BeaconCacheRecord_t record(1000L, "One");

	BeaconCacheEntry_t target;
	target.addEventData(record);

	target.copyDataForSending();

	// when
	auto obtained = target.hasDataToSend();

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
}

TEST_F(BeaconCacheEntryTest, hasDataForSendingReturnsTrueIfActionDataWasAddedBeforeCopying)
{
	// given
	BeaconCacheRecord_t record(1000L, "One");

	BeaconCacheEntry_t target;
	target.addActionData(record);

	target.copyDataForSending();

	// when
	auto obtained = target.hasDataToSend();

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
}