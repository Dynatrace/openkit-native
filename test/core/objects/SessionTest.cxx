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

#include "builder/TestSessionBuilder.h"
#include "mock/MockIOpenKitComposite.h"
#include "mock/MockIOpenKitObject.h"
#include "../mock/MockIBeaconSender.h"
#include "../configuration/mock/MockIBeaconCacheConfiguration.h"
#include "../configuration/mock/MockIBeaconConfiguration.h"
#include "../configuration/mock/MockIPrivacyConfiguration.h"
#include "../configuration/mock/MockIServerConfiguration.h"
#include "../../api/mock/MockILogger.h"
#include "../../protocol/mock/MockIAdditionalQueryParameters.h"
#include "../../protocol/mock/MockIBeacon.h"
#include "../../providers/mock/MockIHTTPClientProvider.h"

#include "core/UTF8String.h"
#include "core/objects/IOpenKitObject.h"
#include "core/objects/NullRootAction.h"
#include "core/objects/NullWebRequestTracer.h"
#include "core/objects/RootAction.h"
#include "core/objects/Session.h"
#include "core/objects/WebRequestTracer.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>
#include <core/objects/Session.h>

using namespace test;

using IOpenKitObject_t = core::objects::IOpenKitObject;
using MockIBeaconSender_sp = std::shared_ptr<MockIBeaconSender>;
using MockILogger_sp = std::shared_ptr<MockILogger>;
using MockIAdditionalQueryParameters_sp = std::shared_ptr<MockIAdditionalQueryParameters>;
using NullRootAction_t = core::objects::NullRootAction;
using NullWebRequestTracer_t = core::objects::NullWebRequestTracer;
using RootAction_t = core::objects::RootAction;
using Session_t = core::objects::Session;
using SessionBuilder_sp = std::shared_ptr<TestSessionBuilder>;
using Utf8String_t = core::UTF8String;
using WebRequestTracer_t = core::objects::WebRequestTracer;

class SessionTest : public testing::Test
{
protected:

	MockILogger_sp mockLogger;
	MockIAdditionalQueryParameters_sp mockAdditionalParameters;

	void SetUp() override
	{
		mockLogger = MockILogger::createNice();
		mockAdditionalParameters = MockIAdditionalQueryParameters::createNice();
	}

	SessionBuilder_sp createSession()
	{
		auto builder = std::make_shared<TestSessionBuilder>();

		builder->with(mockLogger);

		return builder;
	}
};

TEST_F(SessionTest, startSessionCallsBeacon)
{
	// with
	auto mockBeaconStrict = MockIBeacon::createStrict();

	// expect
	EXPECT_CALL(*mockBeaconStrict, startSession())
		.Times(1);

	// given
	auto target = createSession()
		->with(mockBeaconStrict)
		.build();

	// when
	target->startSession();
}

TEST_F(SessionTest, enterActionWithNullActionNameGivesNullRootActionObject)
{
	// with
	auto logger = MockILogger::createStrict();

	// expect
	EXPECT_CALL(*logger, mockWarning("Session [sn=0] enterAction: actionName must not be null or empty"))
		.Times(1);

	// given
	auto target = createSession()
		->with(logger)
		.build();

	// when
	auto obtained = target->enterAction(nullptr);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(std::dynamic_pointer_cast<NullRootAction_t>(obtained), testing::NotNull());
}

TEST_F(SessionTest, enterActionWithEmptyActionNameGivesNullRootActionObject)
{
	// with
	auto logger = MockILogger::createStrict();

	// expect
	EXPECT_CALL(*logger, mockWarning("Session [sn=0] enterAction: actionName must not be null or empty"))
		.Times(1);

	// given
	auto target = createSession()
		->with(logger)
		.build();

	// when
	auto obtained = target->enterAction("");

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(std::dynamic_pointer_cast<NullRootAction_t>(obtained), testing::NotNull());
}

TEST_F(SessionTest, enterActionWitnNonEmptyNameGivesRootAction)
{
	// given
	auto target = createSession()->build();

	// when
	auto obtained = target->enterAction("some action");

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(std::dynamic_pointer_cast<RootAction_t>(obtained), testing::NotNull());

	// break dependency cycle: root action in child objects of session
	obtained->leaveAction();
}

TEST_F(SessionTest, enterActionAlwaysGivesANewInstance)
{
	// given
	const char* actionName = "some action";
	auto target = createSession()->build();

	// when
	auto obtainedOne = target->enterAction(actionName);
	auto obtainedTwo = target->enterAction(actionName);

	// then
	ASSERT_THAT(obtainedOne, testing::NotNull());
	ASSERT_THAT(obtainedTwo, testing::NotNull());
	ASSERT_THAT(obtainedOne, testing::Ne(obtainedTwo));

	// break dependency cycle: root action in child objects of session
	obtainedOne->leaveAction();
	obtainedTwo->leaveAction();
}

TEST_F(SessionTest, enterActionAddsNewlyCreatedActionToTheListOfChildObjects)
{
	// given
	auto target = createSession()
		->build();

	// when
	auto obtainedOne = target->enterAction("some action");

	// then
	auto childList = target->getCopyOfChildObjects();
	ASSERT_THAT(childList.size(), testing::Eq(size_t(1)));

	auto rootActionOne = std::dynamic_pointer_cast<RootAction_t>(obtainedOne);
	ASSERT_THAT(rootActionOne, testing::NotNull());

	auto objectOne = std::dynamic_pointer_cast<IOpenKitObject_t>(rootActionOne->getActionImpl());
	ASSERT_THAT(objectOne, testing::NotNull());
	ASSERT_THAT(objectOne, testing::Eq(*childList.begin()));

	// and when entering a second time
	auto obtainedTwo = target->enterAction("some action");

	// then
	childList = target->getCopyOfChildObjects();
	ASSERT_THAT(childList.size(), testing::Eq(size_t(2)));

	auto rootActionTwo = std::dynamic_pointer_cast<RootAction_t>(obtainedTwo);
	ASSERT_THAT(rootActionTwo, testing::NotNull());

	auto objectTwo = std::dynamic_pointer_cast<IOpenKitObject_t>(rootActionTwo->getActionImpl());
	ASSERT_THAT(objectTwo, testing::NotNull());

	ASSERT_THAT(objectOne, testing::Eq(*childList.begin()));
	ASSERT_THAT(objectTwo, testing::Eq(*(++childList.begin())));

	// break dependency cycle: action Impls in the session's child list
	target->end();
}

TEST_F(SessionTest, enterActionWithDifferentNameGivesDifferentInstance)
{
	// given
	auto target = createSession()->build();

	// when
	auto obtainedOne = target->enterAction("some action 1");
	auto obtainedTwo = target->enterAction("some action 2");

	// then
	ASSERT_THAT(obtainedOne, testing::NotNull());
	ASSERT_THAT(obtainedTwo, testing::NotNull());
	ASSERT_THAT(obtainedOne, testing::Ne(obtainedTwo));

	// break dependency cycle: root action in child objects of session
	obtainedOne->leaveAction();
	obtainedTwo->leaveAction();
}

TEST_F(SessionTest, enterActionDoesNotAddToTheBeaconCache)
{
	// with
	auto mockBeaconStrict = MockIBeacon::createStrict();

	// expect
	EXPECT_CALL(*mockBeaconStrict, getSessionNumber())
		.Times(1); // Session toString on debug log
	EXPECT_CALL(*mockBeaconStrict, getCurrentTimestamp())
		.Times(1); // initialize action
	EXPECT_CALL(*mockBeaconStrict, createSequenceNumber())
		.Times(1); // initialize action
	EXPECT_CALL(*mockBeaconStrict, createID())
		.Times(1); // initialize action

	// given
	auto target = createSession()
		->with(mockBeaconStrict)
		.build();

	// when
	auto obtained = target->enterAction("Some action");

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	testing::Mock::VerifyAndClearExpectations(mockBeaconStrict.get());

	// break dependency cycle: root action in child objects of session
	auto rootAction = std::dynamic_pointer_cast<RootAction_t>(obtained);
	auto actionImpl = rootAction->getActionImpl();
	target->removeChildFromList(std::dynamic_pointer_cast<IOpenKitObject_t>(actionImpl));
}

TEST_F(SessionTest, enterActionLogsInvocation)
{
	// with
	auto logger = MockILogger::createStrict();

	// expect
	EXPECT_CALL(*logger, mockDebug("Session [sn=0] enterAction(some action)"));
	EXPECT_CALL(*logger, isDebugEnabled())
			.Times(2) // 1 when logging method invocation + 1 when leaving action to break dependency cycle
			.WillOnce(testing::Return(true))
			.WillRepeatedly(testing::Return(false));

	// given
	auto target = createSession()
			->with(logger)
			.build();

	// when
	auto obtained = target->enterAction("some action");

	// break dependency cycle: root action in child objects of session
	obtained->leaveAction();
}

TEST_F(SessionTest, identifyUserWithNullTagReportsUser)
{
	// with
	auto logger = MockILogger::createStrict();
	auto mockBeaconStrict = MockIBeacon::createStrict();

	// expect
		EXPECT_CALL(*logger, isDebugEnabled())
		.Times(1)
		.WillOnce(testing::Return(true));
	EXPECT_CALL(*logger, mockDebug("Session [sn=0] identifyUser(nullptr)"))
		.Times(1);
	EXPECT_CALL(*mockBeaconStrict, getSessionNumber())
		.Times(1);
	EXPECT_CALL(*mockBeaconStrict, identifyUser(DefaultValues::UTF8_EMPTY_STRING))
		.Times(1);

	// given
	auto target = createSession()
		->with(mockBeaconStrict)
		.with(logger)
		.build();

	// when
	target->identifyUser(nullptr);
}

TEST_F(SessionTest, identifyUserWithEmptyTagReportsUser)
{
	// with
	auto logger = MockILogger::createStrict();
	auto mockBeaconStrict = MockIBeacon::createStrict();

	// expect
	EXPECT_CALL(*logger, isDebugEnabled())
		.Times(1)
		.WillOnce(testing::Return(true));
	EXPECT_CALL(*logger, mockDebug("Session [sn=0] identifyUser()"))
		.Times(1);
	EXPECT_CALL(*mockBeaconStrict, getSessionNumber())
		.Times(1);
	EXPECT_CALL(*mockBeaconStrict, identifyUser(DefaultValues::UTF8_EMPTY_STRING))
		.Times(1);

	// given
	auto target = createSession()
		->with(mockBeaconStrict)
		.with(logger)
		.build();

	// when
	target->identifyUser("");
}

TEST_F(SessionTest, identifyUserWithNonEmptyTagReportsUser)
{
	// with
	auto mockBeaconStrict = MockIBeacon::createStrict();
	const char* userTag = "user";

	// expect
	EXPECT_CALL(*mockLogger, mockWarning(testing::_))
		.Times(0);
	EXPECT_CALL(*mockBeaconStrict, getSessionNumber())
		.Times(1); // Session toString on debug log
	EXPECT_CALL(*mockBeaconStrict, identifyUser(Utf8String_t(userTag)))
		.Times(1);

	// given
	auto target = createSession()
		->with(mockBeaconStrict)
		.build();

	// when
	target->identifyUser(userTag);
}

TEST_F(SessionTest, identifyUserMultipleTimesAlwaysCallsBeacon)
{
	// with
	auto mockBeaconStrict = MockIBeacon::createStrict();
	const char* userTag = "user";

	// expect
	EXPECT_CALL(*mockLogger, mockWarning(testing::_))
		.Times(0);
	EXPECT_CALL(*mockBeaconStrict, getSessionNumber())
		.Times(2); // session toString in debug log
	EXPECT_CALL(*mockBeaconStrict, identifyUser(Utf8String_t(userTag)))
		.Times(2);

	// given
	auto target = createSession()
		->with(mockBeaconStrict)
		.build();

	// when
	target->identifyUser(userTag);
	target->identifyUser(userTag);
}

TEST_F(SessionTest, identifyUserWithDifferentUsersAlwaysCallsBeacon)
{
	// with
	auto mockBeaconStrict = MockIBeacon::createStrict();
	const char* userTag1 = "user 1";
	const char* userTag2 = "user 2";

	// expect
	EXPECT_CALL(*mockLogger, mockWarning(testing::_))
		.Times(0);
	EXPECT_CALL(*mockBeaconStrict, getSessionNumber())
		.Times(2); // session toString in debug log
	EXPECT_CALL(*mockBeaconStrict, identifyUser(Utf8String_t(userTag1)))
		.Times(1);
	EXPECT_CALL(*mockBeaconStrict, identifyUser(Utf8String_t(userTag2)))
		.Times(1);

	// given
	auto target = createSession()
		->with(mockBeaconStrict)
		.build();

	// when
	target->identifyUser(userTag1);
	target->identifyUser(userTag2);
}

TEST_F(SessionTest, identifyUserLogsInvocation)
{
	// with
	auto logger = MockILogger::createStrict();

	// expect
	EXPECT_CALL(*logger, isDebugEnabled())
			.Times(1)
			.WillOnce(testing::Return(true));
	EXPECT_CALL(*logger, mockDebug("Session [sn=0] identifyUser(user)"));

	// given
	auto target = createSession()
			->with(logger)
			.build();

	// when
	target->identifyUser("user");
}

TEST_F(SessionTest, reportingCrashWithNullErrorNameDoesNotReportAnything)
{
	// with
	auto logger = MockILogger::createStrict();
	auto mockBeaconStrict = MockIBeacon::createStrict();
	const char* errorName = nullptr;
	const char* reason = "some reason";
	const char* stacktrace = "some stack trace";

	// expect
	EXPECT_CALL(*logger, mockWarning("Session [sn=0] reportCrash: errorName must not be null or empty"))
		.Times(1);
	EXPECT_CALL(*mockBeaconStrict,getSessionNumber())
		.Times(1); // session to string in debug log

	// given
	auto target = createSession()
		->with(mockBeaconStrict)
		.with(logger)
		.build();

	// when
	target->reportCrash(errorName, reason, stacktrace);
}

TEST_F(SessionTest, reportingCrashWithEmptyErrorNameDoesNotReportAnything)
{
	// with
	auto logger = MockILogger::createStrict();
	auto mockBeaconStrict = MockIBeacon::createStrict();
	const char* errorName = "";
	const char* reason = "some reason";
	const char* stacktrace = "some stack trace";

	// expect
	EXPECT_CALL(*logger, mockWarning("Session [sn=0] reportCrash: errorName must not be null or empty"))
		.Times(1);
	EXPECT_CALL(*mockBeaconStrict,getSessionNumber())
		.Times(1); // session to string in debug log

	// given
	auto target = createSession()
		->with(mockBeaconStrict)
		.with(logger)
		.build();

	// when
	target->reportCrash(errorName, reason, stacktrace);
}

TEST_F(SessionTest, reportingCrashWithNullReasonAndStacktraceWorks)
{
	// with
	auto mockBeaconNice = MockIBeacon::createNice();
	const char* errorName = "errorName";
	const char* reason = nullptr;
	const char* stacktrace = nullptr;

	// expect
	EXPECT_CALL(*mockBeaconNice, reportCrash(testing::Eq(errorName), testing::Eq(reason), testing::Eq(stacktrace)))
		.Times(1);

	// given
	auto target = createSession()
		->with(mockBeaconNice)
		.build();

	// when
	target->reportCrash(errorName, reason, stacktrace);
}

TEST_F(SessionTest, reportingCrashWithEmptyReasonAndStacktraceStringWorks)
{
	// with
	auto mockBeaconNice = MockIBeacon::createNice();
	const char* errorName = "errorName";
	const char* reason = "";
	const char* stacktrace = "";

	// expect
	EXPECT_CALL(*mockBeaconNice, reportCrash(testing::Eq(errorName), testing::Eq(reason), testing::Eq(stacktrace)))
		.Times(1);

	// given
	auto target = createSession()
		->with(mockBeaconNice)
		.build();

	// when
	target->reportCrash(errorName, reason, stacktrace);
}

TEST_F(SessionTest, reportCrashWorks)
{
	// with
	auto mockBeaconNice = MockIBeacon::createNice();
	const char* errorName = "errorName";
	const char* reason = "some reason";
	const char* stacktrace = "some stacktrace";

	// expect
	EXPECT_CALL(*mockBeaconNice, reportCrash(testing::Eq(errorName), testing::Eq(reason), testing::Eq(stacktrace)))
		.Times(1);

	// given
	auto target = createSession()
		->with(mockBeaconNice)
		.build();

	// when
	target->reportCrash(errorName, reason, stacktrace);
}

TEST_F(SessionTest, reportingCrashWithSameDataMultipleTimesForwardsEachCallToBeacon)
{
	// with
	auto mockBeaconNice = MockIBeacon::createNice();
	const char* errorName = "errorName";
	const char* reason = "some reason";
	const char* stacktrace = "some stacktrace";

	// expect
	EXPECT_CALL(*mockBeaconNice, reportCrash(testing::Eq(errorName), testing::Eq(reason), testing::Eq(stacktrace)))
		.Times(2);

	// given
	auto target = createSession()
		->with(mockBeaconNice)
		.build();

	// when
	target->reportCrash(errorName, reason, stacktrace);
	target->reportCrash(errorName, reason, stacktrace);
}


TEST_F(SessionTest, reportingCrashWithDifferentDataMultipleTimesForwardsEachCallToBeacon)
{
	// with
	auto mockBeaconNice = MockIBeacon::createNice();
	const char* errorName1 = "errorName 1";
	const char* errorName2 = "errorName 2";
	const char* reason1 = "some reason 1";
	const char* reason2 = "some reason 2";
	const char* stacktrace1 = "some stacktrace 1";
	const char* stacktrace2 = "some stacktrace 2";

	// expect
	EXPECT_CALL(*mockBeaconNice, reportCrash(testing::Eq(errorName1), testing::Eq(reason1), testing::Eq(stacktrace1)))
		.Times(1);
	EXPECT_CALL(*mockBeaconNice, reportCrash(testing::Eq(errorName2), testing::Eq(reason2),testing::Eq(stacktrace2)))
		.Times(1);

	// given
	auto target = createSession()
		->with(mockBeaconNice)
		.build();

	// when
	target->reportCrash(errorName1, reason1, stacktrace1);
	target->reportCrash(errorName2, reason2, stacktrace2);
}

TEST_F(SessionTest, reportCrashLogsInvocation)
{
	// with
	auto logger = MockILogger::createStrict();

	// expect
	EXPECT_CALL(*logger, isDebugEnabled())
		.Times(1)
		.WillOnce(testing::Return(true));
	EXPECT_CALL(*logger, mockDebug("Session [sn=0] reportCrash(name, reason, stacktrace)"));

	// given
	auto target = createSession()
		->with(logger)
		.build();

	// when
	target->reportCrash("name", "reason", "stacktrace");
}

TEST_F(SessionTest, endSessionFinishesSessionOnBeacon)
{
	// with
	auto mockBeaconNice = MockIBeacon::createNice();

	// expect
	EXPECT_CALL(*mockBeaconNice, endSession())
		.Times(1);

	// given
	auto target = createSession()
		->with(mockBeaconNice)
		.build();

	// when
	target->end();
}

TEST_F(SessionTest, endASessionDoesNotFinishSessionOnBeacon)
{
	// with
	auto mockBeaconNice = MockIBeacon::createNice();

	// expect
	EXPECT_CALL(*mockBeaconNice, endSession())
		.Times(0);

	// given
	const int64_t timestamp = 4321;
	ON_CALL(*mockBeaconNice, getCurrentTimestamp())
		.WillByDefault(testing::Return(timestamp));

	auto target = createSession()
		->with(mockBeaconNice)
		.build();

	// when
	target->end(false);
}

TEST_F(SessionTest, endingAnAlreadyEndedSessionDoesNothing)
{
	// with
	auto mockBeaconNice = MockIBeacon::createNice();

	// expect
	EXPECT_CALL(*mockBeaconNice, endSession())
		.Times(1); // only first invocation of end


	// given
	auto target = createSession()
		->with(mockBeaconNice)
		.build();

	// when
	target->end();

	// and when
	target->end();
}

TEST_F(SessionTest, endingASessionImplicitlyClosesAllOpenChildOjects)
{
	// with
	const auto childObjectOne = MockIOpenKitObject::createStrict();
	const auto childObjectTwo = MockIOpenKitObject::createStrict();

	// expect
	EXPECT_CALL(*childObjectOne, close())
		.Times(1);
	EXPECT_CALL(*childObjectTwo, close())
		.Times(1);

	// given
	auto target = createSession()->build();
	target->storeChildInList(childObjectOne);
	target->storeChildInList(childObjectTwo);

	// when
	target->end();
}

TEST_F(SessionTest, endingASessionRemovesItselfFromParent)
{
	// with
	auto mockParent = MockIOpenKitComposite::createStrict();

	// expect
	EXPECT_CALL(*mockParent, onChildClosed(testing::_))
		.Times(1);

	// given
	auto target = createSession()
		->with(mockParent)
		.build();

	// when
	target->end();
}

TEST_F(SessionTest, endLogsInvocation)
{
	// with
	auto logger = MockILogger::createStrict();

	// expect
	EXPECT_CALL(*logger, isDebugEnabled())
		.Times(1)
		.WillOnce(testing::Return(true));
	EXPECT_CALL(*logger, mockDebug("Session [sn=0] end()"));

	// given
	auto target = createSession()
		->with(logger)
		.build();

	// when
	target->end();
}

TEST_F(SessionTest, tryEndEndsSessionIfNoMoreChildObjects)
{
	// with
	auto mockBeacon = MockIBeacon::createNice();

	// expect
	EXPECT_CALL(*mockBeacon, endSession())
		.Times(1);

	// given
	auto target = createSession()->with(mockBeacon).build();
	auto action = target->enterAction("action");
	auto tracer = target->traceWebRequest("https://localhost");

	// when
	auto obtained = target->tryEnd();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));

	// and when
	action->leaveAction();
	obtained = target->tryEnd();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));

	// and when
	tracer->stop(200);
	obtained = target->tryEnd();

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
}

TEST_F(SessionTest, tryEndReturnsTrueIfSessionAlreadyEnded)
{
	// given
	auto target = createSession()->build();
	target->end();

	// when
	auto obtained = target->tryEnd();

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
}

TEST_F(SessionTest, tryEndMarksSessionStateAsWasTriedForEndingIfSessionNotClosable)
{
	// given
	auto target = createSession()->build();
	target->enterAction("action");

	// when
	auto obtained = target->tryEnd();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
	ASSERT_THAT(target->wasTriedForEnding(), testing::Eq(true));
	ASSERT_THAT(target->isFinished(), testing::Eq(false));

	// cleanup - otherwise the MockIBeacon is leaked
	target->end();
}

TEST_F(SessionTest, tryEndDoesNotMarkSessionStateAsWasTriedForEndingIfSessionIsClosable)
{
	// given
	auto target = createSession()->build();

	// when
	auto obtained = target->tryEnd();

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
	ASSERT_THAT(target->wasTriedForEnding(), testing::Eq(false));
	ASSERT_THAT(target->isFinished(), testing::Eq(true));
}

TEST_F(SessionTest, sendBeaconForwardsCallToBeacon)
{
	// with
	auto mockBeaconStrict = MockIBeacon::createStrict();
	auto mockHTTPClientProvider = MockIHTTPClientProvider::createNice();

	// expect
	EXPECT_CALL(*mockBeaconStrict, send(testing::Eq(mockHTTPClientProvider), testing::Ref(*mockAdditionalParameters)))
		.Times(1);

	// given
	auto target = createSession()
		->with(mockBeaconStrict)
		.build();

	// when
	target->sendBeacon(mockHTTPClientProvider, *mockAdditionalParameters);
}

TEST_F(SessionTest, clearCapturedDataForwardsCallToBeacon)
{
	// with
	auto mockBeaconStrict = MockIBeacon::createStrict();

	// expect
	EXPECT_CALL(*mockBeaconStrict, clearData())
		.Times(1);

	// given
	auto target = createSession()
		->with(mockBeaconStrict)
		.build();

	// when
	target->clearCapturedData();
}

TEST_F(SessionTest, isEmptyForwardsCallToBeacon)
{
	// with
	auto mockBeaconStrict= MockIBeacon::createStrict();

	// expect
	EXPECT_CALL(*mockBeaconStrict, isEmpty())
		.Times(1);

	// given
	auto target = createSession()
		->with(mockBeaconStrict)
		.build();

	// when
	target->isEmpty();
}

TEST_F(SessionTest, initializeServerConfigurationForwardsCallToBeacon)
{
	// with
	auto mockBeaconStrict = MockIBeacon::createStrict();
	auto mockServerConfig = MockIServerConfiguration::createStrict();

	// expect
	EXPECT_CALL(*mockBeaconStrict, initializeServerConfiguration(testing::Eq(mockServerConfig)))
		.Times(1);

	// given
	auto target = createSession()
		->with(mockBeaconStrict)
		.build();

	// when
	target->initializeServerConfiguration(mockServerConfig);
}

TEST_F(SessionTest, updateServerConfigurationForwardsCallToBeacon)
{
	// with
	auto mockBeaconStrict = MockIBeacon::createStrict();
	auto mockServerConfig = MockIServerConfiguration::createStrict();

	// expect
	EXPECT_CALL(*mockBeaconStrict, updateServerConfiguration(testing::Eq(mockServerConfig)))
		.Times(1);

	// given
	auto target = createSession()
		->with(mockBeaconStrict)
		.build();

	// when
	target->updateServerConfiguration(mockServerConfig);
}

TEST_F(SessionTest, aNewlyCreatedSessionIsNotFinished)
{
	//given
	auto target = createSession()->build();

	//when
	ASSERT_THAT(target->isFinished(), testing::Eq(false));
	ASSERT_THAT(target->isConfiguredAndFinished(), testing::Eq(false));
}

TEST_F(SessionTest, aNewlyCreatedSessionIsNotInStateConfigured)
{
	//given
	auto target = createSession()->build();

	//when
	ASSERT_THAT(target->isConfigured(), testing::Eq(false));
	ASSERT_THAT(target->isConfiguredAndFinished(), testing::Eq(false));
	ASSERT_THAT(target->isConfiguredAndOpen(), testing::Eq(false));
}

TEST_F(SessionTest, aNewlyCreatedSessionIsNotInStateAsWasTriedForEnding)
{
	//given
	auto target = createSession()->build();

	// when, then
	ASSERT_THAT(target->wasTriedForEnding(), testing::Eq(false));
}

TEST_F(SessionTest, aNotConfiguredNotFinishedSessionHasCorrectState)
{
	// with
	auto mockBeacon = MockIBeacon::createNice();

	ON_CALL(*mockBeacon, isServerConfigurationSet()).WillByDefault(testing::Return(false));

	// given
	auto target = createSession()
		->with(mockBeacon)
		.build();

	// when
	ASSERT_THAT(target->isConfigured(), testing::Eq(false));
	ASSERT_THAT(target->isConfiguredAndOpen(), testing::Eq(false));
	ASSERT_THAT(target->isConfiguredAndFinished(), testing::Eq(false));
	ASSERT_THAT(target->isFinished(), testing::Eq(false));
}

TEST_F(SessionTest, aConfiguredNotFinishedSessionHasCorrectState)
{
	// with
	auto mockBeacon = MockIBeacon::createNice();

	ON_CALL(*mockBeacon, isServerConfigurationSet()).WillByDefault(testing::Return(true));

	// given
	auto target = createSession()
		->with(mockBeacon)
		.build();

	// when
	ASSERT_THAT(target->isConfigured(), testing::Eq(true));
	ASSERT_THAT(target->isConfiguredAndOpen(), testing::Eq(true));
	ASSERT_THAT(target->isConfiguredAndFinished(), testing::Eq(false));
	ASSERT_THAT(target->isFinished(), testing::Eq(false));
}

TEST_F(SessionTest, aNotConfiguredFinishedSessionHasCorrectState)
{
	// with
	auto mockBeacon = MockIBeacon::createNice();

	ON_CALL(*mockBeacon, isServerConfigurationSet()).WillByDefault(testing::Return(false));

	// given
	auto target = createSession()
		->with(mockBeacon)
		.build();
	target->end();

	// when
	ASSERT_THAT(target->isConfigured(), testing::Eq(false));
	ASSERT_THAT(target->isConfiguredAndOpen(), testing::Eq(false));
	ASSERT_THAT(target->isConfiguredAndFinished(), testing::Eq(false));
	ASSERT_THAT(target->isFinished(), testing::Eq(true));
}

TEST_F(SessionTest, aConfiguredFinishedSessionHasCorrectState)
{
	// with
	auto mockBeacon = MockIBeacon::createNice();

	ON_CALL(*mockBeacon, isServerConfigurationSet()).WillByDefault(testing::Return(true));

	// given
	auto target = createSession()
		->with(mockBeacon)
		.build();
	target->end();

	// when
	ASSERT_THAT(target->isConfigured(), testing::Eq(true));
	ASSERT_THAT(target->isConfiguredAndOpen(), testing::Eq(false));
	ASSERT_THAT(target->isConfiguredAndFinished(), testing::Eq(true));
	ASSERT_THAT(target->isFinished(), testing::Eq(true));
}

TEST_F(SessionTest, aSessionIsEndedIfEndIsCalled)
{
	// given
	auto target = createSession()->build();

	// when
	target->end();

	// then
	ASSERT_THAT(target->isFinished(), testing::Eq(true));
}

TEST_F(SessionTest, enterActionGivesNullRootActionIfSessionIsAlreadyEnded)
{
	// given
	auto target = createSession()->build();
	target->end();

	// when
	auto obtained = target->enterAction("test");

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(std::dynamic_pointer_cast<NullRootAction_t>(obtained), testing::NotNull());
}

TEST_F(SessionTest, identifyUserDoesNothingIfSessionIsEnded)
{
	// with
	auto mockBeaconNice = MockIBeacon::createNice();

	// expect
	EXPECT_CALL(*mockBeaconNice, identifyUser(testing::_))
		.Times(0);

	// given
	auto target = createSession()
		->with(mockBeaconNice)
		.build();
	target->end();

	// when
	target->identifyUser("Jane Smith");
}

TEST_F(SessionTest, reportCrashDoesNothingIfSessionIsEnded)
{
	// with
	auto mockBeaconNice = MockIBeacon::createNice();

	// expect
	EXPECT_CALL(*mockBeaconNice, reportCrash(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	auto target = createSession()
		->with(mockBeaconNice)
		.build();
	target->end();

	// when
	target->reportCrash("errorName", "reason", "stacktrace");
}

TEST_F(SessionTest, closeEndsTheSession)
{
	// with
	auto mockBeaconNice = MockIBeacon::createNice();
	const int64_t timestamp = 1234;
	ON_CALL(*mockBeaconNice, getCurrentTimestamp())
		.WillByDefault(testing::Return(timestamp));

	// expect
	EXPECT_CALL(*mockBeaconNice, endSession())
		.Times(1);

	// given
	auto target = createSession()
		->with(mockBeaconNice)
		.build();

	// when
	target->close();
}

TEST_F(SessionTest, traceWebRequestWithValidUrlStringGivesAppropriateTracer)
{
	// with
	const char* url = "http://example.com/pages/";

	// given
	auto target = createSession()->build();

	// when
	auto obtained = target->traceWebRequest(url);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	auto tracer = std::dynamic_pointer_cast<WebRequestTracer_t>(obtained);
	ASSERT_THAT(tracer, testing::NotNull());
	ASSERT_THAT(tracer->getURL(), testing::Eq(Utf8String_t(url)));

	// break dependency cycle: tracer as child in session
	target->removeChildFromList(std::dynamic_pointer_cast<IOpenKitObject_t>(obtained));
}

TEST_F(SessionTest, traceWebRequestWithValidUrlStringAddsTracerToListOfChildren)
{
	// with
	const char* url = "http://example.com/pages/";

	// given
	auto target =createSession()->build();

	// when
	auto obtained = target->traceWebRequest(url);

	// then
	ASSERT_THAT(obtained, testing::NotNull());

	auto childObjects = target->getCopyOfChildObjects();
	ASSERT_THAT(childObjects.size(), testing::Eq(size_t(1)));
	ASSERT_THAT(*childObjects.begin(), testing::Eq(std::dynamic_pointer_cast<IOpenKitObject_t>(obtained)));

	// break dependency cycle: tracer in child list of session
	target->removeChildFromList(std::dynamic_pointer_cast<IOpenKitObject_t>(obtained));
}

TEST_F(SessionTest, tracingANullStringWebRequestIsNotAllowed)
{
	// with
	auto logger = MockILogger::createStrict();

	// expect
	EXPECT_CALL(*logger, mockWarning("Session [sn=0] traceWebRequest: url must not be null or empty"))
		.Times(1);

	// given
	const char* url = nullptr;
	auto target = createSession()
		->with(logger)
		.build();

	// when
	auto obtained = target->traceWebRequest(url);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(std::dynamic_pointer_cast<NullWebRequestTracer_t>(obtained), testing::NotNull());
}

TEST_F(SessionTest, tracingAnEmptyStringWebRequestIsNotAllowed)
{
	// with
	auto logger = MockILogger::createStrict();

	// expect
	EXPECT_CALL(*logger, mockWarning("Session [sn=0] traceWebRequest: url must not be null or empty"))
		.Times(1);

	// given
	const char* url = "";
	auto target = createSession()
		->with(logger)
		.build();

	// when
	auto obtained = target->traceWebRequest(url);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(std::dynamic_pointer_cast<NullWebRequestTracer_t>(obtained), testing::NotNull());
}

TEST_F(SessionTest, tracingAStringWebRequestWithInvalidUrlIsNotAllowed)
{
	// with
	auto logger = MockILogger::createStrict();

	// expect
	EXPECT_CALL(*logger, mockWarning("Session [sn=0] traceWebRequest: url \"1337://fourtytwo.com\" does not have a valid scheme"))
			.Times(1);

	// given
	const char* url = "1337://fourtytwo.com";
	auto target = createSession()
		->with(logger)
		.build();

	// when
	auto obtained = target->traceWebRequest(url);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(std::dynamic_pointer_cast<NullWebRequestTracer_t>(obtained), testing::NotNull());
}

TEST_F(SessionTest, traceWebRequestGivesNullTracerIfSessionIsEnded)
{
	// given
	const char* url = "http://example.com/pages/";
	auto target = createSession()->build();
	target->end();

	// when
	auto obtained = target->traceWebRequest(url);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(std::dynamic_pointer_cast<NullWebRequestTracer_t>(obtained), testing::NotNull());
}

TEST_F(SessionTest, traceWebRequestLogsInvocation)
{
	// expect
	EXPECT_CALL(*mockLogger, mockDebug("Session [sn=0] traceWebRequest(https://localhost)"))
		.Times(1);

	// given
	auto target = createSession()->build();

	// when
	auto obtained = target->traceWebRequest("https://localhost");

	// break dependency cycle
	target->removeChildFromList(std::dynamic_pointer_cast<IOpenKitObject_t>(obtained));
}

TEST_F(SessionTest, onChildClosedRemovesChildFromList)
{
	// given
	const auto childObject = MockIOpenKitObject::createNice();
	auto target = createSession()->build();
	target->storeChildInList(childObject);

	auto numChildObjects = target->getChildCount();
	ASSERT_THAT(numChildObjects, testing::Eq(size_t(1)));

	// when
	target->onChildClosed(childObject);

	// then
	numChildObjects = target->getChildCount();
	ASSERT_THAT(numChildObjects, testing::Eq(size_t(0)));
}

TEST_F(SessionTest, onChildClosedEndsSessionWithoutChildrenIfInStateWasTriedForEnding)
{
	// with
	auto mockParent = MockIOpenKitComposite::createNice();
	auto target = createSession()->with(mockParent).build();

	// expect
	EXPECT_CALL(*mockParent, onChildClosed(testing::_))
		.Times(1);

	// given
	auto childObject = MockIOpenKitObject::createNice();
	target->storeChildInList(childObject);

	auto wasClosed = target->tryEnd();
	ASSERT_THAT(wasClosed, testing::Eq(false));
	ASSERT_THAT(target->wasTriedForEnding(), testing::Eq(true));
	ASSERT_THAT(target->isFinished(), testing::Eq(false));

	// when
	target->onChildClosed(childObject);

	// then
	ASSERT_THAT(target->isFinished(), testing::Eq(true));
}


TEST_F(SessionTest, onChildClosedDoesNotEndSessionWithChildrenIfInStateWasTriedForEnding)
{
	// with
	auto mockParent = MockIOpenKitComposite::createNice();
	auto target = createSession()->with(mockParent).build();

	// expect
	EXPECT_CALL(*mockParent, onChildClosed(testing::_))
		.Times(0);

	// given
	auto childObjectOne = MockIOpenKitObject::createNice();
	auto childObjectTwo = MockIOpenKitObject::createNice();
	target->storeChildInList(childObjectOne);
	target->storeChildInList(childObjectTwo);

	auto wasClosed = target->tryEnd();
	ASSERT_THAT(wasClosed, testing::Eq(false));
	ASSERT_THAT(target->wasTriedForEnding(), testing::Eq(true));
	ASSERT_THAT(target->isFinished(), testing::Eq(false));

	// when
	target->onChildClosed(childObjectOne);

	// then
	ASSERT_THAT(target->isFinished(), testing::Eq(false));

	// cleanup
	EXPECT_CALL(*mockParent, onChildClosed(testing::_))
		.Times(1);
	target->close();
}

TEST_F(SessionTest, onChildClosedDoesNotEndSessionIfNotInStateWasTriedForEnding)
{
	// with
	auto mockParent = MockIOpenKitComposite::createNice();
	auto target = createSession()->with(mockParent).build();

	// expect
	EXPECT_CALL(*mockParent, onChildClosed(testing::_))
		.Times(0);

	// given
	auto childObject = MockIOpenKitObject::createNice();
	target->storeChildInList(childObject);

	// when
	target->onChildClosed(childObject);

	// then
	ASSERT_THAT(target->isFinished(), testing::Eq(false));
}

TEST_F(SessionTest, toStringReturnsAppropriateResult)
{
	// with
	int32_t sessionNumber = 21;
	auto mockBeaconNice = MockIBeacon::createNice();
	ON_CALL(*mockBeaconNice, getSessionNumber())
		.WillByDefault(testing::Return(sessionNumber));

	// given
	auto target = createSession()
		->with(mockBeaconNice)
		.build();

	// when
	auto obtained = target->toString();

	// then
	std::stringstream s;
	s << "Session [sn=" << sessionNumber << "]";
	ASSERT_THAT(obtained, testing::Eq(s.str()));
}

TEST_F(SessionTest, aNewSessionCanSendNewSessionRequests)
{
	// given
	auto target = createSession()->build();

	// when
	auto obtained = target->canSendNewSessionRequest();

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
}

TEST_F(SessionTest, canSendNewSessionRequestIsFalseIfAllRequestsAreUsedUp)
{
	// given
	auto target = createSession()->build();

	// when, then
	for (auto i = Session_t::MAX_NEW_SESSION_REQUESTS; i > 0; i--)
	{
		ASSERT_THAT(target->canSendNewSessionRequest(), testing::Eq(true));

		target->decreaseNumRemainingSessionRequests();
	}

	// then
	ASSERT_THAT(target->canSendNewSessionRequest(), testing::Eq(false));
}

TEST_F(SessionTest, isDataSendingAllowedReturnsTrueForConfiguredAndDataCaptureEnabledSession)
{
	// with
	auto mockBeacon = MockIBeacon::createNice();
	ON_CALL(*mockBeacon, isDataCapturingEnabled()).WillByDefault(testing::Return(true));
	ON_CALL(*mockBeacon, isServerConfigurationSet()).WillByDefault(testing::Return(true));

	auto target = createSession()
		->with(mockBeacon)
		.build();

	// when
	auto obtained = target->isDataSendingAllowed();

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
}

TEST_F(SessionTest, isDataSendingAllowedReturnsFalseForNotConfiguredSession)
{
	// given
	auto mockBeacon = MockIBeacon::createNice();
	ON_CALL(*mockBeacon, isDataCapturingEnabled()).WillByDefault(testing::Return(true));
	ON_CALL(*mockBeacon, isServerConfigurationSet()).WillByDefault(testing::Return(false));

	auto target = createSession()
		->with(mockBeacon)
		.build();

	// when
	auto obtained = target->isDataSendingAllowed();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(SessionTest, isDataSendingAllowedReturnsFalseForDataCaptureDisabledSession)
{
	// given
	auto mockBeacon = MockIBeacon::createNice();
	ON_CALL(*mockBeacon, isDataCapturingEnabled()).WillByDefault(testing::Return(false));
	ON_CALL(*mockBeacon, isServerConfigurationSet()).WillByDefault(testing::Return(true));

	auto target = createSession()
		->with(mockBeacon)
		.build();

	// when
	auto obtained = target->isDataSendingAllowed();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(SessionTest, isDataSendingAllowedReturnsFalseForNotConfiguredAndDataCaptureDisabledSession)
{
		// given
	auto mockBeacon = MockIBeacon::createNice();
	ON_CALL(*mockBeacon, isDataCapturingEnabled()).WillByDefault(testing::Return(false));
	ON_CALL(*mockBeacon, isServerConfigurationSet()).WillByDefault(testing::Return(false));

	auto target = createSession()
		->with(mockBeacon)
		.build();

	// when
	auto obtained = target->isDataSendingAllowed();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(SessionTest, enableCaptureDelegatesToBeacon)
{
	// with
	auto mockBeacon = MockIBeacon::createStrict();

	// expect
	EXPECT_CALL(*mockBeacon, enableCapture()).Times(1);

	// given
	auto target = createSession()
		->with(mockBeacon)
		.build();

	// when
	target->enableCapture();
}

TEST_F(SessionTest, disableCaptureDelegatesToBeacon)
{
	// with
	auto mockBeacon = MockIBeacon::createStrict();

	// expect
	EXPECT_CALL(*mockBeacon, disableCapture()).Times(1);

	// given
	auto target = createSession()
		->with(mockBeacon)
		.build();

	// when
	target->disableCapture();
}