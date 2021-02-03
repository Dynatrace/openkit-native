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

#ifndef _TEST_CORE_CACHING_MOCK_MOCKIBEACONCACHEEVICTIONSTRATEGY_H
#define _TEST_CORE_CACHING_MOCK_MOCKIBEACONCACHEEVICTIONSTRATEGY_H

#include "core/caching/IBeaconCacheEvictionStrategy.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <memory>

namespace test
{
	class MockIBeaconCacheEvictionStrategy
		: public core::caching::IBeaconCacheEvictionStrategy
	{
	public:
		MockIBeaconCacheEvictionStrategy() = default;

		~MockIBeaconCacheEvictionStrategy() override = default;

		static std::shared_ptr<testing::NiceMock<MockIBeaconCacheEvictionStrategy>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockIBeaconCacheEvictionStrategy>>();
		}

		static std::shared_ptr<testing::StrictMock<MockIBeaconCacheEvictionStrategy>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockIBeaconCacheEvictionStrategy>>();
		}

		MOCK_METHOD0(execute, void());
	};
}
#endif
