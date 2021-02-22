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

		MOCK_METHOD(std::shared_ptr<openkit::ILogger>, getLogger, (), (const, override));

		MOCK_METHOD(std::shared_ptr<core::configuration::IPrivacyConfiguration>, getPrivacyConfiguration, (), (const, override));

		MOCK_METHOD(std::shared_ptr<core::configuration::IOpenKitConfiguration>, getOpenKitConfiguration, (), (const, override));

		MOCK_METHOD(std::shared_ptr<providers::ITimingProvider>, getTimingProvider, (), (const, override));

		MOCK_METHOD(std::shared_ptr<providers::IThreadIDProvider>, getThreadIdProvider, (), (const, override));

		MOCK_METHOD(std::shared_ptr<providers::ISessionIDProvider>, getSessionIdProvider, (), (const, override));

		MOCK_METHOD(std::shared_ptr<core::caching::IBeaconCache>, getBeaconCache, (), (const, override));

		MOCK_METHOD(std::shared_ptr<core::caching::IBeaconCacheEvictor>, getBeaconCacheEvictor, (), (const, override));

		MOCK_METHOD(std::shared_ptr<core::IBeaconSender>, getBeaconSender, (), (const, override));

		MOCK_METHOD(std::shared_ptr<core::ISessionWatchdog>, getSessionWatchdog, (), (const, override));
	};
}

#endif
