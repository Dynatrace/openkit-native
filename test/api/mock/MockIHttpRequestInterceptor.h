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

#ifndef _TEST_API_MOCK_MOCKIHTTPREQUESTINTERCEPTOR_H
#define _TEST_API_MOCK_MOCKIHTTPREQUESTINTERCEPTOR_H

#include "OpenKit/IHttpRequestInterceptor.h"

#include <gmock/gmock.h>

namespace test
{
	class MockIHttpRequestInterceptor : public openkit::IHttpRequestInterceptor
	{
	public:

		virtual ~MockIHttpRequestInterceptor() override = default;

		static std::shared_ptr<testing::NiceMock<MockIHttpRequestInterceptor>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockIHttpRequestInterceptor>>();
		}

		static std::shared_ptr<testing::StrictMock<MockIHttpRequestInterceptor>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockIHttpRequestInterceptor>>();
		}

		MOCK_METHOD(void, intercept, (openkit::IHttpRequest&), ());
	};
}

#endif
