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

#ifndef _TEST_API_MOCK_MOCKIROOTACTION_H
#define _TEST_API_MOCK_MOCKIROOTACTION_H

#include "OpenKit/IRootAction.h"
#include "OpenKit/IAction.h"
#include "OpenKit/IWebRequestTracer.h"

#include "gmock/gmock.h"

#include <memory>

namespace test
{
	class MockIRootAction
		: public openkit::IRootAction
	{
	public:

		~MockIRootAction() override = default;

		static std::shared_ptr<testing::NiceMock<MockIRootAction>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockIRootAction>>();
		}

		static std::shared_ptr<testing::StrictMock<MockIRootAction>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockIRootAction>>();
		}

		MOCK_METHOD(
			std::shared_ptr<openkit::IAction>, enterAction, (const char*),  (override)
		);

		MOCK_METHOD(
			std::shared_ptr<openkit::IRootAction>, reportEvent, (const char*), (override)
		);

		MOCK_METHOD(
			std::shared_ptr<openkit::IRootAction>, reportValue, (const char*, int32_t), (override)
		);

		MOCK_METHOD(
			std::shared_ptr<openkit::IRootAction>, reportValue, (const char*, int64_t), (override)
		);

		MOCK_METHOD(
			std::shared_ptr<openkit::IRootAction>, reportValue, (const char*, double), (override)
		);

		MOCK_METHOD(
			std::shared_ptr<openkit::IRootAction>, reportValue, (const char*, const char*), (override)
		);

		MOCK_METHOD(
			std::shared_ptr<openkit::IRootAction>, reportError, (const char*, int32_t, const char*), (override)
		);

		MOCK_METHOD(
			std::shared_ptr<openkit::IWebRequestTracer>, traceWebRequest, (const char*), (override)
		);

		MOCK_METHOD(void, leaveAction, (), (override));
	};
}

#endif
