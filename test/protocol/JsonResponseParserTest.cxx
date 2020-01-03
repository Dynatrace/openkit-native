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

#include "protocol/JsonResponseParser.h"
#include "protocol/ResponseAttribute.h"
#include "protocol/ResponseAttributesDefaults.h"
#include "util/json/parser/JsonParserException.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <sstream>

using JsonParserException_t = util::json::parser::JsonParserException;
using JsonResponseParser_t = protocol::JsonResponseParser;
using ResponseAttribute_t = protocol::ResponseAttribute;
using ResponseAttributesDefaults_t = protocol::ResponseAttributesDefaults;

class JsonResponseParserTest : public testing::Test
{
protected:
	std::stringstream input;

	void SetUp() override
	{
		input = std::stringstream();
	}
};

TEST_F(JsonResponseParserTest, parsingAnEmptyStringThrowsException)
{
	// when
	try
	{
		JsonResponseParser_t::parse("");
		FAIL() << "expected JsonParserException to be thrown";
	} catch(JsonParserException_t&)
	{
		// expected
	}
}

TEST_F(JsonResponseParserTest, parsingAnEmptyObjectReturnsInstanceWithDefaultValues)
{
	// given
	auto defaults = ResponseAttributesDefaults_t::JSON_RESPONSE;

	// when
	auto obtained = JsonResponseParser_t::parse("{}");

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

TEST_F(JsonResponseParserTest, parseExtractsMaxBeaconSize)
{
	// given
	const int32_t beaconSize = 73;
	input << "{";
	input << "  \"" << JsonResponseParser_t::RESPONSE_KEY_AGENT_CONFIG << "\": {";
	input << "    \"" << JsonResponseParser_t::RESPONSE_KEY_MAX_BEACON_SIZE_IN_KB << "\": " << beaconSize;
	input << "  }";
	input << "}";

	// when
	auto obtained = JsonResponseParser_t::parse(input.str());

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getMaxBeaconSizeInBytes(), testing::Eq(beaconSize * 1024));
}

TEST_F(JsonResponseParserTest, parseExtractsMaxSessionDuration)
{
	// given
	const int sessionDuration = 73;
	input << "{";
	input << "  \"" << JsonResponseParser_t::RESPONSE_KEY_AGENT_CONFIG << "\": {";
	input << "    \"" << JsonResponseParser_t::RESPONSE_KEY_MAX_SESSION_DURATION_IN_MIN << "\": " << sessionDuration;
	input << "  }";
	input << "}";

	// when
	auto obtained = JsonResponseParser_t::parse(input.str());

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getMaxSessionDurationInMilliseconds(), testing::Eq(sessionDuration * 60 * 1000));
}

TEST_F(JsonResponseParserTest, parseExtractsMaxEventsPerSession)
{
	// given
	const int32_t eventsPerSession = 73;
	input << "{";
	input << "  \"" << JsonResponseParser_t::RESPONSE_KEY_AGENT_CONFIG << "\": {";
	input << "    \"" << JsonResponseParser_t::RESPONSE_KEY_MAX_EVENTS_PER_SESSION << "\": " << eventsPerSession;
	input << "  }";
	input << "}";

	// when
	auto obtained = JsonResponseParser_t::parse(input.str());

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getMaxEventsPerSession(), testing::Eq(eventsPerSession));
}

TEST_F(JsonResponseParserTest, parseExtractsSessionTimeout)
{
	// given
	const int32_t sessionTimeout = 73;
	input << "{";
	input << "  \"" << JsonResponseParser_t::RESPONSE_KEY_AGENT_CONFIG << "\": {";
	input << "    \"" << JsonResponseParser_t::RESPONSE_KEY_SESSION_TIMEOUT_IN_SEC << "\": " << sessionTimeout;
	input << "  }";
	input << "}";

	// when
	auto obtained = JsonResponseParser_t::parse(input.str());

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getSessionTimeoutInMilliseconds(), testing::Eq(sessionTimeout * 1000));
}

TEST_F(JsonResponseParserTest, parseExtractsSendInterval)
{
	// given
	const int32_t sendInterval = 73;
	input << "{";
	input << "  \"" << JsonResponseParser_t::RESPONSE_KEY_AGENT_CONFIG << "\": {";
	input << "    \"" << JsonResponseParser_t::RESPONSE_KEY_SEND_INTERVAL_IN_SEC << "\": " << sendInterval;
	input << "  }";
	input << "}";

	// when
	auto obtained = JsonResponseParser_t::parse(input.str());

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getSendIntervalInMilliseconds(), testing::Eq(sendInterval * 1000));
}

TEST_F(JsonResponseParserTest, parseExtractsVisitStoreVersion)
{
	// given
	const int32_t visitStoreVersion = 73;
	input << "{";
	input << "  \"" << JsonResponseParser_t::RESPONSE_KEY_AGENT_CONFIG << "\": {";
	input << "    \"" << JsonResponseParser_t::RESPONSE_KEY_VISIT_STORE_VERSION << "\": " << visitStoreVersion;
	input << "  }";
	input << "}";

	// when
	auto obtained = JsonResponseParser_t::parse(input.str());

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getVisitStoreVersion(), testing::Eq(visitStoreVersion));
}

TEST_F(JsonResponseParserTest, parseExtractsCaptureEnabled)
{
	// given
	input << "{";
	input << "  \"" << JsonResponseParser_t::RESPONSE_KEY_APP_CONFIG << "\": {";
	input << "    \"" << JsonResponseParser_t::RESPONSE_KEY_CAPTURE << "\": 1";
	input << "  }";
	input << "}";

	// when
	auto obtained = JsonResponseParser_t::parse(input.str());

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->isCapture(), testing::Eq(true));
}

TEST_F(JsonResponseParserTest, parseExtractsCaptureDisabled)
{
	// given
	input << "{";
	input << "  \"" << JsonResponseParser_t::RESPONSE_KEY_APP_CONFIG << "\": {";
	input << "    \"" << JsonResponseParser_t::RESPONSE_KEY_CAPTURE << "\": 0";
	input << "  }";
	input << "}";

	// when
	auto obtained = JsonResponseParser_t::parse(input.str());

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->isCapture(), testing::Eq(false));
}

TEST_F(JsonResponseParserTest, parseExtractsReportCrashesEnabled)
{
	// given
	input << "{";
	input << "  \"" << JsonResponseParser_t::RESPONSE_KEY_APP_CONFIG << "\": {";
	input << "    \"" << JsonResponseParser_t::RESPONSE_KEY_REPORT_CRASHES << "\": 1";
	input << "  }";
	input << "}";

	// when
	auto obtained = JsonResponseParser_t::parse(input.str());

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->isCaptureCrashes(), testing::Eq(true));
}

TEST_F(JsonResponseParserTest, parseExtractsReportCrashesDisabled)
{
	// given
	input << "{";
	input << "  \"" << JsonResponseParser_t::RESPONSE_KEY_APP_CONFIG << "\": {";
	input << "    \"" << JsonResponseParser_t::RESPONSE_KEY_REPORT_CRASHES << "\": 0";
	input << "  }";
	input << "}";

	// when
	auto obtained = JsonResponseParser_t::parse(input.str());

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->isCaptureCrashes(), testing::Eq(false));
}

TEST_F(JsonResponseParserTest, parseExtractsReportErrorsEnabled)
{
	// given
	input << "{";
	input << "  \"" << JsonResponseParser_t::RESPONSE_KEY_APP_CONFIG << "\": {";
	input << "    \"" << JsonResponseParser_t::RESPONSE_KEY_REPORT_ERRORS << "\": 1";
	input << "  }";
	input << "}";

	// when
	auto obtained = JsonResponseParser_t::parse(input.str());

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->isCaptureErrors(), testing::Eq(true));
}

TEST_F(JsonResponseParserTest, parseExtractsReportErrorsDisabled)
{
	// given
	input << "{";
	input << "  \"" << JsonResponseParser_t::RESPONSE_KEY_APP_CONFIG << "\": {";
	input << "    \"" << JsonResponseParser_t::RESPONSE_KEY_REPORT_ERRORS << "\": 0";
	input << "  }";
	input << "}";

	// when
	auto obtained = JsonResponseParser_t::parse(input.str());

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->isCaptureErrors(), testing::Eq(false));
}

TEST_F(JsonResponseParserTest, parseExtractsMultiplicity)
{
	// given
	const int32_t multiplicity = 73;
	input << "{";
	input << "  \"" << JsonResponseParser_t::RESPONSE_KEY_DYNAMIC_CONFIG << "\": {";
	input << "    \"" << JsonResponseParser_t::RESPONSE_KEY_MULTIPLICITY << "\": " << multiplicity;
	input << "  }";
	input << "}";

	// when
	auto obtained = JsonResponseParser_t::parse(input.str());

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getMultiplicity(), testing::Eq(multiplicity));
}

TEST_F(JsonResponseParserTest, parseExtractsServerId)
{
	// given
	const int32_t serverId = 73;
	input << "{";
	input << "  \"" << JsonResponseParser_t::RESPONSE_KEY_DYNAMIC_CONFIG << "\": {";
	input << "    \"" << JsonResponseParser_t::RESPONSE_KEY_SERVER_ID << "\": " << serverId;
	input << "  }";
	input << "}";

	// when
	auto obtained = JsonResponseParser_t::parse(input.str());

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getServerId(), testing::Eq(serverId));
}

TEST_F(JsonResponseParserTest, parseExtractsTimestamp)
{
	// given
	const int64_t timestamp = 73;
	input << "{";
	input << "  \"" << JsonResponseParser_t::RESPONSE_KEY_TIMESTAMP_IN_MILLIS << "\": " << timestamp;
	input << "}";

	// when
	auto obtained = JsonResponseParser_t::parse(input.str());

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getTimestampInMilliseconds(), testing::Eq(timestamp));
}

TEST_F(JsonResponseParserTest, parseResponseWithAllValuesSet)
{
	// given
	const int32_t beaconSize = 73;
	const int32_t sessionDuration = 74;
	const int32_t eventsPerSession = 75;
	const int32_t sessionTimeout = 76;
	const int32_t sendInterval = 77;
	const int32_t visitStoreVersion = 78;
	const int32_t multiplicity = 79;
	const int32_t serverId = 80;
	const int64_t timestamp = 81;

	input << "{";
	input << "  \"" << JsonResponseParser_t::RESPONSE_KEY_AGENT_CONFIG << "\": {";
	input << "    \"" << JsonResponseParser_t::RESPONSE_KEY_MAX_BEACON_SIZE_IN_KB << "\": " << beaconSize;
	input << "   ,\"" << JsonResponseParser_t::RESPONSE_KEY_MAX_SESSION_DURATION_IN_MIN << "\": " << sessionDuration;
	input << "   ,\"" << JsonResponseParser_t::RESPONSE_KEY_MAX_EVENTS_PER_SESSION << "\": " << eventsPerSession;
	input << "   ,\"" << JsonResponseParser_t::RESPONSE_KEY_SESSION_TIMEOUT_IN_SEC << "\": " << sessionTimeout;
	input << "   ,\"" << JsonResponseParser_t::RESPONSE_KEY_SEND_INTERVAL_IN_SEC << "\": " << sendInterval;
	input << "   ,\"" << JsonResponseParser_t::RESPONSE_KEY_VISIT_STORE_VERSION << "\": " << visitStoreVersion;
	input << "  },";
	input << "  \"" << JsonResponseParser_t::RESPONSE_KEY_APP_CONFIG << "\": {";
	input << "    \"" << JsonResponseParser_t::RESPONSE_KEY_CAPTURE << "\": 0";
	input << "   ,\"" << JsonResponseParser_t::RESPONSE_KEY_REPORT_CRASHES << "\": 1";
	input << "   ,\"" << JsonResponseParser_t::RESPONSE_KEY_REPORT_ERRORS << "\": 0";
	input << "  },";
	input << "  \"" << JsonResponseParser_t::RESPONSE_KEY_DYNAMIC_CONFIG << "\": {";
	input << "    \"" << JsonResponseParser_t::RESPONSE_KEY_MULTIPLICITY << "\": " << multiplicity;
	input << "   ,\"" << JsonResponseParser_t::RESPONSE_KEY_SERVER_ID << "\": " << serverId;
	input << "  },";
	input << "  \"" << JsonResponseParser_t::RESPONSE_KEY_TIMESTAMP_IN_MILLIS << "\": " << timestamp;
	input << "}";

		// when
	auto obtained = JsonResponseParser_t::parse(input.str());

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getMaxBeaconSizeInBytes(), testing::Eq(beaconSize * 1024));
	ASSERT_THAT(obtained->getMaxSessionDurationInMilliseconds(), testing::Eq(sessionDuration * 60 * 1000));
	ASSERT_THAT(obtained->getMaxEventsPerSession(), testing::Eq(eventsPerSession));
	ASSERT_THAT(obtained->getSessionTimeoutInMilliseconds(), testing::Eq(sessionTimeout * 1000));
	ASSERT_THAT(obtained->getSendIntervalInMilliseconds(), testing::Eq(sendInterval * 1000));
	ASSERT_THAT(obtained->getVisitStoreVersion(), testing::Eq(visitStoreVersion));
	ASSERT_THAT(obtained->isCapture(), testing::Eq(false));
	ASSERT_THAT(obtained->isCaptureCrashes(), testing::Eq(true));
	ASSERT_THAT(obtained->isCaptureErrors(), testing::Eq(false));
	ASSERT_THAT(obtained->getMultiplicity(), testing::Eq(multiplicity));
	ASSERT_THAT(obtained->getServerId(), testing::Eq(serverId));
	ASSERT_THAT(obtained->getTimestampInMilliseconds(), testing::Eq(timestamp));
	for (const auto attribute : protocol::ALL_RESPONSE_ATTRIBUTES)
	{
		ASSERT_THAT(obtained->isAttributeSet(attribute), testing::Eq(true));
	}
}

TEST_F(JsonResponseParserTest, parseIgnoresUnknownTokens)
{
	// given
	input << "{ ";
	input << "\"unknownObject\": {},";
	input << "  \"" << JsonResponseParser_t::RESPONSE_KEY_AGENT_CONFIG << "\": {";
	input << "    \"" << JsonResponseParser_t::RESPONSE_KEY_MAX_EVENTS_PER_SESSION << "\": 999,";
	input << "    \"unknownAttribute\": 777";
	input << "  },";
	input << "  \"anotherUnknownAttribute\": 666";
	input << "}";

	// when
	auto obtained = JsonResponseParser_t::parse(input.str());

	// then
	for (const auto attribute : protocol::ALL_RESPONSE_ATTRIBUTES)
	{
		if (attribute == ResponseAttribute_t::MAX_EVENTS_PER_SESSION)
		{
			ASSERT_THAT(obtained->isAttributeSet(attribute), testing::Eq(true));
			continue;
		}

		ASSERT_THAT(obtained->isAttributeSet(attribute), testing::Eq(false));
	}
}