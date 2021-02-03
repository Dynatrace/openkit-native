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

#ifndef _TEST_CORE_CONFIGURATION_MOCK_MOCKIBEACONCACHECONFIGURATION_H
#define _TEST_CORE_CONFIGURATION_MOCK_MOCKIBEACONCACHECONFIGURATION_H

#include "core/configuration/ConfigurationDefaults.h"
#include "core/configuration/IBeaconCacheConfiguration.h"

#include "gmock/gmock.h"

#include <memory>

namespace test
{
	class MockIBeaconCacheConfiguration
		: public core::configuration::IBeaconCacheConfiguration
	{
	public:

		MockIBeaconCacheConfiguration()
		{
			ON_CALL(*this, getMaxRecordAge())
				.WillByDefault(testing::Return(core::configuration::DEFAULT_MAX_RECORD_AGE_IN_MILLIS));
			ON_CALL(*this, getCacheSizeLowerBound())
				.WillByDefault(testing::Return(core::configuration::DEFAULT_LOWER_MEMORY_BOUNDARY_IN_BYTES));
			ON_CALL(*this, getCacheSizeUpperBound())
				.WillByDefault(testing::Return(core::configuration::DEFAULT_UPPER_MEMORY_BOUNDARY_IN_BYTES));
		}

		~MockIBeaconCacheConfiguration() override = default;

		static std::shared_ptr<testing::NiceMock<MockIBeaconCacheConfiguration>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockIBeaconCacheConfiguration>>();
		}

		static std::shared_ptr<testing::StrictMock<MockIBeaconCacheConfiguration>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockIBeaconCacheConfiguration>>();
		}

		MOCK_CONST_METHOD0(getMaxRecordAge, int64_t());

		MOCK_CONST_METHOD0(getCacheSizeLowerBound, int64_t());

		MOCK_CONST_METHOD0(getCacheSizeUpperBound, int64_t());
	};
}

#endif
