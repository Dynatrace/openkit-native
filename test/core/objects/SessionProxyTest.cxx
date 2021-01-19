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

#include "core/objects/SessionProxy.h"
#include "core/objects/NullRootAction.h"
#include "core/objects/NullWebRequestTracer.h"
#include "core/configuration/ServerConfiguration.h"
#include "protocol/ResponseAttributes.h"

#include "mock/MockISessionCreator.h"
#include "mock/MockIOpenKitComposite.h"
#include "mock/MockIOpenKitObject.h"
#include "mock/MockSessionInternals.h"
#include "../configuration/mock/MockIServerConfiguration.h"
#include "../mock/MockIBeaconSender.h"
#include "../mock/MockISessionWatchdog.h"
#include "../../api/mock/MockILogger.h"
#include "../../protocol/mock/MockIBeacon.h"
#include "../../providers/mock/MockITimingProvider.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <memory>
#include <array>

using namespace test;

using SessionProxy_t = core::objects::SessionProxy;
using SessionProxy_sp = std::shared_ptr<SessionProxy_t>;
using SessionInternals_t = core::objects::SessionInternals;
using SessionInternals_sp = std::shared_ptr<SessionInternals_t>;
using IOpenKitComposite_sp = std::shared_ptr<core::objects::IOpenKitComposite>;
using NullRootAction_t = core::objects::NullRootAction;
using NullWebRequestTracer_t = core::objects::NullWebRequestTracer;
using ServerConfiguration_t = core::configuration::ServerConfiguration;
using ResponseAttributes_t = protocol::ResponseAttributes;
using MockILogger_sp = std::shared_ptr<MockILogger>;
using MockIOpenKitComposite_sp = std::shared_ptr<MockIOpenKitComposite>;
using MockSessionInternals_sp = std::shared_ptr<MockSessionInternals>;
using MockIBeacon_sp = std::shared_ptr<MockIBeacon>;
using MockISessionCreator_sp = std::shared_ptr<MockISessionCreator>;
using MockIServerConfiguration_sp = std::shared_ptr<MockIServerConfiguration>;
using MockIBeaconSender_sp = std::shared_ptr<MockIBeaconSender>;
using MockISessionWatchdog_sp = std::shared_ptr<MockISessionWatchdog>;
using MockITimginProvider_sp = std::shared_ptr<MockITimingProvider>;


class SessionProxyTest : public testing::Test
{
protected:

    MockILogger_sp mockLogger;
    MockIOpenKitComposite_sp mockParent;
    MockSessionInternals_sp mockSession;
    MockIBeacon_sp mockBeacon;
    MockSessionInternals_sp mockSplitSession1;
    MockIBeacon_sp mockSplitBeacon1;
    MockSessionInternals_sp mockSplitSession2;
    MockIBeacon_sp mockSplitBeacon2;
    MockISessionCreator_sp mockSessionCreator;
    MockIServerConfiguration_sp mockServerConfiguration;
    MockIBeaconSender_sp mockBeaconSender;
    MockISessionWatchdog_sp mockSessionWatchdog;
    MockITimginProvider_sp mockTimingProvider;

    void SetUp() override
    {
        mockLogger = MockILogger::createNice();
        ON_CALL(*mockLogger, isDebugEnabled())
            .WillByDefault(testing::Return(true));
        ON_CALL(*mockLogger, isWarningEnabled())
            .WillByDefault(testing::Return(true));

        mockParent = MockIOpenKitComposite::createNice();
        mockServerConfiguration = MockIServerConfiguration::createNice();

        mockBeacon = MockIBeacon::createNice();
        ON_CALL(*mockBeacon, isActionReportingAllowedByPrivacySettings())
            .WillByDefault(testing::Return(true));
        mockSession = MockSessionInternals::createNice();
        ON_CALL(*mockSession, getBeacon())
            .WillByDefault(testing::Return(mockBeacon));

        mockSplitBeacon1 = MockIBeacon::createNice();
        ON_CALL(*mockSplitBeacon1, isActionReportingAllowedByPrivacySettings())
            .WillByDefault(testing::Return(true));
        mockSplitSession1 = MockSessionInternals::createNice();
        ON_CALL(*mockSplitSession1, getBeacon())
            .WillByDefault(testing::Return(mockSplitBeacon1));

        mockSplitBeacon2 = MockIBeacon::createNice();
        ON_CALL(*mockSplitBeacon2, isActionReportingAllowedByPrivacySettings())
            .WillByDefault(testing::Return(true));
        mockSplitSession2 = MockSessionInternals::createNice();
        ON_CALL(*mockSplitSession2, getBeacon())
            .WillByDefault(testing::Return(mockSplitBeacon2));

        mockSessionCreator = MockISessionCreator::createNice();
        ON_CALL(*mockSessionCreator, createSession(testing::_))
            .WillByDefault(testing::Return(mockSession));

        mockBeaconSender = MockIBeaconSender::createNice();
        mockSessionWatchdog = MockISessionWatchdog::createNice();

        mockTimingProvider = MockITimingProvider::createNice();
    }

    SessionProxy_sp createSessionProxy()
    {
        return SessionProxy_t::createSessionProxy(mockLogger, mockParent, mockSessionCreator, mockTimingProvider, mockBeaconSender, mockSessionWatchdog);
    }
};

TEST_F(SessionProxyTest, constructingASessionProxyCreatesASessionInitially)
{
    // expect
    EXPECT_CALL(*mockSessionCreator, createSession(testing::_))
        .Times(1)
        .WillOnce(testing::Return(mockSession));

    // given, when
    auto target = createSessionProxy();
}

TEST_F(SessionProxyTest, initiallyCreatedSessionIsInitializedWithServerConfiguration)
{
    // with
    auto initialServerConfig = MockIServerConfiguration::createNice();

    // expect
    EXPECT_CALL(*mockSessionCreator, createSession(testing::_))
        .Times(1);
    EXPECT_CALL(*mockSession, initializeServerConfiguration(testing::Eq(initialServerConfig)))
        .Times(1);

    // given
    ON_CALL(*mockBeaconSender, getLastServerConfiguration())
        .WillByDefault(testing::Return(initialServerConfig));
    
    // when
    auto target = createSessionProxy();
}

TEST_F(SessionProxyTest, aNewlyCreatedSessionProxyIsNotFinished)
{
    // given
    auto target = createSessionProxy();

    // when
    auto obtained = target->isFinished();

    // then
    ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(SessionProxyTest, initiallyCreatedSessionRegistersServerConfigurationUpdateCallback)
{
    // expect
    EXPECT_CALL(*mockBeacon, setServerConfigurationUpdateCallback(testing::_))
        .Times(1);
    EXPECT_CALL(*mockSessionCreator, createSession(testing::_))
        .Times(1)
        .WillOnce(testing::Return(mockSession));

    // given, when
    auto target = createSessionProxy();
}

TEST_F(SessionProxyTest, initiallyCreatedSessionIsAddedToTheBeaconSender)
{
    // expect
    EXPECT_CALL(*mockBeaconSender, addSession(testing::_))
        .Times(1);

    // given, when
    auto target = createSessionProxy();
}

TEST_F(SessionProxyTest, initiallyCreatedSessionProvidesStartTimeAsLastInteractionTime)
{
    // given
    const int64_t startTime = 73;
    ON_CALL(*mockBeacon, getSessionStartTime())
        .WillByDefault(testing::Return(startTime));

    // when
    auto target = createSessionProxy();

    // then
    ASSERT_THAT(target->getLastInteractionTime(), testing::Eq(startTime));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// enter action tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


TEST_F(SessionProxyTest, enterActionWithNullActionNameGivesNullRootActionObject)
{
    // expect
    EXPECT_CALL(*mockLogger, mockWarning("SessionProxy [sn=0, seq=0] enterAction: actionName must not be null or empty"))
        .Times(1);

    // given
    auto target = createSessionProxy();

    // when
    auto obtained = target->enterAction(nullptr);

    // then
    ASSERT_THAT(obtained, testing::NotNull());
    ASSERT_THAT(std::dynamic_pointer_cast<NullRootAction_t>(obtained), testing::NotNull());
}

TEST_F(SessionProxyTest, enterActionWithEmptyActionNameGivesNullRootActionObject)
{
    // expect
    EXPECT_CALL(*mockLogger, mockWarning("SessionProxy [sn=0, seq=0] enterAction: actionName must not be null or empty"))
        .Times(1);

    // given
    auto target = createSessionProxy();

    // when
    auto obtained = target->enterAction("");

    // then
    ASSERT_THAT(obtained, testing::NotNull());
    ASSERT_THAT(std::dynamic_pointer_cast<NullRootAction_t>(obtained), testing::NotNull());
}

TEST_F(SessionProxyTest, enterActionDelegatesToRealSession)
{
    // with
    auto actionName = "some action";

    // expect
    EXPECT_CALL(*mockSession, enterAction(testing::Eq(actionName)))
        .Times(1);

    // given
    auto target = createSessionProxy();

    // when
    target->enterAction(actionName);
}

TEST_F(SessionProxyTest, enterActionLogsInvocation)
{
    // with
    auto actionName = "Some action";

    // expect
    EXPECT_CALL(*mockLogger, isDebugEnabled())
        .Times(1);
    EXPECT_CALL(*mockLogger, mockDebug(std::string("SessionProxy [sn=0, seq=0] enterAction(") + actionName + ")"))
        .Times(1);

    // given
    auto target = createSessionProxy();

    // when
    target->enterAction(actionName);
}

TEST_F(SessionProxyTest, enterActionGivesNullRootActionIfSessionIsAlreadyEnded)
{
    // given
    auto target = createSessionProxy();
    target->end();

    // when
    auto obtained = target->enterAction("Test");

    // then
    ASSERT_THAT(obtained, testing::NotNull());
    ASSERT_THAT(std::dynamic_pointer_cast<NullRootAction_t>(obtained), testing::NotNull());
}

TEST_F(SessionProxyTest, enterActionIncreasesTopLevelActionCount)
{
    // given
    auto target = createSessionProxy();
    ASSERT_THAT(target->getTopLevelActionCount(), testing::Eq(0));

    // when
    auto obtained = target->enterAction("test");

    // then
    ASSERT_THAT(target->getTopLevelActionCount(), testing::Eq(1));
}

TEST_F(SessionProxyTest, enterActionSetsLastInterActionTime)
{
    // given
    const int64_t sessionCreationTime = 13;
    const int64_t lastInteractionTime = 17;
    ON_CALL(*mockBeacon, getSessionStartTime())
        .WillByDefault(testing::Return(sessionCreationTime));
    ON_CALL(*mockTimingProvider, provideTimestampInMilliseconds())
        .WillByDefault(testing::Return(lastInteractionTime));
    
    auto target = createSessionProxy();
    ASSERT_THAT(target->getLastInteractionTime(), testing::Eq(sessionCreationTime));

    // when
    target->enterAction("test");

    // then
    ASSERT_THAT(target->getLastInteractionTime(), testing::Eq(lastInteractionTime));
}

TEST_F(SessionProxyTest, enterActionDoesNotSplitSessionIfNoServerConfigurationIsSet)
{
    // expect
    EXPECT_CALL(*mockSessionCreator, createSession(testing::_))
        .Times(1);

    // given
    const int32_t eventCount = 10;

    auto target = createSessionProxy();

    // when
    for (auto i = 0; i < eventCount; i++)
    {
        target->enterAction("some action");
    }

    // then
    ASSERT_THAT(target->getTopLevelActionCount(), testing::Eq(eventCount));
}

TEST_F(SessionProxyTest, enterActionDoesNotSplitSessionIfSessionSplitByEventDisabled)
{
    // expect
    EXPECT_CALL(*mockSessionCreator, createSession(testing::_))
        .Times(1);

    // given
    const int32_t eventCount = 10;

    ON_CALL(*mockServerConfiguration, isSessionSplitByEventsEnabled())
        .WillByDefault(testing::Return(false));
    ON_CALL(*mockServerConfiguration, getMaxEventsPerSession())
        .WillByDefault(testing::Return(1));

    auto target = createSessionProxy();
    target->onServerConfigurationUpdate(mockServerConfiguration);

    // when
    for (auto i = 0; i < eventCount; i++)
    {
        target->enterAction("some action");
    }

    // then
    ASSERT_THAT(target->getTopLevelActionCount(), testing::Eq(eventCount));
}

TEST_F(SessionProxyTest, enterActionSplitsSessionIfSessionSplitByEventsEnabled)
{
    // expect
    EXPECT_CALL(*mockSessionCreator, createSession(testing::_))
        .Times(2)
        .WillOnce(testing::Return(mockSession))
        .WillOnce(testing::Return(mockSplitSession1));

    // given
    ON_CALL(*mockServerConfiguration, isSessionSplitByEventsEnabled())
        .WillByDefault(testing::Return(true));
    ON_CALL(*mockServerConfiguration, getMaxEventsPerSession())
        .WillByDefault(testing::Return(1));

    auto target = createSessionProxy();
    target->onServerConfigurationUpdate(mockServerConfiguration);

    // when
    target->enterAction("some action");

    // and when
    target->enterAction("some other action");
}

TEST_F(SessionProxyTest, enterActionSplitsSessionEveryNthEvent)
{
    // expect
    EXPECT_CALL(*mockSessionCreator, createSession(testing::_))
        .Times(3)
        .WillOnce(testing::Return(mockSession))
        .WillOnce(testing::Return(mockSplitSession1))
        .WillOnce(testing::Return(mockSplitSession2));

    // given
    const int32_t maxEventCount = 3;
    ON_CALL(*mockServerConfiguration, isSessionSplitByEventsEnabled())
        .WillByDefault(testing::Return(true));
    ON_CALL(*mockServerConfiguration, getMaxEventsPerSession())
        .WillByDefault(testing::Return(maxEventCount));

    auto target = createSessionProxy();
    target->onServerConfigurationUpdate(mockServerConfiguration);

    // when
    const int32_t numberOfActions = maxEventCount * 2 + 1;
    for (auto i = 0; i < numberOfActions; i++)
    {
        auto actionName = std::string("action ") + std::to_string(i + 1);
        target->enterAction(actionName.c_str());
    }
}

TEST_F(SessionProxyTest, EnterActionSplitsSessionEveryNthEventFromFirstServerConfiguration)
{
    // expect
    EXPECT_CALL(*mockSessionCreator, createSession(testing::_))
        .Times(3)
        .WillOnce(testing::Return(mockSession))
        .WillOnce(testing::Return(mockSplitSession1))
        .WillOnce(testing::Return(mockSplitSession2));

    // given
    const int32_t maxEventCount = 3;
    auto serverConfig = ServerConfiguration_t::from(
        ResponseAttributes_t::withUndefinedDefaults().withMaxEventsPerSession(maxEventCount).build()
    );

    ASSERT_THAT(serverConfig->isSessionSplitByEventsEnabled(), testing::Eq(true));

    auto target = createSessionProxy();
    target->onServerConfigurationUpdate(serverConfig);

    auto ignoredServerConfig = MockIServerConfiguration::createNice();
    ON_CALL(*ignoredServerConfig, isSessionSplitByEventsEnabled())
        .WillByDefault(testing::Return(true));
    ON_CALL(*ignoredServerConfig, getMaxEventsPerSession())
        .WillByDefault(testing::Return(5));
    target->onServerConfigurationUpdate(ignoredServerConfig);

    // when
    const int32_t numberOfActions = maxEventCount * 2 + 1;
    for (auto i = 0; i < numberOfActions; i++)
    {
        auto actionName = std::string("action ") + std::to_string(i + 1);
        target->enterAction(actionName.c_str());
    }
}

TEST_F(SessionProxyTest, enterActionCallsWatchdogToCloseOldSessionOnSplitByEvents)
{
    // with
    const int32_t sessionIdleTimeout = 10;

    // expect
    EXPECT_CALL(*mockSessionCreator, createSession(testing::_))
        .Times(2)
        .WillOnce(testing::Return(mockSession))
        .WillOnce(testing::Return(mockSplitSession1));
    EXPECT_CALL(*mockSessionWatchdog, closeOrEnqueueForClosing(testing::Eq(mockSession), sessionIdleTimeout / 2))
        .Times(1);

    // given
    ON_CALL(*mockServerConfiguration, isSessionSplitByEventsEnabled())
        .WillByDefault(testing::Return(true));
    ON_CALL(*mockServerConfiguration, getMaxEventsPerSession())
        .WillByDefault(testing::Return(1));
    ON_CALL(*mockServerConfiguration, getSessionTimeoutInMilliseconds())
        .WillByDefault(testing::Return(sessionIdleTimeout));

    auto target = createSessionProxy();

    target->onServerConfigurationUpdate(mockServerConfiguration);

    // when
    target->enterAction("some action");
    target->enterAction("some other action");
}

TEST_F(SessionProxyTest, enterActionAddsSplitSessionToBeaconSenderOnSplitByEvents)
{
    // expect
    EXPECT_CALL(*mockSessionCreator, createSession(testing::_))
        .Times(2)
        .WillOnce(testing::Return(mockSession))
        .WillOnce(testing::Return(mockSplitSession1));
    EXPECT_CALL(*mockBeaconSender, addSession(testing::Eq(mockSession)))
        .Times(1);
    EXPECT_CALL(*mockBeaconSender, addSession(testing::Eq(mockSplitSession1)))
        .Times(1);

    // given
    ON_CALL(*mockServerConfiguration, isSessionSplitByEventsEnabled())
        .WillByDefault(testing::Return(true));
    ON_CALL(*mockServerConfiguration, getMaxEventsPerSession())
        .WillByDefault(testing::Return(1));

    auto target = createSessionProxy();

    target->onServerConfigurationUpdate(mockServerConfiguration);

    // when
    target->enterAction("some action");
    target->enterAction("some other action");
}

TEST_F(SessionProxyTest, enterActionOnlySetsLastInteractionTimeIfActionReportingIsNotAllowed)
{
    // expect
    EXPECT_CALL(*mockSessionCreator, createSession(testing::_))
        .Times(1)
        .WillOnce(testing::Return(mockSession));

    // given
    const int64_t sessionCreationTime = 13;
    const int64_t lastInteractionTime = 17;
    ON_CALL(*mockBeacon, getSessionStartTime())
        .WillByDefault(testing::Return(sessionCreationTime));
    ON_CALL(*mockBeacon, isActionReportingAllowedByPrivacySettings())
        .WillByDefault(testing::Return(false));
    ON_CALL(*mockTimingProvider, provideTimestampInMilliseconds())
        .WillByDefault(testing::Return(lastInteractionTime));

    auto target = createSessionProxy();

    target->onServerConfigurationUpdate(mockServerConfiguration);

    // when
    target->enterAction("test");

    // then
    ASSERT_THAT(target->getTopLevelActionCount(), testing::Eq(0));
    ASSERT_THAT(target->getLastInteractionTime(), testing::Eq(lastInteractionTime));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// identify user tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(SessionProxyTest, identifyUserWithNullTagReportsUser)
{
    // expect
    EXPECT_CALL(*mockLogger, mockDebug("SessionProxy [sn=0, seq=0] identifyUser(nullptr)"))
        .Times(1);
    EXPECT_CALL(*mockSession, identifyUser(nullptr))
        .Times(1);

    // given
    auto target = createSessionProxy();

    // when
    target->identifyUser(nullptr);
}

TEST_F(SessionProxyTest, identifyUserWithEmptyTagReportsUser)
{
    // expect
    EXPECT_CALL(*mockLogger, mockDebug("SessionProxy [sn=0, seq=0] identifyUser()"))
        .Times(1);
    EXPECT_CALL(*mockSession, identifyUser(testing::StrEq("")))
        .Times(1);

    // given
    auto target = createSessionProxy();

    // when
    target->identifyUser("");
}

TEST_F(SessionProxyTest, identifyUserWithNonEmptyTagReportsUser)
{
    // with
    const char* userTag = "user";

    // expect
    EXPECT_CALL(*mockLogger, mockWarning(testing::_))
        .Times(0);
    EXPECT_CALL(*mockSession, identifyUser(testing::Eq(userTag)))
        .Times(1);

    // given
    auto target = createSessionProxy();

    // when
    target->identifyUser(userTag);
}

TEST_F(SessionProxyTest, identifyUserLogsInvocation)
{
    // with
    const char* userTag = "user";

    // expect
    EXPECT_CALL(*mockLogger, mockDebug(std::string("SessionProxy [sn=0, seq=0] identifyUser(") + userTag + ")"))
        .Times(1);
    EXPECT_CALL(*mockLogger, isDebugEnabled())
        .Times(1);

    // given
    auto target = createSessionProxy();

    // when
    target->identifyUser("user");
}

TEST_F(SessionProxyTest, identifyUserDoesNothingIfSessionIsEnded)
{
    // expect
    EXPECT_CALL(*mockSession, identifyUser(testing::_))
        .Times(0);

    // given
    auto target = createSessionProxy();
    target->end();

    // when
    target->identifyUser("Jane Doe");
}

TEST_F(SessionProxyTest, identifyUserDoesNotIncreaseTopLevelEventCount)
{
    // given
    auto target = createSessionProxy();
    ASSERT_THAT(target->getTopLevelActionCount(), testing::Eq(0));
 
    // when
    target->identifyUser("Jane Doe");

    // then
    ASSERT_THAT(target->getTopLevelActionCount(), testing::Eq(0));
}

TEST_F(SessionProxyTest, identifyUserSetsLastInterActionTime)
{
    // given
    const long sessionCreationTime = 13;
    const long lastInteractionTime = 17;
    ON_CALL(*mockBeacon, getSessionStartTime())
        .WillByDefault(testing::Return(sessionCreationTime));
    ON_CALL(*mockTimingProvider, provideTimestampInMilliseconds())
        .WillByDefault(testing::Return(lastInteractionTime));

    auto target = createSessionProxy();
    ASSERT_THAT(target->getLastInteractionTime(), testing::Eq(sessionCreationTime));

    // when
    target->identifyUser("Jane Doe");

    // then
    ASSERT_THAT(target->getLastInteractionTime(), testing::Eq(lastInteractionTime));
}

TEST_F(SessionProxyTest, identifyUserDoesNotSplitSession)
{
    // expect
    EXPECT_CALL(*mockSessionCreator, createSession(testing::_))
        .Times(1)
        .WillOnce(testing::Return(mockSession));

    // given
    ON_CALL(*mockServerConfiguration, isSessionSplitByEventsEnabled())
        .WillByDefault(testing::Return(true));
    ON_CALL(*mockServerConfiguration, getMaxEventsPerSession())
        .WillByDefault(testing::Return(1));

    const int32_t eventCount = 10;
    auto target = createSessionProxy();

    // when
    for (auto i = 0; i < eventCount; i++)
    {
        target->identifyUser("some user");
    }

    // then
    ASSERT_THAT(target->getTopLevelActionCount(), testing::Eq(0));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// report crash tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(SessionProxyTest, reportingCrashWithNullErrorNameDoesNotReportAnything)
{
    // expect
    EXPECT_CALL(*mockLogger, mockWarning("SessionProxy [sn=0, seq=0] reportCrash: errorName must not be null or empty"))
        .Times(1);
    EXPECT_CALL(*mockSession, reportCrash(testing::_, testing::_, testing::_))
        .Times(0);

    // given
    auto target = createSessionProxy();

    // when
    target->reportCrash(nullptr, "some reason", "some stack trace");
}

TEST_F(SessionProxyTest, reportingCrashWithEmptyErrorNameDoesNotReportAnything)
{
    // expect
    EXPECT_CALL(*mockLogger, mockWarning("SessionProxy [sn=0, seq=0] reportCrash: errorName must not be null or empty"))
        .Times(1);
    EXPECT_CALL(*mockSession, reportCrash(testing::_, testing::_, testing::_))
        .Times(0);

    // given
    auto target = createSessionProxy();

    // when
    target->reportCrash("", "some reason", "some stack trace");
}

TEST_F(SessionProxyTest, reportingCrashWithNullReasonAndStacktraceWorks)
{
    // with
    const char* errorName = "errorName";
    const char* errorReason = nullptr;
    const char* stacktrace = nullptr;

    // expect
    EXPECT_CALL(*mockSession, reportCrash(testing::Eq(errorName), testing::Eq(errorReason), testing::Eq(stacktrace)))
        .Times(1);

    // given
    auto target = createSessionProxy();
    target->onServerConfigurationUpdate(mockServerConfiguration);

    // when
    target->reportCrash(errorName, errorReason, stacktrace);
}

TEST_F(SessionProxyTest, reportingCrashWithEmptyReasonAndStacktraceStringWorks)
{
    // with
    const char* errorName = "errorName";
    const char* errorReason = "";
    const char* stacktrace = "";

    // expect
    EXPECT_CALL(*mockSession, reportCrash(testing::Eq(errorName), testing::Eq(errorReason), testing::Eq(stacktrace)))
        .Times(1);

    // given
    auto target = createSessionProxy();
    target->onServerConfigurationUpdate(mockServerConfiguration);

    // when
    target->reportCrash(errorName, errorReason, stacktrace);
}

TEST_F(SessionProxyTest, reportCrashLogsInvocation)
{
    // with
    const char* errorName = "error name";
    const char* reason = "error reason";
    const char* stacktrace = "the stacktrace causing the error";

    // expect
    EXPECT_CALL(*mockLogger, isDebugEnabled())
        .Times(1);
    EXPECT_CALL(*mockLogger, mockDebug(std::string("SessionProxy [sn=0, seq=0] reportCrash(") + errorName + ", " + reason + ", " + stacktrace + ")"))
        .Times(1);

    // given
    auto target = createSessionProxy();
    target->onServerConfigurationUpdate(mockServerConfiguration);

    // when
    target->reportCrash(errorName, reason, stacktrace);
}

TEST_F(SessionProxyTest, reportCrashDoesNothingIfSessionIsEnded)
{
    // expect
    EXPECT_CALL(*mockSession, reportCrash(testing::_, testing::_, testing::_))
        .Times(0);

    // given
    auto target = createSessionProxy();
    target->end();

    // when
    target->reportCrash("errorName", "reason", "stacktrace");
}

TEST_F(SessionProxyTest, reportCrashDoesNotIncreaseTopLevelEventCount)
{
    // given
    auto target = createSessionProxy();
    ASSERT_THAT(target->getTopLevelActionCount(), testing::Eq(0));

    target->onServerConfigurationUpdate(mockServerConfiguration);

    // when
    target->reportCrash("errorName", "reason", "stacktrace");

    // then
    ASSERT_THAT(target->getTopLevelActionCount(), testing::Eq(0));
}

TEST_F(SessionProxyTest, reportCrashAlwaysSplitsSessionAfterReportingCrash)
{
    // expect
    EXPECT_CALL(*mockSessionCreator, createSession(testing::_))
        .Times(3)
        .WillOnce(testing::Return(mockSession))
        .WillOnce(testing::Return(mockSplitSession1))
        .WillOnce(testing::Return(mockSplitSession2));

    EXPECT_CALL(*mockSession, reportCrash(testing::StrEq("error 1"), testing::StrEq("reason 1"), testing::StrEq("stacktrace 1")))
        .Times(1);
    EXPECT_CALL(*mockSplitSession1, reportCrash(testing::StrEq("error 2"), testing::StrEq("reason 2"), testing::StrEq("stacktrace 2")))
        .Times(1);

    EXPECT_CALL(*mockSessionWatchdog, closeOrEnqueueForClosing(testing::Eq(mockSession), mockServerConfiguration->getSendIntervalInMilliseconds()))
        .Times(1);
    EXPECT_CALL(*mockSessionWatchdog, closeOrEnqueueForClosing(testing::Eq(mockSplitSession1), mockServerConfiguration->getSendIntervalInMilliseconds()))
        .Times(1);

    // given
    // explicitly disable session splitting
    ON_CALL(*mockServerConfiguration, isSessionSplitByEventsEnabled())
        .WillByDefault(testing::Return(false));
    ON_CALL(*mockServerConfiguration, isSessionSplitByIdleTimeoutEnabled())
        .WillByDefault(testing::Return(false));
    ON_CALL(*mockServerConfiguration, isSessionSplitBySessionDurationEnabled())
        .WillByDefault(testing::Return(false));

    auto target = createSessionProxy();
    target->onServerConfigurationUpdate(mockServerConfiguration);

    // when
    target->reportCrash("error 1", "reason 1", "stacktrace 1");
    target->reportCrash("error 2", "reason 2", "stacktrace 2");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// trace web request tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(SessionProxyTest, traceWebRequestWithValidUrlStringDelegatesToRealSession)
{
    // with
    const char* url = "https://www.google.com";

    // expect
    EXPECT_CALL(*mockSession, traceWebRequest(testing::Eq(url)))
        .Times(1);

    // given
    auto target = createSessionProxy();

    // when
    target->traceWebRequest(url);
}

TEST_F(SessionProxyTest, tracingANullStringWebRequestIsNotAllowed)
{
    // expect
    EXPECT_CALL(*mockLogger, mockWarning("SessionProxy [sn=0, seq=0] traceWebRequest: url must not be null or empty"))
        .Times(1);
    EXPECT_CALL(*mockSession, traceWebRequest(testing::_))
        .Times(0);

    // given
    auto target = createSessionProxy();

    // when
    auto obtained = target->traceWebRequest(nullptr);

    // then
    ASSERT_THAT(obtained, testing::NotNull());
    ASSERT_THAT(std::dynamic_pointer_cast<NullWebRequestTracer_t>(obtained), testing::NotNull());
}

TEST_F(SessionProxyTest, tracingAnEmptyStringWebRequestIsNotAllowed)
{
    // expect
    EXPECT_CALL(*mockLogger, mockWarning("SessionProxy [sn=0, seq=0] traceWebRequest: url must not be null or empty"))
        .Times(1);
    EXPECT_CALL(*mockSession, traceWebRequest(testing::_))
        .Times(0);

    // given
    auto target = createSessionProxy();

    // when
    auto obtained = target->traceWebRequest("");

    // then
    ASSERT_THAT(obtained, testing::NotNull());
    ASSERT_THAT(std::dynamic_pointer_cast<NullWebRequestTracer_t>(obtained), testing::NotNull());
}

TEST_F(SessionProxyTest, tracingAStringWebRequestWithInvalidUrlIsNotAllowed)
{
    // expect
    EXPECT_CALL(*mockLogger, mockWarning("SessionProxy [sn=0, seq=0] traceWebRequest: url \"foobar/://\" does not have a valid scheme"))
        .Times(1);
    EXPECT_CALL(*mockSession, traceWebRequest(testing::_))
        .Times(0);

    // given
    auto target = createSessionProxy();

    // when
    auto obtained = target->traceWebRequest("foobar/://");

    // then
    ASSERT_THAT(obtained, testing::NotNull());
    ASSERT_THAT(std::dynamic_pointer_cast<NullWebRequestTracer_t>(obtained), testing::NotNull());
}

TEST_F(SessionProxyTest, traceWebRequestWithStringArgumentGivesNullTracerIfSessionIsEnded)
{
    // given
    auto target = createSessionProxy();
    target->end();

    // when
    auto obtained = target->traceWebRequest("http://www.google.com");

    // then
    ASSERT_THAT(obtained, testing::NotNull());
    ASSERT_THAT(std::dynamic_pointer_cast<NullWebRequestTracer_t>(obtained), testing::NotNull());
}

TEST_F(SessionProxyTest, traceWebRequestWithStringLogsInvocation)
{
    // with
    const char* url = "https://localhost";

    // expect
    EXPECT_CALL(*mockLogger, isDebugEnabled())
        .Times(1);
    EXPECT_CALL(*mockLogger, mockDebug(std::string("SessionProxy [sn=0, seq=0] traceWebRequest(") + url + ")"))
        .Times(1);

    // given
    auto target = createSessionProxy();

    // when
    target->traceWebRequest(url);
}

TEST_F(SessionProxyTest, traceWebRequestWithStringDoesNotIncreaseTopLevelEventCount)
{
    // given
    auto target = createSessionProxy();
    ASSERT_THAT(target->getTopLevelActionCount(), testing::Eq(0));

    // when
    target->traceWebRequest("https://localhost");

    // then
    ASSERT_THAT(target->getTopLevelActionCount(), testing::Eq(0));
}

TEST_F(SessionProxyTest, traceWebRequestWithStringUrlSetsLastInterActionTime)
{
    // given
    const long sessionCreationTime = 13;
    const long lastInteractionTime = 17;
    ON_CALL(*mockBeacon, getSessionStartTime())
        .WillByDefault(testing::Return(sessionCreationTime));
    ON_CALL(*mockTimingProvider, provideTimestampInMilliseconds())
        .WillByDefault(testing::Return(lastInteractionTime));

    auto target = createSessionProxy();
    ASSERT_THAT(target->getLastInteractionTime(), testing::Eq(sessionCreationTime));

    // when
    target->traceWebRequest("https://localhost");

    // then
    ASSERT_THAT(target->getLastInteractionTime(), testing::Eq(lastInteractionTime));
}

TEST_F(SessionProxyTest, traceWebRequestWithStringUrlDoesNotSplitSession)
{
    // expect
    EXPECT_CALL(*mockSessionCreator, createSession(testing::_))
        .Times(1);

    // given
    const int32_t eventCount = 10;

    ON_CALL(*mockServerConfiguration, isSessionSplitByEventsEnabled())
        .WillByDefault(testing::Return(false));
    ON_CALL(*mockServerConfiguration, getMaxEventsPerSession())
        .WillByDefault(testing::Return(1));

    auto target = createSessionProxy();
    target->onServerConfigurationUpdate(mockServerConfiguration);

    // when
    for (auto i = 0; i < eventCount; i++)
    {
    	target->traceWebRequest("https://localhost");
    }

    // then
    ASSERT_THAT(target->getTopLevelActionCount(), testing::Eq(0));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// end tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(SessionProxyTest, endFinishesTheSession)
{
    // expect
    EXPECT_CALL(*mockParent, onChildClosed(testing::_))
        .Times(1);

    // given
    auto target = createSessionProxy();

    // when
    target->end();
}

TEST_F(SessionProxyTest, endingAnAlreadyEndedSessionDoesNothing)
{
    // expect
    EXPECT_CALL(*mockParent, onChildClosed(testing::_))
        .Times(1);

    // given
    auto target = createSessionProxy();
    target->end();

    // when ending already ended session
    target->end();
}

TEST_F(SessionProxyTest, endingASessionImplicitlyClosesAllOpenChildObjects)
{
    // with
    auto childObjectOne = MockIOpenKitObject::createNice();
    auto childObjectTwo = MockIOpenKitObject::createNice();

    // expect
    EXPECT_CALL(*childObjectOne, close())
        .Times(1);
    EXPECT_CALL(*childObjectTwo, close())
        .Times(1);

    // given
    auto target = createSessionProxy();
    target->storeChildInList(childObjectOne);
    target->storeChildInList(childObjectTwo);

    // when
    target->end();
}

TEST_F(SessionProxyTest, endLogsInvocation)
{
    // expect
    EXPECT_CALL(*mockLogger, mockDebug("SessionProxy [sn=0, seq=0] end()"))
        .Times(1);

    // given
    auto target = createSessionProxy();

    // when
    target->end();
}

TEST_F(SessionProxyTest, endRemovesSessionProxyFromSessionWatchdog)
{
    // expect
    EXPECT_CALL(*mockSessionWatchdog, removeFromSplitByTimeout(testing::_))
        .Times(1);

    // given
    auto target = createSessionProxy();

    // when
    target->end();
}

TEST_F(SessionProxyTest, closeSessionEndsTheSession)
{
    // expect
    EXPECT_CALL(*mockParent, onChildClosed(testing::_))
        .Times(1);

    // given
    auto target = createSessionProxy();

    // when
    target->close();
}

TEST_F(SessionProxyTest, endCallsDifferentMethodsForSessions)
{
    // with
    auto childObjectOne = MockSessionInternals::createNice();
    auto childObjectTwo = MockSessionInternals::createNice();

    // expect
    EXPECT_CALL(*childObjectOne, end(false))
        .Times(1);
    EXPECT_CALL(*childObjectTwo, end(false))
        .Times(1);
    EXPECT_CALL(*mockSession, end(true))
        .Times(1);

    auto target = createSessionProxy();

    target->storeChildInList(childObjectOne);
    target->storeChildInList(childObjectTwo);

    // when
    target->end();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// split session by time
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


TEST_F(SessionProxyTest, splitSessionByTimeReturnsMinusOneIfSessionProxyIsFinished)
{
    // expect
    EXPECT_CALL(*mockSessionCreator, createSession(testing::_))
        .Times(1);

    // given
    auto target = createSessionProxy();
    target->end();

    // when
    auto obtained = target->splitSessionByTime();

    // then
    ASSERT_THAT(obtained, testing::Eq(-1));
}

TEST_F(SessionProxyTest, splitSessionByTimeReturnsMinusOneIfServerConfigurationIsNotSet)
{
    // expect
    EXPECT_CALL(*mockSessionCreator, createSession(testing::_))
        .Times(1);

    // given
    auto target = createSessionProxy();

    // when
    auto obtained = target->splitSessionByTime();

    // then
    ASSERT_THAT(obtained, testing::Eq(-1));
}

TEST_F(SessionProxyTest, splitByTimeDoesNotPerformSplitIfNeitherSplitByIdleTimeoutNorSplitByDurationEnabled)
{
    // expect
    EXPECT_CALL(*mockSessionCreator, createSession(testing::_))
        .Times(1);

    // given
    ON_CALL(*mockServerConfiguration, isSessionSplitByIdleTimeoutEnabled())
        .WillByDefault(testing::Return(false));
    ON_CALL(*mockServerConfiguration, isSessionSplitBySessionDurationEnabled())
        .WillByDefault(testing::Return(false));

    auto target = createSessionProxy();

    target->onServerConfigurationUpdate(mockServerConfiguration);

    // when
    auto obtained = target->splitSessionByTime();

    // then
    ASSERT_THAT(obtained, testing::Eq(-1));
}

TEST_F(SessionProxyTest, splitByTimeSplitsCurrentSessionIfIdleTimeoutReached)
{
    // with
    const int64_t lastInteractionTimeSessionOne = 60;
    const int32_t idleTimeout = 10; // time to split: last interaction + idle => 70
    const int64_t currentTime = 70;
    const int64_t sessionTwoCreationTime = 80;

    // expect
    EXPECT_CALL(*mockTimingProvider, provideTimestampInMilliseconds())
        .Times(2)
        .WillOnce(testing::Return(lastInteractionTimeSessionOne))
        .WillOnce(testing::Return(currentTime));
    EXPECT_CALL(*mockSessionCreator, createSession(testing::_))
        .Times(2)
        .WillOnce(testing::Return(mockSession))
        .WillOnce(testing::Return(mockSplitSession1));
    EXPECT_CALL(*mockSessionCreator, reset())
        .Times(1);
    EXPECT_CALL(*mockSessionWatchdog, closeOrEnqueueForClosing(testing::Eq(mockSession), idleTimeout / 2))
        .Times(1);

    // given
    ON_CALL(*mockSplitBeacon1, getSessionStartTime())
        .WillByDefault(testing::Return(sessionTwoCreationTime));
    ON_CALL(*mockServerConfiguration, isSessionSplitByIdleTimeoutEnabled())
        .WillByDefault(testing::Return(true));
    ON_CALL(*mockServerConfiguration, getSessionTimeoutInMilliseconds())
        .WillByDefault(testing::Return(idleTimeout));
    ON_CALL(*mockServerConfiguration, isSessionSplitBySessionDurationEnabled())
        .WillByDefault(testing::Return(false));

    auto target = createSessionProxy();
    
    target->onServerConfigurationUpdate(mockServerConfiguration);

    target->identifyUser("test"); // update last interaction time
    ASSERT_THAT(target->getLastInteractionTime(), testing::Eq(lastInteractionTimeSessionOne));

    // when
    auto obtained = target->splitSessionByTime();

    // then
    ASSERT_THAT(obtained, testing::Eq(sessionTwoCreationTime + idleTimeout));
}

TEST_F(SessionProxyTest, splitByTimeSplitsCurrentSessionIfIdleTimeoutExceeded)
{
    // with
    const int64_t lastInteractionTimeSessionOne = 60;
    const int32_t idleTimeout = 10; // time to split: last interaction + idle => 70
    const int64_t currentTime = 80;
    const int64_t sessionTwoCreationTime = 90;

    // expect
    EXPECT_CALL(*mockTimingProvider, provideTimestampInMilliseconds())
        .Times(2)
        .WillOnce(testing::Return(lastInteractionTimeSessionOne))
        .WillOnce(testing::Return(currentTime));
    EXPECT_CALL(*mockSessionCreator, createSession(testing::_))
        .Times(2)
        .WillOnce(testing::Return(mockSession))
        .WillOnce(testing::Return(mockSplitSession1));
    EXPECT_CALL(*mockSessionCreator, reset())
        .Times(1);
    EXPECT_CALL(*mockSessionWatchdog, closeOrEnqueueForClosing(testing::Eq(mockSession), idleTimeout / 2))
        .Times(1);

    // given
    ON_CALL(*mockSplitBeacon1, getSessionStartTime())
        .WillByDefault(testing::Return(sessionTwoCreationTime));
    ON_CALL(*mockServerConfiguration, isSessionSplitByIdleTimeoutEnabled())
        .WillByDefault(testing::Return(true));
    ON_CALL(*mockServerConfiguration, getSessionTimeoutInMilliseconds())
        .WillByDefault(testing::Return(idleTimeout));
    ON_CALL(*mockServerConfiguration, isSessionSplitBySessionDurationEnabled())
        .WillByDefault(testing::Return(false));

    auto target = createSessionProxy();
    target->onServerConfigurationUpdate(mockServerConfiguration);

    target->identifyUser("test"); // update last interaction time
    ASSERT_THAT(target->getLastInteractionTime(), testing::Eq(lastInteractionTimeSessionOne));

    // when
    auto obtained = target->splitSessionByTime();

    // then
    ASSERT_THAT(obtained, testing::Eq(sessionTwoCreationTime + idleTimeout));
}

TEST_F(SessionProxyTest, splitByTimeDoesNotSplitCurrentSessionIfIdleTimeoutNotExpired)
{
    // with
    const int64_t lastInteractionTime = 60;
    const int32_t idleTimeout = 20; // time to split: list interaction + idle => 80
    const int64_t currentTime = 70;

    // expect
    EXPECT_CALL(*mockTimingProvider, provideTimestampInMilliseconds())
        .Times(2)
        .WillOnce(testing::Return(lastInteractionTime))
        .WillOnce(testing::Return(currentTime));
    EXPECT_CALL(*mockSessionCreator, createSession(testing::_))
        .Times(1);

    // given
    ON_CALL(*mockServerConfiguration, isSessionSplitByIdleTimeoutEnabled())
        .WillByDefault(testing::Return(true));
    ON_CALL(*mockServerConfiguration, getSessionTimeoutInMilliseconds())
        .WillByDefault(testing::Return(idleTimeout));
    ON_CALL(*mockServerConfiguration, isSessionSplitBySessionDurationEnabled())
        .WillByDefault(testing::Return(false));

    auto target = createSessionProxy();
    target->onServerConfigurationUpdate(mockServerConfiguration);

    target->identifyUser("test"); // update last interaction time
    ASSERT_THAT(target->getLastInteractionTime(), testing::Eq(lastInteractionTime));

    // when
    auto obtained = target->splitSessionByTime();

    // then
    ASSERT_THAT(obtained, testing::Eq(lastInteractionTime + idleTimeout));
}

TEST_F(SessionProxyTest, splitByTimeSplitsCurrentSessionIfMaxDurationReached)
{
    // with
    const int32_t sendInterval = 15;

    // expect
    EXPECT_CALL(*mockSessionWatchdog, closeOrEnqueueForClosing(testing::Eq(mockSession), sendInterval))
        .Times(1);
    EXPECT_CALL(*mockSessionCreator, reset())
        .Times(1);
    EXPECT_CALL(*mockSessionCreator, createSession(testing::_))
        .Times(2)
        .WillOnce(testing::Return(mockSession))
        .WillOnce(testing::Return(mockSplitSession1));

    // given
    const int64_t startTimeFirstSession = 60;
    const int32_t sessionDuration = 10; // split time: session start + duration = 70
    const int64_t currentTime = 70;
    const int64_t startTimeSecondSession = 80;

    ON_CALL(*mockTimingProvider, provideTimestampInMilliseconds())
        .WillByDefault(testing::Return(currentTime));
    ON_CALL(*mockBeacon, getSessionStartTime())
        .WillByDefault(testing::Return(startTimeFirstSession));
    ON_CALL(*mockSplitBeacon1, getSessionStartTime())
        .WillByDefault(testing::Return(startTimeSecondSession));

    ON_CALL(*mockServerConfiguration, isSessionSplitBySessionDurationEnabled())
        .WillByDefault(testing::Return(true));
    ON_CALL(*mockServerConfiguration, getMaxSessionDurationInMilliseconds())
        .WillByDefault(testing::Return(sessionDuration));
    ON_CALL(*mockServerConfiguration, isSessionSplitByIdleTimeoutEnabled())
        .WillByDefault(testing::Return(false));
    ON_CALL(*mockServerConfiguration, getSendIntervalInMilliseconds())
        .WillByDefault(testing::Return(sendInterval));

    auto target = createSessionProxy();
    target->onServerConfigurationUpdate(mockServerConfiguration);

    // when
    auto obtained = target->splitSessionByTime();

    // then
    ASSERT_THAT(obtained, testing::Eq(startTimeSecondSession + sessionDuration));
}

TEST_F(SessionProxyTest, splitByTimeSplitsCurrentSessionIfMaxDurationExceeded)
{
    // with
    const int32_t sendInterval = 15;

    // expect
    EXPECT_CALL(*mockSessionWatchdog, closeOrEnqueueForClosing(testing::Eq(mockSession), sendInterval))
        .Times(1);
    EXPECT_CALL(*mockSessionCreator, reset())
        .Times(1);
    EXPECT_CALL(*mockSessionCreator, createSession(testing::_))
        .Times(2)
        .WillOnce(testing::Return(mockSession))
        .WillOnce(testing::Return(mockSplitSession1));

    // given
    const int64_t startTimeFirstSession = 60;
    const int32_t sessionDuration = 10; // split time: session start + duration => 70
    const int64_t currentTime = 80;
    const int64_t startTimeSecondSession = 90;

    ON_CALL(*mockTimingProvider, provideTimestampInMilliseconds())
        .WillByDefault(testing::Return(currentTime));
    ON_CALL(*mockBeacon, getSessionStartTime())
        .WillByDefault(testing::Return(startTimeFirstSession));
    ON_CALL(*mockSplitBeacon1, getSessionStartTime())
        .WillByDefault(testing::Return(startTimeSecondSession));

    ON_CALL(*mockServerConfiguration, isSessionSplitBySessionDurationEnabled())
        .WillByDefault(testing::Return(true));
    ON_CALL(*mockServerConfiguration, getMaxSessionDurationInMilliseconds())
        .WillByDefault(testing::Return(sessionDuration));
    ON_CALL(*mockServerConfiguration, isSessionSplitByIdleTimeoutEnabled())
        .WillByDefault(testing::Return(false));
    ON_CALL(*mockServerConfiguration, getSendIntervalInMilliseconds())
        .WillByDefault(testing::Return(sendInterval));

    auto target = createSessionProxy();
    target->onServerConfigurationUpdate(mockServerConfiguration);

    // when
    auto obtained = target->splitSessionByTime();

    // then
    ASSERT_THAT(obtained, testing::Eq(startTimeSecondSession + sessionDuration));
}

TEST_F(SessionProxyTest, splitByTimeDoesNotSplitCurrentSessionIfMaxDurationNotReached)
{
    // expect
    EXPECT_CALL(*mockSessionCreator, createSession(testing::_))
        .Times(1)
        .WillOnce(testing::Return(mockSession));

    // given
    const int64_t sessionStartTime = 60;
    const int32_t sessionDuration = 20; // split time: start time + duration => 80
    const int64_t currentTime = 70;

    ON_CALL(*mockTimingProvider, provideTimestampInMilliseconds())
        .WillByDefault(testing::Return(currentTime));
    ON_CALL(*mockBeacon, getSessionStartTime())
        .WillByDefault(testing::Return(sessionStartTime));

    ON_CALL(*mockServerConfiguration, isSessionSplitBySessionDurationEnabled())
        .WillByDefault(testing::Return(true));
    ON_CALL(*mockServerConfiguration, getMaxSessionDurationInMilliseconds())
        .WillByDefault(testing::Return(sessionDuration));
    ON_CALL(*mockServerConfiguration, isSessionSplitByIdleTimeoutEnabled())
        .WillByDefault(testing::Return(false));

    auto target = createSessionProxy();
    target->onServerConfigurationUpdate(mockServerConfiguration);

    // when
    auto obtained = target->splitSessionByTime();

    // then
    ASSERT_THAT(obtained, testing::Eq(sessionStartTime + sessionDuration));
}

TEST_F(SessionProxyTest, splitBySessionTimeReturnsIdleSplitTimeWhenBeforeSessionDurationSplitTime)
{
    // with
    const int64_t sessionStartTime = 50;
    const int32_t sessionDuration = 40; // duration split time: start time + duration => 90
    const int64_t lastInteractionTime = 60;
    const int32_t idleTimeout = 20; // idle split time: last interaction + idle => 80
    const int64_t currentTime = 70;

    // expect
    EXPECT_CALL(*mockSessionCreator, createSession(testing::_))
        .Times(1)
        .WillOnce(testing::Return(mockSession));

    EXPECT_CALL(*mockTimingProvider, provideTimestampInMilliseconds())
        .Times(2)
        .WillOnce(testing::Return(lastInteractionTime))
        .WillOnce(testing::Return(currentTime));

    // given
    ON_CALL(*mockBeacon, getSessionStartTime())
        .WillByDefault(testing::Return(sessionStartTime));

    ON_CALL(*mockServerConfiguration, isSessionSplitByIdleTimeoutEnabled())
        .WillByDefault(testing::Return(true));
    ON_CALL(*mockServerConfiguration, getSessionTimeoutInMilliseconds())
        .WillByDefault(testing::Return(idleTimeout));
    ON_CALL(*mockServerConfiguration, isSessionSplitBySessionDurationEnabled())
        .WillByDefault(testing::Return(true));
    ON_CALL(*mockServerConfiguration, getMaxSessionDurationInMilliseconds())
        .WillByDefault(testing::Return(sessionDuration));

    auto target = createSessionProxy();
    target->onServerConfigurationUpdate(mockServerConfiguration);

    target->identifyUser("test"); // update last interaction time

    // when
    auto obtained = target->splitSessionByTime();

    // then
    ASSERT_THAT(obtained, testing::Eq(lastInteractionTime + idleTimeout));
}

TEST_F(SessionProxyTest, splitBySessionTimeReturnsDurationSplitTimeWhenBeforeIdleSplitTime)
{
    // with
    const int64_t sessionStartTime = 50;
    const int32_t sessionDuration = 30; // duration split time: start time + duration => 80
    const int64_t lastInteractionTime = 60;
    const int32_t idleTimeout = 50; // idle split time: last interaction + idle => 110
    const int64_t currentTime = 70;

    // expect
    EXPECT_CALL(*mockSessionCreator, createSession(testing::_))
        .Times(1)
        .WillOnce(testing::Return(mockSession));

    EXPECT_CALL(*mockTimingProvider, provideTimestampInMilliseconds())
        .Times(2)
        .WillOnce(testing::Return(lastInteractionTime))
        .WillOnce(testing::Return(currentTime));

    // given
    ON_CALL(*mockBeacon, getSessionStartTime())
        .WillByDefault(testing::Return(sessionStartTime));

    ON_CALL(*mockServerConfiguration, isSessionSplitByIdleTimeoutEnabled())
        .WillByDefault(testing::Return(true));
    ON_CALL(*mockServerConfiguration, getSessionTimeoutInMilliseconds())
        .WillByDefault(testing::Return(idleTimeout));
    ON_CALL(*mockServerConfiguration, isSessionSplitBySessionDurationEnabled())
        .WillByDefault(testing::Return(true));
    ON_CALL(*mockServerConfiguration, getMaxSessionDurationInMilliseconds())
        .WillByDefault(testing::Return(sessionDuration));

    auto target = createSessionProxy();
    target->onServerConfigurationUpdate(mockServerConfiguration);

    target->identifyUser("test"); // update last interaction time

    // when
    auto obtained = target->splitSessionByTime();

    // then
    ASSERT_THAT(obtained, testing::Eq(sessionStartTime + sessionDuration));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// identifyUser on split sessions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(SessionProxyTest, splitByTimeReAppliesUserIdentificationTag)
{
    // given
    const int64_t lastInteractionTimeSessionOne = 60;
    const int32_t idleTimeout = 10; // time to split: last interaction + idle => 70
    const int64_t currentTime = 70;
    const int64_t sessionTwoCreationTime = 80;

    // expect
    EXPECT_CALL(*mockTimingProvider, provideTimestampInMilliseconds())
        .Times(2)
        .WillOnce(testing::Return(lastInteractionTimeSessionOne))
        .WillOnce(testing::Return(currentTime));
    EXPECT_CALL(*mockSessionCreator, createSession(testing::_))
        .Times(2)
        .WillOnce(testing::Return(mockSession))
        .WillOnce(testing::Return(mockSplitSession1));

    EXPECT_CALL(*mockSplitSession1, identifyUser(testing::StrEq("test")))
        .Times(1);

    // given
    ON_CALL(*mockSplitBeacon1, getSessionStartTime())
        .WillByDefault(testing::Return(sessionTwoCreationTime));

    ON_CALL(*mockServerConfiguration, isSessionSplitByIdleTimeoutEnabled())
        .WillByDefault(testing::Return(true));
    ON_CALL(*mockServerConfiguration, getSessionTimeoutInMilliseconds())
        .WillByDefault(testing::Return(idleTimeout));
    ON_CALL(*mockServerConfiguration, isSessionSplitBySessionDurationEnabled())
        .WillByDefault(testing::Return(false));

    auto target = createSessionProxy();
    target->onServerConfigurationUpdate(mockServerConfiguration);

    target->identifyUser("test"); // update last interaction time
    ASSERT_THAT(target->getLastInteractionTime(), testing::Eq(lastInteractionTimeSessionOne));

    // when
    target->splitSessionByTime();
}

TEST_F(SessionProxyTest, splitByEventCountReAppliesUserIdentificationTag)
{
    // expect 
    EXPECT_CALL(*mockSessionCreator, createSession(testing::_))
        .Times(3)
        .WillOnce(testing::Return(mockSession))
        .WillOnce(testing::Return(mockSplitSession1))
        .WillOnce(testing::Return(mockSplitSession2));

    EXPECT_CALL(*mockSplitSession1, identifyUser(testing::StrEq("test1")))
        .Times(1);

    EXPECT_CALL(*mockSplitSession2, identifyUser(testing::StrEq("test1")))
        .Times(1);

    // given
    const int maxEventCount = 1;
    ON_CALL(*mockServerConfiguration, isSessionSplitByEventsEnabled())
        .WillByDefault(testing::Return(true));
    ON_CALL(*mockServerConfiguration, getMaxEventsPerSession())
        .WillByDefault(testing::Return(maxEventCount));

    auto target = createSessionProxy();
    target->onServerConfigurationUpdate(mockServerConfiguration);

    // when
    target->identifyUser("test1");
    target->enterAction("action 1");
    target->enterAction("action 2");
    target->enterAction("action 3");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// further tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(SessionProxyTest, onChildClosedRemovesChildFromList)
{
    // given
    IOpenKitComposite_sp target = createSessionProxy();
    ASSERT_THAT(target->getCopyOfChildObjects().size(), testing::Eq(1)); // initial session

    // when
    auto childObject = MockIOpenKitObject::createNice();

    target->storeChildInList(childObject);

    // then
    ASSERT_THAT(target->getCopyOfChildObjects().size(), testing::Eq(2));

    // when child gets closed
    target->onChildClosed(childObject);

    // then
    ASSERT_THAT(target->getCopyOfChildObjects().size(), testing::Eq(1));
}

TEST_F(SessionProxyTest, onChildClosedCallsDequeueOnSessionWatchdog)
{
    // with
    auto session = MockSessionInternals::createNice();

    // expect
    EXPECT_CALL(*mockSessionWatchdog, dequeueFromClosing(testing::Eq(session)))
        .Times(1);

    // given
    auto target = std::dynamic_pointer_cast<core::objects::IOpenKitComposite>(createSessionProxy());
    target->storeChildInList(session);

    // when
    target->onChildClosed(session);
}

TEST_F(SessionProxyTest, onServerConfigurationUpdateTakesOverServerConfigurationOnFirstCall)
{
    // given
    ON_CALL(*mockServerConfiguration, isSessionSplitByEventsEnabled())
        .WillByDefault(testing::Return(true));
    ON_CALL(*mockServerConfiguration, getMaxEventsPerSession())
        .WillByDefault(testing::Return(1));

    auto target = createSessionProxy();
    ASSERT_THAT(target->getServerConfiguration(), testing::IsNull());

    // when
    target->onServerConfigurationUpdate(mockServerConfiguration);

    // then
    ASSERT_THAT(target->getServerConfiguration(), testing::Eq(mockServerConfiguration));
}

TEST_F(SessionProxyTest, onServerConfigurationUpdateMergesServerConfigurationOnConsecutiveCalls)
{
    // with
    auto mockFirstConfig = MockIServerConfiguration::createNice();
    auto mockSecondConfig = MockIServerConfiguration::createStrict();

    // expect
    EXPECT_CALL(*mockFirstConfig, merge(testing::Eq(mockSecondConfig)))
        .Times(1);
    EXPECT_CALL(*mockFirstConfig, isSessionSplitBySessionDurationEnabled())
        .Times(1);
    EXPECT_CALL(*mockFirstConfig, isSessionSplitByIdleTimeoutEnabled())
        .Times(1);

    // given
    auto target = createSessionProxy();

    // when
    target->onServerConfigurationUpdate(mockFirstConfig);
    target->onServerConfigurationUpdate(mockSecondConfig);
}

TEST_F(SessionProxyTest, onServerConfigurationUpdateAddsSessionProxyToWatchdogIfSplitByDurationEnabled)
{
    // expect
    EXPECT_CALL(*mockSessionWatchdog, addToSplitByTimeout(testing::_))
        .Times(1);

    // given
    auto target = createSessionProxy();
    ON_CALL(*mockServerConfiguration, isSessionSplitBySessionDurationEnabled())
        .WillByDefault(testing::Return(true));

    // when
    target->onServerConfigurationUpdate(mockServerConfiguration);
}

TEST_F(SessionProxyTest, onServerConfigurationUpdateAddsSessionProxyToWatchdogIfSplitByIdleTimeoutEnabled)
{
    // expect
    EXPECT_CALL(*mockSessionWatchdog, addToSplitByTimeout(testing::_))
        .Times(1);

    // given
    ON_CALL(*mockServerConfiguration, isSessionSplitByIdleTimeoutEnabled())
        .WillByDefault(testing::Return(true));

    auto target = createSessionProxy();

    // when
    target->onServerConfigurationUpdate(mockServerConfiguration);
}

TEST_F(SessionProxyTest, onServerConfigurationUpdateDoesNotAddSessionProxyToWatchdogIfSplitByIdleTimeoutAndDurationDisabled)
{
    // expect
    EXPECT_CALL(*mockSessionWatchdog, addToSplitByTimeout(testing::_))
        .Times(0);

    // given
    ON_CALL(*mockServerConfiguration, isSessionSplitByIdleTimeoutEnabled())
        .WillByDefault(testing::Return(false));
    ON_CALL(*mockServerConfiguration, isSessionSplitBySessionDurationEnabled())
        .WillByDefault(testing::Return(false));

    auto target = createSessionProxy();

    // when
    target->onServerConfigurationUpdate(mockServerConfiguration);
}

TEST_F(SessionProxyTest, onServerConfigurationUpdateDoesNotAddSessionProxyToWatchdogOnConsecutiveCalls)
{
    // expect
    EXPECT_CALL(*mockSessionWatchdog, addToSplitByTimeout(testing::_))
        .Times(0);

    // given
    ON_CALL(*mockServerConfiguration, isSessionSplitByIdleTimeoutEnabled())
        .WillByDefault(testing::Return(false));
    ON_CALL(*mockServerConfiguration, isSessionSplitBySessionDurationEnabled())
        .WillByDefault(testing::Return(false));

    auto target = createSessionProxy();
    target->onServerConfigurationUpdate(mockServerConfiguration);

    auto mockServerConfigTwo = MockIServerConfiguration::createNice();
    ON_CALL(*mockServerConfigTwo, isSessionSplitByIdleTimeoutEnabled())
        .WillByDefault(testing::Return(true));
    ON_CALL(*mockServerConfigTwo, isSessionSplitBySessionDurationEnabled())
        .WillByDefault(testing::Return(true));

    // when
    target->onServerConfigurationUpdate(mockServerConfigTwo);
}

TEST_F(SessionProxyTest, toStringReturnsAppropriateResult)
{
    // given
    ON_CALL(*mockBeacon, getSessionNumber())
        .WillByDefault(testing::Return(37));
    ON_CALL(*mockBeacon, getSessionSequenceNumber())
        .WillByDefault(testing::Return(73));
    auto target = createSessionProxy();

    // when
    auto obtained = target->toString();

    // then
    ASSERT_THAT(obtained, testing::Eq("SessionProxy [sn=37, seq=73]"));
}
