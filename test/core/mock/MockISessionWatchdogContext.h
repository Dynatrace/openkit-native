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

#ifndef _TEST_CORE_MOCK_MOCKISESSIONWATCHDOGCONTEXT_H
#define _TEST_CORE_MOCK_MOCKISESSIONWATCHDOGCONTEXT_H

#include "core/ISessionWatchdogContext.h"

#include <memory>

#include "gmock/gmock.h"

namespace test
{
	class MockISessionWatchdogContext
		: public core::ISessionWatchdogContext
	{
	public:

		~MockISessionWatchdogContext() override = default;

		static std::shared_ptr<testing::NiceMock<MockISessionWatchdogContext>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockISessionWatchdogContext>>();
		}

		static std::shared_ptr<testing::StrictMock<MockISessionWatchdogContext>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockISessionWatchdogContext>>();
		}

		MOCK_METHOD(void, execute, (), (override));

		MOCK_METHOD(void, requestShutdown, (), (override));

		MOCK_METHOD(bool, isShutdownRequested, (), (override));

		MOCK_METHOD(
			void,
			closeOrEnqueueForClosing,
			(
				std::shared_ptr<core::objects::SessionInternals>, /* session */
				int64_t /* closeGracePeriodInMillis */
			),
			(override)
		);

		MOCK_METHOD(
			void,
			dequeueFromClosing,
			(
				std::shared_ptr<core::objects::SessionInternals> /* session */
			),
			(override)
		);

		MOCK_METHOD(
			void,
			addToSplitByTimeout,
			(
				std::shared_ptr<core::objects::ISessionProxy> /* sessionProxy */
			),
			(override)
		);

		MOCK_METHOD(
			void,
			removeFromSplitByTimeout,
			(
				std::shared_ptr<core::objects::ISessionProxy> /* sessionProxy */
			),
			(override)
		);
	};
}

#endif
