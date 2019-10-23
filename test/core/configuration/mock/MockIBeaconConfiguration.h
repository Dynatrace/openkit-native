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

#ifndef _TEST_CORE_CONFIGURATION_MOCK_MOCKIBEACONCONFIGURATION_H
#define _TEST_CORE_CONFIGURATION_MOCK_MOCKIBEACONCONFIGURATION_H

#include "OpenKit/CrashReportingLevel.h"
#include "OpenKit/DataCollectionLevel.h"
#include "core/configuration/ConfigurationDefaults.h"
#include "core/configuration/IBeaconConfiguration.h"

#include "gmock/gmock.h"

#include <cstdint>
#include <memory>

namespace test
{
	class MockIBeaconConfiguration
		: public core::configuration::IBeaconConfiguration
	{
	public:

		MockIBeaconConfiguration()
		{
			ON_CALL(*this, getMultiplicity())
				.WillByDefault(testing::Return(core::configuration::DEFAULT_MULTIPLICITY));
			ON_CALL(*this, getCrashReportingLevel())
				.WillByDefault(testing::Return(core::configuration::DEFAULT_CRASH_REPORTING_LEVEL));
			ON_CALL(*this, getDataCollectionLevel())
				.WillByDefault(testing::Return(core::configuration::DEFAULT_DATA_COLLECTION_LEVEL));
		}

		virtual ~MockIBeaconConfiguration() {}

		static std::shared_ptr<testing::NiceMock<MockIBeaconConfiguration>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockIBeaconConfiguration>>();
		}

		static std::shared_ptr<testing::StrictMock<MockIBeaconConfiguration>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockIBeaconConfiguration>>();
		}

		MOCK_CONST_METHOD0(getDataCollectionLevel, openkit::DataCollectionLevel());

		MOCK_CONST_METHOD0(getCrashReportingLevel, openkit::CrashReportingLevel());

		MOCK_CONST_METHOD0(getMultiplicity, int32_t());
	};
}

#endif
