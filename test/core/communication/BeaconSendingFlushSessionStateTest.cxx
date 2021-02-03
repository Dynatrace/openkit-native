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

#include "CustomMatchers.h"
#include "mock/MockIBeaconSendingContext.h"
#include "../configuration/mock/MockIBeaconConfiguration.h"
#include "../objects/mock/MockSessionInternals.h"
#include "../../protocol/mock/MockIHTTPClient.h"
#include "../../protocol/mock/MockIStatusResponse.h"

#include "core/communication/BeaconSendingFlushSessionsState.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <vector>

using namespace test;

using BeaconSendingFlushSessionState_t = core::communication::BeaconSendingFlushSessionsState;
using MockNiceIBeaconSendingContext_sp = std::shared_ptr<testing::NiceMock<MockIBeaconSendingContext>>;
using MockNiceIHTTPClient_sp = std::shared_ptr<testing::NiceMock<MockIHTTPClient>>;
using MockSession_sp = std::shared_ptr<MockSessionInternals>;
using SessionInternals_sp = std::shared_ptr<core::objects::SessionInternals>;

class BeaconSendingFlushSessionsStateTest : public testing::Test
{
protected:

	MockNiceIBeaconSendingContext_sp mockContext;
	MockSession_sp mockSession1Open;
	MockSession_sp mockSession2Open;
	MockSession_sp mockSession3Closed;

	void SetUp() override
	{
		mockSession1Open = MockSessionInternals::createNice();
		ON_CALL(*mockSession1Open, isDataSendingAllowed())
			.WillByDefault(testing::Return(true));

		mockSession2Open = MockSessionInternals::createNice();
		ON_CALL(*mockSession2Open, isDataSendingAllowed())
				.WillByDefault(testing::Return(true));

		mockSession3Closed = MockSessionInternals::createNice();
		ON_CALL(*mockSession3Closed, isDataSendingAllowed())
				.WillByDefault(testing::Return(true));

		mockContext = MockIBeaconSendingContext::createNice();

		auto mockStatusResponse = MockIStatusResponse::createNice();
		ON_CALL(*mockSession1Open, sendBeacon(testing::_, testing::_))
			.WillByDefault(testing::Return(mockStatusResponse));
		ON_CALL(*mockSession2Open, sendBeacon(testing::_, testing::_))
				.WillByDefault(testing::Return(mockStatusResponse));
		ON_CALL(*mockSession3Closed, sendBeacon(testing::_, testing::_))
				.WillByDefault(testing::Return(mockStatusResponse));

		auto mockHTTPClient = MockIHTTPClient::createNice();
		ON_CALL(*mockContext, getHTTPClient())
			.WillByDefault(testing::Return(mockHTTPClient));

		std::vector<SessionInternals_sp> openSession = {mockSession1Open, mockSession2Open};
		ON_CALL(*mockContext, getAllOpenAndConfiguredSessions())
			.WillByDefault(testing::Return(openSession));

		std::vector<SessionInternals_sp> finishedSessions = {mockSession3Closed, mockSession2Open, mockSession1Open};
		ON_CALL(*mockContext, getAllFinishedAndConfiguredSessions())
			.WillByDefault(testing::Return(finishedSessions));
	}
};


TEST_F(BeaconSendingFlushSessionsStateTest, getStateNameReturnsCorrectStateName)
{
	// given
	BeaconSendingFlushSessionState_t target;

	// when
	auto stateName = target.getStateName();

	// then
	ASSERT_STREQ(stateName, "FlushSessions");
}

TEST_F(BeaconSendingFlushSessionsStateTest, aBeaconSendingFlushSessionsStateIsNotATerminalState)
{
	// given
	BeaconSendingFlushSessionState_t target;

	// verify that BeaconSendingCaptureOffState is not a terminal state
	EXPECT_FALSE(target.isTerminalState());
}

TEST_F(BeaconSendingFlushSessionsStateTest, aBeaconSendingFlushSessionsStateHasTerminalStateBeaconSendingTerminalState)
{
	// given
	BeaconSendingFlushSessionState_t target;
	auto terminalState = target.getShutdownState();

	// verify that terminal state is BeaconSendingTerminalState
	ASSERT_THAT(terminalState, IsABeaconSendingTerminalState());
}

TEST_F(BeaconSendingFlushSessionsStateTest, aBeaconSendingFlushSessionsStateTransitionsToTerminalStateWhenDataIsSent)
{
	// expect
	EXPECT_CALL(*mockContext, setNextState(IsABeaconSendingTerminalState()))
		.Times(testing::Exactly(1));

	// given
	BeaconSendingFlushSessionState_t target;

	// when
	target.execute(*mockContext);
}

TEST_F(BeaconSendingFlushSessionsStateTest, aBeaconSendingFlushSessionsStateEnalbesCaptureForNotConfiguredSessions)
{
	// with
	std::vector<SessionInternals_sp> notConfigured = {mockSession1Open, mockSession2Open};
	ON_CALL(*mockContext, getAllNotConfiguredSessions())
		.WillByDefault(testing::Return(notConfigured));
	ON_CALL(*mockContext, getAllOpenAndConfiguredSessions())
		.WillByDefault(testing::Return(std::vector<SessionInternals_sp>()));
	ON_CALL(*mockContext, getAllFinishedAndConfiguredSessions())
			.WillByDefault(testing::Return(std::vector<SessionInternals_sp>()));

	// verify that new sessions are handled correctly
	EXPECT_CALL(*mockSession1Open, enableCapture())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockSession2Open, enableCapture())
		.Times(testing::Exactly(1));

	// given
	BeaconSendingFlushSessionState_t target;

	// when calling execute
	target.execute(*mockContext);
}

TEST_F(BeaconSendingFlushSessionsStateTest, aBeaconSendingFlushSessionsStateClosesOpenSessions)
{
	// expect
	// verify that open sessions are closed
	EXPECT_CALL(*mockSession1Open, end(false))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockSession2Open, end(false))
		.Times(testing::Exactly(1));

	// given
	BeaconSendingFlushSessionState_t target;

	// when
	target.execute(*mockContext);
}

TEST_F(BeaconSendingFlushSessionsStateTest, aBeaconSendingFlushSessionStateSendsAllOpenAndClosedBeacons)
{
	// expect
	EXPECT_CALL(*mockSession1Open, sendBeacon(testing::_, testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockSession2Open, sendBeacon(testing::_, testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockSession3Closed, sendBeacon(testing::_, testing::_))
		.Times(testing::Exactly(1));

	// given
	BeaconSendingFlushSessionState_t target;

	// when calling execute
	target.execute(*mockContext);
}

TEST_F(BeaconSendingFlushSessionsStateTest, aBeaconSendingFlushSessionStateDoesNotSendIfSendingIsNotAllowed)
{
	// with
	ON_CALL(*mockSession1Open, isDataSendingAllowed())
		.WillByDefault(testing::Return(false));
	ON_CALL(*mockSession2Open, isDataSendingAllowed())
		.WillByDefault(testing::Return(false));
	ON_CALL(*mockSession3Closed, isDataSendingAllowed())
		.WillByDefault(testing::Return(false));

	// expect
	EXPECT_CALL(*mockSession1Open, sendBeacon(testing::_, testing::_))
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mockSession1Open, clearCapturedData())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockSession2Open, sendBeacon(testing::_, testing::_))
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mockSession2Open, clearCapturedData())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockSession3Closed, sendBeacon(testing::_, testing::_))
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mockSession3Closed, clearCapturedData())
		.Times(testing::Exactly(1));

	//given
	BeaconSendingFlushSessionState_t target;

	// when
	target.execute(*mockContext);
}

TEST_F(BeaconSendingFlushSessionsStateTest, aBeaconSendingFlushSessionStateStopsSendingIfTooManyRequestsResponseWasReceived)
{
	// with
	auto errorResponse = MockIStatusResponse::createNice();
	ON_CALL(*errorResponse, getResponseCode())
		.WillByDefault(testing::Return(429));
	ON_CALL(*errorResponse, isTooManyRequestsResponse())
			.WillByDefault(testing::Return(true));
	ON_CALL(*errorResponse, isErroneousResponse())
		.WillByDefault(testing::Return(true));

	ON_CALL(*mockSession3Closed, sendBeacon(testing::_, testing::_))
		.WillByDefault(testing::Return(errorResponse));

	// expect
	EXPECT_CALL(*mockSession1Open, sendBeacon(testing::_, testing::_))
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mockSession1Open, clearCapturedData())
			.Times(testing::Exactly(1));
	EXPECT_CALL(*mockSession2Open, sendBeacon(testing::_, testing::_))
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mockSession2Open, clearCapturedData())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockSession3Closed, sendBeacon(testing::_, testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockSession3Closed, clearCapturedData())
		.Times(testing::Exactly(1));

	// given
	auto target = BeaconSendingFlushSessionState_t();

	// when
	target.execute(*mockContext);
}
