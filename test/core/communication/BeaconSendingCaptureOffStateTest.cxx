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
#include "../../protocol/mock/MockIHTTPClient.h"
#include "../../protocol/mock/MockIStatusResponse.h"

#include "core/communication/IBeaconSendingState.h"
#include "core/communication/BeaconSendingCaptureOffState.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace test;

using BeaconSendingCaptureOffState_t = core::communication::BeaconSendingCaptureOffState;
using IBeaconSendingState_t = core::communication::IBeaconSendingState;
using IBeaconSendingState_sp = std::shared_ptr<IBeaconSendingState_t>;
using IStatusResponse_t = protocol::IStatusResponse;
using MockNiceIBeaconSendingContext_sp = std::shared_ptr<testing::NiceMock<MockIBeaconSendingContext>>;
using MockNiceIHttpClient_sp = std::shared_ptr<testing::NiceMock<MockIHTTPClient>>;

class BeaconSendingCaptureOffStateTest : public testing::Test
{
protected:

	MockNiceIBeaconSendingContext_sp mockContext;
	MockNiceIHttpClient_sp mockHTTPClient;

	void SetUp()
	{
		mockHTTPClient = MockIHTTPClient::createNice();
		ON_CALL(*mockHTTPClient, sendStatusRequest())
			.WillByDefault(testing::Return(MockIStatusResponse::createNice()));

		mockContext = MockIBeaconSendingContext::createNice();
		ON_CALL(*mockContext, getHTTPClient())
			.WillByDefault(testing::Return(mockHTTPClient));
}

	void TearDown()
	{
	}

};

TEST_F(BeaconSendingCaptureOffStateTest, aBeaconSendingCaptureOffStateIsNotATerminalState)
{
	// given
	BeaconSendingCaptureOffState_t target;

	// when
	auto obtained = target.isTerminalState();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(BeaconSendingCaptureOffStateTest, getStateNameGivesStatesName)
{
	// given
	auto target = BeaconSendingCaptureOffState_t();

	// when
	auto obtained = target.getStateName();

	// then
	ASSERT_THAT(obtained, testing::StrEq("CaptureOff"));
}


TEST_F(BeaconSendingCaptureOffStateTest, aBeaconSendingCaptureOffStateHasTerminalStateBeaconSendingFlushSessions)
{
	// given
	BeaconSendingCaptureOffState_t target;

	// when
	auto obtained = target.getShutdownState();

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getStateType(), testing::Eq(IBeaconSendingState_t::StateType::BEACON_SENDING_FLUSH_SESSIONS_STATE));
}

TEST_F(BeaconSendingCaptureOffStateTest, aBeaconSendingCaptureOffStateTransitionsToCaptureOnStateWhenCapturingActive)
{
	// given
	ON_CALL(*mockContext, isCaptureOn())
		.WillByDefault(testing::Return(true));

	auto target = BeaconSendingCaptureOffState_t();

	// expect
	EXPECT_CALL(*mockContext, disableCapture())
		.Times(::testing::Exactly(1));
	EXPECT_CALL(*mockContext, setLastStatusCheckTime(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockContext, setNextState(IsABeaconSendingCaptureOnState()))
		.Times(testing::Exactly(1));

	// when
	target.execute(*mockContext);
}

TEST_F(BeaconSendingCaptureOffStateTest, getSleepTimeInMillisecondsReturnsMinusOneForDefaultConstructor)
{
	// given
	auto target = BeaconSendingCaptureOffState_t();

	// when
	auto obtained = target.getSleepTimeInMilliseconds();

	// then
	ASSERT_EQ(int64_t(-1), obtained);
}

TEST_F(BeaconSendingCaptureOffStateTest, getSleepTimeInMillisecondsReturnsSleepTimeSetInConstructor)
{
	// given
	int64_t sleepTime = 654321;
	auto target = BeaconSendingCaptureOffState_t(sleepTime);

	// when
	auto obtained = target.getSleepTimeInMilliseconds();

	// then
	ASSERT_THAT(obtained, testing::Eq(sleepTime));
}

TEST_F(BeaconSendingCaptureOffStateTest, aBeaconSendingCaptureOffStateWaitsForGivenTime)
{
	// with
	int64_t sleepTime = 1234;
	ON_CALL(*mockContext, isCaptureOn())
		.WillByDefault(testing::Return(true));

	// expect
	EXPECT_CALL(*mockContext, sleep(testing::Eq(sleepTime)))
		.Times(testing::Exactly(1));

	// given
	BeaconSendingCaptureOffState_t target(sleepTime);

	// when
	target.execute(*mockContext);
}

TEST_F(BeaconSendingCaptureOffStateTest, aBeaconSendingCaptureOffStateStaysInOffStateWhenServerRespondsWithTooManyRequests)
{
	// with
	int64_t sleepTime = 1234;
	auto statusResponse = MockIStatusResponse::createNice();
	ON_CALL(*statusResponse, getResponseCode())
		.WillByDefault(testing::Return(429));
	ON_CALL(*statusResponse, isTooManyRequestsResponse())
		.WillByDefault(testing::Return(true));
	ON_CALL(*statusResponse, isErroneousResponse())
		.WillByDefault(testing::Return(true));
	ON_CALL(*statusResponse, getRetryAfterInMilliseconds())
		.WillByDefault(testing::Return(sleepTime));

	ON_CALL(*mockHTTPClient, sendStatusRequest())
		.WillByDefault(testing::Return(statusResponse));

	ON_CALL(*mockContext, isCaptureOn())
		.WillByDefault(testing::Return(false));

	// expect
	IBeaconSendingState_sp savedNextState = nullptr;
	EXPECT_CALL(*mockContext, setNextState(IsABeaconSendingCaptureOffState()))
		.Times(testing::Exactly(1))
		.WillOnce(testing::SaveArg<0>(&savedNextState));

	// given
	auto target = BeaconSendingCaptureOffState_t(int64_t(12345));

	// when calling execute
	target.execute(*mockContext);

	// verify captured state
	ASSERT_THAT(savedNextState, testing::NotNull());
	ASSERT_THAT(
		std::static_pointer_cast<BeaconSendingCaptureOffState_t>(savedNextState)->getSleepTimeInMilliseconds(),
		testing::Eq(sleepTime)
	);
}

TEST_F(BeaconSendingCaptureOffStateTest, aBeaconSendingCaptureOffStateDoesDoesNotExecuteStatusRequestWhenInterruptedDuringSleep)
{
	// with
	ON_CALL(*mockContext, isCaptureOn())
		.WillByDefault(testing::Return(false));
	EXPECT_CALL(*mockContext, isShutdownRequested())
		.WillOnce(testing::Return(false))
		.WillRepeatedly(testing::Return(true));

	// expect
	EXPECT_CALL(*mockContext, disableCapture())
		.Times(::testing::Exactly(1));
	// also verify that lastStatusCheckTime was updated
	EXPECT_CALL(*mockContext, setLastStatusCheckTime(testing::_))
		.Times(testing::Exactly(0));
	// verify the sleep - since this is not multi-threaded, the sleep time is still the full time
	EXPECT_CALL(*mockContext, sleep(7200000L))
		.Times(testing::Exactly(1));
	// verify that after sleeping the transition to IsABeaconSendingFlushSessionsState works
	EXPECT_CALL(*mockContext, setNextState(IsABeaconSendingFlushSessionsState()))
		.Times(testing::Exactly(1));

	// given
	auto target = BeaconSendingCaptureOffState_t();

	// when calling execute
	target.execute(*mockContext);
}