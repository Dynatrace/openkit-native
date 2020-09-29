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

#ifndef _TEST_API_MOCK_MOCKIWEBREQUESTTRACER_H
#define _TEST_API_MOCK_MOCKIWEBREQUESTTRACER_H

#include "OpenKit/IWebRequestTracer.h"

#include "gmock/gmock.h"

#include <memory>

namespace test
{
	class MockIWebRequestTracer
		: public openkit::IWebRequestTracer
	{
	public:

		~MockIWebRequestTracer() override = default;

		static std::shared_ptr<testing::NiceMock<MockIWebRequestTracer>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockIWebRequestTracer>>();
		}

		static std::shared_ptr<testing::StrictMock<MockIWebRequestTracer>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockIWebRequestTracer>>();
		}

		MOCK_CONST_METHOD0(getTag, const char*());

		MOCK_METHOD1(setResponseCode,
			std::shared_ptr<openkit::IWebRequestTracer>(
				int32_t
			)
		);

		MOCK_METHOD1(setBytesSent,
			std::shared_ptr<openkit::IWebRequestTracer>(
				int32_t
			)
		);


		MOCK_METHOD1(setBytesReceived,
			std::shared_ptr<openkit::IWebRequestTracer>(
				int32_t
			)
		);

		MOCK_METHOD0(start, std::shared_ptr<openkit::IWebRequestTracer>());

		MOCK_METHOD0(stop, void());

		MOCK_METHOD1(stop,
			void(
				int32_t
			)
		);
	};
}

#endif
