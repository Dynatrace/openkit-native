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

#include "MockTypes.h"
#include "../../api/Types.h"
#include "../../protocol/NullLogger.h"
#include "../../protocol/Types.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace test::types;

class BeaconSendingRequestUtilTest : public testing::Test
{
protected:

	virtual void SetUp() override
	{
		mLogger = std::make_shared<NullLogger>();
		mMockContext = std::make_shared<MockNiceBeaconSendingContext_t>(mLogger);
		mMockHTTPClient = std::make_shared<MockStrictHttpClient_t>(
				std::make_shared<HttpClientConfiguration_t>(Utf8String_t(""), 0, Utf8String_t(""))
		);

		ON_CALL(*mMockContext, getHTTPClient())
			.WillByDefault(testing::Return(mMockHTTPClient));

		ON_CALL(*mMockHTTPClient, sendStatusRequestRawPtrProxy())
			.WillByDefault(testing::Return(std::make_shared<StatusResponse_t>(mLogger, "", 200, Response_t::ResponseHeaders())));
	}

	virtual void TearDown() override
	{
		mMockHTTPClient = nullptr;
		mMockContext = nullptr;
		mLogger = nullptr;
	}

	ILogger_sp mLogger;
	MockNiceBeaconSendingContext_sp mMockContext;
	MockStrictHttpClient_sp mMockHTTPClient;
};

TEST_F(BeaconSendingRequestUtilTest, sendStatusRequestIsAbortedWhenShutdownIsRequested)
{
	// given
	ON_CALL(*mMockHTTPClient, sendStatusRequestRawPtrProxy())
		.WillByDefault(testing::Invoke([&]() ->  StatusResponse_sp
		{
			return std::make_shared<StatusResponse_t>(mLogger, "", 400, Response_t::ResponseHeaders());
		})
	);

	// verify
	EXPECT_CALL(*mMockContext, isShutdownRequested())
		.WillOnce(::testing::Return(false))
		.WillRepeatedly(::testing::Return(true));
	EXPECT_CALL(*mMockContext, getHTTPClient())
		.Times(::testing::Exactly(1));
	EXPECT_CALL(*mMockHTTPClient, sendStatusRequestRawPtrProxy())
		.Times(::testing::Exactly(1));

	// when
	auto obtained = BeaconSendingRequestUtil_t::sendStatusRequest(*mMockContext, 5, 1000L);

	// then
	ASSERT_NE(nullptr, obtained);
}

TEST_F(BeaconSendingRequestUtilTest, sendStatusRequestIsAbortedIfTheNumberOfRetriesIsExceeded)
{
	// given
	ON_CALL(*mMockHTTPClient, sendStatusRequestRawPtrProxy())
		.WillByDefault(testing::Invoke([&]() ->  StatusResponse_sp
		{
			return std::make_shared<StatusResponse_t>(mLogger, "", 400, Response_t::ResponseHeaders());
		})
	);
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
	auto obtained = BeaconSendingRequestUtil_t::sendStatusRequest(*mMockContext, 3, 1000L);

	// then
	ASSERT_NE(nullptr, obtained);
}

TEST_F(BeaconSendingRequestUtilTest, sendStatusRequestIsDoneWhenHttpClientReturnsASuccessfulResponse)
{
	// given
	ON_CALL(*mMockHTTPClient, sendStatusRequestRawPtrProxy())
		.WillByDefault(testing::Invoke([&]() ->  StatusResponse_sp
		{
			return std::make_shared<StatusResponse_t>(mLogger, "", 200, Response_t::ResponseHeaders());
		})
	);
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
	auto obtained = BeaconSendingRequestUtil_t::sendStatusRequest(*mMockContext, 3, 1000L);

	// then
	ASSERT_NE(nullptr, obtained);
}

TEST_F(BeaconSendingRequestUtilTest, sleepTimeIsDoubledBetweenConsecutiveRetries)
{
	// given
	ON_CALL(*mMockHTTPClient, sendStatusRequestRawPtrProxy())
		.WillByDefault(testing::Invoke([&]() ->  StatusResponse_sp
		{
			return std::make_shared<StatusResponse_t>(mLogger, "", 400, Response_t::ResponseHeaders());
		})
	);
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
	auto obtained = BeaconSendingRequestUtil_t::sendStatusRequest(*mMockContext, 5, 1000L);

	// then
	ASSERT_NE(nullptr, obtained);
}

TEST_F(BeaconSendingRequestUtilTest, sendStatusRequestHandlesNullResponsesSameAsErroneousResponses)
{
	// given
	ON_CALL(*mMockHTTPClient, sendStatusRequestRawPtrProxy())
		.WillByDefault(testing::Invoke([&]() ->  StatusResponse_sp { return nullptr; }));
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
	auto obtained = BeaconSendingRequestUtil_t::sendStatusRequest(*mMockContext, 3, 1000L);

	// then
	ASSERT_EQ(nullptr, obtained);
}

TEST_F(BeaconSendingRequestUtilTest, sendStatusRequestReturnsTooManyRequestsResponseImmediately)
{
	// given
	ON_CALL(*mMockHTTPClient, sendStatusRequestRawPtrProxy())
		.WillByDefault(testing::Invoke([&]() ->  StatusResponse_sp
		{
			return std::make_shared<StatusResponse_t>(mLogger, "", 429, Response_t::ResponseHeaders());
		})
	);
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
	auto obtained = BeaconSendingRequestUtil_t::sendStatusRequest(*mMockContext, 5, 1000L);

	// then
	ASSERT_NE(nullptr, obtained);
}
