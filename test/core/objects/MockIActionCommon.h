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

#include "Types.h"
#include "../Types.h"
#include "../../api/Types.h"

#include "gmock/gmock.h"

namespace test
{
	class MockIActionCommon : public types::IActionCommon_t
	{
	public:

		MockIActionCommon()
		{
			ON_CALL(*this, getName()).WillByDefault(testing::ReturnRefOfCopy(types::Utf8String_t("")));
		}

		MOCK_METHOD2(enterAction,
			types::IAction_sp(
					types::IRootAction_sp,
					const char*
			)
		);

		MOCK_METHOD1(reportEvent,
			void(
				const char*
			)
		);

		void reportValue(const char* valueName, int32_t value) override
		{
			reportValueInt(valueName, value);
		}

		MOCK_METHOD2(reportValueInt,
			void(
				const char*,
				int32_t
			)
		);

		void reportValue(const char* valueName, double value) override
		{
			reportValueDouble(valueName, value);
		}

		MOCK_METHOD2(reportValueDouble,
			void(
				const char*,
				double
			)
		);

		void reportValue(const char* valueName, const char* value) override
		{
			reportValueString(valueName, value);
		}

		MOCK_METHOD2(reportValueString,
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
			types::IWebRequestTracer_sp(
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
