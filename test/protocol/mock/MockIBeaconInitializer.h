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

#ifndef _TEST_PROTOCOL_MOCK_MOCKIBEACONINITIALIZER_H
#define _TEST_PROTOCOL_MOCK_MOCKIBEACONINITIALIZER_H

#include "protocol/IBeaconInitializer.h"

#include "gmock/gmock.h"

namespace test
{
	class MockIBeaconInitializer : public protocol::IBeaconInitializer
	{
	public:

		~MockIBeaconInitializer() override = default;

		static std::shared_ptr<testing::NiceMock<MockIBeaconInitializer>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockIBeaconInitializer>>();
		}

		static std::shared_ptr<testing::StrictMock<MockIBeaconInitializer>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockIBeaconInitializer>>();
		}

		MOCK_METHOD(std::shared_ptr<openkit::ILogger>, getLogger, (), (const, override));

		MOCK_METHOD(std::shared_ptr<core::caching::IBeaconCache>, getBeaconCache, (), (const, override));

		MOCK_METHOD(bool, useClientIpAddress, (), (const, override));

		MOCK_METHOD(const core::UTF8String&, getClientIpAddress, (), (const, override));

		MOCK_METHOD(std::shared_ptr<providers::ISessionIDProvider>, getSessionIdProvider, (), (const, override));

		MOCK_METHOD(int32_t, getSessionSequenceNumber, (), (const, override));

		MOCK_METHOD(std::shared_ptr<providers::IThreadIDProvider>, getThreadIdProvider, (), (const, override));

		MOCK_METHOD(std::shared_ptr<providers::ITimingProvider>, getTiminigProvider, (), (const, override));

		MOCK_METHOD(std::shared_ptr<providers::IPRNGenerator>, getRandomNumberGenerator, (), (const, override));

		MOCK_METHOD(std::shared_ptr<core::objects::ISupplementaryBasicData>, getSupplementaryBasicData, (), (const, override));
	};
}

#endif
