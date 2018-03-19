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

#include "caching/BeaconCacheEntry.h"
#include "core/UTF8String.h"

#include <cstring>

using namespace caching;

class BeaconCacheEntryTest : public testing::Test
{
public:
	void SetUp()
	{
	}

	void TearDown()
	{
	}
};

TEST_F(BeaconCacheEntryTest, aDefaultConstructedInstanceHasNoData)
{
	// given
	auto target = new BeaconCacheEntry();
	
	// then
	ASSERT_TRUE(target->getEventData().empty());
	ASSERT_TRUE(target->getActionData().empty());
	ASSERT_TRUE(target->getEventDataBeingSent().empty());
	ASSERT_TRUE(target->getActionDataBeingSent().empty());
}

TEST_F(BeaconCacheEntryTest, addingActionData)
{
	// given
	auto dataOne = new BeaconCacheRecord(0L, "foo");
	auto dataTwo = new BeaconCacheRecord(1L, "bar");

	auto target = new BeaconCacheEntry();

	// when adding first record
	target->addActionData(*dataOne);

	// then
	auto actionData = target->getActionData();
	auto eventData = target->getEventData();
	auto eventDataBeingSent = target->getEventDataBeingSent();
	auto actionDataBeingSent = target->getActionDataBeingSent();
	ASSERT_EQ(actionData.size(), 1);
	auto it = actionData.begin();
	ASSERT_TRUE(it->getData().equals("foo"));
	ASSERT_FALSE(it->isMarkedForSending());
	ASSERT_TRUE(eventData.empty());
	ASSERT_TRUE(eventDataBeingSent.empty());
	ASSERT_TRUE(actionDataBeingSent.empty());

	// and when adding second record
	target->addActionData(*dataTwo);

	// then
	auto actionData2 = target->getActionData();
	auto eventData2 = target->getEventData();
	auto eventDataBeingSent2 = target->getEventDataBeingSent();
	auto actionDataBeingSent2 = target->getActionDataBeingSent();
	ASSERT_EQ(actionData2.size(), 2);
	it = actionData2.begin();
	ASSERT_TRUE(it->getData().equals("foo"));
	ASSERT_FALSE(it->isMarkedForSending());
	it++;
	ASSERT_TRUE(it->getData().equals("bar"));
	ASSERT_FALSE(it->isMarkedForSending());
	ASSERT_TRUE(eventData2.empty());
	ASSERT_TRUE(eventDataBeingSent2.empty());
	ASSERT_TRUE(actionDataBeingSent2.empty());
}

TEST_F(BeaconCacheEntryTest, addingEventData)
{
	// given
	auto dataOne = new BeaconCacheRecord(0L, "foo");
	auto dataTwo = new BeaconCacheRecord(1L, "bar");

	auto target = new BeaconCacheEntry();

	// when adding first record
	target->addEventData(*dataOne);

	// then
	auto eventData = target->getEventData();
	auto actionData = target->getActionData();
	auto eventDataBeingSent = target->getEventDataBeingSent();
	auto actionDataBeingSent = target->getActionDataBeingSent();
	ASSERT_EQ(eventData.size(), 1);
	auto it = eventData.begin();
	ASSERT_TRUE(it->getData().equals("foo"));
	ASSERT_FALSE(it->isMarkedForSending());
	ASSERT_TRUE(actionData.empty());
	ASSERT_TRUE(eventDataBeingSent.empty());
	ASSERT_TRUE(actionDataBeingSent.empty());

	// and when adding second record
	target->addEventData(*dataTwo);

	// then
	auto eventData2 = target->getEventData();
	auto actionData2 = target->getActionData();
	auto eventDataBeingSent2 = target->getEventDataBeingSent();
	auto actionDataBeingSent2 = target->getActionDataBeingSent();
	ASSERT_EQ(eventData2.size(), 2);
	it = eventData2.begin();
	ASSERT_TRUE(it->getData().equals("foo"));
	ASSERT_FALSE(it->isMarkedForSending());
	it++;
	ASSERT_TRUE(it->getData().equals("bar"));
	ASSERT_FALSE(it->isMarkedForSending());
	ASSERT_TRUE(actionData2.empty());
	ASSERT_TRUE(eventDataBeingSent2.empty());
	ASSERT_TRUE(actionDataBeingSent2.empty());
}

TEST_F(BeaconCacheEntryTest, copyDataForChunkingMovesData)
{
	// given
	auto dataOne = new BeaconCacheRecord(0L, "One");
	auto dataTwo = new BeaconCacheRecord(0L, "Two");
	auto dataThree = new BeaconCacheRecord(1L, "Three");
	auto dataFour = new BeaconCacheRecord(1L, "Four");

	auto target = new BeaconCacheEntry();
	target->addEventData(*dataOne);
	target->addEventData(*dataFour);
	target->addActionData(*dataTwo);
	target->addActionData(*dataThree);

	// when copying data for later chunking
	target->copyDataForChunking();

	// then the data was moved
	auto eventData = target->getEventData();
	auto actionData = target->getActionData();
	auto eventDataBeingSent = target->getEventDataBeingSent();
	auto actionDataBeingSent = target->getActionDataBeingSent();
	ASSERT_EQ(eventDataBeingSent.size(), 2);
	auto it = eventDataBeingSent.begin();
	ASSERT_TRUE(it->getData().equals("One"));
	ASSERT_FALSE(it->isMarkedForSending());
	it++;
	ASSERT_TRUE(it->getData().equals("Four"));
	ASSERT_FALSE(it->isMarkedForSending());
	ASSERT_EQ(actionDataBeingSent.size(), 2);
	it = actionDataBeingSent.begin();
	ASSERT_TRUE(it->getData().equals("Two"));
	ASSERT_FALSE(it->isMarkedForSending());
	it++;
	ASSERT_TRUE(it->getData().equals("Three"));
	ASSERT_FALSE(it->isMarkedForSending());
	ASSERT_TRUE(eventData.empty());
	ASSERT_TRUE(actionData.empty());
}

TEST_F(BeaconCacheEntryTest, needsDataCopyBeforeChunkingGivesTrueBeforeDataIsCopied)
{
	// given
	auto dataOne = new BeaconCacheRecord(0L, "One");
	auto dataTwo = new BeaconCacheRecord(0L, "Two");
	auto dataThree = new BeaconCacheRecord(1L, "Three");
	auto dataFour = new BeaconCacheRecord(1L, "Four");

	auto target = new BeaconCacheEntry();
	target->addEventData(*dataOne);
	target->addEventData(*dataFour);
	target->addActionData(*dataTwo);
	target->addActionData(*dataThree);

	// when, then
	ASSERT_TRUE(target->needsDataCopyBeforeChunking());
}

TEST_F(BeaconCacheEntryTest, needsDataCopyBeforeChunkingGivesFalseAfterDataHasBeenCopied)
{
	// given
	auto dataOne = new BeaconCacheRecord(0L, "One");
	auto dataTwo = new BeaconCacheRecord(0L, "Two");
	auto dataThree = new BeaconCacheRecord(1L, "Three");
	auto dataFour = new BeaconCacheRecord(1L, "Four");

	auto target = new BeaconCacheEntry();
	target->addEventData(*dataOne);
	target->addEventData(*dataFour);
	target->addActionData(*dataTwo);
	target->addActionData(*dataThree);

	target->copyDataForChunking();

	// when, then
	ASSERT_FALSE(target->needsDataCopyBeforeChunking());
}

TEST_F(BeaconCacheEntryTest, needsDataCopyBeforeChunkingGivesFalseEvenIfListsAreEmpty)
{
	// given
	auto target = new BeaconCacheEntry();

	target->copyDataForChunking();

	// when, then
	ASSERT_FALSE(target->needsDataCopyBeforeChunking());

	// and all the lists are empty
	auto eventData = target->getEventData();
	auto actionData = target->getActionData();
	auto eventDataBeingSent = target->getEventDataBeingSent();
	auto actionDataBeingSent = target->getActionDataBeingSent();
	ASSERT_TRUE(eventData.empty());
	ASSERT_TRUE(actionData.empty());
	ASSERT_TRUE(eventDataBeingSent.empty());
	ASSERT_TRUE(actionDataBeingSent.empty());
}

TEST_F(BeaconCacheEntryTest, getChunkMarksRetrievedData)
{
	// given
	auto dataOne = new BeaconCacheRecord(0L, "One");
	auto dataTwo = new BeaconCacheRecord(0L, "Two");
	auto dataThree = new BeaconCacheRecord(1L, "Three");
	auto dataFour = new BeaconCacheRecord(1L, "Four");

	auto target = new BeaconCacheEntry();
	target->addEventData(*dataOne);
	target->addEventData(*dataFour);
	target->addActionData(*dataTwo);
	target->addActionData(*dataThree);

	// when copying data for later chunking
	target->copyDataForChunking();

	// when retrieving data
	auto obtained = target->getChunk("prefix", 1024, "&");

	// then
	core::UTF8String expected = "prefix&One&Four&Two&Three";
	ASSERT_TRUE(obtained.equals(expected));
	
	// and all of them are marked
	auto eventDataBeingSent = target->getEventDataBeingSent();
	auto actionDataBeingSent = target->getActionDataBeingSent();

	ASSERT_EQ(eventDataBeingSent.size(), 2);
	auto it = eventDataBeingSent.begin();
	ASSERT_TRUE(it->getData().equals("One"));
	ASSERT_TRUE(it->isMarkedForSending());
	it++;
	ASSERT_TRUE(it->getData().equals("Four"));
	ASSERT_TRUE(it->isMarkedForSending());
	ASSERT_EQ(actionDataBeingSent.size(), 2);
	it = actionDataBeingSent.begin();
	ASSERT_TRUE(it->getData().equals("Two"));
	ASSERT_TRUE(it->isMarkedForSending());
	it++;
	ASSERT_TRUE(it->getData().equals("Three"));
	ASSERT_TRUE(it->isMarkedForSending());
}

TEST_F(BeaconCacheEntryTest, getChunkGetsChunksFromEventDataBeforeActionData)
{
	// given
	auto dataOne = new BeaconCacheRecord(0L, "One");
	auto dataTwo = new BeaconCacheRecord(0L, "Two");
	auto dataThree = new BeaconCacheRecord(1L, "Three");
	auto dataFour = new BeaconCacheRecord(1L, "Four");

	auto target = new BeaconCacheEntry();
	target->addEventData(*dataOne);
	target->addEventData(*dataFour);
	target->addActionData(*dataTwo);
	target->addActionData(*dataThree);

	// when copying data for later chunking
	target->copyDataForChunking();

	// when retrieving data
	auto obtained = target->getChunk("a", 2, "&");

	// then it's the first event data
	ASSERT_TRUE(obtained.equals("a&One"));
	
	// and when removing already sent data and getting next chunk
	target->removeDataMarkedForSending();
	auto obtained2 = target->getChunk("a", 2, "&");

	// then it's second event data
	ASSERT_TRUE(obtained2.equals("a&Four"));
	
	// and when removing already sent data and getting next chunk
	target->removeDataMarkedForSending();
	auto obtained3 = target->getChunk("a", 2, "&");

	// then it's the first action data
	ASSERT_TRUE(obtained3.equals("a&Two"));

	// and when removing already sent data and getting next chunk
	target->removeDataMarkedForSending();
	auto obtained4 = target->getChunk("a", 2, "&");

	// then it's the second action data
	ASSERT_TRUE(obtained4.equals("a&Three"));
	
	// and when removing already sent data and getting next chunk
	target->removeDataMarkedForSending();
	auto obtained5 = target->getChunk("a", 2, "&");

	// then we get an empty string, since all chunks were sent & deleted
	ASSERT_TRUE(obtained5.equals(""));
}

TEST_F(BeaconCacheEntryTest, getChunkGetsAlreadyMarkedData)
{
	// given
	auto dataOne = new BeaconCacheRecord(0L, "One");
	auto dataTwo = new BeaconCacheRecord(0L, "Two");
	auto dataThree = new BeaconCacheRecord(1L, "Three");
	auto dataFour = new BeaconCacheRecord(1L, "Four");

	auto target = new BeaconCacheEntry();
	target->addEventData(*dataOne);
	target->addEventData(*dataFour);
	target->addActionData(*dataTwo);
	target->addActionData(*dataThree);

	target->copyDataForChunking();

	// when getting data to send
	auto obtained = target->getChunk("a", 100, "&");

	// then
	ASSERT_TRUE(obtained.equals("a&One&Four&Two&Three"));
	auto eventDataBeingSent = target->getEventDataBeingSent();
	auto actionDataBeingSent = target->getActionDataBeingSent();
	ASSERT_EQ(eventDataBeingSent.size(), 2);
	auto it = eventDataBeingSent.begin();
	ASSERT_TRUE(it->getData().equals("One"));
	ASSERT_TRUE(it->isMarkedForSending());
	it++;
	ASSERT_TRUE(it->getData().equals("Four"));
	ASSERT_TRUE(it->isMarkedForSending());
	ASSERT_EQ(actionDataBeingSent.size(), 2);
	it = actionDataBeingSent.begin();
	ASSERT_TRUE(it->getData().equals("Two"));
	ASSERT_TRUE(it->isMarkedForSending());
	it++;
	ASSERT_TRUE(it->getData().equals("Three"));
	ASSERT_TRUE(it->isMarkedForSending());

	// when getting data to send once more
	auto obtained2 = target->getChunk("a", 100, "&");

	// then
	ASSERT_TRUE(obtained2.equals("a&One&Four&Two&Three"));
	auto eventDataBeingSent2 = target->getEventDataBeingSent();
	auto actionDataBeingSent2 = target->getActionDataBeingSent();
	ASSERT_EQ(eventDataBeingSent2.size(), 2);
	it = eventDataBeingSent2.begin();
	ASSERT_TRUE(it->getData().equals("One"));
	ASSERT_TRUE(it->isMarkedForSending());
	it++;
	ASSERT_TRUE(it->getData().equals("Four"));
	ASSERT_TRUE(it->isMarkedForSending());
	ASSERT_EQ(actionDataBeingSent2.size(), 2);
	it = actionDataBeingSent2.begin();
	ASSERT_TRUE(it->getData().equals("Two"));
	ASSERT_TRUE(it->isMarkedForSending());
	it++;
	ASSERT_TRUE(it->getData().equals("Three"));
	ASSERT_TRUE(it->isMarkedForSending());
}

TEST_F(BeaconCacheEntryTest, getChunksTakesSizeIntoAccount)
{
	// given
	auto dataOne = new BeaconCacheRecord(0L, "One");
	auto dataTwo = new BeaconCacheRecord(0L, "Two");
	auto dataThree = new BeaconCacheRecord(1L, "Three");
	auto dataFour = new BeaconCacheRecord(1L, "Four");

	auto target = new BeaconCacheEntry();
	target->addEventData(*dataOne);
	target->addEventData(*dataFour);
	target->addActionData(*dataTwo);
	target->addActionData(*dataThree);

	target->copyDataForChunking();

	// when requesting first chunk
	auto obtained = target->getChunk("prefix", 1, "&");

	// then only prefix is returned, since "prefix".length > maxSize (=1)
	ASSERT_TRUE(obtained.equals("prefix"));

	// and when retrieving something which is one character longer than "prefix"
	auto obtained2 = target->getChunk("prefix", std::strlen("prefix"), "&");

	// then only prefix is returned, since "prefix".length > maxSize (=1)
	ASSERT_TRUE(obtained2.equals("prefix&One"));

	// and when retrieving another chunk
	auto obtained3 = target->getChunk("prefix", std::strlen("prefix&One"), "&");

	// then
	ASSERT_TRUE(obtained3.equals("prefix&One&Four"));
}

TEST_F(BeaconCacheEntryTest, removeDataMarkedForSendingReturnsIfDataHasNotBeenCopied)
{
	// given
	auto dataOne = new BeaconCacheRecord(0L, "One");
	auto dataTwo = new BeaconCacheRecord(0L, "Two");
	auto dataThree = new BeaconCacheRecord(1L, "Three");
	auto dataFour = new BeaconCacheRecord(1L, "Four");

	auto target = new BeaconCacheEntry();
	target->addEventData(*dataOne);
	target->addEventData(*dataFour);
	target->addActionData(*dataTwo);
	target->addActionData(*dataThree);

	// when
	target->removeDataMarkedForSending();

	// then
	auto eventData = target->getEventData();
	auto actionData = target->getActionData();
	auto eventDataBeingSent = target->getEventDataBeingSent();
	auto actionDataBeingSent = target->getActionDataBeingSent();
	ASSERT_EQ(eventData.size(), 2);
	auto it = eventData.begin();
	ASSERT_TRUE(it->getData().equals("One"));
	ASSERT_FALSE(it->isMarkedForSending());
	it++;
	ASSERT_TRUE(it->getData().equals("Four"));
	ASSERT_FALSE(it->isMarkedForSending());
	ASSERT_EQ(actionData.size(), 2);
	it = actionData.begin();
	ASSERT_TRUE(it->getData().equals("Two"));
	ASSERT_FALSE(it->isMarkedForSending());
	it++;
	ASSERT_TRUE(it->getData().equals("Three"));
	ASSERT_FALSE(it->isMarkedForSending());
	ASSERT_TRUE(eventDataBeingSent.empty());
	ASSERT_TRUE(actionDataBeingSent.empty());
}

TEST_F(BeaconCacheEntryTest, resetDataMarkedForSendingReturnsIfDataHasNotBeenCopied)
{
	// given
	auto dataOne = new BeaconCacheRecord(0L, "One");
	auto dataTwo = new BeaconCacheRecord(0L, "Two");
	auto dataThree = new BeaconCacheRecord(1L, "Three");
	auto dataFour = new BeaconCacheRecord(1L, "Four");

	auto target = new BeaconCacheEntry();
	target->addEventData(*dataOne);
	target->addEventData(*dataFour);
	target->addActionData(*dataTwo);
	target->addActionData(*dataThree);

	// when
	target->resetDataMarkedForSending();

	// then
	auto eventData = target->getEventData();
	auto actionData = target->getActionData();
	auto eventDataBeingSent = target->getEventDataBeingSent();
	auto actionDataBeingSent = target->getActionDataBeingSent();
	ASSERT_EQ(eventData.size(), 2);
	auto it = eventData.begin();
	ASSERT_TRUE(it->getData().equals("One"));
	ASSERT_FALSE(it->isMarkedForSending());
	it++;
	ASSERT_TRUE(it->getData().equals("Four"));
	ASSERT_FALSE(it->isMarkedForSending());
	ASSERT_EQ(actionData.size(), 2);
	it = actionData.begin();
	ASSERT_TRUE(it->getData().equals("Two"));
	ASSERT_FALSE(it->isMarkedForSending());
	it++;
	ASSERT_TRUE(it->getData().equals("Three"));
	ASSERT_FALSE(it->isMarkedForSending());
	ASSERT_TRUE(eventDataBeingSent.empty());
	ASSERT_TRUE(actionDataBeingSent.empty());
}

TEST_F(BeaconCacheEntryTest, resetDataMarkedForSendingMovesPreviouslyCopiedDataBack)
{
	// given
	auto dataOne = new BeaconCacheRecord(0L, "One");
	auto dataTwo = new BeaconCacheRecord(0L, "Two");
	auto dataThree = new BeaconCacheRecord(1L, "Three");
	auto dataFour = new BeaconCacheRecord(1L, "Four");

	auto target = new BeaconCacheEntry();
	target->addEventData(*dataOne);
	target->addEventData(*dataFour);
	target->addActionData(*dataTwo);
	target->addActionData(*dataThree);

	target->copyDataForChunking();

	// when data is reset
	target->resetDataMarkedForSending();

	// then
	auto eventData = target->getEventData();
	auto actionData = target->getActionData();
	auto eventDataBeingSent = target->getEventDataBeingSent();
	auto actionDataBeingSent = target->getActionDataBeingSent();
	ASSERT_EQ(eventData.size(), 2);
	auto it = eventData.begin();
	ASSERT_TRUE(it->getData().equals("One"));
	ASSERT_FALSE(it->isMarkedForSending());
	it++;
	ASSERT_TRUE(it->getData().equals("Four"));
	ASSERT_FALSE(it->isMarkedForSending());
	ASSERT_EQ(actionData.size(), 2);
	it = actionData.begin();
	ASSERT_TRUE(it->getData().equals("Two"));
	ASSERT_FALSE(it->isMarkedForSending());
	it++;
	ASSERT_TRUE(it->getData().equals("Three"));
	ASSERT_FALSE(it->isMarkedForSending());
	ASSERT_TRUE(eventDataBeingSent.empty());
	ASSERT_TRUE(actionDataBeingSent.empty());
}

TEST_F(BeaconCacheEntryTest, resetDataMarkedForSendingResetsMarkedForSendingFlag)
{
	// given
	auto dataOne = new BeaconCacheRecord(0L, "One");
	auto dataTwo = new BeaconCacheRecord(0L, "Two");
	auto dataThree = new BeaconCacheRecord(1L, "Three");
	auto dataFour = new BeaconCacheRecord(1L, "Four");

	auto target = new BeaconCacheEntry();
	target->addEventData(*dataOne);
	target->addEventData(*dataFour);
	target->addActionData(*dataTwo);
	target->addActionData(*dataThree);

	target->copyDataForChunking();

	// when data is retrieved
	target->getChunk("", 1024, "&");

	// then all records are marked for sending
	auto eventData = target->getEventData();
	auto actionData = target->getActionData();
	auto eventDataBeingSent = target->getEventDataBeingSent();
	auto actionDataBeingSent = target->getActionDataBeingSent();
	ASSERT_EQ(eventDataBeingSent.size(), 2);
	auto it = eventDataBeingSent.begin();
	ASSERT_TRUE(it->getData().equals("One"));
	ASSERT_TRUE(it->isMarkedForSending());
	it++;
	ASSERT_TRUE(it->getData().equals("Four"));
	ASSERT_TRUE(it->isMarkedForSending());
	ASSERT_EQ(actionDataBeingSent.size(), 2);
	it = actionDataBeingSent.begin();
	ASSERT_TRUE(it->getData().equals("Two"));
	ASSERT_TRUE(it->isMarkedForSending());
	it++;
	ASSERT_TRUE(it->getData().equals("Three"));
	ASSERT_TRUE(it->isMarkedForSending());
	ASSERT_TRUE(eventData.empty());
	ASSERT_TRUE(actionData.empty());

	// and when
	target->resetDataMarkedForSending();

	// then
	auto eventData2 = target->getEventData();
	auto actionData2 = target->getActionData();
	auto eventDataBeingSent2 = target->getEventDataBeingSent();
	auto actionDataBeingSent2 = target->getActionDataBeingSent();
	ASSERT_EQ(eventData2.size(), 2);
	it = eventData2.begin();
	ASSERT_TRUE(it->getData().equals("One"));
	ASSERT_FALSE(it->isMarkedForSending());
	it++;
	ASSERT_TRUE(it->getData().equals("Four"));
	ASSERT_FALSE(it->isMarkedForSending());
	ASSERT_EQ(actionData2.size(), 2);
	it = actionData2.begin();
	ASSERT_TRUE(it->getData().equals("Two"));
	ASSERT_FALSE(it->isMarkedForSending());
	it++;
	ASSERT_TRUE(it->getData().equals("Three"));
	ASSERT_FALSE(it->isMarkedForSending());
	ASSERT_TRUE(eventDataBeingSent2.empty());
	ASSERT_TRUE(actionDataBeingSent2.empty());
}

TEST_F(BeaconCacheEntryTest, getTotalNumberOfBytesCountsAddedRecordBytes)
{
	// given
	auto dataOne = new BeaconCacheRecord(0L, "One");
	auto dataTwo = new BeaconCacheRecord(0L, "Two");
	auto dataThree = new BeaconCacheRecord(1L, "Three");
	auto dataFour = new BeaconCacheRecord(1L, "Four");

	auto target = new BeaconCacheEntry();

	// when getting total number of bytes on an empty entry, then
	ASSERT_EQ(target->getTotalNumberOfBytes(), 0L);

	// and when adding first entry
	target->addActionData(*dataOne);

	// then
	ASSERT_EQ(target->getTotalNumberOfBytes(), dataOne->getDataSizeInBytes());

	// and when adding next entry
	target->addEventData(*dataTwo);

	// then
	ASSERT_EQ(target->getTotalNumberOfBytes(), dataOne->getDataSizeInBytes() + dataTwo->getDataSizeInBytes());

	// and when adding next entry
	target->addEventData(*dataThree);

	// then
	ASSERT_EQ(target->getTotalNumberOfBytes(), dataOne->getDataSizeInBytes() + dataTwo->getDataSizeInBytes() + dataThree->getDataSizeInBytes());

	// and when adding next entry
	target->addActionData(*dataFour);

	// then
	ASSERT_EQ(target->getTotalNumberOfBytes(), dataOne->getDataSizeInBytes() + dataTwo->getDataSizeInBytes() + dataThree->getDataSizeInBytes() + dataFour->getDataSizeInBytes());
}

TEST_F(BeaconCacheEntryTest, removeRecordsOlderThanRemovesNothingIfNoActionOrEventDataExists)
{
	// given
	auto target = new BeaconCacheEntry();

	// when
	auto obtained = target->removeRecordsOlderThan(0);

	// then
	ASSERT_EQ(obtained, 0);
}

TEST_F(BeaconCacheEntryTest, removeRecordsOlderThanRemovesRecordsFromActionData)
{
	// given
	auto dataOne = new BeaconCacheRecord(4000L, "One");
	auto dataTwo = new BeaconCacheRecord(3000L, "Two");
	auto dataThree = new BeaconCacheRecord(2000L, "Three");
	auto dataFour = new BeaconCacheRecord(1000L, "Four");

	auto target = new BeaconCacheEntry();
	target->addActionData(*dataOne);
	target->addActionData(*dataTwo);
	target->addActionData(*dataThree);
	target->addActionData(*dataFour);
	
	// when removing everything older than 3000
	auto obtained = target->removeRecordsOlderThan(dataTwo->getTimestamp());

	// then
	ASSERT_EQ(obtained, 2); // two were removed
	auto actionData = target->getActionData();
	ASSERT_EQ(actionData.size(), 2);
	auto it = actionData.begin();
	ASSERT_TRUE(it->getData().equals("One"));
	ASSERT_FALSE(it->isMarkedForSending());
	it++;
	ASSERT_TRUE(it->getData().equals("Two"));
	ASSERT_FALSE(it->isMarkedForSending());
}

TEST_F(BeaconCacheEntryTest, removeRecordsOlderThanRemovesRecordsFromEventData)
{
	// given
	auto dataOne = new BeaconCacheRecord(4000L, "One");
	auto dataTwo = new BeaconCacheRecord(3000L, "Two");
	auto dataThree = new BeaconCacheRecord(2000L, "Three");
	auto dataFour = new BeaconCacheRecord(1000L, "Four");

	auto target = new BeaconCacheEntry();
	target->addEventData(*dataOne);
	target->addEventData(*dataTwo);
	target->addEventData(*dataThree);
	target->addEventData(*dataFour);

	// when removing everything older than 3000
	auto obtained = target->removeRecordsOlderThan(dataTwo->getTimestamp());

	ASSERT_EQ(obtained, 2); // two were removed
	auto eventData = target->getEventData();
	ASSERT_EQ(eventData.size(), 2);
	auto it = eventData.begin();
	ASSERT_TRUE(it->getData().equals("One"));
	ASSERT_FALSE(it->isMarkedForSending());
	it++;
	ASSERT_TRUE(it->getData().equals("Two"));
	ASSERT_FALSE(it->isMarkedForSending());
}

TEST_F(BeaconCacheEntryTest, removeOldestRecordsRemovesNothingIfEntryIsEmpty)
{
	// given
	auto target = new BeaconCacheEntry();

	// when
	auto obtained = target->removeRecordsOlderThan(1);

	// then
	ASSERT_EQ(obtained, 0);
}

TEST_F(BeaconCacheEntryTest, removeOldestRecordsRemovesActionDataIfEventDataIsEmpty)
{
	// given
	auto dataOne = new BeaconCacheRecord(4000L, "One");
	auto dataTwo = new BeaconCacheRecord(3000L, "Two");
	auto dataThree = new BeaconCacheRecord(2000L, "Three");
	auto dataFour = new BeaconCacheRecord(1000L, "Four");

	auto target = new BeaconCacheEntry();
	target->addActionData(*dataOne);
	target->addActionData(*dataTwo);
	target->addActionData(*dataThree);
	target->addActionData(*dataFour);

	// when
	auto obtained = target->removeOldestRecords(2);

	// then
	ASSERT_EQ(obtained, 2); // two were removed
	auto actionData = target->getActionData();
	ASSERT_EQ(actionData.size(), 2);
	auto it = actionData.begin();
	ASSERT_TRUE(it->getData().equals("Three"));
	ASSERT_FALSE(it->isMarkedForSending());
	it++;
	ASSERT_TRUE(it->getData().equals("Four"));
	ASSERT_FALSE(it->isMarkedForSending());
}

TEST_F(BeaconCacheEntryTest, removeOldestRecordsRemovesEventDataIfActionDataIsEmpty)
{
	// given
	auto dataOne = new BeaconCacheRecord(4000L, "One");
	auto dataTwo = new BeaconCacheRecord(3000L, "Two");
	auto dataThree = new BeaconCacheRecord(2000L, "Three");
	auto dataFour = new BeaconCacheRecord(1000L, "Four");

	auto target = new BeaconCacheEntry();
	target->addEventData(*dataOne);
	target->addEventData(*dataTwo);
	target->addEventData(*dataThree);
	target->addEventData(*dataFour);

	// when
	auto obtained = target->removeOldestRecords(2);

	// then
	ASSERT_EQ(obtained, 2); // two were removed
	auto eventData = target->getEventData();
	ASSERT_EQ(eventData.size(), 2);
	auto it = eventData.begin();
	ASSERT_TRUE(it->getData().equals("Three"));
	ASSERT_FALSE(it->isMarkedForSending());
	it++;
	ASSERT_TRUE(it->getData().equals("Four"));
	ASSERT_FALSE(it->isMarkedForSending());
}

TEST_F(BeaconCacheEntryTest, removeOldestRecordsComparesTopActionAndEventDataAndRemovesOldest)
{
	// given
	auto dataOne = new BeaconCacheRecord(1000L, "One");
	auto dataTwo = new BeaconCacheRecord(1100L, "Two");
	auto dataThree = new BeaconCacheRecord(950L, "Three");
	auto dataFour = new BeaconCacheRecord(1200L, "Four");

	auto target = new BeaconCacheEntry();
	target->addEventData(*dataOne);
	target->addActionData(*dataTwo);
	target->addActionData(*dataThree);
	target->addEventData(*dataFour);

	// when
	auto obtained = target->removeOldestRecords(1);

	// then
	ASSERT_EQ(obtained, 1);
	auto eventData = target->getEventData();
	auto actionData = target->getActionData();
	ASSERT_EQ(actionData.size(), 2);
	auto it = actionData.begin();
	ASSERT_TRUE(it->getData().equals("Two"));
	ASSERT_FALSE(it->isMarkedForSending());
	it++;
	ASSERT_TRUE(it->getData().equals("Three"));
	ASSERT_FALSE(it->isMarkedForSending());
	ASSERT_EQ(eventData.size(), 1);
	it = eventData.begin();
	ASSERT_TRUE(it->getData().equals("Four"));
	ASSERT_FALSE(it->isMarkedForSending());
}

TEST_F(BeaconCacheEntryTest, removeOldestRecordsRemovesEventDataIfTopEventDataAndActionDataHaveSameTimestamp)
{
	// given
	auto dataOne = new BeaconCacheRecord(1000L, "One");
	auto dataTwo = new BeaconCacheRecord(1100L, "Two");
	auto dataThree = new BeaconCacheRecord(dataOne->getTimestamp(), "Three");
	auto dataFour = new BeaconCacheRecord(dataTwo->getTimestamp(), "Four");

	auto target = new BeaconCacheEntry();
	target->addEventData(*dataOne);
	target->addEventData(*dataTwo);
	target->addActionData(*dataThree);
	target->addActionData(*dataFour);

	// when
	auto obtained = target->removeOldestRecords(1);

	// then
	ASSERT_EQ(obtained, 1);
	auto eventData = target->getEventData();
	auto actionData = target->getActionData();
	ASSERT_EQ(actionData.size(), 2);
	auto it = actionData.begin();
	ASSERT_TRUE(it->getData().equals("Three"));
	ASSERT_FALSE(it->isMarkedForSending());
	it++;
	ASSERT_TRUE(it->getData().equals("Four"));
	ASSERT_FALSE(it->isMarkedForSending());
	ASSERT_EQ(eventData.size(), 1);
	it = eventData.begin();
	ASSERT_TRUE(it->getData().equals("Two"));
	ASSERT_FALSE(it->isMarkedForSending());
}

TEST_F(BeaconCacheEntryTest, removeOldestRecordsStopsIfListsAreEmpty)
{
	// given
	auto dataOne = new BeaconCacheRecord(4000L, "One");
	auto dataTwo = new BeaconCacheRecord(3000L, "Two");
	auto dataThree = new BeaconCacheRecord(2000L, "Three");
	auto dataFour = new BeaconCacheRecord(1000L, "Four");

	auto target = new BeaconCacheEntry();
	target->addEventData(*dataOne);
	target->addEventData(*dataTwo);
	target->addEventData(*dataThree);
	target->addEventData(*dataFour);

	// when
	auto obtained = target->removeOldestRecords(100);

	// then
	ASSERT_EQ(obtained, 4);
	auto eventData = target->getEventData();
	auto actionData = target->getActionData();
	ASSERT_TRUE(eventData.empty());
	ASSERT_TRUE(actionData.empty());
}

TEST_F(BeaconCacheEntryTest, removeRecordsOlderThanDoesNotRemoveAnythingFromEventAndActionsBeingSent)
{
	// given
	auto dataOne = new BeaconCacheRecord(1000L, "One");
	auto dataTwo = new BeaconCacheRecord(1500L, "Two");
	auto dataThree = new BeaconCacheRecord(2000L, "Three");
	auto dataFour = new BeaconCacheRecord(2500L, "Four");

	auto target = new BeaconCacheEntry();
	target->addEventData(*dataOne);
	target->addEventData(*dataFour);
	target->addActionData(*dataTwo);
	target->addActionData(*dataThree);
	
	target->copyDataForChunking();

	// when
	auto obtained = target->removeRecordsOlderThan(10000);

	// then
	ASSERT_EQ(obtained, 0);
	auto eventDataBeingSent = target->getEventDataBeingSent();
	auto actionDataBeingSent = target->getActionDataBeingSent();
	ASSERT_EQ(eventDataBeingSent.size(), 2);
	auto it = eventDataBeingSent.begin();
	ASSERT_TRUE(it->getData().equals("One"));
	ASSERT_FALSE(it->isMarkedForSending());
	it++;
	ASSERT_TRUE(it->getData().equals("Four"));
	ASSERT_FALSE(it->isMarkedForSending());
	ASSERT_EQ(actionDataBeingSent.size(), 2);
	it = actionDataBeingSent.begin();
	ASSERT_TRUE(it->getData().equals("Two"));
	ASSERT_FALSE(it->isMarkedForSending());
	it++;
	ASSERT_TRUE(it->getData().equals("Three"));
	ASSERT_FALSE(it->isMarkedForSending());
}

TEST_F(BeaconCacheEntryTest, removeOldestRecordsDoesNotRemoveAnythingFromEventAndActionsBeingSent)
{
	// given
	auto dataOne = new BeaconCacheRecord(1000L, "One");
	auto dataTwo = new BeaconCacheRecord(1500L, "Two");
	auto dataThree = new BeaconCacheRecord(2000L, "Three");
	auto dataFour = new BeaconCacheRecord(2500L, "Four");

	auto target = new BeaconCacheEntry();
	target->addEventData(*dataOne);
	target->addEventData(*dataFour);
	target->addActionData(*dataTwo);
	target->addActionData(*dataThree);

	target->copyDataForChunking();

	// when
	auto obtained = target->removeOldestRecords(10000);

	// then
	ASSERT_EQ(obtained, 0);
	auto eventDataBeingSent = target->getEventDataBeingSent();
	auto actionDataBeingSent = target->getActionDataBeingSent();
	ASSERT_EQ(eventDataBeingSent.size(), 2);
	auto it = eventDataBeingSent.begin();
	ASSERT_TRUE(it->getData().equals("One"));
	ASSERT_FALSE(it->isMarkedForSending());
	it++;
	ASSERT_TRUE(it->getData().equals("Four"));
	ASSERT_FALSE(it->isMarkedForSending());
	ASSERT_EQ(actionDataBeingSent.size(), 2);
	it = actionDataBeingSent.begin();
	ASSERT_TRUE(it->getData().equals("Two"));
	ASSERT_FALSE(it->isMarkedForSending());
	it++;
	ASSERT_TRUE(it->getData().equals("Three"));
	ASSERT_FALSE(it->isMarkedForSending());
}