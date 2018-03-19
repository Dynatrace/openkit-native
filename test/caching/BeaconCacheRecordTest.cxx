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

#include "caching/BeaconCacheRecord.h"
#include "core/UTF8String.h"

using namespace caching;

class BeaconCacheRecordTest : public testing::Test
{
public:
	void SetUp()
	{
	}

	void TearDown()
	{
	}
};

TEST_F(BeaconCacheRecordTest, getData)
{
	// when passing null as argument, then
	//auto config = new BeaconCacheRecord(0L, nullptr);
	//ASSERT_EQ(config->getData(), nullptr);

	// when passing an empty string as argument, then
	auto record = new BeaconCacheRecord(0L, "");
	core::UTF8String expectEmpty("");
	ASSERT_TRUE(record->getData().equals(expectEmpty));

	// when passing string as argument, then
	record = new BeaconCacheRecord(0L, "foobar");
	core::UTF8String expectFooBar("foobar");
	ASSERT_TRUE(record->getData().equals(expectFooBar));
}

TEST_F(BeaconCacheRecordTest, getTimestamp)
{
	// when passing negative timestamp, then
	auto record = new BeaconCacheRecord(-1L, "a");
	ASSERT_EQ(record->getTimestamp(), -1L);

	// and when passing zero as timestamp, then
	record = new BeaconCacheRecord(0, "a");
	ASSERT_EQ(record->getTimestamp(), 0L);

	// and when passing a positive timestamp, then
	record = new BeaconCacheRecord(1L,"a");
	ASSERT_EQ(record->getTimestamp(), 1L);
}

TEST_F(BeaconCacheRecordTest, getDataSizeInBytes)
{
	// when data is null, then
	//auto config = new BeaconCacheRecord(0L, nullptr);
	//ASSERT_EQ(config->getDataSizeInBytes(), 0L);

	// when data is an empty string, then
	auto record = new BeaconCacheRecord(0L, "");
	ASSERT_EQ(record->getDataSizeInBytes(), 0L);

	// when data is valid, then
	record = new BeaconCacheRecord(0L, "a");
	ASSERT_EQ(record->getDataSizeInBytes(), 1L);
	record = new BeaconCacheRecord(0L, "ab");
	ASSERT_EQ(record->getDataSizeInBytes(), 2L);
	record = new BeaconCacheRecord(0L, "abc");
	ASSERT_EQ(record->getDataSizeInBytes(), 3L);
}

TEST_F(BeaconCacheRecordTest, markForSending)
{
	// given
	auto target = new BeaconCacheRecord(0L, "abc");

	// then a newly created record is not marked for sending
	ASSERT_FALSE(target->isMarkedForSending());

	// and when explicitly marked for sending
	target->markForSending();

	// then
	ASSERT_TRUE(target->isMarkedForSending());

	// and when the sending mark is removed
	target->unsetSending();

	// then
	ASSERT_FALSE(target->isMarkedForSending());
}
