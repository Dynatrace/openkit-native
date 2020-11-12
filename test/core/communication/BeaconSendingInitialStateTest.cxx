/**
 * Copyright 2018-2020 Dynatrace LLC
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
#include "../../protocol/mock/MockIHTTPClient.h"
#include "../../protocol/mock/MockIStatusResponse.h"
#include "../../protocol/mock/MockIResponseAttributes.h"

#include "core/communication/BeaconSendingInitialState.h"
#include "protocol/IStatusResponse.h"
#include "protocol/StatusResponse.h"
#include "protocol/IResponseAttributes.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace test;

using BeaconSendingInitialState_t = core::communication::BeaconSendingInitialState;
using MockNiceIBeaconSendingContext_sp = std::shared_ptr<testing::NiceMock<MockIBeaconSendingContext>>;
using MockNiceIHTTPClient_sp = std::shared_ptr<testing::NiceMock<MockIHTTPClient>>;
using MockNiceIStatusResponse_sp = std::shared_ptr<testing::NiceMock<MockIStatusResponse>>;
using MockNiceIResponseAttributes_sp = std::shared_ptr<testing::NiceMock<MockIResponseAttributes>>;

class BeaconSendingInitialStateTest : public testing::Test
{
protected:

	MockNiceIHTTPClient_sp mockHTTPClient;
	MockNiceIBeaconSendingContext_sp mockContext;
	MockNiceIStatusResponse_sp mockStatusResponse;
	MockNiceIResponseAttributes_sp mockAttributes;

	void SetUp() override
	{
		mockAttributes = MockIResponseAttributes::createNice();

		mockStatusResponse = MockIStatusResponse::createNice();
		ON_CALL(*mockStatusResponse, getResponseAttributes())
			.WillByDefault(testing::Return(mockAttributes));

		mockHTTPClient = MockIHTTPClient::createNice();
		ON_CALL(*mockHTTPClient, sendStatusRequest(testing::_))
			.WillByDefault(testing::Return(mockStatusResponse));

		mockContext = MockIBeaconSendingContext::createNice();
		ON_CALL(*mockContext, getHTTPClient())
			.WillByDefault(testing::Return(mockHTTPClient));
	}

};

TEST_F(BeaconSendingInitialStateTest, initStateIsNotATerminalState)
{
	// given
	BeaconSendingInitialState_t target;

	// when
	auto obtained = target.isTerminalState();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(BeaconSendingInitialStateTest, getStateNameReturnsCorrectStateName)
{
	// given
	BeaconSendingInitialState_t target;

	// when
	auto obtained = target.getStateName();

	// then
	ASSERT_THAT(obtained, testing::StrEq("Initial"));
}

TEST_F(BeaconSendingInitialStateTest, getShutdownStateGivesABeaconSendingTerminalStateInstance)
{
	// given
	BeaconSendingInitialState_t target;

	// when
	auto obtained = target.getShutdownState();

	//then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getShutdownState(), IsABeaconSendingTerminalState());
}

TEST_F(BeaconSendingInitialStateTest, getShutdownStateAlwaysCreatesANewInstance)
{
	// given
	BeaconSendingInitialState_t target;

	// when
	auto obtainedOne = target.getShutdownState();
	auto obtainedTwo = target.getShutdownState();

	//then
	ASSERT_THAT(obtainedOne, testing::NotNull());
	ASSERT_THAT(obtainedTwo, testing::NotNull());
	ASSERT_THAT(obtainedOne, testing::Ne(obtainedTwo));
}

TEST_F(BeaconSendingInitialStateTest, executeSetsLastOpenSessionBeaconSendTime)
{
	// with
	int64_t timestamp = 123456789;
	ON_CALL(*mockContext, getCurrentTimestamp())
		.WillByDefault(testing::Return(timestamp));
	EXPECT_CALL(*mockContext, isShutdownRequested())
		.WillOnce(testing::Return(false))
		.WillRepeatedly(testing::Return(true));

	// expect
	EXPECT_CALL(*mockContext, setLastOpenSessionBeaconSendTime(testing::Eq(timestamp)))
		.Times(1);

	// given
	BeaconSendingInitialState_t target;

	// when
	target.execute(*mockContext);
}

TEST_F(BeaconSendingInitialStateTest, executeSetsLastStatusCheckTime)
{
	// with
	int64_t timestamp = 123456789;
	ON_CALL(*mockContext, getCurrentTimestamp())
		.WillByDefault(testing::Return(timestamp));
	EXPECT_CALL(*mockContext, isShutdownRequested())
		.WillOnce(testing::Return(false))
		.WillRepeatedly(testing::Return(true));

	// expect
	EXPECT_CALL(*mockContext, setLastStatusCheckTime(testing::Eq(timestamp)))
		.Times(1);

	// given
	BeaconSendingInitialState_t target;

	// when
	target.execute(*mockContext);
}

TEST_F(BeaconSendingInitialStateTest, initIsTerminatedIfShutdownRequestedWithValidResponse)
{
	// with
	ON_CALL(*mockHTTPClient, sendStatusRequest(testing::_))
		.WillByDefault(testing::Return(MockIStatusResponse::createNice()));
	ON_CALL(*mockContext, isShutdownRequested())
		.WillByDefault(testing::Return(true));

	// expect
	EXPECT_CALL(*mockContext, setInitCompleted(false))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockContext, setNextState(IsABeaconSendingTerminalState()))
		.Times(testing::Exactly(1));

	// given
	BeaconSendingInitialState_t target;

	// when
	target.execute(*mockContext);
}

TEST_F(BeaconSendingInitialStateTest, reinitializeSleepsBeforeSendingStatusRequestsAgain)
{
	// given
	auto errorResponse = MockIStatusResponse::createNice();
	ON_CALL(*errorResponse, getResponseCode())
			.WillByDefault(testing::Return(400));
	ON_CALL(*errorResponse, isErroneousResponse())
			.WillByDefault(testing::Return(true));

	ON_CALL(*mockHTTPClient, sendStatusRequest(testing::_))
			.WillByDefault(testing::Return(errorResponse));

	uint32_t callCount = 0;
	ON_CALL(*mockContext, isShutdownRequested())
		.WillByDefault(testing::Invoke(
			[&callCount]() -> bool {
				return callCount++ >= 49;
			}
		)); //should return true the 49th time

	// expect
	{
		testing::InSequence s;

		int64_t initialSleep = BeaconSendingInitialState_t::getInitialRetrySleepTimeMilliseconds().count();

		// from first round
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep))).Times(1);
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep * 2))).Times(1);
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep * 4))).Times(1);
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep * 8))).Times(1);
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep * 16))).Times(1);
		// delay between first and second attempt
		EXPECT_CALL(*mockContext, sleep(testing::Eq(BeaconSendingInitialState_t::getReInitDelayMilliseconds()[0].count())))
				.Times(1);
		// and again
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep))).Times(1);
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep * 2))).Times(1);
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep * 4))).Times(1);
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep * 8))).Times(1);
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep * 16))).Times(1);
		// delay between second and third attempt
		EXPECT_CALL(*mockContext, sleep(testing::Eq(BeaconSendingInitialState_t::getReInitDelayMilliseconds()[1].count())))
				.Times(1);
		// and again
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep))).Times(1);
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep * 2))).Times(1);
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep * 4))).Times(1);
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep * 8))).Times(1);
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep * 16))).Times(1);
		// delay between third and fourth attempt
		EXPECT_CALL(*mockContext, sleep(testing::Eq(BeaconSendingInitialState_t::getReInitDelayMilliseconds()[2].count())))
				.Times(1);
		// and again
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep))).Times(1);
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep * 2))).Times(1);
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep * 4))).Times(1);
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep * 8))).Times(1);
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep * 16))).Times(1);
		// delay between fourth and fifth attempt
		EXPECT_CALL(*mockContext, sleep(testing::Eq(BeaconSendingInitialState_t::getReInitDelayMilliseconds()[3].count())))
				.Times(1);
		// and again
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep))).Times(1);
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep * 2))).Times(1);
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep * 4))).Times(1);
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep * 8))).Times(1);
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep * 16))).Times(1);
		// delay between fifth and sixth attempt
		EXPECT_CALL(*mockContext, sleep(testing::Eq(BeaconSendingInitialState_t::getReInitDelayMilliseconds()[4].count())))
				.Times(1);
		// and again
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep))).Times(1);
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep * 2))).Times(1);
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep * 4))).Times(1);
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep * 8))).Times(1);
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep * 16))).Times(1);
		// delay between sixth and seventh attempt
		EXPECT_CALL(*mockContext, sleep(testing::Eq(BeaconSendingInitialState_t::getReInitDelayMilliseconds()[4].count())))
				.Times(1);
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep))).Times(1);
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep * 2))).Times(1);
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep * 4))).Times(1);
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep * 8))).Times(1);
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep * 16))).Times(1);

		EXPECT_CALL(*mockContext, sleep(BeaconSendingInitialState_t::getReInitDelayMilliseconds()[4].count()))
			.Times(::testing::Exactly(1));

		EXPECT_CALL(*mockContext, sleep(testing::_))
			.Times(0);
	}

	// given
	BeaconSendingInitialState_t target;

	// when
	target.execute(*mockContext);
}

TEST_F(BeaconSendingInitialStateTest, sleepTimeIsDoubledBetweenStatusRequestRetries)
{
	// with
	auto errorResponse = MockIStatusResponse::createNice();
	ON_CALL(*errorResponse, getResponseCode())
			.WillByDefault(testing::Return(400));
	ON_CALL(*errorResponse, isErroneousResponse())
			.WillByDefault(testing::Return(true));

	ON_CALL(*mockHTTPClient, sendStatusRequest(testing::_))
			.WillByDefault(testing::Return(errorResponse));

	uint32_t callCount = 0;

	ON_CALL(*mockContext, isShutdownRequested())
		.WillByDefault(testing::Invoke(
			[&callCount]() -> bool {
		return callCount++ >= 6;
	}
	));

	// expect
	{
		testing::InSequence s;

		int64_t initialSleep = BeaconSendingInitialState_t::getInitialRetrySleepTimeMilliseconds().count();
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep))).Times(1);
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep * 2))).Times(1);
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep * 4))).Times(1);
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep * 8))).Times(1);
		EXPECT_CALL(*mockContext, sleep(testing::Eq(initialSleep * 16))).Times(1);

		EXPECT_CALL(*mockContext, sleep(BeaconSendingInitialState_t::getReInitDelayMilliseconds()[0].count()))
			.Times(::testing::Exactly(1));

		EXPECT_CALL(*mockContext, sleep(testing::_))
			.Times(0);
	}

	// given
	BeaconSendingInitialState_t target;

	// when
	target.execute(*mockContext);
}

TEST_F(BeaconSendingInitialStateTest, initialStatusRequestGivesUpWhenShutdownRequestIsSetDuringExecution)
{
	// with
	auto errorResponse = MockIStatusResponse::createNice();
	ON_CALL(*errorResponse, getResponseCode())
			.WillByDefault(testing::Return(400));
	ON_CALL(*errorResponse, isErroneousResponse())
			.WillByDefault(testing::Return(true));

	ON_CALL(*mockHTTPClient, sendStatusRequest(testing::_))
			.WillByDefault(testing::Return(errorResponse));

	uint32_t callCount = 0;
	//given
	ON_CALL(*mockContext, isShutdownRequested())
		.WillByDefault(testing::Invoke(
			[&callCount]() -> bool {
		return callCount++ >= 4;
	}
	));//should return true the 3rd time


	// expect
	EXPECT_CALL(*mockContext, setInitCompleted(testing::Eq(false)))
		.Times(1);
	EXPECT_CALL(*mockContext, setNextState(IsABeaconSendingTerminalState()))
		.Times(1);
	EXPECT_CALL(*mockContext, getHTTPClient())
		.Times(3);
	EXPECT_CALL(*mockHTTPClient, sendStatusRequest(testing::Ref(*mockContext)))
		.Times(3);

	EXPECT_CALL(*mockContext, sleep(testing::_))
		.Times(4);

	// given
	BeaconSendingInitialState_t target;

	// when
	target.execute(*mockContext);
}

TEST_F(BeaconSendingInitialStateTest, aSuccessfulStatusResponseSetsInitCompletedToTrueForCaptureOn)
{
	// with
	ON_CALL(*mockAttributes, isCapture())
		.WillByDefault(testing::Return(true));

	// expect
	EXPECT_CALL(*mockContext, setInitCompleted(true))
			.Times(1);

	// given
	BeaconSendingInitialState_t target;

	// when
	target.execute(*mockContext);
}

TEST_F(BeaconSendingInitialStateTest, aSuccessfulStatusResponseSetsInitCompletedToTrueForCaptureOff)
{
	// with
	ON_CALL(*mockAttributes, isCapture())
		.WillByDefault(testing::Return(true));

	// expect
	EXPECT_CALL(*mockContext, setInitCompleted(true))
		.Times(1);

	// given
	BeaconSendingInitialState_t target;

	// when
	target.execute(*mockContext);
}

TEST_F(BeaconSendingInitialStateTest, aSuccessfulStatusResponsePerformsStateTransitionToCaptureOnStateIfCapturingIsEnabled)
{
	// with
	ON_CALL(*mockContext, isCaptureOn())
		.WillByDefault(testing::Return(true));

	// expect
	EXPECT_CALL(*mockContext, handleStatusResponse(testing::Eq(mockStatusResponse)))
		.Times(1);
	EXPECT_CALL(*mockContext, setNextState(IsABeaconSendingCaptureOnState()))
		.Times(::testing::Exactly(1));

	// given
	BeaconSendingInitialState_t target;

	// when
	target.execute(*mockContext);
}

TEST_F(BeaconSendingInitialStateTest, aSuccessfulStatusResponsePerformsStateTransitionToCaptureOffStateIfCapturingIsDisabled)
{
	// with
	ON_CALL(*mockContext, isCaptureOn())
		.WillByDefault(testing::Return(false));

	// expect
	EXPECT_CALL(*mockContext, handleStatusResponse(testing::Eq(mockStatusResponse)))
		.Times(1);
	EXPECT_CALL(*mockContext, setNextState(IsABeaconSendingCaptureOffState()))
		.Times(::testing::Exactly(1));

	// given
	auto target = BeaconSendingInitialState_t();

	// when
	target.execute(*mockContext);
}

TEST_F(BeaconSendingInitialStateTest, receivingTooManyRequestsResponseUsesSleepTimeFromResponse)
{
	// with
	int64_t sleepTime = 1234;
	auto errorResponse = MockIStatusResponse::createNice();
	ON_CALL(*errorResponse, getResponseCode())
		.WillByDefault(testing::Return(429));
	ON_CALL(*errorResponse, isTooManyRequestsResponse())
		.WillByDefault(testing::Return(true));
	ON_CALL(*errorResponse, isErroneousResponse())
		.WillByDefault(testing::Return(true));
	ON_CALL(*errorResponse, getRetryAfterInMilliseconds())
		.WillByDefault(testing::Return(sleepTime));

	ON_CALL(*mockHTTPClient, sendStatusRequest(testing::_))
		.WillByDefault(testing::Return(errorResponse));
	EXPECT_CALL(*mockContext, isShutdownRequested())
		.WillOnce(testing::Return(false))
		.WillOnce(testing::Return(false))
		.WillRepeatedly(testing::Return(true));

	// expect
	EXPECT_CALL(*mockContext, sleep(testing::Eq(sleepTime)))
		.Times(1);

	// given
	BeaconSendingInitialState_t target;

	// when
	target.execute(*mockContext);
}

TEST_F(BeaconSendingInitialStateTest, receivingTooManyRequestsResponseDisablesCapturing)
{
	// with
	int64_t sleepTime = 1234;
	auto errorResponse = MockIStatusResponse::createNice();
	ON_CALL(*errorResponse, getResponseCode())
		.WillByDefault(testing::Return(429));
	ON_CALL(*errorResponse, isTooManyRequestsResponse())
		.WillByDefault(testing::Return(true));
	ON_CALL(*errorResponse, isErroneousResponse())
		.WillByDefault(testing::Return(true));
	ON_CALL(*errorResponse, getRetryAfterInMilliseconds())
		.WillByDefault(testing::Return(sleepTime));

	ON_CALL(*mockHTTPClient, sendStatusRequest(testing::_))
		.WillByDefault(testing::Return(errorResponse));
	EXPECT_CALL(*mockContext, isShutdownRequested())
		.WillOnce(testing::Return(false))
		.WillOnce(testing::Return(false))
		.WillRepeatedly(testing::Return(true));

	// expect
	EXPECT_CALL(*mockContext, disableCaptureAndClear())
		.Times(1);

	// given
	BeaconSendingInitialState_t target;

	// when
	target.execute(*mockContext);
}
