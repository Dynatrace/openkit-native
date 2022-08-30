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

		MOCK_METHOD(const std::string&, getOpenKitType, (), (const, override));

		MOCK_METHOD(const std::string&, getApplicationID, (), (const, override));

		MOCK_METHOD(int32_t, getDefaultServerID, (), (const, override));

		MOCK_METHOD(const std::string&, getApplicationVersion, (), (const, override));

		MOCK_METHOD(const std::string&, getOperatingSystem, (), (const, override));

		MOCK_METHOD(const std::string&, getManufacturer, (), (const, override));

		MOCK_METHOD(const std::string&, getModelID, (), (const, override));

		MOCK_METHOD(const std::string&, getEndpointURL, (), (const, override));

		MOCK_METHOD(int64_t, getDeviceID, (), (const, override));

		MOCK_METHOD(const std::string&, getOrigDeviceID, (), (const, override));

		MOCK_METHOD(std::shared_ptr<openkit::ISSLTrustManager>, getTrustManager, (), (const, override));

		MOCK_METHOD(int64_t, getBeaconCacheMaxRecordAge, (), (const, override));

		MOCK_METHOD(int64_t, getBeaconCacheLowerMemoryBoundary, (), (const, override));

		MOCK_METHOD(int64_t, getBeaconCacheUpperMemoryBoundary, (), (const, override));

		MOCK_METHOD(openkit::DataCollectionLevel, getDataCollectionLevel, (), (const, override));

		MOCK_METHOD(openkit::CrashReportingLevel, getCrashReportingLevel, (), (const, override));

		MOCK_METHOD(openkit::LogLevel, getLogLevel, (), (const, override));

		MOCK_METHOD(std::shared_ptr<openkit::ILogger>, getLogger, (), (const, override));

		MOCK_METHOD(std::shared_ptr<openkit::IHttpRequestInterceptor>, getHttpRequestInterceptor, (), (const, override));

		MOCK_METHOD(std::shared_ptr<openkit::IHttpResponseInterceptor>, getHttpResponseInterceptor, (), (const, override));
	};
}

#endif
