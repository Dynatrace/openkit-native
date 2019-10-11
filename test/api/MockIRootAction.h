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

#ifndef _TEST_OBJECTS_MOCKIROOTACTION_H
#define _TEST_OBJECTS_MOCKIROOTACTION_H

#include "Types.h"

#include "gmock/gmock.h"

namespace test
{
	class MockIRootAction : public types::IRootAction_t
	{
	public:

		MOCK_METHOD1(enterAction,
			types::IAction_sp(
				const char*
			)
		);

		MOCK_METHOD1(reportEvent,
			types::IRootAction_sp(
				const char*
			)
		);

		types::IRootAction_sp reportValue(const char* valueName, int32_t value)
		{
			return reportValueInt(valueName, value);
		}

		MOCK_METHOD2(reportValueInt,
			types::IRootAction_sp(
				const char*,
				int32_t
			)
		);

		types::IRootAction_sp reportValue(const char* valueName, double value)
		{
			return reportValueString(valueName, value);
		}

		MOCK_METHOD2(reportValueString,
			types::IRootAction_sp(
				const char*,
				double
			)
		);

		types::IRootAction_sp reportValue(const char* valueName, const char* value)
		{
			return reportValueString(valueName, value);
		}

		MOCK_METHOD2(reportValueString,
			types::IRootAction_sp(
				const char*,
				const char*
			)
		);

		MOCK_METHOD3(reportError,
			types::IRootAction_sp(
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

		MOCK_METHOD0(leaveAction, void());
	};
}

#endif
