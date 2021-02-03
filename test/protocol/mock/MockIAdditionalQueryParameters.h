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

#ifndef _TEST_PROTOCOL_MOCK_MOCKIADDITIONALQUERYPARAMETERS_H
#define _TEST_PROTOCOL_MOCK_MOCKIADDITIONALQUERYPARAMETERS_H

#include "protocol/IAdditionalQueryParameters.h"

#include "gmock/gmock.h"

#include <memory>

namespace test
{
	class MockIAdditionalQueryParameters
		: public protocol::IAdditionalQueryParameters
	{
	public:

		~MockIAdditionalQueryParameters() override = default;

		static std::shared_ptr<testing::NiceMock<MockIAdditionalQueryParameters>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockIAdditionalQueryParameters>>();
		}

		static std::shared_ptr<testing::StrictMock<MockIAdditionalQueryParameters>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockIAdditionalQueryParameters>>();
		}

		MOCK_CONST_METHOD0(getConfigurationTimestamp, int64_t());
	};
}

#endif
