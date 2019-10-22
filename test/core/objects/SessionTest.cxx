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

#include "MockIOpenKitObject.h"
#include "../MockBeaconSender.h"
#include "../../api/MockILogger.h"
#include "../../protocol/mock/MockIBeacon.h"
#include "../../protocol/mock/MockIHTTPClient.h"
#include "../../protocol/mock/MockIStatusResponse.h"
#include "../../providers/MockHTTPClientProvider.h"

#include "OpenKit/ISSLTrustManager.h"
#include "core/UTF8String.h"
#include "core/configuration/BeaconConfiguration.h"
#include "core/configuration/BeaconCacheConfiguration.h"
#include "core/configuration/Configuration.h"
#include "core/configuration/Device.h"
#include "core/configuration/OpenKitType.h"
#include "core/objects/IOpenKitObject.h"
#include "core/objects/NullRootAction.h"
#include "core/objects/NullWebRequestTracer.h"
#include "core/objects/RootAction.h"
#include "core/objects/Session.h"
#include "core/objects/WebRequestTracer.h"
#include "protocol/ssl/SSLStrictTrustManager.h"
#include "providers/DefaultTimingProvider.h"
#include "providers/DefaultSessionIDProvider.h"
#include "providers/ISessionIDProvider.h"
#include "providers/ITimingProvider.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>

using namespace test;

using BeaconCacheConfiguration_t = core::configuration::BeaconCacheConfiguration;
using BeaconCacheConfiguration_sp = std::shared_ptr<BeaconCacheConfiguration_t>;
using BeaconConfiguration_t = core::configuration::BeaconConfiguration;
using BeaconConfiguration_sp = std::shared_ptr<BeaconConfiguration_t>;
using Configuration_t = core::configuration::Configuration;
using Configuration_sp = std::shared_ptr<Configuration_t>;
using Device_t = core::configuration::Device;
using DefaultTimingProvider_t = providers::DefaultTimingProvider;
using DefaultSessionIdProvider_t = providers::DefaultSessionIDProvider;
using HttpClientConfiguration_t = core::configuration::HTTPClientConfiguration;
using IOpenKitObject_t = core::objects::IOpenKitObject;
using ISessionIdProvider_sp = std::shared_ptr<providers::ISessionIDProvider>;
using ISslTrustManager_sp = std::shared_ptr<openkit::ISSLTrustManager>;
using ITimingProvider_sp = std::shared_ptr<providers::ITimingProvider>;
using MockNiceHttpClientProvider_t = testing::NiceMock<MockHTTPClientProvider>;
using MockNiceHttpClientProvider_sp = std::shared_ptr<MockNiceHttpClientProvider_t>;
using MockNiceIBeacon_sp = std::shared_ptr<testing::NiceMock<MockIBeacon>>;
using MockNiceILogger_sp = std::shared_ptr<testing::NiceMock<MockILogger>>;
using MockNiceIHTTPClient_sp = std::shared_ptr<testing::NiceMock<MockIHTTPClient>>;
using MockStrictBeaconSender_t = testing::StrictMock<MockBeaconSender>;
using MockStrictBeaconSender_sp = std::shared_ptr<MockStrictBeaconSender_t>;
using MockStrictIBeacon_sp = std::shared_ptr<testing::StrictMock<MockIBeacon>>;
using NullRootAction_t = core::objects::NullRootAction;
using NullWebRequestTracer_t = core::objects::NullWebRequestTracer;
using OpenKitType_t = core::configuration::OpenKitType;
using RootAction_t = core::objects::RootAction;
using Session_t = core::objects::Session;
using Session_sp = std::shared_ptr<Session_t>;
using SslStrictTrustManager_t = protocol::SSLStrictTrustManager;
using Utf8String_t = core::UTF8String;
using WebRequestTracer_t = core::objects::WebRequestTracer;

static const char APP_ID[] = "appID";
static const char APP_NAME[] = "appName";

class SessionTest : public testing::Test
{
protected:
	MockNiceILogger_sp mockLogger;
	ITimingProvider_sp timingProvider;
	ISessionIdProvider_sp sessionIDProvider;

	ISslTrustManager_sp trustManager;

	BeaconConfiguration_sp beaconConfiguration;
	BeaconCacheConfiguration_sp beaconCacheConfiguration;
	Configuration_sp configuration;

	MockStrictBeaconSender_sp mockBeaconSender;
	MockStrictIBeacon_sp mockBeaconStrict;
	MockNiceIBeacon_sp mockBeaconNice;
	MockNiceHttpClientProvider_sp mockHTTPClientProvider;

	void SetUp()
	{
		mockLogger = MockILogger::createNice();

		timingProvider = std::make_shared<DefaultTimingProvider_t>();
		sessionIDProvider = std::make_shared<DefaultSessionIdProvider_t>();

		auto httpClientConfiguration = std::make_shared<HttpClientConfiguration_t>(Utf8String_t(""), 0, Utf8String_t(""));
		mockHTTPClientProvider = std::make_shared<MockNiceHttpClientProvider_t>();

		trustManager = std::make_shared<SslStrictTrustManager_t>();

		auto device = std::make_shared<Device_t>(Utf8String_t(""), Utf8String_t(""), Utf8String_t(""));

		beaconCacheConfiguration = std::make_shared<BeaconCacheConfiguration_t>(-1, -1, -1);
		beaconConfiguration = std::make_shared<BeaconConfiguration_t>();
		configuration = std::make_shared<Configuration_t>
		(
			device,
			OpenKitType_t::Type::DYNATRACE,
			Utf8String_t(APP_NAME),
			"",
			APP_ID,
			0,
			"0",
			"",
			sessionIDProvider,
			trustManager,
			beaconCacheConfiguration,
			beaconConfiguration
		);
		configuration->enableCapture();

		mockBeaconSender = std::make_shared<MockStrictBeaconSender_t>(mockLogger, configuration, mockHTTPClientProvider, timingProvider);
		mockBeaconStrict = MockIBeacon::createStrict();
		mockBeaconNice = MockIBeacon::createNice();
	}

	void TearDown()
	{
	}

	Session_sp createSessionWithNiceBeacon()
	{
		return std::make_shared<Session_t>(
			mockLogger,
			mockBeaconSender,
			mockBeaconNice
		);
	}

	Session_sp createSessionWithStrictBeacon()
 	{
		return std::make_shared<Session_t>(
			mockLogger,
			mockBeaconSender,
			mockBeaconStrict
		);
	}
};

TEST_F(SessionTest, startSessionCallsBeacon)
{
	// expect
	EXPECT_CALL(*mockBeaconStrict, startSession())
		.Times(1);
	EXPECT_CALL(*mockBeaconSender, startSession(testing::_))
		.Times(1);

	// given
	auto target = createSessionWithStrictBeacon();

	// when
	target->startSession();
}

TEST_F(SessionTest, constructorInitializesValidDefaults)
{
	// given, when
	auto target = createSessionWithStrictBeacon();

	// then
	ASSERT_EQ(target->getEndTime(), -1);
}

TEST_F(SessionTest, enterActionWithNullActionNameGivesNullRootActionObject)
{
	// expect
	EXPECT_CALL(*mockLogger, mockWarning("Session [sn=0] enterAction: actionName must not be null or empty"))
		.Times(1);

	// given
	auto target = createSessionWithNiceBeacon();

	// when
	auto obtained = target->enterAction(nullptr);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(std::dynamic_pointer_cast<NullRootAction_t>(obtained), testing::NotNull());
}

TEST_F(SessionTest, enterActionWithEmptyActionNameGivesNullRootActionObject)
{
	// expect
	EXPECT_CALL(*mockLogger, mockWarning("Session [sn=0] enterAction: actionName must not be null or empty"))
		.Times(1);

	// given
	auto target = createSessionWithNiceBeacon();

	// when
	auto obtained = target->enterAction("");

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(std::dynamic_pointer_cast<NullRootAction_t>(obtained), testing::NotNull());
}

TEST_F(SessionTest, enterActionWitnNonEmptyNameGivesRootAction)
{
	// given
	auto target = createSessionWithNiceBeacon();

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
	auto target = createSessionWithNiceBeacon();

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

TEST_F(SessionTest, enterActionWithDifferentNameGivesDifferentInstance)
{
	// given
	auto target = createSessionWithNiceBeacon();

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
	auto target = createSessionWithStrictBeacon();

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

TEST_F(SessionTest, identifyUserWithNullTagDoesNothing)
{
	// expect
	EXPECT_CALL(*mockLogger, mockWarning("Session [sn=0] identifyUser: userTag must not be null or empty"))
		.Times(1);
	EXPECT_CALL(*mockBeaconStrict, getSessionNumber())
		.Times(1);

	// given
	auto target = createSessionWithStrictBeacon();

	// when
	target->identifyUser(nullptr);
}

TEST_F(SessionTest, identifyUserWithEmptyTagDoesNothing)
{
	// expect
	EXPECT_CALL(*mockLogger, mockWarning("Session [sn=0] identifyUser: userTag must not be null or empty"))
		.Times(1);
	EXPECT_CALL(*mockBeaconStrict, getSessionNumber())
		.Times(1);

	// given
	auto target = createSessionWithStrictBeacon();

	// when
	target->identifyUser("");
}

TEST_F(SessionTest, identifyUserWithNonEmptyTagReportsUser)
{
	// with
	const char* userTag = "user";

	// expect
	EXPECT_CALL(*mockLogger, mockWarning(testing::_))
		.Times(0);
	EXPECT_CALL(*mockBeaconStrict, getSessionNumber())
		.Times(1); // Session toString on debug log
	EXPECT_CALL(*mockBeaconStrict, identifyUser(Utf8String_t(userTag)))
		.Times(1);

	// given
	auto target = createSessionWithStrictBeacon();

	// when
	target->identifyUser(userTag);
}

TEST_F(SessionTest, identifyUserMultipleTimesAlwaysCallsBeacon)
{
	// with
	const char* userTag = "user";

	// expect
	EXPECT_CALL(*mockLogger, mockWarning(testing::_))
		.Times(0);
	EXPECT_CALL(*mockBeaconStrict, getSessionNumber())
		.Times(2); // session toString in debug log
	EXPECT_CALL(*mockBeaconStrict, identifyUser(Utf8String_t(userTag)))
		.Times(2);

	// given
	auto target = createSessionWithStrictBeacon();

	// when
	target->identifyUser(userTag);
	target->identifyUser(userTag);
}

TEST_F(SessionTest, identifyUserWithDifferentUsersAlwasCallsBeacon)
{
	// with
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
	auto target = createSessionWithStrictBeacon();

	// when
	target->identifyUser(userTag1);
	target->identifyUser(userTag2);
}

TEST_F(SessionTest, reportingCrashWithNullErrorNameDoesNotReportAnything)
{
	// with
	const char* errorName = nullptr;
	const char* reason = "some reason";
	const char* stacktrace = "some stack trace";

	// expect
	EXPECT_CALL(*mockLogger, mockWarning("Session [sn=0] reportCrash: errorName must not be null or empty"))
		.Times(1);
	EXPECT_CALL(*mockBeaconStrict,getSessionNumber())
		.Times(1); // session to string in debug log

	// given
	auto target = createSessionWithStrictBeacon();

	// when
	target->reportCrash(errorName, reason, stacktrace);
}

TEST_F(SessionTest, reportingCrashWithEmptyErrorNameDoesNotReportAnything)
{
	// with
	const char* errorName = "";
	const char* reason = "some reason";
	const char* stacktrace = "some stack trace";

	// expect
	EXPECT_CALL(*mockLogger, mockWarning("Session [sn=0] reportCrash: errorName must not be null or empty"))
		.Times(1);
	EXPECT_CALL(*mockBeaconStrict,getSessionNumber())
		.Times(1); // session to string in debug log

	// given
	auto target = createSessionWithStrictBeacon();

	// when
	target->reportCrash(errorName, reason, stacktrace);
}

TEST_F(SessionTest, reportingCrashWithNullReasonAndStacktraceWorks)
{
	// with
	const char* errorName = "errorName";
	const char* reason = nullptr;
	const char* stacktrace = nullptr;

	// expect
	EXPECT_CALL(*mockBeaconNice, reportCrash(testing::Eq(errorName), testing::Eq(reason), testing::Eq(stacktrace)))
		.Times(1);

	// given
	auto target = createSessionWithNiceBeacon();

	// when
	target->reportCrash(errorName, reason, stacktrace);
}

TEST_F(SessionTest, reportingCrashWithEmptyReasonAndStacktraceStringWorks)
{
	// with
	const char* errorName = "errorName";
	const char* reason = "";
	const char* stacktrace = "";

	// expect
	EXPECT_CALL(*mockBeaconNice, reportCrash(testing::Eq(errorName), testing::Eq(reason), testing::Eq(stacktrace)))
		.Times(1);

	// given
	auto target = createSessionWithNiceBeacon();

	// when
	target->reportCrash(errorName, reason, stacktrace);
}

TEST_F(SessionTest, reportCrashWorks)
{
	// with
	const char* errorName = "errorName";
	const char* reason = "some reason";
	const char* stacktrace = "some stacktrace";

	// expect
	EXPECT_CALL(*mockBeaconNice, reportCrash(testing::Eq(errorName), testing::Eq(reason), testing::Eq(stacktrace)))
		.Times(1);

	// given
	auto target = createSessionWithNiceBeacon();

	// when
	target->reportCrash(errorName, reason, stacktrace);
}

TEST_F(SessionTest, reportingCrashWithSameDataMultipleTimesForwardsEachCallToBeacon)
{
	// with
	const char* errorName = "errorName";
	const char* reason = "some reason";
	const char* stacktrace = "some stacktrace";

	// expect
	EXPECT_CALL(*mockBeaconNice, reportCrash(testing::Eq(errorName), testing::Eq(reason), testing::Eq(stacktrace)))
		.Times(2);

	// given
	auto target = createSessionWithNiceBeacon();

	// when
	target->reportCrash(errorName, reason, stacktrace);
	target->reportCrash(errorName, reason, stacktrace);
}


TEST_F(SessionTest, reportingCrashWithDifferentDataMultipleTimesForwardsEachCallToBeacon)
{
	// with
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
	auto target = createSessionWithNiceBeacon();

	// when
	target->reportCrash(errorName1, reason1, stacktrace1);
	target->reportCrash(errorName2, reason2, stacktrace2);
}

TEST_F(SessionTest, endSessionFinishesSessionOnBeacon)
{
	// with
	const int64_t timestamp = 1234;
	ON_CALL(*mockBeaconNice, getCurrentTimestamp())
		.WillByDefault(testing::Return(timestamp));

	// expect
	EXPECT_CALL(*mockBeaconSender, finishSession(testing::_))
		.Times(1);
	EXPECT_CALL(*mockBeaconNice, endSession(testing::_))
		.Times(1);

	// given
	auto target = createSessionWithNiceBeacon();

	// when
	target->end();

	// then
	ASSERT_THAT(target->getEndTime(), testing::Eq(timestamp));
}

TEST_F(SessionTest, endingAnAlreadyEndedSessionDoesNothing)
{
	// with
	const int64_t timestamp = 1234;
	ON_CALL(*mockBeaconNice, getCurrentTimestamp())
		.WillByDefault(testing::Return(timestamp));

	// expect
	EXPECT_CALL(*mockBeaconSender, finishSession(testing::_))
		.Times(1); // only first invocation of end
	EXPECT_CALL(*mockBeaconNice, endSession(testing::_))
		.Times(1); // only first invocation of end

	// given
	auto target = createSessionWithNiceBeacon();

	// when
	target->end();

	// and when
	target->end();

	// then
	ASSERT_THAT(target->getEndTime(), testing::Eq(timestamp));
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
	EXPECT_CALL(*mockBeaconSender, finishSession(testing::_))
		.Times(1);

	// given
	auto target = createSessionWithNiceBeacon();
	target->storeChildInList(childObjectOne);
	target->storeChildInList(childObjectTwo);

	// when
	target->end();
}

TEST_F(SessionTest, sendBeaconForwardsCallToBeacon)
{
	// expect
	EXPECT_CALL(*mockBeaconStrict, send(testing::Eq(mockHTTPClientProvider)))
		.Times(1);

	// given
	auto target = createSessionWithStrictBeacon();

	// when
	target->sendBeacon(mockHTTPClientProvider);
}

TEST_F(SessionTest, clearCapturedDataForwardsCallToBeacon)
{
	// expect
	EXPECT_CALL(*mockBeaconStrict, clearData())
		.Times(1);

	// given
	auto target = createSessionWithStrictBeacon();

	// when
	target->clearCapturedData();
}

TEST_F(SessionTest, isEmptyForwardsCallToBeacon)
{
	// expect
	EXPECT_CALL(*mockBeaconStrict, isEmpty())
		.Times(1);

	// given
	auto target = createSessionWithStrictBeacon();

	// when
	target->isEmpty();
}

TEST_F(SessionTest, setBeaconConfigurationForwardsCallToBeacon)
{
	// expect
	EXPECT_CALL(*mockBeaconStrict, setBeaconConfiguration(testing::Eq(beaconConfiguration)))
		.Times(1);

	// given
	auto target = createSessionWithStrictBeacon();

	// when
	target->setBeaconConfiguration(beaconConfiguration);
}

TEST_F(SessionTest, getBeaconConfigurationForwardsCallToBeacon)
{
	// expect
	EXPECT_CALL(*mockBeaconStrict, getBeaconConfiguration())
		.Times(1);

	// given
	auto target = createSessionWithStrictBeacon();

	// when
	target->getBeaconConfiguration();
}

TEST_F(SessionTest, aNewlyConstructedSessionIsNotEnded)
{
	//given
	auto target = createSessionWithNiceBeacon();

	//when
	ASSERT_THAT(target->isSessionEnded(), testing::Eq(false));
}

TEST_F(SessionTest, aSessionIsEndedIfEndIsCalled)
{
	EXPECT_CALL(*mockBeaconSender, finishSession(testing::_))
		.Times(1);

	// given
	auto target = createSessionWithNiceBeacon();

	// when
	target->end();

	// then
	ASSERT_THAT(target->isSessionEnded(), testing::Eq(true));
}

TEST_F(SessionTest, enterActionGivesNullRootActionIfSessionIsAlreadyEnded)
{
	// expect
	EXPECT_CALL(*mockBeaconSender, finishSession(testing::_))
		.Times(1);

	// given
	auto target = createSessionWithNiceBeacon();
	target->end();

	// when
	auto obtained = target->enterAction("test");

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(std::dynamic_pointer_cast<NullRootAction_t>(obtained), testing::NotNull());
}

TEST_F(SessionTest, identifyUserDoesNothingIfSessionIsEnded)
{
	// expect
	EXPECT_CALL(*mockBeaconNice, identifyUser(testing::_))
		.Times(0);
	EXPECT_CALL(*mockBeaconSender, finishSession(testing::_))
		.Times(1);

	// given
	auto target = createSessionWithNiceBeacon();
	target->end();

	// when
	target->identifyUser("Jane Smith");
}

TEST_F(SessionTest, reportCrashDoesNothingIfSessionIsEnded)
{
	// expect
	EXPECT_CALL(*mockBeaconNice, reportCrash(testing::_, testing::_, testing::_))
		.Times(0);
	EXPECT_CALL(*mockBeaconSender, finishSession(testing::_))
		.Times(1);

	// given
	auto target = createSessionWithNiceBeacon();
	target->end();

	// when
	target->reportCrash("errorName", "reason", "stacktrace");
}

TEST_F(SessionTest, closeEndsTheSession)
{
	// with
	const int64_t timestamp = 1234;
	ON_CALL(*mockBeaconNice, getCurrentTimestamp())
		.WillByDefault(testing::Return(timestamp));

	// expect
	EXPECT_CALL(*mockBeaconNice, endSession(testing::_))
		.Times(1);
	EXPECT_CALL(*mockBeaconSender, finishSession(testing::_))
		.Times(1);

	// given
	auto target = createSessionWithNiceBeacon();

	// when
	target->close();
}

TEST_F(SessionTest, traceWebRequestWithValidUrlStringGivesAppropriateTracer)
{
	// with
	const char* url = "http://example.com/pages/";

	// given
	auto target = createSessionWithNiceBeacon();

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
	auto target = createSessionWithNiceBeacon();

	// when
	auto obtained = target->traceWebRequest(url);

	// then
	ASSERT_THAT(obtained, testing::NotNull());

	auto childObjects = target->getCopyOfChildObjects();
	ASSERT_THAT(childObjects.size(), testing::Eq(1));
	ASSERT_THAT(*childObjects.begin(), testing::Eq(std::dynamic_pointer_cast<IOpenKitObject_t>(obtained)));

	// break dependency cycle: tracer in child list of session
	target->removeChildFromList(std::dynamic_pointer_cast<IOpenKitObject_t>(obtained));
}

TEST_F(SessionTest, tracingANullStringWebRequestIsNotAllowed)
{
	// given
	const char* url = nullptr;
	auto target = createSessionWithNiceBeacon();

	// when
	auto obtained = target->traceWebRequest(url);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(std::dynamic_pointer_cast<NullWebRequestTracer_t>(obtained), testing::NotNull());
}

TEST_F(SessionTest, tracingAnEmptyStringWebRequestIsNotAllowed)
{
	// given
	const char* url = "";
	auto target = createSessionWithNiceBeacon();

	// when
	auto obtained = target->traceWebRequest(url);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(std::dynamic_pointer_cast<NullWebRequestTracer_t>(obtained), testing::NotNull());
}

TEST_F(SessionTest, tracingAnInvalidUrlSchemeIsNotAllowed)
{
	// given
	const char* url = "1337://fourtytwo.com";
	auto target = createSessionWithNiceBeacon();

	// when
	auto obtained = target->traceWebRequest(url);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(std::dynamic_pointer_cast<NullWebRequestTracer_t>(obtained), testing::NotNull());
}

TEST_F(SessionTest, traceWebRequestGivesNullTracerIfSessionIsEnded)
{
	// expect
	EXPECT_CALL(*mockBeaconSender, finishSession(testing::_))
		.Times(1);

	// given
	const char* url = "http://example.com/pages/";
	auto target = createSessionWithNiceBeacon();
	target->end();

	// when
	auto obtained = target->traceWebRequest(url);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(std::dynamic_pointer_cast<NullWebRequestTracer_t>(obtained), testing::NotNull());
}

TEST_F(SessionTest, onChildCloseRemovesChildFromList)
{
	// given
	const auto childObject = MockIOpenKitObject::createNice();
	auto target = createSessionWithNiceBeacon();
	target->storeChildInList(childObject);

	auto childObjects = target->getCopyOfChildObjects();
	ASSERT_THAT(childObjects.size(), testing::Eq(1));

	// when
	target->onChildClosed(childObject);

	// then
	childObjects = target->getCopyOfChildObjects();
	ASSERT_THAT(childObjects.size(), testing::Eq(0));

}
