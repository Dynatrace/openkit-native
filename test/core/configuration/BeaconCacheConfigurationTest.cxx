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

#include "../../api/mock/MockIOpenKitBuilder.h"

#include "core/configuration/BeaconCacheConfiguration.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <memory>

using namespace test;

using BeaconCacheConfiguration_t = core::configuration::BeaconCacheConfiguration;
using MockIOpenKitBuilder_sp = std::shared_ptr<MockIOpenKitBuilder>;

class BeaconCacheConfigurationTest : public testing::Test
{
protected:
	MockIOpenKitBuilder_sp mockBuilder;

	void SetUp() override
	{
		mockBuilder = MockIOpenKitBuilder::createNice();
	}
};

TEST_F(BeaconCacheConfigurationTest, positiveMaxOrderAgeIsTakenOverFromOpenKitBuilder)
{
	// with
	const int64_t maxRecordAge = 73;

	// expect
	EXPECT_CALL(*mockBuilder, getBeaconCacheMaxRecordAge())
		.Times(1)
		.WillOnce(testing::Return(maxRecordAge));

	// given, when
	auto obtained = BeaconCacheConfiguration_t::from(*mockBuilder);

	// then
	ASSERT_THAT(obtained->getMaxRecordAge(), testing::Eq(maxRecordAge));
}

TEST_F(BeaconCacheConfigurationTest, negativeMaxOrderAgeIsTakenOverFromOpenKitBuilder)
{
	// with
	const int64_t maxRecordAge = -73;

	// expect
	EXPECT_CALL(*mockBuilder, getBeaconCacheMaxRecordAge())
		.Times(1)
		.WillOnce(testing::Return(maxRecordAge));

	// given, when
	auto obtained = BeaconCacheConfiguration_t::from(*mockBuilder);

	// then
	ASSERT_THAT(obtained->getMaxRecordAge(), testing::Eq(maxRecordAge));
}

TEST_F(BeaconCacheConfigurationTest, zeroMaxOrderAgeIsTakenOverFromOpenKitBuilder)
{
	// with
	const int64_t maxRecordAge = 0;

	// expect
	EXPECT_CALL(*mockBuilder, getBeaconCacheMaxRecordAge())
		.Times(1)
		.WillOnce(testing::Return(maxRecordAge));

	// given, when
	auto obtained = BeaconCacheConfiguration_t::from(*mockBuilder);

	// then
	ASSERT_THAT(obtained->getMaxRecordAge(), testing::Eq(maxRecordAge));
}

TEST_F(BeaconCacheConfigurationTest, positiveLowerCacheSizeBoundIsTakenOverFromOpenKitBuilder)
{
	// with
	const int64_t lowerBound = 73;

	// expect
	EXPECT_CALL(*mockBuilder, getBeaconCacheLowerMemoryBoundary())
		.Times(1)
		.WillOnce(testing::Return(lowerBound));

	// given, when
	auto obtained = BeaconCacheConfiguration_t::from(*mockBuilder);

	// then
	ASSERT_THAT(obtained->getCacheSizeLowerBound(), testing::Eq(lowerBound));
}

TEST_F(BeaconCacheConfigurationTest, negativeLowerCacheSizeBoundIsTakenOverFromOpenKitBuilder)
{
	// with
	const int64_t lowerBound = -73;

	// expect
	EXPECT_CALL(*mockBuilder, getBeaconCacheLowerMemoryBoundary())
		.Times(1)
		.WillOnce(testing::Return(lowerBound));

	// given, when
	auto obtained = BeaconCacheConfiguration_t::from(*mockBuilder);

	// then
	ASSERT_THAT(obtained->getCacheSizeLowerBound(), testing::Eq(lowerBound));
}

TEST_F(BeaconCacheConfigurationTest, zeroLowerCacheSizeBoundIsTakenOverFromOpenKitBuilder)
{
	// with
	const int64_t lowerBound = -0;

	// expect
	EXPECT_CALL(*mockBuilder, getBeaconCacheLowerMemoryBoundary())
		.Times(1)
		.WillOnce(testing::Return(lowerBound));

	// given, when
	auto obtained = BeaconCacheConfiguration_t::from(*mockBuilder);

	// then
	ASSERT_THAT(obtained->getCacheSizeLowerBound(), testing::Eq(lowerBound));
}

TEST_F(BeaconCacheConfigurationTest, positiveUpperCacheSizeBoundIsTakenOverFromOpenKitBuilder)
{
	// with
	const int64_t upperBound = 73;

	// expect
	EXPECT_CALL(*mockBuilder, getBeaconCacheUpperMemoryBoundary())
		.Times(1)
		.WillOnce(testing::Return(upperBound));

	// given, when
	auto obtained = BeaconCacheConfiguration_t::from(*mockBuilder);

	// then
	ASSERT_THAT(obtained->getCacheSizeUpperBound(), testing::Eq(upperBound));
}

TEST_F(BeaconCacheConfigurationTest, negativeUpperCacheSizeBoundIsTakenOverFromOpenKitBuilder)
{
	// with
	const int64_t upperBound = -73;

	// expect
	EXPECT_CALL(*mockBuilder, getBeaconCacheUpperMemoryBoundary())
		.Times(1)
		.WillOnce(testing::Return(upperBound));

	// given, when
	auto obtained = BeaconCacheConfiguration_t::from(*mockBuilder);

	// then
	ASSERT_THAT(obtained->getCacheSizeUpperBound(), testing::Eq(upperBound));
}

TEST_F(BeaconCacheConfigurationTest, zeroUpperCacheSizeBoundIsTakenOverFromOpenKitBuilder)
{
	// with
	const int64_t upperBound = 0;

	// expect
	EXPECT_CALL(*mockBuilder, getBeaconCacheUpperMemoryBoundary())
		.Times(1)
		.WillOnce(testing::Return(upperBound));

	// given, when
	auto obtained = BeaconCacheConfiguration_t::from(*mockBuilder);

	// then
	ASSERT_THAT(obtained->getCacheSizeUpperBound(), testing::Eq(upperBound));
}