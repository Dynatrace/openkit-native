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
#include "../../api/mock/MockILogger.h"
#include "../../protocol/mock/MockIHTTPClient.h"
#include "../../protocol/mock/MockIStatusResponse.h"

#include "core/communication/AbstractBeaconSendingState.h"
#include "core/communication/BeaconSendingCaptureOffState.h"
#include "protocol/IStatusResponse.h"
#include "protocol/StatusResponse.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace test;

using AbstractBeaconSendingState_t = core::communication::AbstractBeaconSendingState;
using AbstractBeaconSendingState_sp = std::shared_ptr<AbstractBeaconSendingState_t>;
using BeaconSendingCaptureOffState_t = core::communication::BeaconSendingCaptureOffState;
using IStatusResponse_t = protocol::IStatusResponse;
using MockNiceILogger_sp = std::shared_ptr<testing::NiceMock<MockILogger>>;
using MockNiceIHttpClient_sp = std::shared_ptr<testing::NiceMock<MockIHTTPClient>>;
using MockNiceBeaconSendingContext_t = testing::NiceMock<MockBeaconSendingContext>;
using StatusResponse_t = protocol::StatusResponse;

class BeaconSendingCaptureOffStateTest : public testing::Test
{
protected:
	MockNiceILogger_sp mockLogger;
	MockNiceIHttpClient_sp mockHTTPClient;

	void SetUp()
	{
		mockLogger = MockILogger::createNice();

		mockHTTPClient = MockIHTTPClient::createNice();
		ON_CALL(*mockHTTPClient, sendStatusRequest())
			.WillByDefault(testing::Return(MockIStatusResponse::createNice()));
}

	void TearDown()
	{
	}

};

TEST_F(BeaconSendingCaptureOffStateTest, getStateNameGivesStatesName)
{
	// given
	auto target = BeaconSendingCaptureOffState_t();

	// when
	auto stateName = target.getStateName();

	// then
	ASSERT_STREQ(stateName, "CaptureOff");
}

TEST_F(BeaconSendingCaptureOffStateTest, aBeaconSendingCaptureOffStateIsNotATerminalState)
{
	// given
	auto target = BeaconSendingCaptureOffState_t();

	// verify that BeaconSendingCaptureOffState is not a terminal state
	EXPECT_FALSE(target.isTerminalState());
}

TEST_F(BeaconSendingCaptureOffStateTest, aBeaconSendingCaptureOffStateHasTerminalStateBeaconSendingFlushSessions)
{
	// given
	auto target = BeaconSendingCaptureOffState_t();

	// when
	AbstractBeaconSendingState_sp obtained = target.getShutdownState();

	// verify that terminal state is BeaconSendingFlushSessions
	ASSERT_TRUE(obtained != nullptr);
	ASSERT_EQ(obtained->getStateType(), AbstractBeaconSendingState_t::StateType::BEACON_SENDING_FLUSH_SESSIONS_STATE);
}

TEST_F(BeaconSendingCaptureOffStateTest, aBeaconSendingCaptureOffStateTransitionsToCaptureOnStateWhenCapturingActive)
{
	// given
	auto target = BeaconSendingCaptureOffState_t();

	MockNiceBeaconSendingContext_t mockContext(mockLogger);
	ON_CALL(mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mockHTTPClient));
	ON_CALL(mockContext, isCaptureOn())
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
	target.execute(mockContext);
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
	auto target = BeaconSendingCaptureOffState_t(int64_t(654321));

	// when
	auto obtained = target.getSleepTimeInMilliseconds();

	// then
	ASSERT_EQ(int64_t(654321), obtained);
}

TEST_F(BeaconSendingCaptureOffStateTest, aBeaconSendingCaptureOffStateWaitsForGivenTime)
{
	// given
	auto target = BeaconSendingCaptureOffState_t(int64_t(12345));

	MockNiceBeaconSendingContext_t mockContext(mockLogger);
	ON_CALL(mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mockHTTPClient));
	ON_CALL(mockContext, isCaptureOn())
		.WillByDefault(testing::Return(true));

	// verify the custom amount of time was waited
	EXPECT_CALL(mockContext, sleep(int64_t(12345)))
		.Times(testing::Exactly(1));

	// when calling execute
	target.execute(mockContext);
}

TEST_F(BeaconSendingCaptureOffStateTest, aBeaconSendingCaptureOffStateStaysInOffStateWhenServerRespondsWithTooManyRequests)
{
	// given
	auto target = BeaconSendingCaptureOffState_t(int64_t(12345));

	auto responseHeader = IStatusResponse_t::ResponseHeaders
	{
		{ "retry-after",  { "123456" } }
	};
	auto statusResponse = std::make_shared<StatusResponse_t>(mockLogger, "", 429, responseHeader);

	ON_CALL(*mockHTTPClient, sendStatusRequest())
		.WillByDefault(testing::Return(statusResponse));

	MockNiceBeaconSendingContext_t mockContext(mockLogger);
	ON_CALL(mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mockHTTPClient));
	ON_CALL(mockContext, isCaptureOn())
		.WillByDefault(testing::Return(false));

	// verify calls
	AbstractBeaconSendingState_sp savedNextState = nullptr;
	EXPECT_CALL(mockContext, setNextState(IsABeaconSendingCaptureOffState()))
		.Times(testing::Exactly(1))
		.WillOnce(testing::SaveArg<0>(&savedNextState));

	// when calling execute
	target.execute(mockContext);

	// verify captured state
	ASSERT_NE(nullptr, savedNextState);
	ASSERT_EQ(int64_t(123456 * 1000), std::static_pointer_cast<BeaconSendingCaptureOffState_t>(savedNextState)->getSleepTimeInMilliseconds());
}

TEST_F(BeaconSendingCaptureOffStateTest, aBeaconSendingCaptureOffStateDoesDoesNotExecuteStatusRequestWhenInterruptedDuringSleep)
{
	// given
	auto target = BeaconSendingCaptureOffState_t();

	MockNiceBeaconSendingContext_t mockContext(mockLogger);
	ON_CALL(mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mockHTTPClient));
	ON_CALL(mockContext, isCaptureOn())
		.WillByDefault(testing::Return(false));
	EXPECT_CALL(mockContext, isShutdownRequested())
		.WillOnce(testing::Return(false))
		.WillRepeatedly(testing::Return(true));

	// then verify that capturing is set to disabled
	EXPECT_CALL(mockContext, disableCapture())
		.Times(::testing::Exactly(1));
	// also verify that lastStatusCheckTime was updated
	EXPECT_CALL(mockContext, setLastStatusCheckTime(testing::_))
		.Times(testing::Exactly(0));
	// verify the sleep - since this is not multi-threaded, the sleep time is stil the full time
	EXPECT_CALL(mockContext, sleep(7200000L))
		.Times(testing::Exactly(1));
	// verify that after sleeping the transition to IsABeaconSendingFlushSessionsState works
	EXPECT_CALL(mockContext, setNextState(IsABeaconSendingFlushSessionsState()))
		.Times(testing::Exactly(1));

	// when calling execute
	target.execute(mockContext);
}