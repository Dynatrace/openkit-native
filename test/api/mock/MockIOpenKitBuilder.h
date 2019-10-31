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

#ifndef _TEST_API_MOCK_MOCKIOPENKITBUILDER_H
#define _TEST_API_MOCK_MOCKIOPENKITBUILDER_H

#include "OpenKit/IOpenKitBuilder.h"
#include "OpenKit/LogLevel.h"
#include "core/configuration/ConfigurationDefaults.h"

#include "../../DefaultValues.h"

#include "gmock/gmock.h"

#include <memory>

namespace test
{
	class MockIOpenKitBuilder
		: public openkit::IOpenKitBuilder
	{
	public:

		MockIOpenKitBuilder()
		{
			ON_CALL(*this, getOpenKitType()).WillByDefault(testing::ReturnRef(DefaultValues::EMPTY_STRING));
			ON_CALL(*this, getApplicationID()).WillByDefault(testing::ReturnRef(DefaultValues::EMPTY_STRING));
			ON_CALL(*this, getApplicationName()).WillByDefault(testing::ReturnRef(DefaultValues::EMPTY_STRING));
			ON_CALL(*this, getApplicationVersion()).WillByDefault(testing::ReturnRef(DefaultValues::EMPTY_STRING));
			ON_CALL(*this, getOperatingSystem()).WillByDefault(testing::ReturnRef(DefaultValues::EMPTY_STRING));
			ON_CALL(*this, getManufacturer()).WillByDefault(testing::ReturnRef(DefaultValues::EMPTY_STRING));
			ON_CALL(*this, getModelID()).WillByDefault(testing::ReturnRef(DefaultValues::EMPTY_STRING));
			ON_CALL(*this, getEndpointURL()).WillByDefault(testing::ReturnRef(DefaultValues::EMPTY_STRING));
			ON_CALL(*this, getOrigDeviceID()).WillByDefault(testing::ReturnRef(DefaultValues::EMPTY_STRING));
			ON_CALL(*this, getTrustManager()).WillByDefault(testing::Return(nullptr));

			ON_CALL(*this, getDataCollectionLevel())
				.WillByDefault(testing::Return(core::configuration::DEFAULT_DATA_COLLECTION_LEVEL));
			ON_CALL(*this, getCrashReportingLevel())
				.WillByDefault(testing::Return(core::configuration::DEFAULT_CRASH_REPORTING_LEVEL));

			ON_CALL(*this, getLogLevel())
				.WillByDefault(testing::Return(openkit::LogLevel::LOG_LEVEL_WARN));
			ON_CALL(*this, getLogger())
				.WillByDefault(testing::Return(nullptr));
		}

		~MockIOpenKitBuilder() override = default;

		static std::shared_ptr<testing::NiceMock<MockIOpenKitBuilder>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockIOpenKitBuilder>>();
		}

		static std::shared_ptr<testing::StrictMock<MockIOpenKitBuilder>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockIOpenKitBuilder>>();
		}

		MOCK_CONST_METHOD0(getOpenKitType, const std::string&());

		MOCK_CONST_METHOD0(getApplicationID, const std::string&());

		MOCK_CONST_METHOD0(getApplicationName, const std::string&());

		MOCK_CONST_METHOD0(getDefaultServerID, int32_t());

		MOCK_CONST_METHOD0(getApplicationVersion, const std::string&());

		MOCK_CONST_METHOD0(getOperatingSystem, const std::string&());

		MOCK_CONST_METHOD0(getManufacturer, const std::string&());

		MOCK_CONST_METHOD0(getModelID, const std::string&());

		MOCK_CONST_METHOD0(getEndpointURL, const std::string&());

		MOCK_CONST_METHOD0(getDeviceID, int64_t());

		MOCK_CONST_METHOD0(getOrigDeviceID, const std::string&());

		MOCK_CONST_METHOD0(getTrustManager, std::shared_ptr<openkit::ISSLTrustManager>());

		MOCK_CONST_METHOD0(getBeaconCacheMaxRecordAge, int64_t());

		MOCK_CONST_METHOD0(getBeaconCacheLowerMemoryBoundary, int64_t());

		MOCK_CONST_METHOD0(getBeaconCacheUpperMemoryBoundary, int64_t());

		MOCK_CONST_METHOD0(getDataCollectionLevel, openkit::DataCollectionLevel());

		MOCK_CONST_METHOD0(getCrashReportingLevel, openkit::CrashReportingLevel());

		MOCK_CONST_METHOD0(getLogLevel, openkit::LogLevel());

		MOCK_CONST_METHOD0(getLogger, std::shared_ptr<openkit::ILogger>());
	};
}

#endif
