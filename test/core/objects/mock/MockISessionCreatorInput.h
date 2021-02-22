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

#ifndef _TEST_CORE_OBJECTS_MOCK_MOCKISESSIONCREATORINPUT_H
#define _TEST_CORE_OBJECTS_MOCK_MOCKISESSIONCREATORINPUT_H

#include "core/objects/ISessionCreatorInput.h"

#include "gmock/gmock.h"

#include <memory>

namespace test
{
	class MockISessionCreatorInput
		: public core::objects::ISessionCreatorInput
	{
	public:

		MockISessionCreatorInput()
		{
			ON_CALL(*this, getLogger())
				.WillByDefault(testing::Return(nullptr));
			ON_CALL(*this, getOpenKitConfiguration())
				.WillByDefault(testing::Return(nullptr));
			ON_CALL(*this, getPrivacyConfiguration())
				.WillByDefault(testing::Return(nullptr));
			ON_CALL(*this, getBeaconCache())
				.WillByDefault(testing::Return(nullptr));
			ON_CALL(*this, getSessionIdProvider())
				.WillByDefault(testing::Return(nullptr));
			ON_CALL(*this, getThreadIdProvider())
				.WillByDefault(testing::Return(nullptr));
			ON_CALL(*this, getTimingProvider())
				.WillByDefault(testing::Return(nullptr));
			ON_CALL(*this, getCurrentServerId())
				.WillByDefault(testing::Return(1));
		}

		~MockISessionCreatorInput() override = default;

		static std::shared_ptr<testing::NiceMock<MockISessionCreatorInput>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockISessionCreatorInput>>();
		}

		static std::shared_ptr<testing::StrictMock<MockISessionCreatorInput>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockISessionCreatorInput>>();
		}

		MOCK_METHOD(std::shared_ptr<openkit::ILogger>, getLogger, (), (override));

		MOCK_METHOD(std::shared_ptr<core::configuration::IOpenKitConfiguration>, getOpenKitConfiguration, (), (override));

		MOCK_METHOD(std::shared_ptr<core::configuration::IPrivacyConfiguration>, getPrivacyConfiguration, (), (override));

		MOCK_METHOD(std::shared_ptr<core::caching::IBeaconCache>, getBeaconCache, (), (override));

		MOCK_METHOD(std::shared_ptr<providers::ISessionIDProvider>, getSessionIdProvider, (), (override));

		MOCK_METHOD(std::shared_ptr<providers::IThreadIDProvider>, getThreadIdProvider, (), (override));

		MOCK_METHOD(std::shared_ptr<providers::ITimingProvider>, getTimingProvider, (), (override));
		
		MOCK_METHOD(int32_t, getCurrentServerId, (), (override));
	};
}

#endif
