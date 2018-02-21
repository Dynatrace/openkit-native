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

TEST_F(StatusResponseTest, CaptureDefault)
{
	UTF8String s("");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_TRUE(statusResponse.isCapture());
}

TEST_F(StatusResponseTest, CaptureEnabled)
{
	UTF8String s("cp=1");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_TRUE(statusResponse.isCapture());
}

TEST_F(StatusResponseTest, CaptureDisabledWithZero)
{
	UTF8String s("cp=0");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_FALSE(statusResponse.isCapture());
}

TEST_F(StatusResponseTest, CaptureDisabledWithTwo)
{
	UTF8String s("cp=2");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_FALSE(statusResponse.isCapture());
}

TEST_F(StatusResponseTest, CaptureDisabledWithMinusOne)
{
	UTF8String s("cp=-1");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_FALSE(statusResponse.isCapture());
}

TEST_F(StatusResponseTest, CaptureDisabledWithSignedIntegerMax)
{
	UTF8String s("cp=2147483647");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_FALSE(statusResponse.isCapture());
}

TEST_F(StatusResponseTest, DISABLED_CaptureDisabledWithUnsignedIntegerMax)
{
	UTF8String s("cp=4294967295");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_FALSE(statusResponse.isCapture());
}

TEST_F(StatusResponseTest, DISABLED_CaptureDisabledWithUnsignedIntegerMaxPlusOne)
{
	UTF8String s("cp=4294967296");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_FALSE(statusResponse.isCapture());
}

TEST_F(StatusResponseTest, CaptureEnabledButThenTruncatedString)
{
	UTF8String s("cp=1&");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_TRUE(statusResponse.isCapture());
}

TEST_F(StatusResponseTest, CaptureEnabledDefaultDueToTruncatedString)
{
	UTF8String s("cp=");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_TRUE(statusResponse.isCapture());
}

/// -----------------

TEST_F(StatusResponseTest, SendIntervalDefault)
{
	UTF8String s("");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_EQ(-1, statusResponse.getSendInterval());
}

TEST_F(StatusResponseTest, SendIntervalZero)
{
	UTF8String s("si=0");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_EQ(0, statusResponse.getSendInterval());
}

TEST_F(StatusResponseTest, SendIntervalOne)
{
	UTF8String s("si=1");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_EQ(1000, statusResponse.getSendInterval());
}

TEST_F(StatusResponseTest, SendIntervalTwo)
{
	UTF8String s("si=1");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_EQ(1000, statusResponse.getSendInterval());
}

TEST_F(StatusResponseTest, SendIntervalMinusOne)
{
	UTF8String s("si=-1");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_EQ(-1000, statusResponse.getSendInterval());
}

TEST_F(StatusResponseTest, SendIntervalLarge)
{
	UTF8String s("si=2147483000"); // multiplied by 1000 will overflow the signed int
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_EQ(-648000, statusResponse.getSendInterval());
}

TEST_F(StatusResponseTest, DISABLED_SendIntervalSignedIntegerMax)
{
	UTF8String s("si=2147483647");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_EQ(-1, statusResponse.getSendInterval());
}

TEST_F(StatusResponseTest, DISABLED_SendIntervalUnsignedIntegerMax)
{
	UTF8String s("si=4294967295");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_EQ(-1, statusResponse.getSendInterval());
}

TEST_F(StatusResponseTest, SendIntervalTruncated)
{
	UTF8String s("si=");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_EQ(-1, statusResponse.getSendInterval());
}

/// -----------------

TEST_F(StatusResponseTest, MonitoringNameDefault)
{
	UTF8String s("");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_TRUE(statusResponse.isCapture());
	EXPECT_TRUE(statusResponse.getMonitorName().empty());
}

TEST_F(StatusResponseTest, MonitoringNameValidAsciiName)
{
	UTF8String s("bn=HelloWorld");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_TRUE(statusResponse.isCapture());
	EXPECT_TRUE(statusResponse.getMonitorName().compare("HelloWorld"));
}

TEST_F(StatusResponseTest, MonitoringNameValidUtf8Name)
{
	UTF8String s(u8"bn=𐋏𝖾ll𝑜 𝙒ᴑ𝒓l𝖽 ｆ𝓻𝗈ｍ 𝒐ᴜ𝑡𝒆𝓇 𝕤𝟈𝛼𝘤ℯ");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_TRUE(statusResponse.isCapture());
	UTF8String cmp(u8"𐋏𝖾ll𝑜 𝙒ᴑ𝒓l𝖽 ｆ𝓻𝗈ｍ 𝒐ᴜ𝑡𝒆𝓇 𝕤𝟈𝛼𝘤ℯ");
	EXPECT_TRUE(statusResponse.getMonitorName().compare(cmp));
}

/// -----------------

TEST_F(StatusResponseTest, ServerIdDefault)
{
	UTF8String s("");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_EQ(-1, statusResponse.getServerID());
}

TEST_F(StatusResponseTest, ServerIdOne)
{
	UTF8String s("id=1");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_EQ(1, statusResponse.getServerID());
}

TEST_F(StatusResponseTest, ServerIdTwo)
{
	UTF8String s("id=2");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_EQ(2, statusResponse.getServerID());
}

TEST_F(StatusResponseTest, ServerIdMinusOne)
{
	UTF8String s("id=-1");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_EQ(-1, statusResponse.getServerID());
}

TEST_F(StatusResponseTest, ServerIdSignedIntegerMax)
{
	UTF8String s("id=2147483647");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_EQ(2147483647, statusResponse.getServerID());
}

TEST_F(StatusResponseTest, DISABLED_ServerIdUnsignedIntegerMax)
{
	UTF8String s("id=4294967295");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_EQ(4294967295, statusResponse.getServerID());
}

TEST_F(StatusResponseTest, DISABLED_ServerIdUnsignedIntegerMaxPlusOne)
{
	UTF8String s("id=4294967296");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_EQ(-1, statusResponse.getServerID());
}

/// -----------------

TEST_F(StatusResponseTest, MaxBeaconSizeDefault)
{
	UTF8String s("");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_EQ(-1, statusResponse.getMaxBeaconSize());
}

TEST_F(StatusResponseTest, MaxBeaconSizeOne)
{
	UTF8String s("bl=1");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_EQ(1, statusResponse.getMaxBeaconSize());
}

TEST_F(StatusResponseTest, MaxBeaconSizeTwo)
{
	UTF8String s("bl=2");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_EQ(2, statusResponse.getMaxBeaconSize());
}

TEST_F(StatusResponseTest, MaxBeaconSizeMinusOne)
{
	UTF8String s("bl=-1");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_EQ(-1, statusResponse.getMaxBeaconSize());
}

TEST_F(StatusResponseTest, MaxBeaconSizeSignedIntegerMax)
{
	UTF8String s("bl=2147483647");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_EQ(2147483647, statusResponse.getMaxBeaconSize());
}

TEST_F(StatusResponseTest, DISABLED_MaxBeaconSizeUnsignedIntegerMax)
{
	UTF8String s("bl=4294967295");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_EQ(4294967295, statusResponse.getMaxBeaconSize());
}

TEST_F(StatusResponseTest, DISABLED_MaxBeaconSizeUnsignedIntegerMaxPlusOne)
{
	UTF8String s("bl=4294967296");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_EQ(-1, statusResponse.getMaxBeaconSize());
}

/// -----------------

TEST_F(StatusResponseTest, CaptureErrorsDefault)
{
	UTF8String s("");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_TRUE(statusResponse.isCaptureErrors());
}

TEST_F(StatusResponseTest, CaptureErrorsDisabled)
{
	UTF8String s("er=0");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_FALSE(statusResponse.isCaptureErrors());
}

TEST_F(StatusResponseTest, CaptureErrorsEnabledWithOne)
{
	UTF8String s("er=1");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_TRUE(statusResponse.isCaptureErrors());
}

TEST_F(StatusResponseTest, CaptureErrorsEnabledWithTwo)
{
	UTF8String s("er=2");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_TRUE(statusResponse.isCaptureErrors());
}

TEST_F(StatusResponseTest, CaptureErrorsEnabledWithMinusOne)
{
	UTF8String s("er=-1");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_TRUE(statusResponse.isCaptureErrors());
}

TEST_F(StatusResponseTest, CaptureErrorsEnabledWithSignedIntegerMax)
{
	UTF8String s("er=2147483647");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_TRUE(statusResponse.isCaptureErrors());
}

TEST_F(StatusResponseTest, DISABLED_CaptureErrorsEnabledWithUnsignedIntegerMax)
{
	UTF8String s("er=4294967295");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_TRUE(statusResponse.isCaptureErrors());
}

TEST_F(StatusResponseTest, DISABLED_CaptureErrorsEnabledWithUnsignedIntegerMaxPlusOne)
{
	UTF8String s("er=4294967296");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_TRUE(statusResponse.isCaptureErrors());
}

TEST_F(StatusResponseTest, CaptureErrorsEnabledByTruncatedString)
{
	UTF8String s("er=");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_TRUE(statusResponse.isCaptureErrors());
}

TEST_F(StatusResponseTest, CaptureErrorsEnabledButThenTruncatedString)
{
	UTF8String s("er=1&");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_TRUE(statusResponse.isCaptureErrors());
}

/// -----------------

TEST_F(StatusResponseTest, CaptureCrashesDefault)
{
	UTF8String s("");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_TRUE(statusResponse.isCaptureCrashes());
}

TEST_F(StatusResponseTest, CaptureCrashesDisabled)
{
	UTF8String s("cr=0");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_FALSE(statusResponse.isCaptureCrashes());
}

TEST_F(StatusResponseTest, CaptureCrashesEnabledWithOne)
{
	UTF8String s("cr=1");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_TRUE(statusResponse.isCaptureCrashes());
}

TEST_F(StatusResponseTest, CaptureCrashesEnabledWithTwo)
{
	UTF8String s("cr=2");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_TRUE(statusResponse.isCaptureCrashes());
}

TEST_F(StatusResponseTest, CaptureCrashesEnabledWithMinusOne)
{
	UTF8String s("er=-1");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_TRUE(statusResponse.isCaptureCrashes());
}

TEST_F(StatusResponseTest, CaptureCrashesEnabledWithSignedIntegerMax)
{
	UTF8String s("cr=2147483647");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_TRUE(statusResponse.isCaptureCrashes());
}

TEST_F(StatusResponseTest, DISABLED_CaptureCrashesEnabledWithUnsignedIntegerMax)
{
	UTF8String s("cr=4294967295");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_TRUE(statusResponse.isCaptureCrashes());
}

TEST_F(StatusResponseTest, DISABLED_CaptureCrashesEnabledWithUnsignedIntegerMaxPlusOne)
{
	UTF8String s("cr=4294967296");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_TRUE(statusResponse.isCaptureCrashes());
}

TEST_F(StatusResponseTest, CaptureCrashesEnabledByTruncatedString)
{
	UTF8String s("cr=");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_TRUE(statusResponse.isCaptureCrashes());
}

TEST_F(StatusResponseTest, CaptureCrashesEnabledButThenTruncatedString)
{
	UTF8String s("cr=1&");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_TRUE(statusResponse.isCaptureCrashes());
}

/// -----------------

TEST_F(StatusResponseTest, CaptureTogetherWithSendInterval)
{
	UTF8String s("cp=1&si=3");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_TRUE(statusResponse.isCapture());
	EXPECT_EQ(3000, statusResponse.getSendInterval());
}

TEST_F(StatusResponseTest, CaptureTogetherWithCorruptSendInterval)
{
	UTF8String s("cp=1&=");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	EXPECT_TRUE(statusResponse.isCapture());
	EXPECT_EQ(-1, statusResponse.getSendInterval());
}

TEST_F(StatusResponseTest, NotExistingKey)
{
	UTF8String s("hello=world");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode);

	// verify all defaults
	EXPECT_TRUE(statusResponse.isCapture());
	EXPECT_EQ(-1, statusResponse.getSendInterval());
	EXPECT_TRUE(statusResponse.getMonitorName().empty());
	EXPECT_EQ(-1, statusResponse.getServerID());
	EXPECT_EQ(-1, statusResponse.getMaxBeaconSize());
	EXPECT_TRUE(statusResponse.isCaptureErrors());
	EXPECT_TRUE(statusResponse.isCaptureCrashes());
}

/// -----------------

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
