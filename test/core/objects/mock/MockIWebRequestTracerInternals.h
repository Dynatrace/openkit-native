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

#ifndef _TEST_CORE_OBJECTS_MOCK_MOCKIWEBREQUESTTRACERINTERNALS_H
#define _TEST_CORE_OBJECTS_MOCK_MOCKIWEBREQUESTTRACERINTERNALS_H

#include "../../../DefaultValues.h"
#include "OpenKit/IWebRequestTracer.h"
#include "core/UTF8String.h"
#include "core/objects/IOpenKitComposite.h"
#include "core/objects/IWebRequestTracerInternals.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <memory>

namespace test {
	class MockIWebRequestTracerInternals
		: public core::objects::IWebRequestTracerInternals
	{
	public:

		MockIWebRequestTracerInternals()
		{
			ON_CALL(*this, getTag()).WillByDefault(testing::Return(DefaultValues::EMPTY_CHAR_STRING));
			ON_CALL(*this, getURL()).WillByDefault(testing::Return(DefaultValues::UTF8_EMPTY_STRING));

			ON_CALL(*this, getParent()).WillByDefault(testing::Return(nullptr));

			ON_CALL(*this, setResponseCode(testing::_)).WillByDefault(testing::Return(nullptr));
			ON_CALL(*this, setBytesSent(testing::_)).WillByDefault(testing::Return(nullptr));
			ON_CALL(*this, setBytesReceived(testing::_)).WillByDefault(testing::Return(nullptr));
			ON_CALL(*this, start()).WillByDefault(testing::Return(nullptr));
		}

		~MockIWebRequestTracerInternals() override = default;

		static std::shared_ptr<testing::NiceMock<MockIWebRequestTracerInternals>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockIWebRequestTracerInternals>>();
		}

		static std::shared_ptr<testing::StrictMock<MockIWebRequestTracerInternals>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockIWebRequestTracerInternals>>();
		}

		MOCK_CONST_METHOD0(getTag, const char*());

		MOCK_METHOD1(setResponseCode,
			std::shared_ptr<openkit::IWebRequestTracer>(
				int32_t /* responseCode */
			)
		);

		MOCK_METHOD1(setBytesSent,
			std::shared_ptr<openkit::IWebRequestTracer>(
				int32_t /* bytesSent */
			)
		);

		MOCK_METHOD1(setBytesReceived,
			std::shared_ptr<openkit::IWebRequestTracer>(
				int32_t /* bytes/received */
			)
		);

		MOCK_METHOD0(start, std::shared_ptr<openkit::IWebRequestTracer>());

		MOCK_METHOD0(stop, void());

		MOCK_METHOD1(stop,
			void(
				int32_t /* responseCode */
			)
		);

		MOCK_METHOD0(close, void());

		MOCK_CONST_METHOD0(getURL, const core::UTF8String());

		MOCK_CONST_METHOD0(getResponseCode, int32_t());

		MOCK_CONST_METHOD0(getStartTime, int64_t());

		MOCK_CONST_METHOD0(getEndTime, int64_t());

		MOCK_CONST_METHOD0(getStartSequenceNo, int32_t());

		MOCK_CONST_METHOD0(getEndSequenceNo, int32_t());

		MOCK_CONST_METHOD0(getBytesSent, int32_t());

		MOCK_CONST_METHOD0(getBytesReceived, int32_t());

		MOCK_CONST_METHOD0(isStopped, bool());

		MOCK_CONST_METHOD0(getParent, std::shared_ptr<core::objects::IOpenKitComposite>());
	};
}
#endif
