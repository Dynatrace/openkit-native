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

#include <core/WebRequestTracerStringURL.h>

#include "../protocol/MockBeacon.h"

#include <providers/DefaultThreadIDProvider.h>
#include <providers/DefaultTimingProvider.h>
#include <protocol/ssl/SSLStrictTrustManager.h>
#include <providers/DefaultSessionIDProvider.h>
#include <providers/DefaultHTTPClientProvider.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace core;

static const char* APP_ID = "appID";
static const char* APP_NAME = "appName";

class WebRequestTracerStringURLTest : public testing::Test
{
public:

	virtual void SetUp() override
	{
		auto threadIDProvider = std::make_shared<providers::DefaultThreadIDProvider>();
		auto timingProvider = std::make_shared<providers::DefaultTimingProvider>();
		auto sessionIDProvider = std::make_shared<providers::DefaultSessionIDProvider>();

		auto trustManager = std::make_shared<protocol::SSLStrictTrustManager>();

		auto  device = std::shared_ptr<configuration::Device>(new configuration::Device(core::UTF8String(""), core::UTF8String(""), core::UTF8String("")));
		auto beaconConfiguration = std::make_shared<configuration::BeaconConfiguration>();
		auto beaconCacheConfiguration = std::make_shared<configuration::BeaconCacheConfiguration>(-1, -1, -1);
		auto configuration = std::shared_ptr<configuration::Configuration>(new configuration::Configuration(device, configuration::OpenKitType::Type::DYNATRACE,
			core::UTF8String(APP_NAME), "", APP_ID, 0, "0", "",
			sessionIDProvider, trustManager, beaconCacheConfiguration, beaconConfiguration));
		configuration->enableCapture();

		auto beaconCache = std::make_shared<caching::BeaconCache>(logger);

		logger = std::shared_ptr<openkit::ILogger>(new core::util::DefaultLogger(devNull, true));
		mockBeacon = std::make_shared<testing::NiceMock<test::MockBeacon>>(logger, beaconCache, configuration, nullptr, threadIDProvider, timingProvider);
	}

	virtual void TearDown() override
	{
		logger = nullptr;
		mockBeacon = nullptr;
	}

	std::ostringstream devNull;
	std::shared_ptr<openkit::ILogger> logger;
	std::shared_ptr<testing::NiceMock<test::MockBeacon>> mockBeacon;
};


TEST_F(WebRequestTracerStringURLTest, emptyStringIsNotAValidURLScheme)
{
	// then
	ASSERT_FALSE(WebRequestTracerStringURL::isValidURLScheme(UTF8String()));
}

TEST_F(WebRequestTracerStringURLTest, aValidSchemeStartsWithALetter)
{
	// when starting with lower case letter, then
	ASSERT_TRUE(WebRequestTracerStringURL::isValidURLScheme("a://some.host"));
}

TEST_F(WebRequestTracerStringURLTest, aValidSchemeOnlyContainsLettersDigitsPlusPeriodOrHyphen)
{
	// when the url scheme contains all allowed characters, then
	ASSERT_TRUE(WebRequestTracerStringURL::isValidURLScheme("b1+Z6.-://some.host"));
}

TEST_F(WebRequestTracerStringURLTest, aValidSchemeAllowsUpperCaseLettersToo)
{
	// when the url scheme contains all allowed characters, then
	ASSERT_TRUE(WebRequestTracerStringURL::isValidURLScheme("Obp1e+nZK6i.t-://some.host"));
}

TEST_F(WebRequestTracerStringURLTest, aValidSchemeDoesNotStartWithADigit)
{
	// then
	ASSERT_FALSE(WebRequestTracerStringURL::isValidURLScheme("1a://some.host"));
}

TEST_F(WebRequestTracerStringURLTest, aSchemeIsInvalidIfInvalidCharactersAreEncountered)
{
	// then
	ASSERT_FALSE(WebRequestTracerStringURL::isValidURLScheme("a()[]{}@://some.host"));
}

TEST_F(WebRequestTracerStringURLTest, anURLIsOnlySetInConstructorIfItIsValid)
{
	// given
	WebRequestTracerStringURL target(logger, mockBeacon, 42, "a1337://foo");

	// then
	ASSERT_EQ(target.getURL(), "a1337://foo");
}

TEST_F(WebRequestTracerStringURLTest, ifURLIsInvalidTheDefaultValueIsUsed)
{
	// given
	WebRequestTracerStringURL target(logger, mockBeacon, 42, "1337://foo");

	// then
	ASSERT_EQ(target.getURL(), "<unknown>");
}

TEST_F(WebRequestTracerStringURLTest, urlStoredDoesNotContainRequestParameters)
{
	// given
	WebRequestTracerStringURL target(logger, mockBeacon, 42, "https://www.google.com/foo/bar?foo=bar&asdf=jklo");

	// then
	ASSERT_EQ(target.getURL(), "https://www.google.com/foo/bar");
}
