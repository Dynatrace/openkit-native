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

#ifndef _TEST_API_MOCK_MOCKIHTTPRESPONSE_H
#define _TEST_API_MOCK_MOCKIHTTPRESPONSE_H

#include "OpenKit/IHttpResponse.h"
#include "../../DefaultValues.h"

#include <memory>

#include <gmock/gmock.h>


namespace test
{
	class MockIHttpResponse : public openkit::IHttpResponse
	{
	public:

		MockIHttpResponse()
		{
			ON_CALL(*this, getRequestUri())
				.WillByDefault(testing::ReturnRef(DefaultValues::EMPTY_STRING));
			ON_CALL(*this, getRequestMethod())
				.WillByDefault(testing::ReturnRef(DefaultValues::EMPTY_STRING));
			ON_CALL(*this, getReasonPhrase())
				.WillByDefault(testing::ReturnRef(DefaultValues::EMPTY_STRING));
			ON_CALL(*this, getHeader(testing::_))
				.WillByDefault(testing::ReturnRef(DefaultValues::emptyList<std::string>()));
		}

		virtual ~MockIHttpResponse() override = default;

		static std::shared_ptr<testing::NiceMock<MockIHttpResponse>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockIHttpResponse>>();
		}

		static std::shared_ptr<testing::StrictMock<MockIHttpResponse>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockIHttpResponse>>();
		}

		MOCK_METHOD(const std::string&, getRequestUri, (), (const, override));

		MOCK_METHOD(const std::string&, getRequestMethod, (), (const, override));

		MOCK_METHOD(int32_t, getStatusCode, (), (const, override));

		MOCK_METHOD(const std::string&, getReasonPhrase, (), (const, override));

		MOCK_METHOD(bool, containsHeader, (const std::string&), (const, override));

		MOCK_METHOD(const std::list<std::string>&, getHeader, (const std::string&), (const, override));

	};
}

#endif
