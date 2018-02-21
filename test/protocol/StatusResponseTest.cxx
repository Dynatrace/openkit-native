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
#include "protocol/StatusResponse.h"

#include <cstdint>
#include <gtest/gtest.h>

using namespace core;
using namespace protocol;

class StatusResponseTest : public testing::Test
{
public:
	void SetUp()
	{

	}

	void TearDown()
	{

	}
};

TEST_F(StatusResponseTest, AllDefaultsStatusResponse)
{
	UTF8String s("");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_TRUE(statusResponse.isCapture());
	EXPECT_EQ(-1, statusResponse.getSendInterval());
	EXPECT_TRUE(statusResponse.getMonitorName().empty());
	EXPECT_EQ(-1, statusResponse.getServerID());
	EXPECT_EQ(-1, statusResponse.getMaxBeaconSize());
	EXPECT_TRUE(statusResponse.isCaptureErrors());
	EXPECT_TRUE(statusResponse.isCaptureCrashes());
}

TEST_F(StatusResponseTest, SomeTypicalStatusResponse)
{
	UTF8String s("cp=1&si=2&bn=MyName&id=5&bl=3072&er=1&cr=0");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_TRUE(statusResponse.isCapture());
	EXPECT_EQ(2000, statusResponse.getSendInterval());
	EXPECT_TRUE(statusResponse.getMonitorName().compare("MyName"));
	EXPECT_EQ(5, statusResponse.getServerID());
	EXPECT_EQ(3072, statusResponse.getMaxBeaconSize());
	EXPECT_TRUE(statusResponse.isCaptureErrors());
	EXPECT_FALSE(statusResponse.isCaptureCrashes());
}

TEST_F(StatusResponseTest, CorruptStatusResponse)
{
	UTF8String s("cp=");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_TRUE(statusResponse.isCapture());
	EXPECT_EQ(-1, statusResponse.getSendInterval());
	EXPECT_TRUE(statusResponse.getMonitorName().empty());
	EXPECT_EQ(-1, statusResponse.getServerID());
	EXPECT_EQ(-1, statusResponse.getMaxBeaconSize());
	EXPECT_TRUE(statusResponse.isCaptureErrors());
	EXPECT_TRUE(statusResponse.isCaptureCrashes());
}