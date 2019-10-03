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

#include "MockAction.h"
#include "MockOpenKitComposite.h"
#include "MockRootAction.h"
#include "MockSession.h"
#include "MockWebRequestTracer.h"

#include "gmock/gmock.h"

#include <memory>

namespace test
{
	namespace types
	{
		using MockAction_t = MockAction;
		using MockNiceAction_t = testing::NiceMock<MockAction_t>;
		using MockNiceAction_sp = std::shared_ptr<MockNiceAction_t>;

		using MockOpenKitComposite_t = MockOpenKitComposite;
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