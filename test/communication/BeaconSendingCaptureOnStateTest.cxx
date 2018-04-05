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

#include "communication/BeaconSendingCaptureOnState.h"
#include "communication/AbstractBeaconSendingState.h"

#include "../communication/MockBeaconSendingContext.h"
#include "../communication/CustomMatchers.h"
#include "../protocol/MockStatusResponse.h"
#include "../providers/MockHTTPClientProvider.h"
#include "../core/MockSession.h"

class BeaconSendingCaptureOnStateTest : public testing::Test
{
public:

	BeaconSendingCaptureOnStateTest()
		: mMockContext(nullptr)
	{
	}

	void SetUp()
	{
		mMockSession1Open = std::shared_ptr<testing::NiceMock<test::MockSession>>(new testing::NiceMock<test::MockSession>());
		mMockSession2Open = std::shared_ptr<testing::NiceMock<test::MockSession>>(new testing::NiceMock<test::MockSession>());
		mMockSession3Finished = std::shared_ptr<testing::NiceMock<test::MockSession>>(new testing::NiceMock<test::MockSession>());
		mMockSession4Finished = std::shared_ptr<testing::NiceMock<test::MockSession>>(new testing::NiceMock<test::MockSession>());
		ON_CALL(*mMockSession1Open, sendBeaconRawPtrProxy(testing::_))
			.WillByDefault(testing::Return(new protocol::StatusResponse("", 200)));
		ON_CALL(*mMockSession2Open, sendBeaconRawPtrProxy(testing::_))
			.WillByDefault(testing::Return(new protocol::StatusResponse("", 404)));
		mMockedOpenSessions.push_back(mMockSession1Open);
		mMockedOpenSessions.push_back(mMockSession2Open);

		mMockContext = std::shared_ptr<testing::NiceMock<test::MockBeaconSendingContext>>(new testing::NiceMock<test::MockBeaconSendingContext>());
		ON_CALL(*mMockContext, isTimeSyncSupported())
			.WillByDefault(testing::Return(true));
		ON_CALL(*mMockContext, getLastTimeSyncTime())
			.WillByDefault(testing::Return(0L));
		ON_CALL(*mMockContext, getCurrentTimestamp())
			.WillByDefault(testing::Return(42L));
		ON_CALL(*mMockContext, getAllOpenSessions())
			.WillByDefault(testing::Return(mMockedOpenSessions));
		ON_CALL(*mMockContext, getSendInterval())
			.WillByDefault(testing::Return(0L));

		mCallCount = 0;
		ON_CALL(*mMockContext, getNextFinishedSession())
			.WillByDefault(testing::Invoke(
				[this]() -> std::shared_ptr<test::MockSession> {
			// callCount 1 => mMockSession3Finished
			// callCount 2 => mMockSession4Finished
			// callCount 3 => nullptr
			mCallCount++;
			if (mCallCount == 1)
			{
				return mMockSession3Finished;
			}
			else if (mCallCount == 2)
			{
				return mMockSession4Finished;
			}
			return nullptr;
		}
		));

		mMockHttpClientProvider = std::shared_ptr<testing::NiceMock<test::MockHTTPClientProvider>>(new testing::NiceMock<test::MockHTTPClientProvider>());
		ON_CALL(*mMockContext, getHTTPClientProvider())
			.WillByDefault(testing::Return(mMockHttpClientProvider));
	}

	void TearDown()
	{
		mMockContext = nullptr;
	}
	
	uint32_t mCallCount;
	std::shared_ptr<testing::NiceMock<test::MockBeaconSendingContext>> mMockContext;
	std::shared_ptr<testing::NiceMock<test::MockSession>> mMockSession1Open;
	std::shared_ptr<testing::NiceMock<test::MockSession>> mMockSession2Open;
	std::shared_ptr<testing::NiceMock<test::MockSession>> mMockSession3Finished;
	std::shared_ptr<testing::NiceMock<test::MockSession>> mMockSession4Finished;
	std::vector<std::shared_ptr<core::Session>> mMockedOpenSessions;
	std::shared_ptr<testing::NiceMock<test::MockHTTPClientProvider>> mMockHttpClientProvider;
};

TEST_F(BeaconSendingCaptureOnStateTest, aBeaconSendingCaptureOnStateIsNotATerminalState)
{
	// given
	auto target = std::shared_ptr<communication::AbstractBeaconSendingState>(new communication::BeaconSendingCaptureOnState());

	// verify that BeaconSendingCaptureOnState is not a terminal state
	EXPECT_FALSE(target->isTerminalState());
}

TEST_F(BeaconSendingCaptureOnStateTest, aBeaconSendingCaptureOnStateHasTerminalStateBeaconSendingFlushSessions)
{
	// given
	auto target = std::shared_ptr<communication::AbstractBeaconSendingState>(new communication::BeaconSendingCaptureOnState());
	auto terminalState = target->getShutdownState();

	// verify that terminal state is BeaconSendingFlushSessions
	ASSERT_THAT(terminalState, IsABeaconSendingFlushSessionsState());
}

TEST_F(BeaconSendingCaptureOnStateTest, aBeaconSendingCaptureOnStateTransitionsToTimeSyncStateWhenLastSyncTimeIsNegative)
{
	// given
	auto target = std::shared_ptr<communication::AbstractBeaconSendingState>(new communication::BeaconSendingCaptureOnState());

	ON_CALL(*mMockContext, isTimeSyncSupported())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mMockContext, isCaptureOn())
		.WillByDefault(testing::Return(false));
	ON_CALL(*mMockContext, getLastTimeSyncTime())
		.WillByDefault(testing::Return(-1L));

	// then expect lastStatusCheckTime to be updated and that next state is time sync state
	EXPECT_CALL(*mMockContext, setNextState(IsABeaconSendingTimeSyncState()))
		.Times(testing::Exactly(1));

	// when calling execute
	target->execute(*mMockContext);
}

TEST_F(BeaconSendingCaptureOnStateTest, aBeaconSendingCaptureOnStateTransitionsToTimeSyncStateWhenCheckIntervalPassed)
{
	// given
	communication::BeaconSendingCaptureOnState target;
	
	ON_CALL(*mMockContext, isTimeSyncSupported())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mMockContext, isCaptureOn())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mMockContext, getLastTimeSyncTime())
		.WillByDefault(testing::Return(0L));
	ON_CALL(*mMockContext, getCurrentTimestamp())
		.WillByDefault(testing::Return(7500000L));

	// then expect lastStatusCheckTime to be updated and that next state is time sync state
	EXPECT_CALL(*mMockContext, setNextState(IsABeaconSendingTimeSyncState()))
		.Times(testing::Exactly(1));

	// when calling execute
	target.doExecute(*mMockContext);
}

TEST_F(BeaconSendingCaptureOnStateTest, aBeaconSendingCaptureOnStateSendsFinishedSessions)
{
	// given
	communication::BeaconSendingCaptureOnState target;
	ON_CALL(*mMockSession3Finished, sendBeaconRawPtrProxy(testing::_))
		.WillByDefault(testing::Return(new testing::NiceMock<test::MockStatusResponse>()));
	ON_CALL(*mMockSession4Finished, sendBeaconRawPtrProxy(testing::_))
		.WillByDefault(testing::Return(new testing::NiceMock<test::MockStatusResponse>()));

	// then
	EXPECT_CALL(*mMockSession3Finished, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession4Finished, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(1));

	// when calling execute
	target.doExecute(*mMockContext);
}

TEST_F(BeaconSendingCaptureOnStateTest, aBeaconSendingCaptureOnStatePushesBackFinishedSessionIfSendWasUnsuccessful)
{
	// given
	communication::BeaconSendingCaptureOnState target;
	ON_CALL(*mMockSession3Finished, sendBeaconRawPtrProxy(testing::_))
		.WillByDefault(testing::Return(nullptr));
	ON_CALL(*mMockSession4Finished, sendBeaconRawPtrProxy(testing::_))
		.WillByDefault(testing::Return(new testing::NiceMock<test::MockStatusResponse>()));
	ON_CALL(*mMockSession3Finished, isEmpty())
		.WillByDefault(testing::Return(false));

	// then
	EXPECT_CALL(*mMockSession3Finished, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession4Finished, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mMockContext, getNextFinishedSession())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockContext, pushBackFinishedSession(testing::_)) // improvement mMockSession4Finished
		.Times(testing::Exactly(1));

	// when calling execute
	target.doExecute(*mMockContext);
}

TEST_F(BeaconSendingCaptureOnStateTest, aBeaconSendingCaptureOnStateContinuesWithNextFinishedSessionIfSendingWasUnsuccessfulButBeaoonIsEmtpy)
{
	// given
	communication::BeaconSendingCaptureOnState target;
	ON_CALL(*mMockSession3Finished, sendBeaconRawPtrProxy(testing::_))
		.WillByDefault(testing::Return(new testing::NiceMock<test::MockStatusResponse>()));
	ON_CALL(*mMockSession4Finished, sendBeaconRawPtrProxy(testing::_))
		.WillByDefault(testing::Return(new testing::NiceMock<test::MockStatusResponse>()));
	ON_CALL(*mMockSession3Finished, isEmpty())
		.WillByDefault(testing::Return(true));

	// then
	EXPECT_CALL(*mMockSession3Finished, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession4Finished, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession3Finished, clearCapturedData())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession4Finished, clearCapturedData())
		.Times(testing::Exactly(1));

	EXPECT_CALL(*mMockContext, getNextFinishedSession())
		.Times(testing::Exactly(3));
	EXPECT_CALL(*mMockContext, pushBackFinishedSession(testing::_))
		.Times(testing::Exactly(0));

	// when calling execute
	target.doExecute(*mMockContext);
}

TEST_F(BeaconSendingCaptureOnStateTest, aBeaconSendingCaptureOnStateSendsOpenSessionsIfNotExpired)
{
	// given
	communication::BeaconSendingCaptureOnState target;

	// then
	EXPECT_CALL(*mMockSession1Open, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession2Open, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockContext, setLastOpenSessionBeaconSendTime(testing::_))
		.Times(testing::Exactly(1));

	// when calling execute
	target.doExecute(*mMockContext);
}

TEST_F(BeaconSendingCaptureOnStateTest, aBeaconSendingCaptureOnStateTransitionsToTimeSyncStateIfSessionExpired)
{
	// given
	testing::StrictMock<test::MockBeaconSendingContext> mockContext;
	communication::BeaconSendingCaptureOnState target;
	ON_CALL(mockContext, isTimeSyncSupported())
		.WillByDefault(testing::Return(true));
	ON_CALL(mockContext, getLastTimeSyncTime())
		.WillByDefault(testing::Return(0L));
	ON_CALL(mockContext, getCurrentTimestamp())
		.WillByDefault(testing::Return(72000042L));
	
	// then
	EXPECT_CALL(mockContext, isTimeSyncSupported())
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, getCurrentTimestamp())
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, getLastTimeSyncTime())
		.Times(::testing::Exactly(2));
	EXPECT_CALL(mockContext, setNextState(IsABeaconSendingTimeSyncState()))
		.Times(testing::Exactly(1));

	// when calling execute
	target.doExecute(mockContext);
}

TEST_F(BeaconSendingCaptureOnStateTest, aBeaconSendingCaptureOnStateTransitionsToCaptureOffStateWhenCapturingGotDisabled)
{
	// given
	communication::BeaconSendingCaptureOnState target;
	ON_CALL(*mMockContext, isTimeSyncSupported())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mMockContext, isCaptureOn())
		.WillByDefault(testing::Return(false));
	
	// then
	EXPECT_CALL(*mMockContext, isCaptureOn())
		.Times(::testing::Exactly(2));
	EXPECT_CALL(*mMockContext, setNextState(IsABeaconSendingCaptureOffState()))
		.Times(testing::Exactly(1));

	// when calling execute
	target.doExecute(*mMockContext);
}