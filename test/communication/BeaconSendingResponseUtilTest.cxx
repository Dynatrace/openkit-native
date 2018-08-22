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

#include "communication/BeaconSendingResponseUtil.h"
#include "../protocol/NullLogger.h"

#include <memory>

#include <gtest/gtest.h>

class BeaconSendingResponseUtilTest : public testing::Test
{
protected:

	virtual void SetUp() override
	{

	}

	virtual void TearDown() override
	{

	}

	///
	/// Stub class only used to delegate the ctor arguments to protected ctor of base class.
	///
	class StubResponse : public protocol::Response
	{
	public:

		StubResponse(int32_t responseCode, const ResponseHeaders& responseHeaders) :
			protocol::Response(std::make_shared<NullLogger>(), responseCode, responseHeaders)
		{}
	};
};

TEST_F(BeaconSendingResponseUtilTest, isSuccessfulResponseReturnsFalseIfResponseIsNullptr)
{
	// when
	auto obtained = communication::BeaconSendingResponseUtil::isSuccessfulResponse(nullptr);

	// then
	ASSERT_FALSE(obtained);
}

TEST_F(BeaconSendingResponseUtilTest, isSuccessfulResponseReturnsFalseIfResponseIsErroneous)
{
	// given
	auto response = std::make_shared<StubResponse>(400, protocol::Response::ResponseHeaders());

	// when
	auto obtained = communication::BeaconSendingResponseUtil::isSuccessfulResponse(response);

	// then
	ASSERT_FALSE(obtained);
}

TEST_F(BeaconSendingResponseUtilTest, isSuccessfulResponseReturnsTrueIfResponseIsNotErroneous)
{
	// given
	auto response = std::make_shared<StubResponse>(200, protocol::Response::ResponseHeaders());

	// when
	auto obtained = communication::BeaconSendingResponseUtil::isSuccessfulResponse(response);

	// then
	ASSERT_TRUE(obtained);
}

TEST_F(BeaconSendingResponseUtilTest, isTooManyRequestsResponseReturnsFalseIfResponseIsNull)
{
	// when
	auto obtained = communication::BeaconSendingResponseUtil::isTooManyRequestsResponse(nullptr);

	// then
	ASSERT_FALSE(obtained);
}

TEST_F(BeaconSendingResponseUtilTest, isTooManyRequestsResponseReturnsFalseIfResponseCodeIsNotEqualToTooManyRequestsCode)
{
	// given
	auto response = std::make_shared<StubResponse>(404, protocol::Response::ResponseHeaders());

	// when
	auto obtained = communication::BeaconSendingResponseUtil::isTooManyRequestsResponse(response);

	// then
	ASSERT_FALSE(obtained);
}

TEST_F(BeaconSendingResponseUtilTest, isTooManyRequestsResponseReturnsTrueIfResponseCodeIndicatesTooManyRequests)
{
	// given
	auto response = std::make_shared<StubResponse>(429, protocol::Response::ResponseHeaders());

	// when
	auto obtained = communication::BeaconSendingResponseUtil::isTooManyRequestsResponse(response);

	// then
	ASSERT_TRUE(obtained);
}