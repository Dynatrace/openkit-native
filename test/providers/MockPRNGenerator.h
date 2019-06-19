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

#ifndef _TEST_PROVIDERS_MOCKPRNGENERATOR_H
#define _TEST_PROVIDERS_MOCKPRNGENERATOR_H

#include "providers/IPRNGenerator.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

namespace test {
	class MockPRNGenerator : public providers::IPRNGenerator
	{
	public:

		///
		/// Default constructor
		///
		MockPRNGenerator()
		{
		}

		virtual ~MockPRNGenerator() {}

		MOCK_METHOD1(nextInt32, int32_t(int32_t));
		MOCK_METHOD1(nextInt64, int64_t(int64_t));
	};
}

#endif