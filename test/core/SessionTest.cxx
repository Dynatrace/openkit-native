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
#include "memory.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "caching/BeaconCache.h"
#include "core/util/DefaultLogger.h"
#include "providers/DefaultThreadIDProvider.h"
#include "providers/DefaultTimingProvider.h"
#include "protocol/ssl/SSLStrictTrustManager.h"
#include "providers/DefaultSessionIDProvider.h"
#include "providers/DefaultHTTPClientProvider.h"
#include "core/WebRequestTracerStringURL.h"
#include "core/NullWebRequestTracer.h"

#include "OpenKit/IRootAction.h"
#include "configuration/Configuration.h"
#include "configuration/BeaconCacheConfiguration.h"

#include "../protocol/MockHTTPClient.h"
#include "../protocol/MockBeacon.h"
#include "MockBeaconSender.h"
#include "../providers/MockHTTPClientProvider.h"


using namespace core;

static const char APP_ID[] = "appID";
static const char APP_NAME[] = "appName";

class SessionTest : public testing::Test
{
public:
	void SetUp()
	{
		logger = std::shared_ptr<openkit::ILogger>(new core::util::DefaultLogger(devNull, true));

		threadIDProvider = std::make_shared<providers::DefaultThreadIDProvider>();
		timingProvider = std::make_shared<providers::DefaultTimingProvider>();
		sessionIDProvider = std::make_shared<providers::DefaultSessionIDProvider>();

		std::shared_ptr<configuration::HTTPClientConfiguration> httpClientConfiguration = std::make_shared<configuration::HTTPClientConfiguration>(core::UTF8String(""), 0, core::UTF8String(""));
		mockHTTPClientProvider = std::make_shared<testing::NiceMock<test::MockHTTPClientProvider>>();
		mockHTTPClient = std::shared_ptr<testing::NiceMock<test::MockHTTPClient>>(new testing::NiceMock<test::MockHTTPClient>(httpClientConfiguration));

		trustManager = std::make_shared<protocol::SSLStrictTrustManager>();

		std::shared_ptr<configuration::Device> device = std::shared_ptr<configuration::Device>(new configuration::Device(core::UTF8String(""), core::UTF8String(""), core::UTF8String("")));

		beaconCacheConfiguration = std::make_shared<configuration::BeaconCacheConfiguration>(-1, -1, -1);
		beaconConfiguration = std::make_shared<configuration::BeaconConfiguration>();
		configuration = std::shared_ptr<configuration::Configuration>(new configuration::Configuration(device, configuration::OpenKitType::Type::DYNATRACE,
			core::UTF8String(APP_NAME), "", APP_ID, 0, "0", "",
			sessionIDProvider, trustManager, beaconCacheConfiguration, beaconConfiguration));
		configuration->enableCapture();

		beaconCache = std::make_shared<caching::BeaconCache>(logger);

		mockBeaconSender = std::make_shared<testing::StrictMock<test::MockBeaconSender>>(logger, configuration, mockHTTPClientProvider, timingProvider);
		mockBeaconStrict = std::make_shared<testing::StrictMock<test::MockBeacon>>(logger, beaconCache, configuration, nullptr, threadIDProvider, timingProvider);
		mockBeaconNice = std::make_shared<testing::NiceMock<test::MockBeacon>>(logger, beaconCache, configuration, nullptr, threadIDProvider, timingProvider);
	}

	void TearDown()
	{

	}
public:
	std::ostringstream devNull;
	std::shared_ptr<openkit::ILogger> logger;
	std::shared_ptr<providers::IThreadIDProvider> threadIDProvider;
	std::shared_ptr<providers::ITimingProvider> timingProvider;
	std::shared_ptr<providers::ISessionIDProvider> sessionIDProvider;

	std::shared_ptr<testing::NiceMock<test::MockHTTPClient>> mockHTTPClient;
	std::shared_ptr<openkit::ISSLTrustManager> trustManager;

	std::shared_ptr<configuration::BeaconConfiguration> beaconConfiguration;
	std::shared_ptr<configuration::BeaconCacheConfiguration> beaconCacheConfiguration;
	std::shared_ptr<configuration::Configuration> configuration;
	std::shared_ptr<caching::BeaconCache> beaconCache;

	std::shared_ptr<testing::StrictMock<test::MockBeaconSender>> mockBeaconSender;
	std::shared_ptr<testing::StrictMock<test::MockBeacon>> mockBeaconStrict;
	std::shared_ptr<testing::NiceMock<test::MockBeacon>> mockBeaconNice;
	std::shared_ptr<testing::NiceMock<test::MockHTTPClientProvider>> mockHTTPClientProvider;
};

TEST_F(SessionTest, constructorReturnsValidDefaults)
{
	// test the constructor call
	auto target = std::make_shared<core::Session>(logger, mockBeaconSender, mockBeaconNice);

	// verify default values
	ASSERT_EQ(target->getEndTime(), -1);
	ASSERT_TRUE(target->isEmpty());
}

TEST_F(SessionTest, enterActionWithNullActionName)
{
	// create test environment
	auto target = std::make_shared<core::Session>(logger, mockBeaconSender, mockBeaconNice);

	// add/enter "null-action"
	auto obtained = target->enterAction(nullptr);

	// we definitely got a NullRootAction instance
	ASSERT_NE(nullptr, obtained);
	ASSERT_NE(nullptr, std::dynamic_pointer_cast<core::NullRootAction>(obtained));
}

TEST_F(SessionTest, enterActionWithEmptyActionName)
{
	// create test environment
	auto target = std::make_shared<core::Session>(logger, mockBeaconSender, mockBeaconNice);

	// add/enter "null-action"
	auto obtained = target->enterAction("");

	// we definitely got a NullRootAction instance
	ASSERT_NE(nullptr, obtained);
	ASSERT_NE(nullptr, std::dynamic_pointer_cast<core::NullRootAction>(obtained));
}

TEST_F(SessionTest, enterNotClosedAction)
{
	// create test environment
	auto target = std::make_shared<core::Session>(logger, mockBeaconSender, mockBeaconNice);

	// add/enter one action
	auto obtained = target->enterAction("Some action");
	ASSERT_NE(nullptr, obtained);

	// verify that (because the actions is still active) it is not in the beacon cache (thus the cache is empty)
	ASSERT_TRUE(target->isEmpty());
}


TEST_F(SessionTest, enterSingleAction)
{
	// create test environment
	auto target = std::make_shared<core::Session>(logger, mockBeaconSender, mockBeaconNice);

	// add/enter one action
	auto rootAction = target->enterAction("some action");
	rootAction->leaveAction();

	// verify that the action is closed, thus moved to the beacon cache (thus the cache is no longer empty)
	ASSERT_FALSE(target->isEmpty());
}

TEST_F(SessionTest, enterMultipleActions)
{
	// create test environment
	auto target = std::make_shared<core::Session>(logger, mockBeaconSender, mockBeaconNice);

	// add/enter two actions
	auto rootAction1 = target->enterAction("some action 1");
	rootAction1->leaveAction();
	auto rootAction2 = target->enterAction("some action 2");
	rootAction2->leaveAction();

	// verify that the actions are closed, thus moved to the beacon cache (thus the cache is no longer empty)
	ASSERT_FALSE(target->isEmpty());
}


TEST_F(SessionTest, enterSameActions)
{
	// create test environment
	auto target = std::make_shared<core::Session>(logger, mockBeaconSender, mockBeaconNice);

	// add/enter two actions
	auto rootAction1 = target->enterAction("some action");
	rootAction1->leaveAction();
	auto rootAction2 = target->enterAction("some action");
	rootAction2->leaveAction();

	// verify that the actions are closed, thus moved to the beacon cache (thus the cache is no longer empty)
	ASSERT_FALSE(target->isEmpty());
	// verify that multiple actions with same name are possible
	ASSERT_NE(rootAction1, rootAction2);
}

TEST_F(SessionTest, identifyUserWithNullTagDoesNothing)
{
	// create test environment
	auto target = std::make_shared<core::Session>(logger, mockBeaconSender, mockBeaconStrict);

	// identify a "null-user" must be possible
	EXPECT_CALL(*mockBeaconStrict, identifyUser(testing::_))
		.Times(0);

	// verify the correct method is being called
	target->identifyUser(nullptr);
}

TEST_F(SessionTest, identifyUserWithEmptyTagDoesNothing)
{
	// create test environment
	auto target = std::make_shared<core::Session>(logger, mockBeaconSender, mockBeaconStrict);

	// identify a "null-user" must be possible
	EXPECT_CALL(*mockBeaconStrict, identifyUser(testing::_))
		.Times(0);

	// verify the correct methods being called
	target->identifyUser("");
}

TEST_F(SessionTest, identifySingleUser)
{
	// verify the correct methods being called
	EXPECT_CALL(*mockBeaconStrict, identifyUser(core::UTF8String("Some user")))
		.Times(1);
	EXPECT_CALL(*mockBeaconSender, startSession(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockBeaconStrict, startSession())
		.Times(testing::Exactly(1));

	// create test environment
	auto target = std::make_shared<core::Session>(logger, mockBeaconSender, mockBeaconStrict);
	target->startSession();
	// identify a single user
	target->identifyUser("Some user");
}

TEST_F(SessionTest, identifyMultipleUsers)
{
	// verify the correct methods being called
	EXPECT_CALL(*mockBeaconStrict, identifyUser(core::UTF8String("Some user")))
		.Times(1);
	EXPECT_CALL(*mockBeaconStrict, identifyUser(core::UTF8String("Some other user")))
		.Times(1);
	EXPECT_CALL(*mockBeaconStrict, identifyUser(core::UTF8String("Yet another user")))
		.Times(1);
	EXPECT_CALL(*mockBeaconSender, startSession(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockBeaconStrict, startSession())
		.Times(testing::Exactly(1));

	// create test environment
	auto target = std::make_shared<core::Session>(logger, mockBeaconSender, mockBeaconStrict);
	target->startSession();

	// identify multiple users
	target->identifyUser("Some user");
	target->identifyUser("Some other user");
	target->identifyUser("Yet another user");
}

TEST_F(SessionTest, identifySameUser)
{
	// verify the correct methods being called
	EXPECT_CALL(*mockBeaconStrict, identifyUser(core::UTF8String("Some user")))
		.Times(2);
	EXPECT_CALL(*mockBeaconSender, startSession(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockBeaconStrict, startSession())
		.Times(testing::Exactly(1));

	// create test environment
	auto target = std::make_shared<core::Session>(logger, mockBeaconSender, mockBeaconStrict);
	target->startSession();

	// identify two identical users
	target->identifyUser("Some user");
	target->identifyUser("Some user");
}

TEST_F(SessionTest, reportingCrashWithNullErrorNameDoesNotReportAnything)
{
	// verify the correct methods being called
	EXPECT_CALL(*mockBeaconStrict, reportCrash(testing::_, testing::_, testing::_))
		.Times(0);

	// create test environment
	auto target = std::make_shared<core::Session>(logger, mockBeaconSender, mockBeaconStrict);

	// report a crash, passing null values
	target->reportCrash(nullptr, "some reason", "some stack trace");
}

TEST_F(SessionTest, reportingCrashWithEmptyErrorNameDoesNotReportAnything)
{
	// verify the correct methods being called
	EXPECT_CALL(*mockBeaconStrict, reportCrash(testing::_, testing::_, testing::_))
		.Times(0);

	// create test environment
	auto target = std::make_shared<core::Session>(logger, mockBeaconSender, mockBeaconStrict);

	// report a crash, passing null values
	target->reportCrash("", "some reason", "some stack trace");
}

TEST_F(SessionTest, reportingCrashWithNullReasonAndStacktraceWorks)
{
	// verify the correct methods being called
	EXPECT_CALL(*mockBeaconStrict, reportCrash(testing::_, testing::_, testing::_))
		.Times(1);

	// create test environment
	auto target = std::make_shared<core::Session>(logger, mockBeaconSender, mockBeaconStrict);

	// report a crash, passing null values
	target->reportCrash("errorName", nullptr, nullptr);
}

TEST_F(SessionTest, reportSingleCrash)
{
	// verify the correct methods being called
	EXPECT_CALL(*mockBeaconStrict, reportCrash(testing::_, testing::_, testing::_))
		.Times(1);
	EXPECT_CALL(*mockBeaconSender, startSession(testing::_))
		.Times(1);
	EXPECT_CALL(*mockBeaconStrict, startSession())
		.Times(testing::Exactly(1));

	// create test environment
	auto target = std::make_shared<core::Session>(logger, mockBeaconSender, mockBeaconStrict);
	target->startSession();

	// report a single crash
	target->reportCrash("error name", "error reason", "the stacktrace causing the error");
}

TEST_F(SessionTest, reportMultipleCrashes)
{
	// verify the correct methods being called
	EXPECT_CALL(*mockBeaconStrict, reportCrash(testing::_, testing::_, testing::_))
		.Times(2);
	EXPECT_CALL(*mockBeaconSender, startSession(testing::_))
		.Times(1);
	EXPECT_CALL(*mockBeaconStrict, startSession())
		.Times(testing::Exactly(1));

	// create test environment
	auto target = std::make_shared<core::Session>(logger, mockBeaconSender, mockBeaconStrict);
	target->startSession();

	// report multiple crashes
	target->reportCrash("error name 1", "error reason 1", "the stacktrace causing the error 1");
	target->reportCrash("error name 2", "error reason 2", "the stacktrace causing the error 2");
}

TEST_F(SessionTest, reportSameCrash)
{
	// verify the correct methods being called
	EXPECT_CALL(*mockBeaconStrict, reportCrash(testing::_, testing::_, testing::_))
		.Times(2);
	EXPECT_CALL(*mockBeaconSender, startSession(testing::_))
		.Times(1);
	EXPECT_CALL(*mockBeaconStrict, startSession())
		.Times(testing::Exactly(1));

	// create test environment
	auto target = std::make_shared<core::Session>(logger, mockBeaconSender, mockBeaconStrict);
	target->startSession();

	// report multiple crashes
	target->reportCrash("error name", "error reason", "the stacktrace causing the error");
	target->reportCrash("error name", "error reason", "the stacktrace causing the error");
}

TEST_F(SessionTest, endSession)
{
	// verify the correct methods being called
	EXPECT_CALL(*mockBeaconSender, startSession(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockBeaconStrict, getCurrentTimestamp())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockBeaconStrict, startSession())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockBeaconStrict, endSession(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockBeaconSender, finishSession(testing::_))
		.Times(testing::Exactly(1));

	// create test environment
	auto target = std::make_shared<core::Session>(logger, mockBeaconSender, mockBeaconStrict);
	target->startSession();

	//end the session
	target->end();
	ASSERT_NE(target->getEndTime(), -1);
}

TEST_F(SessionTest, endSessionTwice)
{
	// verify the correct methods being called
	EXPECT_CALL(*mockBeaconSender, startSession(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockBeaconStrict, getCurrentTimestamp())
		.Times(testing::Exactly(2));
	EXPECT_CALL(*mockBeaconStrict, startSession())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockBeaconStrict, endSession(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockBeaconSender, finishSession(testing::_))
		.Times(testing::Exactly(1));

	// create test environment
	auto target = std::make_shared<core::Session>(logger, mockBeaconSender, mockBeaconStrict);
	target->startSession();

	//try to end the same session twice
	target->end();
	target->end();

	//check correct session end time
	ASSERT_NE(target->getEndTime(), -1);
}

TEST_F(SessionTest, endSessionWithOpenRootActions)
{
	// set mock behavior of HTTPClient and HTTPClientProvider
	protocol::StatusResponse* response = new protocol::StatusResponse(logger, core::UTF8String(""), 200, protocol::Response::ResponseHeaders());

	ON_CALL(*mockHTTPClientProvider, createClient(testing::_, testing::_))
		.WillByDefault(testing::Return(mockHTTPClient));
	// mock a valid status response via the HTTPClient to be sure the beacon cache is empty
	ON_CALL(*mockHTTPClient, sendBeaconRequestRawPtrProxy(testing::_, testing::_))
		.WillByDefault(testing::Return(response));
	// call the real send method to ensure correct interaction with the beacon cache
	ON_CALL(*mockBeaconStrict, send(testing::_))
		.WillByDefault(testing::WithArgs<0>(testing::Invoke(&*mockBeaconStrict, &test::MockBeacon::RealSend)));

	// verify the proper methods being called
	EXPECT_CALL(*mockBeaconStrict, createSequenceNumber())
		.Times(testing::Exactly(4));
	EXPECT_CALL(*mockBeaconSender, startSession(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockBeaconStrict, getCurrentTimestamp())
		.Times(testing::Exactly(7));
	EXPECT_CALL(*mockBeaconStrict, startSession())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockBeaconStrict, endSession(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockBeaconStrict, mockAddAction(testing::Matcher<std::shared_ptr<core::RootAction>>(testing::_)))
		.Times(testing::Exactly(2)); // via calling end and delegating to end of on both entered root actions
	EXPECT_CALL(*mockBeaconSender, finishSession(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockBeaconStrict, send(testing::_))
		.Times(testing::Exactly(1));


	// create test environment
	auto target = std::make_shared<core::Session>(logger, mockBeaconSender, mockBeaconStrict);
	target->startSession();

	// end the session containing open (=not left) actions
	target->enterAction("Some action 1");
	target->enterAction("Some action 2");
	target->end();

	target->sendBeacon(mockHTTPClientProvider);
	// verify that the actions if the action is still active, it is not in the beacon cache (thus cache is empty)
	ASSERT_TRUE(target->isEmpty());
}

TEST_F(SessionTest, sendBeacon)
{
	// verify the proper methods being called
	EXPECT_CALL(*mockBeaconSender, startSession(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockBeaconNice, send(testing::_))
		.Times(testing::Exactly(1));

	// create test environment
	auto target = std::make_shared<core::Session>(logger, mockBeaconSender, mockBeaconNice);
	target->startSession();

	//when
	target->sendBeacon(mockHTTPClientProvider);
}

TEST_F(SessionTest, clearCapturedData)
{
	//check that finishSession is called
	EXPECT_CALL(*mockBeaconSender, startSession(testing::_))
		.Times(testing::Exactly(1));

	// create test environment
	auto target = std::make_shared<core::Session>(logger, mockBeaconSender, mockBeaconNice);
	target->startSession();

	// end the session containing closed actions (moved to the beacon cache)
	auto rootAction1 = target->enterAction("Some action 1");
	rootAction1->leaveAction();
	auto rootAction2 = target->enterAction("Some action 2");
	rootAction2->leaveAction();

	// verify that the actions are closed, thus moved to the beacon cache (thus the cache is no longer empty)
	ASSERT_FALSE(target->isEmpty());

	// clear the captured data
	target->clearCapturedData();

	// verify that the cached items are cleared and the cache is empty
	ASSERT_TRUE(target->isEmpty());
}

TEST_F(SessionTest, aNewlyConstructedSessionIsNotEnded)
{
	//check that finishSession is called
	EXPECT_CALL(*mockBeaconSender, startSession(testing::_))
		.Times(testing::Exactly(1));

	//given
	auto target = std::make_shared<core::Session>(logger, mockBeaconSender, mockBeaconNice);
	target->startSession();

	//when, then
	ASSERT_FALSE(target->isSessionEnded());
}

TEST_F(SessionTest, aSessionIsEndedIfEndIsCalled)
{
	//check that finishSession is called
	EXPECT_CALL(*mockBeaconSender, startSession(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockBeaconSender, finishSession(testing::_))
		.Times(testing::Exactly(1));

	//given
	auto target = std::make_shared<core::Session>(logger, mockBeaconSender, mockBeaconNice);
	target->startSession();
	target->end();

	// then session is ended
	ASSERT_TRUE(target->isSessionEnded());
}

TEST_F(SessionTest, enterActionGivesNullRootActionIfSessionIsAlreadyEnded)
{
	//check that finishSession is called
	EXPECT_CALL(*mockBeaconSender, startSession(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockBeaconSender, finishSession(testing::_))
		.Times(testing::Exactly(1));

	// given
	auto target = std::make_shared<core::Session>(logger, mockBeaconSender, mockBeaconNice);
	target->startSession();
	target->end();

	// when entering an action on already ended session
	auto obtained = target->enterAction("Test");

	// then
	ASSERT_NE(nullptr, obtained);
	ASSERT_NE(nullptr, std::dynamic_pointer_cast<core::NullRootAction>(obtained));
}

TEST_F(SessionTest, identifyUserDoesNothingIfSessionIsEnded)
{
	EXPECT_CALL(*mockBeaconSender, startSession(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockBeaconSender, finishSession(testing::_))
		.Times(testing::Exactly(1));
	//given
	auto target = std::make_shared<core::Session>(logger, mockBeaconSender, mockBeaconNice);
	target->startSession();
	target->end();

	// when trying to identify a user on an ended session
	target->identifyUser("Jane Smith");

	// then
	ASSERT_TRUE(target->isEmpty());
}

TEST_F(SessionTest, reportCrashDoesNothingIfSessionIsEnded)
{
	EXPECT_CALL(*mockBeaconSender, startSession(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockBeaconSender, finishSession(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockBeaconNice, startSession())
		.Times(testing::Exactly(1));

	//given
	auto target = std::make_shared<core::Session>(logger, mockBeaconSender, mockBeaconNice);
	target->startSession();
	target->end();

	// when trying to identify a user on an ended session
	target->reportCrash("errorName", "reason", "stacktrace");

	//then
	ASSERT_TRUE(target->isEmpty());
}

TEST_F(SessionTest, traceWebRequestWithValidUrlStringGivesAppropriateTracer)
{
	// given
	EXPECT_CALL(*mockBeaconSender, startSession(testing::_))
		.Times(testing::Exactly(1));

	const char* url = "http://example.com/pages/";
	auto target = std::make_shared<core::Session>(logger, mockBeaconSender, mockBeaconNice);
	target->startSession();

	// when
	auto obtained = target->traceWebRequest(url);

	// then
	ASSERT_NE(nullptr, obtained);
	ASSERT_NE(nullptr, std::dynamic_pointer_cast<core::WebRequestTracerStringURL>(obtained));

	auto webRequestTracer = std::dynamic_pointer_cast<core::WebRequestTracerStringURL>(obtained);
	EXPECT_TRUE(webRequestTracer->getURL().equals(url));
}

TEST_F(SessionTest, tracingANullStringWebRequestIsNotAllowed)
{
	// given
	EXPECT_CALL(*mockBeaconSender, startSession(testing::_))
		.Times(testing::Exactly(1));

	auto target = std::make_shared<core::Session>(logger, mockBeaconSender, mockBeaconNice);
	target->startSession();

	// when
	auto obtained = target->traceWebRequest(nullptr);

	// then
	ASSERT_NE(nullptr, obtained);
	ASSERT_NE(nullptr, std::dynamic_pointer_cast<core::NullWebRequestTracer>(obtained));
}

TEST_F(SessionTest, tracingAnEmptyStringWebRequestIsNotAllowed)
{
	// given
	EXPECT_CALL(*mockBeaconSender, startSession(testing::_))
		.Times(testing::Exactly(1));

	auto target = std::make_shared<core::Session>(logger, mockBeaconSender, mockBeaconNice);
	target->startSession();

	// when
	auto obtained = target->traceWebRequest("");

	// then
	ASSERT_NE(nullptr, obtained);
	ASSERT_NE(nullptr, std::dynamic_pointer_cast<core::NullWebRequestTracer>(obtained));
}

TEST_F(SessionTest, tracingAnInvalidUrlSchemeIsNotAllowed)
{
	// given
	EXPECT_CALL(*mockBeaconSender, startSession(testing::_))
		.Times(testing::Exactly(1));

	auto target = std::make_shared<core::Session>(logger, mockBeaconSender, mockBeaconNice);
	target->startSession();

	// when
	auto obtained = target->traceWebRequest("1337://fourtytwo.com");

	// then
	ASSERT_NE(nullptr, obtained);
	ASSERT_NE(nullptr, std::dynamic_pointer_cast<core::NullWebRequestTracer>(obtained));
}

TEST_F(SessionTest, traceWebRequestGivesNullTracerIfSessionIsEnded)
{
	// given
	EXPECT_CALL(*mockBeaconSender, startSession(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockBeaconSender, finishSession(testing::_))
		.Times(testing::Exactly(1));

	const char* url = "http://example.com/pages/";
	auto target = std::make_shared<core::Session>(logger, mockBeaconSender, mockBeaconNice);
	target->startSession();
	target->end();

	// when
	auto obtained = target->traceWebRequest(url);

	// then
	ASSERT_NE(nullptr, obtained);
	ASSERT_NE(nullptr, std::dynamic_pointer_cast<core::NullWebRequestTracer>(obtained));
}
