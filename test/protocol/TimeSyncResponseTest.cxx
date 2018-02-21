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

TEST_F(TimeSyncResponseTest, AllDefaultsTimeSyncResponse)
{
	UTF8String s("");
	uint32_t responseCode = 200;
	TimeSyncResponse timeSyncResponse = TimeSyncResponse(s, responseCode);

	EXPECT_EQ(-1, timeSyncResponse.getRequestReceiveTime());
	EXPECT_EQ(-1, (int)timeSyncResponse.getResponseSendTime());
}

TEST_F(TimeSyncResponseTest, SomeTypicalTimeSyncResponse)
{
	UTF8String s("t1=123&t2=456");
	uint32_t responseCode = 200;
	TimeSyncResponse timeSyncResponse = TimeSyncResponse(s, responseCode);

	EXPECT_EQ(123, timeSyncResponse.getRequestReceiveTime());
	EXPECT_EQ(456, timeSyncResponse.getResponseSendTime());
}

TEST_F(TimeSyncResponseTest, CorruptTimeSyncResponse)
{
	UTF8String s("t1=123&");
	uint32_t responseCode = 200;
	TimeSyncResponse timeSyncResponse = TimeSyncResponse(s, responseCode);

	EXPECT_EQ(123, timeSyncResponse.getRequestReceiveTime());
	EXPECT_EQ(-1, (int)timeSyncResponse.getResponseSendTime());
}