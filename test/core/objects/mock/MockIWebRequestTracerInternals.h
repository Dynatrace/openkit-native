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

		MOCK_METHOD(const char*, getTag, (), (const, override));

		MOCK_METHOD(
			std::shared_ptr<openkit::IWebRequestTracer>,
			setResponseCode,
			(
				int32_t /* responseCode */
			),
			(override)
		);

		MOCK_METHOD(
			std::shared_ptr<openkit::IWebRequestTracer>,
			setBytesSent,
			(
				int32_t /* bytesSent */
			),
			(override)
		);

		MOCK_METHOD(
			std::shared_ptr<openkit::IWebRequestTracer>,
			setBytesReceived,
			(
				int32_t /* bytes/received */
			),
			(override)
		);

		MOCK_METHOD(std::shared_ptr<openkit::IWebRequestTracer>, start, (), (override));

		MOCK_METHOD(void, stop, (), (override));

		MOCK_METHOD(void, stop, (int32_t), (override));

		MOCK_METHOD(void, close, (), (override));

		MOCK_METHOD(const core::UTF8String, getURL, (), (const, override));

		MOCK_METHOD(int32_t, getResponseCode, (), (const, override));

		MOCK_METHOD(int64_t, getStartTime, (), (const, override));

		MOCK_METHOD(int64_t, getEndTime, (), (const, override));

		MOCK_METHOD(int32_t, getStartSequenceNo, (), (const, override));

		MOCK_METHOD(int32_t, getEndSequenceNo, (), (const, override));

		MOCK_METHOD(int32_t, getBytesSent, (), (const, override));

		MOCK_METHOD(int32_t, getBytesReceived, (), (const, override));

		MOCK_METHOD(bool, isStopped, (), (const, override));

		MOCK_METHOD(std::shared_ptr<core::objects::IOpenKitComposite>, getParent, (), (const, override));
	};
}
#endif
