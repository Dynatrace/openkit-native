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
#include "protocol/TimeSyncResponse.h"

#include <cstdint>
#include <gtest/gtest.h>

using namespace core;
using namespace protocol;

class TimeSyncResponseTest : public testing::Test
{
public:
	void SetUp()
	{

	}

	void TearDown()
	{

	}
};

TEST_F(TimeSyncResponseTest, requestReceiveTimeDefault)
{
	UTF8String s("");
	uint32_t responseCode = 200;
	TimeSyncResponse timeSyncResponse = TimeSyncResponse(s, responseCode);

	EXPECT_EQ(-1, timeSyncResponse.getRequestReceiveTime());
}

TEST_F(TimeSyncResponseTest, requestReceiveTimeZero)
{
	UTF8String s("t1=0");
	uint32_t responseCode = 200;
	TimeSyncResponse timeSyncResponse = TimeSyncResponse(s, responseCode);

	EXPECT_EQ(0, timeSyncResponse.getRequestReceiveTime());
}

TEST_F(TimeSyncResponseTest, requestReceiveTimeOne)
{
	UTF8String s("t1=1");
	uint32_t responseCode = 200;
	TimeSyncResponse timeSyncResponse = TimeSyncResponse(s, responseCode);

	EXPECT_EQ(1, timeSyncResponse.getRequestReceiveTime());
}

TEST_F(TimeSyncResponseTest, requestReceiveTimeMinusOne)
{
	UTF8String s("t1=-1");
	uint32_t responseCode = 200;
	TimeSyncResponse timeSyncResponse = TimeSyncResponse(s, responseCode);

	EXPECT_EQ(-1, timeSyncResponse.getRequestReceiveTime());
}

TEST_F(TimeSyncResponseTest, requestReceiveTimeLongMax)
{
	UTF8String s("t1=9223372036854775807"); // signed int64 max
	uint32_t responseCode = 200;
	TimeSyncResponse timeSyncResponse = TimeSyncResponse(s, responseCode);

	EXPECT_EQ(9223372036854775807, timeSyncResponse.getRequestReceiveTime());
}

TEST_F(TimeSyncResponseTest, DISABLED_requestReceiveTimeLongMaxPlusOne)
{
	UTF8String s("t1=9223372036854775808"); // signed int64 max + 1
	uint32_t responseCode = 200;
	TimeSyncResponse timeSyncResponse = TimeSyncResponse(s, responseCode);

	EXPECT_EQ(9223372036854775807, timeSyncResponse.getRequestReceiveTime());
}

/// -----------------

TEST_F(TimeSyncResponseTest, responseSendTimeDefault)
{
	UTF8String s("");
	uint32_t responseCode = 200;
	TimeSyncResponse timeSyncResponse = TimeSyncResponse(s, responseCode);

	EXPECT_EQ(-1, timeSyncResponse.getResponseSendTime());
}

TEST_F(TimeSyncResponseTest, responseSendTimeZero)
{
	UTF8String s("t2=0");
	uint32_t responseCode = 200;
	TimeSyncResponse timeSyncResponse = TimeSyncResponse(s, responseCode);

	EXPECT_EQ(0, timeSyncResponse.getResponseSendTime());
}

TEST_F(TimeSyncResponseTest, responseSendTimeOne)
{
	UTF8String s("t2=1");
	uint32_t responseCode = 200;
	TimeSyncResponse timeSyncResponse = TimeSyncResponse(s, responseCode);

	EXPECT_EQ(1, timeSyncResponse.getResponseSendTime());
}

TEST_F(TimeSyncResponseTest, responseSendTimeMinusOne)
{
	UTF8String s("t2=-1");
	uint32_t responseCode = 200;
	TimeSyncResponse timeSyncResponse = TimeSyncResponse(s, responseCode);

	EXPECT_EQ(-1, timeSyncResponse.getResponseSendTime());
}

TEST_F(TimeSyncResponseTest, responseSendTimeLongMax)
{
	UTF8String s("t2=9223372036854775807"); // signed int64 max
	uint32_t responseCode = 200;
	TimeSyncResponse timeSyncResponse = TimeSyncResponse(s, responseCode);

	EXPECT_EQ(9223372036854775807, timeSyncResponse.getResponseSendTime());
}

TEST_F(TimeSyncResponseTest, DISABLED_responseSendTimeLongMaxPlusOne)
{
	UTF8String s("t2=9223372036854775808"); // signed int64 max + 1
	uint32_t responseCode = 200;
	TimeSyncResponse timeSyncResponse = TimeSyncResponse(s, responseCode);

	EXPECT_EQ(9223372036854775807, timeSyncResponse.getResponseSendTime());
}

// ---------

TEST_F(TimeSyncResponseTest, requestReceiveTimeTogetherWithResponseSendTime)
{
	UTF8String s("t1=123&t2=456");
	uint32_t responseCode = 200;
	TimeSyncResponse timeSyncResponse = TimeSyncResponse(s, responseCode);

	EXPECT_EQ(123, timeSyncResponse.getRequestReceiveTime());
	EXPECT_EQ(456, timeSyncResponse.getResponseSendTime());
}

TEST_F(TimeSyncResponseTest, notExistingKey)
{
	UTF8String s("hello=world");
	uint32_t responseCode = 200;
	TimeSyncResponse timeSyncResponse = TimeSyncResponse(s, responseCode);

	// verify all defaults
	EXPECT_EQ(-1, timeSyncResponse.getRequestReceiveTime());
	EXPECT_EQ(-1, timeSyncResponse.getResponseSendTime());
}