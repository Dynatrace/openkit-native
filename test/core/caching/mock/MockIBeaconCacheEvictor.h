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

#ifndef _TEST_CORE_CACHING_MOCK_MOCKIBEACONCACHEEVICTOR_H
#define _TEST_CORE_CACHING_MOCK_MOCKIBEACONCACHEEVICTOR_H

#include "core/caching/IBeaconCacheEvictor.h"

#include "gmock/gmock.h"

#include <chrono>
#include <memory>

namespace test
{
	class MockIBeaconCacheEvictor
		: public core::caching::IBeaconCacheEvictor
	{
	public:

		~MockIBeaconCacheEvictor() override = default;

		static std::shared_ptr<testing::NiceMock<MockIBeaconCacheEvictor>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockIBeaconCacheEvictor>>();
		}

		static std::shared_ptr<testing::StrictMock<MockIBeaconCacheEvictor>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockIBeaconCacheEvictor>>();
		}

		MOCK_METHOD(bool, isAlive, (), (override));

		MOCK_METHOD(bool, start, (), (override));

		MOCK_METHOD(bool, stop, (), (override));

		MOCK_METHOD(bool, stop, (std::chrono::milliseconds), (override));
	};
}
#endif
