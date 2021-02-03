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

#include "core/objects/SessionCreator.h"

#include "mock/MockISessionCreatorInput.h"
#include "mock/MockIOpenKitComposite.h"
#include "../caching/mock/MockIBeaconCache.h"
#include "../configuration/mock/MockIOpenKitConfiguration.h"
#include "../configuration/mock/MockIPrivacyConfiguration.h"
#include "../../api/mock/MockILogger.h"
#include "../../providers/mock/MockISessionIDProvider.h"
#include "../../providers/mock/MockIThreadIDProvider.h"
#include "../../providers/mock/MockITimingProvider.h"

#include <cstdint>
#include <memory>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace test;

using MockISessionCreatorInput_sp = std::shared_ptr<MockISessionCreatorInput>;
using MockILogger_sp = std::shared_ptr<MockILogger>;
using MockIOpenKitConfiguration_sp = std::shared_ptr<MockIOpenKitConfiguration>;
using MockIPrivacyConfiguration_sp = std::shared_ptr<MockIPrivacyConfiguration>;
using MockIBeaconCache_sp = std::shared_ptr<MockIBeaconCache>;
using MockISessionIDProvider_sp = std::shared_ptr<MockISessionIDProvider>;
using MockIThreadIDProvider_sp = std::shared_ptr<MockIThreadIDProvider>;
using MockITimingProvider_sp = std::shared_ptr<MockITimingProvider>;
using MockIThreadIDProvider_sp = std::shared_ptr<MockIThreadIDProvider>;
using MockITimingProvider_sp = std::shared_ptr<MockITimingProvider>;
using MockIOpenKitComposite_sp = std::shared_ptr<MockIOpenKitComposite>;
using SessionCreator_t = core::objects::SessionCreator;
using SessionCreator_up = std::unique_ptr<SessionCreator_t>;
using ISessionCreator_up = std::unique_ptr<core::objects::ISessionCreator>;
using SessionCreator_sp = std::shared_ptr<SessionCreator_t>;
using ISessionCreator_sp = std::shared_ptr<core::objects::ISessionCreator>;

class SessionCreatorTest : public testing::Test
{
protected:

	MockISessionCreatorInput_sp mockInput;
	MockILogger_sp mockLogger;
    MockIOpenKitConfiguration_sp mockOpenKitConfiguration;
    MockIPrivacyConfiguration_sp mockPrivacyConfiguration;
    MockIBeaconCache_sp mockBeaconCache;
    MockISessionIDProvider_sp mockSessionIdProvider;
    MockIThreadIDProvider_sp mockThreadIdProvider;
    MockITimingProvider_sp mockTimingProvider;
    MockIOpenKitComposite_sp mockParent;

    static constexpr int32_t ServerId = 999;
    static constexpr int32_t SessionId = 777;
    static constexpr int64_t DeviceId = 1;
    static constexpr const char* IpAddress = "127.0.0.1";

	void SetUp() override
	{
		mockLogger = MockILogger::createNice();
        
        mockOpenKitConfiguration = MockIOpenKitConfiguration::createNice();
        ON_CALL(*mockOpenKitConfiguration, getDeviceId())
            .WillByDefault(testing::Return(DeviceId));

        mockPrivacyConfiguration = MockIPrivacyConfiguration::createNice();
        mockBeaconCache = MockIBeaconCache::createNice();
        mockSessionIdProvider = MockISessionIDProvider::createNice();
        mockThreadIdProvider = MockIThreadIDProvider::createNice();
        mockTimingProvider = MockITimingProvider::createNice();
        mockParent = MockIOpenKitComposite::createNice();

        mockInput = MockISessionCreatorInput::createNice();
        ON_CALL(*mockInput, getLogger())
            .WillByDefault(testing::Return(mockLogger));
        ON_CALL(*mockInput, getOpenKitConfiguration())
            .WillByDefault(testing::Return(mockOpenKitConfiguration));
        ON_CALL(*mockInput, getPrivacyConfiguration())
            .WillByDefault(testing::Return(mockPrivacyConfiguration));
        ON_CALL(*mockInput, getBeaconCache())
            .WillByDefault(testing::Return(mockBeaconCache));
        ON_CALL(*mockInput, getSessionIdProvider())
            .WillByDefault(testing::Return(mockSessionIdProvider));
        ON_CALL(*mockInput, getThreadIdProvider())
            .WillByDefault(testing::Return(mockThreadIdProvider));
        ON_CALL(*mockInput, getTimingProvider())
            .WillByDefault(testing::Return(mockTimingProvider));
        ON_CALL(*mockInput, getCurrentServerId())
            .WillByDefault(testing::Return(ServerId));
	}
};

constexpr const char* SessionCreatorTest::IpAddress; // make linker happy

TEST_F(SessionCreatorTest, constructorTakesOverLogger)
{
    // expect
    EXPECT_CALL(*mockInput, getLogger())
        .Times(1);

    // given
    SessionCreator_t target(*mockInput, IpAddress);
}

TEST_F(SessionCreatorTest, constructorTakesOverOpenKitConfiguration)
{
    // expect
    EXPECT_CALL(*mockInput, getOpenKitConfiguration())
        .Times(1);

    // given
    SessionCreator_t target(*mockInput, IpAddress);
}

TEST_F(SessionCreatorTest, constructorTakesOverPrivacyConfiguration)
{
    // expect
    EXPECT_CALL(*mockInput, getPrivacyConfiguration())
        .Times(1);

    // given
    SessionCreator_t target(*mockInput, IpAddress);
}

TEST_F(SessionCreatorTest, constructorTakesOverBeaconCache)
{
    // expect
    EXPECT_CALL(*mockInput, getBeaconCache())
        .Times(1);

    // given
    SessionCreator_t target(*mockInput, IpAddress);
}

TEST_F(SessionCreatorTest, constructorTakesOverThreadIdProvider)
{
    // expect
    EXPECT_CALL(*mockInput, getThreadIdProvider())
        .Times(1);

    // given
    SessionCreator_t target(*mockInput, IpAddress);
}

TEST_F(SessionCreatorTest, constructorTakesOverTimingProvider)
{
    // expect
    EXPECT_CALL(*mockInput, getTimingProvider())
        .Times(1);

    // given
    SessionCreator_t target(*mockInput, IpAddress);
}

TEST_F(SessionCreatorTest, constructorTakesOverServerId)
{
    // expect
    EXPECT_CALL(*mockInput, getCurrentServerId())
        .Times(1);

    // given
    SessionCreator_t target(*mockInput, IpAddress);
}

TEST_F(SessionCreatorTest, constructorDrawsNextSessionId)
{
    // expect
    EXPECT_CALL(*mockSessionIdProvider, getNextSessionID())
        .Times(1);

    // given
    SessionCreator_t target(*mockInput, IpAddress);
}

TEST_F(SessionCreatorTest, createSessionReturnsNewSessionInstance)
{
    // given
    ISessionCreator_up target(new SessionCreator_t(*mockInput, IpAddress));

    // when
    auto obtained = target->createSession(mockParent);

    // then
    ASSERT_THAT(obtained, testing::NotNull());
}

TEST_F(SessionCreatorTest, createSessionGivesSessionsWithAlwaysSameSessionNumber)
{
    // expect
    EXPECT_CALL(*mockSessionIdProvider, getNextSessionID())
        .Times(1)
        .WillOnce(testing::Return(SessionId));

    // given
    ON_CALL(*mockPrivacyConfiguration, isDeviceIdSendingAllowed())
        .WillByDefault(testing::Return(true));

    ISessionCreator_up target(new SessionCreator_t(*mockInput, IpAddress));

    // when
    auto obtainedOne = target->createSession(mockParent);
    auto obtainedTwo = target->createSession(mockParent);
    auto obtainedThree = target->createSession(mockParent);

    // then
    ASSERT_THAT(obtainedOne->getBeacon()->getSessionNumber(), testing::Eq(SessionId));
    ASSERT_THAT(obtainedTwo->getBeacon()->getSessionNumber(), testing::Eq(SessionId));
    ASSERT_THAT(obtainedThree->getBeacon()->getSessionNumber(), testing::Eq(SessionId));
}

TEST_F(SessionCreatorTest, createSessionGivesSessionsWithSameRandomizedDeviceId)
{
    // given
    ON_CALL(*mockPrivacyConfiguration, isDeviceIdSendingAllowed())
        .WillByDefault(testing::Return(false));

    SessionCreator_up sessionCreator(new SessionCreator_t(*mockInput, IpAddress));
    auto randomizedDeviceId = sessionCreator->getRandomNumberGenerator()->nextPositiveInt64();

    ISessionCreator_up target(sessionCreator.release());

    // when
    auto obtainedOne = target->createSession(mockParent);
    auto obtainedTwo = target->createSession(mockParent);
    auto obtainedThree = target->createSession(mockParent);

    // then
    ASSERT_THAT(obtainedOne->getBeacon()->getDeviceID(), testing::Eq(randomizedDeviceId));
    ASSERT_THAT(obtainedTwo->getBeacon()->getDeviceID(), testing::Eq(randomizedDeviceId));
    ASSERT_THAT(obtainedThree->getBeacon()->getDeviceID(), testing::Eq(randomizedDeviceId));
}

TEST_F(SessionCreatorTest, createSessionIncreasesSessionSequenceNumber)
{
    // given
    SessionCreator_sp sessionCreator = std::make_shared<SessionCreator_t>(*mockInput, IpAddress);
    ISessionCreator_sp target = sessionCreator;

    ASSERT_THAT(sessionCreator->getSessionSequenceNumber(), testing::Eq(0));

    // when
    target->createSession(mockParent);

    // then
    ASSERT_THAT(sessionCreator->getSessionSequenceNumber(), testing::Eq(1));

    // and when
    target->createSession(mockParent);

    // then
    ASSERT_THAT(sessionCreator->getSessionSequenceNumber(), testing::Eq(2));
}

TEST_F(SessionCreatorTest, resetResetsSessionSequenceNumber)
{
    // given
    SessionCreator_sp sessionCreator = std::make_shared<SessionCreator_t>(*mockInput, IpAddress);
    ISessionCreator_sp target = sessionCreator;

    for (auto i = 0; i < 5; i++)
    {
        // when
        auto session = target->createSession(mockParent);

        // then
        ASSERT_THAT(session->getBeacon()->getSessionSequenceNumber(), testing::Eq(i));
    }

    // and when
    target->reset();

    // then
    ASSERT_THAT(sessionCreator->getSessionSequenceNumber(), testing::Eq(0));
}

TEST_F(SessionCreatorTest, resetMakesFixedSessionIdProviderToUseNextSessionId)
{
    // with
    const int32_t sessionIdBeforeReset = 17;
    const int32_t sessionIdAfterReset = 42;

    // expect
    EXPECT_CALL(*mockSessionIdProvider, getNextSessionID())
        .Times(2)
        .WillOnce(testing::Return(sessionIdBeforeReset))
        .WillOnce(testing::Return(sessionIdAfterReset));

    // given
    ON_CALL(*mockPrivacyConfiguration, isSessionNumberReportingAllowed())
        .WillByDefault(testing::Return(true));

    ISessionCreator_sp target = std::make_shared<SessionCreator_t>(*mockInput, IpAddress);

    // when
    for (auto i = 0; i < 5; i++)
    {
        auto session = target->createSession(mockParent);
        auto beacon = session->getBeacon();

        // then
        ASSERT_THAT(beacon->getSessionNumber(), testing::Eq(sessionIdBeforeReset));
        ASSERT_THAT(beacon->getSessionSequenceNumber(), testing::Eq(i));
    }

    // and when
    target->reset();

    // then
    for (auto i = 0; i < 5; i++)
    {
        auto session = target->createSession(mockParent);
        auto beacon = session->getBeacon();

        // then
        ASSERT_THAT(beacon->getSessionNumber(), testing::Eq(sessionIdAfterReset));
        ASSERT_THAT(beacon->getSessionSequenceNumber(), testing::Eq(i));
    }
}

TEST_F(SessionCreatorTest, resetMakesFixedRandomNumberGeneratorToUseNextRandomNumber)
{
    // given
    SessionCreator_sp target = std::make_shared<SessionCreator_t>(*mockInput, IpAddress);
    ISessionCreator_sp targetExplicit = target;
    auto randomNumberBeforeReset = target->getRandomNumberGenerator()->nextPositiveInt64();

    // when
    for (auto i = 0; i < 5; i++)
    {
        // then
        ASSERT_THAT(target->getRandomNumberGenerator()->nextPositiveInt64(), testing::Eq(randomNumberBeforeReset));
    }

    // and when
    targetExplicit->reset();
    auto randomNumberAfterReset = target->getRandomNumberGenerator()->nextPositiveInt64();

    // then
    ASSERT_THAT(randomNumberBeforeReset, testing::Ne(randomNumberAfterReset));
    for (auto i = 0; i < 5; i++)
    {
        ASSERT_THAT(target->getRandomNumberGenerator()->nextPositiveInt64(), testing::Eq(randomNumberAfterReset));
    }
}