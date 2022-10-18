/**
 * Copyright 2018-2022 Dynatrace LLC
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

#include "core/objects/SupplementaryBasicData.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>

class SupplementaryBasicDataTest : public testing::Test
{
};

TEST_F(SupplementaryBasicDataTest, setNetworkTechnologyAndIsAvailable)
{
	// with
	auto supplementaryBasicData = std::make_shared<core::objects::SupplementaryBasicData>();
	core::UTF8String technology("technology");

	// then
	EXPECT_FALSE(supplementaryBasicData->isNetworkTechnologyAvailable());
	supplementaryBasicData->setNetworkTechnology(technology);
	EXPECT_TRUE(supplementaryBasicData->isNetworkTechnologyAvailable());
	EXPECT_THAT(supplementaryBasicData->getNetworkTechnology(), technology);
};

TEST_F(SupplementaryBasicDataTest, setCarrierAndIsAvailable)
{
	// with
	auto supplementaryBasicData = std::make_shared<core::objects::SupplementaryBasicData>();
	core::UTF8String carrier("carrier");

	// then
	EXPECT_FALSE(supplementaryBasicData->isCarrierAvailable());
	supplementaryBasicData->setCarrier(carrier);
	EXPECT_TRUE(supplementaryBasicData->isCarrierAvailable());
	EXPECT_THAT(supplementaryBasicData->getCarrier(), carrier);
};

TEST_F(SupplementaryBasicDataTest, setConnectionTypeAndIsAvailable)
{
	// with
	auto supplementaryBasicData = std::make_shared<core::objects::SupplementaryBasicData>();

	// then
	EXPECT_FALSE(supplementaryBasicData->isConnectionTypeAvailable());
	EXPECT_THAT(supplementaryBasicData->getConnectionType(), openkit::ConnectionType::UNSET);
	supplementaryBasicData->setConnectionType(openkit::ConnectionType::WIFI);
	EXPECT_TRUE(supplementaryBasicData->isConnectionTypeAvailable());
	EXPECT_THAT(supplementaryBasicData->getConnectionType(), openkit::ConnectionType::WIFI);
};

TEST_F(SupplementaryBasicDataTest, resetNetworkTechnologyAndIsNoLongerAvailable)
{
	// with
	core::UTF8String technology("technology");
	auto supplementaryBasicData = std::make_shared<core::objects::SupplementaryBasicData>();

	// given
	supplementaryBasicData->setNetworkTechnology(technology);
	supplementaryBasicData->resetNetworkTechnology();

	// then
	EXPECT_FALSE(supplementaryBasicData->isNetworkTechnologyAvailable());
}

TEST_F(SupplementaryBasicDataTest, resetCarrierAndIsNoLongerAvailable)
{
	// with
	core::UTF8String carrier("carrier");
	auto supplementaryBasicData = std::make_shared<core::objects::SupplementaryBasicData>();

	// given
	supplementaryBasicData->setCarrier(carrier);
	supplementaryBasicData->resetCarrier();

	// then
	EXPECT_FALSE(supplementaryBasicData->isCarrierAvailable());
}

TEST_F(SupplementaryBasicDataTest, resetConnectionTypeAndIsNoLongerAvailable)
{
	// with
	auto supplementaryBasicData = std::make_shared<core::objects::SupplementaryBasicData>();

	// given
	supplementaryBasicData->setConnectionType(openkit::ConnectionType::MOBILE);
	supplementaryBasicData->resetConnectionType();

	// then
	EXPECT_FALSE(supplementaryBasicData->isConnectionTypeAvailable());
}