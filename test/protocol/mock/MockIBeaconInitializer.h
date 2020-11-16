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

		MOCK_CONST_METHOD0(getLogger, std::shared_ptr<openkit::ILogger>());

		MOCK_CONST_METHOD0(getBeaconCache, std::shared_ptr<core::caching::IBeaconCache>());

		MOCK_CONST_METHOD0(useClientIpAddress, bool());

		MOCK_CONST_METHOD0(getClientIpAddress, const core::UTF8String& ());

		MOCK_CONST_METHOD0(getSessionIdProvider, std::shared_ptr<providers::ISessionIDProvider>());

		MOCK_CONST_METHOD0(getSessionSequenceNumber, int32_t());

		MOCK_CONST_METHOD0(getThreadIdProvider, std::shared_ptr<providers::IThreadIDProvider>());

		MOCK_CONST_METHOD0(getTiminigProvider, std::shared_ptr<providers::ITimingProvider>());

		MOCK_CONST_METHOD0(getRandomNumberGenerator, std::shared_ptr<providers::IPRNGenerator>());
	};
}

#endif
