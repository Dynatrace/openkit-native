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

#include "OpenKit/CrashReportingLevel.h"
#include "OpenKit/DataCollectionLevel.h"
#include "core/configuration/PrivacyConfiguration.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using CrashReportingLevel_t = openkit::CrashReportingLevel;
using DataCollectionLevel_t = openkit::DataCollectionLevel;
using PrivacyConfiguration_t = core::configuration::PrivacyConfiguration;

class PrivacyConfigurationTest : public testing::Test
{
};

TEST_F(PrivacyConfigurationTest, getDataCollectionLevelReturnsLevelPassedInConstructor)
{
	// when, then
	ASSERT_THAT(PrivacyConfiguration_t(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF).getDataCollectionLevel(),
		testing::Eq(DataCollectionLevel_t::OFF));
	ASSERT_THAT(PrivacyConfiguration_t(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF).getDataCollectionLevel(),
		testing::Eq(DataCollectionLevel_t::PERFORMANCE));
	ASSERT_THAT(PrivacyConfiguration_t(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF).getDataCollectionLevel(),
		testing::Eq(DataCollectionLevel_t::USER_BEHAVIOR));
}

TEST_F(PrivacyConfigurationTest, getCrashReportingLevelReturnsLevelPassedInConstructor)
{
	// when, then
	ASSERT_THAT(PrivacyConfiguration_t(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF).getCrashReportingLevel(),
		testing::Eq(CrashReportingLevel_t::OFF));
	ASSERT_THAT(PrivacyConfiguration_t(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OPT_OUT_CRASHES).getCrashReportingLevel(),
		testing::Eq(CrashReportingLevel_t::OPT_OUT_CRASHES));
	ASSERT_THAT(PrivacyConfiguration_t(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OPT_IN_CRASHES).getCrashReportingLevel(),
		testing::Eq(CrashReportingLevel_t::OPT_IN_CRASHES));
}

TEST_F(PrivacyConfigurationTest, sessionNumberReportingIsAllowedIfDataCollectionLevelIsEqualToUserBehavior)
{
	// given, when
	PrivacyConfiguration_t target(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF);

	// then
	ASSERT_THAT(target.isSessionNumberReportingAllowed(), testing::Eq(true));
}

TEST_F(PrivacyConfigurationTest, sessionNumberReportingIsNotAllowedIfDataCollectionLevelIsEqualToUserPerformance)
{
	// given, when
	PrivacyConfiguration_t target(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF);

	// then
	ASSERT_THAT(target.isSessionNumberReportingAllowed(), testing::Eq(false));
}

TEST_F(PrivacyConfigurationTest, sessionNumberReportingIsAllowedIfDataCollectionLevelIsEqualToUserOff)
{
	// given, when
	PrivacyConfiguration_t target(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF);

	// then
	ASSERT_THAT(target.isSessionNumberReportingAllowed(), testing::Eq(false));
}

TEST_F(PrivacyConfigurationTest, deviceIdSendingIsAllowedIfDataCollectionLevelIsEqualToUserBehavior)
{
	// given, when
	PrivacyConfiguration_t target(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF);

	// then
	ASSERT_THAT(target.isDeviceIdSendingAllowed(), testing::Eq(true));
}

TEST_F(PrivacyConfigurationTest, deviceIdSendingIsNotAllowedIfDataCollectionLevelIsEqualToPerformance)
{
	// given, when
	PrivacyConfiguration_t target(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF);

	// then
	ASSERT_THAT(target.isDeviceIdSendingAllowed(), testing::Eq(false));
}

TEST_F(PrivacyConfigurationTest, deviceIdSendingIsNotAllowedIfDataCollectionLevelIsEqualToOff)
{
	// given, when
	PrivacyConfiguration_t target(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF);

	// then
	ASSERT_THAT(target.isDeviceIdSendingAllowed(), testing::Eq(false));
}

TEST_F(PrivacyConfigurationTest, webRequestTracingIsAllowedIfDataCollectionLevelIsEqualToUserBehavior)
{
	// given, when
	PrivacyConfiguration_t target(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF);

	// then
	ASSERT_THAT(target.isWebRequestTracingAllowed(), testing::Eq(true));
}

TEST_F(PrivacyConfigurationTest, webRequestTracingIsAllowedIfDataCollectionLevelIsEqualToPerformance)
{
	// given, when
	PrivacyConfiguration_t target(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF);

	// then
	ASSERT_THAT(target.isWebRequestTracingAllowed(), testing::Eq(true));
}

TEST_F(PrivacyConfigurationTest, webRequestTracingIsNotAllowedIfDataCollectionLevelIsEqualToOff)
{
	// given, when
	PrivacyConfiguration_t target(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF);

	// then
	ASSERT_THAT(target.isWebRequestTracingAllowed(), testing::Eq(false));
}

TEST_F(PrivacyConfigurationTest, sessionReportingIsAllowedIfDataCollectionLevelIsEqualToUserBehavior)
{
	// given, when
	PrivacyConfiguration_t target(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF);

	// then
	ASSERT_THAT(target.isSessionReportingAllowed(), testing::Eq(true));
}

TEST_F(PrivacyConfigurationTest, sessionReportingIsAllowedIfDataCollectionLevelIsEqualToPerformance)
{
	// given, when
	PrivacyConfiguration_t target(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF);

	// then
	ASSERT_THAT(target.isSessionReportingAllowed(), testing::Eq(true));
}

TEST_F(PrivacyConfigurationTest, sessionReportingIsNotAllowedIfDataCollectionLevelIsEqualToOff)
{
	// given, when
	PrivacyConfiguration_t target(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF);

	// then
	ASSERT_THAT(target.isSessionReportingAllowed(), testing::Eq(false));
}

TEST_F(PrivacyConfigurationTest, actionReportingIsAllowedIfDataCollectionLevelIsEqualToUserBehavior)
{
	// given, when
	PrivacyConfiguration_t target(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF);

	// then
	ASSERT_THAT(target.isActionReportingAllowed(), testing::Eq(true));
}

TEST_F(PrivacyConfigurationTest, actionReportingIsAllowedIfDataCollectionLevelIsEqualToPerformance)
{
	// given, when
	PrivacyConfiguration_t target(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF);

	// then
	ASSERT_THAT(target.isActionReportingAllowed(), testing::Eq(true));
}

TEST_F(PrivacyConfigurationTest, actionReportingIsNotAllowedIfDataCollectionLevelIsEqualToOff)
{
	// given, when
	PrivacyConfiguration_t target(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF);

	// then
	ASSERT_THAT(target.isActionReportingAllowed(), testing::Eq(false));
}

TEST_F(PrivacyConfigurationTest, valueReportingIsAllowedIfDataCollectionLevelIsEqualToUserBehavior)
{
	// given, when
	PrivacyConfiguration_t target(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF);

	// then
	ASSERT_THAT(target.isValueReportingAllowed(), testing::Eq(true));
}

TEST_F(PrivacyConfigurationTest, valueReportingIsNotAllowedIfDataCollectionLevelIsEqualToPerformance)
{
	// given, when
	PrivacyConfiguration_t target(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF);

	// then
	ASSERT_THAT(target.isValueReportingAllowed(), testing::Eq(false));
}

TEST_F(PrivacyConfigurationTest, valueReportingIsNotAllowedIfDataCollectionLevelIsEqualToOff)
{
	// given, when
	PrivacyConfiguration_t target(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF);

	// then
	ASSERT_THAT(target.isValueReportingAllowed(), testing::Eq(false));
}

TEST_F(PrivacyConfigurationTest, eventReportingIsAllowedIfDataCollectionLevelIsEqualToUserBehavior)
{
	// given, when
	PrivacyConfiguration_t target(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF);

	// then
	ASSERT_THAT(target.isEventReportingAllowed(), testing::Eq(true));
}

TEST_F(PrivacyConfigurationTest, eventReportingIsNotAllowedIfDataCollectionLevelIsEqualToPerformance)
{
	// given, when
	PrivacyConfiguration_t target(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF);

	// then
	ASSERT_THAT(target.isEventReportingAllowed(), testing::Eq(false));
}

TEST_F(PrivacyConfigurationTest, eventReportingIsNotAllowedIfDataCollectionLevelIsEqualToOff)
{
	// given, when
	PrivacyConfiguration_t target(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF);

	// then
	ASSERT_THAT(target.isEventReportingAllowed(), testing::Eq(false));
}

TEST_F(PrivacyConfigurationTest, errorReportingIsAllowedIfDataCollectionLevelIsEqualToUserBehavior)
{
	// given, when
	PrivacyConfiguration_t target(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF);

	// then
	ASSERT_THAT(target.isErrorReportingAllowed(), testing::Eq(true));
}

TEST_F(PrivacyConfigurationTest, errorReportingIsAllowedIfDataCollectionLevelIsEqualToUserPerformance)
{
	// given, when
	PrivacyConfiguration_t target(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF);

	// then
	ASSERT_THAT(target.isErrorReportingAllowed(), testing::Eq(true));
}

TEST_F(PrivacyConfigurationTest, errorReportingIsNotAllowedIfDataCollectionLevelIsEqualToUserOff)
{
	// given, when
	PrivacyConfiguration_t target(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF);

	// then
	ASSERT_THAT(target.isErrorReportingAllowed(), testing::Eq(false));
}

TEST_F(PrivacyConfigurationTest, crashReportingIsAllowedIfCrashReportingLevelLevelIsEqualToOptInCrashes)
{
	// given, when
	PrivacyConfiguration_t target(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OPT_IN_CRASHES);

	// then
	ASSERT_THAT(target.isCrashReportingAllowed(), testing::Eq(true));
}

TEST_F(PrivacyConfigurationTest, crashReportingIsNotAllowedIfCrashReportingLevelLevelIsEqualToOptOutCrashes)
{
	// given, when
	PrivacyConfiguration_t target(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OPT_OUT_CRASHES);

	// then
	ASSERT_THAT(target.isCrashReportingAllowed(), testing::Eq(false));
}

TEST_F(PrivacyConfigurationTest, crashReportingIsNotAllowedIfCrashReportingLevelLevelIsEqualToOff)
{
	// given, when
	PrivacyConfiguration_t target(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF);

	// then
	ASSERT_THAT(target.isCrashReportingAllowed(), testing::Eq(false));
}

TEST_F(PrivacyConfigurationTest, userIdentificationIsAllowedIfDataCollectionLevelIsEqualToUserBehavior)
{
	// given, when
	PrivacyConfiguration_t target(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF);

	// then
	ASSERT_THAT(target.isUserIdentificationAllowed(), testing::Eq(true));
}

TEST_F(PrivacyConfigurationTest, userIdentificationIsNotAllowedIfDataCollectionLevelIsEqualToPerformance)
{
	// given, when
	PrivacyConfiguration_t target(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF);

	// then
	ASSERT_THAT(target.isUserIdentificationAllowed(), testing::Eq(false));
}

TEST_F(PrivacyConfigurationTest, userIdentificationIsNotAllowedIfDataCollectionLevelIsEqualToOff)
{
	// given, when
	PrivacyConfiguration_t target(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF);

	// then
	ASSERT_THAT(target.isUserIdentificationAllowed(), testing::Eq(false));
}