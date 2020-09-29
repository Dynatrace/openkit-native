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

#include "mock/MockISessionCreator.h"
#include "mock/MockIOpenKitComposite.h"
#include "mock/MockIOpenKitObject.h"
#include "mock/MockSessionInternals.h"
#include "../configuration/mock/MockIServerConfiguration.h"
#include "../../api/mock/MockILogger.h"
#include "../../protocol/mock/MockIBeacon.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <memory>
#include <array>

using namespace test;

using SessionProxy_t = core::objects::SessionProxy;
using SessionProxy_sp = std::shared_ptr<SessionProxy_t>;
using SessionInternals_sp = std::shared_ptr<core::objects::SessionInternals>;
using IOpenKitComposite_sp = std::shared_ptr<core::objects::IOpenKitComposite>;
using NullRootAction_t = core::objects::NullRootAction;
using NullWebRequestTracer_t = core::objects::NullWebRequestTracer;
using MockILogger_sp = std::shared_ptr<MockILogger>;
using MockIOpenKitComposite_sp = std::shared_ptr<MockIOpenKitComposite>;
using MockSessionInternals_sp = std::shared_ptr<MockSessionInternals>;
using MockIBeacon_sp = std::shared_ptr<MockIBeacon>;
using MockISessionCreator_sp = std::shared_ptr<MockISessionCreator>;
using MockIServerConfiguration_sp = std::shared_ptr<MockIServerConfiguration>;


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
        mockSession = MockSessionInternals::createNice();
        ON_CALL(*mockSession, getBeacon())
            .WillByDefault(testing::Return(mockBeacon));

        mockSplitBeacon1 = MockIBeacon::createNice();
        mockSplitSession1 = MockSessionInternals::createNice();
        ON_CALL(*mockSplitSession1, getBeacon())
            .WillByDefault(testing::Return(mockSplitBeacon1));

        mockSplitBeacon2 = MockIBeacon::createNice();
        mockSplitSession2 = MockSessionInternals::createNice();
        ON_CALL(*mockSplitSession2, getBeacon())
            .WillByDefault(testing::Return(mockSplitBeacon2));

        mockSessionCreator = MockISessionCreator::createNice();
        ON_CALL(*mockSessionCreator, createSession(testing::_))
            .WillByDefault(testing::Return(mockSession));
    }

    SessionProxy_sp createSessionProxy()
    {
        return SessionProxy_t::createSessionProxy(mockLogger, mockParent, mockSessionCreator);
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// enter action tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


TEST_F(SessionProxyTest, enterActionWithNullActionNameGivesNullRootActionObject)
{
    // expect
    EXPECT_CALL(*mockLogger, mockWarning("SessionProxy enterAction: actionName must not be null or empty"))
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
    EXPECT_CALL(*mockLogger, mockWarning("SessionProxy enterAction: actionName must not be null or empty"))
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
    EXPECT_CALL(*mockLogger, mockDebug(std::string("SessionProxy enterAction(") + actionName + ")"))
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
        .WillOnce(testing::Return(mockSplitSession1))
        .WillRepeatedly(testing::ReturnNull());

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
        .WillOnce(testing::Return(mockSplitSession2))
        .WillRepeatedly(testing::ReturnNull());

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
        .WillOnce(testing::Return(mockSplitSession2))
        .WillRepeatedly(testing::ReturnNull());

    // given
    const int32_t maxEventCount = 3;
    ON_CALL(*mockServerConfiguration, isSessionSplitByEventsEnabled())
        .WillByDefault(testing::Return(true));
    ON_CALL(*mockServerConfiguration, getMaxEventsPerSession())
        .WillByDefault(testing::Return(maxEventCount));

    auto target = createSessionProxy();
    target->onServerConfigurationUpdate(mockServerConfiguration);

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// identify user tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(SessionProxyTest, identifyUserWithNullTagDoesNothing)
{
    // expect
    EXPECT_CALL(*mockLogger, mockWarning("SessionProxy identifyUser: userTag must not be null or empty"))
        .Times(1);
    EXPECT_CALL(*mockSession, identifyUser(testing::_))
        .Times(0);

    // given
    auto target = createSessionProxy();

    // when
    target->identifyUser(nullptr);
}

TEST_F(SessionProxyTest, identifyUserWithEmptyTagDoesNothing)
{
    // expect
    EXPECT_CALL(*mockLogger, mockWarning("SessionProxy identifyUser: userTag must not be null or empty"))
        .Times(1);
    EXPECT_CALL(*mockSession, identifyUser(testing::_))
        .Times(0);

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
    EXPECT_CALL(*mockLogger, mockDebug(std::string("SessionProxy identifyUser(") + userTag + ")"))
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

TEST_F(SessionProxyTest, identifyUserDoesNotSplitSession)
{
    // expect
    EXPECT_CALL(*mockSessionCreator, createSession(testing::_))
        .Times(1)
        .WillOnce(testing::Return(mockSession))
        .WillRepeatedly(testing::ReturnNull());

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
    EXPECT_CALL(*mockLogger, mockWarning("SessionProxy reportCrash: errorName must not be null or empty"))
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
    EXPECT_CALL(*mockLogger, mockWarning("SessionProxy reportCrash: errorName must not be null or empty"))
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
    EXPECT_CALL(*mockLogger, mockDebug(std::string("SessionProxy reportCrash(") + errorName + ", " + reason + ", " + stacktrace + ")"))
        .Times(1);

    // given
    auto target = createSessionProxy();

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

    // when
    target->reportCrash("errorName", "reason", "stacktrace");

    // then
    ASSERT_THAT(target->getTopLevelActionCount(), testing::Eq(0));
}

TEST_F(SessionProxyTest, reportCrashUserDoesNotSplitSession)
{
    // expect
    EXPECT_CALL(*mockSessionCreator, createSession(testing::_))
        .Times(1);

    // given
    ON_CALL(*mockServerConfiguration, isSessionSplitByEventsEnabled())
        .WillByDefault(testing::Return(true));
    ON_CALL(*mockServerConfiguration, getMaxEventsPerSession())
        .WillByDefault(testing::Return(1));

    const int32_t eventCount = 10;

    auto target = createSessionProxy();
    target->onServerConfigurationUpdate(mockServerConfiguration);

    // when
    for (auto i = 0; i < eventCount; i++)
    {
        target->reportCrash("error", "reason", "stacktrace");
    }

    // then
    ASSERT_THAT(target->getTopLevelActionCount(), testing::Eq(0));
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
    EXPECT_CALL(*mockLogger, mockWarning("SessionProxy traceWebRequest: url must not be null or empty"))
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
    EXPECT_CALL(*mockLogger, mockWarning("SessionProxy traceWebRequest: url must not be null or empty"))
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
    EXPECT_CALL(*mockLogger, mockWarning("SessionProxy traceWebRequest: url \"foobar/://\" does not have a valid scheme"))
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
    EXPECT_CALL(*mockLogger, mockDebug(std::string("SessionProxy traceWebRequest(") + url + ")"))
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
    EXPECT_CALL(*mockLogger, mockDebug("SessionProxy end()"))
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

TEST_F(SessionProxyTest, toStringReturnsAppropriateResult)
{
    // given
    auto target = createSessionProxy();

    // when
    auto obtained = target->toString();

    // then
    ASSERT_THAT(obtained, testing::Eq("SessionProxy"));
}
