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

#include "MockBeaconSendingContext.h"
#include "../../api/mock/MockILogger.h"
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
using MockNiceBeaconSendingContext_t = testing::NiceMock<MockBeaconSendingContext>;
using MockNiceBeaconSendingContext_sp = std::shared_ptr<MockNiceBeaconSendingContext_t>;
using MockNiceILogger_sp = std::shared_ptr<testing::NiceMock<MockILogger>>;
using MockStrictIHTTPClient_sp = std::shared_ptr<testing::StrictMock<MockIHTTPClient>>;
using StatusResponse_t = protocol::StatusResponse;
using StatusResponse_sp = std::shared_ptr<StatusResponse_t>;

class BeaconSendingRequestUtilTest : public testing::Test
{
protected:

	MockNiceBeaconSendingContext_sp mockContext;
	MockNiceILogger_sp mockLogger;
	MockStrictIHTTPClient_sp mockHTTPClient;

	virtual void SetUp() override
	{
		mockLogger = MockILogger::createNice();

		mockHTTPClient = MockIHTTPClient::createStrict();
		ON_CALL(*mockHTTPClient, sendStatusRequest())
			.WillByDefault(testing::Return(MockIStatusResponse::createNice()));

		mockContext = std::make_shared<MockNiceBeaconSendingContext_t>(mockLogger);
		ON_CALL(*mockContext, getHTTPClient())
			.WillByDefault(testing::Return(mockHTTPClient));

	}
};

TEST_F(BeaconSendingRequestUtilTest, sendStatusRequestIsAbortedWhenShutdownIsRequested)
{
	// given
	ON_CALL(*mockHTTPClient, sendStatusRequest())
		.WillByDefault(testing::Invoke([&]() ->  StatusResponse_sp
		{
			return std::make_shared<StatusResponse_t>(mockLogger, "", 400, IStatusResponse_t::ResponseHeaders());
		})
	);

	// verify
	EXPECT_CALL(*mockContext, isShutdownRequested())
		.WillOnce(::testing::Return(false))
		.WillRepeatedly(::testing::Return(true));
	EXPECT_CALL(*mockContext, getHTTPClient())
		.Times(::testing::Exactly(1));
	EXPECT_CALL(*mockHTTPClient, sendStatusRequest())
		.Times(::testing::Exactly(1));

	// when
	auto obtained = BeaconSendingRequestUtil_t::sendStatusRequest(*mockContext, 5, 1000L);

	// then
	ASSERT_NE(nullptr, obtained);
}

TEST_F(BeaconSendingRequestUtilTest, sendStatusRequestIsAbortedIfTheNumberOfRetriesIsExceeded)
{
	// given
	ON_CALL(*mockHTTPClient, sendStatusRequest())
		.WillByDefault(testing::Invoke([&]() ->  StatusResponse_sp
		{
			return std::make_shared<StatusResponse_t>(mockLogger, "", 400, IStatusResponse_t::ResponseHeaders());
		})
	);
	ON_CALL(*mockContext, isShutdownRequested())
		.WillByDefault(testing::Return(false));

	// verify
	EXPECT_CALL(*mockContext, getHTTPClient())
		.Times(::testing::Exactly(4));
	EXPECT_CALL(*mockContext, sleep(testing::_))
		.Times(::testing::Exactly(3));
	EXPECT_CALL(*mockHTTPClient, sendStatusRequest())
		.Times(::testing::Exactly(4));

	// when
	auto obtained = BeaconSendingRequestUtil_t::sendStatusRequest(*mockContext, 3, 1000L);

	// then
	ASSERT_NE(nullptr, obtained);
}

TEST_F(BeaconSendingRequestUtilTest, sendStatusRequestIsDoneWhenHttpClientReturnsASuccessfulResponse)
{
	// given
	ON_CALL(*mockHTTPClient, sendStatusRequest())
		.WillByDefault(testing::Invoke([&]() ->  StatusResponse_sp
		{
			return std::make_shared<StatusResponse_t>(mockLogger, "", 200, IStatusResponse_t::ResponseHeaders());
		})
	);
	ON_CALL(*mockContext, isShutdownRequested())
		.WillByDefault(testing::Return(false));

	// verify
	EXPECT_CALL(*mockContext, getHTTPClient())
		.Times(::testing::Exactly(1));
	EXPECT_CALL(*mockContext, sleep(testing::AnyOf(1000L, 2000L, 4000L)))
		.Times(::testing::Exactly(0));
	EXPECT_CALL(*mockHTTPClient, sendStatusRequest())
		.Times(::testing::Exactly(1));

	// when
	auto obtained = BeaconSendingRequestUtil_t::sendStatusRequest(*mockContext, 3, 1000L);

	// then
	ASSERT_NE(nullptr, obtained);
}

TEST_F(BeaconSendingRequestUtilTest, sleepTimeIsDoubledBetweenConsecutiveRetries)
{
	// given
	ON_CALL(*mockHTTPClient, sendStatusRequest())
		.WillByDefault(testing::Invoke([&]() ->  StatusResponse_sp
		{
			return std::make_shared<StatusResponse_t>(mockLogger, "", 400, IStatusResponse_t::ResponseHeaders());
		})
	);
	ON_CALL(*mockContext, isShutdownRequested())
		.WillByDefault(testing::Return(false));

	// verify
	{
		testing::InSequence dummy;
		EXPECT_CALL(*mockContext, sleep(1000L));
		EXPECT_CALL(*mockContext, sleep(2000L));
		EXPECT_CALL(*mockContext, sleep(4000L));
		EXPECT_CALL(*mockContext, sleep(8000L));
		EXPECT_CALL(*mockContext, sleep(16000L));
	}

	EXPECT_CALL(*mockHTTPClient, sendStatusRequest())
		.Times(::testing::Exactly(6));

	// when
	auto obtained = BeaconSendingRequestUtil_t::sendStatusRequest(*mockContext, 5, 1000L);

	// then
	ASSERT_NE(nullptr, obtained);
}

TEST_F(BeaconSendingRequestUtilTest, sendStatusRequestHandlesNullResponsesSameAsErroneousResponses)
{
	// given
	ON_CALL(*mockHTTPClient, sendStatusRequest())
		.WillByDefault(testing::Invoke([&]() ->  StatusResponse_sp { return nullptr; }));
	ON_CALL(*mockContext, isShutdownRequested())
		.WillByDefault(testing::Return(false));

	// verify
	EXPECT_CALL(*mockContext, getHTTPClient())
		.Times(::testing::Exactly(4));
	EXPECT_CALL(*mockContext, sleep(testing::_))
		.Times(::testing::Exactly(3));
	EXPECT_CALL(*mockHTTPClient, sendStatusRequest())
		.Times(::testing::Exactly(4));

	// when
	auto obtained = BeaconSendingRequestUtil_t::sendStatusRequest(*mockContext, 3, 1000L);

	// then
	ASSERT_EQ(nullptr, obtained);
}

TEST_F(BeaconSendingRequestUtilTest, sendStatusRequestReturnsTooManyRequestsResponseImmediately)
{
	// given
	ON_CALL(*mockHTTPClient, sendStatusRequest())
		.WillByDefault(testing::Invoke([&]() ->  StatusResponse_sp
		{
			return std::make_shared<StatusResponse_t>(mockLogger, "", 429, IStatusResponse_t::ResponseHeaders());
		})
	);
	ON_CALL(*mockContext, isShutdownRequested())
		.WillByDefault(testing::Return(false));

	// verify
	EXPECT_CALL(*mockContext, getHTTPClient())
		.Times(::testing::Exactly(1));
	EXPECT_CALL(*mockContext, sleep(testing::_))
		.Times(::testing::Exactly(0));
	EXPECT_CALL(*mockHTTPClient, sendStatusRequest())
		.Times(::testing::Exactly(1));

	// when
	auto obtained = BeaconSendingRequestUtil_t::sendStatusRequest(*mockContext, 5, 1000L);

	// then
	ASSERT_NE(nullptr, obtained);
}
