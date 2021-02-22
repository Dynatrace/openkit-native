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

#ifndef _TEST_CORE_MOCK_MOCKIBEACONSENDER_H
#define _TEST_CORE_MOCK_MOCKIBEACONSENDER_H

#include "core/IBeaconSender.h"
#include "core/objects/SessionInternals.h"

#include "gmock/gmock.h"

#include <memory>

namespace test
{
	class MockIBeaconSender
		: public core::IBeaconSender
	{
	public:

		~MockIBeaconSender() override = default;

		static std::shared_ptr<testing::NiceMock<MockIBeaconSender>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockIBeaconSender>>();
		}

		static std::shared_ptr<testing::StrictMock<MockIBeaconSender>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockIBeaconSender>>();
		}

		MOCK_METHOD(bool, initialize, (), (override));

		MOCK_METHOD(bool, waitForInit, (), (const, override));

		MOCK_METHOD(bool, waitForInit, (int64_t), (const, override));

		MOCK_METHOD(bool, isInitialized, (), (const, override));

		MOCK_METHOD(void, shutdown, (), (override));

		MOCK_METHOD(std::shared_ptr<core::configuration::IServerConfiguration>, getLastServerConfiguration, (), (override));

		MOCK_METHOD(int32_t, getCurrentServerID, (), (const, override));

		MOCK_METHOD(void, addSession, (std::shared_ptr<core::objects::SessionInternals>), (override));
	};
}
#endif
