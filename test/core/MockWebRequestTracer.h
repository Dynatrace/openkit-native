/**
* Copyright 2018 Dynatrace LLC
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

#ifndef _TEST_CORE_MOCKWEBREQUESTTRACER_H
#define _TEST_CORE_MOCKWEBREQUESTTRACER_H

#include "OpenKit/ILogger.h"

#include "core/WebRequestTracerBase.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

namespace test {
	class MockWebRequestTracer : public core::WebRequestTracerBase
	{
	public:
		MockWebRequestTracer(std::shared_ptr<openkit::ILogger> logger, std::shared_ptr<protocol::Beacon> beacon)
			: WebRequestTracerBase(logger, beacon, 0)
		{
		}


		virtual ~MockWebRequestTracer() {}

		MOCK_CONST_METHOD0(getResponseCode, int32_t(void));
		MOCK_CONST_METHOD0(getBytesSent, int32_t(void));
		MOCK_CONST_METHOD0(getBytesReceived, int32_t(void));

		MOCK_METHOD0(start, std::shared_ptr<openkit::IWebRequestTracer>(void));
		MOCK_METHOD0(stop, void());

	};
}
#endif
