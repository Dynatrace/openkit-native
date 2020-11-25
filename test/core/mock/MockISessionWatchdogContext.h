/**
 * Copyright 2018-2020 Dynatrace LLC
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

		MOCK_METHOD0(execute, void());

		MOCK_METHOD0(requestShutdown, void());

		MOCK_METHOD0(isShutdownRequested, bool());

		MOCK_METHOD2(closeOrEnqueueForClosing,
			void(
				std::shared_ptr<core::objects::SessionInternals>, /* session */
				int64_t /* closeGracePeriodInMillis */
			)
		);

		MOCK_METHOD1(dequeueFromClosing,
			void(
				std::shared_ptr<core::objects::SessionInternals> /* session */
			)
		);

		MOCK_METHOD1(addToSplitByTimeout,
			void(
				std::shared_ptr<core::objects::ISessionProxy> /* sessionProxy */
			)
		);

		MOCK_METHOD1(removeFromSplitByTimeout,
			void(
				std::shared_ptr<core::objects::ISessionProxy> /* sessionProxy */
			)
		);
	};
}

#endif
