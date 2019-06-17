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
#include "core/BeaconSender.h"
#include "core/WebRequestTracerStringURL.h"
#include "core/RootAction.h"
#include "configuration/Configuration.h"

#include "../protocol/MockBeacon.h"
#include "../protocol/MockHTTPClient.h"
#include "../providers/MockHTTPClientProvider.h"

using namespace core;

static const char APP_ID[] = "appID";
static const char APP_NAME[] = "appName";

class RootActionTest : public testing::Test
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

		beaconConfiguration = std::make_shared<configuration::BeaconConfiguration>();
		beaconCacheConfiguration = std::make_shared<configuration::BeaconCacheConfiguration>(-1, -1, -1);
		configuration = std::shared_ptr<configuration::Configuration>(new configuration::Configuration(device, configuration::OpenKitType::Type::DYNATRACE,
			core::UTF8String(APP_NAME), "", APP_ID, 0,"0", "",
			sessionIDProvider, trustManager, beaconCacheConfiguration, beaconConfiguration));
		configuration->enableCapture();

		beaconCache = std::make_shared<caching::BeaconCache>(logger);

		beaconSender = std::make_shared<core::BeaconSender>(logger, configuration, mockHTTPClientProvider, timingProvider);
		mockBeacon = std::make_shared<testing::NiceMock<test::MockBeacon>>(logger, beaconCache, configuration, nullptr, threadIDProvider, timingProvider);

		session = std::make_shared<core::Session>(logger, beaconSender, mockBeacon);
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

	std::shared_ptr<core::BeaconSender> beaconSender;
	std::shared_ptr<testing::NiceMock<test::MockBeacon>> mockBeacon;
	std::shared_ptr<testing::NiceMock<test::MockHTTPClientProvider>> mockHTTPClientProvider;

	std::shared_ptr<core::Session> session;
};

TEST_F(RootActionTest, enterActionWithNullNameGivesNullAction)
{
	// create test environment
	// create root action with a null child action (must be valid)
	auto testRootAction = std::make_shared<core::RootAction>(logger, mockBeacon, core::UTF8String("test root action"), session);

	//when
	auto childAction = testRootAction->enterAction(nullptr);

	ASSERT_TRUE(childAction != nullptr);
	std::shared_ptr<core::NullAction> typeCast = std::dynamic_pointer_cast<core::NullAction>(childAction);
	ASSERT_TRUE(typeCast != nullptr);
}

TEST_F(RootActionTest, enterActionWithEmptyNameGivesNullAction)
{
	// create test environment
	// create root action with a null child action (must be valid)
	auto testRootAction = std::make_shared<core::RootAction>(logger, mockBeacon, core::UTF8String("test root action"), session);

	//when
	auto childAction = testRootAction->enterAction("");

	ASSERT_TRUE(childAction != nullptr);
	std::shared_ptr<core::NullAction> typeCast = std::dynamic_pointer_cast<core::NullAction>(childAction);
	ASSERT_TRUE(typeCast != nullptr);
}

TEST_F(RootActionTest, enterAndLeaveActions)
{
	// given: create root action with child action
	const char* rootActionName = "test root action";
	auto testRootAction = std::make_shared<core::RootAction>(logger, mockBeacon, core::UTF8String(rootActionName), session);
	const char* childActionName = "test child action";
	auto childAction = testRootAction->enterAction(childActionName);

	//verify
	ASSERT_TRUE(testRootAction->getName().equals(core::UTF8String(rootActionName)));
	std::shared_ptr<core::Action> childActionCast = std::static_pointer_cast<core::Action>(childAction);
	ASSERT_TRUE(childActionCast->getName().equals(core::UTF8String(childActionName)));

	// child leaves
	childAction->leaveAction();

	//parent leaves
	testRootAction->leaveAction();

	// verify that open child actions are now empty
	ASSERT_FALSE(testRootAction->hasOpenChildActions());
}

TEST_F(RootActionTest, enterAndLeaveActionsWithMultipleChildren)
{
	// given: create root action with child action
	const char* rootActionName = "test root action";
	auto testRootAction = std::make_shared<core::RootAction>(logger, mockBeacon, core::UTF8String(rootActionName), session);
	const char* childActionName1 = "first test child action";
	const char* childActionName2 = "second test child action";
	auto childActionOne = testRootAction->enterAction(childActionName1);
	auto childActionTwo = testRootAction->enterAction(childActionName2);

	//verify
	ASSERT_TRUE(testRootAction->getName().equals(core::UTF8String(rootActionName)));
	std::shared_ptr<core::Action> childActionCast1 = std::static_pointer_cast<core::Action>(childActionOne);
	ASSERT_TRUE(childActionCast1->getName().equals(core::UTF8String(childActionName1)));
	std::shared_ptr<core::Action> childActionCast2 = std::static_pointer_cast<core::Action>(childActionTwo);
	ASSERT_TRUE(childActionCast2->getName().equals(core::UTF8String(childActionName2)));

	// child leaves
	childActionOne->leaveAction();
	childActionTwo->leaveAction();

	//parent leaves
	testRootAction->leaveAction();

	// verify that open child actions are now empty
	ASSERT_FALSE(testRootAction->hasOpenChildActions());
}

TEST_F(RootActionTest, enterActionGivesNullActionIfAlreadyLeft)
{
	const char* rootActionName = "test root action";
	auto testRootAction = std::make_shared<core::RootAction>(logger, mockBeacon, core::UTF8String(rootActionName), session);

	testRootAction->leaveAction();

	auto obtained = testRootAction->enterAction("open another action");

	ASSERT_TRUE(obtained != nullptr);
	std::shared_ptr<core::NullAction> typeCast = std::dynamic_pointer_cast<core::NullAction>(obtained);
	ASSERT_TRUE(typeCast != nullptr);
}

TEST_F(RootActionTest, reportEvent)
{
	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<core::RootAction>(logger, mockBeacon, core::UTF8String("test action"), session);

	//when
	const char* eventName = "TestEvent";
	auto returnedAction = testAction->reportEvent(eventName);

	ASSERT_EQ(testAction, returnedAction);


}

TEST_F(RootActionTest, reportEventDoesNothingIfEventNameIsNull)
{
	//verify the following calls
	EXPECT_CALL(*mockBeacon, reportEvent(testing::_, testing::_))
		.Times(testing::Exactly(0));

	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<core::RootAction>(logger, mockBeacon, core::UTF8String("test action"), session);

	//when
	auto returnedAction = testAction->reportEvent(nullptr);

	ASSERT_EQ(testAction, returnedAction);
}

TEST_F(RootActionTest, reportEventDoesNothingIfEventNameIsEmpty)
{
	//verify the following calls
	EXPECT_CALL(*mockBeacon, reportEvent(testing::_, testing::_))
		.Times(testing::Exactly(0));

	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<core::RootAction>(logger, mockBeacon, core::UTF8String("test action"), session);

	//when
	auto returnedAction = testAction->reportEvent("");

	ASSERT_EQ(testAction, returnedAction);
}

TEST_F(RootActionTest, reportValueIntWithNullNameDoesNotReportValue)
{
	//verify the following calls
	EXPECT_CALL(*mockBeacon, reportValueInt32(testing::_, testing::_, testing::_))
		.Times(testing::Exactly(0));

	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<core::RootAction>(logger, mockBeacon, core::UTF8String("test action"), session);

	//when
	auto returnedAction = testAction->reportValue(nullptr, 42);

	ASSERT_EQ(testAction, returnedAction);
}

TEST_F(RootActionTest, reportValueIntWithEmptyNameDoesNotReportValue)
{
	//verify the following calls
	EXPECT_CALL(*mockBeacon, reportValueInt32(testing::_, testing::_, testing::_))
		.Times(testing::Exactly(0));

	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<core::RootAction>(logger, mockBeacon, core::UTF8String("test action"), session);

	//when
	auto returnedAction = testAction->reportValue("", 42);

	ASSERT_EQ(testAction, returnedAction);
}

TEST_F(RootActionTest, reportValueIntWithValidValue)
{
	const char* integerValueName = "IntegerValue";

	//verify the following calls
	EXPECT_CALL(*mockBeacon, reportValueInt32(testing::_, core::UTF8String(integerValueName), 42))
		.Times(testing::Exactly(1));

	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<core::RootAction>(logger, mockBeacon, core::UTF8String("test action"), session);

	//when
	auto returnedAction = testAction->reportValue(integerValueName, 42);

	ASSERT_EQ(testAction, returnedAction);
}

TEST_F(RootActionTest, reportValueDoubleWithNullNameDoesNotReportValue)
{
	//verify the following calls
	EXPECT_CALL(*mockBeacon, reportValueDouble(testing::_, testing::_, testing::_))
		.Times(testing::Exactly(0));

	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<core::RootAction>(logger, mockBeacon, core::UTF8String("test action"), session);

	//when
	auto returnedAction = testAction->reportValue(nullptr, 42.1337);

	ASSERT_EQ(testAction, returnedAction);
}

TEST_F(RootActionTest, reportValueDoubleWithEmptyNameDoesNotReportValue)
{
	//verify the following calls
	EXPECT_CALL(*mockBeacon, reportValueDouble(testing::_, testing::_, testing::_))
		.Times(testing::Exactly(0));

	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<core::RootAction>(logger, mockBeacon, core::UTF8String("test action"), session);

	//when
	auto returnedAction = testAction->reportValue("", 42.1337);

	ASSERT_EQ(testAction, returnedAction);
}

TEST_F(RootActionTest, reportValueDoubleWithValidValue)
{
	//verify the following calls
	EXPECT_CALL(*mockBeacon, reportValueDouble(testing::_, testing::_, 42.1337))
		.Times(testing::Exactly(1));

	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<core::RootAction>(logger, mockBeacon, core::UTF8String("test action"), session);

	//when
	auto returnedAction = testAction->reportValue("DoubleValue", 42.1337);

	ASSERT_EQ(testAction, returnedAction);
}

TEST_F(RootActionTest, reportValueStringWithValidValue)
{
	//verify the following calls
	EXPECT_CALL(*mockBeacon, reportValueString(testing::_, testing::_, core::UTF8String("This is a string")))
		.Times(testing::Exactly(1));

	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<core::RootAction>(logger, mockBeacon, core::UTF8String("test action"), session);

	//when
	auto returnedAction = testAction->reportValue("StringValue", "This is a string");

	ASSERT_EQ(testAction, returnedAction);
}

TEST_F(RootActionTest, reportValueStringWithValueNull)
{
	//verify the following calls
	EXPECT_CALL(*mockBeacon, reportValueString(testing::_, testing::_, core::UTF8String("")))
		.Times(testing::Exactly(1));

	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<core::RootAction>(logger, mockBeacon, core::UTF8String("test action"), session);

	//when
	auto returnedAction = testAction->reportValue("StringValue", nullptr);

	ASSERT_EQ(testAction, returnedAction);
}

TEST_F(RootActionTest, reportErrorWithAllValuesSet)
{
	//verify the following calls
	EXPECT_CALL(*mockBeacon, reportError(testing::_, core::UTF8String("FATAL Error"), 0x8005037, core::UTF8String("Some reason for this fatal error")))
		.Times(testing::Exactly(1));

	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<core::RootAction>(logger, mockBeacon, core::UTF8String("test action"), session);

	//when
	auto returnedAction = testAction->reportError("FATAL Error", 0x8005037, "Some reason for this fatal error");

	ASSERT_EQ(testAction, returnedAction);
}

TEST_F(RootActionTest, reportErrorWithNullErrorNameDoesNotReportTheError)
{
	//verify the following calls
	EXPECT_CALL(*mockBeacon, reportError(testing::_, testing::_, testing::_, testing::_))
		.Times(testing::Exactly(0));

	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<core::RootAction>(logger, mockBeacon, core::UTF8String("test action"), session);

	//when
	auto returnedAction = testAction->reportError(nullptr, 0x8005037, "Some reason for this fatal error");

	ASSERT_EQ(testAction, returnedAction);
}

TEST_F(RootActionTest, reportErrorWithEmptyErrorNameDoesNotReportTheError)
{
	//verify the following calls
	EXPECT_CALL(*mockBeacon, reportError(testing::_, testing::_, testing::_, testing::_))
		.Times(testing::Exactly(0));

	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<core::RootAction>(logger, mockBeacon, core::UTF8String("test action"), session);

	//when
	auto returnedAction = testAction->reportError("", 0x8005037, "Some reason for this fatal error");

	ASSERT_EQ(testAction, returnedAction);
}

TEST_F(RootActionTest, reportErrorWithEmptyNullErrorReasonDoesReport)
{
	//verify the following calls
	EXPECT_CALL(*mockBeacon, reportError(testing::_, core::UTF8String("FATAL ERROR"), 0x8005037, core::UTF8String("")))
		.Times(testing::Exactly(1));

	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<core::RootAction>(logger, mockBeacon, core::UTF8String("test action"), session);

	//when
	auto returnedAction = testAction->reportError("FATAL ERROR", 0x8005037, nullptr);

	ASSERT_EQ(testAction, returnedAction);
}

TEST_F(RootActionTest, canTraceWebRequestUrl)
{
	core::UTF8String urlStr("http://example.com/pages/");
	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<core::Action>(logger, mockBeacon, core::UTF8String("test action"));

	// execute the test call
	auto webRequestTracer = testAction->traceWebRequest(urlStr.getStringData().c_str());

	// verify the returned request
	std::shared_ptr<core::WebRequestTracerStringURL> webRequestTracerStringURL = std::static_pointer_cast<core::WebRequestTracerStringURL>(webRequestTracer);
	EXPECT_TRUE(webRequestTracerStringURL->getURL().equals(urlStr));
}

TEST_F(RootActionTest, canTraceWebRequestUrlWithParameters)
{
	core::UTF8String urlStr("http://example.com/pages/");
	core::UTF8String paramString("someParameter=hello&someOtherParameter=world");

	core::UTF8String urlWithParamString(urlStr);
	urlWithParamString.concatenate("?");
	urlWithParamString.concatenate(paramString);

	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<core::Action>(logger, mockBeacon, core::UTF8String("test action"));

	// execute the test call
	auto webRequestTracer = testAction->traceWebRequest(urlWithParamString.getStringData().c_str());

	// verify the returned request
	std::shared_ptr<core::WebRequestTracerStringURL> webRequestTracerStringURL = std::static_pointer_cast<core::WebRequestTracerStringURL>(webRequestTracer);
	EXPECT_TRUE(webRequestTracerStringURL->getURL().equals(urlStr));
}

TEST_F(RootActionTest, tracingANullStringWebRequestIsNotAllowed)
{
	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<core::Action>(logger, mockBeacon, core::UTF8String("test action"));

	// execute the test call
	auto webRequestTracer = testAction->traceWebRequest(nullptr);

	// verify the returned request
	ASSERT_TRUE(webRequestTracer != nullptr);
	std::shared_ptr<core::NullWebRequestTracer> typeCast = std::dynamic_pointer_cast<core::NullWebRequestTracer>(webRequestTracer);
	ASSERT_TRUE(typeCast != nullptr);
}

TEST_F(RootActionTest, tracingAnEmptyStringWebRequestIsNotAllowed)
{
	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<core::Action>(logger, mockBeacon, core::UTF8String("test action"));

	// execute the test call
	auto webRequestTracer = testAction->traceWebRequest("");

	// verify the returned request
	ASSERT_TRUE(webRequestTracer != nullptr);
	std::shared_ptr<core::NullWebRequestTracer> typeCast = std::dynamic_pointer_cast<core::NullWebRequestTracer>(webRequestTracer);
	ASSERT_TRUE(typeCast != nullptr);
}


TEST_F(RootActionTest, leaveAction)
{
	EXPECT_CALL(*mockBeacon, createSequenceNumber())
		.WillOnce(testing::Return((int32_t)1))
		.WillRepeatedly(testing::Return((int32_t)2));
	EXPECT_CALL(*mockBeacon, getCurrentTimestamp())
		.WillOnce(testing::Return((int32_t)42))
		.WillRepeatedly(testing::Return((int32_t)48));
	auto testAction = std::make_shared<core::RootAction>(logger, mockBeacon, core::UTF8String("test action"), session);
	// execute the test call: simulate a few reportValues and then leaveAction
	ASSERT_EQ(testAction->getStartTime(), (int64_t)42);
	ASSERT_EQ(testAction->getEndTime(), (int64_t)-1);
	ASSERT_EQ(testAction->getStartSequenceNo(), 1);
	ASSERT_EQ(testAction->getEndSequenceNo(), -1);
	testAction->reportValue("DoubleValue", 3.141592654);
	testAction->reportValue("IntValue", 42);
	testAction->reportValue("StringValue", "nice value!");
	testAction->leaveAction();

	// verify
	ASSERT_EQ(testAction->getStartTime(), (int64_t)42);
	ASSERT_EQ(testAction->getEndTime(), (int64_t)48);
	ASSERT_EQ(testAction->getStartSequenceNo(), 1);
	ASSERT_EQ(testAction->getEndSequenceNo(), 2);
}

TEST_F(RootActionTest, leaveActionTwice)
{
	auto testAction = std::make_shared<core::RootAction>(logger, mockBeacon, core::UTF8String("test action"), session);
	// execute the test call: simulate a few reportValues and then leaveAction
	testAction->reportValue("DoubleValue", 3.141592654);
	testAction->reportValue("IntValue", 42);
	testAction->reportValue("StringValue", "nice value!");
	testAction->leaveAction();
	testAction->leaveAction();
}

TEST_F(RootActionTest, verifySequenceNumbersParents)
{
	EXPECT_CALL(*mockBeacon, createSequenceNumber())
		.WillOnce(testing::Return((int32_t)1))
		.WillOnce(testing::Return((int32_t)2))
		.WillOnce(testing::Return((int32_t)3))
		.WillRepeatedly(testing::Return((int32_t)4));

	//create two actions
	auto testAction1 = std::make_shared<core::RootAction>(logger, mockBeacon, core::UTF8String("test action 1"), session);
	ASSERT_EQ(testAction1->getStartSequenceNo(), 1);
	ASSERT_EQ(testAction1->getEndSequenceNo(), -1);
	auto testAction2 = std::make_shared<core::RootAction>(logger, mockBeacon, core::UTF8String("test action 2"), session);
	ASSERT_EQ(testAction2->getStartSequenceNo(), 2);
	ASSERT_EQ(testAction2->getEndSequenceNo(), -1);

	// leave parents (parent1 leaves first) -> this shall set the end sequence numbers
	testAction1->leaveAction();
	ASSERT_EQ(testAction1->getStartSequenceNo(), 1);
	ASSERT_EQ(testAction1->getEndSequenceNo(), 3);
	testAction2->leaveAction();
	ASSERT_EQ(testAction2->getStartSequenceNo(), 2);
	ASSERT_EQ(testAction2->getEndSequenceNo(), 4);
}

TEST_F(RootActionTest, verifySequenceNumbersParents2)
{
	EXPECT_CALL(*mockBeacon, createSequenceNumber())
		.WillOnce(testing::Return((int32_t)1))
		.WillOnce(testing::Return((int32_t)2))
		.WillOnce(testing::Return((int32_t)3))
		.WillRepeatedly(testing::Return((int32_t)4));

	//create two actions
	auto testAction1 = std::make_shared<core::RootAction>(logger, mockBeacon, core::UTF8String("test action 1"), session);
	ASSERT_EQ(testAction1->getStartSequenceNo(), 1);
	ASSERT_EQ(testAction1->getEndSequenceNo(), -1);
	auto testAction2 = std::make_shared<core::RootAction>(logger, mockBeacon, core::UTF8String("test action 2"), session);
	ASSERT_EQ(testAction2->getStartSequenceNo(), 2);
	ASSERT_EQ(testAction2->getEndSequenceNo(), -1);

	// leave parents (parent2 leaves first) -> this shall set the end sequence numbers
	testAction2->leaveAction();
	ASSERT_EQ(testAction2->getStartSequenceNo(), 2);
	ASSERT_EQ(testAction2->getEndSequenceNo(), 3);
	testAction1->leaveAction();
	ASSERT_EQ(testAction1->getStartSequenceNo(), 1);
	ASSERT_EQ(testAction1->getEndSequenceNo(), 4);
}

TEST_F(RootActionTest, verifyGetters)
{
	//given
	auto testAction = std::make_shared<core::RootAction>(logger, mockBeacon, core::UTF8String("test action"), session);

	//then
	ASSERT_EQ(testAction->getID(), 1);
	ASSERT_EQ(testAction->getName(), core::UTF8String("test action"));
}

TEST_F(RootActionTest, aNewlyCreatedActionIsNotLeft)
{
	//given
	auto testAction = std::make_shared<core::RootAction>(logger, mockBeacon, core::UTF8String("test action"), session);

	//then
	ASSERT_FALSE(testAction->isActionLeft());
}

TEST_F(RootActionTest, afterLeavingAnActionItIsLeft)
{
	//given
	auto testAction = std::make_shared<core::RootAction>(logger, mockBeacon, core::UTF8String("test action"), session);

	//when
	testAction->leaveAction();

	//then
	ASSERT_TRUE(testAction->isActionLeft());
}

TEST_F(RootActionTest, reportEventDoesNothingIfActionIsLeft)
{
	//given
	auto testAction = std::make_shared<core::RootAction>(logger, mockBeacon, core::UTF8String("test action"), session);
	testAction->leaveAction();
	mockBeacon->clearData();

	//when
	auto obtained = testAction->reportEvent("eventName");

	//then
	ASSERT_TRUE(mockBeacon->isEmpty());
	ASSERT_EQ(testAction, obtained);
}

TEST_F(RootActionTest, reportIntValueDoesNothingIfActionIsLeft)
{
	//given
	auto testAction = std::make_shared<core::RootAction>(logger, mockBeacon, core::UTF8String("test action"), session);
	testAction->leaveAction();
	mockBeacon->clearData();

	//when
	auto obtained = testAction->reportValue("intValue", 42);

	//then
	ASSERT_TRUE(mockBeacon->isEmpty());
	ASSERT_EQ(testAction, obtained);
}

TEST_F(RootActionTest, reportDoubleValueDoesNothingIfActionIsLeft)
{
	//given
	auto testAction = std::make_shared<core::RootAction>(logger, mockBeacon, core::UTF8String("test action"), session);
	testAction->leaveAction();
	mockBeacon->clearData();

	//when
	auto obtained = testAction->reportValue("doubleValue", 42.1337);

	//then
	ASSERT_TRUE(mockBeacon->isEmpty());
	ASSERT_EQ(testAction, obtained);
}

TEST_F(RootActionTest, reportStringValueDoesNothingIfActionIsLeft)
{
	//given
	auto testAction = std::make_shared<core::RootAction>(logger, mockBeacon, core::UTF8String("test action"), session);
	testAction->leaveAction();
	mockBeacon->clearData();

	//when
	auto obtained = testAction->reportValue("stringValue", "yet another test string");

	//then
	ASSERT_TRUE(mockBeacon->isEmpty());
	ASSERT_EQ(testAction, obtained);
}

TEST_F(RootActionTest, reportErrorDoesNothingIfActionIsLeft)
{
	//given
	auto testAction = std::make_shared<core::RootAction>(logger, mockBeacon, core::UTF8String("test action"), session);
	testAction->leaveAction();
	mockBeacon->clearData();

	//when
	auto obtained = testAction->reportError("teapot", 418, "I'm a teapot");

	//then
	ASSERT_TRUE(mockBeacon->isEmpty());
	ASSERT_EQ(testAction, obtained);
}