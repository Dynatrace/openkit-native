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

#include "mock/MockIOpenKitComposite.h"
#include "../../api/mock/MockILogger.h"
#include "../../protocol/mock/MockIBeacon.h"

#include "core/UTF8String.h"
#include "core/objects/WebRequestTracer.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace test;

using MockNiceILogger_sp = std::shared_ptr<testing::NiceMock<MockILogger>>;
using MockNiceIBeacon_sp = std::shared_ptr<testing::NiceMock<MockIBeacon>>;
using MockNiceIOpenKitComposite_sp = std::shared_ptr<testing::NiceMock<MockIOpenKitComposite>>;
using WebRequestTracer_t = core::objects::WebRequestTracer;
using Utf8String_t = core::UTF8String;

class WebRequestTracerURLValidityTest : public testing::Test
{
protected:

	MockNiceILogger_sp logger;
	MockNiceIBeacon_sp mockBeacon;
	MockNiceIOpenKitComposite_sp mockParent;

	virtual void SetUp() override
	{
		logger = MockILogger::createNice();
		mockBeacon = MockIBeacon::createNice();

		mockParent = MockIOpenKitComposite::createNice();
		ON_CALL(*mockParent, getActionId())
			.WillByDefault(testing::Return(42));
	}

	virtual void TearDown() override
	{
		logger = nullptr;
		mockBeacon = nullptr;
	}

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
