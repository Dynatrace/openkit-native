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

#include "Types.h"
#include "../../api/Types.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace test::types;

class BeaconConfigurationTest : public testing::Test
{
};

TEST_F(BeaconConfigurationTest, getDataCollectionLevel)
{
	ASSERT_EQ(DataCollectionLevel_t::OFF,
		BeaconConfiguration_t(1, DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF).getDataCollectionLevel());
	ASSERT_EQ(DataCollectionLevel_t::PERFORMANCE,
		BeaconConfiguration_t(1, DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF).getDataCollectionLevel());
	ASSERT_EQ(DataCollectionLevel_t::USER_BEHAVIOR,
		BeaconConfiguration_t(1, DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF).getDataCollectionLevel());
}

TEST_F(BeaconConfigurationTest, getCrashReportingLevel)
{
	ASSERT_EQ(CrashReportingLevel_t::OFF,
		BeaconConfiguration_t(1, DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF).getCrashReportingLevel());
	ASSERT_EQ(CrashReportingLevel_t::OPT_OUT_CRASHES,
		BeaconConfiguration_t(1, DataCollectionLevel_t::OFF, CrashReportingLevel_t::OPT_OUT_CRASHES).getCrashReportingLevel());
	ASSERT_EQ(CrashReportingLevel_t::OPT_IN_CRASHES,
		BeaconConfiguration_t(1, DataCollectionLevel_t::OFF, CrashReportingLevel_t::OPT_IN_CRASHES).getCrashReportingLevel());
}

TEST_F(BeaconConfigurationTest, getMultiplicity)
{
	ASSERT_EQ(0,
		BeaconConfiguration_t(0, DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF).getMultiplicity());
	ASSERT_EQ(1,
		BeaconConfiguration_t(1, DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF).getMultiplicity());
	ASSERT_EQ(2,
		BeaconConfiguration_t(2, DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF).getMultiplicity());
	ASSERT_EQ(3,
		BeaconConfiguration_t(3, DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF).getMultiplicity());
	ASSERT_EQ(4,
		BeaconConfiguration_t(4, DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF).getMultiplicity());
}