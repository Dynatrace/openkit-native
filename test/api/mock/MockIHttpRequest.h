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

#ifndef _TEST_API_MOCK_MOCKIHTTPREQUEST_H
#define _TEST_API_MOCK_MOCKIHTTPREQUEST_H

#include "OpenKit/IHttpRequest.h"
#include "../../DefaultValues.h"

#include <memory>

#include <gmock/gmock.h>

namespace test
{
	class MockIHttpRequest : public openkit::IHttpRequest
	{
	public:

		MockIHttpRequest()
		{
			ON_CALL(*this, getUri())
				.WillByDefault(testing::ReturnRef(DefaultValues::EMPTY_STRING));
			ON_CALL(*this, getMethod())
				.WillByDefault(testing::ReturnRef(DefaultValues::EMPTY_STRING));
			ON_CALL(*this, getHeader(testing::_))
				.WillByDefault(testing::ReturnRef(DefaultValues::EMPTY_STRING));
		}

		virtual ~MockIHttpRequest() override = default;

		static std::shared_ptr<testing::NiceMock<MockIHttpRequest>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockIHttpRequest>>();
		}

		static std::shared_ptr<testing::StrictMock<MockIHttpRequest>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockIHttpRequest>>();
		}

		MOCK_METHOD(const std::string&,  getUri, (), (const, override));

		MOCK_METHOD(const std::string&, getMethod, (), (const, override));

		MOCK_METHOD(bool, containsHeader, (const std::string&), (const, override));

		MOCK_METHOD(const std::string&, getHeader, (const std::string&), (const, override));

		MOCK_METHOD(void, setHeader, (const std::string&, const std::string&), (override));
	};
}

#endif
