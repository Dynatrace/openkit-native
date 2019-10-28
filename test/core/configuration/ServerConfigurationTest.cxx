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

#include "../../protocol/mock/MockIStatusResponse.h"

#include "core/configuration/ServerConfiguration.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace test;

using MockIStatusResponse_sp = std::shared_ptr<MockIStatusResponse>;
using ServerConfiguration_t = core::configuration::ServerConfiguration;

class ServerConfigurationTest : public testing::Test
{
protected:

	MockIStatusResponse_sp mockStatusResponse;

	void SetUp() override
	{
		mockStatusResponse = MockIStatusResponse::createNice();
		ON_CALL(*mockStatusResponse, isCapture())
			.WillByDefault(testing::Return(ServerConfiguration_t::DEFAULT_CAPTURE_ENABLED));
		ON_CALL(*mockStatusResponse, isCaptureCrashes())
			.WillByDefault(testing::Return(ServerConfiguration_t::DEFAULT_CRASH_REPORTING_ENABLED));
		ON_CALL(*mockStatusResponse, isCaptureErrors())
			.WillByDefault(testing::Return(ServerConfiguration_t::DEFAULT_ERROR_REPORTING_ENABLED));
		ON_CALL(*mockStatusResponse, getSendInterval())
			.WillByDefault(testing::Return(ServerConfiguration_t::DEFAULT_SEND_INTERVAL));
		ON_CALL(*mockStatusResponse, getServerID())
			.WillByDefault(testing::Return(ServerConfiguration_t::DEFAULT_SERVER_ID));
		ON_CALL(*mockStatusResponse, getMaxBeaconSize())
			.WillByDefault(testing::Return(ServerConfiguration_t::DEFAULT_BEACON_SIZE));
		ON_CALL(*mockStatusResponse, getMultiplicity())
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// creation via 'from' factory tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(ServerConfigurationTest, creatingAServerConfigurationFromNullStatusResponseGivesNull)
{
	ASSERT_THAT(ServerConfiguration_t::from(nullptr), testing::Eq(nullptr));
}

TEST_F(ServerConfigurationTest, creatingAServerConfigurationFromStatusResponseCopiesCaptureSettings)
{
	// expect
	EXPECT_CALL(*mockStatusResponse, isCapture())
		.Times(1)
		.WillOnce(testing::Return(false));

	// when
	auto target = ServerConfiguration_t::from(mockStatusResponse);

	// then
	ASSERT_THAT(target->isCaptureEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, creatingAServerConfigurationFromStatusResponseCopiesCrashReportingSettings)
{
	// expect
	EXPECT_CALL(*mockStatusResponse, isCaptureCrashes())
		.Times(1)
		.WillOnce(testing::Return(false));

	// when
	auto target = ServerConfiguration_t::from(mockStatusResponse);

	// then
	ASSERT_THAT(target->isCrashReportingEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, creatingAServerConfigurationFromStatusResponseCopiesErrorReportingSettings)
{
	// expect
	EXPECT_CALL(*mockStatusResponse, isCaptureErrors())
		.Times(1)
		.WillOnce(testing::Return(false));

	// when
	auto target = ServerConfiguration_t::from(mockStatusResponse);

	// then
	ASSERT_THAT(target->isErrorReportingEnabled(), testing::Eq(false));
}

TEST_F(ServerConfigurationTest, creatingAServerConfigurationFromStatusResponseCopiesSendingIntervalSettings)
{
	// with
	int32_t sendInterval = 1234;

	// expect
	EXPECT_CALL(*mockStatusResponse, getSendInterval())
		.Times(1)
		.WillOnce(testing::Return(sendInterval));

	// when
	auto target = ServerConfiguration_t::from(mockStatusResponse);

	// then
	ASSERT_THAT(target->getSendIntervalInMilliseconds(), testing::Eq(sendInterval));
}

TEST_F(ServerConfigurationTest, creatingAServerConfigurationFromStatusResponseCopiesServerIdSettings)
{
	// with
	int32_t serverId = 73;

	// expect
	EXPECT_CALL(*mockStatusResponse, getServerID())
		.Times(1)
		.WillOnce(testing::Return(serverId));

	// when
	auto target = ServerConfiguration_t::from(mockStatusResponse);

	// then
	ASSERT_THAT(target->getServerId(), testing::Eq(serverId));
}

TEST_F(ServerConfigurationTest, creatingAServerConfigurationFromStatusResponseCopiesBeaconSizeSettings)
{
	// with
	int32_t beaconSize = 37;

	// expect
	EXPECT_CALL(*mockStatusResponse, getMaxBeaconSize())
		.Times(1)
		.WillOnce(testing::Return(beaconSize));

	// when
	auto target = ServerConfiguration_t::from(mockStatusResponse);

	// then
	ASSERT_THAT(target->getBeaconSizeInBytes(), testing::Eq(beaconSize));
}

TEST_F(ServerConfigurationTest, creatingAServerConfigurationFromStatusResponseCopiesMultiplicitySettings)
{
	// with
	int32_t multiplicity = 37;

	// expect
	EXPECT_CALL(*mockStatusResponse, getMultiplicity())
		.Times(1)
		.WillOnce(testing::Return(multiplicity));

	// when
	auto target = ServerConfiguration_t::from(mockStatusResponse);

	// then
	ASSERT_THAT(target->getMultiplicity(), testing::Eq(multiplicity));
}

TEST_F(ServerConfigurationTest, sendingDataToTheServerIsAllowedIfCapturingIsEnabledAndMultiplicityIsGreaterThanZero)
{
	// with
	ON_CALL(*mockStatusResponse, isCapture()).WillByDefault(testing::Return(true));
	ON_CALL(*mockStatusResponse, getMultiplicity()).WillByDefault(testing::Return(1));

	// given
	auto target = ServerConfiguration_t::from(mockStatusResponse);

	// when
	auto obtained = target->isSendingDataAllowed();

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
}

TEST_F(ServerConfigurationTest, sendingDataToTheServerIsNotAllowedIfCapturingIsDisallowed)
{
	// with
	ON_CALL(*mockStatusResponse, isCapture()).WillByDefault(testing::Return(false));
	ON_CALL(*mockStatusResponse, getMultiplicity()).WillByDefault(testing::Return(1));

	// given
	auto target = ServerConfiguration_t::from(mockStatusResponse);

	// when
	auto obtained = target->isSendingDataAllowed();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(ServerConfigurationTest, sendingDataToTheServerIsNotAllowedIfCapturingIsEnabledButMultiplicityIsZero)
{
	// with
	ON_CALL(*mockStatusResponse, isCapture()).WillByDefault(testing::Return(true));
	ON_CALL(*mockStatusResponse, getMultiplicity()).WillByDefault(testing::Return(0));

	// given
	auto target = ServerConfiguration_t::from(mockStatusResponse);

	// when
	auto obtained = target->isSendingDataAllowed();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(ServerConfigurationTest, sendingCrashesToTheServerIsAllowedIfDataSendingIsAllowedAndCaptureCrashesIsEnabled)
{
	// with
	ON_CALL(*mockStatusResponse, isCapture()).WillByDefault(testing::Return(true));
	ON_CALL(*mockStatusResponse, getMultiplicity()).WillByDefault(testing::Return(1));
	ON_CALL(*mockStatusResponse, isCaptureCrashes()).WillByDefault(testing::Return(true));

	// given
	auto target = ServerConfiguration_t::from(mockStatusResponse);

	// when
	auto obtained = target->isSendingCrashesAllowed();

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
}

TEST_F(ServerConfigurationTest, sendingCrashesToTheServerIsNotAllowedIfDataSendingIsNotAllowed)
{
	// with
	ON_CALL(*mockStatusResponse, isCapture()).WillByDefault(testing::Return(false));
	ON_CALL(*mockStatusResponse, getMultiplicity()).WillByDefault(testing::Return(1));
	ON_CALL(*mockStatusResponse, isCaptureCrashes()).WillByDefault(testing::Return(true));

	// given
	auto target = ServerConfiguration_t::from(mockStatusResponse);

	// when
	auto obtained = target->isSendingCrashesAllowed();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(ServerConfigurationTest, sendingCrashesToTheServerIsNotAllowedIfDataSendingIsAllowedButCaptureCrashesIsDisabled)
{
	// with
	ON_CALL(*mockStatusResponse, isCapture()).WillByDefault(testing::Return(true));
	ON_CALL(*mockStatusResponse, getMultiplicity()).WillByDefault(testing::Return(1));
	ON_CALL(*mockStatusResponse, isCaptureCrashes()).WillByDefault(testing::Return(false));

	// given
	auto target = ServerConfiguration_t::from(mockStatusResponse);

	// when
	auto obtained = target->isSendingCrashesAllowed();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(ServerConfigurationTest, sendingErrorToTheServerIsAllowedIfDataSendingIsAllowedAndCaptureErrorIsEnabled)
{
	// with
	ON_CALL(*mockStatusResponse, isCapture()).WillByDefault(testing::Return(true));
	ON_CALL(*mockStatusResponse, getMultiplicity()).WillByDefault(testing::Return(1));
	ON_CALL(*mockStatusResponse, isCaptureErrors()).WillByDefault(testing::Return(true));

	// given
	auto target = ServerConfiguration_t::from(mockStatusResponse);

	// when
	auto obtained = target->isSendingErrorsAllowed();

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
}

TEST_F(ServerConfigurationTest, sendingErrorToTheServerIsNotAllowedIfDataSendingIsNotAllowed)
{
	// with
	ON_CALL(*mockStatusResponse, isCapture()).WillByDefault(testing::Return(false));
	ON_CALL(*mockStatusResponse, getMultiplicity()).WillByDefault(testing::Return(1));
	ON_CALL(*mockStatusResponse, isCaptureErrors()).WillByDefault(testing::Return(true));

	// given
	auto target = ServerConfiguration_t::from(mockStatusResponse);

	// when
	auto obtained = target->isSendingErrorsAllowed();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(ServerConfigurationTest, sendingErrorToTheServerIsNotAllowedIfDataSendingIsAllowedButCaptureErrorsDisabled)
{
	// with
	ON_CALL(*mockStatusResponse, isCapture()).WillByDefault(testing::Return(true));
	ON_CALL(*mockStatusResponse, getMultiplicity()).WillByDefault(testing::Return(1));
	ON_CALL(*mockStatusResponse, isCaptureErrors()).WillByDefault(testing::Return(false));

	// given
	auto target = ServerConfiguration_t::from(mockStatusResponse);

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
