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

#ifndef _TEST_CORE_OBJECTS_MOCKTYPES_H
#define _TEST_CORE_OBJECTS_MOCKTYPES_H

#include "MockLeafAction.h"
#include "MockIActionCommon.h"
#include "MockIOpenKitObject.h"
#include "../../api/MockIWebRequestTracer.h"
#include "MockIOpenKitComposite.h"
#include "MockRootAction.h"
#include "MockSession.h"
#include "MockWebRequestTracer.h"

#include "gmock/gmock.h"

#include <memory>

namespace test
{
	namespace types
	{
		using MockLeafAction_t = MockLeafAction;
		using MockNiceLeafAction_t = testing::NiceMock<MockLeafAction_t>;
		using MockNiceLeafAction_sp = std::shared_ptr<MockNiceLeafAction_t>;

		using MockIActionCommon_t = MockIActionCommon;
		using MockNiceIActionCommon_t = testing::NiceMock<MockIActionCommon_t>;
		using MockNiceIActionCommon_sp = std::shared_ptr<MockNiceIActionCommon_t>;
		using MockStrictIActionCommon_t = testing::StrictMock<MockIActionCommon_t>;
		using MockStrictIActionCommon_sp = std::shared_ptr<MockStrictIActionCommon_t>;

		using MockIOpenKitComposite_t = MockIOpenKitComposite;
		using MockNiceIOpenKitComposite_t = testing::NiceMock<MockIOpenKitComposite_t>;
		using MockNiceIOpenKitComposite_sp = std::shared_ptr<MockNiceIOpenKitComposite_t>;
		using MockStrictIOpenKitComposite_t = testing::StrictMock<MockIOpenKitComposite_t>;
		using MockStrictIOpenKitComposite_sp = std::shared_ptr<MockStrictIOpenKitComposite_t>;

		using MockIOpenKitObject_t = MockIOpenKitObject;
		using MockNiceIOpenKitObject_t = testing::NiceMock<MockIOpenKitObject_t>;
		using MockNiceIOpenKitObject_sp = std::shared_ptr<MockNiceIOpenKitObject_t>;
		using MockStrictIOpenKitObject_t = testing::StrictMock<MockIOpenKitObject_t>;
		using MockStrictIOpenKitObject_sp = std::shared_ptr<MockStrictIOpenKitObject_t>;

		using MockIWebRequestTracer_t = MockIWebRequestTracer;
		using MockNiceIWebRequestTracer_t = testing::NiceMock<MockIWebRequestTracer_t>;
		using MockNiceIWebRequestTracer_sp = std::shared_ptr<MockNiceIWebRequestTracer_t>;
		using MockStrictIWebRequestTracer_t = testing::StrictMock<MockIWebRequestTracer_t>;
		using MockStrictIWebRequestTracer_sp = std::shared_ptr<MockStrictIWebRequestTracer_t>;

		using MockOpenKitComposite_t = MockIOpenKitComposite;
		using MockNiceOpenKitComposite_t = testing::NiceMock<MockOpenKitComposite_t>;
		using MockNiceOpenKitComposite_sp = std::shared_ptr<MockNiceOpenKitComposite_t>;
		using MockStrictOpenKitComposite_t = testing::StrictMock<MockOpenKitComposite_t>;
		using MockStrictOpenKitComposite_sp = std::shared_ptr<MockStrictOpenKitComposite_t>;

		using MockRootAction_t = MockRootAction;
		using MockNiceRootAction_t = testing::NiceMock<MockRootAction_t>;
		using MockNiceRootAction_sp = std::shared_ptr<MockNiceRootAction_t>;

		using MockSession_t = MockSession;
		using MockNiceSession_t = testing::NiceMock<MockSession_t>;
		using MockNiceSession_sp = std::shared_ptr<MockNiceSession_t>;
		using MockStrictSession_t = testing::StrictMock<MockSession_t>;
		using MockStrictSession_sp = std::shared_ptr<MockStrictSession_t>;

		using MockWebRequestTracer_t = MockWebRequestTracer;
		using MockNiceWebRequestTracer_t = testing::NiceMock<MockWebRequestTracer_t>;
		using MockNiceWebRequestTracer_sp = std::shared_ptr<MockNiceWebRequestTracer_t>;
	}
}

#endif