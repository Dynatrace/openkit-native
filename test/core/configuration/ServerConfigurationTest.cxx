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

#include "../../protocol/mock/MockIResponseAttributes.h"
#include "../../core/configuration/mock/MockIServerConfiguration.h"

#include "core/configuration/ServerConfiguration.h"
#include "protocol/ResponseAttributesDefaults.h"
#include "protocol/ResponseAttribute.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace test;

using IResponseAttributes_sp = std::shared_ptr<protocol::IResponseAttributes>;
using ResponseAttributesDefaults_t = protocol::ResponseAttributesDefaults;
using ResponseAttribute_t = protocol::ResponseAttribute;
using ServerConfiguration_t = core::configuration::ServerConfiguration;
using MockIResponseAttributes_sp = std::shared_ptr<MockIResponseAttributes>;
using MockIServerConfiguration_sp = std::shared_ptr<MockIServerConfiguration>;

class ServerConfigurationTest : public testing::Test
{
protected:

	IResponseAttributes_sp defaultValues;
	MockIResponseAttributes_sp mockAttributes;
	MockIServerConfiguration_sp mockServerConfiguration;

	void SetUp() override
	{
		defaultValues = ResponseAttributesDefaults_t::undefined();
		mockAttributes = MockIResponseAttributes::createNice();
		ON_CALL(*mockAttributes, isCapture())
			.WillByDefault(testing::Return(defaultValues->isCapture()));
		ON_CALL(*mockAttributes, isCaptureCrashes())
			.WillByDefault(testing::Return(defaultValues->isCaptureCrashes()));
		ON_CALL(*mockAttributes, isCaptureErrors())
			.WillByDefault(testing::Return(defaultValues->isCaptureErrors()));
		ON_CALL(*mockAttributes, getServerId())
			.WillByDefault(testing::Return(defaultValues->getServerId()));
		ON_CALL(*mockAttributes, getMaxBeaconSizeInBytes())
			.WillByDefault(testing::Return(defaultValues->getMaxBeaconSizeInBytes()));
		ON_CALL(*mockAttributes, getMultiplicity())
			.WillByDefault(testing::Return(defaultValues->getMultiplicity()));
		ON_CALL(*mockAttributes, getSendIntervalInMilliseconds())
			.WillByDefault(testing::Return(defaultValues->getSendIntervalInMilliseconds()));
		ON_CALL(*mockAttributes, getMaxSessionDurationInMilliseconds())
			.WillByDefault(testing::Return(defaultValues->getMaxSessionDurationInMilliseconds()));
		ON_CALL(*mockAttributes, getMaxEventsPerSession())
			.WillByDefault(testing::Return(defaultValues->getMaxEventsPerSession()));
		ON_CALL(*mockAttributes, getSessionTimeoutInMilliseconds())
			.WillByDefault(testing::Return(defaultValues->getSessionTimeoutInMilliseconds()));
		ON_CALL(*mockAttributes, getVisitStoreVersion())
			.WillByDefault(testing::Return(defaultValues->getVisitStoreVersion()));
		ON_CALL(*mockAttributes, getTrafficControlPercentage())
			.WillByDefault(testing::Return(defaultValues->getTrafficControlPercentage()));
		ON_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::MAX_SESSION_DURATION))
			.WillByDefault(testing::Return(false));
		ON_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::MAX_EVENTS_PER_SESSION))
			.WillByDefault(testing::Return(false));
		ON_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::SESSION_IDLE_TIMEOUT))
			.WillByDefault(testing::Return(false));

		mockServerConfiguration = MockIServerConfiguration::createNice();
		ON_CALL(*mockServerConfiguration, isCaptureEnabled())
			.WillByDefault(testing::Return(defaultValues->isCapture()));
		ON_CALL(*mockServerConfiguration, isCrashReportingEnabled())
			.WillByDefault(testing::Return(defaultValues->isCaptureCrashes()));
		ON_CALL(*mockServerConfiguration, isErrorReportingEnabled())
			.WillByDefault(testing::Return(defaultValues->isCaptureErrors()));
		ON_CALL(*mockServerConfiguration, getServerId())
			.WillByDefault(testing::Return(defaultValues->getServerId()));
		ON_CALL(*mockServerConfiguration, getBeaconSizeInBytes())
			.WillByDefault(testing::Return(defaultValues->getMaxBeaconSizeInBytes()));
		ON_CALL(*mockServerConfiguration, getMultiplicity())
			.WillByDefault(testing::Return(defaultValues->getMultiplicity()));
		ON_CALL(*mockServerConfiguration, getSendIntervalInMilliseconds())
			.WillByDefault(testing::Return(defaultValues->getSendIntervalInMilliseconds()));
		ON_CALL(*mockServerConfiguration, getMaxSessionDurationInMilliseconds())
			.WillByDefault(testing::Return(defaultValues->getMaxSessionDurationInMilliseconds()));
		ON_CALL(*mockServerConfiguration, isSessionSplitBySessionDurationEnabled())
			.WillByDefault(testing::Return(false));
		ON_CALL(*mockServerConfiguration, getMaxEventsPerSession())
			.WillByDefault(testing::Return(defaultValues->getMaxEventsPerSession()));
		ON_CALL(*mockServerConfiguration, isSessionSplitByEventsEnabled())
			.WillByDefault(testing::Return(false));
		ON_CALL(*mockServerConfiguration, getSessionTimeoutInMilliseconds())
			.WillByDefault(testing::Return(defaultValues->getSessionTimeoutInMilliseconds()));
		ON_CALL(*mockServerConfiguration, isSessionSplitByIdleTimeoutEnabled())
			.WillByDefault(testing::Return(false));
		ON_CALL(*mockServerConfiguration, getVisitStoreVersion())
			.WillByDefault(testing::Return(defaultValues->getVisitStoreVersion()));
		ON_CALL(*mockServerConfiguration, getTrafficControlPercentage())
			.WillByDefault(testing::Return(defaultValues->getTrafficControlPercentage()));
	}
};

TEST_F(ServerConfigurationTest, inDefaultServerConfigurationCapturingIsEnabled)
{
	ASSERT_THAT(ServerConfiguration_t::defaultInstance()->isCaptureEnabled(), testing::Eq(true));
}

TEST_F(ServerConfigurationTest, isDefaultServerConfigurationCrashReportingIsEnabled)
{
	ASSERT_THAT(ServerConfiguration_t::defaultInstance()->isCrashReportingEnabled(), testing::Eq(true));
}

TEST_F(ServerConfigurationTest, isDefaultServerConfigurationErrorReportingIsEnabled)
{
	ASSERT_THAT(ServerConfiguration_t::defaultInstance()->isErrorReportingEnabled(), testing::Eq(true));
}

TEST_F(ServerConfigurationTest, inDefaultServerConfigurationServerIdIsMinusOne)
{
	ASSERT_THAT(ServerConfiguration_t::defaultInstance()->getServerId(), testing::Eq(-1));
}

TEST_F(ServerConfigurationTest, inDefaultServerConfigurationBeaconSizeIsThirtyKb)
{
	ASSERT_THAT(ServerConfiguration_t::defaultInstance()->getBeaconSizeInBytes(), testing::Eq(30 * 1024));
}

TEST_F(ServerConfigurationTest, inDefaultServerConfigurationMultiplicityIsOne)
{
	ASSERT_THAT(ServerConfiguration_t::defaultInstance()->getMultiplicity(), testing::Eq(1));
}

TEST_F(ServerConfigurationTest, inDefaultServerConfigurationSendIntervalIs120Seconds)
{
	ASSERT_THAT(ServerConfiguration_t::defaultInstance()->getSendIntervalInMilliseconds(), testing::Eq(120 * 1000));
}

TEST_F(ServerConfigurationTest, inDefaultServerConfigurationMaxSessionDurationIsMinusOne)
{
	ASSERT_THAT(ServerConfiguration_t::defaultInstance()->getMaxSessionDurationInMilliseconds(), testing::Eq(-1));
}

TEST_F(ServerConfigurationTest, inDefaultServerConfigurationIsSessionSplitBySessionDurationEnabledIsFalse)
{
	ASSERT_THAT(ServerConfiguration_t::defaultInstance()->isSessionSplitBySessionDurationEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, inDefaultServerConfigurationMaxEventsPerSessionIsMinusOne)
{
	ASSERT_THAT(ServerConfiguration_t::defaultInstance()->getMaxEventsPerSession(), testing::Eq(-1));
}

TEST_F(ServerConfigurationTest, inDefaultServerConfigurationIsSessionSplitByEventsEnabledIsFalse)
{
	ASSERT_THAT(ServerConfiguration_t::defaultInstance()->isSessionSplitByEventsEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, inDefaultServerConfigurationSessionTimeoutIsMinusOne)
{
	ASSERT_THAT(ServerConfiguration_t::defaultInstance()->getSessionTimeoutInMilliseconds(), testing::Eq(-1));
}

TEST_F(ServerConfigurationTest, inDefaultServerConfigurationIsSessionSplitByIdleTimeoutEnabledIsFalse)
{
	ASSERT_THAT(ServerConfiguration_t::defaultInstance()->isSessionSplitByIdleTimeoutEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, inDefaultServerConfigurationVisitStoreVersionIsOne)
{
	ASSERT_THAT(ServerConfiguration_t::defaultInstance()->getVisitStoreVersion(), testing::Eq(1));
}

TEST_F(ServerConfigurationTest, inDefaultServerConfigurationTrafficControlPercentageIsOneHundred)
{
	ASSERT_THAT(ServerConfiguration_t::defaultInstance()->getTrafficControlPercentage(), testing::Eq(100));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// creation via 'from' factory tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(ServerConfigurationTest, creatingAServerConfigurationFromNullResponseAttributesGivesNull)
{
	ASSERT_THAT(ServerConfiguration_t::from(nullptr), testing::Eq(nullptr));
}

TEST_F(ServerConfigurationTest, creatingAServerConfigurationFromResponseAttributesCopiesCaptureSettings)
{
	// expect
	EXPECT_CALL(*mockAttributes, isCapture())
		.Times(1)
		.WillOnce(testing::Return(false));

	// when
	auto target = ServerConfiguration_t::from(mockAttributes);

	// then
	ASSERT_THAT(target->isCaptureEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, creatingAServerConfigurationFromResponseAttributesCopiesCrashReportingSettings)
{
	// expect
	EXPECT_CALL(*mockAttributes, isCaptureCrashes())
		.Times(1)
		.WillOnce(testing::Return(false));

	// when
	auto target = ServerConfiguration_t::from(mockAttributes);

	// then
	ASSERT_THAT(target->isCrashReportingEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, creatingAServerConfigurationFromResponseAttributesCopiesErrorReportingSettings)
{
	// mockAttributs
	EXPECT_CALL(*mockAttributes, isCaptureErrors())
		.Times(1)
		.WillOnce(testing::Return(false));

	// when
	auto target = ServerConfiguration_t::from(mockAttributes);

	// then
	ASSERT_THAT(target->isErrorReportingEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, creatingAServerConfigurationFromResponseAttributesCopiesServerIdSettings)
{
	// with
	int32_t serverId = 73;

	// expect
	EXPECT_CALL(*mockAttributes, getServerId())
		.Times(1)
		.WillOnce(testing::Return(serverId));

	// when
	auto target = ServerConfiguration_t::from(mockAttributes);

	// then
	ASSERT_THAT(target->getServerId(), testing::Eq(serverId));
}

TEST_F(ServerConfigurationTest, creatingAServerConfigurationFromResponseAttributesCopiesBeaconSizeSettings)
{
	// with
	int32_t beaconSize = 37;

	// expect
	EXPECT_CALL(*mockAttributes, getMaxBeaconSizeInBytes())
		.Times(1)
		.WillOnce(testing::Return(beaconSize));

	// when
	auto target = ServerConfiguration_t::from(mockAttributes);

	// then
	ASSERT_THAT(target->getBeaconSizeInBytes(), testing::Eq(beaconSize));
}

TEST_F(ServerConfigurationTest, creatingAServerConfigurationFromResponseAttributesCopiesMultiplicitySettings)
{
	// with
	int32_t multiplicity = 37;

	// expect
	EXPECT_CALL(*mockAttributes, getMultiplicity())
		.Times(1)
		.WillOnce(testing::Return(multiplicity));

	// when
	auto target = ServerConfiguration_t::from(mockAttributes);

	// then
	ASSERT_THAT(target->getMultiplicity(), testing::Eq(multiplicity));
}

TEST_F(ServerConfigurationTest, creatingAServerConfigurationFromResponseAttributesCopiesSendInterval)
{
	// with
	const int32_t sendInterval = 1234;

	// expect
	EXPECT_CALL(*mockAttributes, getSendIntervalInMilliseconds())
		.Times(1)
		.WillOnce(testing::Return(sendInterval));

	// when
	auto target = ServerConfiguration_t::from(mockAttributes);

	// then
	ASSERT_THAT(target->getSendIntervalInMilliseconds(), testing::Eq(sendInterval));
}

TEST_F(ServerConfigurationTest, creatingAServerConfigurationFromResponseAttributesCopiesSessionDuration)
{
	// with
	int32_t sessionDuration = 73;

	// expect
	EXPECT_CALL(*mockAttributes, getMaxSessionDurationInMilliseconds())
		.Times(1)
		.WillOnce(testing::Return(sessionDuration));

	// when
	auto target = ServerConfiguration_t::from(mockAttributes);

	// then
	ASSERT_THAT(target->getMaxSessionDurationInMilliseconds(), testing::Eq(sessionDuration));
}

TEST_F(ServerConfigurationTest, creatingAServerConfigurationFromResponseAttributesHasSplitBySessionDurationEnabledIfMaxSessionDurationGreaterZero)
{
	// with
	int32_t sessionDuration = 1;

	// expect
	EXPECT_CALL(*mockAttributes, getMaxSessionDurationInMilliseconds())
		.Times(1)
		.WillOnce(testing::Return(sessionDuration));
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::MAX_SESSION_DURATION))
		.Times(1)
		.WillOnce(testing::Return(true));
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::SESSION_IDLE_TIMEOUT))
		.Times(1);
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::MAX_EVENTS_PER_SESSION))
		.Times(1);

	// when
	auto target = ServerConfiguration_t::from(mockAttributes);

	// then
	ASSERT_THAT(target->isSessionSplitBySessionDurationEnabled(), testing::Eq(true));
}

TEST_F(ServerConfigurationTest, creatingAServerConfigurationStatusResponseHasSplitBySessionDurationDisabledIfMaxDurationZero)
{
	// with
	int32_t sessionDuration = 0;

	// expect
	EXPECT_CALL(*mockAttributes, getMaxSessionDurationInMilliseconds())
		.Times(1)
		.WillOnce(testing::Return(sessionDuration));
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::MAX_SESSION_DURATION))
		.Times(1)
		.WillOnce(testing::Return(true));
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::SESSION_IDLE_TIMEOUT))
		.Times(1);
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::MAX_EVENTS_PER_SESSION))
		.Times(1);

	// when
	auto target = ServerConfiguration_t::from(mockAttributes);

	// then
	ASSERT_THAT(target->isSessionSplitBySessionDurationEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, creatingAServerConfigurationStatusResponseHasSplitBySessionDurationDisabledIfMaxDurationEventsSmallerZero)
{
	// with
	int32_t sessionDuration = -1;

	// expect
	EXPECT_CALL(*mockAttributes, getMaxSessionDurationInMilliseconds())
		.Times(1)
		.WillOnce(testing::Return(sessionDuration));
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::MAX_SESSION_DURATION))
		.Times(1)
		.WillOnce(testing::Return(true));
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::SESSION_IDLE_TIMEOUT))
		.Times(1);
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::MAX_EVENTS_PER_SESSION))
		.Times(1);

	// when
	auto target = ServerConfiguration_t::from(mockAttributes);

	// then
	ASSERT_THAT(target->isSessionSplitBySessionDurationEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, creatingAServerConfigurationStatusResponseHasSplitBySessionDurationDisabledIfMaxDurationIsNotSet)
{
	// with
	int32_t sessionDuration = 1;

	// expect
	EXPECT_CALL(*mockAttributes, getMaxSessionDurationInMilliseconds())
		.Times(1)
		.WillOnce(testing::Return(sessionDuration));
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::MAX_SESSION_DURATION))
		.Times(1)
		.WillOnce(testing::Return(false));
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::SESSION_IDLE_TIMEOUT))
		.Times(1);
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::MAX_EVENTS_PER_SESSION))
		.Times(1);

	// when
	auto target = ServerConfiguration_t::from(mockAttributes);

	// then
	ASSERT_THAT(target->isSessionSplitBySessionDurationEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, creatingAServerConfigurationFromResponseAttributesCopiesMaxEventsPerSession)
{
	// with
	int32_t eventsPerSession = 37;

	// expect
	EXPECT_CALL(*mockAttributes, getMaxEventsPerSession())
		.Times(1)
		.WillOnce(testing::Return(eventsPerSession));

	// when
	auto target = ServerConfiguration_t::from(mockAttributes);

	// then
	ASSERT_THAT(target->getMaxEventsPerSession(), testing::Eq(eventsPerSession));
}

TEST_F(ServerConfigurationTest, creatingAServerConfigurationFromResponseAttributesHasSplitByEventsEnabledIfMaxEventsGreaterZero)
{
	// with
	int32_t eventsPerSession = 1;

	// expect
	EXPECT_CALL(*mockAttributes, getMaxEventsPerSession())
		.Times(1)
		.WillOnce(testing::Return(eventsPerSession));
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::MAX_EVENTS_PER_SESSION))
		.Times(1)
		.WillOnce(testing::Return(true));
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::MAX_SESSION_DURATION))
		.Times(1);
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::SESSION_IDLE_TIMEOUT))
		.Times(1);

	// when
	auto target = ServerConfiguration_t::from(mockAttributes);

	// then
	ASSERT_THAT(target->isSessionSplitByEventsEnabled(), testing::Eq(true));
}

TEST_F(ServerConfigurationTest, creatingAServerConfigurationFromResponseAttributesHasSplitByEventsDisabledIfMaxEventsZero)
{
	// with
	int32_t eventsPerSession = 0;

	// expect
	EXPECT_CALL(*mockAttributes, getMaxEventsPerSession())
		.Times(1)
		.WillOnce(testing::Return(eventsPerSession));
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::MAX_EVENTS_PER_SESSION))
		.Times(1)
		.WillOnce(testing::Return(true));
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::MAX_SESSION_DURATION))
		.Times(1);
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::SESSION_IDLE_TIMEOUT))
		.Times(1);

	// when
	auto target = ServerConfiguration_t::from(mockAttributes);

	// then
	ASSERT_THAT(target->isSessionSplitByEventsEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, creatingAServerConfigurationFromResponseAttributesHasSplitByEventsDisabledIfMaxEventsEventsSmallerZero)
{
	// with
	int32_t eventsPerSession = -1;

	// expect
	EXPECT_CALL(*mockAttributes, getMaxEventsPerSession())
		.Times(1)
		.WillOnce(testing::Return(eventsPerSession));
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::MAX_EVENTS_PER_SESSION))
		.WillOnce(testing::Return(true));
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::MAX_SESSION_DURATION))
		.Times(1);
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::SESSION_IDLE_TIMEOUT))
		.Times(1);

	// when
	auto target = ServerConfiguration_t::from(mockAttributes);

	// then
	ASSERT_THAT(target->isSessionSplitByEventsEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, creatingAServerConfigurationFromResponseAttributesHasSplitByEventsDisabledIfMaxEventsIsNotSet)
{
	// with
	int32_t eventsPerSession = 1;

	// expect
	EXPECT_CALL(*mockAttributes, getMaxEventsPerSession())
		.Times(1)
		.WillOnce(testing::Return(eventsPerSession));
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::MAX_EVENTS_PER_SESSION))
		.Times(1)
		.WillOnce(testing::Return(false));
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::MAX_SESSION_DURATION))
		.Times(1);
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::SESSION_IDLE_TIMEOUT))
		.Times(1);

	// when
	auto target = ServerConfiguration_t::from(mockAttributes);

	// then
	ASSERT_THAT(target->isSessionSplitByEventsEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, creatingAServerConfigurationFromResponseAttributesCopiesSessionTimeout)
{
	// with
	int32_t sessionTimeout = 42;

	// expect
	EXPECT_CALL(*mockAttributes, getSessionTimeoutInMilliseconds())
		.Times(1)
		.WillOnce(testing::Return(sessionTimeout));

	// when
	auto target = ServerConfiguration_t::from(mockAttributes);

	// then
	ASSERT_THAT(target->getSessionTimeoutInMilliseconds(), testing::Eq(sessionTimeout));
}

TEST_F(ServerConfigurationTest, creatingAServerConfigurationFromResponseAttributesHasSplitByIdleTimeoutEnabledIfTimeoutGreaterZero)
{
	// with
	int32_t sessionTimeout = 1;

	// expect
	EXPECT_CALL(*mockAttributes, getSessionTimeoutInMilliseconds())
		.Times(1)
		.WillOnce(testing::Return(sessionTimeout));
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::SESSION_IDLE_TIMEOUT))
		.Times(1)
		.WillOnce(testing::Return(true));
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::MAX_SESSION_DURATION))
		.Times(1);
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::MAX_EVENTS_PER_SESSION))
		.Times(1);

	// when
	auto target = ServerConfiguration_t::from(mockAttributes);

	// then
	ASSERT_THAT(target->isSessionSplitByIdleTimeoutEnabled(), testing::Eq(true));
}

TEST_F(ServerConfigurationTest, creatingAServerConfigurationStatusResponseHasSplitByIdleTimeoutDisabledIfTimeoutZero)
{
	// with
	int32_t sessionTimeout = 0;

	// expect
	EXPECT_CALL(*mockAttributes, getSessionTimeoutInMilliseconds())
		.Times(1)
		.WillOnce(testing::Return(sessionTimeout));
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::SESSION_IDLE_TIMEOUT))
		.Times(1)
		.WillOnce(testing::Return(true));
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::MAX_SESSION_DURATION))
		.Times(1);
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::MAX_EVENTS_PER_SESSION))
		.Times(1);

	// when
	auto target = ServerConfiguration_t::from(mockAttributes);

	// then
	ASSERT_THAT(target->isSessionSplitByIdleTimeoutEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, creatingAServerConfigurationStatusResponseHasSplitByIdleTimeoutDisabledIfTimeoutSmallerZero)
{
	// with
	int32_t sessionTimeout = -1;

	// expect
	EXPECT_CALL(*mockAttributes, getSessionTimeoutInMilliseconds())
		.Times(1)
		.WillOnce(testing::Return(sessionTimeout));
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::SESSION_IDLE_TIMEOUT))
		.Times(1)
		.WillOnce(testing::Return(true));
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::MAX_SESSION_DURATION))
		.Times(1);
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::MAX_EVENTS_PER_SESSION))
		.Times(1);

	// when
	auto target = ServerConfiguration_t::from(mockAttributes);

	// then
	ASSERT_THAT(target->isSessionSplitByIdleTimeoutEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, creatingAServerConfigurationStatusResponseHasSplitByIdleTimeoutDisabledIfTimeoutIsNotSet)
{
	// with
	int32_t sessionTimeout = 1;

	// expect
	EXPECT_CALL(*mockAttributes, getSessionTimeoutInMilliseconds())
		.Times(1)
		.WillOnce(testing::Return(sessionTimeout));
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::SESSION_IDLE_TIMEOUT))
		.Times(1)
		.WillOnce(testing::Return(false));
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::MAX_SESSION_DURATION))
		.Times(1);
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::MAX_EVENTS_PER_SESSION))
		.Times(1);

	// when
	auto target = ServerConfiguration_t::from(mockAttributes);

	// then
	ASSERT_THAT(target->isSessionSplitByIdleTimeoutEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, creatingAServerConfigurationFromResponseAttributesCopiesVisitStoreVersion)
{
	// with
	int32_t visitStoreVersion = 73;

	// expect
	EXPECT_CALL(*mockAttributes, getVisitStoreVersion())
		.Times(1)
		.WillOnce(testing::Return(visitStoreVersion));

	// when
	auto target = ServerConfiguration_t::from(mockAttributes);

	// then
	ASSERT_THAT(target->getVisitStoreVersion(), testing::Eq(visitStoreVersion));
}

TEST_F(ServerConfigurationTest, creatingAServerConfigurationFromResponseAttributesCopiesTrafficControlPercentage)
{
	// with
	const auto trafficControlPercentage = 42;

	// expect
	EXPECT_CALL(*mockAttributes, getTrafficControlPercentage())
		.Times(1)
		.WillOnce(testing::Return(trafficControlPercentage));

	// when
	auto target = ServerConfiguration_t::from(mockAttributes);

	// then
	ASSERT_THAT(target->getTrafficControlPercentage(), testing::Eq(trafficControlPercentage));
}

TEST_F(ServerConfigurationTest, sendingDataToTheServerIsAllowedIfCapturingIsEnabledAndMultiplicityIsGreaterThanZero)
{
	// with
	ON_CALL(*mockAttributes, isCapture()).WillByDefault(testing::Return(true));
	ON_CALL(*mockAttributes, getMultiplicity()).WillByDefault(testing::Return(1));

	// given
	auto target = ServerConfiguration_t::from(mockAttributes);

	// when
	auto obtained = target->isSendingDataAllowed();

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
}

TEST_F(ServerConfigurationTest, sendingDataToTheServerIsNotAllowedIfCapturingIsDisallowed)
{
	// with
	ON_CALL(*mockAttributes, isCapture()).WillByDefault(testing::Return(false));
	ON_CALL(*mockAttributes, getMultiplicity()).WillByDefault(testing::Return(1));

	// given
	auto target = ServerConfiguration_t::from(mockAttributes);

	// when
	auto obtained = target->isSendingDataAllowed();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(ServerConfigurationTest, sendingDataToTheServerIsNotAllowedIfCapturingIsEnabledButMultiplicityIsZero)
{
	// with
	ON_CALL(*mockAttributes, isCapture()).WillByDefault(testing::Return(true));
	ON_CALL(*mockAttributes, getMultiplicity()).WillByDefault(testing::Return(0));

	// given
	auto target = ServerConfiguration_t::from(mockAttributes);

	// when
	auto obtained = target->isSendingDataAllowed();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(ServerConfigurationTest, sendingCrashesToTheServerIsAllowedIfDataSendingIsAllowedAndCaptureCrashesIsEnabled)
{
	// with
	ON_CALL(*mockAttributes, isCapture()).WillByDefault(testing::Return(true));
	ON_CALL(*mockAttributes, getMultiplicity()).WillByDefault(testing::Return(1));
	ON_CALL(*mockAttributes, isCaptureCrashes()).WillByDefault(testing::Return(true));

	// given
	auto target = ServerConfiguration_t::from(mockAttributes);

	// when
	auto obtained = target->isSendingCrashesAllowed();

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
}

TEST_F(ServerConfigurationTest, sendingCrashesToTheServerIsNotAllowedIfDataSendingIsNotAllowed)
{
	// with
	ON_CALL(*mockAttributes, isCapture()).WillByDefault(testing::Return(false));
	ON_CALL(*mockAttributes, getMultiplicity()).WillByDefault(testing::Return(1));
	ON_CALL(*mockAttributes, isCaptureCrashes()).WillByDefault(testing::Return(true));

	// given
	auto target = ServerConfiguration_t::from(mockAttributes);

	// when
	auto obtained = target->isSendingCrashesAllowed();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(ServerConfigurationTest, sendingCrashesToTheServerIsNotAllowedIfDataSendingIsAllowedButCaptureCrashesIsDisabled)
{
	// with
	ON_CALL(*mockAttributes, isCapture()).WillByDefault(testing::Return(true));
	ON_CALL(*mockAttributes, getMultiplicity()).WillByDefault(testing::Return(1));
	ON_CALL(*mockAttributes, isCaptureCrashes()).WillByDefault(testing::Return(false));

	// given
	auto target = ServerConfiguration_t::from(mockAttributes);

	// when
	auto obtained = target->isSendingCrashesAllowed();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(ServerConfigurationTest, sendingErrorToTheServerIsAllowedIfDataSendingIsAllowedAndCaptureErrorIsEnabled)
{
	// with
	ON_CALL(*mockAttributes, isCapture()).WillByDefault(testing::Return(true));
	ON_CALL(*mockAttributes, getMultiplicity()).WillByDefault(testing::Return(1));
	ON_CALL(*mockAttributes, isCaptureErrors()).WillByDefault(testing::Return(true));

	// given
	auto target = ServerConfiguration_t::from(mockAttributes);

	// when
	auto obtained = target->isSendingErrorsAllowed();

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
}

TEST_F(ServerConfigurationTest, sendingErrorToTheServerIsNotAllowedIfDataSendingIsNotAllowed)
{
	// with
	ON_CALL(*mockAttributes, isCapture()).WillByDefault(testing::Return(false));
	ON_CALL(*mockAttributes, getMultiplicity()).WillByDefault(testing::Return(1));
	ON_CALL(*mockAttributes, isCaptureErrors()).WillByDefault(testing::Return(true));

	// given
	auto target = ServerConfiguration_t::from(mockAttributes);

	// when
	auto obtained = target->isSendingErrorsAllowed();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(ServerConfigurationTest, sendingErrorToTheServerIsNotAllowedIfDataSendingIsAllowedButCaptureErrorsDisabled)
{
	// with
	ON_CALL(*mockAttributes, isCapture()).WillByDefault(testing::Return(true));
	ON_CALL(*mockAttributes, getMultiplicity()).WillByDefault(testing::Return(1));
	ON_CALL(*mockAttributes, isCaptureErrors()).WillByDefault(testing::Return(false));

	// given
	auto target = ServerConfiguration_t::from(mockAttributes);

	// when
	auto obtained = target->isSendingErrorsAllowed();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// creating builder from server config tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TEST_F(ServerConfigurationTest, builderFromServerConfigCopiesCaptureSettings)
{
	// expect
	EXPECT_CALL(*mockServerConfiguration, isCaptureEnabled())
		.Times(1)
		.WillOnce(testing::Return(false));

	// given
	auto target = ServerConfiguration_t::Builder(mockServerConfiguration).build();

	// then
	ASSERT_THAT(target->isCaptureEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, builderFromServerConfigCopiesCrashReportingSettings)
{
	// expect
	EXPECT_CALL(*mockServerConfiguration, isCrashReportingEnabled())
		.Times(1)
		.WillOnce(testing::Return(false));

	// given
	auto target = ServerConfiguration_t::Builder(mockServerConfiguration).build();

	// then
	ASSERT_THAT(target->isCrashReportingEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, builderFromServerConfigCopiesErrorReportingSettings)
{
	// expect
	EXPECT_CALL(*mockServerConfiguration, isErrorReportingEnabled())
		.Times(1)
		.WillOnce(testing::Return(false));

	// given
	auto target = ServerConfiguration_t::Builder(mockServerConfiguration).build();

	// then
	ASSERT_THAT(target->isErrorReportingEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, builderFromServerConfigCopiesServerIdSettings)
{
	// expect
	EXPECT_CALL(*mockServerConfiguration, getServerId())
		.Times(1)
		.WillOnce(testing::Return(42));

	// given
	auto target = ServerConfiguration_t::Builder(mockServerConfiguration).build();

	// then
	ASSERT_THAT(target->getServerId(), testing::Eq(42));
}

TEST_F(ServerConfigurationTest, builderFromServerConfigCopiesBeaconSizeSettings)
{
	// expect
	EXPECT_CALL(*mockServerConfiguration, getBeaconSizeInBytes())
		.Times(1)
		.WillOnce(testing::Return(100 * 1024));

	// given
	auto target = ServerConfiguration_t::Builder(mockServerConfiguration).build();

	// then
	ASSERT_THAT(target->getBeaconSizeInBytes(), testing::Eq(100 * 1024));
}

TEST_F(ServerConfigurationTest, builderFromServerConfigCopiesMultiplicitySettings)
{
	// expect
	EXPECT_CALL(*mockServerConfiguration, getMultiplicity())
		.Times(1)
		.WillOnce(testing::Return(7));

	// given
	auto target = ServerConfiguration_t::Builder(mockServerConfiguration).build();

	// then
	ASSERT_THAT(target->getMultiplicity(), testing::Eq(7));
}

TEST_F(ServerConfigurationTest, builderFromServerConfigCopiesSendInterval)
{
	// with
	const int32_t sendInterval = 4321;

	// expect
	EXPECT_CALL(*mockServerConfiguration, getSendIntervalInMilliseconds())
		.Times(1)
		.WillOnce(testing::Return(sendInterval));

	// with
	auto target = ServerConfiguration_t::Builder(mockServerConfiguration).build();

	// then
	ASSERT_THAT(target->getSendIntervalInMilliseconds(), testing::Eq(sendInterval));
}

TEST_F(ServerConfigurationTest, builderFromServerConfigCopiesSessionDuration)
{
	// expect
	EXPECT_CALL(*mockServerConfiguration, getMaxSessionDurationInMilliseconds())
		.Times(1)
		.WillOnce(testing::Return(73));

	// given
	auto target = ServerConfiguration_t::Builder(mockServerConfiguration).build();

	// then
	ASSERT_THAT(target->getMaxSessionDurationInMilliseconds(), testing::Eq(73));
}

TEST_F(ServerConfigurationTest, builderFromServerConfigHasSplitBySessionDurationEnabledIfMaxEventsGreaterZero)
{
	// with
	const int32_t sessionDuration = 1;

	// expect
	EXPECT_CALL(*mockServerConfiguration, getMaxSessionDurationInMilliseconds())
		.Times(1)
		.WillOnce(testing::Return(sessionDuration));
	EXPECT_CALL(*mockServerConfiguration, isSessionSplitBySessionDurationEnabled())
		.Times(1)
		.WillOnce(testing::Return(true));

	// given
	auto target = ServerConfiguration_t::Builder(mockServerConfiguration).build();

	// then
	ASSERT_THAT(target->isSessionSplitBySessionDurationEnabled(), testing::Eq(true));
}

TEST_F(ServerConfigurationTest, builderFromServerConfigHasSplitBySessionDurationDisabledIfMaxEventsZero)
{
	// with
	const int32_t sessionDuration = 0;

	// expect
	EXPECT_CALL(*mockServerConfiguration, getMaxSessionDurationInMilliseconds())
		.Times(1)
		.WillOnce(testing::Return(sessionDuration));
	EXPECT_CALL(*mockServerConfiguration, isSessionSplitBySessionDurationEnabled())
		.Times(1)
		.WillOnce(testing::Return(true));

	// given
	auto target = ServerConfiguration_t::Builder(mockServerConfiguration).build();

	// then
	ASSERT_THAT(target->isSessionSplitBySessionDurationEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, builderFromServerConfigHasSplitBySessionDurationDisabledIfMaxEventsEventsSmallerZero)
{
	// with
	const int32_t sessionDuration = -1;

	// expect
	EXPECT_CALL(*mockServerConfiguration, getMaxSessionDurationInMilliseconds())
		.Times(1)
		.WillOnce(testing::Return(sessionDuration));
	EXPECT_CALL(*mockServerConfiguration, isSessionSplitBySessionDurationEnabled())
		.Times(1)
		.WillOnce(testing::Return(true));

	// given
	auto target = ServerConfiguration_t::Builder(mockServerConfiguration).build();

	// then
	ASSERT_THAT(target->isSessionSplitBySessionDurationEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, builderFromServerConfigHasSplitBySessionDurationDisabledIfMaxEventsIsNotSet)
{
	// with
	const int32_t sessionDuration = 1;

	// expect
	EXPECT_CALL(*mockServerConfiguration, getMaxSessionDurationInMilliseconds())
		.Times(1)
		.WillOnce(testing::Return(sessionDuration));
	EXPECT_CALL(*mockServerConfiguration, isSessionSplitBySessionDurationEnabled())
		.Times(1)
		.WillOnce(testing::Return(false));

	// given
	auto target = ServerConfiguration_t::Builder(mockServerConfiguration).build();

	// then
	ASSERT_THAT(target->isSessionSplitBySessionDurationEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, builderFromServerConfigCopiesMaxEventsPerSession)
{
	// expect
	EXPECT_CALL(*mockServerConfiguration, getMaxEventsPerSession())
		.Times(1)
		.WillOnce(testing::Return(37));

	// given
	auto target = ServerConfiguration_t::Builder(mockServerConfiguration).build();

	// then
	ASSERT_THAT(target->getMaxEventsPerSession(), testing::Eq(37));
}

TEST_F(ServerConfigurationTest, builderFromServerConfigHasSplitByEventsEnabledIfMaxEventsGreaterZero)
{
	// with
	int32_t eventsPerSession = 1;

	// expect
	EXPECT_CALL(*mockServerConfiguration, getMaxEventsPerSession())
		.Times(1)
		.WillOnce(testing::Return(eventsPerSession));
	EXPECT_CALL(*mockServerConfiguration, isSessionSplitByEventsEnabled())
		.Times(1)
		.WillOnce(testing::Return(true));

	// given
	auto target = ServerConfiguration_t::Builder(mockServerConfiguration).build();

	// then
	ASSERT_THAT(target->isSessionSplitByEventsEnabled(), testing::Eq(true));
}

TEST_F(ServerConfigurationTest, builderFromServerConfigHasSplitByEventsDisabledIfMaxEventsZero)
{
	// with
	int32_t eventsPerSession = 0;

	// expect
	EXPECT_CALL(*mockServerConfiguration, getMaxEventsPerSession())
		.Times(1)
		.WillOnce(testing::Return(eventsPerSession));
	EXPECT_CALL(*mockServerConfiguration, isSessionSplitByEventsEnabled())
		.Times(1)
		.WillOnce(testing::Return(true));

	// given
	auto target = ServerConfiguration_t::Builder(mockServerConfiguration).build();

	// then
	ASSERT_THAT(target->isSessionSplitByEventsEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, builderFromServerConfigHasSplitByEventsDisabledIfMaxEventsSmallerZero)
{
	// with
	int32_t eventsPerSession = -1;

	// expect
	EXPECT_CALL(*mockServerConfiguration, getMaxEventsPerSession())
		.Times(1)
		.WillOnce(testing::Return(eventsPerSession));
	EXPECT_CALL(*mockServerConfiguration, isSessionSplitByEventsEnabled())
		.Times(1)
		.WillOnce(testing::Return(true));

	// given
	auto target = ServerConfiguration_t::Builder(mockServerConfiguration).build();

	// then
	ASSERT_THAT(target->isSessionSplitByEventsEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, builderFromServerConfigHasSplitByEventsDisabledIfMaxEventsIsNotSet)
{
	// with
	int32_t eventsPerSession = 1;

	// expect
	EXPECT_CALL(*mockServerConfiguration, getMaxEventsPerSession())
		.Times(1)
		.WillOnce(testing::Return(eventsPerSession));
	EXPECT_CALL(*mockServerConfiguration, isSessionSplitByEventsEnabled())
		.Times(1)
		.WillOnce(testing::Return(false));

	// given
	auto target = ServerConfiguration_t::Builder(mockServerConfiguration).build();

	// then
	ASSERT_THAT(target->isSessionSplitByEventsEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, builderFromServerConfigCopiesSessionTimeout)
{
	// expect
	EXPECT_CALL(*mockServerConfiguration, getSessionTimeoutInMilliseconds())
		.Times(1)
		.WillOnce(testing::Return(42));

	// given
	auto target = ServerConfiguration_t::Builder(mockServerConfiguration).build();

	// then
	ASSERT_THAT(target->getSessionTimeoutInMilliseconds(), testing::Eq(42));
}

TEST_F(ServerConfigurationTest, builderFromServerConfigHasSplitByIdleTimeoutEnabledIfMaxEventsGreaterZero)
{
	// with
	const int32_t idleTimeout = 1;

	// expect
	EXPECT_CALL(*mockServerConfiguration, getSessionTimeoutInMilliseconds())
		.Times(1)
		.WillOnce(testing::Return(idleTimeout));
	EXPECT_CALL(*mockServerConfiguration, isSessionSplitByIdleTimeoutEnabled())
		.Times(1)
		.WillOnce(testing::Return(true));

	// given
	auto target = ServerConfiguration_t::Builder(mockServerConfiguration).build();

	// then
	ASSERT_THAT(target->isSessionSplitByIdleTimeoutEnabled(), testing::Eq(true));
}

TEST_F(ServerConfigurationTest, builderFromServerConfigHasSplitByIdleTimeoutDisabledIfMaxEventsZero)
{
	// with
	const int32_t idleTimeout = 0;

	// expect
	EXPECT_CALL(*mockServerConfiguration, getSessionTimeoutInMilliseconds())
		.Times(1)
		.WillOnce(testing::Return(idleTimeout));
	EXPECT_CALL(*mockServerConfiguration, isSessionSplitByIdleTimeoutEnabled())
		.Times(1)
		.WillOnce(testing::Return(true));

	// given
	auto target = ServerConfiguration_t::Builder(mockServerConfiguration).build();

	// then
	ASSERT_THAT(target->isSessionSplitByIdleTimeoutEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, builderFromServerConfigHasSplitByIdleTimeoutDisabledIfMaxEventsEventsSmallerZero)
{
	// with
	const int32_t idleTimeout = -1;

	// expect
	EXPECT_CALL(*mockServerConfiguration, getSessionTimeoutInMilliseconds())
		.Times(1)
		.WillOnce(testing::Return(idleTimeout));
	EXPECT_CALL(*mockServerConfiguration, isSessionSplitByIdleTimeoutEnabled())
		.Times(1)
		.WillOnce(testing::Return(true));

	// given
	auto target = ServerConfiguration_t::Builder(mockServerConfiguration).build();

	// then
	ASSERT_THAT(target->isSessionSplitByIdleTimeoutEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, builderFromServerConfigHasSplitByIdleTimeoutDisabledIfMaxEventsIsNotSet)
{
	// with
	const int32_t idleTimeout = 1;

	// expect
	EXPECT_CALL(*mockServerConfiguration, getSessionTimeoutInMilliseconds())
		.Times(1)
		.WillOnce(testing::Return(idleTimeout));
	EXPECT_CALL(*mockServerConfiguration, isSessionSplitByIdleTimeoutEnabled())
		.Times(1)
		.WillOnce(testing::Return(false));

	// given
	auto target = ServerConfiguration_t::Builder(mockServerConfiguration).build();

	// then
	ASSERT_THAT(target->isSessionSplitByIdleTimeoutEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, builderFromServerConfigCopiesVisitStoreVersion)
{
	// expect
	EXPECT_CALL(*mockServerConfiguration, getVisitStoreVersion())
		.Times(1)
		.WillOnce(testing::Return(73));

	// given
	auto target = ServerConfiguration_t::Builder(mockServerConfiguration).build();

	// then
	ASSERT_THAT(target->getVisitStoreVersion(), testing::Eq(73));
}

TEST_F(ServerConfigurationTest, builderFromServerConfigCopiesTrafficControlPercentage)
{
	// with
	const auto trafficControlPercentage = 37;

	// expect
	EXPECT_CALL(*mockServerConfiguration, getTrafficControlPercentage())
		.Times(1)
		.WillOnce(testing::Return(trafficControlPercentage));

	// given
	auto target = ServerConfiguration_t::Builder(mockServerConfiguration).build();

	// then
	ASSERT_THAT(target->getTrafficControlPercentage(), testing::Eq(trafficControlPercentage));
}

TEST_F(ServerConfigurationTest, builderFromServerConfigSendingDataToTheServerIsAllowedIfCapturingIsEnabledAndMultiplicityIsGreaterThanZero)
{
	// expect
	EXPECT_CALL(*mockServerConfiguration, isCaptureEnabled())
		.Times(1)
		.WillOnce(testing::Return(true));
	EXPECT_CALL(*mockServerConfiguration, getMultiplicity())
		.Times(1)
		.WillOnce(testing::Return(1));

	// given
	auto target = ServerConfiguration_t::Builder(mockServerConfiguration).build();

	// then
	ASSERT_THAT(target->isSendingDataAllowed(), testing::Eq(true));
}

TEST_F(ServerConfigurationTest, builderFromServerConfigSendingDataToTheServerIsNotAllowedIfCapturingIsDisabled)
{
	// expect
	EXPECT_CALL(*mockServerConfiguration, isCaptureEnabled())
		.Times(1)
		.WillOnce(testing::Return(false));
	EXPECT_CALL(*mockServerConfiguration, getMultiplicity())
		.Times(1)
		.WillOnce(testing::Return(1));

	// given
	auto target = ServerConfiguration_t::Builder(mockServerConfiguration).build();

	// then
	ASSERT_THAT(target->isSendingDataAllowed(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, builderFromServerConfigSendingDataToTheServerIsNotAllowedIfCapturingIsEnabledButMultiplicityIsZero)
{
	// expect
	EXPECT_CALL(*mockServerConfiguration, isCaptureEnabled())
		.Times(1)
		.WillOnce(testing::Return(true));
	EXPECT_CALL(*mockServerConfiguration, getMultiplicity())
		.Times(1)
		.WillOnce(testing::Return(0));

	// given
	auto target = ServerConfiguration_t::Builder(mockServerConfiguration).build();

	// then
	ASSERT_THAT(target->isSendingDataAllowed(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, builderFromServerConfigSendingCrashesToTheServerIsAllowedIfDataSendingIsAllowedAndCaptureCrashesIsEnabled)
{
	// expect
	EXPECT_CALL(*mockServerConfiguration, isCaptureEnabled())
		.Times(1)
		.WillOnce(testing::Return(true));
	EXPECT_CALL(*mockServerConfiguration, getMultiplicity())
		.Times(1)
		.WillOnce(testing::Return(1));
	EXPECT_CALL(*mockServerConfiguration, isCrashReportingEnabled())
		.Times(1)
		.WillOnce(testing::Return(true));

	// given
	auto target = ServerConfiguration_t::Builder(mockServerConfiguration).build();

	// then
	ASSERT_THAT(target->isSendingCrashesAllowed(), testing::Eq(true));
}

TEST_F(ServerConfigurationTest, builderFromServerConfigSendingCrashesToTheServerIsNotAllowedIfDataSendingIsNotAllowed)
{
	// expect
	EXPECT_CALL(*mockServerConfiguration, isCaptureEnabled())
		.Times(1)
		.WillOnce(testing::Return(false));
	EXPECT_CALL(*mockServerConfiguration, getMultiplicity())
		.Times(1)
		.WillOnce(testing::Return(1));
	EXPECT_CALL(*mockServerConfiguration, isCrashReportingEnabled())
		.Times(1)
		.WillOnce(testing::Return(true));

	// given
	auto target = ServerConfiguration_t::Builder(mockServerConfiguration).build();

	// then
	ASSERT_THAT(target->isSendingCrashesAllowed(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, builderFromServerConfigSendingCrashesToTheServerIsNotAllowedIfDataSendingIsAllowedButCaptureCrashesIsDisabled)
{
	// expect
	EXPECT_CALL(*mockServerConfiguration, isCaptureEnabled())
		.Times(1)
		.WillOnce(testing::Return(true));
	EXPECT_CALL(*mockServerConfiguration, getMultiplicity())
		.Times(1)
		.WillOnce(testing::Return(1));
	EXPECT_CALL(*mockServerConfiguration, isCrashReportingEnabled())
		.Times(1)
		.WillOnce(testing::Return(false));

	// given
	auto target = ServerConfiguration_t::Builder(mockServerConfiguration).build();

	// then
	ASSERT_THAT(target->isSendingCrashesAllowed(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, builderFromServerConfigSendingErrorToTheServerIsAllowedIfDataSendingIsAllowedAndCaptureErrorIsEnabled)
{
	// expect
	EXPECT_CALL(*mockServerConfiguration, isCaptureEnabled())
		.Times(1)
		.WillOnce(testing::Return(true));
	EXPECT_CALL(*mockServerConfiguration, getMultiplicity())
		.Times(1)
		.WillOnce(testing::Return(1));
	EXPECT_CALL(*mockServerConfiguration, isErrorReportingEnabled())
		.Times(1)
		.WillOnce(testing::Return(true));

	// given
	auto target = ServerConfiguration_t::Builder(mockServerConfiguration).build();

	// then
	ASSERT_THAT(target->isSendingErrorsAllowed(), testing::Eq(true));
}

TEST_F(ServerConfigurationTest, builderFromServerConfigSendingErrorToTheServerIsNotAllowedIfDataSendingIsNotAllowed)
{
	// expect
	EXPECT_CALL(*mockServerConfiguration, isCaptureEnabled())
		.Times(1)
		.WillOnce(testing::Return(false));
	EXPECT_CALL(*mockServerConfiguration, getMultiplicity())
		.Times(1)
		.WillOnce(testing::Return(1));
	EXPECT_CALL(*mockServerConfiguration, isErrorReportingEnabled())
		.Times(1)
		.WillOnce(testing::Return(true));

	// given
	auto target = ServerConfiguration_t::Builder(mockServerConfiguration).build();

	// then
	ASSERT_THAT(target->isSendingErrorsAllowed(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, BuilderFromServerConfigSendingErrorsToTheServerIsNotAllowedIfDataSendingIsAllowedButCaptureErrorsDisabled)
{
	// expect
	EXPECT_CALL(*mockServerConfiguration, isCaptureEnabled())
		.Times(1)
		.WillOnce(testing::Return(true));
	EXPECT_CALL(*mockServerConfiguration, getMultiplicity())
		.Times(1)
		.WillOnce(testing::Return(1));
	EXPECT_CALL(*mockServerConfiguration, isErrorReportingEnabled())
		.Times(1)
		.WillOnce(testing::Return(false));

	// given
	auto target = ServerConfiguration_t::Builder(mockServerConfiguration).build();

	// then
	ASSERT_THAT(target->isSendingErrorsAllowed(), testing::Eq(false));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// merge server configuration tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(ServerConfigurationTest, mergeTakesOverEnabledCapture)
{
	// given
	auto target = ServerConfiguration_t::Builder(defaultValues).withCapture(false).build();
	auto other = ServerConfiguration_t::Builder(defaultValues).withCapture(true).build();

	// when
	auto obtained = target->merge(other);

	// then
	ASSERT_THAT(obtained->isCaptureEnabled(), testing::Eq(true));
}

TEST_F(ServerConfigurationTest, mergeTakesOverDisabledCapture)
{
	// given
	auto target = ServerConfiguration_t::Builder(defaultValues).withCapture(true).build();
	auto other = ServerConfiguration_t::Builder(defaultValues).withCapture(false).build();

	// when
	auto obtained = target->merge(other);

	// then
	ASSERT_THAT(obtained->isCaptureEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, mergeTakesOverEnabledCrashReporting)
{
	// given
	auto target = ServerConfiguration_t::Builder(defaultValues).withCrashReporting(false).build();
	auto other = ServerConfiguration_t::Builder(defaultValues).withCrashReporting(true).build();

	// when
	auto obtained = target->merge(other);

	// then
	ASSERT_THAT(obtained->isCrashReportingEnabled(), testing::Eq(true));
}

TEST_F(ServerConfigurationTest, mergeTakesOverDisabledCrashReporting)
{
	// given
	auto target = ServerConfiguration_t::Builder(defaultValues).withCrashReporting(true).build();
	auto other = ServerConfiguration_t::Builder(defaultValues).withCrashReporting(false).build();

	// when
	auto obtained = target->merge(other);

	// then
	ASSERT_THAT(obtained->isCrashReportingEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, mergeTakesOverEnabledErrorReporting)
{
	// given
	auto target = ServerConfiguration_t::Builder(defaultValues).withErrorReporting(false).build();
	auto other = ServerConfiguration_t::Builder(defaultValues).withErrorReporting(true).build();

	// when
	auto obtained = target->merge(other);

	// then
	ASSERT_THAT(obtained->isErrorReportingEnabled(), testing::Eq(true));
}

TEST_F(ServerConfigurationTest, mergeTakesOverDisabledErrorReporting)
{
	// given
	auto target = ServerConfiguration_t::Builder(defaultValues).withErrorReporting(true).build();
	auto other = ServerConfiguration_t::Builder(defaultValues).withErrorReporting(false).build();

	// when
	auto obtained = target->merge(other);

	// then
	ASSERT_THAT(obtained->isErrorReportingEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, mergeTakesOverBeaconSize)
{
	// given
	int32_t beaconSize = 73;
	auto target = ServerConfiguration_t::Builder(defaultValues).withBeaconSizeInBytes(37).build();
	auto other = ServerConfiguration_t::Builder(defaultValues).withBeaconSizeInBytes(beaconSize).build();

	// when
	auto obtained = target->merge(other);

	// then
	ASSERT_THAT(obtained->getBeaconSizeInBytes(), testing::Eq(beaconSize));
}

TEST_F(ServerConfigurationTest, mergeIgnoresMultiplicity)
{
	// given
	int32_t multiplicity = 73;
	auto target = ServerConfiguration_t::Builder(defaultValues).withMultiplicity(multiplicity).build();
	auto other = ServerConfiguration_t::Builder(defaultValues).withMultiplicity(37).build();

	// when
	auto obtained = target->merge(other);

	// then
	ASSERT_THAT(obtained->getMultiplicity(), testing::Eq(multiplicity));
}

TEST_F(ServerConfigurationTest, mergeIgnoresServerId)
{
	// given
	int32_t serverId = 73;
	auto target = ServerConfiguration_t::Builder(defaultValues).withServerId(serverId).build();
	auto other = ServerConfiguration_t::Builder(defaultValues).withServerId(37).build();

	// when
	auto obtained = target->merge(other);

	// then
	ASSERT_THAT(obtained->getServerId(), testing::Eq(serverId));
}

TEST_F(ServerConfigurationTest, mergeKeepsOriginalMaxSessionDuration)
{
	// given
	int32_t sessionDuration = 73;
	auto target = ServerConfiguration_t::Builder(defaultValues)
		.withMaxSessionDurationInMilliseconds(sessionDuration)
		.build();
	auto other = ServerConfiguration_t::Builder(defaultValues)
		.withMaxSessionDurationInMilliseconds(1234)
		.build();

	// when
	auto obtained = target->merge(other);

	// then
	ASSERT_THAT(obtained->getMaxSessionDurationInMilliseconds(), testing::Eq(sessionDuration));
}

TEST_F(ServerConfigurationTest, mergeKeepsIsSessionSplitBySessionDurationEnabledWhenMaxEventsIsGreaterZeroAndAttributeIsSet)
{
	// given
	const int32_t sessionDuration = 73;

	ON_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::MAX_SESSION_DURATION))
		.WillByDefault(testing::Return(true));
	ON_CALL(*mockAttributes, getMaxSessionDurationInMilliseconds())
		.WillByDefault(testing::Return(sessionDuration));

	auto target = ServerConfiguration_t::from(mockAttributes);
	auto other = ServerConfiguration_t::Builder(defaultValues).build();
	
	ASSERT_THAT(other->isSessionSplitBySessionDurationEnabled(), testing::Eq(false));
	ASSERT_THAT(target->isSessionSplitBySessionDurationEnabled(), testing::Eq(true));

	// when
	auto obtained = target->merge(other);

	// then
	ASSERT_THAT(obtained->isSessionSplitBySessionDurationEnabled(), testing::Eq(true));
}

TEST_F(ServerConfigurationTest, mergeKeepsIsSessionSplitBySessionDurationEnabledWhenMaxEventsIsSmallerZeroButAttributeIsSet)
{
	// given
	const int32_t sessionDuration = 0;

	ON_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::MAX_SESSION_DURATION))
		.WillByDefault(testing::Return(true));
	ON_CALL(*mockAttributes, getMaxSessionDurationInMilliseconds())
		.WillByDefault(testing::Return(sessionDuration));

	auto target = ServerConfiguration_t::from(mockAttributes);
	auto other = MockIServerConfiguration::createNice();
	ON_CALL(*other, isSessionSplitBySessionDurationEnabled())
		.WillByDefault(testing::Return(true));

	ASSERT_THAT(target->isSessionSplitBySessionDurationEnabled(), testing::Eq(false));

	// when
	auto obtained = target->merge(other);

	// then
	ASSERT_THAT(obtained->isSessionSplitBySessionDurationEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, mergeKeepsIsSessionSplitBySessionDurationEnabledWhenMaxEventsIsGreaterZeroButAttributeIsNotSet)
{
	// given
	const int32_t sessionDuration = 73;

	ON_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::MAX_SESSION_DURATION))
		.WillByDefault(testing::Return(false));
	ON_CALL(*mockAttributes, getMaxSessionDurationInMilliseconds())
		.WillByDefault(testing::Return(sessionDuration));

	auto target = ServerConfiguration_t::from(mockAttributes);
	auto other = MockIServerConfiguration::createNice();
	ON_CALL(*other, isSessionSplitBySessionDurationEnabled())
		.WillByDefault(testing::Return(true));

	ASSERT_THAT(target->isSessionSplitBySessionDurationEnabled(), testing::Eq(false));

	// when
	auto obtained = target->merge(other);

	// then
	ASSERT_THAT(obtained->isSessionSplitBySessionDurationEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, mergeKeepsOriginalMaxEventsPerSession)
{
	// given
	int32_t eventsPerSession = 73;
	auto target = ServerConfiguration_t::Builder(defaultValues)
		.withMaxEventsPerSession(eventsPerSession)
		.build();
	auto other = ServerConfiguration_t::Builder(defaultValues)
		.withMaxEventsPerSession(1234)
		.build();

	// when
	auto obtained = target->merge(other);

	// then
	ASSERT_THAT(obtained->getMaxEventsPerSession(), testing::Eq(eventsPerSession));
}

TEST_F(ServerConfigurationTest, mergeKeepsIsSessionSplitByEventsEnabledWhenMaxEventsIsGreaterZeroAndAttributeIsSet)
{
	// with
	int32_t eventsPerSession = 73;

	// expect
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::MAX_EVENTS_PER_SESSION))
		.Times(1)
		.WillOnce(testing::Return(true));
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::MAX_SESSION_DURATION))
		.Times(1);
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::SESSION_IDLE_TIMEOUT))
		.Times(1);
	EXPECT_CALL(*mockAttributes, getMaxEventsPerSession())
		.Times(1)
		.WillOnce(testing::Return(eventsPerSession));

	// given
	auto target = ServerConfiguration_t::from(mockAttributes);
	auto other = ServerConfiguration_t::Builder(defaultValues).build();

	ASSERT_THAT(other->isSessionSplitByEventsEnabled(), testing::Eq(false));
	ASSERT_THAT(target->isSessionSplitByEventsEnabled(), testing::Eq(true));

	// when
	auto obtained = target->merge(other);

	// then
	ASSERT_THAT(obtained->isSessionSplitByEventsEnabled(), testing::Eq(true));
}

TEST_F(ServerConfigurationTest, mergeKeepsIsSessionSplitByEventsEnabledWhenMaxEventsIsEqualZeroAndAttributeIsSet)
{
	// with
	int32_t eventsPerSession = 0;

	// expect
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::MAX_EVENTS_PER_SESSION))
		.Times(1)
		.WillOnce(testing::Return(true));
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::MAX_SESSION_DURATION))
		.Times(1);
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::SESSION_IDLE_TIMEOUT))
		.Times(1);
	EXPECT_CALL(*mockAttributes, getMaxEventsPerSession())
		.Times(1)
		.WillOnce(testing::Return(eventsPerSession));

	// given
	auto target = ServerConfiguration_t::from(mockAttributes);
	auto other = MockIServerConfiguration::createNice();
	ON_CALL(*other, isSessionSplitByEventsEnabled())
		.WillByDefault(testing::Return(true));

	ASSERT_THAT(target->isSessionSplitByEventsEnabled(), testing::Eq(false));

	// when
	auto obtained = target->merge(other);

	// then
	ASSERT_THAT(obtained->isSessionSplitByEventsEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, mergeKeepsIsSessionSplitByEventsEnabledWhenMaxEventsIsLessThanZeroAndAttributeIsSet)
{
	// with
	int32_t eventsPerSession = -1;

	// expect
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::MAX_EVENTS_PER_SESSION))
		.Times(1)
		.WillOnce(testing::Return(true));
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::MAX_SESSION_DURATION))
		.Times(1);
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::SESSION_IDLE_TIMEOUT))
		.Times(1);
	EXPECT_CALL(*mockAttributes, getMaxEventsPerSession())
		.Times(1)
		.WillOnce(testing::Return(eventsPerSession));

	// given
	auto target = ServerConfiguration_t::from(mockAttributes);
	auto other = MockIServerConfiguration::createNice();
	ON_CALL(*other, isSessionSplitByEventsEnabled())
		.WillByDefault(testing::Return(true));

	ASSERT_THAT(target->isSessionSplitByEventsEnabled(), testing::Eq(false));

	// when
	auto obtained = target->merge(other);

	// then
	ASSERT_THAT(obtained->isSessionSplitByEventsEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, mergeKeepsIsSessionSplitByEventsEnabledWhenMaxEventsIsGreaterZeroAndAttributeIsNotSet)
{
	// with
	int32_t eventsPerSession = 73;

	// expect
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::MAX_EVENTS_PER_SESSION))
		.Times(1)
		.WillOnce(testing::Return(false));
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::MAX_SESSION_DURATION))
		.Times(1);
	EXPECT_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::SESSION_IDLE_TIMEOUT))
		.Times(1);
	EXPECT_CALL(*mockAttributes, getMaxEventsPerSession())
		.Times(1)
		.WillOnce(testing::Return(eventsPerSession));

	// given
	auto target = ServerConfiguration_t::from(mockAttributes);
	auto other = MockIServerConfiguration::createNice();
	ON_CALL(*other, isSessionSplitByEventsEnabled())
		.WillByDefault(testing::Return(true));

	ASSERT_THAT(target->isSessionSplitByEventsEnabled(), testing::Eq(false));

	// when
	auto obtained = target->merge(other);

	// then
	ASSERT_THAT(obtained->isSessionSplitByEventsEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, mergeKeepsOriginalSessionTimeout)
{
	// given
	int32_t sessionTimeout = 73;
	auto target = ServerConfiguration_t::Builder(defaultValues).withSessionTimeoutInMilliseconds(sessionTimeout).build();
	auto other = ServerConfiguration_t::Builder(defaultValues).withSessionTimeoutInMilliseconds(456).build();

	// when
	auto obtained = target->merge(other);

	// then
	ASSERT_THAT(obtained->getSessionTimeoutInMilliseconds(), testing::Eq(sessionTimeout));
}

TEST_F(ServerConfigurationTest, mergeKeepsIsSessionSplitByIdleTimeoutEnabledWhenMaxEventsIsGreaterZeroAndAttributeIsSet)
{
	// given
	int idleTimeout = 73;

	ON_CALL(*mockAttributes, getSessionTimeoutInMilliseconds())
		.WillByDefault(testing::Return(idleTimeout));
	ON_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::SESSION_IDLE_TIMEOUT))
		.WillByDefault(testing::Return(true));

	auto target = ServerConfiguration_t::from(mockAttributes);
	auto other = ServerConfiguration_t::Builder(defaultValues).build();

	ASSERT_THAT(other->isSessionSplitByIdleTimeoutEnabled(), testing::Eq(false));
	ASSERT_THAT(target->isSessionSplitByIdleTimeoutEnabled(), testing::Eq(true));

	// when
	auto obtained = target->merge(other);

	// then
	ASSERT_THAT(obtained->isSessionSplitByIdleTimeoutEnabled(), testing::Eq(true));
}

TEST_F(ServerConfigurationTest, mergeKeepsIsSessionSplitByIdleTimeoutEnabledWhenMaxEventsIsSmallerZeroButAttributeIsSet)
{
	// given
	const int32_t idleTimeout = 0;

	ON_CALL(*mockAttributes, getSessionTimeoutInMilliseconds())
		.WillByDefault(testing::Return(idleTimeout));
	ON_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::SESSION_IDLE_TIMEOUT))
		.WillByDefault(testing::Return(true));

	auto target = ServerConfiguration_t::from(mockAttributes);
	auto other = MockIServerConfiguration::createNice();
	ON_CALL(*other, isSessionSplitByIdleTimeoutEnabled())
		.WillByDefault(testing::Return(false));

	ASSERT_THAT(other->isSessionSplitByIdleTimeoutEnabled(), testing::Eq(false));

	// when
	auto obtained = target->merge(other);

	// then
	ASSERT_THAT(obtained->isSessionSplitByIdleTimeoutEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, mergeKeepsIsSessionSplitByIdleTimeoutEnabledWhenMaxEventsIsGreaterZeroButAttributeIsNotSet)
{
	// given
	const int32_t idleTimeout = 73;

	ON_CALL(*mockAttributes, getSessionTimeoutInMilliseconds())
		.WillByDefault(testing::Return(idleTimeout));
	ON_CALL(*mockAttributes, isAttributeSet(ResponseAttribute_t::SESSION_IDLE_TIMEOUT))
		.WillByDefault(testing::Return(false));

	auto target = ServerConfiguration_t::from(mockAttributes);
	auto other = MockIServerConfiguration::createNice();
	ON_CALL(*other, isSessionSplitByIdleTimeoutEnabled())
		.WillByDefault(testing::Return(true));

	ASSERT_THAT(target->isSessionSplitByIdleTimeoutEnabled(), testing::Eq(false));

	// when
	auto obtained = target->merge(other);

	// then
	ASSERT_THAT(obtained->isSessionSplitByIdleTimeoutEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, mergeKeepsOriginalVisitStoreVersion)
{
	// given
	int32_t visitStoreVersion = 73;
	auto target = ServerConfiguration_t::Builder(defaultValues).withVisitStoreVersion(visitStoreVersion).build();
	auto other = ServerConfiguration_t::Builder(defaultValues).withVisitStoreVersion(234).build();

	// when
	auto obtained = target->merge(other);

	// then
	ASSERT_THAT(obtained->getVisitStoreVersion(), testing::Eq(visitStoreVersion));
}

TEST_F(ServerConfigurationTest, mergeKeepsOriginalTrafficControlPercentage)
{
	// given
	const auto trafficControlPercentage = 73;
	auto target = ServerConfiguration_t::Builder(defaultValues).withTrafficControlPercentage(trafficControlPercentage).build();
	auto other = ServerConfiguration_t::Builder(defaultValues).withTrafficControlPercentage(37).build();

	// when
	auto obtained = target->merge(other);

	// then
	ASSERT_THAT(obtained->getTrafficControlPercentage(), testing::Eq(trafficControlPercentage));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// builder tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(ServerConfigurationTest, buildPropagatesCaptureEnabledToInstance)
{
	// given
	const bool capture = true;

	// when
	auto obtained = ServerConfiguration_t::Builder(defaultValues).withCapture(capture).build();

	// then
	ASSERT_THAT(obtained->isCaptureEnabled(), testing::Eq(capture));
}

TEST_F(ServerConfigurationTest, buildPropagatesCaptureDisabledToInstance)
{
	// given
	const bool capture = false;

	// when
	auto obtained = ServerConfiguration_t::Builder(defaultValues).withCapture(capture).build();

	// then
	ASSERT_THAT(obtained->isCaptureEnabled(), testing::Eq(capture));
}

TEST_F(ServerConfigurationTest, buildPropagatesCrashReportingEnabledToInstance)
{
	// given
	const bool crashReporting = true;

	// when
	auto obtained = ServerConfiguration_t::Builder(defaultValues).withCrashReporting(crashReporting).build();

	// then
	ASSERT_THAT(obtained->isCrashReportingEnabled(), testing::Eq(crashReporting));
}

TEST_F(ServerConfigurationTest, buildPropagatesCrashReportingDisabledToInstance)
{
	// given
	const bool crashReporting = false;

	// when
	auto obtained = ServerConfiguration_t::Builder(defaultValues).withCrashReporting(crashReporting).build();

	// then
	ASSERT_THAT(obtained->isCrashReportingEnabled(), testing::Eq(crashReporting));
}

TEST_F(ServerConfigurationTest, buildPropagatesErrorReportingEnabledToInstance)
{
	// given
	const bool errorReporting = true;

	// when
	auto obtained = ServerConfiguration_t::Builder(defaultValues).withErrorReporting(errorReporting).build();

	// then
	ASSERT_THAT(obtained->isErrorReportingEnabled(), testing::Eq(errorReporting));
}

TEST_F(ServerConfigurationTest, buildPropagatesErrorReportingDisabledToInstance)
{
	// given
	const bool errorReporting = false;

	// when
	auto obtained = ServerConfiguration_t::Builder(defaultValues).withErrorReporting(errorReporting).build();

	// then
	ASSERT_THAT(obtained->isErrorReportingEnabled(), testing::Eq(errorReporting));
}

TEST_F(ServerConfigurationTest, buildPropagatesServerIdToInstance)
{
	// given
	const int32_t serverId = 73;

	// when
	auto obtained = ServerConfiguration_t::Builder(defaultValues).withServerId(serverId).build();

	// then
	ASSERT_THAT(obtained->getServerId(), testing::Eq(serverId));
}

TEST_F(ServerConfigurationTest, buildPropagatesBeaconSizeToInstance)
{
	// given
	const int32_t beaconSize = 73;

	// when
	auto obtained = ServerConfiguration_t::Builder(defaultValues).withBeaconSizeInBytes(beaconSize).build();

	// then
	ASSERT_THAT(obtained->getBeaconSizeInBytes(), testing::Eq(beaconSize));
}

TEST_F(ServerConfigurationTest, buildPropagatesMultiplicityToInstance)
{
	// given
	const int32_t multiplicity = 73;

	// when
	auto obtained = ServerConfiguration_t::Builder(defaultValues).withMultiplicity(multiplicity).build();

	// then
	ASSERT_THAT(obtained->getMultiplicity(), testing::Eq(multiplicity));
}

TEST_F(ServerConfigurationTest, buildPropagatesSendIntervalToInstance)
{
	// given
	const int32_t sendInterval = 777;

	// when
	auto obtained = ServerConfiguration_t::Builder(defaultValues)
		.withSendIntervalInMilliseconds(sendInterval).build();

	// then
	ASSERT_THAT(obtained->getSendIntervalInMilliseconds(), testing::Eq(sendInterval));
}

TEST_F(ServerConfigurationTest, buildPropagatesMaxSessionDurationToInstance)
{
	// given
	const int32_t sessionDuration = 73;

	// when
	auto obtained = ServerConfiguration_t::Builder(defaultValues).withMaxSessionDurationInMilliseconds(sessionDuration).build();

	// then
	ASSERT_THAT(obtained->getMaxSessionDurationInMilliseconds(), testing::Eq(sessionDuration));
}

TEST_F(ServerConfigurationTest, buildPropagatesMaxEventsPerSessionToInstance)
{
	// given
	const int32_t eventsPerSession = 73;

	// when
	auto obtained = ServerConfiguration_t::Builder(defaultValues).withMaxEventsPerSession(eventsPerSession).build();

	// then
	ASSERT_THAT(obtained->getMaxEventsPerSession(), testing::Eq(eventsPerSession));
}

TEST_F(ServerConfigurationTest, buildPropagatesSessionTimeoutToInstance)
{
	// given
	const int32_t sessionTimeout = 73;

	// when
	auto obtained = ServerConfiguration_t::Builder(defaultValues).withSessionTimeoutInMilliseconds(sessionTimeout).build();

	// then
	ASSERT_THAT(obtained->getSessionTimeoutInMilliseconds(), testing::Eq(sessionTimeout));
}

TEST_F(ServerConfigurationTest, buildPropagatesVisitStoreVersionToInstance)
{
	// given
	const int32_t visitStoreVersion = 73;

	// when
	auto obtained = ServerConfiguration_t::Builder(defaultValues).withVisitStoreVersion(visitStoreVersion).build();

	// then
	ASSERT_THAT(obtained->getVisitStoreVersion(), testing::Eq(visitStoreVersion));
}

TEST_F(ServerConfigurationTest, buildPropagatesTrafficControlPercentageToInstance)
{
	// given
	const auto trafficControlPercentage = 73;

	// when
	auto obtained = ServerConfiguration_t::Builder(defaultValues)
		.withTrafficControlPercentage(trafficControlPercentage)
		.build();

	// then
	ASSERT_THAT(obtained->getTrafficControlPercentage(), testing::Eq(trafficControlPercentage));
}
