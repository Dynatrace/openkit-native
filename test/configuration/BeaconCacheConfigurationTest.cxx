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

#include "gtest/gtest.h"

#include "configuration/BeaconCacheConfiguration.h"

using namespace configuration;

class BeaconCacheConfigurationTest : public testing::Test
{
public:
	void SetUp()
	{
	}

	void TearDown()
	{
	}
};

TEST_F(BeaconCacheConfigurationTest, getMaxRecordAge)
{
	// then
	auto config = new BeaconCacheConfiguration(-100L, 1, 2);
	ASSERT_EQ(config->getMaxRecordAge(), -100L);

	config = new BeaconCacheConfiguration(0L, 1, 2);
	ASSERT_EQ(config->getMaxRecordAge(), 0L);

	config = new BeaconCacheConfiguration(200L, 1, 2);
	ASSERT_EQ(config->getMaxRecordAge(), 200L);
}

TEST_F(BeaconCacheConfigurationTest, getCacheSizeLowerBound)
{
	// then
	auto config = new BeaconCacheConfiguration(0L, -1, 2);
	ASSERT_EQ(config->getCacheSizeLowerBound(), -1L);

	config = new BeaconCacheConfiguration(-1L, 0, 2);
	ASSERT_EQ(config->getCacheSizeLowerBound(), 0L);

	config = new BeaconCacheConfiguration(0L, 1, 2);
	ASSERT_EQ(config->getCacheSizeLowerBound(), 1L);
}

TEST_F(BeaconCacheConfigurationTest, getCacheSizeUpperBound)
{
	// then
	auto config = new BeaconCacheConfiguration(0L, -1, -2);
	ASSERT_EQ(config->getCacheSizeUpperBound(), -2L);

	config = new BeaconCacheConfiguration(-1L, 1, 0);
	ASSERT_EQ(config->getCacheSizeUpperBound(), 0L);

	config = new BeaconCacheConfiguration(0L, 1, 2);
	ASSERT_EQ(config->getCacheSizeUpperBound(), 2L);
}