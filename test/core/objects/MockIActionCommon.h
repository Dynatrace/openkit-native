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

#ifndef _TEST_OBJECTS_MOCKIACTIONCOMMON_H
#define _TEST_OBJECTS_MOCKIACTIONCOMMON_H

#include "../../DefaultValues.h"

#include "OpenKit/IAction.h"
#include "OpenKit/IRootAction.h"
#include "OpenKit/IWebRequestTracer.h"
#include "core/objects/IActionCommon.h"

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

		virtual ~MockIActionCommon() {}

		MOCK_METHOD2(enterAction,
			std::shared_ptr<openkit::IAction>(
				std::shared_ptr<openkit::IRootAction>,
				const char*
			)
		);

		MOCK_METHOD1(reportEvent,
			void(
				const char*
			)
		);

		MOCK_METHOD2(reportValue,
			void(
				const char*,
				int32_t
			)
		);

		MOCK_METHOD2(reportValue,
			void(
				const char*,
				double
			)
		);

		MOCK_METHOD2(reportValue,
			void(
				const char*,
				const char*
			)
		);

		MOCK_METHOD3(reportError,
			void(
				const char*,
				int32_t,
				const char*
			)
		);

		MOCK_METHOD1(traceWebRequest,
			std::shared_ptr<openkit::IWebRequestTracer>(
				const char*
			)
		);

		MOCK_METHOD0(leaveAction, bool());

		MOCK_CONST_METHOD0(isActionLeft, bool());

		MOCK_CONST_METHOD0(getID, int32_t());

		MOCK_CONST_METHOD0(getParentID, int32_t());

		MOCK_CONST_METHOD0(getName, const core::UTF8String&());

		MOCK_CONST_METHOD0(getStartTime, int64_t());

		MOCK_CONST_METHOD0(getEndTime, int64_t());

		MOCK_CONST_METHOD0(getStartSequenceNumber, int32_t());

		MOCK_CONST_METHOD0(getEndSequenceNumber, int32_t());

		MOCK_METHOD1(onChildClosed,
			void(
				types::IOpenKitObject_sp
			)
		);

		MOCK_CONST_METHOD0(toString, const std::string());
	};
}

#endif
