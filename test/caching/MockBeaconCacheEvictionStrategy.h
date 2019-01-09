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

#ifndef _TEST_CACHING_MOCKBEACONCACHEEVICTIONSTRATEGY_H
#define _TEST_CACHING_MOCKBEACONCACHEEVICTIONSTRATEGY_H

#include "caching/IBeaconCacheEvictionStrategy.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace caching;
namespace test
{
	class MockBeaconCacheEvictionStrategy : public IBeaconCacheEvictionStrategy
	{
	public:
		MockBeaconCacheEvictionStrategy()
		{
		}

		virtual ~MockBeaconCacheEvictionStrategy() {}

		MOCK_METHOD0(execute, void());
	};
}
#endif
