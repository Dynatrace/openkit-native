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

#include "../../protocol/mock/MockIResponseAttributes.h"

#include "core/configuration/ServerConfiguration.h"
#include "protocol/ResponseAttributesDefaults.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace test;

using IResponseAttributes_sp = std::shared_ptr<protocol::IResponseAttributes>;
using ResponseAttributesDefaults_t = protocol::ResponseAttributesDefaults;
using ServerConfiguration_t = core::configuration::ServerConfiguration;
using MockIResponseAttributes_sp = std::shared_ptr<MockIResponseAttributes>;

class ServerConfigurationTest : public testing::Test
{
protected:

	IResponseAttributes_sp defaultValues;
	MockIResponseAttributes_sp mockAttributes;

	void SetUp() override
	{
		defaultValues = ResponseAttributesDefaults_t::UNDEFINED;
		mockAttributes = MockIResponseAttributes::createNice();
		ON_CALL(*mockAttributes, isCapture())
			.WillByDefault(testing::Return(ServerConfiguration_t::DEFAULT_CAPTURE_ENABLED));
		ON_CALL(*mockAttributes, isCaptureCrashes())
			.WillByDefault(testing::Return(ServerConfiguration_t::DEFAULT_CRASH_REPORTING_ENABLED));
		ON_CALL(*mockAttributes, isCaptureErrors())
			.WillByDefault(testing::Return(ServerConfiguration_t::DEFAULT_ERROR_REPORTING_ENABLED));
		ON_CALL(*mockAttributes, getSendIntervalInMilliseconds())
			.WillByDefault(testing::Return(ServerConfiguration_t::DEFAULT_SEND_INTERVAL));
		ON_CALL(*mockAttributes, getServerId())
			.WillByDefault(testing::Return(ServerConfiguration_t::DEFAULT_SERVER_ID));
		ON_CALL(*mockAttributes, getMaxBeaconSizeInBytes())
			.WillByDefault(testing::Return(ServerConfiguration_t::DEFAULT_BEACON_SIZE));
		ON_CALL(*mockAttributes, getMultiplicity())
			.WillByDefault(testing::Return(ServerConfiguration_t::DEFAULT_MULTIPLICITY));
	}
};

TEST_F(ServerConfigurationTest, inDefaultServerConfigurationCapturingIsEnabled)
{
	ASSERT_THAT(ServerConfiguration_t::DEFAULT->isCaptureEnabled(), testing::Eq(true));
}

TEST_F(ServerConfigurationTest, isDefaultServerConfigurationCrashReportingIsEnabled)
{
	ASSERT_THAT(ServerConfiguration_t::DEFAULT->isCrashReportingEnabled(), testing::Eq(true));
}

TEST_F(ServerConfigurationTest, isDefaultServerConfigurationErrorReportingIsEnabled)
{
	ASSERT_THAT(ServerConfiguration_t::DEFAULT->isErrorReportingEnabled(), testing::Eq(true));
}

TEST_F(ServerConfigurationTest, inDefaultServerConfigurationSendIntervalIsMinusOne)
{
	ASSERT_THAT(ServerConfiguration_t::DEFAULT->getSendIntervalInMilliseconds(), testing::Eq(-1));
}

TEST_F(ServerConfigurationTest, inDefaultServerConfigurationServerIdIsMinusOne)
{
	ASSERT_THAT(ServerConfiguration_t::DEFAULT->getServerId(), testing::Eq(-1));
}

TEST_F(ServerConfigurationTest, inDefaultServerConfigurationBeaconSizeIsMinusOne)
{
	ASSERT_THAT(ServerConfiguration_t::DEFAULT->getBeaconSizeInBytes(), testing::Eq(-1));
}

TEST_F(ServerConfigurationTest, inDefaultServerConfigurationMultiplicityIsOne)
{
	ASSERT_THAT(ServerConfiguration_t::DEFAULT->getMultiplicity(), testing::Eq(1));
}

TEST_F(ServerConfigurationTest, inDefaultServerConfigurationMaxSessionDurationIsMinusOne)
{
	ASSERT_THAT(ServerConfiguration_t::DEFAULT->getMaxSessionDurationInMilliseconds(), testing::Eq(-1));
}

TEST_F(ServerConfigurationTest, inDefaultServerConfigurationMaxEventsPerSessionIsMinusOne)
{
	ASSERT_THAT(ServerConfiguration_t::DEFAULT->getMaxEventsPerSession(), testing::Eq(-1));
}

TEST_F(ServerConfigurationTest, inDefaultServerConfigurationSessionTimeoutIsMinusOne)
{
	ASSERT_THAT(ServerConfiguration_t::DEFAULT->getSessionTimeoutInMilliseconds(), testing::Eq(-1));
}

TEST_F(ServerConfigurationTest, inDefaultServerConfigurationVisitStoreVersionIsMinusOne)
{
	ASSERT_THAT(ServerConfiguration_t::DEFAULT->getVisitStoreVersion(), testing::Eq(1));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// creation via 'from' factory tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(ServerConfigurationTest, creatingAServerConfigurationFromNullStatusResponseGivesNull)
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

TEST_F(ServerConfigurationTest, creatingAServerConfigurationFromResponseAttributesCopiesSendingIntervalSettings)
{
	// with
	int32_t sendInterval = 1234;

	// expect
	EXPECT_CALL(*mockAttributes, getSendIntervalInMilliseconds())
		.Times(1)
		.WillOnce(testing::Return(sendInterval));

	// when
	auto target = ServerConfiguration_t::from(mockAttributes);

	// then
	ASSERT_THAT(target->getSendIntervalInMilliseconds(), testing::Eq(sendInterval));
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
/// merge server configuration tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(ServerConfigurationTest, mergeTakesOverEnabledCapture)
{
	// given
	auto target = ServerConfiguration_t::Builder().withCapture(false).build();
	auto other = ServerConfiguration_t::Builder().withCapture(true).build();

	// when
	auto obtained = target->merge(other);

	// then
	ASSERT_THAT(obtained->isCaptureEnabled(), testing::Eq(true));
}

TEST_F(ServerConfigurationTest, mergeTakesOverDisabledCapture)
{
	// given
	auto target = ServerConfiguration_t::Builder().withCapture(true).build();
	auto other = ServerConfiguration_t::Builder().withCapture(false).build();

	// when
	auto obtained = target->merge(other);

	// then
	ASSERT_THAT(obtained->isCaptureEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, mergeTakesOverEnabledCrashReporting)
{
	// given
	auto target = ServerConfiguration_t::Builder().withCrashReporting(false).build();
	auto other = ServerConfiguration_t::Builder().withCrashReporting(true).build();

	// when
	auto obtained = target->merge(other);

	// then
	ASSERT_THAT(obtained->isCrashReportingEnabled(), testing::Eq(true));
}

TEST_F(ServerConfigurationTest, mergeTakesOverDisabledCrashReporting)
{
	// given
	auto target = ServerConfiguration_t::Builder().withCrashReporting(true).build();
	auto other = ServerConfiguration_t::Builder().withCrashReporting(false).build();

	// when
	auto obtained = target->merge(other);

	// then
	ASSERT_THAT(obtained->isCrashReportingEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, mergeTakesOverEnabledErrorReporting)
{
	// given
	auto target = ServerConfiguration_t::Builder().withErrorReporting(false).build();
	auto other = ServerConfiguration_t::Builder().withErrorReporting(true).build();

	// when
	auto obtained = target->merge(other);

	// then
	ASSERT_THAT(obtained->isErrorReportingEnabled(), testing::Eq(true));
}

TEST_F(ServerConfigurationTest, mergeTakesOverDisabledErrorReporting)
{
	// given
	auto target = ServerConfiguration_t::Builder().withErrorReporting(true).build();
	auto other = ServerConfiguration_t::Builder().withErrorReporting(false).build();

	// when
	auto obtained = target->merge(other);

	// then
	ASSERT_THAT(obtained->isErrorReportingEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, mergeTakesOverSendInterval)
{
	// given
	int32_t sendInterval = 73;
	auto target = ServerConfiguration_t::Builder().withSendIntervalInMilliseconds(37).build();
	auto other = ServerConfiguration_t::Builder().withSendIntervalInMilliseconds(sendInterval).build();

	// when
	auto obtained = target->merge(other);

	// then
	ASSERT_THAT(obtained->getSendIntervalInMilliseconds(), testing::Eq(sendInterval));
}

TEST_F(ServerConfigurationTest, mergeTakesOverBeaconSize)
{
	// given
	int32_t beaconSize = 73;
	auto target = ServerConfiguration_t::Builder().withBeaconSizeInBytes(37).build();
	auto other = ServerConfiguration_t::Builder().withBeaconSizeInBytes(beaconSize).build();

	// when
	auto obtained = target->merge(other);

	// then
	ASSERT_THAT(obtained->getBeaconSizeInBytes(), testing::Eq(beaconSize));
}

TEST_F(ServerConfigurationTest, mergeIgnoresMultiplicity)
{
	// given
	int32_t multiplicity = 73;
	auto target = ServerConfiguration_t::Builder().withMultiplicity(multiplicity).build();
	auto other = ServerConfiguration_t::Builder().withMultiplicity(37).build();

	// when
	auto obtained = target->merge(other);

	// then
	ASSERT_THAT(obtained->getMultiplicity(), testing::Eq(multiplicity));
}

TEST_F(ServerConfigurationTest, mergeIgnoresServerId)
{
	// given
	int32_t serverId = 73;
	auto target = ServerConfiguration_t::Builder().withServerId(serverId).build();
	auto other = ServerConfiguration_t::Builder().withServerId(37).build();

	// when
	auto obtained = target->merge(other);

	// then
	ASSERT_THAT(obtained->getServerId(), testing::Eq(serverId));
}

TEST_F(ServerConfigurationTest, mergeTakesOverMaxSessionDuration)
{
	// given
	int32_t sessionDuration = 73;
	auto target = ServerConfiguration_t::Builder().withMaxSessionDurationInMilliseconds(37).build();
	auto other = ServerConfiguration_t::Builder().withMaxSessionDurationInMilliseconds(sessionDuration).build();

	// when
	auto obtained = target->merge(other);

	// then
	ASSERT_THAT(obtained->getMaxSessionDurationInMilliseconds(), testing::Eq(sessionDuration));
}

TEST_F(ServerConfigurationTest, mergeTakesOverMaxEventsPerSession)
{
	// given
	int32_t eventsPerSession = 73;
	auto target = ServerConfiguration_t::Builder().withMaxEventsPerSession(37).build();
	auto other = ServerConfiguration_t::Builder().withMaxEventsPerSession(eventsPerSession).build();

	// when
	auto obtained = target->merge(other);

	// then
	ASSERT_THAT(obtained->getMaxEventsPerSession(), testing::Eq(eventsPerSession));
}

TEST_F(ServerConfigurationTest, mergeTakesOverSessionTimeout)
{
	// given
	int32_t sessionTimeout = 73;
	auto target = ServerConfiguration_t::Builder().withSessionTimeoutInMilliseconds(37).build();
	auto other = ServerConfiguration_t::Builder().withSessionTimeoutInMilliseconds(sessionTimeout).build();

	// when
	auto obtained = target->merge(other);

	// then
	ASSERT_THAT(obtained->getSessionTimeoutInMilliseconds(), testing::Eq(sessionTimeout));
}

TEST_F(ServerConfigurationTest, mergeTakesOverVisitStoreVersion)
{
	// given
	int32_t visitStoreVersion = 73;
	auto target = ServerConfiguration_t::Builder().withVisitStoreVersion(37).build();
	auto other = ServerConfiguration_t::Builder().withVisitStoreVersion(visitStoreVersion).build();

	// when
	auto obtained = target->merge(other);

	// then
	ASSERT_THAT(obtained->getVisitStoreVersion(), testing::Eq(visitStoreVersion));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// builder tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(ServerConfigurationTest, buildPropagatesCaptureEnabledToInstance)
{
	// given
	const bool capture = true;

	// when
	auto obtained = ServerConfiguration_t::Builder().withCapture(capture).build();

	// then
	ASSERT_THAT(obtained->isCaptureEnabled(), testing::Eq(capture));
}

TEST_F(ServerConfigurationTest, buildPropagatesCaptureDisabledToInstance)
{
	// given
	const bool capture = false;

	// when
	auto obtained = ServerConfiguration_t::Builder().withCapture(capture).build();

	// then
	ASSERT_THAT(obtained->isCaptureEnabled(), testing::Eq(capture));
}

TEST_F(ServerConfigurationTest, buildPropagatesCrashReportingEnabledToInstance)
{
	// given
	const bool crashReporting = true;

	// when
	auto obtained = ServerConfiguration_t::Builder().withCrashReporting(crashReporting).build();

	// then
	ASSERT_THAT(obtained->isCrashReportingEnabled(), testing::Eq(crashReporting));
}

TEST_F(ServerConfigurationTest, buildPropagatesCrashReportingDisabledToInstance)
{
	// given
	const bool crashReporting = false;

	// when
	auto obtained = ServerConfiguration_t::Builder().withCrashReporting(crashReporting).build();

	// then
	ASSERT_THAT(obtained->isCrashReportingEnabled(), testing::Eq(crashReporting));
}

TEST_F(ServerConfigurationTest, buildPropagatesErrorReportingEnabledToInstance)
{
	// given
	const bool errorReporting = true;

	// when
	auto obtained = ServerConfiguration_t::Builder().withErrorReporting(errorReporting).build();

	// then
	ASSERT_THAT(obtained->isErrorReportingEnabled(), testing::Eq(errorReporting));
}

TEST_F(ServerConfigurationTest, buildPropagatesErrorReportingDisabledToInstance)
{
	// given
	const bool errorReporting = false;

	// when
	auto obtained = ServerConfiguration_t::Builder().withErrorReporting(errorReporting).build();

	// then
	ASSERT_THAT(obtained->isErrorReportingEnabled(), testing::Eq(errorReporting));
}

TEST_F(ServerConfigurationTest, buildPropagatesSendIntervalToInstance)
{
	// given
	const int32_t sendInterval = 73;

	// when
	auto obtained = ServerConfiguration_t::Builder().withSendIntervalInMilliseconds(sendInterval).build();

	// then
	ASSERT_THAT(obtained->getSendIntervalInMilliseconds(), testing::Eq(sendInterval));
}

TEST_F(ServerConfigurationTest, buildPropagatesServerIdToInstance)
{
	// given
	const int32_t serverId = 73;

	// when
	auto obtained = ServerConfiguration_t::Builder().withServerId(serverId).build();

	// then
	ASSERT_THAT(obtained->getServerId(), testing::Eq(serverId));
}

TEST_F(ServerConfigurationTest, buildPropagatesBeaconSizeToInstance)
{
	// given
	const int32_t beaconSize = 73;

	// when
	auto obtained = ServerConfiguration_t::Builder().withBeaconSizeInBytes(beaconSize).build();

	// then
	ASSERT_THAT(obtained->getBeaconSizeInBytes(), testing::Eq(beaconSize));
}

TEST_F(ServerConfigurationTest, buildPropagatesMultiplicityToInstance)
{
	// given
	const int32_t multiplicity = 73;

	// when
	auto obtained = ServerConfiguration_t::Builder().withMultiplicity(multiplicity).build();

	// then
	ASSERT_THAT(obtained->getMultiplicity(), testing::Eq(multiplicity));
}
