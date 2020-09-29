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

		MOCK_CONST_METHOD0(getEndpointUrl, const core::UTF8String&());

		MOCK_CONST_METHOD0(getDeviceId, int64_t());

		MOCK_CONST_METHOD0(getOrigDeviceId, const core::UTF8String&());

		MOCK_CONST_METHOD0(getOpenKitType, const std::string&());

		MOCK_CONST_METHOD0(getApplicationId, const core::UTF8String&());

		MOCK_CONST_METHOD0(getApplicationIdPercentEncoded, const core::UTF8String&());

		MOCK_CONST_METHOD0(getApplicationName, const core::UTF8String&());

		MOCK_CONST_METHOD0(getApplicationVersion, const core::UTF8String&());

		MOCK_CONST_METHOD0(getOperatingSystem, const core::UTF8String&());

		MOCK_CONST_METHOD0(getManufacturer, const core::UTF8String&());

		MOCK_CONST_METHOD0(getModelId, const core::UTF8String&());

		MOCK_CONST_METHOD0(getDefaultServerId, int32_t());

		MOCK_CONST_METHOD0(getTrustManager, std::shared_ptr<openkit::ISSLTrustManager>());
	};
}

#endif
