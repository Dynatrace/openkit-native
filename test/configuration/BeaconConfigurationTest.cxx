/**
* Copyright 2018 Dynatrace LLC
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
#include "gmock/gmock.h"

#include "configuration/BeaconConfiguration.h"

using namespace configuration;

class BeaconConfigurationTest : public testing::Test
{

};

TEST_F(BeaconConfigurationTest, getDataCollectionLevel)
{
	ASSERT_EQ(openkit::DataCollectionLevel::OFF,
		BeaconConfiguration(openkit::DataCollectionLevel::OFF, openkit::CrashReportingLevel::OFF).getDataCollectionLevel());
	ASSERT_EQ(openkit::DataCollectionLevel::PERFORMANCE,
		BeaconConfiguration(openkit::DataCollectionLevel::PERFORMANCE, openkit::CrashReportingLevel::OFF).getDataCollectionLevel());
	ASSERT_EQ(openkit::DataCollectionLevel::USER_BEHAVIOR,
		BeaconConfiguration(openkit::DataCollectionLevel::USER_BEHAVIOR, openkit::CrashReportingLevel::OFF).getDataCollectionLevel());
}

TEST_F(BeaconConfigurationTest, getCrashReportingLevel)
{
	ASSERT_EQ(openkit::CrashReportingLevel::OFF,
		BeaconConfiguration(openkit::DataCollectionLevel::OFF, openkit::CrashReportingLevel::OFF).getCrashReportingLevel());
	ASSERT_EQ(openkit::CrashReportingLevel::OPT_OUT_CRASHES,
		BeaconConfiguration(openkit::DataCollectionLevel::OFF, openkit::CrashReportingLevel::OPT_OUT_CRASHES).getCrashReportingLevel());
	ASSERT_EQ(openkit::CrashReportingLevel::OPT_IN_CRASHES,
		BeaconConfiguration(openkit::DataCollectionLevel::OFF, openkit::CrashReportingLevel::OPT_IN_CRASHES).getCrashReportingLevel());
}
