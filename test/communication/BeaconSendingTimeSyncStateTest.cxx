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

#include "communication/BeaconSendingTimeSyncState.h"
#include "communication/AbstractBeaconSendingState.h"

#include "MockBeaconSendingContext.h"
#include "../protocol/MockHTTPClient.h"

class BeaconSendingTimeSyncTest : public testing::Test
{
public:

	BeaconSendingTimeSyncTest()
		: target(nullptr)
		, mockHTTPClient(nullptr)
	{
	}

	void SetUp()
	{
		target = std::shared_ptr<communication::AbstractBeaconSendingState>(new communication::BeaconSendingTimeSyncState(true));
		std::shared_ptr<configuration::HTTPClientConfiguration> httpClientConfiguration = std::make_shared<configuration::HTTPClientConfiguration>(core::UTF8String(""),0, core::UTF8String(""));
		mockHTTPClient = std::shared_ptr<test::MockHTTPClient>(new test::MockHTTPClient(httpClientConfiguration));
	}

	void TearDown()
	{
	}

	std::shared_ptr<communication::AbstractBeaconSendingState> target;//time sync state with initialTimeSync set to true
	std::shared_ptr<test::MockHTTPClient> mockHTTPClient;

};

TEST_F(BeaconSendingTimeSyncTest, timeSyncStateIsNotATerminalState)
{
	ASSERT_FALSE(target->isAShutdownState());
}

TEST_F(BeaconSendingTimeSyncTest, getShutdownStateGivesATerminalStateInstanceForInitialTimeSync)
{
	// when
	std::shared_ptr<AbstractBeaconSendingState> obtained = target->getShutdownState();

	//then
	ASSERT_TRUE(obtained != nullptr);
	ASSERT_EQ(obtained->getStateType(), communication::AbstractBeaconSendingState::StateType::BEACON_SENDING_TERMINAL_STATE);
}

TEST_F(BeaconSendingTimeSyncTest, DISABLED_getShutdownStateGivesAFlushSessionsStateInstanceForInitialTimeSync)
{
	// when
	std::shared_ptr<AbstractBeaconSendingState> obtained = target->getShutdownState();

	//then
	ASSERT_TRUE(obtained != nullptr);
	//ASSERT_EQ(obtained->getStateType(), communication::AbstractBeaconSendingState::StateType::BEACON_SENDING_FLUSHSESSIONS_STATE);
}

TEST_F(BeaconSendingTimeSyncTest, isTimeSyncRequiredReturnsFalseImmediatelyIfTimeSyncIsNotSupported)
{
	testing::NiceMock<test::MockBeaconSendingContext> mockContext;//NiceMock: ensure that required calls are there but do not object about other calls

    // given
	ON_CALL(mockContext, isTimeSyncSupported())
		.WillByDefault(testing::Return(false));
	ON_CALL(mockContext, getLastTimeSyncTime())
		.WillByDefault(testing::Return(-1));

	// when/then
	BeaconSendingTimeSyncState* underTest = static_cast<BeaconSendingTimeSyncState*>(target.get());
	ASSERT_FALSE(underTest->isTimeSyncRequired(mockContext));
}

TEST_F(BeaconSendingTimeSyncTest, timeSyncIsRequiredWhenLastTimeSyncTimeIsNegative)
{
	//given
	testing::NiceMock<test::MockBeaconSendingContext> mockContext;//NiceMock: ensure that required calls are there but do not object about other calls
	ON_CALL(mockContext, getLastTimeSyncTime())
		.WillByDefault(testing::Return(-1));
	ON_CALL(mockContext, isTimeSyncSupported())
		.WillByDefault(testing::Return(true));
	// when/then
	BeaconSendingTimeSyncState* underTest = static_cast<BeaconSendingTimeSyncState*>(target.get());
	ASSERT_TRUE(underTest->isTimeSyncRequired(mockContext));
}

TEST_F(BeaconSendingTimeSyncTest, isTimeSyncRequiredBoundaries)
{
	//given
	BeaconSendingTimeSyncState* underTest = static_cast<BeaconSendingTimeSyncState*>(target.get());

	testing::NiceMock<test::MockBeaconSendingContext> mockContext;//NiceMock: ensure that required calls are there but do not object about other calls
	ON_CALL(mockContext, isTimeSyncSupported())
		.WillByDefault(testing::Return(true));
	ON_CALL(mockContext, getLastTimeSyncTime())
		.WillByDefault(testing::Return(0));

	// when the last sync time is TIME_SYNC_INTERVAL_IN_MILLIS - 1 milliseconds ago
	ON_CALL(mockContext, getCurrentTimestamp())
		.WillByDefault(testing::Return(BeaconSendingTimeSyncState::TIME_SYNC_INTERVAL_IN_MILLIS.count() -1));

	 // then
	 ASSERT_FALSE(underTest->isTimeSyncRequired(mockContext));

	 // when the last sync time is TIME_SYNC_INTERVAL_IN_MILLIS milliseconds ago
	 ON_CALL(mockContext, getCurrentTimestamp())
		 .WillByDefault(testing::Return(BeaconSendingTimeSyncState::TIME_SYNC_INTERVAL_IN_MILLIS.count()));

	 // then
	 ASSERT_FALSE(underTest->isTimeSyncRequired(mockContext));

	 // when the last sync time is TIME_SYNC_INTERVAL_IN_MILLIS + 1 milliseconds ago
	 ON_CALL(mockContext, getCurrentTimestamp())
		 .WillByDefault(testing::Return(BeaconSendingTimeSyncState::TIME_SYNC_INTERVAL_IN_MILLIS.count() + 1));

	 // then
	 ASSERT_TRUE(underTest->isTimeSyncRequired(mockContext));
 }

TEST_F(BeaconSendingTimeSyncTest, DISABLED_timeSyncNotRequiredAndCaptureOnTruePerformsStateTransitionToCaptureOnState)
{
	// given
	testing::NiceMock<test::MockBeaconSendingContext> mockContext;//NiceMock: ensure that required calls are there but do not object about other calls
	ON_CALL(mockContext, isTimeSyncSupported())
		.WillByDefault(testing::Return(false));
	ON_CALL(mockContext, isCaptureOn())
		.WillByDefault(testing::Return(true));

	// then
	EXPECT_CALL(mockContext, setNextState(testing::_))
		.Times(::testing::Exactly(1));

	// when
	target->execute(mockContext);

	ASSERT_EQ(mockContext.getCurrentStateType(), AbstractBeaconSendingState::StateType::BEACON_SENDING_CAPTUREON_STATE);
}