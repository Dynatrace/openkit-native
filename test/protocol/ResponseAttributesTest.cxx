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

#include "mock/MockIResponseAttributes.h"

#include "protocol/ResponseAttribute.h"
#include "protocol/ResponseAttributes.h"
#include "protocol/ResponseAttributesDefaults.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace test;

using ResponseAttribute_t = protocol::ResponseAttribute;
using ResponseAttributes_t = protocol::ResponseAttributes;
using ResponseAttributesDefaults_t = protocol::ResponseAttributesDefaults;

class ResponseAttributesTest : public testing::Test
{
};

TEST_F(ResponseAttributesTest, buildWithJsonDefaultsHasNoAttributesSetOnInstance)
{
	// given
	auto target = ResponseAttributes_t::withJsonDefaults().build();

	// when, then
	for (const auto attribute : protocol::ALL_RESPONSE_ATTRIBUTES)
	{
		ASSERT_THAT(target->isAttributeSet(attribute), testing::Eq(false));
	}
}

TEST_F(ResponseAttributesTest, buildWithKeyValueDefaultsHasNoAttributeSetOnInstance)
{
	// given
	auto target = ResponseAttributes_t::withKeyValueDefaults().build();

	// when, then
	for (const auto attribute : protocol::ALL_RESPONSE_ATTRIBUTES)
	{
		ASSERT_THAT(target->isAttributeSet(attribute), testing::Eq(false));
	}
}

TEST_F(ResponseAttributesTest, buildForwardsJsonDefaultsToInstance)
{
	// given
	auto defaults = ResponseAttributesDefaults_t::jsonResponse();
	auto target = ResponseAttributes_t::withJsonDefaults();

	// when
	auto obtained = target.build();

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
	ASSERT_THAT(obtained->getTrafficControlPercentage(), testing::Eq(defaults->getTrafficControlPercentage()));
	ASSERT_THAT(obtained->getApplicationId(), testing::Eq(defaults->getApplicationId()));

	ASSERT_THAT(obtained->getMultiplicity(), testing::Eq(defaults->getMultiplicity()));
	ASSERT_THAT(obtained->getServerId(), testing::Eq(defaults->getServerId()));
	ASSERT_THAT(obtained->getStatus(), testing::Eq(defaults->getStatus()));

	ASSERT_THAT(obtained->getTimestampInMilliseconds(), testing::Eq(defaults->getTimestampInMilliseconds()));
}

TEST_F(ResponseAttributesTest, buildForwardsKeyValueDefaultsToInstance)
{
	// given
	auto defaults = ResponseAttributesDefaults_t::jsonResponse();
	auto target = ResponseAttributes_t::withJsonDefaults();

	// when
	auto obtained = target.build();

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
	ASSERT_THAT(obtained->getTrafficControlPercentage(), testing::Eq(defaults->getTrafficControlPercentage()));
	ASSERT_THAT(obtained->getApplicationId(), testing::Eq(defaults->getApplicationId()));

	ASSERT_THAT(obtained->getMultiplicity(), testing::Eq(defaults->getMultiplicity()));
	ASSERT_THAT(obtained->getServerId(), testing::Eq(defaults->getServerId()));
	ASSERT_THAT(obtained->getStatus(), testing::Eq(defaults->getStatus()));

	ASSERT_THAT(obtained->getTimestampInMilliseconds(), testing::Eq(defaults->getTimestampInMilliseconds()));
}

TEST_F(ResponseAttributesTest, buildPropagatesStatusToInstance)
{
	// given
	const core::UTF8String status{ "status" };
	auto target = ResponseAttributes_t::withJsonDefaults();

	// when
	const auto obtained = target.withStatus(status).build();

	// then
	ASSERT_THAT(obtained->getStatus(), testing::Eq(status));
}

TEST_F(ResponseAttributesTest, withStatusSetsAttributeOnInstance)
{
	// given
	const auto attribute = ResponseAttribute_t::STATUS;
	auto target = ResponseAttributes_t::withJsonDefaults();

	// when
	const auto obtained = target.withStatus("status").build();

	// then
	ASSERT_THAT(obtained->isAttributeSet(attribute), testing::Eq(true));

	for (const auto unsetAttribute : protocol::ALL_RESPONSE_ATTRIBUTES)
	{
		if (attribute == unsetAttribute)
		{
			continue;
		}
		
		ASSERT_THAT(obtained->isAttributeSet(unsetAttribute), testing::Eq(false));
	}
}

TEST_F(ResponseAttributesTest, buildPropagatesMaxBeaconSizeToInstance)
{
	// given
	const int32_t beaconSize = 73;
	auto target = ResponseAttributes_t::withJsonDefaults();

	// when
	auto obtained = target.withMaxBeaconSizeInBytes(beaconSize).build();

	// then
	ASSERT_THAT(obtained->getMaxBeaconSizeInBytes(), testing::Eq(beaconSize));
}

TEST_F(ResponseAttributesTest, buildPropagatesApplicationIdToInstance)
{
	// given
	const core::UTF8String applicationId{"B7D8995C-F91F-4CAA-A248-7AEC8A05E261"};
	auto target = ResponseAttributes_t::withJsonDefaults();

	// when
	const auto obtained = target.withApplicationId(applicationId).build();

	// then
	ASSERT_THAT(obtained->getApplicationId(), testing::Eq(applicationId));
}

TEST_F(ResponseAttributesTest, withApplicationIdSetsAttributeOnInstance)
{
	// given
	const auto attribute = ResponseAttribute_t::APPLICATION_ID;
	const core::UTF8String applicationId{ "B7D8995C-F91F-4CAA-A248-7AEC8A05E261" };
	auto target = ResponseAttributes_t::withJsonDefaults();

	// when
	const auto obtained = target.withApplicationId(applicationId).build();

	// then
	ASSERT_THAT(obtained->getApplicationId(), testing::Eq(applicationId));
	ASSERT_THAT(obtained->isAttributeSet(attribute), testing::Eq(true));

	for (const auto unsetAttribute : protocol::ALL_RESPONSE_ATTRIBUTES)
	{
		if (attribute == unsetAttribute)
		{
			continue;
		}
		ASSERT_THAT(obtained->isAttributeSet(unsetAttribute), testing::Eq(false));
	}
}

TEST_F(ResponseAttributesTest, withMaxBeaconSizeSetsAttributeOnInstance)
{
	// given
	auto attribute = ResponseAttribute_t::MAX_BEACON_SIZE;
	auto target = ResponseAttributes_t::withJsonDefaults();

	// when
	auto obtained = target.withMaxBeaconSizeInBytes(37).build();

	// then
	ASSERT_THAT(obtained->isAttributeSet(attribute), testing::Eq(true));

	for (const auto unsetAttribute : protocol::ALL_RESPONSE_ATTRIBUTES)
	{
		if (attribute == unsetAttribute)
		{
			continue;
		}

		ASSERT_THAT(obtained->isAttributeSet(unsetAttribute), testing::Eq(false));
	}
}

TEST_F(ResponseAttributesTest, buildPropagatesMaxSessionDurationToInstance)
{
	// given
	const int32_t sessionDuration = 73;
	auto target = ResponseAttributes_t::withJsonDefaults();

	// when
	auto obtained = target.withMaxSessionDurationInMilliseconds(sessionDuration).build();

	// then
	ASSERT_THAT(obtained->getMaxSessionDurationInMilliseconds(), testing::Eq(sessionDuration));
}

TEST_F(ResponseAttributesTest, withMaxSessionDurationSetsAttributeOnInstance)
{
	// given
	auto attribute = ResponseAttribute_t::MAX_SESSION_DURATION;
	auto target = ResponseAttributes_t::withJsonDefaults();

	// when
	auto obtained = target.withMaxSessionDurationInMilliseconds(37).build();

	// then
	ASSERT_THAT(obtained->isAttributeSet(attribute), testing::Eq(true));

	for (const auto unsetAttribute : protocol::ALL_RESPONSE_ATTRIBUTES)
	{
		if (attribute == unsetAttribute)
		{
			continue;
		}

		ASSERT_THAT(obtained->isAttributeSet(unsetAttribute), testing::Eq(false));
	}
}

TEST_F(ResponseAttributesTest, buildPropagatesMaxEventsPerSessionToInstance)
{
	// given
	const int32_t eventsPerSession = 73;
	auto target = ResponseAttributes_t::withJsonDefaults();

	// when
	auto obtained = target.withMaxEventsPerSession(eventsPerSession).build();

	// then
	ASSERT_THAT(obtained->getMaxEventsPerSession(), testing::Eq(eventsPerSession));
}

TEST_F(ResponseAttributesTest, withMaxEventsPerSessionSetsAttributeOnInstance)
{
	// given
	auto attribute = ResponseAttribute_t::MAX_EVENTS_PER_SESSION;
	auto target = ResponseAttributes_t::withJsonDefaults();

	// when
	auto obtained = target.withMaxEventsPerSession(37).build();

	// then
	ASSERT_THAT(obtained->isAttributeSet(attribute), testing::Eq(true));

	for (const auto unsetAttribute : protocol::ALL_RESPONSE_ATTRIBUTES)
	{
		if (attribute == unsetAttribute)
		{
			continue;
		}

		ASSERT_THAT(obtained->isAttributeSet(unsetAttribute), testing::Eq(false));
	}
}

TEST_F(ResponseAttributesTest, buildPropagatesSessionTimeoutToInstance)
{
	// given
	const int32_t sessionTimeout = 73;
	auto target = ResponseAttributes_t::withJsonDefaults();

	// when
	auto obtained = target.withSessionTimeoutInMilliseconds(sessionTimeout).build();

	// then
	ASSERT_THAT(obtained->getSessionTimeoutInMilliseconds(), testing::Eq(sessionTimeout));
}

TEST_F(ResponseAttributesTest, withSessionTimeoutSetsAttributeOnInstance)
{
	// given
	auto attribute = ResponseAttribute_t::SESSION_IDLE_TIMEOUT;
	auto target = ResponseAttributes_t::withJsonDefaults();

	// when
	auto obtained = target.withSessionTimeoutInMilliseconds(37).build();

	// then
	ASSERT_THAT(obtained->isAttributeSet(attribute), testing::Eq(true));

	for (const auto unsetAttribute : protocol::ALL_RESPONSE_ATTRIBUTES)
	{
		if (attribute == unsetAttribute)
		{
			continue;
		}

		ASSERT_THAT(obtained->isAttributeSet(unsetAttribute), testing::Eq(false));
	}
}

TEST_F(ResponseAttributesTest, buildPropagatesSendIntervalToInstance)
{
	// given
	const int32_t sendInterval = 73;
	auto target = ResponseAttributes_t::withJsonDefaults();

	// when
	auto obtained = target.withSendIntervalInMilliseconds(sendInterval).build();

	// then
	ASSERT_THAT(obtained->getSendIntervalInMilliseconds(), testing::Eq(sendInterval));
}

TEST_F(ResponseAttributesTest, withSendIntervalSetsAttributeOnInstance)
{
	// given
	auto attribute = ResponseAttribute_t::SEND_INTERVAL;
	auto target = ResponseAttributes_t::withJsonDefaults();

	// when
	auto obtained = target.withSendIntervalInMilliseconds(37).build();

	// then
	ASSERT_THAT(obtained->isAttributeSet(attribute), testing::Eq(true));

	for (const auto unsetAttribute : protocol::ALL_RESPONSE_ATTRIBUTES)
	{
		if (attribute == unsetAttribute)
		{
			continue;
		}

		ASSERT_THAT(obtained->isAttributeSet(unsetAttribute), testing::Eq(false));
	}
}

TEST_F(ResponseAttributesTest, buildPropagatesVisitStoreVersionToInstance)
{
	// given
	const int32_t visitStoreVersion = 73;
	auto target = ResponseAttributes_t::withJsonDefaults();

	// when
	auto obtained = target.withVisitStoreVersion(visitStoreVersion).build();

	// then
	ASSERT_THAT(obtained->getVisitStoreVersion(), testing::Eq(visitStoreVersion));
}

TEST_F(ResponseAttributesTest, withVisitStoreVersionSetsAttributeOnInstance)
{
	// given
	auto attribute = ResponseAttribute_t::VISIT_STORE_VERSION;
	auto target = ResponseAttributes_t::withJsonDefaults();

	// when
	auto obtained = target.withVisitStoreVersion(37).build();

	// then
	ASSERT_THAT(obtained->isAttributeSet(attribute), testing::Eq(true));

	for (const auto unsetAttribute : protocol::ALL_RESPONSE_ATTRIBUTES)
	{
		if (attribute == unsetAttribute)
		{
			continue;
		}

		ASSERT_THAT(obtained->isAttributeSet(unsetAttribute), testing::Eq(false));
	}
}

TEST_F(ResponseAttributesTest, buildPropagatesIsCaptureToInstance)
{
	// given
	auto isCapture = !ResponseAttributesDefaults_t::jsonResponse()->isCapture();
	auto target = ResponseAttributes_t::withJsonDefaults();

	// when
	auto obtained = target.withCapture(isCapture).build();

	// then
	ASSERT_THAT(obtained->isCapture(), testing::Eq(isCapture));
}

TEST_F(ResponseAttributesTest, withCaptureSetsAttributeOnInstance)
{
	// given
	auto attribute = ResponseAttribute_t::IS_CAPTURE;
	auto target = ResponseAttributes_t::withJsonDefaults();

	// when
	auto obtained = target.withCapture(!ResponseAttributesDefaults_t::jsonResponse()->isCapture()).build();

	// then
	ASSERT_THAT(obtained->isAttributeSet(attribute), testing::Eq(true));

	for (const auto unsetAttribute : protocol::ALL_RESPONSE_ATTRIBUTES)
	{
		if (attribute == unsetAttribute)
		{
			continue;
		}

		ASSERT_THAT(obtained->isAttributeSet(unsetAttribute), testing::Eq(false));
	}
}

TEST_F(ResponseAttributesTest, buildPropagatesIsCaptureCrashesToInstance)
{
	// given
	auto isCaptureCrashes = !ResponseAttributesDefaults_t::jsonResponse()->isCaptureCrashes();
	auto target = ResponseAttributes_t::withJsonDefaults();

	// when
	auto obtained = target.withCaptureCrashes(isCaptureCrashes).build();

	// then
	ASSERT_THAT(obtained->isCaptureCrashes(), testing::Eq(isCaptureCrashes));
}

TEST_F(ResponseAttributesTest, withCaptureCrashesSetsAttributeOnInstance)
{
	// given
	auto attribute = ResponseAttribute_t::IS_CAPTURE_CRASHES;
	auto target = ResponseAttributes_t::withJsonDefaults();

	// when
	auto obtained = target.withCaptureCrashes(!ResponseAttributesDefaults_t::jsonResponse()->isCaptureCrashes()).build();

	// then
	ASSERT_THAT(obtained->isAttributeSet(attribute), testing::Eq(true));

	for (const auto unsetAttribute : protocol::ALL_RESPONSE_ATTRIBUTES)
	{
		if (attribute == unsetAttribute)
		{
			continue;
		}

		ASSERT_THAT(obtained->isAttributeSet(unsetAttribute), testing::Eq(false));
	}
}

TEST_F(ResponseAttributesTest, buildPropagatesIsCaptureErrorsToInstance)
{
	// given
	auto isCaptureErrors = !ResponseAttributesDefaults_t::jsonResponse()->isCaptureErrors();
	auto target = ResponseAttributes_t::withJsonDefaults();

	// when
	auto obtained = target.withCaptureErrors(isCaptureErrors).build();

	// then
	ASSERT_THAT(obtained->isCaptureErrors(), testing::Eq(isCaptureErrors));
}

TEST_F(ResponseAttributesTest, withCaptureErrorsSetsAttributeOnInstance)
{
	// given
	auto attribute = ResponseAttribute_t::IS_CAPTURE_ERRORS;
	auto target = ResponseAttributes_t::withJsonDefaults();

	// when
	auto obtained = target.withCaptureErrors(!ResponseAttributesDefaults_t::jsonResponse()->isCaptureErrors()).build();

	// then
	ASSERT_THAT(obtained->isAttributeSet(attribute), testing::Eq(true));

	for (const auto unsetAttribute : protocol::ALL_RESPONSE_ATTRIBUTES)
	{
		if (attribute == unsetAttribute)
		{
			continue;
		}

		ASSERT_THAT(obtained->isAttributeSet(unsetAttribute), testing::Eq(false));
	}
}

TEST_F(ResponseAttributesTest, buildPropagatesTrafficControlPercentageToInstance)
{
	// given
	const auto trafficControlPercentage = 65;
	auto target = ResponseAttributes_t::withJsonDefaults();

	// when
	auto obtained = target.withTrafficControlPercentage(trafficControlPercentage).build();

	// then
	ASSERT_THAT(obtained->getTrafficControlPercentage(), testing::Eq(trafficControlPercentage));
}

TEST_F(ResponseAttributesTest, withTrafficControlPercentageSetsAttributeOnInstance)
{
	// given
	auto attribute = ResponseAttribute_t::TRAFFIC_CONTROL_PERCENTAGE;
	auto target = ResponseAttributes_t::withJsonDefaults();

	// when
	auto obtained = target.withTrafficControlPercentage(42).build();

	// then
	ASSERT_THAT(obtained->isAttributeSet(attribute), testing::Eq(true));

	for (const auto unsetAttribute : protocol::ALL_RESPONSE_ATTRIBUTES)
	{
		if (attribute == unsetAttribute)
		{
			continue;
		}

		ASSERT_THAT(obtained->isAttributeSet(unsetAttribute), testing::Eq(false));
	}
}

TEST_F(ResponseAttributesTest, buildPropagatesMultiplicityToInstance)
{
	// given
	const int32_t multiplicity = 73;
	auto target = ResponseAttributes_t::withJsonDefaults();

	// when
	auto obtained = target.withMultiplicity(multiplicity).build();

	// then
	ASSERT_THAT(obtained->getMultiplicity(), testing::Eq(multiplicity));
}

TEST_F(ResponseAttributesTest, withMultiplicitySetsAttributeOnInstance)
{
	// given
	auto attribute = ResponseAttribute_t::MULTIPLICITY;
	auto target = ResponseAttributes_t::withJsonDefaults();

	// when
	auto obtained = target.withMultiplicity(37).build();

	// then
	ASSERT_THAT(obtained->isAttributeSet(attribute), testing::Eq(true));

	for (const auto unsetAttribute : protocol::ALL_RESPONSE_ATTRIBUTES)
	{
		if (attribute == unsetAttribute)
		{
			continue;
		}

		ASSERT_THAT(obtained->isAttributeSet(unsetAttribute), testing::Eq(false));
	}
}

TEST_F(ResponseAttributesTest, buildPropagatesServerIdToInstance)
{
	// given
	const int32_t serverId = 73;
	auto target = ResponseAttributes_t::withJsonDefaults();

	// when
	auto obtained = target.withServerId(serverId).build();

	// then
	ASSERT_THAT(obtained->getServerId(), testing::Eq(serverId));
}

TEST_F(ResponseAttributesTest, withServerIdSetsAttributeOnInstance)
{
	// given
	auto attribute = ResponseAttribute_t::SERVER_ID;
	auto target = ResponseAttributes_t::withJsonDefaults();

	// when
	auto obtained = target.withServerId(37).build();

	// then
	ASSERT_THAT(obtained->isAttributeSet(attribute), testing::Eq(true));

	for (const auto unsetAttribute : protocol::ALL_RESPONSE_ATTRIBUTES)
	{
		if (attribute == unsetAttribute)
		{
			continue;
		}

		ASSERT_THAT(obtained->isAttributeSet(unsetAttribute), testing::Eq(false));
	}
}

TEST_F(ResponseAttributesTest, buildPropagatesTimestampToInstance)
{
	// given
	const int64_t timestamp = 73;
	auto target = ResponseAttributes_t::withJsonDefaults();

	// when
	auto obtained = target.withTimestampInMilliseconds(timestamp).build();

	// then
	ASSERT_THAT(obtained->getTimestampInMilliseconds(), testing::Eq(timestamp));
}

TEST_F(ResponseAttributesTest, mergeTakesStatusFromMergeTargetIfNotSetInSource)
{
	// given
	const core::UTF8String status{ "status" };
	auto source = ResponseAttributes_t::withUndefinedDefaults().build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().withStatus(status).build();

	// when
	const auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getStatus(), testing::Eq(status));
}

TEST_F(ResponseAttributesTest, mergeTakesStatusFromMergeSourceIfSetInSource)
{
	// given
	const core::UTF8String status{ "status" };
	auto source = ResponseAttributes_t::withKeyValueDefaults().withStatus(status).build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().build();

	// when
	const auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getStatus(), testing::Eq(status));
}

TEST_F(ResponseAttributesTest, mergeTakesStatusFromMergeSourceIfSetInSourceAndTarget)
{
	// given
	const core::UTF8String status{ "status" };
	const core::UTF8String foobar{ "foobar" };
	auto source = ResponseAttributes_t::withKeyValueDefaults().withStatus(status).build();
	auto target = ResponseAttributes_t::withKeyValueDefaults().withStatus(foobar).build();

	// when
	const auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getStatus(), testing::Eq(status));
}

TEST_F(ResponseAttributesTest, withTimestampSetsAttributeOnInstance)
{
	// given
	auto attribute = ResponseAttribute_t::TIMESTAMP;
	auto target = ResponseAttributes_t::withJsonDefaults();

	// when
	auto obtained = target.withTimestampInMilliseconds(37L).build();

	// then
	ASSERT_THAT(obtained->isAttributeSet(attribute), testing::Eq(true));

	for (const auto unsetAttribute : protocol::ALL_RESPONSE_ATTRIBUTES)
	{
		if (attribute == unsetAttribute)
		{
			continue;
		}

		ASSERT_THAT(obtained->isAttributeSet(unsetAttribute), testing::Eq(false));
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// merge response tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(ResponseAttributesTest, mergingDefaultResponsesReturnsResponseWithoutAnyAttributeSet)
{
	// given
	auto toMerge = ResponseAttributes_t::withUndefinedDefaults().build();
	auto target = ResponseAttributes_t::withJsonDefaults().build();

	// when
	auto obtained = target->merge(toMerge);

	// then
	for (const auto attribute : protocol::ALL_RESPONSE_ATTRIBUTES)
	{
		ASSERT_THAT(obtained->isAttributeSet(attribute), testing::Eq(false));
	}
}

TEST_F(ResponseAttributesTest, mergeResponseWithAllValuesSetToDefaultResponse)
{
	// given
	auto toMerge = MockIResponseAttributes::createNice();
	ON_CALL(*toMerge, isAttributeSet(testing::_)).WillByDefault(testing::Return(true));
	auto target = ResponseAttributes_t::withUndefinedDefaults().build();

	// when
	auto obtained = target->merge(toMerge);

	// then
	for (const auto attribute : protocol::ALL_RESPONSE_ATTRIBUTES)
	{
		ASSERT_THAT(obtained->isAttributeSet(attribute), testing::Eq(true));
	}
}

TEST_F(ResponseAttributesTest, mergeTakesBeaconSizeFromMergeTargetIfNotSetInSource)
{
	// given
	const int32_t beaconSize = 73;
	auto source = ResponseAttributes_t::withUndefinedDefaults().build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().withMaxBeaconSizeInBytes(beaconSize).build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getMaxBeaconSizeInBytes(), testing::Eq(beaconSize));
}

TEST_F(ResponseAttributesTest, mergeTakesBeaconSizeFromMergeSourceIfSetInSource)
{
	// given
	const int32_t beaconSize = 73;
	auto source = ResponseAttributes_t::withUndefinedDefaults().withMaxBeaconSizeInBytes(beaconSize).build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getMaxBeaconSizeInBytes(), testing::Eq(beaconSize));
}

TEST_F(ResponseAttributesTest, mergeTakesBeaconSizeFromMergeSourceIfSetInSourceAndTarget)
{
	// given
	const int32_t beaconSize = 73;
	auto source = ResponseAttributes_t::withUndefinedDefaults().withMaxBeaconSizeInBytes(beaconSize).build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().withMaxBeaconSizeInBytes(37).build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getMaxBeaconSizeInBytes(), testing::Eq(beaconSize));
}

TEST_F(ResponseAttributesTest, mergeTakesSessionDurationFromMergeTargetIfNotSetInSource)
{
	// given
	const int32_t sessionDuration = 73;
	auto source = ResponseAttributes_t::withUndefinedDefaults().build();
	auto target = ResponseAttributes_t::withUndefinedDefaults()
		.withMaxSessionDurationInMilliseconds(sessionDuration).build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getMaxSessionDurationInMilliseconds(), testing::Eq(sessionDuration));
}

TEST_F(ResponseAttributesTest, mergeTakesSessionDurationFromMergeSourceIfSetInSource)
{
	// given
	const int32_t sessionDuration = 73;
	auto source = ResponseAttributes_t::withUndefinedDefaults()
		.withMaxSessionDurationInMilliseconds(sessionDuration).build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getMaxSessionDurationInMilliseconds(), testing::Eq(sessionDuration));
}

TEST_F(ResponseAttributesTest, mergeTakesSessionDurationFromMergeSourceIfSetInSourceAndTarget)
{
	// given
	const int32_t sessionDuration = 73;
	auto source = ResponseAttributes_t::withUndefinedDefaults()
		.withMaxSessionDurationInMilliseconds(sessionDuration).build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().withMaxSessionDurationInMilliseconds(37).build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getMaxSessionDurationInMilliseconds(), testing::Eq(sessionDuration));
}

TEST_F(ResponseAttributesTest, mergeTakesEventsPerSessionFromMergeTargetIfNotSetInSource)
{
	// given
	const int32_t eventsPerSession = 73;
	auto source = ResponseAttributes_t::withUndefinedDefaults().build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().withMaxEventsPerSession(eventsPerSession).build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getMaxEventsPerSession(), testing::Eq(eventsPerSession));
}

TEST_F(ResponseAttributesTest, mergeTakesEventsPerSessionFromMergeSourceIfSetInSource)
{
	// given
	const int32_t eventsPerSession = 73;
	auto source = ResponseAttributes_t::withUndefinedDefaults()
		.withMaxEventsPerSession(eventsPerSession).build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getMaxEventsPerSession(), testing::Eq(eventsPerSession));
}

TEST_F(ResponseAttributesTest, mergeTakesEventsPerSessionFromMergeSourceIfSetInSourceAndTarget)
{
	// given
	const int32_t eventsPerSession = 73;
	auto source = ResponseAttributes_t::withUndefinedDefaults().withMaxEventsPerSession(eventsPerSession).build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().withMaxEventsPerSession(37).build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getMaxEventsPerSession(), testing::Eq(eventsPerSession));
}

TEST_F(ResponseAttributesTest, mergeTakesSessionTimeoutFromMergeTargetIfNotSetInSource)
{
	// given
	const int32_t sessionTimeout = 73;
	auto source = ResponseAttributes_t::withUndefinedDefaults().build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().withSessionTimeoutInMilliseconds(sessionTimeout)
		.build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getSessionTimeoutInMilliseconds(), testing::Eq(sessionTimeout));
}

TEST_F(ResponseAttributesTest, mergeTakesSessionTimeoutFromMergeSourceIfSetInSource)
{
	// given
	const int32_t sessionTimeout = 73;
	auto source = ResponseAttributes_t::withUndefinedDefaults().withSessionTimeoutInMilliseconds(sessionTimeout)
		.build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getSessionTimeoutInMilliseconds(), testing::Eq(sessionTimeout));
}

TEST_F(ResponseAttributesTest, mergeTakesSessionTimeoutFromMergeSourceIfSetInSourceAndTarget)
{
	// given
	const int32_t sessionTimeout = 73;
	auto source = ResponseAttributes_t::withUndefinedDefaults().withSessionTimeoutInMilliseconds(sessionTimeout)
		.build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().withSessionTimeoutInMilliseconds(37).build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getSessionTimeoutInMilliseconds(), testing::Eq(sessionTimeout));
}

TEST_F(ResponseAttributesTest, mergeTakesSendIntervalFromMergeTargetIfNotSetInSource)
{
	// given
	const int32_t sendInterval = 73;
	auto source = ResponseAttributes_t::withUndefinedDefaults().build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().withSendIntervalInMilliseconds(sendInterval)
		.build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getSendIntervalInMilliseconds(), testing::Eq(sendInterval));
}

TEST_F(ResponseAttributesTest, mergeTakesSendIntervalFromMergeSourceIfSetInSource)
{
	// given
	const int32_t sendInterval = 73;
	auto source = ResponseAttributes_t::withUndefinedDefaults().withSendIntervalInMilliseconds(sendInterval)
		.build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getSendIntervalInMilliseconds(), testing::Eq(sendInterval));
}

TEST_F(ResponseAttributesTest, mergeTakesSendIntervalFromMergeSourceIfSetInSourceAndTarget)
{
	// given
	const int32_t sendInterval = 73;
	auto source = ResponseAttributes_t::withUndefinedDefaults().withSendIntervalInMilliseconds(sendInterval)
		.build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().withSendIntervalInMilliseconds(37).build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getSendIntervalInMilliseconds(), testing::Eq(sendInterval));
}

TEST_F(ResponseAttributesTest, mergeTakesVisitStoreVersionFromMergeTargetIfNotSetInSource)
{
	// given
	const int32_t visitStoreVersion = 73;
	auto source = ResponseAttributes_t::withUndefinedDefaults().build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().withVisitStoreVersion(visitStoreVersion).build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getVisitStoreVersion(), testing::Eq(visitStoreVersion));
}

TEST_F(ResponseAttributesTest, mergeTakesVisitStoreVersionFromMergeSourceIfSetInSource)
{
	// given
	const int32_t visitStoreVersion = 73;
	auto source = ResponseAttributes_t::withUndefinedDefaults()
		.withVisitStoreVersion(visitStoreVersion).build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getVisitStoreVersion(), testing::Eq(visitStoreVersion));
}

TEST_F(ResponseAttributesTest, ergeTakesVisitStoreVersionFromMergeSourceIfSetInSourceAndTarget)
{
	// given
	const int32_t visitStoreVersion = 73;
	auto source = ResponseAttributes_t::withUndefinedDefaults().withVisitStoreVersion(visitStoreVersion).build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().withVisitStoreVersion(37).build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getVisitStoreVersion(), testing::Eq(visitStoreVersion));
}

TEST_F(ResponseAttributesTest, mergeTakesCaptureFromMergeTargetIfNotSetInSource)
{
	// given
	auto capture = !ResponseAttributesDefaults_t::undefined()->isCapture();
	auto source = ResponseAttributes_t::withUndefinedDefaults().build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().withCapture(capture).build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->isCapture(), testing::Eq(capture));
}

TEST_F(ResponseAttributesTest, mergeTakesCaptureFromMergeSourceIfSetInSource)
{
	// given
	auto capture = !ResponseAttributesDefaults_t::undefined()->isCapture();
	auto source = ResponseAttributes_t::withUndefinedDefaults().withCapture(capture).build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->isCapture(), testing::Eq(capture));
}

TEST_F(ResponseAttributesTest, mergeTakesCaptureFromMergeSourceIfSetInSourceAndTarget)
{
	// given
	auto capture = !ResponseAttributesDefaults_t::undefined()->isCapture();
	auto source = ResponseAttributes_t::withUndefinedDefaults().withCapture(capture).build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().withCapture(!capture).build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->isCapture(), testing::Eq(capture));
}

TEST_F(ResponseAttributesTest, mergeTakesCaptureCrashesFromMergeTargetIfNotSetInSource)
{
	// given
	auto captureCrashes = !ResponseAttributesDefaults_t::undefined()->isCaptureCrashes();
	auto source = ResponseAttributes_t::withUndefinedDefaults().build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().withCaptureCrashes(captureCrashes).build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->isCaptureCrashes(), testing::Eq(captureCrashes));
}

TEST_F(ResponseAttributesTest, mergeTakesCaptureCrashesFromMergeSourceIfSetInSource)
{
	// given
	auto captureCrashes = !ResponseAttributesDefaults_t::undefined()->isCaptureCrashes();
	auto source = ResponseAttributes_t::withUndefinedDefaults().withCaptureCrashes(captureCrashes).build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->isCaptureCrashes(), testing::Eq(captureCrashes));
}

TEST_F(ResponseAttributesTest, mergeTakesCaptureCrashesFromMergeSourceIfSetInSourceAndTarget)
{
	// given
	auto captureCrashes = !ResponseAttributesDefaults_t::undefined()->isCaptureCrashes();
	auto source = ResponseAttributes_t::withUndefinedDefaults().withCaptureCrashes(captureCrashes).build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().withCaptureCrashes(!captureCrashes).build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->isCaptureCrashes(), testing::Eq(captureCrashes));
}

TEST_F(ResponseAttributesTest, mergeTakesCaptureErrorsFromMergeTargetIfNotSetInSource)
{
	// given
	auto captureErrors = !ResponseAttributesDefaults_t::undefined()->isCaptureErrors();
	auto source = ResponseAttributes_t::withUndefinedDefaults().build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().withCaptureErrors(captureErrors).build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->isCaptureErrors(), testing::Eq(captureErrors));
}

TEST_F(ResponseAttributesTest, mergeTakesCaptureErrorsFromMergeSourceIfSetInSource)
{
	// given
	auto captureErrors = !ResponseAttributesDefaults_t::undefined()->isCaptureErrors();
	auto source = ResponseAttributes_t::withUndefinedDefaults().withCaptureErrors(captureErrors).build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->isCaptureErrors(), testing::Eq(captureErrors));
}

TEST_F(ResponseAttributesTest, mergeTakesCaptureErrorsFromMergeSourceIfSetInSourceAndTarget)
{
	// given
	auto captureErrors = !ResponseAttributesDefaults_t::undefined()->isCaptureErrors();
	auto source = ResponseAttributes_t::withUndefinedDefaults().withCaptureErrors(captureErrors).build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().withCaptureErrors(!captureErrors).build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->isCaptureErrors(), testing::Eq(captureErrors));
}

TEST_F(ResponseAttributesTest, mergeTakesTrafficControlPercentageFromMergeTargetIfNotSetInSource)
{
	// given
	const auto trafficControlPercentage = 73;
	auto source = ResponseAttributes_t::withUndefinedDefaults().build();
	auto target = ResponseAttributes_t::withUndefinedDefaults()
		.withTrafficControlPercentage(trafficControlPercentage)
		.build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getTrafficControlPercentage(), testing::Eq(trafficControlPercentage));
}

TEST_F(ResponseAttributesTest, mergeTakesTrafficControlPercentageFromMergeSourceIfSetInSource)
{
	// given
	const auto trafficControlPercentage = 73;
	auto source = ResponseAttributes_t::withUndefinedDefaults()
		.withTrafficControlPercentage(trafficControlPercentage)
		.build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getTrafficControlPercentage(), testing::Eq(trafficControlPercentage));
}

TEST_F(ResponseAttributesTest, mergeTakesTrafficControlPercentageFromMergeSourceIfSetInSourceAndTarget)
{
	// given
	const auto trafficControlPercentage = 73;
	auto source = ResponseAttributes_t::withUndefinedDefaults()
		.withTrafficControlPercentage(trafficControlPercentage)
		.build();
	auto target = ResponseAttributes_t::withUndefinedDefaults()
		.withTrafficControlPercentage(37)
		.build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getTrafficControlPercentage(), testing::Eq(trafficControlPercentage));
}

TEST_F(ResponseAttributesTest, mergeTakesApplicationIdFromMergeTargetIfNotSetInSource)
{
	// given
	const core::UTF8String applicationId{"034D4C04-899B-458F-8514-268CC735BE7F"};
	auto source = ResponseAttributes_t::withUndefinedDefaults().build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().withApplicationId(applicationId).build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getApplicationId(), testing::Eq(applicationId));
}

TEST_F(ResponseAttributesTest, mergeTakesApplicationIdFromMergeSourceIfSetInSource)
{
	// given
	const core::UTF8String applicationId{ "034D4C04-899B-458F-8514-268CC735BE7F" };
	auto source = ResponseAttributes_t::withUndefinedDefaults().withApplicationId(applicationId).build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getApplicationId(), testing::Eq(applicationId));
}

TEST_F(ResponseAttributesTest, mergeTakesApplicationIdFromMergeSourceIfSetInSourceAndTarget)
{
	// given
	const core::UTF8String applicationId{ "034D4C04-899B-458F-8514-268CC735BE7F" };
	auto source = ResponseAttributes_t::withUndefinedDefaults().withApplicationId(applicationId).build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().withApplicationId("C5A175BD-F553-419B-9AD4-B57373676030").build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getApplicationId(), testing::Eq(applicationId));
}

TEST_F(ResponseAttributesTest, mergeTakesMultiplicityFromMergeTargetIfNotSetInSource)
{
	// given
	const int32_t multiplicity = 73;
	auto source = ResponseAttributes_t::withUndefinedDefaults().build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().withMultiplicity(multiplicity).build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getMultiplicity(), testing::Eq(multiplicity));
}

TEST_F(ResponseAttributesTest, mergeTakesMultiplicityFromMergeSourceIfSetInSource)
{
	// given
	const int32_t multiplicity = 73;
	auto source = ResponseAttributes_t::withUndefinedDefaults().withMultiplicity(multiplicity).build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getMultiplicity(), testing::Eq(multiplicity));
}

TEST_F(ResponseAttributesTest, mergeTakesMultiplicityFromMergeSourceIfSetInSourceAndTarget)
{
	// given
	const int32_t multiplicity = 73;
	auto source = ResponseAttributes_t::withUndefinedDefaults().withMultiplicity(multiplicity).build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().withMultiplicity(37).build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getMultiplicity(), testing::Eq(multiplicity));
}

TEST_F(ResponseAttributesTest, mergeTakesServerIdFromMergeTargetIfNotSetInSource)
{
	// given
	const int32_t serverId = 73;
	auto source = ResponseAttributes_t::withUndefinedDefaults().build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().withServerId(serverId).build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getServerId(), testing::Eq(serverId));
}

TEST_F(ResponseAttributesTest, mergeTakesServerIdFromMergeSourceIfSetInSource)
{
	// given
	const int32_t serverId = 73;
	auto source = ResponseAttributes_t::withUndefinedDefaults().withServerId(serverId).build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getServerId(), testing::Eq(serverId));
}

TEST_F(ResponseAttributesTest, mergeTakesServerIdFromMergeSourceIfSetInSourceAndTarget)
{
	// given
	const int32_t serverId = 73;
	auto source = ResponseAttributes_t::withUndefinedDefaults().withServerId(serverId).build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().withServerId(37).build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getServerId(), testing::Eq(serverId));
}

TEST_F(ResponseAttributesTest, mergeTakesTimestampFromMergeTargetIfNotSetInSource)
{
	// given
	const int64_t timestamp = 73;
	auto source = ResponseAttributes_t::withUndefinedDefaults().build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().withTimestampInMilliseconds(timestamp).build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getTimestampInMilliseconds(), testing::Eq(timestamp));
}

TEST_F(ResponseAttributesTest, mergeTakesTimestampFromMergeSourceIfSetInSource)
{
	// given
	const int64_t timestamp = 73;
	auto source = ResponseAttributes_t::withUndefinedDefaults().withTimestampInMilliseconds(timestamp).build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getTimestampInMilliseconds(), testing::Eq(timestamp));
}

TEST_F(ResponseAttributesTest, mergeTakesTimestampFromMergeSourceIfSetInSourceAndTarget)
{
	// given
	const int64_t timestamp = 73;
	auto source = ResponseAttributes_t::withUndefinedDefaults().withTimestampInMilliseconds(timestamp).build();
	auto target = ResponseAttributes_t::withUndefinedDefaults().withTimestampInMilliseconds(37).build();

	// when
	auto obtained = target->merge(source);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getTimestampInMilliseconds(), testing::Eq(timestamp));
}