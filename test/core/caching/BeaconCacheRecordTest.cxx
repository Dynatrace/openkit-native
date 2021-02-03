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
#include "core/caching/BeaconCacheRecord.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using BeaconCacheRecord_t = core::caching::BeaconCacheRecord;
using Utf8String_t = core::UTF8String;

class BeaconCacheRecordTest : public testing::Test
{
};

TEST_F(BeaconCacheRecordTest, getData)
{
	// when passing an empty string as argument, then
	BeaconCacheRecord_t record(0L, "");
	ASSERT_THAT(record.getData(), testing::Eq(""));

	// when passing string as argument, then
	BeaconCacheRecord_t record2(0L, "foobar");
	ASSERT_THAT(record2.getData(), testing::Eq("foobar"));
}

TEST_F(BeaconCacheRecordTest, getTimestamp)
{
	// when passing negative timestamp, then
	BeaconCacheRecord_t record(-1L, "a");
	ASSERT_THAT(record.getTimestamp(), testing::Eq(-1L));

	// and when passing zero as timestamp, then
	BeaconCacheRecord_t record2(0, "a");
	ASSERT_THAT(record2.getTimestamp(), testing::Eq(0L));

	// and when passing a positive timestamp, then
	BeaconCacheRecord_t record3(1L,"a");
	ASSERT_THAT(record3.getTimestamp(), testing::Eq(1L));
}

TEST_F(BeaconCacheRecordTest, getDataSizeInBytes)
{
	// when data is an empty string, then
	BeaconCacheRecord_t record(0L, "");
	ASSERT_THAT(record.getDataSizeInBytes(), testing::Eq(0L));

	// when data is valid, then
	BeaconCacheRecord_t record2(0L, "a");
	ASSERT_THAT(record2.getDataSizeInBytes(), testing::Eq(1L));
	BeaconCacheRecord_t record3(0L, "ab");
	ASSERT_THAT(record3.getDataSizeInBytes(), testing::Eq(2L));
	BeaconCacheRecord_t record4(0L, "abc");
	ASSERT_THAT(record4.getDataSizeInBytes(), testing::Eq(3L));
	BeaconCacheRecord_t record5(0L, "\xf0\x9d\x84\x9f"); // "Musical Symbol G Clef Ottava Alta" encoded in UTF8
	ASSERT_THAT(record5.getDataSizeInBytes(), testing::Eq(4L));
}

TEST_F(BeaconCacheRecordTest, markForSending)
{
	// given
	BeaconCacheRecord_t target(0L, "abc");

	// then a newly created record is not marked for sending
	ASSERT_THAT(target.isMarkedForSending(), testing::Eq(false));

	// and when explicitly marked for sending
	target.markForSending();

	// then
	ASSERT_THAT(target.isMarkedForSending(), testing::Eq(true));

	// and when the sending mark is removed
	target.unsetSending();

	// then
	ASSERT_THAT(target.isMarkedForSending(), testing::Eq(false));
}

TEST_F(BeaconCacheRecordTest, twoInstancesAreEqualIfFieldsAreEqual)
{
	// given
	BeaconCacheRecord_t lhs(1234L, "abc");
	BeaconCacheRecord_t rhs(1234L, "abc");

	// then
	ASSERT_THAT(lhs == rhs, testing::Eq(true));

	// and when setting send flag
	lhs.markForSending();
	rhs.markForSending();

	// then
	ASSERT_THAT(lhs == rhs, testing::Eq(true));
}

TEST_F(BeaconCacheRecordTest, twoInstancesAreNotInequalIfFieldsAreEqual)
{
	// given
	BeaconCacheRecord_t lhs(1234L, "abc");
	BeaconCacheRecord_t rhs(1234L, "abc");

	// then
	ASSERT_THAT(lhs != rhs, testing::Eq(false));

	// and when setting send flag
	lhs.markForSending();
	rhs.markForSending();

	// then
	ASSERT_THAT(lhs != rhs, testing::Eq(false));
}

TEST_F(BeaconCacheRecordTest, twoInstancesAreNotEqualIfTimestampDiffers)
{
	// given
	BeaconCacheRecord_t lhs(1234L, "abc");
	BeaconCacheRecord_t rhs(4321L, "abc");

	// then
	ASSERT_THAT(lhs == rhs, testing::Eq(false));
}

TEST_F(BeaconCacheRecordTest, twoInstancesAreInequalIfTimestampDiffers)
{
	// given
	BeaconCacheRecord_t lhs(1234L, "abc");
	BeaconCacheRecord_t rhs(4321L, "abc");

	// then
	ASSERT_THAT(lhs != rhs, testing::Eq(true));
}

TEST_F(BeaconCacheRecordTest, twoInstancesAreNotEqualIfDataDiffers)
{
	// given
	BeaconCacheRecord_t lhs(1234L, "abc");
	BeaconCacheRecord_t rhs(1234L, "abcd");

	// then
	ASSERT_THAT(lhs == rhs, testing::Eq(false));
}

TEST_F(BeaconCacheRecordTest, twoInstancesAreInequalIfDataDiffers)
{
	// given
	BeaconCacheRecord_t lhs(1234L, "abc");
	BeaconCacheRecord_t rhs(1234L, "abcd");

	// then
	ASSERT_THAT(lhs != rhs, testing::Eq(true));
}

TEST_F(BeaconCacheRecordTest, twoInstancesAreNotEqualIfMarkedForSendingDiffers)
{
	// given
	BeaconCacheRecord_t lhs(1234L, "abc");
	BeaconCacheRecord_t rhs(1234L, "abc");
	rhs.markForSending();

	// then
	ASSERT_THAT(lhs == rhs, testing::Eq(false));
}

TEST_F(BeaconCacheRecordTest, twoInstancesAreInequalIfMarkedForSendingDiffers)
{
	// given
	BeaconCacheRecord_t lhs(1234L, "abc");
	BeaconCacheRecord_t rhs(1234L, "abc");
	rhs.markForSending();

	// then
	ASSERT_THAT(lhs != rhs, testing::Eq(true));
}
