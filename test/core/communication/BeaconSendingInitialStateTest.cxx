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
#include "MockBeaconSendingContext.h"
#include "../../api/MockILogger.h"
#include "../../protocol/mock/MockIHTTPClient.h"
#include "../../protocol/mock/MockIStatusResponse.h"

#include "core/UTF8String.h"
#include "core/communication/AbstractBeaconSendingState.h"
#include "core/communication/BeaconSendingInitialState.h"
#include "protocol/IStatusResponse.h"
#include "protocol/StatusResponse.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace test;

using AbstractBeaconSendingState_t = core::communication::AbstractBeaconSendingState;
using AbstractBeaconSendingState_sp = std::shared_ptr<AbstractBeaconSendingState_t>;
using BeaconSendingInitialState_t = core::communication::BeaconSendingInitialState;
using IStatusResponse_t = protocol::IStatusResponse;
using MockNiceBeaconSendingContext_t = testing::NiceMock<MockBeaconSendingContext>;
using MockNiceIHTTPClient_sp = std::shared_ptr<testing::NiceMock<MockIHTTPClient>>;
using MockNiceILogger_sp = std::shared_ptr<testing::NiceMock<MockILogger>>;
using StatusResponse_t = protocol::StatusResponse;
using StatusResponse_sp = std::shared_ptr<StatusResponse_t>;
using Utf8String_t = core::UTF8String;

class BeaconSendingInitialStateTest : public testing::Test
{
protected:

	MockNiceIHTTPClient_sp mockHTTPClient;
	MockNiceILogger_sp mockLogger;

	void SetUp()
	{
		mockLogger = MockILogger::createNice();
		mockHTTPClient = MockIHTTPClient::createNice();

		auto mockStatusResponse = MockIStatusResponse::createNice();
		ON_CALL(*mockHTTPClient, sendStatusRequest())
			.WillByDefault(testing::Return(MockIStatusResponse::createNice()));
	}

};

TEST_F(BeaconSendingInitialStateTest, initStateIsNotATerminalState)
{
	// given
	auto target = BeaconSendingInitialState_t();

	// then
	EXPECT_FALSE(target.isTerminalState());
}

TEST_F(BeaconSendingInitialStateTest, getShutdownStateGivesABeaconSendingTerminalStateInstance)
{
	// given
	auto target = BeaconSendingInitialState_t();

	// when
	AbstractBeaconSendingState_sp obtained = target.getShutdownState();

	//then
	ASSERT_TRUE(obtained != nullptr);
	ASSERT_EQ(obtained->getStateType(), AbstractBeaconSendingState_t::StateType::BEACON_SENDING_TERMINAL_STATE);
}

TEST_F(BeaconSendingInitialStateTest, getShutdownStateAlwaysCreatesANewInstance)
{
	// given
	auto target = BeaconSendingInitialState_t();

	// when calling getShutDownState twice
	AbstractBeaconSendingState_sp obtainedOne = target.getShutdownState();
	AbstractBeaconSendingState_sp obtainedTwo = target.getShutdownState();

	//then
	ASSERT_TRUE(obtainedOne != nullptr);
	ASSERT_TRUE(obtainedTwo != nullptr);
	EXPECT_NE(obtainedOne, obtainedTwo);
}

TEST_F(BeaconSendingInitialStateTest, executeSetsLastOpenSessionBeaconSendTime)
{
	// given
	auto target = BeaconSendingInitialState_t();

	MockNiceBeaconSendingContext_t mockContext(mockLogger);//NiceMock: ensure that required calls are there but do not object about other calls

	ON_CALL(mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mockHTTPClient));
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
	auto target = BeaconSendingInitialState_t();

	MockNiceBeaconSendingContext_t mockContext(mockLogger);//NiceMock: ensure that required calls are there but do not object about other calls

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
	auto target = BeaconSendingInitialState_t();

	MockNiceBeaconSendingContext_t mockContext(mockLogger);//NiceMock: ensure that required calls are there but do not object about other calls

	ON_CALL(mockContext, isShutdownRequested())
		.WillByDefault(testing::Return(true));
	ON_CALL(mockContext, setNextState(testing::_))
		.WillByDefault(testing::WithArgs<0>(testing::Invoke(&mockContext, &MockBeaconSendingContext::RealSetNextState)));
	ON_CALL(mockContext, isInTerminalState())
		.WillByDefault(testing::Invoke(&mockContext, &MockBeaconSendingContext::RealIsInTerminalState));

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
	auto target = BeaconSendingInitialState_t();

	MockNiceBeaconSendingContext_t mockContext(mockLogger);

	uint32_t callCount = 0;

	ON_CALL(mockContext, isShutdownRequested())
		.WillByDefault(testing::Invoke(
			[&callCount]() -> bool {
		return callCount++ >= 6;
	}
	));//should return true the 6th time

	ON_CALL(mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mockHTTPClient));
	ON_CALL(*mockHTTPClient, sendStatusRequest())
		.WillByDefault(testing::Invoke([&]() ->StatusResponse_sp {
			return std::make_shared<StatusResponse_t>(mockLogger, Utf8String_t(), 400, IStatusResponse_t::ResponseHeaders());
		}));

	// check for
	uint64_t initialSleep = BeaconSendingInitialState_t::INITIAL_RETRY_SLEEP_TIME_MILLISECONDS.count();
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
	EXPECT_CALL(mockContext, sleep(BeaconSendingInitialState_t::REINIT_DELAY_MILLISECONDS[0].count()))
		.Times(::testing::Exactly(1));

	// when executing the state
	target.execute(mockContext);
}

TEST_F(BeaconSendingInitialStateTest, initialStatusRequestGivesUpWhenShutdownRequestIsSetDuringExecution)
{
	// given
	auto target = BeaconSendingInitialState_t();

	MockNiceBeaconSendingContext_t mockContext(mockLogger);//NiceMock: ensure that required calls are there but do not object about other calls

	ON_CALL(mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mockHTTPClient));
	ON_CALL(*mockHTTPClient, sendStatusRequest())
		.WillByDefault(testing::Invoke([&]() -> StatusResponse_sp {
			return std::make_shared<StatusResponse_t>(mockLogger, Utf8String_t(), 400, IStatusResponse_t::ResponseHeaders());
		}));
	ON_CALL(mockContext, setNextState(testing::_))
		.WillByDefault(testing::WithArgs<0>(testing::Invoke(&mockContext, &MockBeaconSendingContext::RealSetNextState)));

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
	auto target = BeaconSendingInitialState_t();

	MockNiceBeaconSendingContext_t mockContext(mockLogger);//NiceMock: ensure that required calls are there but do not object about other calls

	ON_CALL(mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mockHTTPClient));
	ON_CALL(mockContext, isCaptureOn())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mockHTTPClient, sendStatusRequest())
		.WillByDefault(testing::Invoke([&]() -> StatusResponse_sp {
		return std::make_shared<StatusResponse_t>(mockLogger, Utf8String_t("type=m&cp=1"), 200, IStatusResponse_t::ResponseHeaders());
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
	auto target = BeaconSendingInitialState_t();

	MockNiceBeaconSendingContext_t mockContext(mockLogger);//NiceMock: ensure that required calls are there but do not object about other calls

	ON_CALL(mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mockHTTPClient));
	ON_CALL(mockContext, isCaptureOn())
		.WillByDefault(testing::Return(false));
	ON_CALL(*mockHTTPClient, sendStatusRequest())
		.WillByDefault(testing::Invoke([&]() -> StatusResponse_sp {
		return std::make_shared<StatusResponse_t>(mockLogger, Utf8String_t("type=m&cp=0"), 200, IStatusResponse_t::ResponseHeaders());
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
	auto target = BeaconSendingInitialState_t();

	MockNiceBeaconSendingContext_t mockContext(mockLogger);//NiceMock: ensure that required calls are there but do not object about other calls

	ON_CALL(mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mockHTTPClient));
	ON_CALL(mockContext, isCaptureOn())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mockHTTPClient, sendStatusRequest())
		.WillByDefault(testing::Invoke([&]() -> StatusResponse_sp {
		return std::make_shared<StatusResponse_t>(mockLogger, Utf8String_t("type=m&cp=1"), 200, IStatusResponse_t::ResponseHeaders());
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
	auto target = BeaconSendingInitialState_t();

	MockNiceBeaconSendingContext_t mockContext(mockLogger);//NiceMock: ensure that required calls are there but do not object about other calls

	ON_CALL(mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mockHTTPClient));
	ON_CALL(mockContext, isCaptureOn())
		.WillByDefault(testing::Return(false));
	ON_CALL(*mockHTTPClient, sendStatusRequest())
		.WillByDefault(testing::Invoke([&]() -> StatusResponse_sp {
		return std::make_shared<StatusResponse_t>(mockLogger, Utf8String_t("type=m&cp=0"), 200, IStatusResponse_t::ResponseHeaders());
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
	auto target = BeaconSendingInitialState_t();

	MockNiceBeaconSendingContext_t mockContext(mockLogger);//NiceMock: ensure that required calls are there but do not object about other calls

	uint32_t callCount = 0;

	ON_CALL(mockContext, isShutdownRequested())
		.WillByDefault(testing::Invoke(
			[&callCount]() -> bool {
				return callCount++ >= 49;
			}
		));//should return true the 49th time

	ON_CALL(mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mockHTTPClient));
	ON_CALL(*mockHTTPClient, sendStatusRequest())
		.WillByDefault(testing::Invoke([&]() -> StatusResponse_sp
		{
			return std::make_shared<StatusResponse_t>(mockLogger, Utf8String_t(), 400, IStatusResponse_t::ResponseHeaders());
		})
	);

	testing::InSequence s;

	uint64_t initialSleep = BeaconSendingInitialState_t::INITIAL_RETRY_SLEEP_TIME_MILLISECONDS.count();

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
	EXPECT_CALL(mockContext, sleep(BeaconSendingInitialState_t::REINIT_DELAY_MILLISECONDS[0].count()))
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
	EXPECT_CALL(mockContext, sleep(BeaconSendingInitialState_t::REINIT_DELAY_MILLISECONDS[1].count()))
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
	EXPECT_CALL(mockContext, sleep(BeaconSendingInitialState_t::REINIT_DELAY_MILLISECONDS[2].count()))
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
	EXPECT_CALL(mockContext, sleep(BeaconSendingInitialState_t::REINIT_DELAY_MILLISECONDS[3].count()))
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
	EXPECT_CALL(mockContext, sleep(BeaconSendingInitialState_t::REINIT_DELAY_MILLISECONDS[4].count()))
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
	EXPECT_CALL(mockContext, sleep(BeaconSendingInitialState_t::REINIT_DELAY_MILLISECONDS[4].count()))
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
		EXPECT_CALL(mockContext, sleep(BeaconSendingInitialState_t::REINIT_DELAY_MILLISECONDS[4].count()))
		.Times(::testing::Exactly(1));

	// when executing the state multiple times (7 times)
	target.execute(mockContext);
}

TEST_F(BeaconSendingInitialStateTest, getStateNameReturnsCorrectStateName)
{
	// given
	auto target = BeaconSendingInitialState_t();

	// when
	auto stateName = target.getStateName();

	// then
	ASSERT_STREQ(stateName, "Initial");
}

TEST_F(BeaconSendingInitialStateTest, receivingTooManyRequestsResponseUsesSleepTimeFromResponse)
{
	// given
	auto responseHeaders = IStatusResponse_t::ResponseHeaders
	{
		{ "retry-after", { "1234" } }
	};
	ON_CALL(*mockHTTPClient, sendStatusRequest())
		.WillByDefault(testing::Invoke([&]() -> StatusResponse_sp
		{
			return std::make_shared<StatusResponse_t>(mockLogger, Utf8String_t(), 429, responseHeaders);
		})
	);

	MockNiceBeaconSendingContext_t mockContext(mockLogger);//NiceMock: ensure that required calls are there but do not object about other calls
	ON_CALL(mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mockHTTPClient));

	auto target = BeaconSendingInitialState_t();

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
	auto responseHeaders = IStatusResponse_t::ResponseHeaders
	{
		{ "retry-after", { "1234" } }
	};
	ON_CALL(*mockHTTPClient, sendStatusRequest())
		.WillByDefault(testing::Invoke([&]() -> StatusResponse_sp
		{
			return std::make_shared<StatusResponse_t>(mockLogger, Utf8String_t(), 429, responseHeaders);
		})
	);

	MockNiceBeaconSendingContext_t mockContext(mockLogger);//NiceMock: ensure that required calls are there but do not object about other calls
	ON_CALL(mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mockHTTPClient));

	auto target = BeaconSendingInitialState_t();

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
