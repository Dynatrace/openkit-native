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
/// Static methods for obtaining instances
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TEST_F(ResponseAttributesDefaultsTest, jsonResponseGivesSameRawPointer)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::jsonResponse().get(),
		testing::Eq(ResponseAttributesDefaults_t::jsonResponse().get()));
}

TEST_F(ResponseAttributesDefaultsTest, keyValueResponseGivesSameRawPointer)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::keyValueResponse().get(),
		testing::Eq(ResponseAttributesDefaults_t::keyValueResponse().get()));
}

TEST_F(ResponseAttributesDefaultsTest, undefinedGivesSameRawPointer)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::undefined().get(),
		testing::Eq(ResponseAttributesDefaults_t::undefined().get()));
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// JSON response tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(ResponseAttributesDefaultsTest, defaultJsonBeaconSize)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::jsonResponse()->getMaxBeaconSizeInBytes(), testing::Eq(150 * 1024)); // 150 kB
}

TEST_F(ResponseAttributesDefaultsTest, defaultJsonSessionDuration)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::jsonResponse()->getMaxSessionDurationInMilliseconds(),
			testing::Eq(360 * 60 * 1000)); // 360 minutes
}

TEST_F(ResponseAttributesDefaultsTest, defaultJsonEventsPerSession)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::jsonResponse()->getMaxEventsPerSession(), testing::Eq(200));
}

TEST_F(ResponseAttributesDefaultsTest, defaultJsonSessionTimeout)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::jsonResponse()->getSessionTimeoutInMilliseconds(), testing::Eq(600 * 1000)); // 600 sec
}

TEST_F(ResponseAttributesDefaultsTest, defaultJsonSendInterval)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::jsonResponse()->getSendIntervalInMilliseconds(), testing::Eq(120 * 1000)); // 120 sec
}

TEST_F(ResponseAttributesDefaultsTest, defaultJsonVisitStoreVersion)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::jsonResponse()->getVisitStoreVersion(), testing::Eq(1));
}

TEST_F(ResponseAttributesDefaultsTest, defaultJsonIsCapture)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::jsonResponse()->isCapture(), testing::Eq(true));
}

TEST_F(ResponseAttributesDefaultsTest, defaultJsonIsCaptureCrashes)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::jsonResponse()->isCaptureCrashes(), testing::Eq(true));
}

TEST_F(ResponseAttributesDefaultsTest, defaultJsonIsCaptureErrors)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::jsonResponse()->isCaptureErrors(), testing::Eq(true));
}

TEST_F(ResponseAttributesDefaultsTest, defaultJsonApplicationId)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::jsonResponse()->getApplicationId(), testing::IsEmpty());
}

TEST_F(ResponseAttributesDefaultsTest, defaultJsonMultiplicity)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::jsonResponse()->getMultiplicity(), testing::Eq(1));
}

TEST_F(ResponseAttributesDefaultsTest, defaultJsonServerId)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::jsonResponse()->getServerId(), testing::Eq(1));
}

TEST_F(ResponseAttributesDefaultsTest, defaultJsonStatus)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::jsonResponse()->getStatus(), testing::IsEmpty());
}

TEST_F(ResponseAttributesDefaultsTest, defaultJsonTimestamp)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::jsonResponse()->getTimestampInMilliseconds(), testing::Eq(0L));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Key/Value pair response tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(ResponseAttributesDefaultsTest, defaultKeyValueBeaconSize)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::keyValueResponse()->getMaxBeaconSizeInBytes(), testing::Eq(30 * 1024)); // 30 kB
}

TEST_F(ResponseAttributesDefaultsTest, defaultKeyValueSessionDuration)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::keyValueResponse()->getMaxSessionDurationInMilliseconds(), testing::Eq(-1)); // not set
}

TEST_F(ResponseAttributesDefaultsTest, defaultKeyValueEventsPerSession)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::keyValueResponse()->getMaxEventsPerSession(), testing::Eq(-1)); // not set
}

TEST_F(ResponseAttributesDefaultsTest, defaultKeyValueSessionTimeout)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::keyValueResponse()->getSessionTimeoutInMilliseconds(), testing::Eq(-1)); // not set
}

TEST_F(ResponseAttributesDefaultsTest, defaultKeyValueSendInterval)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::keyValueResponse()->getSendIntervalInMilliseconds(), testing::Eq(120000)); // 120 sec
}

TEST_F(ResponseAttributesDefaultsTest, defaultKeyValueVisitStoreVersion)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::keyValueResponse()->getVisitStoreVersion(), testing::Eq(1));
}

TEST_F(ResponseAttributesDefaultsTest, defaultKeyValueIsCapture)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::keyValueResponse()->isCapture(), testing::Eq(true));
}

TEST_F(ResponseAttributesDefaultsTest, defaultKeyValueIsCaptureCrashes)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::keyValueResponse()->isCaptureCrashes(), testing::Eq(true));
}

TEST_F(ResponseAttributesDefaultsTest, defaultKeyValueIsCaptureErrors)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::keyValueResponse()->isCaptureErrors(), testing::Eq(true));
}

TEST_F(ResponseAttributesDefaultsTest, defaultKeyValueApplicationId)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::keyValueResponse()->getApplicationId(), testing::IsEmpty());
}

TEST_F(ResponseAttributesDefaultsTest, defaultKeyValueMultiplicity)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::keyValueResponse()->getMultiplicity(), testing::Eq(1));
}

TEST_F(ResponseAttributesDefaultsTest, defaultKeyValueServerId)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::keyValueResponse()->getServerId(), testing::Eq(1));
}

TEST_F(ResponseAttributesDefaultsTest, defaultKeyValueStatus)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::keyValueResponse()->getStatus(), testing::IsEmpty());
}

TEST_F(ResponseAttributesDefaultsTest, defaultKeyValueTimestamp)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::keyValueResponse()->getTimestampInMilliseconds(), testing::Eq(0L));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Undefined defaults tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(ResponseAttributesDefaultsTest, defaultUndefinedBeaconSize)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::undefined()->getMaxBeaconSizeInBytes(), testing::Eq(30 * 1024));
}

TEST_F(ResponseAttributesDefaultsTest, defaultUndefinedSessionDuration)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::undefined()->getMaxSessionDurationInMilliseconds(), testing::Eq(-1)); // not set
}

TEST_F(ResponseAttributesDefaultsTest, defaultUndefinedEventsPerSession)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::undefined()->getMaxEventsPerSession(), testing::Eq(-1)); // not set
}

TEST_F(ResponseAttributesDefaultsTest, defaultUndefinedSessionTimeout)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::undefined()->getSessionTimeoutInMilliseconds(), testing::Eq(-1)); // not set
}

TEST_F(ResponseAttributesDefaultsTest, defaultUndefinedSendInterval)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::undefined()->getSendIntervalInMilliseconds(), testing::Eq(120 * 1000)); // 120 sec
}

TEST_F(ResponseAttributesDefaultsTest, defaultUndefinedVisitStoreVersion)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::undefined()->getVisitStoreVersion(), testing::Eq(1));
}

TEST_F(ResponseAttributesDefaultsTest, defaultUndefinedIsCapture)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::undefined()->isCapture(), testing::Eq(true));
}

TEST_F(ResponseAttributesDefaultsTest, defaultUndefinedIsCaptureCrashes)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::undefined()->isCaptureCrashes(), testing::Eq(true));
}

TEST_F(ResponseAttributesDefaultsTest, defaultUndefinedIsCaptureErrors)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::undefined()->isCaptureErrors(), testing::Eq(true));
}

TEST_F(ResponseAttributesDefaultsTest, defaultUndefinedApplicationId)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::undefined()->getApplicationId(), testing::IsEmpty());
}

TEST_F(ResponseAttributesDefaultsTest, defaultUndefinedMultiplicity)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::undefined()->getMultiplicity(), testing::Eq(1));
}

TEST_F(ResponseAttributesDefaultsTest, defaultUndefinedServerId)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::undefined()->getServerId(), testing::Eq(-1));
}

TEST_F(ResponseAttributesDefaultsTest, defaultUndefinedStatus)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::undefined()->getStatus(), testing::IsEmpty());
}

TEST_F(ResponseAttributesDefaultsTest, defaultUndefinedTimestamp)
{
	ASSERT_THAT(ResponseAttributesDefaults_t::undefined()->getTimestampInMilliseconds(), testing::Eq(0L));
}

TEST_F(ResponseAttributesDefaultsTest, defaultUndefinedMergeReturnsPassedValue)
{
	// given
	auto responseAttributes = MockIResponseAttributes::createNice();

	// when
	auto obtained = ResponseAttributesDefaults_t::undefined()->merge(responseAttributes);

	// then
	ASSERT_THAT(obtained, testing::Eq(responseAttributes));
}
