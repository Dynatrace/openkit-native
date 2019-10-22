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
#include "../../api/mock/MockILogger.h"
#include "../../protocol/Types.h"

#include <gtest/gtest.h>
#include <memory>

using namespace test;
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

	IStatusResponse_sp createStatusResponse(int32_t responseCode)
	{
		return std::make_shared<StatusResponse_t>(
			MockILogger::createNice(),
			"",
			responseCode,
			IStatusResponse_t::ResponseHeaders()
		);
	}
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
	auto response = createStatusResponse(400);

	// when
	auto obtained = BeaconSendingResponseUtil_t::isSuccessfulResponse(response);

	// then
	ASSERT_FALSE(obtained);
}

TEST_F(BeaconSendingResponseUtilTest, isSuccessfulResponseReturnsTrueIfResponseIsNotErroneous)
{
	// given
	auto response = createStatusResponse(200);

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
	auto response = createStatusResponse(404);

	// when
	auto obtained = BeaconSendingResponseUtil_t::isTooManyRequestsResponse(response);

	// then
	ASSERT_FALSE(obtained);
}

TEST_F(BeaconSendingResponseUtilTest, isTooManyRequestsResponseReturnsTrueIfResponseCodeIndicatesTooManyRequests)
{
	// given
	auto response = createStatusResponse(429);

	// when
	auto obtained = BeaconSendingResponseUtil_t::isTooManyRequestsResponse(response);

	// then
	ASSERT_TRUE(obtained);
}