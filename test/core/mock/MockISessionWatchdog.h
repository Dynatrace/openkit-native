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

#ifndef _TEST_CORE_MOCK_MOCKISESSIONWATCHDOG_H
#define _TEST_CORE_MOCK_MOCKISESSIONWATCHDOG_H

#include "core/ISessionWatchdog.h"

#include "gmock/gmock.h"

namespace test
{
	class MockISessionWatchdog
		: public core::ISessionWatchdog
	{
	public:

		~MockISessionWatchdog() override = default;

		static std::shared_ptr<testing::NiceMock<MockISessionWatchdog>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockISessionWatchdog>>();
		}

		static std::shared_ptr<testing::StrictMock<MockISessionWatchdog>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockISessionWatchdog>>();
		}

		MOCK_METHOD0(initialize, void());

		MOCK_METHOD0(shutdown, void());

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
	};
}

#endif
