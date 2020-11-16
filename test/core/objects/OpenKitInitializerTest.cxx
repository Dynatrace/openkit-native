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

#include "core/objects/OpenKitInitializer.h"
#include "core/objects/IOpenKitInitializer.h"

#include "../../api/mock/MockILogger.h"
#include "../../api/mock/MockIOpenKitBuilder.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <memory>

using namespace test;

using MockILogger_sp = std::shared_ptr<MockILogger>;
using MockIOpenKitBuilder_sp = std::shared_ptr<MockIOpenKitBuilder>;

class OpenKitInitializerTest : public testing::Test
{
protected:


	MockILogger_sp mockLogger;
	MockIOpenKitBuilder_sp mockBuilder;

	void SetUp() override
	{
		mockLogger = MockILogger::createNice();
		mockBuilder = MockIOpenKitBuilder::createNice();
		ON_CALL(*mockBuilder, getLogger())
			.WillByDefault(testing::Return(mockLogger));
	}

	std::shared_ptr<core::objects::IOpenKitInitializer> createOpenKitInitializer()
	{
		return std::make_shared<core::objects::OpenKitInitializer>(*mockBuilder);
	}
};

TEST_F(OpenKitInitializerTest, constructorTakesOverLogger)
{
	// given, when
    auto target = createOpenKitInitializer();

    // then
    ASSERT_THAT(target->getLogger(), testing::Eq(mockLogger));
}

TEST_F(OpenKitInitializerTest, constructorInitializesPrivacyConfiguration)
{
    // given, when
    auto target = createOpenKitInitializer();

    // then
    ASSERT_THAT(target->getPrivacyConfiguration(), testing::NotNull());
}

TEST_F(OpenKitInitializerTest, constructorInitializesOpenKitConfiguration)
{
    // given, when
    auto target = createOpenKitInitializer();

    // then
    ASSERT_THAT(target->getOpenKitConfiguration(), testing::NotNull());
}


TEST_F(OpenKitInitializerTest, constructorInitializesTimingProvider)
{
    // given, when
    auto target = createOpenKitInitializer();

    // then
    ASSERT_THAT(target->getTimingProvider(), testing::NotNull());
}

TEST_F(OpenKitInitializerTest, constructorInitializesThreadIdProvider)
{
    // given, when
    auto target = createOpenKitInitializer();

    // then
    ASSERT_THAT(target->getThreadIdProvider(), testing::NotNull());
}

TEST_F(OpenKitInitializerTest, constructorInitializesSessionIdProvider)
{
    // given, when
    auto target = createOpenKitInitializer();

    // then
    ASSERT_THAT(target->getSessionIdProvider(), testing::NotNull());
}

TEST_F(OpenKitInitializerTest, constructorInitializesBeaconCache)
{
    // given, when
    auto target = createOpenKitInitializer();

    // then
    ASSERT_THAT(target->getBeaconCache(), testing::NotNull());
}

TEST_F(OpenKitInitializerTest, constructorInitializesBeaconCacheEvictor)
{
    // given, when
    auto target = createOpenKitInitializer();

    // then
    ASSERT_THAT(target->getBeaconCacheEvictor(), testing::NotNull());
}

TEST_F(OpenKitInitializerTest, constructorInitializesBeaconSender)
{
    // given, when
    auto target = createOpenKitInitializer();

    // then
    ASSERT_THAT(target->getBeaconSender(), testing::NotNull());
}

TEST_F(OpenKitInitializerTest, constructorInitializesSessionWatchdog)
{
    // given, when
    auto target = createOpenKitInitializer();

    // then
    ASSERT_THAT(target->getSessionWatchdog(), testing::NotNull());
}
