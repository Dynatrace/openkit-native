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

#include "core/configuration/BeaconConfiguration.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"


using BeaconConfiguration_t = core::configuration::BeaconConfiguration;

class BeaconConfigurationTest : public testing::Test
{
};

TEST_F(BeaconConfigurationTest, getMultiplicityReturnsMultiplicitySetInConstructor)
{
	ASSERT_EQ(0, BeaconConfiguration_t(0).getMultiplicity());
	ASSERT_EQ(1, BeaconConfiguration_t(1).getMultiplicity());
	ASSERT_EQ(2, BeaconConfiguration_t(2).getMultiplicity());
	ASSERT_EQ(3, BeaconConfiguration_t(3).getMultiplicity());
	ASSERT_EQ(4, BeaconConfiguration_t(4).getMultiplicity());
}

TEST_F(BeaconConfigurationTest, capturingIsAllowedWhenMultilicityIsGreaterThanZero)
{
	// when, then
	ASSERT_THAT(BeaconConfiguration_t(1).isCapturingAllowed(), testing::Eq(true));
	ASSERT_THAT(BeaconConfiguration_t(2).isCapturingAllowed(), testing::Eq(true));
	ASSERT_THAT(BeaconConfiguration_t(std::numeric_limits<int32_t>::max()).isCapturingAllowed(), testing::Eq(true));
}

TEST_F(BeaconConfigurationTest, capturingIsDisallowedWhenMUltiplicityIsLessThanOrEqualToZero)
{
	// when, then
	ASSERT_THAT(BeaconConfiguration_t(0).isCapturingAllowed(), testing::Eq(false));
	ASSERT_THAT(BeaconConfiguration_t(-1).isCapturingAllowed(), testing::Eq(false));
	ASSERT_THAT(BeaconConfiguration_t(std::numeric_limits<int32_t>::min()).isCapturingAllowed(), testing::Eq(false));
}