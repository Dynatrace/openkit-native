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

#ifndef _TEST_PROTOCOL_MOCK_MOCKISTATUSRESPONSE_H
#define _TEST_PROTOCOL_MOCK_MOCKISTATUSRESPONSE_H

#include "../../DefaultValues.h"
#include "protocol/IStatusResponse.h"
#include "protocol/IResponseAttributes.h"
#include "protocol/ResponseAttributes.h"

#include "gmock/gmock.h"

#include <memory>

namespace test
{
	class MockIStatusResponse
		: public protocol::IStatusResponse
	{
	public:

		MockIStatusResponse()
		{
			ON_CALL(*this, getResponseHeaders())
				.WillByDefault(testing::ReturnRefOfCopy(protocol::IStatusResponse::ResponseHeaders()));
			ON_CALL(*this, getResponseCode())
				.WillByDefault(testing::Return(200));
			ON_CALL(*this, isErroneousResponse())
				.WillByDefault(testing::Return(false));
			ON_CALL(*this, getResponseAttributes())
				.WillByDefault(testing::Return(protocol::ResponseAttributes::withUndefinedDefaults().build()));
		}

		///
		/// Destructor
		///
		~MockIStatusResponse() override = default;

		static std::shared_ptr<testing::NiceMock<MockIStatusResponse>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockIStatusResponse>>();
		}

		static std::shared_ptr<testing::StrictMock<MockIStatusResponse>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockIStatusResponse>>();
		}

		MOCK_CONST_METHOD0(isErroneousResponse, bool());

		MOCK_CONST_METHOD0(isTooManyRequestsResponse, bool());

		MOCK_CONST_METHOD0(getResponseCode, int32_t());

		MOCK_CONST_METHOD0(getResponseHeaders, protocol::IStatusResponse::ResponseHeaders&());

		MOCK_CONST_METHOD0(getRetryAfterInMilliseconds, int64_t());

		MOCK_CONST_METHOD0(getResponseAttributes, std::shared_ptr<protocol::IResponseAttributes>());
	};
}

#endif
