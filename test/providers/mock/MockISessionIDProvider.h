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

#ifndef _TEST_PROVIDERS_MOCK_MOCKISESSIONIDPROVIDER_H
#define _TEST_PROVIDERS_MOCK_MOCKISESSIONIDPROVIDER_H

#include "providers/ISessionIDProvider.h"

#include "gmock/gmock.h"

#include <memory>

namespace test {
	class MockISessionIDProvider
		: public providers::ISessionIDProvider
	{
	public:

		///
		/// Default constructor
		///
		MockISessionIDProvider() = default;

		~MockISessionIDProvider() override = default;

		static std::shared_ptr<testing::NiceMock<MockISessionIDProvider>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockISessionIDProvider>>();
		}

		static std::shared_ptr<testing::StrictMock<MockISessionIDProvider>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockISessionIDProvider>>();
		}

		MOCK_METHOD0(getNextSessionID, int32_t());

	};
}

#endif