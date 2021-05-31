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

#ifndef _TEST_PROVIDERS_MOCK_MOCKIPRNGENERATOR_H
#define _TEST_PROVIDERS_MOCK_MOCKIPRNGENERATOR_H

#include "providers/IPRNGenerator.h"

#include "gmock/gmock.h"

#include <memory>

namespace test {
	class MockIPRNGenerator
		: public providers::IPRNGenerator
	{
	public:

		///
		/// Default constructor
		///
		MockIPRNGenerator() = default;

		~MockIPRNGenerator() override = default;

		static std::shared_ptr<testing::NiceMock<MockIPRNGenerator>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockIPRNGenerator>>();
		}

		static std::shared_ptr<testing::StrictMock<MockIPRNGenerator>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockIPRNGenerator>>();
		}

		MOCK_METHOD(int32_t, nextPercentageValue, (), (override));
		MOCK_METHOD(int64_t, nextPositiveInt64, (), (override));
	};
}

#endif