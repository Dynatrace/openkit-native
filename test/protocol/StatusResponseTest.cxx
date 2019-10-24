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
#include "protocol/IStatusResponse.h"
#include "protocol/StatusResponse.h"

#include "../api/mock/MockILogger.h"

#include "gtest/gtest.h"

#include <cstdint>

using namespace test;

using IStatusResponse_t = protocol::IStatusResponse;
using MockNiceILogger_sp = std::shared_ptr<testing::NiceMock<MockILogger>>;
using StatusResponse_t = protocol::StatusResponse;
using Utf8String_t = core::UTF8String;

static constexpr char RESPONSE_KEY_RETRY_AFTER[] = "retry-after";
static constexpr int64_t DEFAULT_RETRY_AFTER_IN_MILLISECONDS = 10L * 60L * 1000L;

class StatusResponseTest : public testing::Test
{
protected:
	MockNiceILogger_sp logger;

	void SetUp()
	{
		logger = MockILogger::createNice();
	}
};

TEST_F(StatusResponseTest, isErroneousResponseGivesFlaseForResponseCodesLessThan400)
{
	// given
	auto target = StatusResponse_t(logger, "", 399, IStatusResponse_t::ResponseHeaders());

	// then
	ASSERT_THAT(target.isErroneousResponse(), testing::Eq(false));
}

TEST_F(StatusResponseTest, isErroneousResponseGivesTrueForResponseCodesEqualTo400)
{
	// given
	auto target = StatusResponse_t(logger, "", 400, IStatusResponse_t::ResponseHeaders());

	// then
	ASSERT_THAT(target.isErroneousResponse(), testing::Eq(true));
}

TEST_F(StatusResponseTest, isErroneousResponseGivesTrueForResponseCodesGreaterThan400)
{
	// given
	auto target = StatusResponse_t(logger, "", 401, IStatusResponse_t::ResponseHeaders());

	// then
	ASSERT_THAT(target.isErroneousResponse(), testing::Eq(true));
}

TEST_F(StatusResponseTest, isErroneousResponseGivesTrueForErrorCodeEqualTo400)
{
	// given
	auto target = StatusResponse_t(logger, "", 400, IStatusResponse_t::ResponseHeaders());

	// then
	ASSERT_TRUE(target.isErroneousResponse());
}

TEST_F(StatusResponseTest, isErroneousResponseGivesTrueForErrorCodeGreaterThan400)
{
	// given
	auto target = StatusResponse_t(logger, "", 401, IStatusResponse_t::ResponseHeaders());

	// then
	ASSERT_TRUE(target.isErroneousResponse());
}

TEST_F(StatusResponseTest, isErroneousResponseGivesFalseForErrorCodeLessThan400)
{
	// given
	auto target = StatusResponse_t(logger, "", 399, IStatusResponse_t::ResponseHeaders());

	// then
	ASSERT_FALSE(target.isErroneousResponse());
}

TEST_F(StatusResponseTest, isTooManyRequestsResponseGivesTrueIfResponseCodeIsEqualTo429)
{
	// given
	auto target = StatusResponse_t(logger, "", 429, IStatusResponse_t::ResponseHeaders());

	// then
	ASSERT_TRUE(target.isTooManyRequestsResponse());
}

TEST_F(StatusResponseTest, isTooManyRequestsResponseGivesFalseIfResponseCodeIsNotEqualTo429)
{
	// given
	auto target = StatusResponse_t(logger, "", 404, IStatusResponse_t::ResponseHeaders());

	// then
	ASSERT_FALSE(target.isTooManyRequestsResponse());
}

TEST_F(StatusResponseTest, responseCodeIsSet)
{
	// given
	auto target = StatusResponse_t(logger, "", 418, IStatusResponse_t::ResponseHeaders());

	// then
	ASSERT_EQ(418, target.getResponseCode());
}

TEST_F(StatusResponseTest, headersAreSet)
{
	// given
	auto headers = IStatusResponse_t::ResponseHeaders
	{
		{ "X-Foo", std::vector<std::string> { "X-BAR" } },
		{ "X-YZ", std::vector<std::string> { } }
	};
	auto target = StatusResponse_t(logger, "", 418, headers);

	// then
	ASSERT_EQ(headers, target.getResponseHeaders());
}

TEST_F(StatusResponseTest, getRetryAfterReturnsDefaultValueIfResponseKeyDoesNotExist)
{
	// given
	auto target = StatusResponse_t(logger, "", 429, IStatusResponse_t::ResponseHeaders());

	// when
	auto obtained = target.getRetryAfterInMilliseconds();

	// then
	ASSERT_EQ(DEFAULT_RETRY_AFTER_IN_MILLISECONDS, obtained);
}

TEST_F(StatusResponseTest, getRetryAfterReturnsDefaultValueIfMultipleValuesWereRetrieved)
{
	// given
	auto responseHeaders = IStatusResponse_t::ResponseHeaders
	{
		{ RESPONSE_KEY_RETRY_AFTER, std::vector<std::string>{ "100", "200" } }
	};
	auto target = StatusResponse_t(logger, "", 429, responseHeaders);

	// when
	auto obtained = target.getRetryAfterInMilliseconds();

	// then
	ASSERT_EQ(DEFAULT_RETRY_AFTER_IN_MILLISECONDS, obtained);
}

TEST_F(StatusResponseTest, getRetryAfterReturnsDefaultValueIfValueIsNotAnIntegerValue)
{
	// given
	auto responseHeaders = IStatusResponse_t::ResponseHeaders
	{
		{ RESPONSE_KEY_RETRY_AFTER, std::vector<std::string>{ "a" } }
	};
	auto target = StatusResponse_t(logger, "", 429, responseHeaders);

	// when
	auto obtained = target.getRetryAfterInMilliseconds();

	// then
	ASSERT_EQ(DEFAULT_RETRY_AFTER_IN_MILLISECONDS, obtained);
}

TEST_F(StatusResponseTest, getRetryAfterReturnsDefaultValueIfValueIsOutOfIntegerRange)
{
	// given
	auto responseHeaders = IStatusResponse_t::ResponseHeaders
	{
		// use string value 2^31, which is one too high for int32_t
		{ RESPONSE_KEY_RETRY_AFTER, std::vector<std::string>{ "2147483648" } }
	};
	auto target = StatusResponse_t(logger, "", 429, responseHeaders);

	// when
	auto obtained = target.getRetryAfterInMilliseconds();

	// then
	ASSERT_EQ(DEFAULT_RETRY_AFTER_IN_MILLISECONDS, obtained);
}

TEST_F(StatusResponseTest, getRetryAfterReturnsParsedValue)
{
	// given
	auto responseHeaders = IStatusResponse_t::ResponseHeaders
	{
		{ RESPONSE_KEY_RETRY_AFTER, std::vector<std::string>{ "1234" } }
	};
	auto target = StatusResponse_t(logger, "", 429, responseHeaders);

	// when
	auto obtained = target.getRetryAfterInMilliseconds();

	// then
	ASSERT_EQ(1234L * 1000L, obtained);
}

TEST_F(StatusResponseTest, captureDefault)
{
	Utf8String_t s("");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCapture());
}

TEST_F(StatusResponseTest, captureEnabled)
{
	Utf8String_t s("cp=1");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCapture());
}

TEST_F(StatusResponseTest, captureDisabledWithZero)
{
	Utf8String_t s("cp=0");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_FALSE(statusResponse.isCapture());
}

TEST_F(StatusResponseTest, captureDisabledWithTwo)
{
	Utf8String_t s("cp=2");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_FALSE(statusResponse.isCapture());
}

TEST_F(StatusResponseTest, captureDisabledWithMinusOne)
{
	Utf8String_t s("cp=-1");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_FALSE(statusResponse.isCapture());
}

TEST_F(StatusResponseTest, captureDisabledWithSignedIntegerMax)
{
	Utf8String_t s("cp=2147483647");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_FALSE(statusResponse.isCapture());
}

TEST_F(StatusResponseTest, DISABLED_captureDisabledWithUnsignedIntegerMax)
{
	Utf8String_t s("cp=4294967295");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_FALSE(statusResponse.isCapture());
}

TEST_F(StatusResponseTest, DISABLED_captureDisabledWithUnsignedIntegerMaxPlusOne)
{
	Utf8String_t s("cp=4294967296");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_FALSE(statusResponse.isCapture());
}

TEST_F(StatusResponseTest, captureEnabledButThenTruncatedString)
{
	Utf8String_t s("cp=1&");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCapture());
}

TEST_F(StatusResponseTest, captureEnabledDefaultDueToTruncatedString)
{
	Utf8String_t s("cp=");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCapture());
}

/// -----------------

TEST_F(StatusResponseTest, sendIntervalDefault)
{
	Utf8String_t s("");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_EQ(-1, statusResponse.getSendInterval());
}

TEST_F(StatusResponseTest, sendIntervalZero)
{
	Utf8String_t s("si=0");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_EQ(0, statusResponse.getSendInterval());
}

TEST_F(StatusResponseTest, sendIntervalOne)
{
	Utf8String_t s("si=1");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_EQ(1000, statusResponse.getSendInterval());
}

TEST_F(StatusResponseTest, sendIntervalTwo)
{
	Utf8String_t s("si=1");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_EQ(1000, statusResponse.getSendInterval());
}

TEST_F(StatusResponseTest, sendIntervalMinusOne)
{
	Utf8String_t s("si=-1");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_EQ(-1000, statusResponse.getSendInterval());
}

TEST_F(StatusResponseTest, sendIntervalLarge)
{
	Utf8String_t s("si=2147483000"); // multiplied by 1000 will overflow the signed int
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_EQ(-648000, statusResponse.getSendInterval());
}

TEST_F(StatusResponseTest, DISABLED_sendIntervalSignedIntegerMax)
{
	Utf8String_t s("si=2147483647");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_EQ(-1, statusResponse.getSendInterval());
}

TEST_F(StatusResponseTest, DISABLED_sendIntervalUnsignedIntegerMax)
{
	Utf8String_t s("si=4294967295");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_EQ(-1, statusResponse.getSendInterval());
}

TEST_F(StatusResponseTest, sendIntervalTruncated)
{
	Utf8String_t s("si=");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_EQ(-1, statusResponse.getSendInterval());
}

/// -----------------

TEST_F(StatusResponseTest, monitoringNameDefault)
{
	Utf8String_t s("");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCapture());
	EXPECT_TRUE(statusResponse.getMonitorName().empty());
}

TEST_F(StatusResponseTest, monitoringNameValidAsciiName)
{
	Utf8String_t s("bn=HelloWorld");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCapture());
	EXPECT_TRUE(statusResponse.getMonitorName().equals("HelloWorld"));
}

TEST_F(StatusResponseTest, monitoringNameValidUtf8Name)
{
	Utf8String_t s(u8"bn=𐋏𝖾ll𝑜 𝙒ᴑ𝒓l𝖽 ｆ𝓻𝗈ｍ 𝒐ᴜ𝑡𝒆𝓇 𝕤𝟈𝛼𝘤ℯ");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCapture());
	Utf8String_t cmp(u8"𐋏𝖾ll𝑜 𝙒ᴑ𝒓l𝖽 ｆ𝓻𝗈ｍ 𝒐ᴜ𝑡𝒆𝓇 𝕤𝟈𝛼𝘤ℯ");
	EXPECT_TRUE(statusResponse.getMonitorName().equals(cmp));
}

/// -----------------

TEST_F(StatusResponseTest, serverIdDefault)
{
	Utf8String_t s("");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_EQ(-1, statusResponse.getServerID());
}

TEST_F(StatusResponseTest, serverIdOne)
{
	Utf8String_t s("id=1");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_EQ(1, statusResponse.getServerID());
}

TEST_F(StatusResponseTest, serverIdTwo)
{
	Utf8String_t s("id=2");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_EQ(2, statusResponse.getServerID());
}

TEST_F(StatusResponseTest, serverIdMinusOne)
{
	Utf8String_t s("id=-1");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_EQ(-1, statusResponse.getServerID());
}

TEST_F(StatusResponseTest, serverIdSignedIntegerMax)
{
	Utf8String_t s("id=2147483647");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_EQ(2147483647, statusResponse.getServerID());
}

TEST_F(StatusResponseTest, DISABLED_serverIdUnsignedIntegerMax)
{
	Utf8String_t s("id=4294967295");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_EQ(4294967295, statusResponse.getServerID());
}

TEST_F(StatusResponseTest, DISABLED_serverIdUnsignedIntegerMaxPlusOne)
{
	Utf8String_t s("id=4294967296");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_EQ(-1, statusResponse.getServerID());
}

/// -----------------

TEST_F(StatusResponseTest, maxBeaconSizeDefault)
{
	Utf8String_t s("");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_EQ(-1, statusResponse.getMaxBeaconSize());
}

TEST_F(StatusResponseTest, maxBeaconSizeOne)
{
	Utf8String_t s("bl=1");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_EQ(1, statusResponse.getMaxBeaconSize());
}

TEST_F(StatusResponseTest, maxBeaconSizeTwo)
{
	Utf8String_t s("bl=2");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_EQ(2, statusResponse.getMaxBeaconSize());
}

TEST_F(StatusResponseTest, maxBeaconSizeMinusOne)
{
	Utf8String_t s("bl=-1");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_EQ(-1, statusResponse.getMaxBeaconSize());
}

TEST_F(StatusResponseTest, maxBeaconSizeSignedIntegerMax)
{
	Utf8String_t s("bl=2147483647");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_EQ(2147483647, statusResponse.getMaxBeaconSize());
}

TEST_F(StatusResponseTest, DISABLED_maxBeaconSizeUnsignedIntegerMax)
{
	Utf8String_t s("bl=4294967295");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_EQ(4294967295, statusResponse.getMaxBeaconSize());
}

TEST_F(StatusResponseTest, DISABLED_maxBeaconSizeUnsignedIntegerMaxPlusOne)
{
	Utf8String_t s("bl=4294967296");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_EQ(-1, statusResponse.getMaxBeaconSize());
}

/// -----------------

TEST_F(StatusResponseTest, captureErrorsDefault)
{
	Utf8String_t s("");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCaptureErrors());
}

TEST_F(StatusResponseTest, captureErrorsDisabled)
{
	Utf8String_t s("er=0");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_FALSE(statusResponse.isCaptureErrors());
}

TEST_F(StatusResponseTest, captureErrorsEnabledWithOne)
{
	Utf8String_t s("er=1");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCaptureErrors());
}

TEST_F(StatusResponseTest, captureErrorsEnabledWithTwo)
{
	Utf8String_t s("er=2");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCaptureErrors());
}

TEST_F(StatusResponseTest, captureErrorsEnabledWithMinusOne)
{
	Utf8String_t s("er=-1");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCaptureErrors());
}

TEST_F(StatusResponseTest, captureErrorsEnabledWithSignedIntegerMax)
{
	Utf8String_t s("er=2147483647");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCaptureErrors());
}

TEST_F(StatusResponseTest, DISABLED_captureErrorsEnabledWithUnsignedIntegerMax)
{
	Utf8String_t s("er=4294967295");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCaptureErrors());
}

TEST_F(StatusResponseTest, DISABLED_captureErrorsEnabledWithUnsignedIntegerMaxPlusOne)
{
	Utf8String_t s("er=4294967296");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCaptureErrors());
}

TEST_F(StatusResponseTest, captureErrorsEnabledByTruncatedString)
{
	Utf8String_t s("er=");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCaptureErrors());
}

TEST_F(StatusResponseTest, captureErrorsEnabledButThenTruncatedString)
{
	Utf8String_t s("er=1&");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCaptureErrors());
}

/// -----------------

TEST_F(StatusResponseTest, captureCrashesDefault)
{
	Utf8String_t s("");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCaptureCrashes());
}

TEST_F(StatusResponseTest, captureCrashesDisabled)
{
	Utf8String_t s("cr=0");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_FALSE(statusResponse.isCaptureCrashes());
}

TEST_F(StatusResponseTest, captureCrashesEnabledWithOne)
{
	Utf8String_t s("cr=1");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCaptureCrashes());
}

TEST_F(StatusResponseTest, captureCrashesEnabledWithTwo)
{
	Utf8String_t s("cr=2");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCaptureCrashes());
}

TEST_F(StatusResponseTest, captureCrashesEnabledWithMinusOne)
{
	Utf8String_t s("er=-1");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCaptureCrashes());
}

TEST_F(StatusResponseTest, captureCrashesEnabledWithSignedIntegerMax)
{
	Utf8String_t s("cr=2147483647");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCaptureCrashes());
}

TEST_F(StatusResponseTest, DISABLED_captureCrashesEnabledWithUnsignedIntegerMax)
{
	Utf8String_t s("cr=4294967295");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCaptureCrashes());
}

TEST_F(StatusResponseTest, DISABLED_captureCrashesEnabledWithUnsignedIntegerMaxPlusOne)
{
	Utf8String_t s("cr=4294967296");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCaptureCrashes());
}

TEST_F(StatusResponseTest, captureCrashesEnabledByTruncatedString)
{
	Utf8String_t s("cr=");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCaptureCrashes());
}

TEST_F(StatusResponseTest, captureCrashesEnabledButThenTruncatedString)
{
	Utf8String_t s("cr=1&");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCaptureCrashes());
}

/// -----------------

TEST_F(StatusResponseTest, captureTogetherWithSendInterval)
{
	Utf8String_t s("cp=1&si=3");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCapture());
	EXPECT_EQ(3000, statusResponse.getSendInterval());
}

TEST_F(StatusResponseTest, captureTogetherWithCorruptSendInterval)
{
	Utf8String_t s("cp=1&=");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCapture());
	EXPECT_EQ(-1, statusResponse.getSendInterval());
}

TEST_F(StatusResponseTest, notExistingKey)
{
	Utf8String_t s("hello=world");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

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

TEST_F(StatusResponseTest, someTypicalStatusResponse)
{
	Utf8String_t s("cp=1&si=2&bn=MyName&id=5&bl=3072&er=1&cr=0");
	uint32_t responseCode = 200;
	StatusResponse_t statusResponse = StatusResponse_t(logger, s, responseCode, IStatusResponse_t::ResponseHeaders());

	EXPECT_TRUE(statusResponse.isCapture());
	EXPECT_EQ(2000, statusResponse.getSendInterval());
	EXPECT_TRUE(statusResponse.getMonitorName().equals("MyName"));
	EXPECT_EQ(5, statusResponse.getServerID());
	EXPECT_EQ(3072, statusResponse.getMaxBeaconSize());
	EXPECT_TRUE(statusResponse.isCaptureErrors());
	EXPECT_FALSE(statusResponse.isCaptureCrashes());
}
