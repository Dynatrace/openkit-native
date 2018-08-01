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

TEST_F(StatusResponseTest, captureDefault)
{
	UTF8String s("");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCapture());
}

TEST_F(StatusResponseTest, captureEnabled)
{
	UTF8String s("cp=1");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCapture());
}

TEST_F(StatusResponseTest, captureDisabledWithZero)
{
	UTF8String s("cp=0");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_FALSE(statusResponse.isCapture());
}

TEST_F(StatusResponseTest, captureDisabledWithTwo)
{
	UTF8String s("cp=2");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_FALSE(statusResponse.isCapture());
}

TEST_F(StatusResponseTest, captureDisabledWithMinusOne)
{
	UTF8String s("cp=-1");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_FALSE(statusResponse.isCapture());
}

TEST_F(StatusResponseTest, captureDisabledWithSignedIntegerMax)
{
	UTF8String s("cp=2147483647");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_FALSE(statusResponse.isCapture());
}

TEST_F(StatusResponseTest, DISABLED_captureDisabledWithUnsignedIntegerMax)
{
	UTF8String s("cp=4294967295");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_FALSE(statusResponse.isCapture());
}

TEST_F(StatusResponseTest, DISABLED_captureDisabledWithUnsignedIntegerMaxPlusOne)
{
	UTF8String s("cp=4294967296");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_FALSE(statusResponse.isCapture());
}

TEST_F(StatusResponseTest, captureEnabledButThenTruncatedString)
{
	UTF8String s("cp=1&");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCapture());
}

TEST_F(StatusResponseTest, captureEnabledDefaultDueToTruncatedString)
{
	UTF8String s("cp=");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCapture());
}

/// -----------------

TEST_F(StatusResponseTest, sendIntervalDefault)
{
	UTF8String s("");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_EQ(-1, statusResponse.getSendInterval());
}

TEST_F(StatusResponseTest, sendIntervalZero)
{
	UTF8String s("si=0");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_EQ(0, statusResponse.getSendInterval());
}

TEST_F(StatusResponseTest, sendIntervalOne)
{
	UTF8String s("si=1");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_EQ(1000, statusResponse.getSendInterval());
}

TEST_F(StatusResponseTest, sendIntervalTwo)
{
	UTF8String s("si=1");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_EQ(1000, statusResponse.getSendInterval());
}

TEST_F(StatusResponseTest, sendIntervalMinusOne)
{
	UTF8String s("si=-1");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_EQ(-1000, statusResponse.getSendInterval());
}

TEST_F(StatusResponseTest, sendIntervalLarge)
{
	UTF8String s("si=2147483000"); // multiplied by 1000 will overflow the signed int
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_EQ(-648000, statusResponse.getSendInterval());
}

TEST_F(StatusResponseTest, DISABLED_sendIntervalSignedIntegerMax)
{
	UTF8String s("si=2147483647");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_EQ(-1, statusResponse.getSendInterval());
}

TEST_F(StatusResponseTest, DISABLED_sendIntervalUnsignedIntegerMax)
{
	UTF8String s("si=4294967295");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_EQ(-1, statusResponse.getSendInterval());
}

TEST_F(StatusResponseTest, sendIntervalTruncated)
{
	UTF8String s("si=");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_EQ(-1, statusResponse.getSendInterval());
}

/// -----------------

TEST_F(StatusResponseTest, monitoringNameDefault)
{
	UTF8String s("");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCapture());
	EXPECT_TRUE(statusResponse.getMonitorName().empty());
}

TEST_F(StatusResponseTest, monitoringNameValidAsciiName)
{
	UTF8String s("bn=HelloWorld");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCapture());
	EXPECT_TRUE(statusResponse.getMonitorName().equals("HelloWorld"));
}

TEST_F(StatusResponseTest, monitoringNameValidUtf8Name)
{
	UTF8String s(u8"bn=𐋏𝖾ll𝑜 𝙒ᴑ𝒓l𝖽 ｆ𝓻𝗈ｍ 𝒐ᴜ𝑡𝒆𝓇 𝕤𝟈𝛼𝘤ℯ");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCapture());
	UTF8String cmp(u8"𐋏𝖾ll𝑜 𝙒ᴑ𝒓l𝖽 ｆ𝓻𝗈ｍ 𝒐ᴜ𝑡𝒆𝓇 𝕤𝟈𝛼𝘤ℯ");
	EXPECT_TRUE(statusResponse.getMonitorName().equals(cmp));
}

/// -----------------

TEST_F(StatusResponseTest, serverIdDefault)
{
	UTF8String s("");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_EQ(-1, statusResponse.getServerID());
}

TEST_F(StatusResponseTest, serverIdOne)
{
	UTF8String s("id=1");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_EQ(1, statusResponse.getServerID());
}

TEST_F(StatusResponseTest, serverIdTwo)
{
	UTF8String s("id=2");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_EQ(2, statusResponse.getServerID());
}

TEST_F(StatusResponseTest, serverIdMinusOne)
{
	UTF8String s("id=-1");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_EQ(-1, statusResponse.getServerID());
}

TEST_F(StatusResponseTest, serverIdSignedIntegerMax)
{
	UTF8String s("id=2147483647");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_EQ(2147483647, statusResponse.getServerID());
}

TEST_F(StatusResponseTest, DISABLED_serverIdUnsignedIntegerMax)
{
	UTF8String s("id=4294967295");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_EQ(4294967295, statusResponse.getServerID());
}

TEST_F(StatusResponseTest, DISABLED_serverIdUnsignedIntegerMaxPlusOne)
{
	UTF8String s("id=4294967296");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_EQ(-1, statusResponse.getServerID());
}

/// -----------------

TEST_F(StatusResponseTest, maxBeaconSizeDefault)
{
	UTF8String s("");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_EQ(-1, statusResponse.getMaxBeaconSize());
}

TEST_F(StatusResponseTest, maxBeaconSizeOne)
{
	UTF8String s("bl=1");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_EQ(1, statusResponse.getMaxBeaconSize());
}

TEST_F(StatusResponseTest, maxBeaconSizeTwo)
{
	UTF8String s("bl=2");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_EQ(2, statusResponse.getMaxBeaconSize());
}

TEST_F(StatusResponseTest, maxBeaconSizeMinusOne)
{
	UTF8String s("bl=-1");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_EQ(-1, statusResponse.getMaxBeaconSize());
}

TEST_F(StatusResponseTest, maxBeaconSizeSignedIntegerMax)
{
	UTF8String s("bl=2147483647");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_EQ(2147483647, statusResponse.getMaxBeaconSize());
}

TEST_F(StatusResponseTest, DISABLED_maxBeaconSizeUnsignedIntegerMax)
{
	UTF8String s("bl=4294967295");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_EQ(4294967295, statusResponse.getMaxBeaconSize());
}

TEST_F(StatusResponseTest, DISABLED_maxBeaconSizeUnsignedIntegerMaxPlusOne)
{
	UTF8String s("bl=4294967296");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_EQ(-1, statusResponse.getMaxBeaconSize());
}

/// -----------------

TEST_F(StatusResponseTest, captureErrorsDefault)
{
	UTF8String s("");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCaptureErrors());
}

TEST_F(StatusResponseTest, captureErrorsDisabled)
{
	UTF8String s("er=0");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_FALSE(statusResponse.isCaptureErrors());
}

TEST_F(StatusResponseTest, captureErrorsEnabledWithOne)
{
	UTF8String s("er=1");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCaptureErrors());
}

TEST_F(StatusResponseTest, captureErrorsEnabledWithTwo)
{
	UTF8String s("er=2");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCaptureErrors());
}

TEST_F(StatusResponseTest, captureErrorsEnabledWithMinusOne)
{
	UTF8String s("er=-1");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCaptureErrors());
}

TEST_F(StatusResponseTest, captureErrorsEnabledWithSignedIntegerMax)
{
	UTF8String s("er=2147483647");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCaptureErrors());
}

TEST_F(StatusResponseTest, DISABLED_captureErrorsEnabledWithUnsignedIntegerMax)
{
	UTF8String s("er=4294967295");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCaptureErrors());
}

TEST_F(StatusResponseTest, DISABLED_captureErrorsEnabledWithUnsignedIntegerMaxPlusOne)
{
	UTF8String s("er=4294967296");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCaptureErrors());
}

TEST_F(StatusResponseTest, captureErrorsEnabledByTruncatedString)
{
	UTF8String s("er=");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCaptureErrors());
}

TEST_F(StatusResponseTest, captureErrorsEnabledButThenTruncatedString)
{
	UTF8String s("er=1&");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCaptureErrors());
}

/// -----------------

TEST_F(StatusResponseTest, captureCrashesDefault)
{
	UTF8String s("");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCaptureCrashes());
}

TEST_F(StatusResponseTest, captureCrashesDisabled)
{
	UTF8String s("cr=0");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_FALSE(statusResponse.isCaptureCrashes());
}

TEST_F(StatusResponseTest, captureCrashesEnabledWithOne)
{
	UTF8String s("cr=1");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCaptureCrashes());
}

TEST_F(StatusResponseTest, captureCrashesEnabledWithTwo)
{
	UTF8String s("cr=2");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCaptureCrashes());
}

TEST_F(StatusResponseTest, captureCrashesEnabledWithMinusOne)
{
	UTF8String s("er=-1");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCaptureCrashes());
}

TEST_F(StatusResponseTest, captureCrashesEnabledWithSignedIntegerMax)
{
	UTF8String s("cr=2147483647");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCaptureCrashes());
}

TEST_F(StatusResponseTest, DISABLED_captureCrashesEnabledWithUnsignedIntegerMax)
{
	UTF8String s("cr=4294967295");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCaptureCrashes());
}

TEST_F(StatusResponseTest, DISABLED_captureCrashesEnabledWithUnsignedIntegerMaxPlusOne)
{
	UTF8String s("cr=4294967296");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCaptureCrashes());
}

TEST_F(StatusResponseTest, captureCrashesEnabledByTruncatedString)
{
	UTF8String s("cr=");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCaptureCrashes());
}

TEST_F(StatusResponseTest, captureCrashesEnabledButThenTruncatedString)
{
	UTF8String s("cr=1&");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCaptureCrashes());
}

/// -----------------

TEST_F(StatusResponseTest, captureTogetherWithSendInterval)
{
	UTF8String s("cp=1&si=3");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCapture());
	EXPECT_EQ(3000, statusResponse.getSendInterval());
}

TEST_F(StatusResponseTest, captureTogetherWithCorruptSendInterval)
{
	UTF8String s("cp=1&=");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCapture());
	EXPECT_EQ(-1, statusResponse.getSendInterval());
}

TEST_F(StatusResponseTest, notExistingKey)
{
	UTF8String s("hello=world");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	// verify all defaults
	EXPECT_TRUE(statusResponse.isCapture());
	EXPECT_EQ(-1, statusResponse.getSendInterval());
	EXPECT_TRUE(statusResponse.getMonitorName().empty());
	EXPECT_EQ(-1, statusResponse.getServerID());
	EXPECT_EQ(-1, statusResponse.getMaxBeaconSize());
	EXPECT_TRUE(statusResponse.isCaptureErrors());
	EXPECT_TRUE(statusResponse.isCaptureCrashes());
}

TEST_F(StatusResponseTest, responseCodeIsSet)
{
	// given
	auto target = StatusResponse(UTF8String(), 418, Response::ResponseHeaders());

	// then
	ASSERT_EQ(418, target.getResponseCode());
}

TEST_F(StatusResponseTest, isErroneousResponseGivesTrueForErrorCodeEqualTo400)
{
	// given
	auto target = StatusResponse(UTF8String(), 400, Response::ResponseHeaders());

	// then
	ASSERT_TRUE(target.isErroneousResponse());
}

TEST_F(StatusResponseTest, isErroneousResponseGivesTrueForErrorCodeGreaterThan400)
{
	// given
	auto target = StatusResponse(UTF8String(), 401, Response::ResponseHeaders());

	// then
	ASSERT_TRUE(target.isErroneousResponse());
}

TEST_F(StatusResponseTest, isErroneousResponseGivesFalseForErrorCodeLessThan400)
{
	// given
	auto target = StatusResponse(UTF8String(), 399, Response::ResponseHeaders());

	// then
	ASSERT_FALSE(target.isErroneousResponse());
}

TEST_F(StatusResponseTest, headersAreSet)
{
	// given
	auto responseHeaders = Response::ResponseHeaders();
	responseHeaders["date"] = { "2018-07-18" };
	responseHeaders["content-type"] = { "application/json" };
	responseHeaders["X-Foo"] = { "bar" };
	auto target = StatusResponse(UTF8String(), 200, responseHeaders);

	// when, then
	ASSERT_EQ(responseHeaders, target.getResponseHeaders());
}

/// -----------------

TEST_F(StatusResponseTest, someTypicalStatusResponse)
{
	UTF8String s("cp=1&si=2&bn=MyName&id=5&bl=3072&er=1&cr=0");
	uint32_t responseCode = 200;
	StatusResponse statusResponse = StatusResponse(s, responseCode, Response::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCapture());
	EXPECT_EQ(2000, statusResponse.getSendInterval());
	EXPECT_TRUE(statusResponse.getMonitorName().equals("MyName"));
	EXPECT_EQ(5, statusResponse.getServerID());
	EXPECT_EQ(3072, statusResponse.getMaxBeaconSize());
	EXPECT_TRUE(statusResponse.isCaptureErrors());
	EXPECT_FALSE(statusResponse.isCaptureCrashes());
}
