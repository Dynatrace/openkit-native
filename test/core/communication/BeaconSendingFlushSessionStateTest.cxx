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
#include "../../api/mock/MockILogger.h"
#include "../../core/configuration/mock/MockIBeaconConfiguration.h"
#include "../../core/mock/MockSessionWrapper.h"
#include "../../core/objects/MockSession.h"
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
using MockNiceSession_t = testing::NiceMock<MockSession>;
using MockNiceSession_sp = std::shared_ptr<MockNiceSession_t>;
using MockNiceSessionWrapper_sp = std::shared_ptr<testing::NiceMock<MockSessionWrapper>>;
using SessionWrapper_sp = std::shared_ptr<core::SessionWrapper>;

class BeaconSendingFlushSessionsStateTest : public testing::Test
{
protected:

	MockNiceIBeaconSendingContext_sp mockContext;
	MockNiceSession_sp mockSession1Open;
	MockNiceSessionWrapper_sp mockSessionWrapper1Open;
	MockNiceSession_sp mockSession2Open;
	MockNiceSessionWrapper_sp mockSessionWrapper2Open;
	MockNiceSession_sp mockSession3Closed;
	MockNiceSessionWrapper_sp mockSessionWrapper3Closed;

	void SetUp()
	{
		auto mockLogger = MockILogger::createNice();
		mockSession1Open = std::make_shared<MockNiceSession_t>(mockLogger);
		mockSessionWrapper1Open = MockSessionWrapper::createNice(mockSession1Open);
		ON_CALL(*mockSessionWrapper1Open, isDataSendingAllowed())
			.WillByDefault(testing::Return(true));

		mockSession2Open = std::make_shared<MockNiceSession_t>(mockLogger);
		mockSessionWrapper2Open = MockSessionWrapper::createNice(mockSession2Open);
		ON_CALL(*mockSessionWrapper2Open, isDataSendingAllowed())
				.WillByDefault(testing::Return(true));

		mockSession3Closed = std::make_shared<MockNiceSession_t>(mockLogger);
		mockSessionWrapper3Closed = MockSessionWrapper::createNice(mockSession3Closed);
		ON_CALL(*mockSessionWrapper3Closed, isDataSendingAllowed())
				.WillByDefault(testing::Return(true));

		mockContext = MockIBeaconSendingContext::createNice();

		auto mockStatusResponse = MockIStatusResponse::createNice();
		ON_CALL(*mockSessionWrapper1Open, sendBeacon(testing::_))
			.WillByDefault(testing::Return(mockStatusResponse));
		ON_CALL(*mockSessionWrapper2Open, sendBeacon(testing::_))
				.WillByDefault(testing::Return(mockStatusResponse));
		ON_CALL(*mockSessionWrapper3Closed, sendBeacon(testing::_))
				.WillByDefault(testing::Return(mockStatusResponse));

		auto mockHTTPClient = MockIHTTPClient::createNice();
		ON_CALL(*mockContext, getHTTPClient())
			.WillByDefault(testing::Return(mockHTTPClient));

		std::vector<SessionWrapper_sp> openSession = {mockSessionWrapper1Open, mockSessionWrapper2Open};
		ON_CALL(*mockContext, getAllOpenAndConfiguredSessions())
			.WillByDefault(testing::Return(openSession));

		std::vector<SessionWrapper_sp> finishedSessions = {mockSessionWrapper3Closed, mockSessionWrapper2Open, mockSessionWrapper1Open};
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

TEST_F(BeaconSendingFlushSessionsStateTest, aBeaconSendingFlushSessionsStateRequestsNewSessionAndMultitplicity)
{
	// with
	auto mockBeaconConfig = MockIBeaconConfiguration::createNice();
	ON_CALL(*mockSessionWrapper1Open, getBeaconConfiguration())
		.WillByDefault(testing::Return(mockBeaconConfig));
	ON_CALL(*mockSessionWrapper2Open, getBeaconConfiguration())
		.WillByDefault(testing::Return(mockBeaconConfig));

	std::vector<SessionWrapper_sp> newSessions = {mockSessionWrapper1Open, mockSessionWrapper2Open};
	ON_CALL(*mockContext, getAllNewSessions())
		.WillByDefault(testing::Return(newSessions));
	ON_CALL(*mockContext, getAllOpenAndConfiguredSessions())
		.WillByDefault(testing::Return(std::vector<SessionWrapper_sp>()));
	ON_CALL(*mockContext, getAllFinishedAndConfiguredSessions())
			.WillByDefault(testing::Return(std::vector<SessionWrapper_sp>()));

	// verify that new sessions are handled correctly
	EXPECT_CALL(*mockSessionWrapper1Open, updateBeaconConfiguration(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockSessionWrapper2Open, updateBeaconConfiguration(testing::_))
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
	EXPECT_CALL(*mockSession1Open, end())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockSession2Open, end())
		.Times(testing::Exactly(1));

	// given
	BeaconSendingFlushSessionState_t target;

	// when
	target.execute(*mockContext);
}

TEST_F(BeaconSendingFlushSessionsStateTest, aBeaconSendingFlushSessionStateSendsAllOpenAndClosedBeacons)
{
	// expect
	EXPECT_CALL(*mockSessionWrapper1Open, sendBeacon(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockSessionWrapper2Open, sendBeacon(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockSessionWrapper3Closed, sendBeacon(testing::_))
		.Times(testing::Exactly(1));

	// given
	BeaconSendingFlushSessionState_t target;

	// when calling execute
	target.execute(*mockContext);
}

TEST_F(BeaconSendingFlushSessionsStateTest, aBeaconSendingFlushSessionStateDoesNotSendIfSendingIsNotAllowed)
{
	// with
	ON_CALL(*mockSessionWrapper1Open, isDataSendingAllowed())
		.WillByDefault(testing::Return(false));
	ON_CALL(*mockSessionWrapper2Open, isDataSendingAllowed())
		.WillByDefault(testing::Return(false));
	ON_CALL(*mockSessionWrapper3Closed, isDataSendingAllowed())
		.WillByDefault(testing::Return(false));

	// expect
	EXPECT_CALL(*mockSessionWrapper1Open, sendBeacon(testing::_))
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mockSessionWrapper1Open, clearCapturedData())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockSessionWrapper2Open, sendBeacon(testing::_))
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mockSessionWrapper2Open, clearCapturedData())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockSessionWrapper3Closed, sendBeacon(testing::_))
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mockSessionWrapper3Closed, clearCapturedData())
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

	ON_CALL(*mockSessionWrapper3Closed, sendBeacon(testing::_))
		.WillByDefault(testing::Return(errorResponse));

	// expect
	EXPECT_CALL(*mockSessionWrapper1Open, sendBeacon(testing::_))
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mockSessionWrapper1Open, clearCapturedData())
			.Times(testing::Exactly(1));
	EXPECT_CALL(*mockSessionWrapper2Open, sendBeacon(testing::_))
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mockSessionWrapper2Open, clearCapturedData())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockSessionWrapper3Closed, sendBeacon(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockSessionWrapper3Closed, clearCapturedData())
		.Times(testing::Exactly(1));

	// given
	auto target = BeaconSendingFlushSessionState_t();

	// when
	target.execute(*mockContext);
}
