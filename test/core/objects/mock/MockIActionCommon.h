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

#ifndef _TEST_OBJECTS_MOCKIACTIONCOMMON_H
#define _TEST_OBJECTS_MOCKIACTIONCOMMON_H

#include "../../../DefaultValues.h"

#include "OpenKit/IAction.h"
#include "OpenKit/IRootAction.h"
#include "OpenKit/IWebRequestTracer.h"
#include "core/objects/IActionCommon.h"
#include "core/objects/IOpenKitObject.h"

#include "gmock/gmock.h"

#include <memory>

namespace test
{
	class MockIActionCommon
		: public core::objects::IActionCommon
	{
	public:

		MockIActionCommon()
		{
			ON_CALL(*this, getName())
					.WillByDefault(testing::ReturnRefOfCopy(DefaultValues::UTF8_EMPTY_STRING));
		}

		~MockIActionCommon() override = default;

		static std::shared_ptr<testing::NiceMock<MockIActionCommon>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockIActionCommon>>();
		}

		static std::shared_ptr<testing::StrictMock<MockIActionCommon>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockIActionCommon>>();
		}

		MOCK_METHOD(
			std::shared_ptr<openkit::IAction>,
			enterAction,
			(
				std::shared_ptr<openkit::IRootAction>,
				const char*
			),
			(override)
		);

		MOCK_METHOD(void, reportEvent, (const char*), (override));

		MOCK_METHOD(
			void,
			reportValue,
			(
				const char*,
				int32_t
			),
			(override)
		);

		MOCK_METHOD(
			void,
			reportValue,
			(
				const char*,
				int64_t
			),
			(override)
		);

		MOCK_METHOD(
			void,
			reportValue,
			(
				const char*,
				double
			),
			(override)
		);

		MOCK_METHOD(
			void,
			reportValue,
			(
				const char*,
				const char*
			),
			(override)
		);

		MOCK_METHOD(
			void,
			reportError,
			(
				const char*,
				int32_t
			),
			(override)
		);

		MOCK_METHOD(
			void,
			reportError,
			(
				const char*,
				const char*,
				const char*,
				const char*
			),
			(override)
		);

		MOCK_METHOD(
			std::shared_ptr<openkit::IWebRequestTracer>,
			traceWebRequest,
			(
				const char*
			),
			(override)
		);

		MOCK_METHOD(bool, leaveAction, (), (override));

		MOCK_METHOD(bool, isActionLeft, (), (const, override));

		MOCK_METHOD(bool, cancelAction, (), (override));

		MOCK_METHOD(std::chrono::milliseconds, getDuration, (), (override));

		MOCK_METHOD(int32_t, getID, (), (const, override));

		MOCK_METHOD(int32_t, getParentID, (), (const, override));

		MOCK_METHOD(const core::UTF8String&, getName, (), (const, override));

		MOCK_METHOD(int64_t, getStartTime, (), (const, override));

		MOCK_METHOD(int64_t, getEndTime, (), (const, override));

		MOCK_METHOD(int32_t, getStartSequenceNumber, (), (const, override));

		MOCK_METHOD(int32_t, getEndSequenceNumber, (), (const, override));

		MOCK_METHOD(
			void,
			onChildClosed,
			(
				std::shared_ptr<core::objects::IOpenKitObject>
			),
			(override)
		);

		MOCK_METHOD(const std::string, toString, (), (const, override));
	};
}

#endif
