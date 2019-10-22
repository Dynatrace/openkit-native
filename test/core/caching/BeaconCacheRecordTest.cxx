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

#include "core/UTF8String.h"
#include "core/caching/BeaconCacheRecord.h"

#include "gtest/gtest.h"

using BeaconCacheRecord_t = core::caching::BeaconCacheRecord;
using Utf8String_t = core::UTF8String;

class BeaconCacheRecordTest : public testing::Test
{
};

TEST_F(BeaconCacheRecordTest, getData)
{
	// when passing an empty string as argument, then
	BeaconCacheRecord_t record(0L, "");
	Utf8String_t expectEmpty("");
	ASSERT_TRUE(record.getData().equals(expectEmpty));

	// when passing string as argument, then
	BeaconCacheRecord_t record2(0L, "foobar");
	Utf8String_t expectFooBar("foobar");
	ASSERT_TRUE(record2.getData().equals(expectFooBar));
}

TEST_F(BeaconCacheRecordTest, getTimestamp)
{
	// when passing negative timestamp, then
	BeaconCacheRecord_t record(-1L, "a");
	ASSERT_EQ(record.getTimestamp(), -1L);

	// and when passing zero as timestamp, then
	BeaconCacheRecord_t record2(0, "a");
	ASSERT_EQ(record2.getTimestamp(), 0L);

	// and when passing a positive timestamp, then
	BeaconCacheRecord_t record3(1L,"a");
	ASSERT_EQ(record3.getTimestamp(), 1L);
}

TEST_F(BeaconCacheRecordTest, getDataSizeInBytes)
{
	// when data is an empty string, then
	BeaconCacheRecord_t record(0L, "");
	ASSERT_EQ(record.getDataSizeInBytes(), 0L);

	// when data is valid, then
	BeaconCacheRecord_t record2(0L, "a");
	ASSERT_EQ(record2.getDataSizeInBytes(), 1L);
	BeaconCacheRecord_t record3(0L, "ab");
	ASSERT_EQ(record3.getDataSizeInBytes(), 2L);
	BeaconCacheRecord_t record4(0L, "abc");
	ASSERT_EQ(record4.getDataSizeInBytes(), 3L);
}

TEST_F(BeaconCacheRecordTest, markForSending)
{
	// given
	BeaconCacheRecord_t target(0L, "abc");

	// then a newly created record is not marked for sending
	ASSERT_FALSE(target.isMarkedForSending());

	// and when explicitly marked for sending
	target.markForSending();

	// then
	ASSERT_TRUE(target.isMarkedForSending());

	// and when the sending mark is removed
	target.unsetSending();

	// then
	ASSERT_FALSE(target.isMarkedForSending());
}
