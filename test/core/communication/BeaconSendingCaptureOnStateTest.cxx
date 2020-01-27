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

#include "CustomMatchers.h"
#include "mock/MockIBeaconSendingContext.h"
#include "../configuration/mock/MockIBeaconConfiguration.h"
#include "../objects/mock/MockSessionInternals.h"
#include "../../api/mock/MockILogger.h"
#include "../../protocol/mock/MockIHTTPClient.h"
#include "../../protocol/mock/MockIStatusResponse.h"
#include "../../protocol/mock/MockIResponseAttributes.h"
#include "../../providers/mock/MockIHTTPClientProvider.h"

#include "core/communication/BeaconSendingCaptureOffState.h"
#include "core/communication/BeaconSendingCaptureOnState.h"
#include "core/configuration/IBeaconConfiguration.h"
#include "core/configuration/IServerConfiguration.h"
#include "core/objects/Session.h"
#include "core/objects/SessionInternals.h"
#include "protocol/StatusResponse.h"
#include "protocol/IStatusResponse.h"
#include "protocol/ResponseAttributes.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace test;

using BeaconSendingCaptureOffState_t = core::communication::BeaconSendingCaptureOffState;
using BeaconSendingCaptureOnState_t = core::communication::BeaconSendingCaptureOnState;
using IBeaconSendingState_sp = std::shared_ptr<core::communication::IBeaconSendingState>;
using IServerConfiguration_sp = std::shared_ptr<core::configuration::IServerConfiguration>;
using IStatusResponse_t = protocol::IStatusResponse;
using MockNiceIBeaconSendingContext_sp = std::shared_ptr<MockIBeaconSendingContext>;
using MockSession_sp = std::shared_ptr<MockSessionInternals>;
using SessionInternals_sp = std::shared_ptr<core::objects::SessionInternals>;
using StatusResponse_t = protocol::StatusResponse;
using ResponseAttributes_t = protocol::ResponseAttributes;
using IStatusResponse_sp = std::shared_ptr<IStatusResponse_t>;

class BeaconSendingCaptureOnStateTest : public testing::Test
{
protected:

	MockNiceIBeaconSendingContext_sp mockContext;
	MockSession_sp mockSession1Open;
	MockSession_sp mockSession2Open;
	MockSession_sp mockSession3Finished;
	MockSession_sp mockSession4Finished;
	MockSession_sp mockSession5New;
	MockSession_sp mockSession6New;

	void SetUp() override
	{
		auto mockLogger = MockILogger::createNice();
		auto responseAttributes = ResponseAttributes_t::withUndefinedDefaults().build();
		auto successResponse = StatusResponse_t::createSuccessResponse(mockLogger, responseAttributes, 200, IStatusResponse_t::ResponseHeaders());
		auto errorResponse = StatusResponse_t::createErrorResponse(mockLogger, 404);

		mockSession1Open = MockSessionInternals::createNice();
		ON_CALL(*mockSession1Open, sendBeacon(testing::_))
			.WillByDefault(testing::Return(successResponse));
		ON_CALL(*mockSession1Open, isDataSendingAllowed())
			.WillByDefault(testing::Return(true));

		mockSession2Open = MockSessionInternals::createNice();
		ON_CALL(*mockSession2Open, sendBeacon(testing::_))
			.WillByDefault(testing::Return(errorResponse));

		mockSession3Finished = MockSessionInternals::createNice();
		mockSession4Finished = MockSessionInternals::createNice();
		mockSession5New = MockSessionInternals::createNice();
		mockSession6New = MockSessionInternals::createNice();


		auto mockHTTPClientProvider = MockIHTTPClientProvider::createNice();

		mockContext = MockIBeaconSendingContext::createNice();
		ON_CALL(*mockContext, getCurrentTimestamp())
			.WillByDefault(testing::Return(42L));
		ON_CALL(*mockContext, getHTTPClientProvider())
			.WillByDefault(testing::Return(mockHTTPClientProvider));

		std::vector<SessionInternals_sp> openSessions = {mockSession1Open, mockSession2Open};
		ON_CALL(*mockContext, getAllOpenAndConfiguredSessions())
			.WillByDefault(testing::Return(openSessions));

		std::vector<SessionInternals_sp> finishedSessions = {mockSession3Finished, mockSession4Finished};
		ON_CALL(*mockContext, getAllFinishedAndConfiguredSessions())
			.WillByDefault(testing::Return(finishedSessions));
	}
};

TEST_F(BeaconSendingCaptureOnStateTest, aBeaconSendingCaptureOnStateIsNotATerminalState)
{
	// given
	BeaconSendingCaptureOnState_t target;

	// when
	auto obtained = target.isTerminalState();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(BeaconSendingCaptureOnStateTest, aBeaconSendingCaptureOnStateHasTerminalStateBeaconSendingFlushSessions)
{
	// given
	BeaconSendingCaptureOnState_t target;

	// when
	auto obtained = target.getShutdownState();

	// then
	ASSERT_THAT(obtained, IsABeaconSendingFlushSessionsState());
}

TEST_F(BeaconSendingCaptureOnStateTest, aBeaconSendingCaptureOnStateTransitionsToFlushStateIfShutdownRequested)
{
	// with
	ON_CALL(*mockContext, isShutdownRequested())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mockContext, isCaptureOn())
		.WillByDefault(testing::Return(true));

	// expect
	EXPECT_CALL(*mockContext, setNextState(IsABeaconSendingFlushSessionsState()))
		.Times(testing::Exactly(1));

	// given
	BeaconSendingCaptureOnState_t target;

	// when
	target.execute(*mockContext);
}


TEST_F(BeaconSendingCaptureOnStateTest, getStateNameReturnsCorrectStateName)
{
	// given
	BeaconSendingCaptureOnState_t target;

	// when
	auto obtained = target.getStateName();

	// then
	ASSERT_THAT(obtained, testing::StrEq("CaptureOn"));
}

TEST_F(BeaconSendingCaptureOnStateTest, newSessionRequestsAreMadeForAllNotConfiguredSessions)
{
	// with
	auto mockClient = MockIHTTPClient::createNice();
	std::vector<SessionInternals_sp> notConfiguredSessions = {mockSession5New, mockSession6New};
	auto mockLogger = MockILogger::createNice();
	auto responseAttributes = ResponseAttributes_t::withJsonDefaults().withMultiplicity(5).build();
	auto successResponse = StatusResponse_t::createSuccessResponse(mockLogger, responseAttributes, 200, IStatusResponse_t::ResponseHeaders());
	auto errorResponse = StatusResponse_t::createErrorResponse(mockLogger, 400);

	ON_CALL(*mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mockClient));
	ON_CALL(*mockContext, getAllNotConfiguredSessions())
		.WillByDefault(testing::Return(notConfiguredSessions));
	ON_CALL(*mockContext, updateLastResponseAttributesFrom(testing::_))
		.WillByDefault(testing::Return(successResponse->getResponseAttributes()));

	EXPECT_CALL(*mockClient, sendNewSessionRequest())
		.Times(testing::Exactly(2))
		.WillOnce(testing::Return(successResponse))
		.WillOnce(testing::Return(errorResponse))
	;

	ON_CALL(*mockSession5New, canSendNewSessionRequest())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mockSession6New, canSendNewSessionRequest())
		.WillByDefault(testing::Return(true));

	// expect
	IServerConfiguration_sp serverConfigCapture = nullptr;
	EXPECT_CALL(*mockSession5New, updateServerConfiguration(testing::_))
		.Times(testing::Exactly(1))
		.WillOnce(testing::SaveArg<0>(&serverConfigCapture));
	EXPECT_CALL(*mockSession6New, decreaseNumRemainingSessionRequests())
		.Times(1);

	// given
	BeaconSendingCaptureOnState_t target;

	// when
	target.execute(*mockContext);

	// then
	ASSERT_THAT(serverConfigCapture, testing::NotNull());
	ASSERT_THAT(serverConfigCapture->getMultiplicity(), testing::Eq(5));
}

TEST_F(BeaconSendingCaptureOnStateTest, successfulNewSessionRequestUpdateLastResponseAttributes)
{
	// with
	constexpr auto beaconSize = 73;
	auto mockResponseAttributes = MockIResponseAttributes::createNice();
	ON_CALL(*mockResponseAttributes, getMaxBeaconSizeInBytes())
		.WillByDefault(testing::Return(beaconSize));
	auto sessionRequestResponse = MockIStatusResponse::createNice();
	ON_CALL(*sessionRequestResponse, getResponseAttributes())
		.WillByDefault(testing::Return(mockResponseAttributes));

	EXPECT_CALL(*mockContext, updateLastResponseAttributesFrom(testing::Eq(sessionRequestResponse)))
		.Times(testing::Exactly(1))
		.WillOnce(testing::Return(mockResponseAttributes));

	auto mockClient = MockIHTTPClient::createNice();
	ON_CALL(*mockClient, sendNewSessionRequest())
		.WillByDefault(testing::Return(sessionRequestResponse));

	ON_CALL(*mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mockClient));
	ON_CALL(*mockContext, getAllNotConfiguredSessions())
		.WillByDefault(testing::Return(std::vector<SessionInternals_sp>{mockSession5New}));

	ON_CALL(*mockSession5New, canSendNewSessionRequest())
		.WillByDefault(testing::Return(true));

	IServerConfiguration_sp serverConfigCapture = nullptr;
	EXPECT_CALL(*mockSession5New, updateServerConfiguration(testing::_))
		.Times(testing::Exactly(1))
		.WillOnce(testing::SaveArg<0>(&serverConfigCapture));

	// given
	BeaconSendingCaptureOnState_t target;

	// when
	target.execute(*mockContext);

	// then
	ASSERT_THAT(serverConfigCapture, testing::NotNull());
	ASSERT_THAT(serverConfigCapture->getBeaconSizeInBytes(), testing::Eq(beaconSize));
}

TEST_F(BeaconSendingCaptureOnStateTest, unsuccessfulNewSessionRequestDoesNotMergeStatusResponse)
{
	// with
	auto sessionRequestResponse = MockIStatusResponse::createNice();
	ON_CALL(*sessionRequestResponse, isErroneousResponse())
		.WillByDefault(testing::Return(true));

	auto mockClient = MockIHTTPClient::createNice();
	ON_CALL(*mockClient, sendNewSessionRequest())
		.WillByDefault(testing::Return(sessionRequestResponse));

	auto contextAttributes = MockIResponseAttributes::createStrict();

	ON_CALL(*mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mockClient));
	ON_CALL(*mockContext, getLastResponseAttributes())
		.WillByDefault(testing::Return(contextAttributes));
	ON_CALL(*mockContext, getAllNotConfiguredSessions())
		.WillByDefault(testing::Return(std::vector<SessionInternals_sp>{mockSession5New}));
	
	ON_CALL(*mockSession5New, canSendNewSessionRequest())
		.WillByDefault(testing::Return(true));
	EXPECT_CALL(*mockSession5New, updateServerConfiguration(testing::_))
		.Times(0);

	// given
	BeaconSendingCaptureOnState_t target;

	// when
	target.execute(*mockContext);
}

TEST_F(BeaconSendingCaptureOnStateTest, captureIsDisabledIfNoFurtherNewSessionRequestsAreAllowed)
{
	// with
	auto mockClient = MockIHTTPClient::createNice();
	std::vector<SessionInternals_sp> notConfiguredSessions = {mockSession5New, mockSession6New};
	ON_CALL(*mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mockClient));
	ON_CALL(*mockContext, getAllNotConfiguredSessions())
		.WillByDefault(testing::Return(notConfiguredSessions));

	ON_CALL(*mockSession5New, canSendNewSessionRequest())
		.WillByDefault(testing::Return(false));
	ON_CALL(*mockSession6New, canSendNewSessionRequest())
		.WillByDefault(testing::Return(false));

	// expect
	EXPECT_CALL(*mockClient, sendNewSessionRequest())
		.Times(0);

	EXPECT_CALL(*mockSession5New, disableCapture()).Times(1);
	EXPECT_CALL(*mockSession6New, disableCapture()).Times(1);

	// given
	BeaconSendingCaptureOnState_t target;

	// when
	target.execute(*mockContext);
}

TEST_F(BeaconSendingCaptureOnStateTest, newSessionRequestsAreAbortedWhenTooManyRequestsResponseIsReceived)
{
	// with
	mockSession1Open = MockSessionInternals::createStrict();
	mockSession2Open = MockSessionInternals::createStrict();
	mockSession3Finished = MockSessionInternals::createStrict();
	mockSession4Finished = MockSessionInternals::createStrict();
	mockSession5New = MockSessionInternals::createStrict();
	mockSession6New = MockSessionInternals::createStrict();
	std::vector<SessionInternals_sp> notConfiguredSessions = {mockSession5New, mockSession6New};
	ON_CALL(*mockContext, getAllNotConfiguredSessions())
		.WillByDefault(testing::Return(notConfiguredSessions));
	std::vector<SessionInternals_sp> openSessions ={mockSession1Open, mockSession2Open};
	ON_CALL(*mockContext, getAllOpenAndConfiguredSessions())
		.WillByDefault(testing::Return(openSessions));
	std::vector<SessionInternals_sp> finishedSessions = {mockSession3Finished, mockSession4Finished};
	ON_CALL(*mockContext, getAllFinishedAndConfiguredSessions())
		.WillByDefault(testing::Return(finishedSessions));

	int64_t sleepTime = 6543;
	auto statusResponse = MockIStatusResponse::createNice();
	ON_CALL(*statusResponse, getResponseCode())
		.WillByDefault(testing::Return(429));
	ON_CALL(*statusResponse, isTooManyRequestsResponse())
		.WillByDefault(testing::Return(true));
	ON_CALL(*statusResponse, isErroneousResponse())
			.WillByDefault(testing::Return(true));
	ON_CALL(*statusResponse, getRetryAfterInMilliseconds())
			.WillByDefault(testing::Return(sleepTime));

	auto mockClient = MockIHTTPClient::createNice();
	ON_CALL(*mockClient, sendNewSessionRequest())
		.WillByDefault(testing::Return(statusResponse));

	ON_CALL(*mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mockClient));

	// expect
	EXPECT_CALL(*mockSession5New, canSendNewSessionRequest())
		.Times(testing::Exactly(1))
		.WillOnce(testing::Return(true));

	EXPECT_CALL(*mockSession6New, canSendNewSessionRequest())
		.Times(0);

	EXPECT_CALL(*mockClient, sendNewSessionRequest())
		.Times(1);

	IBeaconSendingState_sp captureOffStateCapture = nullptr;
	EXPECT_CALL(*mockContext, setNextState(testing::_))
		.Times(testing::Exactly(1))
		.WillOnce(testing::SaveArg<0>(&captureOffStateCapture));

	// given
	BeaconSendingCaptureOnState_t target;

	// when
	target.execute(*mockContext);

	// then
	ASSERT_THAT(captureOffStateCapture, testing::NotNull());
	auto captureOffState = std::dynamic_pointer_cast<BeaconSendingCaptureOffState_t>(captureOffStateCapture);
	ASSERT_THAT(captureOffState, testing::NotNull());
	ASSERT_THAT(captureOffState->getSleepTimeInMilliseconds(), testing::Eq(sleepTime));
}

TEST_F(BeaconSendingCaptureOnStateTest, aBeaconSendingCaptureOnStateSendsFinishedSessions)
{
	// with
	auto statusResponse = MockIStatusResponse::createNice();

	ON_CALL(*mockSession3Finished, sendBeacon(testing::_))
		.WillByDefault(testing::Return(statusResponse));
	ON_CALL(*mockSession3Finished, isDataSendingAllowed())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mockSession4Finished, sendBeacon(testing::_))
		.WillByDefault(testing::Return(statusResponse));
	ON_CALL(*mockSession4Finished, isDataSendingAllowed())
		.WillByDefault(testing::Return(true));

	// expect
	EXPECT_CALL(*mockSession3Finished, sendBeacon(testing::_))
		.Times(1);
	EXPECT_CALL(*mockSession4Finished, sendBeacon(testing::_))
		.Times(1);

	EXPECT_CALL(*mockContext, removeSession(testing::Eq(mockSession3Finished)))
		.Times(1);
	EXPECT_CALL(*mockContext, removeSession(testing::Eq(mockSession4Finished)))
		.Times(1);

	// given
	auto target = BeaconSendingCaptureOnState_t();

	// when
	target.execute(*mockContext);
}

TEST_F(BeaconSendingCaptureOnStateTest, aBeaconSendingCaptureOnStateClearsFinishedSessionsIfSendingIsNotAllowed)
{
	// with
	ON_CALL(*mockSession3Finished, sendBeacon(testing::_))
		.WillByDefault(testing::Return(MockIStatusResponse::createNice()));
	ON_CALL(*mockSession3Finished, isDataSendingAllowed())
		.WillByDefault(testing::Return(false));

	ON_CALL(*mockSession4Finished, sendBeacon(testing::_))
		.WillByDefault(testing::Return(MockIStatusResponse::createNice()));
	ON_CALL(*mockSession4Finished, isDataSendingAllowed())
		.WillByDefault(testing::Return(false));

	// expect
	EXPECT_CALL(*mockSession3Finished, sendBeacon(testing::_))
		.Times(0);
	EXPECT_CALL(*mockSession4Finished, sendBeacon(testing::_))
		.Times(0);

	EXPECT_CALL(*mockContext, removeSession(testing::Eq(mockSession3Finished)))
		.Times(1);
	EXPECT_CALL(*mockContext, removeSession(testing::Eq(mockSession4Finished)))
			.Times(1);

	// given
	BeaconSendingCaptureOnState_t target;

	// when
	target.execute(*mockContext);
}

TEST_F(BeaconSendingCaptureOnStateTest, aBeaconSendingCaptureOnStateDoesNotRemoveFinishedSessionIfSendWasUnsuccessful)
{
	// with
	auto statusResponse = MockIStatusResponse::createNice();
	ON_CALL(*statusResponse, getResponseCode())
		.WillByDefault(testing::Return(400));
	ON_CALL(*statusResponse, isErroneousResponse())
		.WillByDefault(testing::Return(true));

	ON_CALL(*mockSession3Finished, sendBeacon(testing::_))
		.WillByDefault(testing::Return(statusResponse));
	ON_CALL(*mockSession3Finished, isDataSendingAllowed())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mockSession3Finished, isEmpty())
		.WillByDefault(testing::Return(false));

	ON_CALL(*mockSession4Finished, sendBeacon(testing::_))
			.WillByDefault(testing::Return(statusResponse));
	ON_CALL(*mockSession4Finished, isDataSendingAllowed())
			.WillByDefault(testing::Return(true));

	// expect
	EXPECT_CALL(*mockSession3Finished, sendBeacon(testing::_))
		.Times(1);
	EXPECT_CALL(*mockSession4Finished, sendBeacon(testing::_))
		.Times(0);

	EXPECT_CALL(*mockContext, getAllFinishedAndConfiguredSessions())
		.Times(1);
	EXPECT_CALL(*mockContext, removeSession(testing::_))
		.Times(0);

	// given
	BeaconSendingCaptureOnState_t target;

	// when
	target.execute(*mockContext);
}

TEST_F(BeaconSendingCaptureOnStateTest, aBeaconSendingCaptureOnStateContinuesWithNextFinishedSessionIfSendingWasUnsuccessfulButBeaconIsEmtpy)
{
	// with
	auto erroneousStatusResponse = MockIStatusResponse::createNice();
	ON_CALL(*erroneousStatusResponse, getResponseCode())
		.WillByDefault(testing::Return(400));
	ON_CALL(*erroneousStatusResponse, isErroneousResponse())
		.WillByDefault(testing::Return(true));

	auto statusResponse = MockIStatusResponse::createNice();

	ON_CALL(*mockSession3Finished, sendBeacon(testing::_))
		.WillByDefault(testing::Return(erroneousStatusResponse));
	ON_CALL(*mockSession3Finished, isDataSendingAllowed())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mockSession3Finished, isEmpty())
		.WillByDefault(testing::Return(true));

	ON_CALL(*mockSession4Finished, sendBeacon(testing::_))
			.WillByDefault(testing::Return(statusResponse));
	ON_CALL(*mockSession4Finished, isDataSendingAllowed())
			.WillByDefault(testing::Return(true));

	// expect
	EXPECT_CALL(*mockSession3Finished, sendBeacon(testing::_))
		.Times(1);
	EXPECT_CALL(*mockSession3Finished, clearCapturedData())
		.Times(1);

	EXPECT_CALL(*mockSession4Finished, sendBeacon(testing::_))
			.Times(1);
	EXPECT_CALL(*mockSession4Finished, clearCapturedData())
		.Times(1);

	EXPECT_CALL(*mockContext, getAllFinishedAndConfiguredSessions())
		.Times(1);
	EXPECT_CALL(*mockContext, removeSession(testing::Eq(mockSession3Finished)))
		.Times(1);
	EXPECT_CALL(*mockContext, removeSession(testing::Eq(mockSession4Finished)))
		.Times(1);

	// given
	BeaconSendingCaptureOnState_t target;

	// when
	target.execute(*mockContext);
}

TEST_F(BeaconSendingCaptureOnStateTest, sendingFinishedSessionsIsAbortedImmediatelyWhenTooManyRequestsResponseIsReceived)
{
	// with
	auto beaconConfig = MockIBeaconConfiguration::createNice();
	mockSession1Open = MockSessionInternals::createStrict();
	mockSession2Open = MockSessionInternals::createStrict();
	mockSession3Finished = MockSessionInternals::createStrict();
	mockSession4Finished = MockSessionInternals::createStrict();

	std::vector<SessionInternals_sp> openSessions = {mockSession1Open, mockSession2Open};
	ON_CALL(*mockContext, getAllOpenAndConfiguredSessions())
			.WillByDefault(testing::Return(openSessions));
	std::vector<SessionInternals_sp> finishedSessions = {mockSession3Finished, mockSession4Finished};
	ON_CALL(*mockContext, getAllFinishedAndConfiguredSessions())
			.WillByDefault(testing::Return(finishedSessions));

	int64_t sleepTime = 12345;
	auto statusResponse = MockIStatusResponse::createNice();
	ON_CALL(*statusResponse, getResponseCode())
		.WillByDefault(testing::Return(429));
	ON_CALL(*statusResponse, isTooManyRequestsResponse())
			.WillByDefault(testing::Return(true));
	ON_CALL(*statusResponse, isErroneousResponse())
		.WillByDefault(testing::Return(true));
	ON_CALL(*statusResponse, getRetryAfterInMilliseconds())
		.WillByDefault(testing::Return(sleepTime));

	ON_CALL(*mockSession3Finished, sendBeacon(testing::_))
		.WillByDefault(testing::Return(statusResponse));
	ON_CALL(*mockSession3Finished, isDataSendingAllowed())
		.WillByDefault(testing::Return(true));

	ON_CALL(*mockSession4Finished, sendBeacon(testing::_))
			.WillByDefault(testing::Return(statusResponse));
	ON_CALL(*mockSession4Finished, isDataSendingAllowed())
			.WillByDefault(testing::Return(true));

	// expect
	EXPECT_CALL(*mockSession3Finished, isDataSendingAllowed())
		.Times(1);
	EXPECT_CALL(*mockSession3Finished, sendBeacon(testing::_))
			.Times(1);

	EXPECT_CALL(*mockContext, getAllFinishedAndConfiguredSessions())
		.Times(1);
	EXPECT_CALL(*mockContext, removeSession(testing::_))
		.Times(0);

	IBeaconSendingState_sp captureOffCapture = nullptr;
	EXPECT_CALL(*mockContext, setNextState(testing::_))
		.Times(testing::Exactly(1))
		.WillOnce(testing::SaveArg<0>(&captureOffCapture));

	// given
	BeaconSendingCaptureOnState_t target;

	// when
	target.execute(*mockContext);

	// then
	ASSERT_THAT(captureOffCapture, testing::NotNull());
	auto captureOffState = std::dynamic_pointer_cast<BeaconSendingCaptureOffState_t>(captureOffCapture);
	ASSERT_THAT(captureOffState, testing::NotNull());
	ASSERT_THAT(captureOffState->getSleepTimeInMilliseconds(), testing::Eq(sleepTime));
}

TEST_F(BeaconSendingCaptureOnStateTest, aBeaconSendingCaptureOnStateSendsOpenSessionsIfNotExpired)
{
	// with
	ON_CALL(*mockSession1Open, isDataSendingAllowed())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mockSession2Open, isDataSendingAllowed())
			.WillByDefault(testing::Return(true));

	// expect
	EXPECT_CALL(*mockSession1Open, sendBeacon(testing::_))
		.Times(1);
	EXPECT_CALL(*mockSession2Open, sendBeacon(testing::_))
		.Times(1);
	EXPECT_CALL(*mockContext, setLastOpenSessionBeaconSendTime(testing::_))
		.Times(1);

	// given
	BeaconSendingCaptureOnState_t target;

	// when
	target.execute(*mockContext);
}

TEST_F(BeaconSendingCaptureOnStateTest, aBeaconSendingCaptureOnStateClearsOpenSessionDataIfSendingIsNotAllowed)
{
	// with
	ON_CALL(*mockSession1Open, isDataSendingAllowed())
		.WillByDefault(testing::Return(false));
	ON_CALL(*mockSession2Open, isDataSendingAllowed())
			.WillByDefault(testing::Return(false));

	// expect
	EXPECT_CALL(*mockSession1Open, sendBeacon(testing::_))
		.Times(0);
	EXPECT_CALL(*mockSession1Open, clearCapturedData())
		.Times(1);
	EXPECT_CALL(*mockSession2Open, sendBeacon(testing::_))
		.Times(0);
	EXPECT_CALL(*mockSession2Open, clearCapturedData())
		.Times(1);
	EXPECT_CALL(*mockContext, setLastOpenSessionBeaconSendTime(testing::_))
		.Times(1);

	// given
	auto target = BeaconSendingCaptureOnState_t();

	// when
	target.execute(*mockContext);
}

TEST_F(BeaconSendingCaptureOnStateTest, sendingOpenSessionsIsAbortedImmediatelyWhenTooManyRequestsResponseIsReceived)
{
	// with
	mockSession1Open = MockSessionInternals::createStrict();
	mockSession2Open = MockSessionInternals::createStrict();
	std::vector<SessionInternals_sp> openSessions ={mockSession1Open, mockSession2Open};
	ON_CALL(*mockContext, getAllOpenAndConfiguredSessions())
			.WillByDefault(testing::Return(openSessions));

	int64_t sleepTime = 12345;
	auto statusResponse = MockIStatusResponse::createNice();
	ON_CALL(*statusResponse, getResponseCode())
		.WillByDefault(testing::Return(419));
	ON_CALL(*statusResponse, isTooManyRequestsResponse())
		.WillByDefault(testing::Return(true));
	ON_CALL(*statusResponse, isErroneousResponse())
		.WillByDefault(testing::Return(true));
	ON_CALL(*statusResponse, getRetryAfterInMilliseconds())
		.WillByDefault(testing::Return(sleepTime));

	ON_CALL(*mockSession1Open, sendBeacon(testing::_))
		.WillByDefault(testing::Return(statusResponse));
	ON_CALL(*mockSession1Open, isDataSendingAllowed())
		.WillByDefault(testing::Return(true));

	ON_CALL(*mockSession2Open, sendBeacon(testing::_))
			.WillByDefault(testing::Return(statusResponse));
	ON_CALL(*mockSession2Open, isDataSendingAllowed())
			.WillByDefault(testing::Return(true));

	// expect
	EXPECT_CALL(*mockSession1Open, sendBeacon(testing::_))
		.Times(1);
	EXPECT_CALL(*mockSession1Open, isDataSendingAllowed())
		.Times(1);

	IBeaconSendingState_sp captureOffCapture = nullptr;
	EXPECT_CALL(*mockContext, setNextState(testing::_))
		.Times(testing::Exactly(1))
		.WillOnce(testing::SaveArg<0>(&captureOffCapture));

	// given
	BeaconSendingCaptureOnState_t target;

	// when
	target.execute(*mockContext);

	// then
	ASSERT_THAT(captureOffCapture, testing::NotNull());
	auto captureOffState = std::dynamic_pointer_cast<BeaconSendingCaptureOffState_t>(captureOffCapture);
	ASSERT_THAT(captureOffState, testing::NotNull());
	ASSERT_THAT(captureOffState->getSleepTimeInMilliseconds(), testing::Eq(sleepTime));
}

TEST_F(BeaconSendingCaptureOnStateTest, nothingIsSentIfStateIsInterruptedDuringSleep)
{
	// given
	auto session1 = MockSessionInternals::createNice();
	std::vector<SessionInternals_sp> notConfiguredSessions = { session1 };

	auto session2 = MockSessionInternals::createNice();
	std::vector<SessionInternals_sp> openSessions = { session2 };

	auto session3 = MockSessionInternals::createNice();
	std::vector<SessionInternals_sp> finishedSessions = { session3 };

	//only return the two new sessions for the new session request
	ON_CALL(*mockContext, getAllNotConfiguredSessions())
		.WillByDefault(testing::Return(notConfiguredSessions));
	ON_CALL(*mockContext, getAllOpenAndConfiguredSessions())
		.WillByDefault(testing::Return(openSessions));
	ON_CALL(*mockContext, getAllFinishedAndConfiguredSessions())
		.WillByDefault(testing::Return(finishedSessions));
	ON_CALL(*mockContext, isCaptureOn())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mockContext, getCurrentTimestamp())
		.WillByDefault(testing::Return(100));
	ON_CALL(*mockContext, getSendInterval())
		.WillByDefault(testing::Return(50));
	ON_CALL(*mockContext, getLastOpenSessionBeaconSendTime())
		.WillByDefault(testing::Return(45));
	ON_CALL(*mockContext, isShutdownRequested())
		.WillByDefault(testing::Return(true));

	auto statusResponseSuccess = MockIStatusResponse::createNice();
	ON_CALL(*mockSession1Open, sendBeacon(testing::_))
		.WillByDefault(testing::Return(statusResponseSuccess));
	ON_CALL(*mockSession2Open, sendBeacon(testing::_))
		.WillByDefault(testing::Return(statusResponseSuccess));
	ON_CALL(*mockSession3Finished, sendBeacon(testing::_))
		.WillByDefault(testing::Return(statusResponseSuccess));

	auto mockHTTPClient = MockIHTTPClient::createStrict();
	ON_CALL(*mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mockHTTPClient));

	// expect
	EXPECT_CALL(*mockHTTPClient, sendNewSessionRequest())
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mockSession1Open, sendBeacon(testing::_))
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mockSession2Open, sendBeacon(testing::_))
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mockSession3Finished, sendBeacon(testing::_))
		.Times(testing::Exactly(0));

	EXPECT_CALL(*mockContext, setNextState(IsABeaconSendingFlushSessionsState()))
		.Times(testing::Exactly(1));

	//given
	BeaconSendingCaptureOnState_t target;

	// when
	target.execute(*mockContext);
}

TEST_F(BeaconSendingCaptureOnStateTest, aBeaconSendingCaptureOnStateTransitionsToCaptureOffStateWhenCapturingGotDisabled)
{
	// with
	ON_CALL(*mockContext, isCaptureOn())
			.WillByDefault(testing::Return(false));

	// then
	EXPECT_CALL(*mockContext, handleStatusResponse(testing::_))
		.Times(1);
	EXPECT_CALL(*mockContext, isCaptureOn())
		.Times(::testing::Exactly(1))
		.WillOnce(testing::Return(false));
	EXPECT_CALL(*mockContext, setNextState(IsABeaconSendingCaptureOffState()))
		.Times(testing::Exactly(1));

	// given
	BeaconSendingCaptureOnState_t target;

	// when
	target.execute(*mockContext);
}
