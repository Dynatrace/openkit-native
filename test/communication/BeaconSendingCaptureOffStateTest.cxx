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

#include "communication/BeaconSendingCaptureOffState.h"
#include "communication/AbstractBeaconSendingState.h"

#include "MockBeaconSendingContext.h"
#include "../protocol/MockHTTPClient.h"
#include "../protocol/MockStatusResponse.h"
#include "../communication/CustomMatchers.h"

class BeaconSendingCaptureOffStateTest : public testing::Test
{
public:

	BeaconSendingCaptureOffStateTest()
		: target(nullptr)
		, mockHTTPClient(nullptr)
	{
	}

	void SetUp()
	{
		target = std::shared_ptr<communication::AbstractBeaconSendingState>(new communication::BeaconSendingCaptureOffState());
		std::shared_ptr<configuration::HTTPClientConfiguration> httpClientConfiguration = std::make_shared<configuration::HTTPClientConfiguration>(core::UTF8String(""),0, core::UTF8String(""));
		mockHTTPClient = std::shared_ptr<testing::NiceMock<test::MockHTTPClient>>(new testing::NiceMock<test::MockHTTPClient>(httpClientConfiguration));
		
		ON_CALL(*mockHTTPClient, sendStatusRequestRawPtrProxy())
			.WillByDefault(testing::Return(new protocol::StatusResponse()));
	}

	void TearDown()
	{
		target = nullptr;
		mockHTTPClient = nullptr;
	}

	std::shared_ptr<communication::AbstractBeaconSendingState> target;
	std::shared_ptr<testing::NiceMock<test::MockHTTPClient>> mockHTTPClient;
};

TEST_F(BeaconSendingCaptureOffStateTest, aBeaconSendingCaptureOffStateIsNotATerminalState)
{
	// verify that BeaconSendingCaptureOffState is not a terminal state
	EXPECT_FALSE(target->isTerminalState());
}

TEST_F(BeaconSendingCaptureOffStateTest, aBeaconSendingCaptureOffStateHasTerminalStateBeaconSendingFlushSessions)
{
	// when
	std::shared_ptr<AbstractBeaconSendingState> obtained = target->getShutdownState();

	// verify that terminal state is BeaconSendingFlushSessions
	ASSERT_TRUE(obtained != nullptr);
	ASSERT_EQ(obtained->getStateType(), communication::AbstractBeaconSendingState::StateType::BEACON_SENDING_FLUSH_SESSIONS_STATE);
}

TEST_F(BeaconSendingCaptureOffStateTest, aBeaconSendingCaptureOffStateTransitionsToTimeSyncStateWhenNotYetTimeSynched)
{
	// given
	testing::NiceMock<test::MockBeaconSendingContext> mockContext;
	ON_CALL(mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mockHTTPClient));
	ON_CALL(mockContext, isTimeSyncSupported())
		.WillByDefault(testing::Return(true));
	ON_CALL(mockContext, isCaptureOn())
		.WillByDefault(testing::Return(false));
	ON_CALL(mockContext, isTimeSynced())
		.WillByDefault(testing::Return(false));

	// then verify that capturing is set to disabled
	EXPECT_CALL(mockContext, disableCapture())
		.Times(::testing::Exactly(1));
	// also verify that lastStatusCheckTime was updated
	EXPECT_CALL(mockContext, setLastStatusCheckTime(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(mockContext, setNextState(IsABeaconSendingTimeSyncState()))
		.Times(testing::Exactly(1));

	// when calling execute
	target->execute(mockContext);
}

TEST_F(BeaconSendingCaptureOffStateTest, aBeaconSendingCaptureOffStateTransitionsToCaptureOnStateWhenCapturingActive)
{
	// given
	testing::NiceMock<test::MockBeaconSendingContext> mockContext;
	ON_CALL(mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mockHTTPClient));
	ON_CALL(mockContext, isTimeSyncSupported())
		.WillByDefault(testing::Return(true));
	ON_CALL(mockContext, isCaptureOn())
		.WillByDefault(testing::Return(true));
	ON_CALL(mockContext, isTimeSynced())
		.WillByDefault(testing::Return(true));

	// then verify that capturing is set to disabled
	EXPECT_CALL(mockContext, disableCapture())
		.Times(::testing::Exactly(1));
	// also verify that lastStatusCheckTime was updated
	EXPECT_CALL(mockContext, setLastStatusCheckTime(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(mockContext, setNextState(IsABeaconSendingCaptureOnState()))
		.Times(testing::Exactly(1));

	// when calling execute
	target->execute(mockContext);
}

TEST_F(BeaconSendingCaptureOffStateTest, aBeaconSendingCaptureOffStateWaitsForSpecifiedTimeWhenTimeSyncFails)
{
	// given
	testing::NiceMock<test::MockBeaconSendingContext> mockContext;
	ON_CALL(mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mockHTTPClient));
	ON_CALL(mockContext, isTimeSyncSupported())
		.WillByDefault(testing::Return(true));
	ON_CALL(mockContext, isCaptureOn())
		.WillByDefault(testing::Return(false));
	ON_CALL(mockContext, isTimeSynced())
		.WillByDefault(testing::Return(false));

	// then verify that capturing is set to disabled
	EXPECT_CALL(mockContext, disableCapture())
		.Times(::testing::Exactly(1));
	// also verify that lastStatusCheckTime was updated
	EXPECT_CALL(mockContext, setLastStatusCheckTime(0L))
		.Times(testing::Exactly(1));
	// verify that the next time sync operation will follow after a sleep of 7200000 ms
	EXPECT_CALL(mockContext, sleep(7200000L))
		.Times(testing::Exactly(1));
	// verify that after sleeping the transition to BeaconSendingTimeSyncState works
	EXPECT_CALL(mockContext, setNextState(IsABeaconSendingTimeSyncState()))
		.Times(testing::Exactly(1));

	// when calling execute
	target->execute(mockContext);
}