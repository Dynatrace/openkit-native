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

#ifndef _TEST_PROVIDERS_MOCK_MOCKITHREADIDPROVIDER_H
#define _TEST_PROVIDERS_MOCK_MOCKITHREADIDPROVIDER_H

#include "providers/IThreadIDProvider.h"

#include "gmock/gmock.h"

#include <memory>

namespace test
{
	class MockIThreadIDProvider
		: public providers::IThreadIDProvider
	{
	public:

		MockIThreadIDProvider() = default;

		~MockIThreadIDProvider() override = default;

		static std::shared_ptr<testing::NiceMock<MockIThreadIDProvider>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockIThreadIDProvider>>();
		}

		static std::shared_ptr<testing::StrictMock<MockIThreadIDProvider>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockIThreadIDProvider>>();
		}

		MOCK_METHOD0(getThreadID, int32_t());
	};
}

#endif
