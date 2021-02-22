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

#ifndef _TEST_CORE_CONFIGURATION_MOCK_MOCKISERVERCONFIGURATION_H
#define _TEST_CORE_CONFIGURATION_MOCK_MOCKISERVERCONFIGURATION_H

#include "core/configuration/IServerConfiguration.h"
#include "core/configuration/ServerConfiguration.h"

#include "gmock/gmock.h"

#include <memory>

namespace test
{
	class MockIServerConfiguration
		: public core::configuration::IServerConfiguration
	{
	public:

		MockIServerConfiguration()
		{
			ON_CALL(*this, isCaptureEnabled())
				.WillByDefault(testing::Return(core::configuration::ServerConfiguration::defaultValues()->isCapture()));
			ON_CALL(*this, isCrashReportingEnabled())
				.WillByDefault(testing::Return(core::configuration::ServerConfiguration::defaultValues()->isCaptureCrashes()));
			ON_CALL(*this, isErrorReportingEnabled())
				.WillByDefault(testing::Return(core::configuration::ServerConfiguration::defaultValues()->isCaptureErrors()));
			ON_CALL(*this, getServerId())
				.WillByDefault(testing::Return(core::configuration::ServerConfiguration::defaultValues()->getServerId()));
			ON_CALL(*this, getBeaconSizeInBytes())
				.WillByDefault(testing::Return(core::configuration::ServerConfiguration::defaultValues()->getMaxBeaconSizeInBytes()));
			ON_CALL(*this, getMultiplicity())
				.WillByDefault(testing::Return(core::configuration::ServerConfiguration::defaultValues()->getMultiplicity()));
			ON_CALL(*this, getSendIntervalInMilliseconds())
				.WillByDefault(testing::Return(core::configuration::ServerConfiguration::defaultValues()->getSendIntervalInMilliseconds()));
			ON_CALL(*this, isSendingDataAllowed())
				.WillByDefault(testing::Return(true));
			ON_CALL(*this, isSendingCrashesAllowed())
				.WillByDefault(testing::Return(true));
			ON_CALL(*this, isSendingErrorsAllowed())
				.WillByDefault(testing::Return(true));
		}

		~MockIServerConfiguration() override = default;

		static std::shared_ptr<testing::NiceMock<MockIServerConfiguration>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockIServerConfiguration>>();
		}

		static std::shared_ptr<testing::StrictMock<MockIServerConfiguration>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockIServerConfiguration>>();
		}

		MOCK_METHOD(bool, isCaptureEnabled, (), (const, override));

		MOCK_METHOD(bool, isCrashReportingEnabled, (), (const, override));

		MOCK_METHOD(bool, isErrorReportingEnabled, (), (const, override));

		MOCK_METHOD(int32_t, getServerId, (), (const, override));

		MOCK_METHOD(int32_t, getBeaconSizeInBytes, (), (const, override));

		MOCK_METHOD(int32_t, getMultiplicity, (), (const, override));

		MOCK_METHOD(int32_t, getSendIntervalInMilliseconds, (), (const, override));

		MOCK_METHOD(int32_t, getMaxSessionDurationInMilliseconds, (), (const, override));

		MOCK_METHOD(bool, isSessionSplitBySessionDurationEnabled, (), (const, override));

		MOCK_METHOD(int32_t, getMaxEventsPerSession, (), (const, override));

		MOCK_METHOD(bool, isSessionSplitByEventsEnabled, (), (const, override));

		MOCK_METHOD(int32_t, getSessionTimeoutInMilliseconds, (), (const, override));

		MOCK_METHOD(bool, isSessionSplitByIdleTimeoutEnabled, (), (const, override));

		MOCK_METHOD(int32_t, getVisitStoreVersion, (), (const, override));

		MOCK_METHOD(bool, isSendingDataAllowed, (), (const, override));

		MOCK_METHOD(bool, isSendingCrashesAllowed, (), (const, override));

		MOCK_METHOD(bool, isSendingErrorsAllowed, (), (const, override));

		MOCK_METHOD(
			std::shared_ptr<core::configuration::IServerConfiguration>,
			merge,
			(
				std::shared_ptr<core::configuration::IServerConfiguration> /* other */
			),
			(const, override)
		);
	};
}

#endif
