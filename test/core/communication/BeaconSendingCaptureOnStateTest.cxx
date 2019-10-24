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
#include "../mock/MockSessionWrapper.h"
#include "../configuration/mock/MockIBeaconConfiguration.h"
#include "../objects/mock/MockSessionInternals.h"
#include "../../api/mock/MockILogger.h"
#include "../../protocol/mock/MockIHTTPClient.h"
#include "../../protocol/mock/MockIStatusResponse.h"
#include "../../providers/mock/MockIHTTPClientProvider.h"

#include "core/SessionWrapper.h"
#include "core/communication/BeaconSendingCaptureOffState.h"
#include "core/communication/BeaconSendingCaptureOnState.h"
#include "core/configuration/IBeaconConfiguration.h"
#include "core/objects/Session.h"
#include "protocol/StatusResponse.h"
#include "protocol/IStatusResponse.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace test;

using BeaconSendingCaptureOffState_t = core::communication::BeaconSendingCaptureOffState;
using BeaconSendingCaptureOnState_t = core::communication::BeaconSendingCaptureOnState;
using IBeaconConfiguration_sp = std::shared_ptr<core::configuration::IBeaconConfiguration>;
using IBeaconSendingState_sp = std::shared_ptr<core::communication::IBeaconSendingState>;
using IStatusResponse_t = protocol::IStatusResponse;
using MockNiceIBeaconSendingContext_sp = std::shared_ptr<MockIBeaconSendingContext>;
using MockNiceSession_t = testing::NiceMock<MockSessionInternals>;
using MockNiceSession_sp = std::shared_ptr<MockNiceSession_t>;
using MockSessionWrapper_sp = std::shared_ptr<MockSessionWrapper>;
using SessionWrapper_t = core::SessionWrapper;
using SessionWrapper_sp = std::shared_ptr<SessionWrapper_t>;
using StatusResponse_t = protocol::StatusResponse;
using IStatusResponse_sp = std::shared_ptr<IStatusResponse_t>;

class BeaconSendingCaptureOnStateTest : public testing::Test
{
protected:

	MockNiceIBeaconSendingContext_sp mockContext;
	MockSessionWrapper_sp mockSessionWrapper1Open;
	MockSessionWrapper_sp mockSessionWrapper2Open;
	MockSessionWrapper_sp mockSessionWrapper3Finished;
	MockSessionWrapper_sp mockSessionWrapper4Finished;
	MockSessionWrapper_sp mockSessionWrapper5New;
	MockSessionWrapper_sp mockSessionWrapper6New;

	void SetUp()
	{
		auto mockLogger = MockILogger::createNice();
		auto successResponse = std::make_shared<StatusResponse_t>(mockLogger, "", 200, IStatusResponse_t::ResponseHeaders());
		auto errorResponse = std::make_shared<StatusResponse_t>(mockLogger, "", 404, IStatusResponse_t::ResponseHeaders());

		mockSessionWrapper1Open = MockSessionWrapper::createNice(MockSessionInternals::createNice());
		ON_CALL(*mockSessionWrapper1Open, sendBeacon(testing::_))
			.WillByDefault(testing::Return(successResponse));
		ON_CALL(*mockSessionWrapper1Open, isDataSendingAllowed())
			.WillByDefault(testing::Return(true));

		mockSessionWrapper2Open = MockSessionWrapper::createNice(MockSessionInternals::createNice());
		ON_CALL(*mockSessionWrapper2Open, sendBeacon(testing::_))
			.WillByDefault(testing::Return(errorResponse));

		mockSessionWrapper3Finished = MockSessionWrapper::createNice(MockSessionInternals::createNice());
		mockSessionWrapper4Finished = MockSessionWrapper::createNice(MockSessionInternals::createNice());
		mockSessionWrapper5New = MockSessionWrapper::createNice(MockSessionInternals::createNice());
		mockSessionWrapper6New = MockSessionWrapper::createNice(MockSessionInternals::createNice());


		auto mockHTTPClientProvider = MockIHTTPClientProvider::createNice();

		mockContext = MockIBeaconSendingContext::createNice();
		ON_CALL(*mockContext, getCurrentTimestamp())
			.WillByDefault(testing::Return(42L));
		ON_CALL(*mockContext, getHTTPClientProvider())
			.WillByDefault(testing::Return(mockHTTPClientProvider));

		std::vector<SessionWrapper_sp> openSessions = {mockSessionWrapper1Open, mockSessionWrapper2Open};
		ON_CALL(*mockContext, getAllOpenAndConfiguredSessions())
			.WillByDefault(testing::Return(openSessions));

		std::vector<SessionWrapper_sp> finishedSessions = {mockSessionWrapper3Finished, mockSessionWrapper4Finished};
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

TEST_F(BeaconSendingCaptureOnStateTest, newSessionRequestsAreMadeForAllNewSessions)
{
	// with
	auto mockClient = MockIHTTPClient::createNice();
	std::vector<SessionWrapper_sp> newSessions = {mockSessionWrapper5New, mockSessionWrapper6New};
	ON_CALL(*mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mockClient));
	ON_CALL(*mockContext, getAllNewSessions())
		.WillByDefault(testing::Return(newSessions));

	ON_CALL(*mockSessionWrapper5New, getBeaconConfiguration())
		.WillByDefault(testing::Return(MockIBeaconConfiguration::createNice()));

	auto mockLogger = MockILogger::createNice();
	EXPECT_CALL(*mockClient, sendNewSessionRequest())
		.Times(testing::Exactly(2))
		.WillOnce(testing::Return(std::make_shared<StatusResponse_t>(mockLogger, "mp=5", 200, IStatusResponse_t::ResponseHeaders())))
		.WillOnce(testing::Return(std::make_shared<StatusResponse_t>(mockLogger, "", 400, IStatusResponse_t::ResponseHeaders())))
	;

	ON_CALL(*mockSessionWrapper5New, canSendNewSessionRequest())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mockSessionWrapper6New, canSendNewSessionRequest())
		.WillByDefault(testing::Return(true));

	// expect
	IBeaconConfiguration_sp beaconConfigCapture = nullptr;
	EXPECT_CALL(*mockSessionWrapper5New, updateBeaconConfiguration(testing::_))
		.Times(testing::Exactly(1))
		.WillOnce(testing::SaveArg<0>(&beaconConfigCapture));
	EXPECT_CALL(*mockSessionWrapper6New, decreaseNumberOfNewSessionRequests())
		.Times(1);

	// given
	BeaconSendingCaptureOnState_t target;

	// when
	target.execute(*mockContext);

	// then
	ASSERT_THAT(beaconConfigCapture, testing::NotNull());
	ASSERT_THAT(beaconConfigCapture->getMultiplicity(), testing::Eq(5));
}

TEST_F(BeaconSendingCaptureOnStateTest, multiplicityIsSetToZeroIfNoFurtherNewSessionRequestsAreAllowed)
{
	// with
	auto mockClient = MockIHTTPClient::createNice();
	std::vector<SessionWrapper_sp> newSessions = {mockSessionWrapper5New, mockSessionWrapper6New};
	ON_CALL(*mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mockClient));
	ON_CALL(*mockContext, getAllNewSessions())
		.WillByDefault(testing::Return(newSessions));

	auto mockDefaultConfig = MockIBeaconConfiguration::createNice();
	ON_CALL(*mockSessionWrapper5New, canSendNewSessionRequest())
		.WillByDefault(testing::Return(false));
	ON_CALL(*mockSessionWrapper5New, getBeaconConfiguration())
		.WillByDefault(testing::Return(mockDefaultConfig));
	ON_CALL(*mockSessionWrapper6New, canSendNewSessionRequest())
		.WillByDefault(testing::Return(false));
	ON_CALL(*mockSessionWrapper6New, getBeaconConfiguration())
		.WillByDefault(testing::Return(mockDefaultConfig));

	// expect
	EXPECT_CALL(*mockClient, sendNewSessionRequest())
		.Times(0);

	IBeaconConfiguration_sp beaconConfigSession5Capture = nullptr;
	EXPECT_CALL(*mockSessionWrapper5New, updateBeaconConfiguration(testing::_))
		.Times(testing::Exactly(1))
		.WillOnce(testing::SaveArg<0>(&beaconConfigSession5Capture));

	IBeaconConfiguration_sp beaconConfigSession6Capture = nullptr;
	EXPECT_CALL(*mockSessionWrapper6New, updateBeaconConfiguration(testing::_))
		.Times(testing::Exactly(1))
		.WillOnce(testing::SaveArg<0>(&beaconConfigSession6Capture));


	// given
	BeaconSendingCaptureOnState_t target;

	// when
	target.execute(*mockContext);

	// then
	ASSERT_THAT(beaconConfigSession5Capture, testing::NotNull());
	ASSERT_THAT(beaconConfigSession5Capture->getMultiplicity(), testing::Eq(0));

	ASSERT_THAT(beaconConfigSession6Capture, testing::NotNull());
	ASSERT_THAT(beaconConfigSession6Capture->getMultiplicity(), testing::Eq(0));
}

TEST_F(BeaconSendingCaptureOnStateTest, newSessionRequestsAreAbortedWhenTooManyRequestsResponseIsReceived)
{
	// with
	mockSessionWrapper1Open = MockSessionWrapper::createStrict(MockSessionInternals::createStrict());
	mockSessionWrapper2Open = MockSessionWrapper::createStrict(MockSessionInternals::createStrict());
	mockSessionWrapper3Finished = MockSessionWrapper::createStrict(MockSessionInternals::createStrict());
	mockSessionWrapper4Finished = MockSessionWrapper::createStrict(MockSessionInternals::createStrict());
	mockSessionWrapper5New = MockSessionWrapper::createStrict(MockSessionInternals::createStrict());
	mockSessionWrapper6New = MockSessionWrapper::createStrict(MockSessionInternals::createStrict());
	std::vector<SessionWrapper_sp> newSessions = {mockSessionWrapper5New, mockSessionWrapper6New};
	ON_CALL(*mockContext, getAllNewSessions())
		.WillByDefault(testing::Return(newSessions));
	std::vector<SessionWrapper_sp> openSessions ={mockSessionWrapper1Open, mockSessionWrapper2Open};
	ON_CALL(*mockContext, getAllOpenAndConfiguredSessions())
		.WillByDefault(testing::Return(openSessions));
	std::vector<SessionWrapper_sp> finishedSessions = {mockSessionWrapper3Finished, mockSessionWrapper4Finished};
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
	EXPECT_CALL(*mockSessionWrapper5New, canSendNewSessionRequest())
		.Times(testing::Exactly(1))
		.WillOnce(testing::Return(true));

	EXPECT_CALL(*mockSessionWrapper6New, canSendNewSessionRequest())
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

	ON_CALL(*mockSessionWrapper3Finished, sendBeacon(testing::_))
		.WillByDefault(testing::Return(statusResponse));
	ON_CALL(*mockSessionWrapper3Finished, isDataSendingAllowed())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mockSessionWrapper4Finished, sendBeacon(testing::_))
		.WillByDefault(testing::Return(statusResponse));
	ON_CALL(*mockSessionWrapper4Finished, isDataSendingAllowed())
		.WillByDefault(testing::Return(true));

	// expect
	EXPECT_CALL(*mockSessionWrapper3Finished, sendBeacon(testing::_))
		.Times(1);
	EXPECT_CALL(*mockSessionWrapper4Finished, sendBeacon(testing::_))
		.Times(1);

	EXPECT_CALL(*mockContext, removeSession(testing::Eq(mockSessionWrapper3Finished)))
		.Times(1);
	EXPECT_CALL(*mockContext, removeSession(testing::Eq(mockSessionWrapper4Finished)))
		.Times(1);

	// given
	auto target = BeaconSendingCaptureOnState_t();

	// when
	target.execute(*mockContext);
}

TEST_F(BeaconSendingCaptureOnStateTest, aBeaconSendingCaptureOnStateClearsFinishedSessionsIfSendingIsNotAllowed)
{
	// with
	ON_CALL(*mockSessionWrapper3Finished, sendBeacon(testing::_))
		.WillByDefault(testing::Return(MockIStatusResponse::createNice()));
	ON_CALL(*mockSessionWrapper3Finished, isDataSendingAllowed())
		.WillByDefault(testing::Return(false));

	ON_CALL(*mockSessionWrapper4Finished, sendBeacon(testing::_))
		.WillByDefault(testing::Return(MockIStatusResponse::createNice()));
	ON_CALL(*mockSessionWrapper4Finished, isDataSendingAllowed())
		.WillByDefault(testing::Return(false));

	// expect
	EXPECT_CALL(*mockSessionWrapper3Finished, sendBeacon(testing::_))
		.Times(0);
	EXPECT_CALL(*mockSessionWrapper4Finished, sendBeacon(testing::_))
		.Times(0);

	EXPECT_CALL(*mockContext, removeSession(testing::Eq(mockSessionWrapper3Finished)))
		.Times(1);
	EXPECT_CALL(*mockContext, removeSession(testing::Eq(mockSessionWrapper4Finished)))
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

	ON_CALL(*mockSessionWrapper3Finished, sendBeacon(testing::_))
		.WillByDefault(testing::Return(statusResponse));
	ON_CALL(*mockSessionWrapper3Finished, isDataSendingAllowed())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mockSessionWrapper3Finished, isEmpty())
		.WillByDefault(testing::Return(false));

	ON_CALL(*mockSessionWrapper4Finished, sendBeacon(testing::_))
			.WillByDefault(testing::Return(statusResponse));
	ON_CALL(*mockSessionWrapper4Finished, isDataSendingAllowed())
			.WillByDefault(testing::Return(true));

	// expect
	EXPECT_CALL(*mockSessionWrapper3Finished, sendBeacon(testing::_))
		.Times(1);
	EXPECT_CALL(*mockSessionWrapper4Finished, sendBeacon(testing::_))
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

	ON_CALL(*mockSessionWrapper3Finished, sendBeacon(testing::_))
		.WillByDefault(testing::Return(erroneousStatusResponse));
	ON_CALL(*mockSessionWrapper3Finished, isDataSendingAllowed())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mockSessionWrapper3Finished, isEmpty())
		.WillByDefault(testing::Return(true));

	ON_CALL(*mockSessionWrapper4Finished, sendBeacon(testing::_))
			.WillByDefault(testing::Return(statusResponse));
	ON_CALL(*mockSessionWrapper4Finished, isDataSendingAllowed())
			.WillByDefault(testing::Return(true));

	// expect
	EXPECT_CALL(*mockSessionWrapper3Finished, sendBeacon(testing::_))
		.Times(1);
	EXPECT_CALL(*mockSessionWrapper3Finished, clearCapturedData())
		.Times(1);

	EXPECT_CALL(*mockSessionWrapper4Finished, sendBeacon(testing::_))
			.Times(1);
	EXPECT_CALL(*mockSessionWrapper4Finished, clearCapturedData())
		.Times(1);

	EXPECT_CALL(*mockContext, getAllFinishedAndConfiguredSessions())
		.Times(1);
	EXPECT_CALL(*mockContext, removeSession(testing::Eq(mockSessionWrapper3Finished)))
		.Times(1);
	EXPECT_CALL(*mockContext, removeSession(testing::Eq(mockSessionWrapper4Finished)))
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
	mockSessionWrapper1Open = MockSessionWrapper::createStrict(MockSessionInternals::createStrict());
	mockSessionWrapper2Open = MockSessionWrapper::createStrict(MockSessionInternals::createStrict());
	mockSessionWrapper3Finished = MockSessionWrapper::createStrict(MockSessionInternals::createStrict());
	mockSessionWrapper4Finished = MockSessionWrapper::createStrict(MockSessionInternals::createStrict());

	std::vector<SessionWrapper_sp> openSessions = {mockSessionWrapper1Open, mockSessionWrapper2Open};
	ON_CALL(*mockContext, getAllOpenAndConfiguredSessions())
			.WillByDefault(testing::Return(openSessions));
	std::vector<SessionWrapper_sp> finishedSessions = {mockSessionWrapper3Finished, mockSessionWrapper4Finished};
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

	ON_CALL(*mockSessionWrapper3Finished, sendBeacon(testing::_))
		.WillByDefault(testing::Return(statusResponse));
	ON_CALL(*mockSessionWrapper3Finished, isDataSendingAllowed())
		.WillByDefault(testing::Return(true));

	ON_CALL(*mockSessionWrapper4Finished, sendBeacon(testing::_))
			.WillByDefault(testing::Return(statusResponse));
	ON_CALL(*mockSessionWrapper4Finished, isDataSendingAllowed())
			.WillByDefault(testing::Return(true));

	// expect
	EXPECT_CALL(*mockSessionWrapper3Finished, isDataSendingAllowed())
		.Times(1);
	EXPECT_CALL(*mockSessionWrapper3Finished, sendBeacon(testing::_))
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
	ON_CALL(*mockSessionWrapper1Open, isDataSendingAllowed())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mockSessionWrapper2Open, isDataSendingAllowed())
			.WillByDefault(testing::Return(true));

	// expect
	EXPECT_CALL(*mockSessionWrapper1Open, sendBeacon(testing::_))
		.Times(1);
	EXPECT_CALL(*mockSessionWrapper2Open, sendBeacon(testing::_))
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
	ON_CALL(*mockSessionWrapper1Open, isDataSendingAllowed())
		.WillByDefault(testing::Return(false));
	ON_CALL(*mockSessionWrapper2Open, isDataSendingAllowed())
			.WillByDefault(testing::Return(false));

	// expect
	EXPECT_CALL(*mockSessionWrapper1Open, sendBeacon(testing::_))
		.Times(0);
	EXPECT_CALL(*mockSessionWrapper1Open, clearCapturedData())
		.Times(1);
	EXPECT_CALL(*mockSessionWrapper2Open, sendBeacon(testing::_))
		.Times(0);
	EXPECT_CALL(*mockSessionWrapper2Open, clearCapturedData())
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
	mockSessionWrapper1Open = MockSessionWrapper::createStrict(MockSessionInternals::createStrict());
	mockSessionWrapper2Open = MockSessionWrapper::createStrict(MockSessionInternals::createStrict());
	std::vector<SessionWrapper_sp> openSessions ={mockSessionWrapper1Open, mockSessionWrapper2Open};
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

	ON_CALL(*mockSessionWrapper1Open, sendBeacon(testing::_))
		.WillByDefault(testing::Return(statusResponse));
	ON_CALL(*mockSessionWrapper1Open, isDataSendingAllowed())
		.WillByDefault(testing::Return(true));

	ON_CALL(*mockSessionWrapper2Open, sendBeacon(testing::_))
			.WillByDefault(testing::Return(statusResponse));
	ON_CALL(*mockSessionWrapper2Open, isDataSendingAllowed())
			.WillByDefault(testing::Return(true));

	// expect
	EXPECT_CALL(*mockSessionWrapper1Open, sendBeacon(testing::_))
		.Times(1);
	EXPECT_CALL(*mockSessionWrapper1Open, isDataSendingAllowed())
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
	auto mockBeaconConfig = MockIBeaconConfiguration::createNice();
	ON_CALL(*mockBeaconConfig, getMultiplicity())
		.WillByDefault(testing::Return(2));

	auto sessionWrapper1 = std::make_shared<SessionWrapper_t>(MockSessionInternals::createNice());
	std::vector<SessionWrapper_sp> newSessions = { sessionWrapper1 };
	auto sessionWrapper2 = std::make_shared<SessionWrapper_t>(MockSessionInternals::createNice());
	sessionWrapper2->updateBeaconConfiguration(mockBeaconConfig);
	std::vector<SessionWrapper_sp> openSessions = { sessionWrapper2 };
	auto sessionWrapper3 = std::make_shared<SessionWrapper_t>(MockSessionInternals::createNice());
	sessionWrapper3->updateBeaconConfiguration(MockIBeaconConfiguration::createNice());
	std::vector<SessionWrapper_sp> finishedSessions = { sessionWrapper3 };

	//only return the two new sessions for the new session request
	ON_CALL(*mockContext, getAllNewSessions())
		.WillByDefault(testing::Return(newSessions));
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
	ON_CALL(*mockSessionWrapper1Open, sendBeacon(testing::_))
		.WillByDefault(testing::Return(statusResponseSuccess));
	ON_CALL(*mockSessionWrapper2Open, sendBeacon(testing::_))
		.WillByDefault(testing::Return(statusResponseSuccess));
	ON_CALL(*mockSessionWrapper3Finished, sendBeacon(testing::_))
		.WillByDefault(testing::Return(statusResponseSuccess));

	auto mockHTTPClient = MockIHTTPClient::createStrict();
	ON_CALL(*mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mockHTTPClient));

	// expect
	EXPECT_CALL(*mockHTTPClient, sendNewSessionRequest())
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mockSessionWrapper1Open, sendBeacon(testing::_))
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mockSessionWrapper2Open, sendBeacon(testing::_))
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mockSessionWrapper3Finished, sendBeacon(testing::_))
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
