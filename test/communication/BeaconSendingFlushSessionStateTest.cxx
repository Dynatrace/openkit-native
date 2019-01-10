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

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "communication/BeaconSendingFlushSessionsState.h"

#include "../protocol/NullLogger.h"
#include "../communication/MockBeaconSendingContext.h"
#include "../communication/CustomMatchers.h"
#include "../protocol/MockStatusResponse.h"
#include "../core/MockSession.h"

class BeaconSendingFlushSessionsStateTest : public testing::Test
{
public:

	BeaconSendingFlushSessionsStateTest()
		: mLogger(nullptr)
		, mMockContext(nullptr)
		, mMockSession1Open(nullptr)
		, mMockSession2Open(nullptr)
		, mMockSession3Closed(nullptr)
	{
	}

	void SetUp()
	{
		mLogger = std::make_shared<NullLogger>();
		mMockSession1Open = std::make_shared<testing::NiceMock<test::MockSession>>(mLogger);
		mMockSession2Open = std::make_shared<testing::NiceMock<test::MockSession>>(mLogger);
		mMockSession3Closed = std::make_shared<testing::NiceMock<test::MockSession>>(mLogger);

		mMockContext = std::make_shared<testing::NiceMock<test::MockBeaconSendingContext>>(mLogger);
		ON_CALL(*mMockContext, getHTTPClient())
			.WillByDefault(testing::Return(mMockHTTPClient));

		ON_CALL(*mMockContext, getAllNewSessions())
			.WillByDefault(testing::Invoke(&*mMockContext, &test::MockBeaconSendingContext::RealGetAllNewSessions));
		ON_CALL(*mMockContext, getAllOpenAndConfiguredSessions())
			.WillByDefault(testing::Invoke(&*mMockContext, &test::MockBeaconSendingContext::RealGetAllOpenAndConfiguredSessions));
		ON_CALL(*mMockContext, getAllFinishedAndConfiguredSessions())
			.WillByDefault(testing::Invoke(&*mMockContext, &test::MockBeaconSendingContext::RealGetAllFinishedAndConfiguredSessions));
		ON_CALL(*mMockContext, finishSession(testing::_))
			.WillByDefault(testing::WithArgs<0>(testing::Invoke(&*mMockContext, &test::MockBeaconSendingContext::RealFinishSession)));

		std::shared_ptr<configuration::HTTPClientConfiguration> httpClientConfiguration = std::make_shared<configuration::HTTPClientConfiguration>(core::UTF8String(""), 0, core::UTF8String(""));
		mMockHTTPClient = std::shared_ptr<testing::NiceMock<test::MockHTTPClient>>(new testing::NiceMock<test::MockHTTPClient>(httpClientConfiguration));
		ON_CALL(*mMockHTTPClient, sendNewSessionRequestRawPtrProxy())
			.WillByDefault(testing::Invoke([&]() -> protocol::StatusResponse* { return new protocol::StatusResponse(mLogger, "", 200, protocol::Response::ResponseHeaders()); }));

		ON_CALL(*mMockSession1Open, sendBeaconRawPtrProxy(testing::_))
			.WillByDefault(testing::Invoke([&](std::shared_ptr<providers::IHTTPClientProvider>) -> protocol::StatusResponse* { return new protocol::StatusResponse(mLogger, "", 200, protocol::Response::ResponseHeaders()); }));
		ON_CALL(*mMockSession2Open, sendBeaconRawPtrProxy(testing::_))
			.WillByDefault(testing::Invoke([&](std::shared_ptr<providers::IHTTPClientProvider>) -> protocol::StatusResponse* { return new protocol::StatusResponse(mLogger, "", 200, protocol::Response::ResponseHeaders()); }));
		ON_CALL(*mMockSession3Closed, sendBeaconRawPtrProxy(testing::_))
			.WillByDefault(testing::Invoke([&](std::shared_ptr<providers::IHTTPClientProvider>) -> protocol::StatusResponse* { return new protocol::StatusResponse(mLogger, "", 200, protocol::Response::ResponseHeaders()); }));
		ON_CALL(*mMockSession1Open, getBeaconConfiguration())
			.WillByDefault(testing::Return(std::make_shared<configuration::BeaconConfiguration>()));
		ON_CALL(*mMockSession2Open, getBeaconConfiguration())
			.WillByDefault(testing::Return(std::make_shared<configuration::BeaconConfiguration>()));
		ON_CALL(*mMockSession3Closed, getBeaconConfiguration())
			.WillByDefault(testing::Return(std::make_shared<configuration::BeaconConfiguration>()));

		mMockContext->startSession(mMockSession1Open);
		mMockContext->startSession(mMockSession2Open);
		mMockContext->startSession(mMockSession3Closed);
		mMockContext->finishSession(mMockSession3Closed);
		
	}

	void TearDown()
	{
		mLogger = nullptr;
		mMockContext = nullptr;
		mMockSession1Open = nullptr;
		mMockSession2Open = nullptr;
		mMockSession3Closed = nullptr;
	}

	std::ostringstream devNull;
	std::shared_ptr<openkit::ILogger> mLogger;
	std::shared_ptr<testing::NiceMock<test::MockBeaconSendingContext>> mMockContext;
	std::shared_ptr<testing::NiceMock<test::MockSession>> mMockSession1Open;
	std::shared_ptr<testing::NiceMock<test::MockSession>> mMockSession2Open;
	std::shared_ptr<testing::NiceMock<test::MockSession>> mMockSession3Closed;
	std::shared_ptr<testing::NiceMock<test::MockHTTPClient>> mMockHTTPClient;
};

TEST_F(BeaconSendingFlushSessionsStateTest, aBeaconSendingFlushSessionsStateIsNotATerminalState)
{
	// given
	auto target = communication::BeaconSendingFlushSessionsState();

	// verify that BeaconSendingCaptureOffState is not a terminal state
	EXPECT_FALSE(target.isTerminalState());
}

TEST_F(BeaconSendingFlushSessionsStateTest, aBeaconSendingFlushSessionsStateHasTerminalStateBeaconSendingTerminalState)
{
	// given
	auto target = communication::BeaconSendingFlushSessionsState();
	auto terminalState = target.getShutdownState();

	// verify that terminal state is BeaconSendingTerminalState
	ASSERT_THAT(terminalState, IsABeaconSendingTerminalState());
}

TEST_F(BeaconSendingFlushSessionsStateTest, aBeaconSendingFlushSessionsStateTransitionsToTerminalStateWhenDataIsSent)
{
	// given
	auto target = communication::BeaconSendingFlushSessionsState();
	
	// verify transition to terminal state
	EXPECT_CALL(*mMockContext, setNextState(IsABeaconSendingTerminalState()))
		.Times(testing::Exactly(1));

	// when calling execute
	target.execute(*mMockContext);
}

TEST_F(BeaconSendingFlushSessionsStateTest, aBeaconSendingFlushSessionsStateRequestsNewSessionAndMulitplicity)
{
	// given
	auto target = communication::BeaconSendingFlushSessionsState();

	// verify that new sessions are handled correctly
	EXPECT_CALL(*mMockSession1Open, setBeaconConfiguration(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession2Open, setBeaconConfiguration(testing::_))
		.Times(testing::Exactly(1));

	// when calling execute
	target.execute(*mMockContext);
}

TEST_F(BeaconSendingFlushSessionsStateTest, aBeaconSendingFlushSessionsStateClosesOpenSessions)
{
	// given
	auto target = communication::BeaconSendingFlushSessionsState();

	// verify that open sessions are closed
	EXPECT_CALL(*mMockSession1Open, end())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession2Open, end())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession3Closed, end())
		.Times(testing::Exactly(0));//has already been closed

	// when calling execute
	target.execute(*mMockContext);
}

TEST_F(BeaconSendingFlushSessionsStateTest, aBeaconSendingFlushSessionStateSendsAllOpenAndClosedBeacons)
{
	// given
	auto target = communication::BeaconSendingFlushSessionsState();

	// verify that open sessions are closed
	EXPECT_CALL(*mMockSession1Open, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession2Open, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession3Closed, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(1));

	// move open sessions to finished session by calling BeaconSendinContext::finishSessions
	mMockContext->finishSession(mMockSession1Open);
	mMockContext->finishSession(mMockSession2Open);

	// when calling execute
	target.execute(*mMockContext);
}

TEST_F(BeaconSendingFlushSessionsStateTest, aBeaconSendingFlushSessionStateDoesNotSendIfSendingIsNotAllowed)
{
	//given
	auto target = communication::BeaconSendingFlushSessionsState();

	auto beaconConfiguration = std::make_shared<configuration::BeaconConfiguration>(0, openkit::DataCollectionLevel::OFF, openkit::CrashReportingLevel::OFF);

	ON_CALL(*mMockSession1Open, getBeaconConfiguration())
		.WillByDefault(testing::Return(beaconConfiguration));
	ON_CALL(*mMockSession2Open, getBeaconConfiguration())
		.WillByDefault(testing::Return(beaconConfiguration));
	ON_CALL(*mMockSession3Closed , getBeaconConfiguration())
		.WillByDefault(testing::Return(beaconConfiguration));

	//verify that session is closed without reporting data
	EXPECT_CALL(*mMockSession1Open, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mMockSession2Open, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mMockSession3Closed, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(0));

	// when calling execute
	target.execute(*mMockContext);
}

TEST_F(BeaconSendingFlushSessionsStateTest, getStateNameReturnsCorrectStateName)
{
	// given
	communication::BeaconSendingFlushSessionsState target;

	// when
	auto stateName = target.getStateName();

	// then
	ASSERT_STREQ(stateName, "FlushSessions");
}

TEST_F(BeaconSendingFlushSessionsStateTest, aBeaconSendingFlushSessionStateStopsSendingIfTooManyRequestsResponseWasReceived)
{
	//given
	auto target = communication::BeaconSendingFlushSessionsState();

	auto responseHeaders = protocol::Response::ResponseHeaders
	{
		{ "retry-after",  { "123456" } }
	};
	ON_CALL(*mMockSession1Open, sendBeaconRawPtrProxy(testing::_))
		.WillByDefault(testing::Invoke([&](std::shared_ptr<providers::IHTTPClientProvider>) -> protocol::StatusResponse* { return new protocol::StatusResponse(mLogger, "", 429, responseHeaders); }));
	ON_CALL(*mMockSession2Open, sendBeaconRawPtrProxy(testing::_))
		.WillByDefault(testing::Invoke([&](std::shared_ptr<providers::IHTTPClientProvider>) -> protocol::StatusResponse* { return new protocol::StatusResponse(mLogger, "", 429, responseHeaders); }));
	ON_CALL(*mMockSession3Closed, sendBeaconRawPtrProxy(testing::_))
		.WillByDefault(testing::Invoke([&](std::shared_ptr<providers::IHTTPClientProvider>) -> protocol::StatusResponse* { return new protocol::StatusResponse(mLogger, "", 429, responseHeaders); }));

	// verify that open sessions are closed
	EXPECT_CALL(*mMockSession1Open, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession2Open, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mMockSession3Closed, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mMockSession1Open, clearCapturedData())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession2Open, clearCapturedData())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession3Closed, clearCapturedData())
		.Times(testing::Exactly(1));

	// move open sessions to finished session by calling BeaconSendinContext::finishSessions
	mMockContext->finishSession(mMockSession1Open);
	mMockContext->finishSession(mMockSession2Open);

	// when calling execute
	target.execute(*mMockContext);
}
