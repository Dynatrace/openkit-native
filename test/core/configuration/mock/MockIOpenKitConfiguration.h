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

#ifndef _TEST_CORE_CONFIGURATION_MOCK_MOCKIOPENKITCONFIGURATION_H
#define _TEST_CORE_CONFIGURATION_MOCK_MOCKIOPENKITCONFIGURATION_H

#include "core/configuration/IOpenKitConfiguration.h"

#include "../../../DefaultValues.h"

#include "gmock/gmock.h"

#include <memory>

namespace test
{
	class MockIOpenKitConfiguration
		: public core::configuration::IOpenKitConfiguration
	{
	public:

		MockIOpenKitConfiguration()
		{
			ON_CALL(*this, getEndpointUrl())
				.WillByDefault(testing::ReturnRef(DefaultValues::UTF8_EMPTY_STRING));
			ON_CALL(*this, getOrigDeviceId())
				.WillByDefault(testing::ReturnRef(DefaultValues::UTF8_EMPTY_STRING));
			ON_CALL(*this, getOpenKitType())
				.WillByDefault(testing::ReturnRef(DefaultValues::EMPTY_STRING));
			ON_CALL(*this, getApplicationId())
				.WillByDefault(testing::ReturnRef(DefaultValues::UTF8_EMPTY_STRING));
			ON_CALL(*this, getApplicationIdPercentEncoded())
				.WillByDefault(testing::ReturnRef(DefaultValues::UTF8_EMPTY_STRING));
			ON_CALL(*this, getApplicationName())
				.WillByDefault(testing::ReturnRef(DefaultValues::UTF8_EMPTY_STRING));
			ON_CALL(*this, getApplicationVersion())
				.WillByDefault(testing::ReturnRef(DefaultValues::UTF8_EMPTY_STRING));
			ON_CALL(*this, getOperatingSystem())
				.WillByDefault(testing::ReturnRef(DefaultValues::UTF8_EMPTY_STRING));
			ON_CALL(*this, getManufacturer())
				.WillByDefault(testing::ReturnRef(DefaultValues::UTF8_EMPTY_STRING));
			ON_CALL(*this, getModelId())
				.WillByDefault(testing::ReturnRef(DefaultValues::UTF8_EMPTY_STRING));

			ON_CALL(*this, getTrustManager())
				.WillByDefault(testing::Return(nullptr));
		}

		~MockIOpenKitConfiguration() override = default;

		static std::shared_ptr<testing::NiceMock<MockIOpenKitConfiguration>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockIOpenKitConfiguration>>();
		}

		static std::shared_ptr<testing::StrictMock<MockIOpenKitConfiguration>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockIOpenKitConfiguration>>();
		}

		MOCK_METHOD(const core::UTF8String&, getEndpointUrl, (), (const, override));

		MOCK_METHOD(int64_t, getDeviceId, (), (const, override));

		MOCK_METHOD(const core::UTF8String&, getOrigDeviceId, (), (const, override));

		MOCK_METHOD(const std::string&, getOpenKitType, (), (const, override));

		MOCK_METHOD(const core::UTF8String&, getApplicationId, (), (const, override));

		MOCK_METHOD(const core::UTF8String&, getApplicationIdPercentEncoded, (), (const, override));

		MOCK_METHOD(const core::UTF8String&, getApplicationName, (), (const, override));

		MOCK_METHOD(const core::UTF8String&, getApplicationVersion, (), (const, override));

		MOCK_METHOD(const core::UTF8String&, getOperatingSystem, (), (const, override));

		MOCK_METHOD(const core::UTF8String&, getManufacturer, (), (const, override));

		MOCK_METHOD(const core::UTF8String&, getModelId, (), (const, override));

		MOCK_METHOD(int32_t, getDefaultServerId, (), (const, override));

		MOCK_METHOD(std::shared_ptr<openkit::ISSLTrustManager>, getTrustManager, (), (const, override));

		MOCK_METHOD(std::shared_ptr<openkit::IHttpRequestInterceptor>, getHttpRequestInterceptor, (), (const, override));

		MOCK_METHOD(std::shared_ptr<openkit::IHttpResponseInterceptor>, getHttpResponseInterceptor, (), (const, override));
	};
}

#endif
