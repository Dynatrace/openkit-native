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

#include "communication/BeaconSendingInitialState.h"
#include "communication/AbstractBeaconSendingState.h"
#include "protocol/StatusResponse.h"

#include "../communication/MockBeaconSendingContext.h"
#include "../protocol/MockHTTPClient.h"
#include "../communication/CustomMatchers.h"
#include "../protocol/NullLogger.h"

class BeaconSendingInitialStateTest : public testing::Test
{
protected:

	BeaconSendingInitialStateTest()
		: mLogger(nullptr)
		, mMockHTTPClient(nullptr)
	{
	}

	void SetUp()
	{
		mLogger = std::make_shared<NullLogger>();
		std::shared_ptr<configuration::HTTPClientConfiguration> httpClientConfiguration = std::make_shared<configuration::HTTPClientConfiguration>(core::UTF8String(""),0, core::UTF8String(""));
		mMockHTTPClient = std::make_shared<testing::NiceMock<test::MockHTTPClient>>(httpClientConfiguration);

		ON_CALL(*mMockHTTPClient, sendStatusRequestRawPtrProxy())
			.WillByDefault(testing::Invoke([&]() -> protocol::StatusResponse* {
				return new protocol::StatusResponse(mLogger, core::UTF8String(), 200, protocol::Response::ResponseHeaders());
			}));
	}

	void TearDown()
	{
		mLogger = nullptr;
		mMockHTTPClient = nullptr;
	}

	std::ostringstream devNull;
	std::shared_ptr<openkit::ILogger> mLogger;
	std::shared_ptr<testing::NiceMock<test::MockHTTPClient>> mMockHTTPClient;
};

TEST_F(BeaconSendingInitialStateTest, initStateIsNotATerminalState)
{
	// given
	auto target = BeaconSendingInitialState();

	// then
	EXPECT_FALSE(target.isTerminalState());
}

TEST_F(BeaconSendingInitialStateTest, getShutdownStateGivesABeaconSendingTerminalStateInstance)
{
	// given
	auto target = BeaconSendingInitialState();

	// when
	std::shared_ptr<AbstractBeaconSendingState> obtained = target.getShutdownState();

	//then
	ASSERT_TRUE(obtained != nullptr);
	ASSERT_EQ(obtained->getStateType(), communication::AbstractBeaconSendingState::StateType::BEACON_SENDING_TERMINAL_STATE);
}

TEST_F(BeaconSendingInitialStateTest, getShutdownStateAlwaysCreatesANewInstance)
{
	// given
	auto target = BeaconSendingInitialState();

	// when calling getShutDownState twice
	std::shared_ptr<AbstractBeaconSendingState> obtainedOne = target.getShutdownState();
	std::shared_ptr<AbstractBeaconSendingState> obtainedTwo = target.getShutdownState();

	//then
	ASSERT_TRUE(obtainedOne != nullptr);
	ASSERT_TRUE(obtainedTwo != nullptr);
	EXPECT_NE(obtainedOne, obtainedTwo);
}

TEST_F(BeaconSendingInitialStateTest, executeSetsLastOpenSessionBeaconSendTime)
{
	// given
	auto target = BeaconSendingInitialState();

	testing::NiceMock<test::MockBeaconSendingContext> mockContext(mLogger);//NiceMock: ensure that required calls are there but do not object about other calls

	ON_CALL(mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mMockHTTPClient));
	ON_CALL(mockContext, getCurrentTimestamp())
		.WillByDefault(testing::Return(123456789L));
	EXPECT_CALL(mockContext, isShutdownRequested())
		.WillOnce(testing::Return(false))
		.WillRepeatedly(testing::Return(true));
	
	// check
	EXPECT_CALL(mockContext, setLastOpenSessionBeaconSendTime(123456789L))
		.Times(::testing::Exactly(1));

	//when 
	target.execute(mockContext);
}

TEST_F(BeaconSendingInitialStateTest, executeSetsLastStatusCheckTime)
{
	// given
	auto target = BeaconSendingInitialState();

	testing::NiceMock<test::MockBeaconSendingContext> mockContext(mLogger);//NiceMock: ensure that required calls are there but do not object about other calls

	ON_CALL(mockContext, getCurrentTimestamp())
		.WillByDefault(testing::Return(123456789L));
	EXPECT_CALL(mockContext, isShutdownRequested())
		.WillOnce(testing::Return(false))
		.WillRepeatedly(testing::Return(true));

	//check
	EXPECT_CALL(mockContext, setLastStatusCheckTime(123456789L))
		.Times(::testing::Exactly(1));

	//when
	target.execute(mockContext);
}

TEST_F(BeaconSendingInitialStateTest, initIsTerminatedIfShutdownRequestedWithValidResponse)
{
	// given
	auto target = BeaconSendingInitialState();

	testing::NiceMock<test::MockBeaconSendingContext> mockContext(mLogger);//NiceMock: ensure that required calls are there but do not object about other calls
	
	ON_CALL(mockContext, isShutdownRequested())
		.WillByDefault(testing::Return(true));
	ON_CALL(mockContext, setNextState(testing::_))
		.WillByDefault(testing::WithArgs<0>(testing::Invoke(&mockContext, &test::MockBeaconSendingContext::RealSetNextState)));
	ON_CALL(mockContext, isInTerminalState())
		.WillByDefault(testing::Invoke(&mockContext, &test::MockBeaconSendingContext::RealIsInTerminalState));

	//check
	EXPECT_CALL(mockContext, setInitCompleted(false))
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, setNextState(IsABeaconSendingTerminalState()))
		.Times(::testing::Exactly(1));

	//when
	target.execute(mockContext);
	auto nextState = mockContext.RealGetNextState();
	EXPECT_TRUE(nextState != nullptr);
	EXPECT_TRUE(nextState->isTerminalState());
}



TEST_F(BeaconSendingInitialStateTest, sleepTimeIsDoubledBetweenStatusRequestRetries)
{
	// given
	auto target = BeaconSendingInitialState();
	
	testing::NiceMock<test::MockBeaconSendingContext> mockContext(mLogger);

	uint32_t callCount = 0;

	ON_CALL(mockContext, isShutdownRequested())
		.WillByDefault(testing::Invoke(
			[&callCount]() -> bool {
		return callCount++ >= 6;
	}
	));//should return true the 6th time

	ON_CALL(mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mMockHTTPClient));
	ON_CALL(*mMockHTTPClient, sendStatusRequestRawPtrProxy())
		.WillByDefault(testing::Invoke([&]() -> protocol::StatusResponse* {
			return new protocol::StatusResponse(mLogger, core::UTF8String(), 400, protocol::Response::ResponseHeaders());
		}));

	// check for 
	uint64_t initialSleep = communication::BeaconSendingInitialState::INITIAL_RETRY_SLEEP_TIME_MILLISECONDS.count();
	testing::InSequence s;
	EXPECT_CALL(mockContext, sleep(::testing::_))
		.Times(::testing::Exactly(0));
	EXPECT_CALL(mockContext, sleep(initialSleep))
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, sleep(initialSleep * 2))
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, sleep(initialSleep * 4))
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, sleep(initialSleep * 8))
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, sleep(initialSleep * 16))
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, sleep(BeaconSendingInitialState::REINIT_DELAY_MILLISECONDS[0].count()))
		.Times(::testing::Exactly(1));

	// when executing the state
	target.execute(mockContext);
}

TEST_F(BeaconSendingInitialStateTest, initialStatusRequestGivesUpWhenShutdownRequestIsSetDuringExecution)
{
	// given
	auto target = BeaconSendingInitialState();

	testing::NiceMock<test::MockBeaconSendingContext> mockContext(mLogger);//NiceMock: ensure that required calls are there but do not object about other calls
	
	ON_CALL(mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mMockHTTPClient));
	ON_CALL(*mMockHTTPClient, sendStatusRequestRawPtrProxy())
		.WillByDefault(testing::Invoke([&]() -> protocol::StatusResponse* {
			return new protocol::StatusResponse(mLogger, core::UTF8String(), 400, protocol::Response::ResponseHeaders());
		}));
	ON_CALL(mockContext, setNextState(testing::_))
		.WillByDefault(testing::WithArgs<0>(testing::Invoke(&mockContext, &test::MockBeaconSendingContext::RealSetNextState)));

	uint32_t callCount = 0;
	//given
	ON_CALL(mockContext, isShutdownRequested())
		.WillByDefault(testing::Invoke(
			[&callCount]() -> bool {
		return callCount++ >= 4;
	}
	));//should return true the 3rd time

	// then
	EXPECT_CALL(mockContext, setInitCompleted(false))
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, setNextState(IsABeaconSendingTerminalState()))
		.Times(::testing::Exactly(1));
							
	// verify that the requests where sent N times - defined as constants in the state itself
	EXPECT_CALL(mockContext, getHTTPClient())
		.Times(::testing::Exactly(3));

	// verify sleeps between each retry
	EXPECT_CALL(mockContext, sleep(testing::_))
		.Times(::testing::Exactly(4));

	// when executing the state
	target.execute(mockContext);

	auto nextState = mockContext.RealGetNextState();
	ASSERT_TRUE(nextState != nullptr);
	ASSERT_TRUE(nextState->isTerminalState());
}

TEST_F(BeaconSendingInitialStateTest, aSuccessfulStatusResponsePerformsStateTransitionToCaptureOnStateIfCapturingIsEnabled)
{
	// given
	auto target = BeaconSendingInitialState();

	testing::NiceMock<test::MockBeaconSendingContext> mockContext(mLogger);//NiceMock: ensure that required calls are there but do not object about other calls

	ON_CALL(mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mMockHTTPClient));
	ON_CALL(mockContext, isCaptureOn())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mMockHTTPClient, sendStatusRequestRawPtrProxy())
		.WillByDefault(testing::Invoke([&]() -> protocol::StatusResponse* {
		return new protocol::StatusResponse(mLogger, core::UTF8String("type=m&cp=1"), 200, protocol::Response::ResponseHeaders());
	}));

	// verify state transition to CaptureOn state
	EXPECT_CALL(mockContext, setNextState(IsABeaconSendingCaptureOnState()))
		.Times(::testing::Exactly(1));

	// when
	target.execute(mockContext);
}

TEST_F(BeaconSendingInitialStateTest, aSuccessfulStatusResponsePerformsStateTransitionToCaptureOffStateIfCapturingIsDisabled)
{
	// given
	auto target = BeaconSendingInitialState();

	testing::NiceMock<test::MockBeaconSendingContext> mockContext(mLogger);//NiceMock: ensure that required calls are there but do not object about other calls

	ON_CALL(mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mMockHTTPClient));
	ON_CALL(mockContext, isCaptureOn())
		.WillByDefault(testing::Return(false));
	ON_CALL(*mMockHTTPClient, sendStatusRequestRawPtrProxy())
		.WillByDefault(testing::Invoke([&]() -> protocol::StatusResponse* {
		return new protocol::StatusResponse(mLogger, core::UTF8String("type=m&cp=0"), 200, protocol::Response::ResponseHeaders());
	}));

	// verify state transition to CaptureOff state
	EXPECT_CALL(mockContext, setNextState(IsABeaconSendingCaptureOffState()))
		.Times(::testing::Exactly(1));

	// when
	target.execute(mockContext);
}

TEST_F(BeaconSendingInitialStateTest, aSuccessfulStatusResponseSetsInitCompletedIfCapturingIsEnabled)
{
	// given
	auto target = BeaconSendingInitialState();

	testing::NiceMock<test::MockBeaconSendingContext> mockContext(mLogger);//NiceMock: ensure that required calls are there but do not object about other calls

	ON_CALL(mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mMockHTTPClient));
	ON_CALL(mockContext, isCaptureOn())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mMockHTTPClient, sendStatusRequestRawPtrProxy())
		.WillByDefault(testing::Invoke([&]() -> protocol::StatusResponse* {
		return new protocol::StatusResponse(mLogger, core::UTF8String("type=m&cp=1"), 200, protocol::Response::ResponseHeaders());
	}));

	// verify that setInitCompleted is set
	EXPECT_CALL(mockContext, setInitCompleted(true))
		.Times(::testing::Exactly(1));

	// when
	target.execute(mockContext);
}

TEST_F(BeaconSendingInitialStateTest, aSuccessfulStatusResponseSetsInitCompletedIfCapturingIsDisabled)
{
	// given
	auto target = BeaconSendingInitialState();

	testing::NiceMock<test::MockBeaconSendingContext> mockContext(mLogger);//NiceMock: ensure that required calls are there but do not object about other calls

	ON_CALL(mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mMockHTTPClient));
	ON_CALL(mockContext, isCaptureOn())
		.WillByDefault(testing::Return(false));
	ON_CALL(*mMockHTTPClient, sendStatusRequestRawPtrProxy())
		.WillByDefault(testing::Invoke([&]() -> protocol::StatusResponse* {
		return new protocol::StatusResponse(mLogger, core::UTF8String("type=m&cp=0"), 200, protocol::Response::ResponseHeaders());
	}));

	// verify that setInitCompleted is set
	EXPECT_CALL(mockContext, setInitCompleted(true))
		.Times(::testing::Exactly(1));

	// when
	target.execute(mockContext);
}

TEST_F(BeaconSendingInitialStateTest, reinitializeSleepsBeforeSendingStatusRequestsAgain)
{
	// given
	auto target = BeaconSendingInitialState();

	testing::NiceMock<test::MockBeaconSendingContext> mockContext(mLogger);//NiceMock: ensure that required calls are there but do not object about other calls

	uint32_t callCount = 0;

	ON_CALL(mockContext, isShutdownRequested())
		.WillByDefault(testing::Invoke(
			[&callCount]() -> bool {
				return callCount++ >= 49; 
			}
		));//should return true the 49th time

	ON_CALL(mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mMockHTTPClient));
	ON_CALL(*mMockHTTPClient, sendStatusRequestRawPtrProxy())
		.WillByDefault(testing::Invoke([&]() -> protocol::StatusResponse* {
			return new protocol::StatusResponse(mLogger, core::UTF8String(), 400, protocol::Response::ResponseHeaders());
		}));

	testing::InSequence s;

	uint64_t initialSleep = communication::BeaconSendingInitialState::INITIAL_RETRY_SLEEP_TIME_MILLISECONDS.count();
	
	// then
	// the "unexpected" call
	EXPECT_CALL(mockContext, sleep(::testing::_))
		.Times(::testing::Exactly(0));
	// from first round
	EXPECT_CALL(mockContext, sleep(initialSleep))
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, sleep(initialSleep * 2))
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, sleep(initialSleep * 4))
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, sleep(initialSleep * 8))
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, sleep(initialSleep * 16))
		.Times(::testing::Exactly(1));
	// delay between first and second attempt
	EXPECT_CALL(mockContext, sleep(communication::BeaconSendingInitialState::REINIT_DELAY_MILLISECONDS[0].count()))
		.Times(::testing::Exactly(1));
	// and again the sequence
	EXPECT_CALL(mockContext, sleep(initialSleep))
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, sleep(initialSleep * 2))
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, sleep(initialSleep * 4))
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, sleep(initialSleep * 8))
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, sleep(initialSleep * 16))
		.Times(::testing::Exactly(1));
	// delay between second and third attempt
	EXPECT_CALL(mockContext, sleep(communication::BeaconSendingInitialState::REINIT_DELAY_MILLISECONDS[1].count()))
		.Times(::testing::Exactly(1));
	// and again the sequence
	EXPECT_CALL(mockContext, sleep(initialSleep))
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, sleep(initialSleep * 2))
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, sleep(initialSleep * 4))
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, sleep(initialSleep * 8))
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, sleep(initialSleep * 16))
		.Times(::testing::Exactly(1));
	// delay between third and fourth attempt
	EXPECT_CALL(mockContext, sleep(communication::BeaconSendingInitialState::REINIT_DELAY_MILLISECONDS[2].count()))
		.Times(::testing::Exactly(1));
	// and again the sequence
	EXPECT_CALL(mockContext, sleep(initialSleep))
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, sleep(initialSleep * 2))
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, sleep(initialSleep * 4))
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, sleep(initialSleep * 8))
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, sleep(initialSleep * 16))
		.Times(::testing::Exactly(1));
	// delay between fourth and fifth attempt
	EXPECT_CALL(mockContext, sleep(communication::BeaconSendingInitialState::REINIT_DELAY_MILLISECONDS[3].count()))
		.Times(::testing::Exactly(1));
	// and again the sequence
	EXPECT_CALL(mockContext, sleep(initialSleep))
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, sleep(initialSleep * 2))
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, sleep(initialSleep * 4))
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, sleep(initialSleep * 8))
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, sleep(initialSleep * 16))
		.Times(::testing::Exactly(1));
	// delay between fifth and sixth attempt
	EXPECT_CALL(mockContext, sleep(communication::BeaconSendingInitialState::REINIT_DELAY_MILLISECONDS[4].count()))
		.Times(::testing::Exactly(1));
	// and again the sequence
	EXPECT_CALL(mockContext, sleep(initialSleep))
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, sleep(initialSleep * 2))
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, sleep(initialSleep * 4))
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, sleep(initialSleep * 8))
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, sleep(initialSleep * 16))
		.Times(::testing::Exactly(1));
	// delay between sixth and seventh attempt
	EXPECT_CALL(mockContext, sleep(communication::BeaconSendingInitialState::REINIT_DELAY_MILLISECONDS[4].count()))
		.Times(::testing::Exactly(1));
	// and again the sequence
	EXPECT_CALL(mockContext, sleep(initialSleep))
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, sleep(initialSleep * 2))
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, sleep(initialSleep * 4))
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, sleep(initialSleep * 8))
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, sleep(initialSleep * 
		16))
		.Times(::testing::Exactly(1));
		EXPECT_CALL(mockContext, sleep(communication::BeaconSendingInitialState::REINIT_DELAY_MILLISECONDS[4].count()))
		.Times(::testing::Exactly(1));

	// when executing the state multiple times (7 times)
	target.execute(mockContext);
}

TEST_F(BeaconSendingInitialStateTest, getStateNameReturnsCorrectStateName)
{
	// given
	auto target = BeaconSendingInitialState();

	// when
	auto stateName = target.getStateName();

	// then
	ASSERT_STREQ(stateName, "Initial");
}

TEST_F(BeaconSendingInitialStateTest, receivingTooManyRequestsResponseUsesSleepTimeFromResponse)
{
	// given
	auto responseHeaders = protocol::Response::ResponseHeaders
	{
		{ "retry-after", { "1234" } }
	};
	ON_CALL(*mMockHTTPClient, sendStatusRequestRawPtrProxy())
		.WillByDefault(testing::Invoke([&]() -> protocol::StatusResponse* {
			return new protocol::StatusResponse(mLogger, core::UTF8String(), 429, responseHeaders);
		}));

	testing::NiceMock<test::MockBeaconSendingContext> mockContext(mLogger);//NiceMock: ensure that required calls are there but do not object about other calls
	ON_CALL(mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mMockHTTPClient));

	auto target = BeaconSendingInitialState();

	// verify
	EXPECT_CALL(mockContext, isShutdownRequested())
		.WillOnce(testing::Return(false))
		.WillOnce(testing::Return(false))
		.WillRepeatedly(testing::Return(true));

	EXPECT_CALL(mockContext, sleep(1234 * 1000))
		.Times(testing::Exactly(1));

	// when
	target.execute(mockContext);
}

TEST_F(BeaconSendingInitialStateTest, receivingTooManyRequestsResponseDisablesCapturing)
{
	// given
	auto responseHeaders = protocol::Response::ResponseHeaders
	{
		{ "retry-after", { "1234" } }
	};
	ON_CALL(*mMockHTTPClient, sendStatusRequestRawPtrProxy())
		.WillByDefault(testing::Invoke([&]() -> protocol::StatusResponse* {
		return new protocol::StatusResponse(mLogger, core::UTF8String(), 429, responseHeaders);
	}));

	testing::NiceMock<test::MockBeaconSendingContext> mockContext(mLogger);//NiceMock: ensure that required calls are there but do not object about other calls
	ON_CALL(mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mMockHTTPClient));

	auto target = BeaconSendingInitialState();

	// verify
	EXPECT_CALL(mockContext, isShutdownRequested())
		.WillOnce(testing::Return(false))
		.WillOnce(testing::Return(false))
		.WillRepeatedly(testing::Return(true));

	EXPECT_CALL(mockContext, disableCapture())
		.Times(testing::Exactly(1));

	// when
	target.execute(mockContext);
}
