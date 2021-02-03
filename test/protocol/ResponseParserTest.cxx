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

#include "protocol/JsonResponseParser.h"
#include "protocol/ResponseAttributesDefaults.h"
#include "protocol/ResponseParser.h"
#include "util/json/parser/JsonParserException.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <sstream>

using JsonResponseParser_t = protocol::JsonResponseParser;
using ResponseAttributesDefaults_t = protocol::ResponseAttributesDefaults;
using ResponseParser_t = protocol::ResponseParser;

class ResponseParserTest : public testing::Test
{
};

TEST_F(ResponseParserTest, parsingEmptyStringThrowsException)
{
	try
	{
		// given, when
		ResponseParser_t::parseResponse("");

		FAIL() << "Expected JsonParserException to be thrown";
	}
	catch (util::json::parser::JsonParserException&)
	{
		// expected
	}
}

TEST_F(ResponseParserTest, parsingArbitraryResponseThrowsException)
{
	try
	{
		// given, when
		ResponseParser_t::parseResponse("some response text");

		FAIL() << "Expected JsonParserException to be thrown";
	}
	catch (util::json::parser::JsonParserException&)
	{
		// expected
	}
}

TEST_F(ResponseParserTest, parseKeyValueResponseWorks)
{
	// given
	std::stringstream input;
	input << "type=m&bl=17&id=18&cp=0";

	// when
	auto obtained = ResponseParser_t::parseResponse(input.str());

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getMaxBeaconSizeInBytes(), testing::Eq(17 * 1024));
	ASSERT_THAT(obtained->getServerId(), testing::Eq(18));
	ASSERT_THAT(obtained->isCapture(), testing::Eq(false));
}

TEST_F(ResponseParserTest, parseWithPartiallyMatchingKeyValuePrefixThrowsException)
{
	try
	{
		// given, when
		ResponseParser_t::parseResponse("type=mobile&bl=17");

		FAIL() << "Expected JsonParserException to be thrown";
	}
	catch (util::json::parser::JsonParserException&)
	{
		// expected
	}
}

TEST_F(ResponseParserTest, parseWithOnlyKeyValuePrefixReturnsDefaultResponse)
{
	// given
	auto defaults = ResponseAttributesDefaults_t::keyValueResponse();

	// when
	auto obtained = ResponseParser_t::parseResponse("type=m");

	// then
	ASSERT_THAT(obtained, testing::NotNull());

	ASSERT_THAT(obtained->getMaxBeaconSizeInBytes(), testing::Eq(defaults->getMaxBeaconSizeInBytes()));
	ASSERT_THAT(obtained->getMaxSessionDurationInMilliseconds(),
		testing::Eq(defaults->getMaxSessionDurationInMilliseconds()));
	ASSERT_THAT(obtained->getMaxEventsPerSession(), testing::Eq(defaults->getMaxEventsPerSession()));
	ASSERT_THAT(obtained->getSessionTimeoutInMilliseconds(), testing::Eq(defaults->getSessionTimeoutInMilliseconds()));
	ASSERT_THAT(obtained->getSendIntervalInMilliseconds(), testing::Eq(defaults->getSendIntervalInMilliseconds()));
	ASSERT_THAT(obtained->getVisitStoreVersion(), testing::Eq(defaults->getVisitStoreVersion()));

	ASSERT_THAT(obtained->isCapture(), testing::Eq(defaults->isCapture()));
	ASSERT_THAT(obtained->isCaptureCrashes(), testing::Eq(defaults->isCaptureCrashes()));
	ASSERT_THAT(obtained->isCaptureErrors(), testing::Eq(defaults->isCaptureErrors()));

	ASSERT_THAT(obtained->getMultiplicity(), testing::Eq(defaults->getMultiplicity()));
	ASSERT_THAT(obtained->getServerId(), testing::Eq(defaults->getServerId()));

	ASSERT_THAT(obtained->getTimestampInMilliseconds(), testing::Eq(defaults->getTimestampInMilliseconds()));
}

TEST_F(ResponseParserTest, parseJsonResponseWorks)
{
	// given
	std::stringstream input;
	input << "{";
	input << "  \"" << JsonResponseParser_t::RESPONSE_KEY_AGENT_CONFIG << "\": {";
	input << "    \"" << JsonResponseParser_t::RESPONSE_KEY_MAX_BEACON_SIZE_IN_KB << "\": 17";
	input << "  },";
	input << "  \"" << JsonResponseParser_t::RESPONSE_KEY_APP_CONFIG << "\": {";
	input << "    \"" << JsonResponseParser_t::RESPONSE_KEY_CAPTURE << "\": 0";
	input << "  },";
	input << "  \"" << JsonResponseParser_t::RESPONSE_KEY_DYNAMIC_CONFIG << "\": {";
	input << "    \"" << JsonResponseParser_t::RESPONSE_KEY_SERVER_ID << "\": 18";
	input << "  },";
	input << "  \"" << JsonResponseParser_t::RESPONSE_KEY_TIMESTAMP_IN_MILLIS << "\": 19";
	input << "}";

	// when
	auto obtained = ResponseParser_t::parseResponse(input.str());

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getMaxBeaconSizeInBytes(), testing::Eq(17 * 1024));
	ASSERT_THAT(obtained->getServerId(), testing::Eq(18));
	ASSERT_THAT(obtained->isCapture(), testing::Eq(false));
	ASSERT_THAT(obtained->getTimestampInMilliseconds(), testing::Eq(19L));
}