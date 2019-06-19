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
#include "communication/BeaconSendingRequestUtil.h"
#include "../protocol/NullLogger.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

class BeaconSendingRequestUtilTest : public testing::Test
{
protected:

	virtual void SetUp() override
	{
		mLogger = std::make_shared<NullLogger>();
		mMockContext = std::make_shared<testing::NiceMock<test::MockBeaconSendingContext>>(mLogger);
		mMockHTTPClient = std::make_shared<testing::StrictMock<test::MockHTTPClient>>(std::make_shared<configuration::HTTPClientConfiguration>(core::UTF8String(""), 0, core::UTF8String("")));

		ON_CALL(*mMockContext, getHTTPClient())
			.WillByDefault(testing::Return(mMockHTTPClient));

		ON_CALL(*mMockHTTPClient, sendStatusRequestRawPtrProxy())
			.WillByDefault(testing::Return(new protocol::StatusResponse(mLogger, "", 200, protocol::Response::ResponseHeaders())));
	}

	virtual void TearDown() override
	{
		mMockHTTPClient = nullptr;
		mMockContext = nullptr;
		mLogger = nullptr;
	}

	std::shared_ptr<openkit::ILogger> mLogger;
	std::shared_ptr<testing::NiceMock<test::MockBeaconSendingContext>> mMockContext;
	std::shared_ptr<testing::StrictMock<test::MockHTTPClient>> mMockHTTPClient;
};

TEST_F(BeaconSendingRequestUtilTest, sendStatusRequestIsAbortedWhenShutdownIsRequested)
{
	// given
	ON_CALL(*mMockHTTPClient, sendStatusRequestRawPtrProxy())
		.WillByDefault(testing::Invoke([&]() ->  protocol::StatusResponse* { return new protocol::StatusResponse(mLogger, "", 400, protocol::Response::ResponseHeaders()); }));

	// verify
	EXPECT_CALL(*mMockContext, isShutdownRequested())
		.WillOnce(::testing::Return(false))
		.WillRepeatedly(::testing::Return(true));
	EXPECT_CALL(*mMockContext, getHTTPClient())
		.Times(::testing::Exactly(1));
	EXPECT_CALL(*mMockHTTPClient, sendStatusRequestRawPtrProxy())
		.Times(::testing::Exactly(1));

	// when
	auto obtained = BeaconSendingRequestUtil::sendStatusRequest(*mMockContext, 5, 1000L);

	// then
	ASSERT_NE(nullptr, obtained);
}

TEST_F(BeaconSendingRequestUtilTest, sendStatusRequestIsAbortedIfTheNumberOfRetriesIsExceeded)
{
	// given
	ON_CALL(*mMockHTTPClient, sendStatusRequestRawPtrProxy())
		.WillByDefault(testing::Invoke([&]() ->  protocol::StatusResponse* { return new protocol::StatusResponse(mLogger, "", 400, protocol::Response::ResponseHeaders()); }));
	ON_CALL(*mMockContext, isShutdownRequested())
		.WillByDefault(testing::Return(false));

	// verify
	EXPECT_CALL(*mMockContext, getHTTPClient())
		.Times(::testing::Exactly(4));
	EXPECT_CALL(*mMockContext, sleep(testing::_))
		.Times(::testing::Exactly(3));
	EXPECT_CALL(*mMockHTTPClient, sendStatusRequestRawPtrProxy())
		.Times(::testing::Exactly(4));

	// when
	auto obtained = BeaconSendingRequestUtil::sendStatusRequest(*mMockContext, 3, 1000L);

	// then
	ASSERT_NE(nullptr, obtained);
}

TEST_F(BeaconSendingRequestUtilTest, sendStatusRequestIsDoneWhenHttpClientReturnsASuccessfulResponse)
{
	// given
	ON_CALL(*mMockHTTPClient, sendStatusRequestRawPtrProxy())
		.WillByDefault(testing::Invoke([&]() ->  protocol::StatusResponse* { return new protocol::StatusResponse(mLogger, "", 200, protocol::Response::ResponseHeaders()); }));
	ON_CALL(*mMockContext, isShutdownRequested())
		.WillByDefault(testing::Return(false));

	// verify
	EXPECT_CALL(*mMockContext, getHTTPClient())
		.Times(::testing::Exactly(1));
	EXPECT_CALL(*mMockContext, sleep(testing::AnyOf(1000L, 2000L, 4000L)))
		.Times(::testing::Exactly(0));
	EXPECT_CALL(*mMockHTTPClient, sendStatusRequestRawPtrProxy())
		.Times(::testing::Exactly(1));

	// when
	auto obtained = BeaconSendingRequestUtil::sendStatusRequest(*mMockContext, 3, 1000L);

	// then
	ASSERT_NE(nullptr, obtained);
}

TEST_F(BeaconSendingRequestUtilTest, sleepTimeIsDoubledBetweenConsecutiveRetries)
{
	// given
	ON_CALL(*mMockHTTPClient, sendStatusRequestRawPtrProxy())
		.WillByDefault(testing::Invoke([&]() ->  protocol::StatusResponse* { return new protocol::StatusResponse(mLogger, "", 400, protocol::Response::ResponseHeaders()); }));
	ON_CALL(*mMockContext, isShutdownRequested())
		.WillByDefault(testing::Return(false));

	// verify
	{
		testing::InSequence dummy;
		EXPECT_CALL(*mMockContext, sleep(1000L));
		EXPECT_CALL(*mMockContext, sleep(2000L));
		EXPECT_CALL(*mMockContext, sleep(4000L));
		EXPECT_CALL(*mMockContext, sleep(8000L));
		EXPECT_CALL(*mMockContext, sleep(16000L));
	}

	EXPECT_CALL(*mMockHTTPClient, sendStatusRequestRawPtrProxy())
		.Times(::testing::Exactly(6));

	// when
	auto obtained = BeaconSendingRequestUtil::sendStatusRequest(*mMockContext, 5, 1000L);

	// then
	ASSERT_NE(nullptr, obtained);
}

TEST_F(BeaconSendingRequestUtilTest, sendStatusRequestHandlesNullResponsesSameAsErroneousResponses)
{
	// given
	ON_CALL(*mMockHTTPClient, sendStatusRequestRawPtrProxy())
		.WillByDefault(testing::Invoke([&]() ->  protocol::StatusResponse* { return nullptr; }));
	ON_CALL(*mMockContext, isShutdownRequested())
		.WillByDefault(testing::Return(false));

	// verify
	EXPECT_CALL(*mMockContext, getHTTPClient())
		.Times(::testing::Exactly(4));
	EXPECT_CALL(*mMockContext, sleep(testing::_))
		.Times(::testing::Exactly(3));
	EXPECT_CALL(*mMockHTTPClient, sendStatusRequestRawPtrProxy())
		.Times(::testing::Exactly(4));

	// when
	auto obtained = BeaconSendingRequestUtil::sendStatusRequest(*mMockContext, 3, 1000L);

	// then
	ASSERT_EQ(nullptr, obtained);
}

TEST_F(BeaconSendingRequestUtilTest, sendStatusRequestReturnsTooManyRequestsResponseImmediately)
{
	// given
	ON_CALL(*mMockHTTPClient, sendStatusRequestRawPtrProxy())
		.WillByDefault(testing::Invoke([&]() ->  protocol::StatusResponse* { return new protocol::StatusResponse(mLogger, "", 429, protocol::Response::ResponseHeaders()); }));
	ON_CALL(*mMockContext, isShutdownRequested())
		.WillByDefault(testing::Return(false));

	// verify
	EXPECT_CALL(*mMockContext, getHTTPClient())
		.Times(::testing::Exactly(1));
	EXPECT_CALL(*mMockContext, sleep(testing::_))
		.Times(::testing::Exactly(0));
	EXPECT_CALL(*mMockHTTPClient, sendStatusRequestRawPtrProxy())
		.Times(::testing::Exactly(1));

	// when
	auto obtained = BeaconSendingRequestUtil::sendStatusRequest(*mMockContext, 5, 1000L);

	// then
	ASSERT_NE(nullptr, obtained);
}
