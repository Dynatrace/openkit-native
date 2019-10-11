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

#ifndef _TEST_OBJECTS_MOCKIWEBREQUESTTRACER_H
#define _TEST_OBJECTS_MOCKIWEBREQUESTTRACER_H

#include "Types.h"

#include "gmock/gmock.h"

namespace test
{
	class MockIWebRequestTracer : public types::IWebRequestTracer_t
	{
	public:

		virtual ~MockIWebRequestTracer() {}

		MOCK_CONST_METHOD0(getTag, const char*());

		MOCK_METHOD1(setResponseCode,
			types::IWebRequestTracer_sp(
				int32_t
			)
		);

		MOCK_METHOD1(setBytesSent,
			types::IWebRequestTracer_sp(
				int32_t
			)
		);


		MOCK_METHOD1(setBytesReceived,
			types::IWebRequestTracer_sp(
				int32_t
			)
		);

		MOCK_METHOD0(start, types::IWebRequestTracer_sp());

		MOCK_METHOD0(stop, void());

		MOCK_METHOD1(stop,
			void(
				int32_t
			)
		);
	};
}

#endif
