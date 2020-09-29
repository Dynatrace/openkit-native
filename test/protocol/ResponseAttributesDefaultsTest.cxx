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

#include "protocol/ResponseAttributesDefaults.h"

#include "mock/MockIResponseAttributes.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace test;

using ResponseAttributesDefaults_t = protocol::ResponseAttributesDefaults;

class ResponseAttributesDefaultsTest : public testing::Test
{
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// JSON response tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(ResponseAttributesDefaultsTest, defaultJsonBeaconSize)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::JSON_RESPONSE->getMaxBeaconSizeInBytes(), testing::Eq(150 * 1024)); // 150 kB
}

TEST_F(ResponseAttributesDefaultsTest, defaultJsonSessionDuration)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::JSON_RESPONSE->getMaxSessionDurationInMilliseconds(),
			testing::Eq(360 * 60 * 1000)); // 360 minutes
}

TEST_F(ResponseAttributesDefaultsTest, defaultJsonEventsPerSession)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::JSON_RESPONSE->getMaxEventsPerSession(), testing::Eq(200));
}

TEST_F(ResponseAttributesDefaultsTest, defaultJsonSessionTimeout)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::JSON_RESPONSE->getSessionTimeoutInMilliseconds(), testing::Eq(600 * 1000)); // 600 sec
}

TEST_F(ResponseAttributesDefaultsTest, defaultJsonSendInterval)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::JSON_RESPONSE->getSendIntervalInMilliseconds(), testing::Eq(120 * 1000)); // 120 sec
}

TEST_F(ResponseAttributesDefaultsTest, defaultJsonVisitStoreVersion)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::JSON_RESPONSE->getVisitStoreVersion(), testing::Eq(1));
}

TEST_F(ResponseAttributesDefaultsTest, defaultJsonIsCapture)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::JSON_RESPONSE->isCapture(), testing::Eq(true));
}

TEST_F(ResponseAttributesDefaultsTest, defaultJsonIsCaptureCrashes)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::JSON_RESPONSE->isCaptureCrashes(), testing::Eq(true));
}

TEST_F(ResponseAttributesDefaultsTest, defaultJsonIsCaptureErrors)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::JSON_RESPONSE->isCaptureErrors(), testing::Eq(true));
}

TEST_F(ResponseAttributesDefaultsTest, defaultJsonMultiplicity)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::JSON_RESPONSE->getMultiplicity(), testing::Eq(1));
}

TEST_F(ResponseAttributesDefaultsTest, defaultJsonServerId)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::JSON_RESPONSE->getServerId(), testing::Eq(1));
}

TEST_F(ResponseAttributesDefaultsTest, defaultJsonTimestamp)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::JSON_RESPONSE->getTimestampInMilliseconds(), testing::Eq(0L));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Key/Value pair response tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(ResponseAttributesDefaultsTest, defaultKeyValueBeaconSize)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::KEY_VALUE_RESPONSE->getMaxBeaconSizeInBytes(), testing::Eq(30 * 1024)); // 30 kB
}

TEST_F(ResponseAttributesDefaultsTest, defaultKeyValueSessionDuration)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::KEY_VALUE_RESPONSE->getMaxSessionDurationInMilliseconds(), testing::Eq(-1)); // not set
}

TEST_F(ResponseAttributesDefaultsTest, defaultKeyValueEventsPerSession)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::KEY_VALUE_RESPONSE->getMaxEventsPerSession(), testing::Eq(-1)); // not set
}

TEST_F(ResponseAttributesDefaultsTest, defaultKeyValueSessionTimeout)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::KEY_VALUE_RESPONSE->getSessionTimeoutInMilliseconds(), testing::Eq(-1)); // not set
}

TEST_F(ResponseAttributesDefaultsTest, defaultKeyValueSendInterval)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::KEY_VALUE_RESPONSE->getSendIntervalInMilliseconds(), testing::Eq(120000)); // 120 sec
}

TEST_F(ResponseAttributesDefaultsTest, defaultKeyValueVisitStoreVersion)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::KEY_VALUE_RESPONSE->getVisitStoreVersion(), testing::Eq(1));
}

TEST_F(ResponseAttributesDefaultsTest, defaultKeyValueIsCapture)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::KEY_VALUE_RESPONSE->isCapture(), testing::Eq(true));
}

TEST_F(ResponseAttributesDefaultsTest, defaultKeyValueIsCaptureCrashes)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::KEY_VALUE_RESPONSE->isCaptureCrashes(), testing::Eq(true));
}

TEST_F(ResponseAttributesDefaultsTest, defaultKeyValueIsCaptureErrors)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::KEY_VALUE_RESPONSE->isCaptureErrors(), testing::Eq(true));
}

TEST_F(ResponseAttributesDefaultsTest, defaultKeyValueMultiplicity)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::KEY_VALUE_RESPONSE->getMultiplicity(), testing::Eq(1));
}

TEST_F(ResponseAttributesDefaultsTest, defaultKeyValueServerId)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::KEY_VALUE_RESPONSE->getServerId(), testing::Eq(1));
}

TEST_F(ResponseAttributesDefaultsTest, defaultKeyValueTimestamp)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::KEY_VALUE_RESPONSE->getTimestampInMilliseconds(), testing::Eq(0L));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Undefined defaults tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(ResponseAttributesDefaultsTest, defaultUndefinedBeaconSize)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::UNDEFINED->getMaxBeaconSizeInBytes(), testing::Eq(30 * 1024));
}

TEST_F(ResponseAttributesDefaultsTest, defaultUndefinedSessionDuration)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::UNDEFINED->getMaxSessionDurationInMilliseconds(), testing::Eq(-1)); // not set
}

TEST_F(ResponseAttributesDefaultsTest, defaultUndefinedEventsPerSession)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::UNDEFINED->getMaxEventsPerSession(), testing::Eq(-1)); // not set
}

TEST_F(ResponseAttributesDefaultsTest, defaultUndefinedSessionTimeout)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::UNDEFINED->getSessionTimeoutInMilliseconds(), testing::Eq(-1)); // not set
}

TEST_F(ResponseAttributesDefaultsTest, defaultUndefinedSendInterval)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::UNDEFINED->getSendIntervalInMilliseconds(), testing::Eq(120 * 1000)); // 120 sec
}

TEST_F(ResponseAttributesDefaultsTest, defaultUndefinedVisitStoreVersion)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::UNDEFINED->getVisitStoreVersion(), testing::Eq(1));
}

TEST_F(ResponseAttributesDefaultsTest, defaultUndefinedIsCapture)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::UNDEFINED->isCapture(), testing::Eq(true));
}

TEST_F(ResponseAttributesDefaultsTest, defaultUndefinedIsCaptureCrashes)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::UNDEFINED->isCaptureCrashes(), testing::Eq(true));
}

TEST_F(ResponseAttributesDefaultsTest, defaultUndefinedIsCaptureErrors)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::UNDEFINED->isCaptureErrors(), testing::Eq(true));
}

TEST_F(ResponseAttributesDefaultsTest, defaultUndefinedMultiplicity)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::UNDEFINED->getMultiplicity(), testing::Eq(1));
}

TEST_F(ResponseAttributesDefaultsTest, defaultUndefinedServerId)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::UNDEFINED->getServerId(), testing::Eq(-1));
}

TEST_F(ResponseAttributesDefaultsTest, defaultUndefinedTimestamp)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::UNDEFINED->getTimestampInMilliseconds(), testing::Eq(0L));
}

TEST_F(ResponseAttributesDefaultsTest, defaultUndefinedMergeReturnsPassedValue)
{
	// given
	auto responseAttributes = MockIResponseAttributes::createNice();

	// when
	auto obtained = ResponseAttributesDefaults_t::UNDEFINED->merge(responseAttributes);

	// then
	ASSERT_THAT(obtained, testing::Eq(responseAttributes));
}
