/**
 * Copyright 2018-2020 Dynatrace LLC
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

#include "../DefaultValues.h"

#include "OpenKit/DynatraceOpenKitBuilder.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace test;

using DynatraceOpenKitBuilder_t = openkit::DynatraceOpenKitBuilder;

constexpr char ENDPOINT_URL[] = "https://localhost";
constexpr char APPLICATION_ID[] = "the-application-identifier";
constexpr char APPLICATION_NAME[] = "the-application-name";
constexpr int64_t DEVICE_ID = 777;

class DynatraceOpenKitBuilderTest : public testing::Test
{
};

TEST_F(DynatraceOpenKitBuilderTest, constructorInitializesApplicationId)
{
	// given, when
	DynatraceOpenKitBuilder_t target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// then
	ASSERT_THAT(target.getApplicationID(), testing::Eq(APPLICATION_ID));
}

TEST_F(DynatraceOpenKitBuilderTest, constructorInitializesDeviceIdString)
{
	// given, when
	const char* deviceIdAsString = "777";
	DynatraceOpenKitBuilder_t target(ENDPOINT_URL, APPLICATION_ID, deviceIdAsString);

	// then
	ASSERT_THAT(target.getDeviceID(), testing::Eq(777));
	ASSERT_THAT(target.getOrigDeviceID(), testing::Eq(deviceIdAsString));
}

TEST_F(DynatraceOpenKitBuilderTest, getOpenKitTypeGivesAppropriateValue)
{
	// given, when
	DynatraceOpenKitBuilder_t target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// then
	ASSERT_THAT(target.getOpenKitType(), testing::Eq(DynatraceOpenKitBuilder_t::OPENKIT_TYPE));
}

TEST_F(DynatraceOpenKitBuilderTest, getDefaultServerIdGivesAppropriateValue)
{
	// given, when
	DynatraceOpenKitBuilder_t target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// then
	ASSERT_THAT(target.getDefaultServerID(), testing::Eq(DynatraceOpenKitBuilder_t::DEFAULT_SERVER_ID));
}

TEST_F(DynatraceOpenKitBuilderTest, defaultApplicationNameIsEmptyString)
{
	// given
	DynatraceOpenKitBuilder_t target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	auto obtained = target.getApplicationName();

	// then
	ASSERT_THAT(obtained, testing::Eq(DefaultValues::EMPTY_STRING));
}

TEST_F(DynatraceOpenKitBuilderTest, getApplicationNameGivesPreviouslySetApplicationName)
{
	// given
	DynatraceOpenKitBuilder_t target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	target.withApplicationName(APPLICATION_NAME);
	auto obtained = target.getApplicationName();

	// then
	ASSERT_THAT(obtained, testing::Eq(APPLICATION_NAME));
}

TEST_F(DynatraceOpenKitBuilderTest, withApplicationNameIgnoresNullAsArgument)
{
	// given
	DynatraceOpenKitBuilder_t target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	target.withApplicationName(nullptr);

	// then
	ASSERT_THAT(target.getApplicationName(), testing::Eq(DefaultValues::EMPTY_STRING));
}