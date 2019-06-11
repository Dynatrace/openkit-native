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
#include "core/WebRequestTracer.h"
#include "core/Action.h"
#include "configuration/Configuration.h"

#include "../protocol/MockHTTPClient.h"
#include "../providers/MockHTTPClientProvider.h"
#include "../protocol/MockBeacon.h"



using namespace core;

static const char APP_ID[] = "appID";
static const char APP_NAME[] = "appName";

class ActionTest : public testing::Test
{
protected:
	void SetUp()
	{
		logger = std::make_shared<core::util::DefaultLogger>(devNull, openkit::LogLevel::LOG_LEVEL_DEBUG);
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
			core::UTF8String(APP_NAME), "", APP_ID, 0, "",
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

TEST_F(ActionTest, reportEvent)
{
	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<core::Action>(logger, mockBeacon, core::UTF8String("test action"));

	//when
	const char* eventName = "TestEvent";
	auto returnedAction = testAction->reportEvent(eventName);

	ASSERT_EQ(testAction, returnedAction);


}

TEST_F(ActionTest, reportEventDoesNothingIfEventNameIsNull)
{
	//verify the following calls
	EXPECT_CALL(*mockBeacon, reportEvent(testing::_, testing::_))
		.Times(testing::Exactly(0));

	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<core::Action>(logger, mockBeacon, core::UTF8String("test action"));

	//when
	auto returnedAction = testAction->reportEvent(nullptr);

	ASSERT_EQ(testAction, returnedAction);
}

TEST_F(ActionTest, reportEventDoesNothingIfEventNameIsEmpty)
{
	//verify the following calls
	EXPECT_CALL(*mockBeacon, reportEvent(testing::_, testing::_))
		.Times(testing::Exactly(0));

	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<core::Action>(logger, mockBeacon, core::UTF8String("test action"));

	//when
	auto returnedAction = testAction->reportEvent("");

	ASSERT_EQ(testAction, returnedAction);
}

TEST_F(ActionTest, reportValueIntWithNullNameDoesNotReportValue)
{
	//verify the following calls
	EXPECT_CALL(*mockBeacon, reportValueInt32(testing::_, testing::_, testing::_))
		.Times(testing::Exactly(0));

	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<core::Action>(logger, mockBeacon, core::UTF8String("test action"));

	//when
	auto returnedAction = testAction->reportValue(nullptr, 42);

	ASSERT_EQ(testAction, returnedAction);
}

TEST_F(ActionTest, reportValueIntWithEmptyNameDoesNotReportValue)
{
	//verify the following calls
	EXPECT_CALL(*mockBeacon, reportValueInt32(testing::_, testing::_, testing::_))
		.Times(testing::Exactly(0));

	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<core::Action>(logger, mockBeacon, core::UTF8String("test action"));

	//when
	auto returnedAction = testAction->reportValue("", 42);

	ASSERT_EQ(testAction, returnedAction);
}

TEST_F(ActionTest, reportValueIntWithValidValue)
{
	const char* integerValueName = "IntegerValue";

	//verify the following calls
	EXPECT_CALL(*mockBeacon, reportValueInt32(testing::_, core::UTF8String(integerValueName), 42))
		.Times(testing::Exactly(1));

	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<core::Action>(logger, mockBeacon, core::UTF8String("test action"));

	//when
	auto returnedAction = testAction->reportValue(integerValueName, 42);

	ASSERT_EQ(testAction, returnedAction);
}

TEST_F(ActionTest, reportValueDoubleWithNullNameDoesNotReportValue)
{
	//verify the following calls
	EXPECT_CALL(*mockBeacon, reportValueDouble(testing::_, testing::_, testing::_))
		.Times(testing::Exactly(0));

	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<core::Action>(logger, mockBeacon, core::UTF8String("test action"));

	//when
	auto returnedAction = testAction->reportValue(nullptr, 42.1337);

	ASSERT_EQ(testAction, returnedAction);
}

TEST_F(ActionTest, reportValueDoubleWithEmptyNameDoesNotReportValue)
{
	//verify the following calls
	EXPECT_CALL(*mockBeacon, reportValueDouble(testing::_, testing::_, testing::_))
		.Times(testing::Exactly(0));

	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<core::Action>(logger, mockBeacon, core::UTF8String("test action"));

	//when
	auto returnedAction = testAction->reportValue("", 42.1337);

	ASSERT_EQ(testAction, returnedAction);
}

TEST_F(ActionTest, reportValueDoubleWithValidValue)
{
	//verify the following calls
	EXPECT_CALL(*mockBeacon, reportValueDouble(testing::_, testing::_,42.1337))
		.Times(testing::Exactly(1));

	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<core::Action>(logger, mockBeacon, core::UTF8String("test action"));

	//when
	auto returnedAction = testAction->reportValue("DoubleValue", 42.1337);

	ASSERT_EQ(testAction, returnedAction);
}

TEST_F(ActionTest, reportValueStringWithValidValue)
{
	//verify the following calls
	EXPECT_CALL(*mockBeacon, reportValueString(testing::_, testing::_, core::UTF8String("This is a string")))
		.Times(testing::Exactly(1));

	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<core::Action>(logger, mockBeacon, core::UTF8String("test action"));

	//when
	auto returnedAction = testAction->reportValue("StringValue", "This is a string");

	ASSERT_EQ(testAction, returnedAction);
}

TEST_F(ActionTest, reportValueStringWithValueNull)
{
	//verify the following calls
	EXPECT_CALL(*mockBeacon, reportValueString(testing::_, testing::_, core::UTF8String("")))
		.Times(testing::Exactly(1));

	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<core::Action>(logger, mockBeacon, core::UTF8String("test action"));

	//when
	auto returnedAction = testAction->reportValue("StringValue", nullptr);

	ASSERT_EQ(testAction, returnedAction);
}

TEST_F(ActionTest, reportErrorWithAllValuesSet)
{
	//verify the following calls
	EXPECT_CALL(*mockBeacon, reportError(testing::_,core::UTF8String("FATAL Error"), 0x8005037, core::UTF8String("Some reason for this fatal error")))
		.Times(testing::Exactly(1));

	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<core::Action>(logger, mockBeacon, core::UTF8String("test action"));

	//when
	auto returnedAction = testAction->reportError("FATAL Error", 0x8005037, "Some reason for this fatal error");

	ASSERT_EQ(testAction, returnedAction);
}

TEST_F(ActionTest, reportErrorWithNullErrorNameDoesNotReportTheError)
{
	//verify the following calls
	EXPECT_CALL(*mockBeacon, reportError(testing::_, testing::_, testing::_, testing::_))
		.Times(testing::Exactly(0));

	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<core::Action>(logger, mockBeacon, core::UTF8String("test action"));

	//when
	auto returnedAction = testAction->reportError(nullptr, 0x8005037, "Some reason for this fatal error");

	ASSERT_EQ(testAction, returnedAction);
}

TEST_F(ActionTest, reportErrorWithEmptyErrorNameDoesNotReportTheError)
{
	//verify the following calls
	EXPECT_CALL(*mockBeacon, reportError(testing::_, testing::_, testing::_, testing::_))
		.Times(testing::Exactly(0));

	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<core::Action>(logger, mockBeacon, core::UTF8String("test action"));

	//when
	auto returnedAction = testAction->reportError("", 0x8005037, "Some reason for this fatal error");

	ASSERT_EQ(testAction, returnedAction);
}

TEST_F(ActionTest, reportErrorWithEmptyNullErrorReasonDoesReport)
{
	//verify the following calls
	EXPECT_CALL(*mockBeacon, reportError(testing::_, core::UTF8String("FATAL ERROR"), 0x8005037, core::UTF8String("")))
		.Times(testing::Exactly(1));

	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<core::Action>(logger, mockBeacon, core::UTF8String("test action"));

	//when
	auto returnedAction = testAction->reportError("FATAL ERROR", 0x8005037, nullptr);

	ASSERT_EQ(testAction, returnedAction);
}


TEST_F(ActionTest, canTraceWebRequestUrl)
{
	core::UTF8String urlStr("http://example.com/pages/");
	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<core::Action>(logger, mockBeacon, core::UTF8String("test action"));

	// execute the test call
	auto webRequestTracer = testAction->traceWebRequest(urlStr.getStringData().c_str());

	// verify the returned request
	std::shared_ptr<core::WebRequestTracer> webRequestTracerImpl = std::static_pointer_cast<core::WebRequestTracer>(webRequestTracer);
	EXPECT_TRUE(webRequestTracerImpl->getURL().equals(urlStr));
}

TEST_F(ActionTest, canTraceWebRequestUrlWithParameters)
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
	std::shared_ptr<core::WebRequestTracer> webRequestTracerImpl = std::static_pointer_cast<core::WebRequestTracer>(webRequestTracer);
	EXPECT_TRUE(webRequestTracerImpl->getURL().equals(urlStr));
}

TEST_F(ActionTest, tracingANullStringWebRequestIsNotAllowed)
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

TEST_F(ActionTest, tracingAnEmptyStringWebRequestIsNotAllowed)
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

TEST_F(ActionTest, tracingAnInvalidUrlSchemeIsNotAllowed)
{
	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<core::Action>(logger, mockBeacon, core::UTF8String("test action"));

	// execute the test call
	auto webRequestTracer = testAction->traceWebRequest("1337://fourtytwo.com");

	// verify the returned request
	ASSERT_TRUE(webRequestTracer != nullptr);
	std::shared_ptr<core::NullWebRequestTracer> typeCast = std::dynamic_pointer_cast<core::NullWebRequestTracer>(webRequestTracer);
	ASSERT_TRUE(typeCast != nullptr);
}

TEST_F(ActionTest, actionsEnteredAndLeft)
{
	session->startSession();

	// create a new parent action
	auto testParentAction = std::make_shared<core::RootAction>(logger, mockBeacon, core::UTF8String("test root action"), session);
	ASSERT_EQ(testParentAction->getID(), 1);

	// create child
	auto testAction = std::make_shared<core::Action>(logger, mockBeacon, core::UTF8String("test action"), testParentAction);
	ASSERT_EQ(testAction->getID(), 2);
	ASSERT_EQ(testAction->getParentID(), 1);

	//leave child
	auto parent = testAction->leaveAction();
	ASSERT_EQ(parent, testParentAction);
}

TEST_F(ActionTest, leaveAction)
{
	EXPECT_CALL(*mockBeacon, getCurrentTimestamp())
		.WillOnce(testing::Return((int32_t)42))
		.WillRepeatedly(testing::Return((int32_t)48));

	EXPECT_CALL(*mockBeacon, createSequenceNumber())
		.WillOnce(testing::Return((int32_t)1))
		.WillRepeatedly(testing::Return((int32_t)2));
	auto testAction = std::make_shared<core::Action>(logger, mockBeacon, core::UTF8String("test action"));

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

TEST_F(ActionTest, leaveActionTwice)
{
	auto testAction = std::make_shared<core::Action>(logger, mockBeacon, core::UTF8String("test action"));
	// execute the test call: simulate a few reportValues and then leaveAction
	testAction->reportValue("DoubleValue", 3.141592654);
	testAction->reportValue("IntValue", 42);
	testAction->reportValue("StringValue", "nice value!");
	auto retAction1 = testAction->leaveAction();
	ASSERT_TRUE(retAction1 == nullptr);
	auto retAction2 = testAction->leaveAction();
	ASSERT_TRUE(retAction2 == nullptr);
}

TEST_F(ActionTest, verifySequenceNumbersParents)
{
	EXPECT_CALL(*mockBeacon, createSequenceNumber())
		.WillOnce(testing::Return((int32_t)1))
		.WillOnce(testing::Return((int32_t)2))
		.WillOnce(testing::Return((int32_t)3))
		.WillRepeatedly(testing::Return((int32_t)4));

	//create two actions
	auto testAction1 = std::make_shared<core::Action>(logger, mockBeacon, core::UTF8String("test action 1"));
	ASSERT_EQ(testAction1->getStartSequenceNo(), 1);
	ASSERT_EQ(testAction1->getEndSequenceNo(), -1);
	auto testAction2 = std::make_shared<core::Action>(logger, mockBeacon, core::UTF8String("test action 2"));
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

TEST_F(ActionTest, verifySequenceNumbersParents2)
{
	EXPECT_CALL(*mockBeacon, createSequenceNumber())
		.WillOnce(testing::Return((int32_t)1))
		.WillOnce(testing::Return((int32_t)2))
		.WillOnce(testing::Return((int32_t)3))
		.WillRepeatedly(testing::Return((int32_t)4));

	//create two actions
	auto testAction1 = std::make_shared<core::Action>(logger, mockBeacon, core::UTF8String("test action 1"));
	ASSERT_EQ(testAction1->getStartSequenceNo(), 1);
	ASSERT_EQ(testAction1->getEndSequenceNo(), -1);
	auto testAction2 = std::make_shared<core::Action>(logger, mockBeacon, core::UTF8String("test action 2"));
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

TEST_F(ActionTest, verifySequenceNumbersParentWithTwoChildren)
{
	EXPECT_CALL(*mockBeacon, createSequenceNumber())
		.WillOnce(testing::Return((int32_t)1))
		.WillOnce(testing::Return((int32_t)2))
		.WillOnce(testing::Return((int32_t)3))
		.WillOnce(testing::Return((int32_t)4))
		.WillOnce(testing::Return((int32_t)5))
		.WillRepeatedly(testing::Return((int32_t)6));

	//create root action with two child actions attached via parent link in the child action
	auto testRootAction = std::make_shared<core::RootAction>(logger, mockBeacon, core::UTF8String("test action"), session);
	ASSERT_EQ(testRootAction->getStartSequenceNo(), 1);
	ASSERT_EQ(testRootAction->getEndSequenceNo(), -1);

	auto testAction1 = std::make_shared<core::Action>(logger, mockBeacon, core::UTF8String("test action"), testRootAction);
	ASSERT_EQ(testAction1->getStartSequenceNo(), 2);
	ASSERT_EQ(testAction1->getEndSequenceNo(), -1);
	auto testAction2 = std::make_shared<core::Action>(logger, mockBeacon, core::UTF8String("test action"), testRootAction);
	ASSERT_EQ(testAction2->getStartSequenceNo(), 3);
	ASSERT_EQ(testAction2->getEndSequenceNo(), -1);

	// leave parents (action1 leaves first) -> this shall set the end sequence numbers
	testAction1->leaveAction();
	ASSERT_EQ(testAction1->getStartSequenceNo(), 2);
	ASSERT_EQ(testAction1->getEndSequenceNo(), 4);
	testAction2->leaveAction();
	ASSERT_EQ(testAction2->getStartSequenceNo(), 3);
	ASSERT_EQ(testAction2->getEndSequenceNo(), 5);
	testRootAction->leaveAction();
	ASSERT_EQ(testRootAction->getStartSequenceNo(), 1);
	ASSERT_EQ(testRootAction->getEndSequenceNo(), 6);
}

TEST_F(ActionTest, verifySequenceNumbersParentWithTwoChildrenParentLeavesFirst)
{
	EXPECT_CALL(*mockBeacon, createSequenceNumber())
		.WillOnce(testing::Return((int32_t)1))
		.WillOnce(testing::Return((int32_t)2))
		.WillOnce(testing::Return((int32_t)3))
		.WillOnce(testing::Return((int32_t)4))
		.WillOnce(testing::Return((int32_t)5))
		.WillRepeatedly(testing::Return((int32_t)6));

	//create root action and create two child actions via the root action method enterAction
	auto testRootAction = std::make_shared<core::RootAction>(logger, mockBeacon, core::UTF8String("test action"), session);
	ASSERT_EQ(testRootAction->getStartSequenceNo(), 1);
	ASSERT_EQ(testRootAction->getEndSequenceNo(), -1);

	auto testAction1 = testRootAction->enterAction("child 1");
	std::shared_ptr<core::Action> castToActualTypeChild1 = std::static_pointer_cast<core::Action>(testAction1);
	ASSERT_EQ(castToActualTypeChild1->getStartSequenceNo(), 2);
	ASSERT_EQ(castToActualTypeChild1->getEndSequenceNo(), -1);

	auto testAction2 = testRootAction->enterAction("child 2");
	std::shared_ptr<core::Action> castToActualTypeChild2 = std::static_pointer_cast<core::Action>(testAction2);
	ASSERT_EQ(castToActualTypeChild2->getStartSequenceNo(), 3);
	ASSERT_EQ(castToActualTypeChild2->getEndSequenceNo(), -1);

	// parent leaves first => this also leaves the children
	testRootAction->leaveAction();
	ASSERT_EQ(castToActualTypeChild1->getStartSequenceNo(), 2);
	ASSERT_EQ(castToActualTypeChild1->getEndSequenceNo(), 4);
	ASSERT_EQ(castToActualTypeChild2->getStartSequenceNo(), 3);
	ASSERT_EQ(castToActualTypeChild2->getEndSequenceNo(), 5);
	ASSERT_EQ(testRootAction->getStartSequenceNo(), 1);
	ASSERT_EQ(testRootAction->getEndSequenceNo(), 6);
}

TEST_F(ActionTest, verifyGetters)
{
	//given
	auto testAction = std::make_shared<core::Action>(logger, mockBeacon, core::UTF8String("test action"));

	//then
	ASSERT_EQ(testAction->getID(), 1);
	ASSERT_EQ(testAction->getName(), core::UTF8String("test action"));
	ASSERT_EQ(testAction->getParentID(), 0);
}

TEST_F(ActionTest, aNewlyCreatedActionIsNotLeft)
{
	//given
	auto testAction = std::make_shared<core::Action>(logger, mockBeacon, core::UTF8String("test action"));

	//then
	ASSERT_FALSE(testAction->isActionLeft());
}

TEST_F(ActionTest, afterLeavingAnActionItIsLeft)
{
	//given
	auto testAction = std::make_shared<core::Action>(logger, mockBeacon, core::UTF8String("test action"));

	//when
	testAction->leaveAction();

	//then
	ASSERT_TRUE(testAction->isActionLeft());
}

TEST_F(ActionTest, reportEventDoesNothingIfActionIsLeft)
{
	//given
	auto testAction = std::make_shared<core::Action>(logger, mockBeacon, core::UTF8String("test action"));
	testAction->leaveAction();
	mockBeacon->clearData();

	//when
	auto obtained = testAction->reportEvent("eventName");

	//then
	ASSERT_TRUE(mockBeacon->isEmpty());
	ASSERT_EQ(testAction, obtained);
}

TEST_F(ActionTest, reportIntValueDoesNothingIfActionIsLeft)
{
	//given
	auto testAction = std::make_shared<core::Action>(logger, mockBeacon, core::UTF8String("test action"));
	testAction->leaveAction();
	mockBeacon->clearData();

	//when
	auto obtained = testAction->reportValue("intValue", 42);

	//then
	ASSERT_TRUE(mockBeacon->isEmpty());
	ASSERT_EQ(testAction, obtained);
}

TEST_F(ActionTest, reportDoubleValueDoesNothingIfActionIsLeft)
{
	//given
	auto testAction = std::make_shared<core::Action>(logger, mockBeacon, core::UTF8String("test action"));
	testAction->leaveAction();
	mockBeacon->clearData();

	//when
	auto obtained = testAction->reportValue("doubleValue", 42.1337);

	//then
	ASSERT_TRUE(mockBeacon->isEmpty());
	ASSERT_EQ(testAction, obtained);
}

TEST_F(ActionTest, reportStringValueDoesNothingIfActionIsLeft)
{
	//given
	auto testAction = std::make_shared<core::Action>(logger, mockBeacon, core::UTF8String("test action"));
	testAction->leaveAction();
	mockBeacon->clearData();

	//when
	auto obtained = testAction->reportValue("stringValue", "yet another test string");

	//then
	ASSERT_TRUE(mockBeacon->isEmpty());
	ASSERT_EQ(testAction, obtained);
}

TEST_F(ActionTest, reportErrorDoesNothingIfActionIsLeft)
{
	//given
	auto testAction = std::make_shared<core::Action>(logger, mockBeacon, core::UTF8String("test action"));
	testAction->leaveAction();
	mockBeacon->clearData();

	//when
	auto obtained = testAction->reportError("teapot", 418, "I'm a teapot");

	//then
	ASSERT_TRUE(mockBeacon->isEmpty());
	ASSERT_EQ(testAction, obtained);
}