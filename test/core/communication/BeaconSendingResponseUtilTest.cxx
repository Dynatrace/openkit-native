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

#include "../../protocol/mock/MockIStatusResponse.h"

#include "core/communication/BeaconSendingResponseUtil.h"

#include <gtest/gtest.h>
#include <memory>

using namespace test;

using BeaconSendingResponseUtil_t = core::communication::BeaconSendingResponseUtil;

class BeaconSendingResponseUtilTest : public testing::Test
{
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
	auto response = MockIStatusResponse::createStrict();

	// expect
	EXPECT_CALL(*response, isErroneousResponse())
		.Times(testing::Exactly(1))
		.WillOnce(testing::Return(true));

	// when
	auto obtained = BeaconSendingResponseUtil_t::isSuccessfulResponse(response);

	// then
	ASSERT_FALSE(obtained);
}

TEST_F(BeaconSendingResponseUtilTest, isSuccessfulResponseReturnsTrueIfResponseIsNotErroneous)
{
	// given
	auto response = MockIStatusResponse::createStrict();

	// expect
	EXPECT_CALL(*response, isErroneousResponse())
		.Times(testing::Exactly(1))
		.WillOnce(testing::Return(false));

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

TEST_F(BeaconSendingResponseUtilTest, isTooManyRequestsResponseReturnsFalseIfResponseHasIsToTooManyRequestsResponseFalse)
{
	// given
	auto response = MockIStatusResponse::createStrict();

	// expect
	EXPECT_CALL(*response, isTooManyRequestsResponse())
		.Times(testing::Exactly(1))
		.WillOnce(testing::Return(false));

	// when
	auto obtained = BeaconSendingResponseUtil_t::isTooManyRequestsResponse(response);

	// then
	ASSERT_FALSE(obtained);
}

TEST_F(BeaconSendingResponseUtilTest, isTooManyRequestsResponseReturnsFalseIfResponseHasIsToTooManyRequestsResponseTrue)
{
	// given
	auto response = MockIStatusResponse::createStrict();

	// expect
	EXPECT_CALL(*response, isTooManyRequestsResponse())
		.Times(testing::Exactly(1))
		.WillOnce(testing::Return(true));

	// when
	auto obtained = BeaconSendingResponseUtil_t::isTooManyRequestsResponse(response);

	// then
	ASSERT_TRUE(obtained);
}