/**
* Copyright 2018 Dynatrace LLC
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

#include "../communication/MockBeaconSendingContext.h"
#include "../communication/CustomMatchers.h"
#include "../protocol/MockStatusResponse.h"
#include "../core/MockSession.h"

class BeaconSendingFlushSessionsStateTest : public testing::Test
{
public:

	BeaconSendingFlushSessionsStateTest()
		: mLogger(nullptr)
		, mCallCount(0)
		, mMockContext(nullptr)
		, mMockSession1Open(nullptr)
		, mMockSession2Open(nullptr)
		, mMockSession3Closed(nullptr)
		, mMockedNewSessions()
	{
	}

	void SetUp()
	{
		mLogger = std::shared_ptr<openkit::ILogger>(new core::util::DefaultLogger(devNull, true));
		mMockSession1Open = std::shared_ptr<testing::NiceMock<test::MockSession>>(new testing::NiceMock<test::MockSession>(mLogger));
		std::shared_ptr<core::SessionWrapper> sessionWrapper1 = std::make_shared<core::SessionWrapper>(mMockSession1Open);
		mMockSession2Open = std::shared_ptr<testing::NiceMock<test::MockSession>>(new testing::NiceMock<test::MockSession>(mLogger));
		std::shared_ptr<core::SessionWrapper> sessionWrapper2 = std::make_shared<core::SessionWrapper>(mMockSession2Open);
		mMockedNewSessions.push_back(sessionWrapper1);
		mMockedNewSessions.push_back(sessionWrapper2);

		mMockSession3Closed = std::shared_ptr<testing::NiceMock<test::MockSession>>(new testing::NiceMock<test::MockSession>(mLogger));
		std::shared_ptr<core::SessionWrapper> sessionWrapper3 = std::make_shared<core::SessionWrapper>(mMockSession3Closed);
		mMockedFinishedSessions.push_back(sessionWrapper3);

		std::shared_ptr<configuration::HTTPClientConfiguration> httpClientConfiguration = std::make_shared<configuration::HTTPClientConfiguration>(core::UTF8String(""), 0, core::UTF8String(""));
		mMockHTTPClient = std::shared_ptr<testing::NiceMock<test::MockHTTPClient>>(new testing::NiceMock<test::MockHTTPClient>(httpClientConfiguration));
		ON_CALL(*mMockHTTPClient, sendStatusRequestRawPtrProxy())
			.WillByDefault(testing::Return(new protocol::StatusResponse()));

		mMockContext = std::shared_ptr<testing::NiceMock<test::MockBeaconSendingContext>>(new testing::NiceMock<test::MockBeaconSendingContext>(mLogger));
		ON_CALL(*mMockContext, getHTTPClient())
			.WillByDefault(testing::Return(mMockHTTPClient));
		ON_CALL(*mMockContext, getAllNewSessions())
			.WillByDefault(testing::Return(mMockedNewSessions));
		mCallCount = 0;
		ON_CALL(*mMockContext, getAllFinishedAndConfiguredSessions())
			.WillByDefault(testing::Return(mMockedFinishedSessions));
		ON_CALL(*mMockSession1Open, getBeaconConfiguration())
			.WillByDefault(testing::Return(std::make_shared<configuration::BeaconConfiguration>()));
		ON_CALL(*mMockSession2Open, getBeaconConfiguration())
			.WillByDefault(testing::Return(std::make_shared<configuration::BeaconConfiguration>()));
		ON_CALL(*mMockSession3Closed, getBeaconConfiguration())
			.WillByDefault(testing::Return(std::make_shared<configuration::BeaconConfiguration>()));
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
	uint32_t mCallCount;
	std::shared_ptr<testing::NiceMock<test::MockBeaconSendingContext>> mMockContext;
	std::shared_ptr<testing::NiceMock<test::MockSession>> mMockSession1Open;
	std::shared_ptr<testing::NiceMock<test::MockSession>> mMockSession2Open;
	std::shared_ptr<testing::NiceMock<test::MockSession>> mMockSession3Closed;
	std::vector<std::shared_ptr<core::SessionWrapper>> mMockedNewSessions;
	std::vector<std::shared_ptr<core::SessionWrapper>> mMockedFinishedSessions;
	std::shared_ptr<testing::NiceMock<test::MockHTTPClient>> mMockHTTPClient;
};

TEST_F(BeaconSendingFlushSessionsStateTest, aBeaconSendingFlushSessionsStateIsNotATerminalState)
{
	// given
	communication::BeaconSendingFlushSessionsState target;

	// verify that BeaconSendingCaptureOffState is not a terminal state
	EXPECT_FALSE(target.isTerminalState());
}

TEST_F(BeaconSendingFlushSessionsStateTest, aBeaconSendingFlushSessionsStateHasTerminalStateBeaconSendingTerminalState)
{
	// given
	communication::BeaconSendingFlushSessionsState target;
	auto terminalState = target.getShutdownState();

	// verify that terminal state is BeaconSendingTerminalState
	ASSERT_THAT(terminalState, IsABeaconSendingTerminalState());
}

TEST_F(BeaconSendingFlushSessionsStateTest, aBeaconSendingFlushSessionsStateTransitionsToTerminalStateWhenDataIsSent)
{
	// given
	communication::BeaconSendingFlushSessionsState target;
	
	// verify transition to terminal state
	EXPECT_CALL(*mMockContext, setNextState(IsABeaconSendingTerminalState()))
		.Times(testing::Exactly(1));

	// when calling execute
	target.execute(*mMockContext);
}

TEST_F(BeaconSendingFlushSessionsStateTest, aBeaconSendingFlushSessionsClosesOpenSessions)
{
	// given
	communication::BeaconSendingFlushSessionsState target;

	// verify that open sessions are closed
	EXPECT_CALL(*mMockSession1Open, end())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession2Open, end())
		.Times(testing::Exactly(1));

	// when calling execute
	target.execute(*mMockContext);
}

TEST_F(BeaconSendingFlushSessionsStateTest, aBeaconSendingFlushSessionStateSendsAllOpenAndClosedBeacons)
{
	// given
	communication::BeaconSendingFlushSessionsState target;

	// verify that open sessions are closed
	EXPECT_CALL(*mMockSession1Open, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession2Open, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession3Closed, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(1));

	// when calling execute
	target.execute(*mMockContext);
}