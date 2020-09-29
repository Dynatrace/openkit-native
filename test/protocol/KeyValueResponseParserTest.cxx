/**
 * Copyright 2018-2020 Dynatrace LLC
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

#include "protocol/KeyValueResponseParser.h"
#include "protocol/ResponseAttributes.h"
#include "protocol/ResponseAttributesDefaults.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using KeyValueResponseParser_t = protocol::KeyValueResponseParser;
using ResponseAttribute_t = protocol::ResponseAttribute;
using ResponseAttributesDefaults_t = protocol::ResponseAttributesDefaults;

class KeyValueResponseParserTest : public testing::Test
{
protected:
	std::stringstream input;

	void SetUp() override
	{
		input = std::stringstream();
	}
};

TEST_F(KeyValueResponseParserTest, parsingAnEmptyStringReturnsResponseWithDefaultValues)
{
	// given
	auto defaults = ResponseAttributesDefaults_t::KEY_VALUE_RESPONSE;

	// when
	auto obtained = KeyValueResponseParser_t::parse("");

	// then
	ASSERT_THAT(obtained, testing::NotNull());

	// then
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

TEST_F(KeyValueResponseParserTest, parseExtractsBeaconSize)
{
	// given
	const int32_t beaconSize = 37;
	input << "type=m&" << KeyValueResponseParser_t::RESPONSE_KEY_MAX_BEACON_SIZE_IN_KB << "=" << beaconSize;

	// when
	auto obtained = KeyValueResponseParser_t::parse(input.str());

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getMaxBeaconSizeInBytes(), testing::Eq(beaconSize * 1024));
}

TEST_F(KeyValueResponseParserTest, parseExtractsSendInterval)
{
	// given
	const int32_t sendInterval = 37;
	input << "type=m&" << KeyValueResponseParser_t::RESPONSE_KEY_SEND_INTERVAL_IN_SEC << "=" << sendInterval;

	// when
	auto obtained = KeyValueResponseParser_t::parse(input.str());

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getSendIntervalInMilliseconds(), testing::Eq(sendInterval * 1000));
}

TEST_F(KeyValueResponseParserTest, parseExtractsCaptureEnabled)
{
	// given
	input << "type=m&" << KeyValueResponseParser_t::RESPONSE_KEY_CAPTURE << "=1";

	// when
	auto obtained = KeyValueResponseParser_t::parse(input.str());

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->isCapture(), testing::Eq(true));
}

TEST_F(KeyValueResponseParserTest, parseExtractsCaptureDisabled)
{
	// given
	input << "type=m&" << KeyValueResponseParser_t::RESPONSE_KEY_CAPTURE << "=0";

	// when
	auto obtained = KeyValueResponseParser_t::parse(input.str());

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->isCapture(), testing::Eq(false));
}

TEST_F(KeyValueResponseParserTest, parseExtractsCaptureCrashesEnabled)
{
	// given
	input << "type=m&" << KeyValueResponseParser_t::RESPONSE_KEY_REPORT_CRASHES << "=1";

	// when
	auto obtained = KeyValueResponseParser_t::parse(input.str());

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->isCaptureCrashes(), testing::Eq(true));
}

TEST_F(KeyValueResponseParserTest, parseExtractsCaptureCrashesDisabled)
{
	// given
	input << "type=m&" << KeyValueResponseParser_t::RESPONSE_KEY_REPORT_CRASHES << "=0";

	// when
	auto obtained = KeyValueResponseParser_t::parse(input.str());

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->isCaptureCrashes(), testing::Eq(false));
}

TEST_F(KeyValueResponseParserTest, parseExtractsCaptureErrorsEnabled)
{
	// given
	input << "type=m&" << KeyValueResponseParser_t::RESPONSE_KEY_REPORT_ERRORS << "=1";

	// when
	auto obtained = KeyValueResponseParser_t::parse(input.str());

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->isCaptureErrors(), testing::Eq(true));
}

TEST_F(KeyValueResponseParserTest, parseExtractsCaptureErrorsDisabled)
{
	// given
	input << "type=m&" << KeyValueResponseParser_t::RESPONSE_KEY_REPORT_ERRORS << "=0";

	// when
	auto obtained = KeyValueResponseParser_t::parse(input.str());

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->isCaptureErrors(), testing::Eq(false));
}

TEST_F(KeyValueResponseParserTest, parseExtractsServerId)
{
	// given
	const int32_t serverId = 73;
	input << "type=m&" << KeyValueResponseParser_t::RESPONSE_KEY_SERVER_ID << "=" << serverId;

	// when
	auto obtained = KeyValueResponseParser_t::parse(input.str());

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getServerId(), testing::Eq(serverId));
}

TEST_F(KeyValueResponseParserTest, parseExtractsMultiplicity)
{
	// given
	const int32_t multiplicity = 73;
	input << "type=m&" << KeyValueResponseParser_t::RESPONSE_KEY_MULTIPLICITY << "=" << multiplicity;

	// when
	auto obtained = KeyValueResponseParser_t::parse(input.str());

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getMultiplicity(), testing::Eq(multiplicity));
}

TEST_F(KeyValueResponseParserTest, parseResponseWithAllParametersSet)
{
	// given
	auto defaults = ResponseAttributesDefaults_t::KEY_VALUE_RESPONSE;
	const int32_t beaconSize = 73;
	const int32_t sendInterval = 74;
	const int32_t serverId = 75;
	const int32_t multiplicity = 76;

	input << "type=m";
	input << "&" << KeyValueResponseParser_t::RESPONSE_KEY_MAX_BEACON_SIZE_IN_KB << "=" << beaconSize;
	input << "&" << KeyValueResponseParser_t::RESPONSE_KEY_SEND_INTERVAL_IN_SEC << "=" << sendInterval;
	input << "&" << KeyValueResponseParser_t::RESPONSE_KEY_CAPTURE << "=0";
	input << "&" << KeyValueResponseParser_t::RESPONSE_KEY_REPORT_CRASHES << "=1";
	input << "&" << KeyValueResponseParser_t::RESPONSE_KEY_REPORT_ERRORS << "=0";
	input << "&" << KeyValueResponseParser_t::RESPONSE_KEY_SERVER_ID << "=" << serverId;
	input << "&" << KeyValueResponseParser_t::RESPONSE_KEY_MULTIPLICITY << "=" << multiplicity;

	// when
	auto obtained = KeyValueResponseParser_t::parse(input.str());

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getMaxBeaconSizeInBytes(), testing::Eq(beaconSize * 1024));
	ASSERT_THAT(obtained->isAttributeSet(ResponseAttribute_t::MAX_BEACON_SIZE), testing::Eq(true));

	ASSERT_THAT(obtained->getSendIntervalInMilliseconds(), testing::Eq(sendInterval * 1000));
	ASSERT_THAT(obtained->isAttributeSet(ResponseAttribute_t::SEND_INTERVAL), testing::Eq(true));

	ASSERT_THAT(obtained->isCapture(), testing::Eq(false));
	ASSERT_THAT(obtained->isAttributeSet(ResponseAttribute_t::IS_CAPTURE), testing::Eq(true));

	ASSERT_THAT(obtained->isCaptureCrashes(), testing::Eq(true));
	ASSERT_THAT(obtained->isAttributeSet(ResponseAttribute_t::IS_CAPTURE_CRASHES), testing::Eq(true));

	ASSERT_THAT(obtained->isCaptureErrors(), testing::Eq(false));
	ASSERT_THAT(obtained->isAttributeSet(ResponseAttribute_t::IS_CAPTURE_ERRORS), testing::Eq(true));

	ASSERT_THAT(obtained->getMultiplicity(), testing::Eq(multiplicity));
	ASSERT_THAT(obtained->isAttributeSet(ResponseAttribute_t::MULTIPLICITY), testing::Eq(true));

	ASSERT_THAT(obtained->getServerId(), testing::Eq(serverId));
	ASSERT_THAT(obtained->isAttributeSet(ResponseAttribute_t::SERVER_ID), testing::Eq(true));

	ASSERT_THAT(obtained->getMaxSessionDurationInMilliseconds(),
		testing::Eq(defaults->getMaxSessionDurationInMilliseconds()));
	ASSERT_THAT(obtained->isAttributeSet(ResponseAttribute_t::MAX_SESSION_DURATION), testing::Eq(false));
	ASSERT_THAT(obtained->getMaxEventsPerSession(), testing::Eq(defaults->getMaxEventsPerSession()));
	ASSERT_THAT(obtained->isAttributeSet(ResponseAttribute_t::MAX_EVENTS_PER_SESSION), testing::Eq(false));
	ASSERT_THAT(obtained->getSessionTimeoutInMilliseconds(), testing::Eq(defaults->getSessionTimeoutInMilliseconds()));
	ASSERT_THAT(obtained->isAttributeSet(ResponseAttribute_t::SESSION_IDLE_TIMEOUT), testing::Eq(false));
	ASSERT_THAT(obtained->getVisitStoreVersion(), testing::Eq(defaults->getVisitStoreVersion()));
	ASSERT_THAT(obtained->isAttributeSet(ResponseAttribute_t::VISIT_STORE_VERSION), testing::Eq(false));
	ASSERT_THAT(obtained->getTimestampInMilliseconds(), testing::Eq(defaults->getTimestampInMilliseconds()));
	ASSERT_THAT(obtained->isAttributeSet(ResponseAttribute_t::TIMESTAMP), testing::Eq(false));
}