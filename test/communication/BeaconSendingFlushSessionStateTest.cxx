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
		, mMockedOpenSessions()
	{
	}

	void SetUp()
	{
		mLogger = std::shared_ptr<openkit::ILogger>(new core::util::DefaultLogger(devNull, true));
		mMockSession1Open = std::shared_ptr<testing::NiceMock<test::MockSession>>(new testing::NiceMock<test::MockSession>(mLogger));
		mMockSession2Open = std::shared_ptr<testing::NiceMock<test::MockSession>>(new testing::NiceMock<test::MockSession>(mLogger));
		mMockSession3Closed = std::shared_ptr<testing::NiceMock<test::MockSession>>(new testing::NiceMock<test::MockSession>(mLogger));
		mMockedOpenSessions.push_back(mMockSession1Open);
		mMockedOpenSessions.push_back(mMockSession2Open);

		std::shared_ptr<configuration::HTTPClientConfiguration> httpClientConfiguration = std::make_shared<configuration::HTTPClientConfiguration>(core::UTF8String(""), 0, core::UTF8String(""));
		mMockHTTPClient = std::shared_ptr<testing::NiceMock<test::MockHTTPClient>>(new testing::NiceMock<test::MockHTTPClient>(httpClientConfiguration));
		ON_CALL(*mMockHTTPClient, sendStatusRequestRawPtrProxy())
			.WillByDefault(testing::Return(new protocol::StatusResponse()));

		mMockContext = std::shared_ptr<testing::NiceMock<test::MockBeaconSendingContext>>(new testing::NiceMock<test::MockBeaconSendingContext>(mLogger));
		ON_CALL(*mMockContext, getHTTPClient())
			.WillByDefault(testing::Return(mMockHTTPClient));
		ON_CALL(*mMockContext, getAllOpenSessions())
			.WillByDefault(testing::Return(mMockedOpenSessions));
		mCallCount = 0;
		ON_CALL(*mMockContext, getNextFinishedSession())
			.WillByDefault(testing::Invoke(
				[this]() -> std::shared_ptr<test::MockSession> {
			// callCount 1 => mockSession3Closed
			// callCount 2 => mockSession2Open
			// callCount 3 => mockSession1Open
			// callCount 4 => nullptr
			mCallCount++;
			if (mCallCount == 1)
			{
				return mMockSession3Closed;
			}
			else if (mCallCount == 2)
			{
				return mMockSession2Open;
			}
			else if (mCallCount == 3)
			{
				return mMockSession1Open;
			}
			return nullptr;
		}
		));
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
	std::vector<std::shared_ptr<core::Session>> mMockedOpenSessions;
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