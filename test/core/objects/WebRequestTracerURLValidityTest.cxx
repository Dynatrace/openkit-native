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

#include "Types.h"
#include "MockTypes.h"
#include "../Types.h"
#include "../caching/Types.h"
#include "../configuration/Types.h"
#include "../../api/Types.h"
#include "../../protocol/Types.h"
#include "../../protocol/MockTypes.h"
#include "../../providers/Types.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace test::types;

static const char* APP_ID = "appID";
static const char* APP_NAME = "appName";

class WebRequestTracerURLValidityTest : public testing::Test
{
protected:

	virtual void SetUp() override
	{
		auto threadIDProvider = std::make_shared<DefaultThreadIdProvider_t>();
		auto timingProvider = std::make_shared<DefaultTimingProvider_t>();
		auto sessionIDProvider = std::make_shared<DefaultSessionIdProvider_t>();

		auto trustManager = std::make_shared<SslStrictTrustManager_t>();

		auto  device = std::make_shared<Device_t>(Utf8String_t(""), Utf8String_t(""), Utf8String_t(""));
		auto beaconConfiguration = std::make_shared<BeaconConfiguration_t>();
		auto beaconCacheConfiguration = std::make_shared<BeaconCacheConfiguration_t>(-1, -1, -1);
		auto configuration = std::make_shared<Configuration_t>
		(
			device,
			OpenKitType_t::Type::DYNATRACE,
			Utf8String_t(APP_NAME),
			"",
			APP_ID,
			0,
			"0",
			"",
			sessionIDProvider,
			trustManager,
			beaconCacheConfiguration,
			beaconConfiguration
		);
		configuration->enableCapture();

		auto beaconCache = std::make_shared<BeaconCache_t>(logger);

		logger = std::make_shared<DefaultLogger_t>(devNull, LogLevel_t::LOG_LEVEL_DEBUG);
		mockBeacon = std::make_shared<MockNiceBeacon_t>(logger, beaconCache, configuration, nullptr, threadIDProvider, timingProvider);
		mockParent = std::make_shared<MockNiceOpenKitComposite_t>();
		ON_CALL(*mockParent, getActionId())
			.WillByDefault(testing::Return(42));
	}

	virtual void TearDown() override
	{
		logger = nullptr;
		mockBeacon = nullptr;
	}

	std::ostringstream devNull;
	ILogger_sp logger;
	MockNiceBeacon_sp mockBeacon;
	MockNiceOpenKitComposite_sp mockParent;
};


TEST_F(WebRequestTracerURLValidityTest, emptyStringIsNotAValidURLScheme)
{
	// then
	ASSERT_FALSE(WebRequestTracer_t::isValidURLScheme(Utf8String_t()));
}

TEST_F(WebRequestTracerURLValidityTest, aValidSchemeStartsWithALetter)
{
	// when starting with lower case letter, then
	ASSERT_TRUE(WebRequestTracer_t::isValidURLScheme("a://some.host"));
}

TEST_F(WebRequestTracerURLValidityTest, aValidSchemeOnlyContainsLettersDigitsPlusPeriodOrHyphen)
{
	// when the url scheme contains all allowed characters, then
	ASSERT_TRUE(WebRequestTracer_t::isValidURLScheme("b1+Z6.-://some.host"));
}

TEST_F(WebRequestTracerURLValidityTest, aValidSchemeAllowsUpperCaseLettersToo)
{
	// when the url scheme contains all allowed characters, then
	ASSERT_TRUE(WebRequestTracer_t::isValidURLScheme("Obp1e+nZK6i.t-://some.host"));
}

TEST_F(WebRequestTracerURLValidityTest, aValidSchemeDoesNotStartWithADigit)
{
	// then
	ASSERT_FALSE(WebRequestTracer_t::isValidURLScheme("1a://some.host"));
}

TEST_F(WebRequestTracerURLValidityTest, aSchemeIsInvalidIfInvalidCharactersAreEncountered)
{
	// then
	ASSERT_FALSE(WebRequestTracer_t::isValidURLScheme("a()[]{}@://some.host"));
}

TEST_F(WebRequestTracerURLValidityTest, anURLIsOnlySetInConstructorIfItIsValid)
{
	// given
	Utf8String_t url("a1337://foo");
	WebRequestTracer_t target(logger, mockParent, mockBeacon, url);

	// then
	ASSERT_EQ(target.getURL(), "a1337://foo");
}

TEST_F(WebRequestTracerURLValidityTest, ifURLIsInvalidTheDefaultValueIsUsed)
{
	// given
	Utf8String_t url("1337://foo");
	WebRequestTracer_t target(logger, mockParent, mockBeacon, url);

	// then
	ASSERT_EQ(target.getURL(), "<unknown>");
}

TEST_F(WebRequestTracerURLValidityTest, urlStoredDoesNotContainRequestParameters)
{
	// given
	Utf8String_t url("https://www.google.com/foo/bar?foo=bar&asdf=jklo");
	WebRequestTracer_t target(logger, mockParent, mockBeacon, url);

	// then
	ASSERT_EQ(target.getURL(), "https://www.google.com/foo/bar");
}
