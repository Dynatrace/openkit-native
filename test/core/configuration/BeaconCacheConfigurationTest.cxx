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

#include "core/configuration/BeaconCacheConfiguration.h"

#include "gtest/gtest.h"

using BeaconCacheConfiguration_t = core::configuration::BeaconCacheConfiguration;

class BeaconCacheConfigurationTest : public testing::Test
{
};

TEST_F(BeaconCacheConfigurationTest, getMaxRecordAge)
{
	// then
	auto config = BeaconCacheConfiguration_t(-100L, 1, 2);
	ASSERT_EQ(config.getMaxRecordAge(), -100L);

	config = BeaconCacheConfiguration_t(0L, 1, 2);
	ASSERT_EQ(config.getMaxRecordAge(), 0L);

	config = BeaconCacheConfiguration_t(200L, 1, 2);
	ASSERT_EQ(config.getMaxRecordAge(), 200L);
}

TEST_F(BeaconCacheConfigurationTest, getCacheSizeLowerBound)
{
	// then
	auto config = BeaconCacheConfiguration_t(0L, -1, 2);
	ASSERT_EQ(config.getCacheSizeLowerBound(), -1L);

	config = BeaconCacheConfiguration_t(-1L, 0, 2);
	ASSERT_EQ(config.getCacheSizeLowerBound(), 0L);

	config = BeaconCacheConfiguration_t(0L, 1, 2);
	ASSERT_EQ(config.getCacheSizeLowerBound(), 1L);
}

TEST_F(BeaconCacheConfigurationTest, getCacheSizeUpperBound)
{
	// then
	auto config = BeaconCacheConfiguration_t(0L, -1, -2);
	ASSERT_EQ(config.getCacheSizeUpperBound(), -2L);

	config = BeaconCacheConfiguration_t(-1L, 1, 0);
	ASSERT_EQ(config.getCacheSizeUpperBound(), 0L);

	config = BeaconCacheConfiguration_t(0L, 1, 2);
	ASSERT_EQ(config.getCacheSizeUpperBound(), 2L);
}