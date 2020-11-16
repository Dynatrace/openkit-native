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

#ifndef _TEST_OBJECTS_MOCKIOPENKITINITIALIZER_H
#define _TEST_OBJECTS_MOCKIOPENKITINITIALIZER_H

#include "core/objects/IOpenKitInitializer.h"

#include "gmock/gmock.h"

namespace test
{
	class MockIOpenKitInitializer : public core::objects::IOpenKitInitializer
	{
	public:

		~MockIOpenKitInitializer() override = default;

		static std::shared_ptr<testing::NiceMock<MockIOpenKitInitializer>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockIOpenKitInitializer>>();
		}

		static std::shared_ptr<testing::StrictMock<MockIOpenKitInitializer>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockIOpenKitInitializer>>();
		}

		MOCK_CONST_METHOD0(getLogger, std::shared_ptr<openkit::ILogger>());

		MOCK_CONST_METHOD0(getPrivacyConfiguration, std::shared_ptr<core::configuration::IPrivacyConfiguration>());

		MOCK_CONST_METHOD0(getOpenKitConfiguration, std::shared_ptr<core::configuration::IOpenKitConfiguration>());

		MOCK_CONST_METHOD0(getTimingProvider, std::shared_ptr<providers::ITimingProvider>());

		MOCK_CONST_METHOD0(getThreadIdProvider, std::shared_ptr<providers::IThreadIDProvider>());

		MOCK_CONST_METHOD0(getSessionIdProvider, std::shared_ptr<providers::ISessionIDProvider>());

		MOCK_CONST_METHOD0(getBeaconCache, std::shared_ptr<core::caching::IBeaconCache>());

		MOCK_CONST_METHOD0(getBeaconCacheEvictor, std::shared_ptr<core::caching::IBeaconCacheEvictor>());

		MOCK_CONST_METHOD0(getBeaconSender, std::shared_ptr<core::IBeaconSender>());

		MOCK_CONST_METHOD0(getSessionWatchdog, std::shared_ptr<core::ISessionWatchdog>());
	};
}

#endif
