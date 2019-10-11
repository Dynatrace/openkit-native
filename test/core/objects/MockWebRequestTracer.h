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

#ifndef _TEST_CORE_OBJECTS_MOCKWEBREQUESTTRACER_H
#define _TEST_CORE_OBJECTS_MOCKWEBREQUESTTRACER_H

#include "Types.h"
#include "../../api/Types.h"
#include "../../protocol/Types.h"

#include "core/objects/WebRequestTracer.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

namespace test {
	class MockWebRequestTracer : public types::WebRequestTracer_t
	{
	public:
		MockWebRequestTracer
		(
			types::ILogger_sp logger,
			types::IOpenKitComposite_sp parent,
			types::Beacon_sp beacon,
			const types::Utf8String_t& url
		)
		: types::WebRequestTracer_t
		(
			logger,
			parent,
			beacon,
			url
		)
		{
		}


		virtual ~MockWebRequestTracer() {}

		MOCK_CONST_METHOD0(getResponseCode, int32_t(void));
		MOCK_CONST_METHOD0(getBytesSent, int32_t(void));
		MOCK_CONST_METHOD0(getBytesReceived, int32_t(void));

		MOCK_METHOD0(start, types::IWebRequestTracer_sp(void));
		MOCK_METHOD0(stop, void());

	};
}
#endif
