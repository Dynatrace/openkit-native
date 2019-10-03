/**
 * Copyright 2018-2019 Dynatrace LLC
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

#ifndef _TEST_CORE_CACHING_MOCKTYPES_H
#define _TEST_CORE_CACHING_MOCKTYPES_H

#include "MockBeaconCache.h"
#include "MockBeaconCacheEvictionStrategy.h"
#include "MockObserver.h"

#include "gmock/gmock.h"

namespace test
{
	namespace types
	{
		using MockBeaconCache_t = test::MockBeaconCache;
		using MockNiceBeaconCache_t = testing::NiceMock<MockBeaconCache_t>;
		using MockNiceBeaconCache_sp = std::shared_ptr<MockNiceBeaconCache_t>;
		using MockStrictBeaconCache_t = testing::StrictMock<MockBeaconCache_t>;
		using MockStrictBeaconCache_sp = std::shared_ptr<MockStrictBeaconCache_t>;

		using MockBeaconCacheEvictionStrategy_t = test::MockBeaconCacheEvictionStrategy;
		using MockNiceBeaconCacheEvictionStrategy_t = testing::NiceMock<MockBeaconCacheEvictionStrategy_t>;
		using MockNiceBeaconCacheEvictionStrategy_sp = std::shared_ptr<MockNiceBeaconCacheEvictionStrategy_t>;

		using MockObserver_t = test::MockObserver;
		using MockNiceObserver_t = testing::NiceMock<MockObserver_t>;
		using MockNiceObserver_sp = std::shared_ptr<MockNiceObserver_t>;
	}
}

#endif
