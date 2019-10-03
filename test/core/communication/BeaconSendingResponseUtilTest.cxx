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

#include "Types.h"
#include "../../protocol/NullLogger.h"
#include "../../protocol/Types.h"

#include <gtest/gtest.h>
#include <memory>

using namespace test::types;

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
	class StubResponse : public Response_t
	{
	public:

		StubResponse(int32_t responseCode, const ResponseHeaders& responseHeaders) :
			Response_t(std::make_shared<NullLogger>(), responseCode, responseHeaders)
		{}
	};
};

TEST_F(BeaconSendingResponseUtilTest, isSuccessfulResponseReturnsFalseIfResponseIsNullptr)
{
	// when
	auto obtained = BeaconSendingResponseUtil_t::isSuccessfulResponse(nullptr);

	// then
	ASSERT_FALSE(obtained);
}

TEST_F(BeaconSendingResponseUtilTest, isSuccessfulResponseReturnsFalseIfResponseIsErroneous)
{
	// given
	auto response = std::make_shared<StubResponse>(400, Response_t::ResponseHeaders());

	// when
	auto obtained = BeaconSendingResponseUtil_t::isSuccessfulResponse(response);

	// then
	ASSERT_FALSE(obtained);
}

TEST_F(BeaconSendingResponseUtilTest, isSuccessfulResponseReturnsTrueIfResponseIsNotErroneous)
{
	// given
	auto response = std::make_shared<StubResponse>(200, Response_t::ResponseHeaders());

	// when
	auto obtained = BeaconSendingResponseUtil_t::isSuccessfulResponse(response);

	// then
	ASSERT_TRUE(obtained);
}

TEST_F(BeaconSendingResponseUtilTest, isTooManyRequestsResponseReturnsFalseIfResponseIsNull)
{
	// when
	auto obtained = BeaconSendingResponseUtil_t::isTooManyRequestsResponse(nullptr);

	// then
	ASSERT_FALSE(obtained);
}

TEST_F(BeaconSendingResponseUtilTest, isTooManyRequestsResponseReturnsFalseIfResponseCodeIsNotEqualToTooManyRequestsCode)
{
	// given
	auto response = std::make_shared<StubResponse>(404, Response_t::ResponseHeaders());

	// when
	auto obtained = BeaconSendingResponseUtil_t::isTooManyRequestsResponse(response);

	// then
	ASSERT_FALSE(obtained);
}

TEST_F(BeaconSendingResponseUtilTest, isTooManyRequestsResponseReturnsTrueIfResponseCodeIndicatesTooManyRequests)
{
	// given
	auto response = std::make_shared<StubResponse>(429, Response_t::ResponseHeaders());

	// when
	auto obtained = BeaconSendingResponseUtil_t::isTooManyRequestsResponse(response);

	// then
	ASSERT_TRUE(obtained);
}