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

		MOCK_CONST_METHOD0(isCaptureEnabled, bool());

		MOCK_CONST_METHOD0(isCrashReportingEnabled, bool());

		MOCK_CONST_METHOD0(isErrorReportingEnabled, bool());

		MOCK_CONST_METHOD0(getServerId, int32_t());

		MOCK_CONST_METHOD0(getBeaconSizeInBytes, int32_t());

		MOCK_CONST_METHOD0(getMultiplicity, int32_t());

		MOCK_CONST_METHOD0(getMaxSessionDurationInMilliseconds, int32_t());

		MOCK_CONST_METHOD0(getMaxEventsPerSession, int32_t());

		MOCK_CONST_METHOD0(getSessionTimeoutInMilliseconds, int32_t());

		MOCK_CONST_METHOD0(isSessionSplitByEventsEnabled, bool());

		MOCK_CONST_METHOD0(getVisitStoreVersion, int32_t());

		MOCK_CONST_METHOD0(isSendingDataAllowed, bool());

		MOCK_CONST_METHOD0(isSendingCrashesAllowed, bool());

		MOCK_CONST_METHOD0(isSendingErrorsAllowed, bool());

		MOCK_CONST_METHOD1(merge,
			std::shared_ptr<core::configuration::IServerConfiguration>(
				std::shared_ptr<core::configuration::IServerConfiguration> /* other */
			)
		);
	};
}

#endif
