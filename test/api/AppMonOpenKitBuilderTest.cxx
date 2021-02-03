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

#include "OpenKit/AppMonOpenKitBuilder.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <string>

using AppMonOpenKitBuilder_t = openkit::AppMonOpenKitBuilder;

constexpr char ENDPOINT_URL[] = "https://localhost";
constexpr char APPLICATION_NAME[] = "the-application-name";
constexpr int64_t DEVICE_ID = 777;

class AppMonOpenKitBuilderTest : public testing::Test
{
};

TEST_F(AppMonOpenKitBuilderTest, constructorInitializesApplicationName)
{
	// given, when
	AppMonOpenKitBuilder_t target(ENDPOINT_URL, APPLICATION_NAME, DEVICE_ID);

	// then
	ASSERT_THAT(target.getApplicationName(), testing::Eq(APPLICATION_NAME));
}

TEST_F(AppMonOpenKitBuilderTest, constructorInitializesDeviceIdString)
{
	// given, when
	const char* deviceIdAsString = "777";
	AppMonOpenKitBuilder_t target(ENDPOINT_URL, APPLICATION_NAME, deviceIdAsString);

	// then
	ASSERT_THAT(target.getDeviceID(), testing::Eq(777));
	ASSERT_THAT(target.getOrigDeviceID(), testing::Eq(deviceIdAsString));
}

TEST_F(AppMonOpenKitBuilderTest, getApplicationIdGivesSameValueAsApplicationName)
{
	// given, when
	AppMonOpenKitBuilder_t target(ENDPOINT_URL, APPLICATION_NAME, DEVICE_ID);

	// then
	ASSERT_THAT(target.getApplicationID(), testing::Eq(APPLICATION_NAME));
	ASSERT_THAT(target.getApplicationName(), testing::Eq(APPLICATION_NAME));
}

TEST_F(AppMonOpenKitBuilderTest, getOpenKitTypeGivesAppropriateValue)
{
	// given, when
	AppMonOpenKitBuilder_t target(ENDPOINT_URL, APPLICATION_NAME, DEVICE_ID);

	// then
	ASSERT_THAT(target.getOpenKitType(), testing::Eq(AppMonOpenKitBuilder_t::OPENKIT_TYPE));
}

TEST_F(AppMonOpenKitBuilderTest, getDefaultServerIdGivesAppropriateValue)
{
	// given, when
	AppMonOpenKitBuilder_t target(ENDPOINT_URL, APPLICATION_NAME, DEVICE_ID);

	// then
	ASSERT_THAT(target.getDefaultServerID(), testing::Eq(AppMonOpenKitBuilder_t::DEFAULT_SERVER_ID));
}