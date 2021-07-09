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

#ifndef _TEST_API_MOCK_MOCKIHTTPRESPONSEINTERCEPTOR_H
#define _TEST_API_MOCK_MOCKIHTTPRESPONSEINTERCEPTOR_H

#include "OpenKit/IHttpResponseInterceptor.h"

#include <gmock/gmock.h>

namespace test
{
	class MockIHttpResponseInterceptor : public openkit::IHttpResponseInterceptor
	{
	public:

		virtual ~MockIHttpResponseInterceptor() override = default;

		static std::shared_ptr<testing::NiceMock<MockIHttpResponseInterceptor>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockIHttpResponseInterceptor>>();
		}

		static std::shared_ptr<testing::StrictMock<MockIHttpResponseInterceptor>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockIHttpResponseInterceptor>>();
		}

		MOCK_METHOD(void, intercept, (const openkit::IHttpResponse&), ());
	};
}

#endif
