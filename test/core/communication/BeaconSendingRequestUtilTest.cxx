/**
 * Copyright 2018-2021 Dynatrace LLC
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

#include "mock/MockIBeaconSendingContext.h"
#include "../../protocol/mock/MockIHTTPClient.h"
#include "../../protocol/mock/MockIStatusResponse.h"

#include "core/communication/BeaconSendingRequestUtil.h"
#include "protocol/IStatusResponse.h"
#include "protocol/StatusResponse.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace test;

using BeaconSendingRequestUtil_t = core::communication::BeaconSendingRequestUtil;
using IStatusResponse_t = protocol::IStatusResponse;
using MockNiceIBeaconSendingContext_sp = std::shared_ptr<testing::NiceMock<MockIBeaconSendingContext>>;
using MockNiceIStatusResponse_sp = std::shared_ptr<testing::NiceMock<MockIStatusResponse>>;
using MockStrictIBeaconSendingContext_sp = std::shared_ptr<testing::StrictMock<MockIBeaconSendingContext>>;
using MockStrictIHTTPClient_sp = std::shared_ptr<testing::StrictMock<MockIHTTPClient>>;
using StatusResponse_t = protocol::StatusResponse;
using StatusResponse_sp = std::shared_ptr<StatusResponse_t>;

class BeaconSendingRequestUtilTest : public testing::Test
{
protected:

	MockNiceIBeaconSendingContext_sp mockContextNice;
	MockStrictIBeaconSendingContext_sp mockContextStrict;
	MockStrictIHTTPClient_sp mockHTTPClient;
	MockNiceIStatusResponse_sp mockStatusResponse;

	virtual void SetUp() override
	{
		mockStatusResponse = MockIStatusResponse::createNice();

		mockHTTPClient = MockIHTTPClient::createStrict();
		ON_CALL(*mockHTTPClient, sendStatusRequest(testing::_))
			.WillByDefault(testing::Return(mockStatusResponse));

		mockContextNice = MockIBeaconSendingContext::createNice();
		ON_CALL(*mockContextNice, getHTTPClient())
			.WillByDefault(testing::Return(mockHTTPClient));

		mockContextStrict = MockIBeaconSendingContext::createStrict();
		ON_CALL(*mockContextStrict, getHTTPClient())
			.WillByDefault(testing::Return(mockHTTPClient));
	}
};

TEST_F(BeaconSendingRequestUtilTest, sendStatusRequestIsAbortedWhenShutdownIsRequested)
{
	// with
	ON_CALL(*mockStatusResponse, isErroneousResponse())
		.WillByDefault(testing::Return(true));

	// expect
	EXPECT_CALL(*mockContextStrict, isShutdownRequested())
		.Times(testing::Exactly(2))
		.WillOnce(testing::Return(false))
		.WillOnce(testing::Return(true));
	EXPECT_CALL(*mockContextStrict, getHTTPClient())
		.Times(1);
	EXPECT_CALL(*mockContextStrict, sleep(testing::_))
		.Times(1);

	EXPECT_CALL(*mockHTTPClient, sendStatusRequest(testing::Ref(*mockContextStrict)))
		.Times(1);

	// given, when
	auto obtained = BeaconSendingRequestUtil_t::sendStatusRequest(*mockContextStrict, 5, 1000L);

	// then
	ASSERT_THAT(obtained, testing::Eq(mockStatusResponse));
}

TEST_F(BeaconSendingRequestUtilTest, sendStatusRequestIsAbortedIfTheNumberOfRetriesIsExceeded)
{
	// with
	ON_CALL(*mockStatusResponse, isErroneousResponse())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mockContextNice, isShutdownRequested())
		.WillByDefault(testing::Return(false));

	// expect
	EXPECT_CALL(*mockContextNice, getHTTPClient())
		.Times(4);
	EXPECT_CALL(*mockContextNice, sleep(testing::_))
		.Times(3);

	EXPECT_CALL(*mockHTTPClient, sendStatusRequest(testing::Ref(*mockContextNice)))
		.Times(4);

	// given, when
	auto obtained = BeaconSendingRequestUtil_t::sendStatusRequest(*mockContextNice, 3, 1000L);

	// then
	ASSERT_THAT(obtained, testing::Eq(mockStatusResponse));
}

TEST_F(BeaconSendingRequestUtilTest, sendStatusRequestIsDoneWhenHttpClientReturnsASuccessfulResponse)
{
	// expect
	EXPECT_CALL(*mockContextStrict, isShutdownRequested())
		.Times(1)
		.WillRepeatedly(testing::Return(false));
	EXPECT_CALL(*mockContextStrict, getHTTPClient())
		.Times(::testing::Exactly(1));
	EXPECT_CALL(*mockHTTPClient, sendStatusRequest(testing::Ref(*mockContextStrict)))
		.Times(::testing::Exactly(1));

	// given, when
	auto obtained =  BeaconSendingRequestUtil_t::sendStatusRequest(*mockContextStrict, 3, 1000L);

	// then
	ASSERT_THAT(obtained, testing::Eq(mockStatusResponse));
}

TEST_F(BeaconSendingRequestUtilTest, sleepTimeIsDoubledBetweenConsecutiveRetries)
{
	// with
	ON_CALL(*mockStatusResponse, isErroneousResponse())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mockContextNice, isShutdownRequested())
		.WillByDefault(testing::Return(false));

	// expect
	{
		testing::InSequence s;
		EXPECT_CALL(*mockContextNice, sleep(1000L));
		EXPECT_CALL(*mockContextNice, sleep(2000L));
		EXPECT_CALL(*mockContextNice, sleep(4000L));
		EXPECT_CALL(*mockContextNice, sleep(8000L));
		EXPECT_CALL(*mockContextNice, sleep(16000L));
	}

	EXPECT_CALL(*mockHTTPClient, sendStatusRequest(testing::Ref(*mockContextNice)))
		.Times((6));
	EXPECT_CALL(*mockContextNice, getHTTPClient())
		.Times(6);

	// given, when
	auto obtained = BeaconSendingRequestUtil_t::sendStatusRequest(*mockContextNice, 5, 1000L);

	// then
	ASSERT_THAT(obtained, testing::Eq(mockStatusResponse));
}

TEST_F(BeaconSendingRequestUtilTest, sendStatusRequestHandlesNullResponsesSameAsErroneousResponses)
{
	// with
	ON_CALL(*mockContextNice, isShutdownRequested())
		.WillByDefault(testing::Return(false));
	ON_CALL(*mockHTTPClient, sendStatusRequest(testing::_))
		.WillByDefault(testing::ReturnNull());

	// expect
	EXPECT_CALL(*mockContextNice, getHTTPClient())
		.Times(::testing::Exactly(4));
	EXPECT_CALL(*mockContextNice, sleep(testing::_))
		.Times(::testing::Exactly(3));
	EXPECT_CALL(*mockHTTPClient, sendStatusRequest(testing::Ref(*mockContextNice)))
		.Times(::testing::Exactly(4));

	// given, when
	auto obtained = BeaconSendingRequestUtil_t::sendStatusRequest(*mockContextNice, 3, 1000L);

	// then
	ASSERT_THAT(obtained, testing::IsNull());
}

TEST_F(BeaconSendingRequestUtilTest, sendStatusRequestReturnsTooManyRequestsResponseImmediately)
{
	// with
	ON_CALL(*mockStatusResponse, isTooManyRequestsResponse())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mockStatusResponse, isErroneousResponse())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mockContextNice, isShutdownRequested())
		.WillByDefault(testing::Return(false));

	// expect
	EXPECT_CALL(*mockContextNice, getHTTPClient())
		.Times(::testing::Exactly(1));
	EXPECT_CALL(*mockContextNice, sleep(testing::_))
		.Times(::testing::Exactly(0));
	EXPECT_CALL(*mockHTTPClient, sendStatusRequest(testing::Ref(*mockContextNice)))
		.Times(::testing::Exactly(1));

	// given, when
	 auto obtained = BeaconSendingRequestUtil_t::sendStatusRequest(*mockContextNice, 3, 1000L);

	 // then
	 ASSERT_THAT(obtained, testing::Eq(mockStatusResponse));
}
