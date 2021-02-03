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

#ifndef _TEST_CORE_CONFIGURATION_MOCK_MOCKIBEACONCONFIGURATION_H
#define _TEST_CORE_CONFIGURATION_MOCK_MOCKIBEACONCONFIGURATION_H

#include "core/configuration/ConfigurationDefaults.h"
#include "core/configuration/IBeaconConfiguration.h"
#include "core/configuration/IHTTPClientConfiguration.h"
#include "core/configuration/IOpenKitConfiguration.h"
#include "core/configuration/IPrivacyConfiguration.h"
#include "core/configuration/IServerConfiguration.h"

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
			ON_CALL(*this, getOpenKitConfiguration()).WillByDefault(testing::Return(nullptr));
			ON_CALL(*this, getPrivacyConfiguration()).WillByDefault(testing::Return(nullptr));
			ON_CALL(*this, getHTTPClientConfiguration()).WillByDefault(testing::Return(nullptr));
			ON_CALL(*this, getServerConfiguration()).WillByDefault(testing::Return(nullptr));
		}

		~MockIBeaconConfiguration() override = default;

		static std::shared_ptr<testing::NiceMock<MockIBeaconConfiguration>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockIBeaconConfiguration>>();
		}

		static std::shared_ptr<testing::StrictMock<MockIBeaconConfiguration>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockIBeaconConfiguration>>();
		}

		MOCK_CONST_METHOD0(getOpenKitConfiguration, std::shared_ptr<core::configuration::IOpenKitConfiguration>());

		MOCK_CONST_METHOD0(getPrivacyConfiguration, std::shared_ptr<core::configuration::IPrivacyConfiguration>());

		MOCK_CONST_METHOD0(getHTTPClientConfiguration, std::shared_ptr<core::configuration::IHTTPClientConfiguration>());

		MOCK_METHOD0(getServerConfiguration, std::shared_ptr<core::configuration::IServerConfiguration>());

		MOCK_METHOD1(initializeServerConfiguration,
			void(
				std::shared_ptr<core::configuration::IServerConfiguration> /* initialServerConfiguration */
			)
		);

		MOCK_METHOD0(enableCapture, void());

		MOCK_METHOD0(disableCapture, void());

		MOCK_METHOD1(updateServerConfiguration,
			void(
				std::shared_ptr<core::configuration::IServerConfiguration>
			)
		);

		MOCK_METHOD0(isServerConfigurationSet, bool());

		MOCK_METHOD1(setServerConfigurationUpdateCallback, void(core::configuration::ServerConfigurationUpdateCallback serverConfigurationUpdateCallback));
	};
}

#endif
