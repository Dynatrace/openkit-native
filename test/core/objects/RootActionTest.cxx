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

#include "Types.h"
#include "../Types.h"
#include "../util/Types.h"
#include "../caching/Types.h"
#include "../configuration/Types.h"
#include "../../api/Types.h"
#include "../../protocol/Types.h"
#include "../../protocol/MockTypes.h"
#include "../../providers/Types.h"
#include "../../providers/MockTypes.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>

using namespace test::types;

static const char APP_ID[] = "appID";
static const char APP_NAME[] = "appName";

class RootActionTest : public testing::Test
{
protected:

	void SetUp()
	{
		logger = std::make_shared<DefaultLogger_t>(devNull, LogLevel_t::LOG_LEVEL_DEBUG);
		threadIDProvider = std::make_shared<DefaultThreadIdProvider_t>();
		timingProvider = std::make_shared<DefaultTimingProvider_t>();
		sessionIDProvider = std::make_shared<DefaultSessionIdProvider_t>();

		auto httpClientConfiguration = std::make_shared<HttpClientConfiguration_t>(Utf8String_t(""), 0, Utf8String_t(""));
		mockHTTPClientProvider = std::make_shared<MockNiceHttpClientProvider_t>();
		mockHTTPClient = std::make_shared<MockNiceHttpClient_t>(httpClientConfiguration);

		trustManager = std::make_shared<SslStrictTrustManager_t>();

		auto device = std::make_shared<Device_t>(Utf8String_t(""), Utf8String_t(""), Utf8String_t(""));

		beaconConfiguration = std::make_shared<BeaconConfiguration_t>();
		beaconCacheConfiguration = std::make_shared<BeaconCacheConfiguration_t>(-1, -1, -1);
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

		beaconCache = std::make_shared<BeaconCache_t>(logger);

		beaconSender = std::make_shared<BeaconSender_t>(logger, configuration, mockHTTPClientProvider, timingProvider);
		mockBeacon = std::make_shared<MockNiceBeacon_t>(logger, beaconCache, configuration, nullptr, threadIDProvider, timingProvider);

		session = std::make_shared<Session_t>(logger, beaconSender, mockBeacon);
	}

	void TearDown()
	{
	}

	std::ostringstream devNull;
	ILogger_sp logger;
	IThreadIdProvider_sp threadIDProvider;
	ITimingProvider_sp timingProvider;
	ISessionIdProvider_sp sessionIDProvider;

	MockNiceHttpClient_sp mockHTTPClient;
	ISslTrustManager_sp trustManager;

	BeaconConfiguration_sp beaconConfiguration;
	BeaconCacheConfiguration_sp beaconCacheConfiguration;
	Configuration_sp configuration;
	BeaconCache_sp beaconCache;

	BeaconSender_sp beaconSender;
	MockNiceBeacon_sp mockBeacon;
	MockNiceHttpClientProvider_sp mockHTTPClientProvider;

	Session_sp session;
};

TEST_F(RootActionTest, enterActionWithNullNameGivesNullAction)
{
	// create test environment
	// create root action with a null child action (must be valid)
	auto testRootAction = std::make_shared<RootAction_t>(logger, mockBeacon, Utf8String_t("test root action"), session);

	//when
	auto childAction = testRootAction->enterAction(nullptr);

	ASSERT_TRUE(childAction != nullptr);
	NullAction_sp typeCast = std::dynamic_pointer_cast<NullAction_t>(childAction);
	ASSERT_TRUE(typeCast != nullptr);
}

TEST_F(RootActionTest, enterActionWithEmptyNameGivesNullAction)
{
	// create test environment
	// create root action with a null child action (must be valid)
	auto testRootAction = std::make_shared<RootAction_t>(logger, mockBeacon, Utf8String_t("test root action"), session);

	//when
	auto childAction = testRootAction->enterAction("");

	ASSERT_TRUE(childAction != nullptr);
	NullAction_sp typeCast = std::dynamic_pointer_cast<NullAction_t>(childAction);
	ASSERT_TRUE(typeCast != nullptr);
}

TEST_F(RootActionTest, enterAndLeaveActions)
{
	// given: create root action with child action
	const char* rootActionName = "test root action";
	auto testRootAction = std::make_shared<RootAction_t>(logger, mockBeacon, Utf8String_t(rootActionName), session);
	const char* childActionName = "test child action";
	auto childAction = testRootAction->enterAction(childActionName);

	//verify
	ASSERT_TRUE(testRootAction->getName().equals(Utf8String_t(rootActionName)));
	Action_sp childActionCast = std::static_pointer_cast<Action_t>(childAction);
	ASSERT_TRUE(childActionCast->getName().equals(Utf8String_t(childActionName)));

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
	auto testRootAction = std::make_shared<RootAction_t>(logger, mockBeacon, Utf8String_t(rootActionName), session);
	const char* childActionName1 = "first test child action";
	const char* childActionName2 = "second test child action";
	auto childActionOne = testRootAction->enterAction(childActionName1);
	auto childActionTwo = testRootAction->enterAction(childActionName2);

	//verify
	ASSERT_TRUE(testRootAction->getName().equals(Utf8String_t(rootActionName)));
	Action_sp childActionCast1 = std::static_pointer_cast<Action_t>(childActionOne);
	ASSERT_TRUE(childActionCast1->getName().equals(Utf8String_t(childActionName1)));
	Action_sp childActionCast2 = std::static_pointer_cast<Action_t>(childActionTwo);
	ASSERT_TRUE(childActionCast2->getName().equals(Utf8String_t(childActionName2)));

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
	auto testRootAction = std::make_shared<RootAction_t>(logger, mockBeacon, Utf8String_t(rootActionName), session);

	testRootAction->leaveAction();

	auto obtained = testRootAction->enterAction("open another action");

	ASSERT_TRUE(obtained != nullptr);
	NullAction_sp typeCast = std::dynamic_pointer_cast<NullAction_t>(obtained);
	ASSERT_TRUE(typeCast != nullptr);
}

TEST_F(RootActionTest, reportEvent)
{
	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<RootAction_t>(logger, mockBeacon, Utf8String_t("test action"), session);

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
	auto testAction = std::make_shared<RootAction_t>(logger, mockBeacon, Utf8String_t("test action"), session);

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
	auto testAction = std::make_shared<RootAction_t>(logger, mockBeacon, Utf8String_t("test action"), session);

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
	auto testAction = std::make_shared<RootAction_t>(logger, mockBeacon, Utf8String_t("test action"), session);

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
	auto testAction = std::make_shared<RootAction_t>(logger, mockBeacon, Utf8String_t("test action"), session);

	//when
	auto returnedAction = testAction->reportValue("", 42);

	ASSERT_EQ(testAction, returnedAction);
}

TEST_F(RootActionTest, reportValueIntWithValidValue)
{
	const char* integerValueName = "IntegerValue";

	//verify the following calls
	EXPECT_CALL(*mockBeacon, reportValueInt32(testing::_, Utf8String_t(integerValueName), 42))
		.Times(testing::Exactly(1));

	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<RootAction_t>(logger, mockBeacon, Utf8String_t("test action"), session);

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
	auto testAction = std::make_shared<RootAction_t>(logger, mockBeacon, Utf8String_t("test action"), session);

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
	auto testAction = std::make_shared<RootAction_t>(logger, mockBeacon, Utf8String_t("test action"), session);

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
	auto testAction = std::make_shared<RootAction_t>(logger, mockBeacon, Utf8String_t("test action"), session);

	//when
	auto returnedAction = testAction->reportValue("DoubleValue", 42.1337);

	ASSERT_EQ(testAction, returnedAction);
}

TEST_F(RootActionTest, reportValueStringWithValidValue)
{
	//verify the following calls
	EXPECT_CALL(*mockBeacon, reportValueString(testing::_, testing::_, Utf8String_t("This is a string")))
		.Times(testing::Exactly(1));

	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<RootAction_t>(logger, mockBeacon, Utf8String_t("test action"), session);

	//when
	auto returnedAction = testAction->reportValue("StringValue", "This is a string");

	ASSERT_EQ(testAction, returnedAction);
}

TEST_F(RootActionTest, reportValueStringWithValueNull)
{
	//verify the following calls
	EXPECT_CALL(*mockBeacon, reportValueString(testing::_, testing::_, Utf8String_t("")))
		.Times(testing::Exactly(1));

	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<RootAction_t>(logger, mockBeacon, Utf8String_t("test action"), session);

	//when
	auto returnedAction = testAction->reportValue("StringValue", nullptr);

	ASSERT_EQ(testAction, returnedAction);
}

TEST_F(RootActionTest, reportErrorWithAllValuesSet)
{
	//verify the following calls
	EXPECT_CALL(*mockBeacon, reportError(testing::_, Utf8String_t("FATAL Error"), 0x8005037, Utf8String_t("Some reason for this fatal error")))
		.Times(testing::Exactly(1));

	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<RootAction_t>(logger, mockBeacon, Utf8String_t("test action"), session);

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
	auto testAction = std::make_shared<RootAction_t>(logger, mockBeacon, Utf8String_t("test action"), session);

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
	auto testAction = std::make_shared<RootAction_t>(logger, mockBeacon, Utf8String_t("test action"), session);

	//when
	auto returnedAction = testAction->reportError("", 0x8005037, "Some reason for this fatal error");

	ASSERT_EQ(testAction, returnedAction);
}

TEST_F(RootActionTest, reportErrorWithEmptyNullErrorReasonDoesReport)
{
	//verify the following calls
	EXPECT_CALL(*mockBeacon, reportError(testing::_, Utf8String_t("FATAL ERROR"), 0x8005037, Utf8String_t("")))
		.Times(testing::Exactly(1));

	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<RootAction_t>(logger, mockBeacon, Utf8String_t("test action"), session);

	//when
	auto returnedAction = testAction->reportError("FATAL ERROR", 0x8005037, nullptr);

	ASSERT_EQ(testAction, returnedAction);
}

TEST_F(RootActionTest, canTraceWebRequestUrl)
{
	Utf8String_t urlStr("http://example.com/pages/");
	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<Action_t>(logger, mockBeacon, Utf8String_t("test action"));

	// execute the test call
	auto webRequestTracer = testAction->traceWebRequest(urlStr.getStringData().c_str());

	// verify the returned request
	WebRequestTracer_sp webRequestTracerImpl = std::static_pointer_cast<WebRequestTracer_t>(webRequestTracer);
	EXPECT_TRUE(webRequestTracerImpl->getURL().equals(urlStr));
}

TEST_F(RootActionTest, canTraceWebRequestUrlWithParameters)
{
	Utf8String_t urlStr("http://example.com/pages/");
	Utf8String_t paramString("someParameter=hello&someOtherParameter=world");

	Utf8String_t urlWithParamString(urlStr);
	urlWithParamString.concatenate("?");
	urlWithParamString.concatenate(paramString);

	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<Action_t>(logger, mockBeacon, Utf8String_t("test action"));

	// execute the test call
	auto webRequestTracer = testAction->traceWebRequest(urlWithParamString.getStringData().c_str());

	// verify the returned request
	WebRequestTracer_sp webRequestTracerImpl = std::static_pointer_cast<WebRequestTracer_t>(webRequestTracer);
	EXPECT_TRUE(webRequestTracerImpl->getURL().equals(urlStr));
}

TEST_F(RootActionTest, tracingANullStringWebRequestIsNotAllowed)
{
	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<Action_t>(logger, mockBeacon, Utf8String_t("test action"));

	// execute the test call
	auto webRequestTracer = testAction->traceWebRequest(nullptr);

	// verify the returned request
	ASSERT_TRUE(webRequestTracer != nullptr);
	NullWebRequestTracer_sp typeCast = std::dynamic_pointer_cast<NullWebRequestTracer_t>(webRequestTracer);
	ASSERT_TRUE(typeCast != nullptr);
}

TEST_F(RootActionTest, tracingAnEmptyStringWebRequestIsNotAllowed)
{
	// create test environment
	// create action without parent action
	auto testAction = std::make_shared<Action_t>(logger, mockBeacon, Utf8String_t("test action"));

	// execute the test call
	auto webRequestTracer = testAction->traceWebRequest("");

	// verify the returned request
	ASSERT_TRUE(webRequestTracer != nullptr);
	NullWebRequestTracer_sp typeCast = std::dynamic_pointer_cast<NullWebRequestTracer_t>(webRequestTracer);
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
	auto testAction = std::make_shared<RootAction_t>(logger, mockBeacon, Utf8String_t("test action"), session);
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
	auto testAction = std::make_shared<RootAction_t>(logger, mockBeacon, Utf8String_t("test action"), session);
	// execute the test call: simulate a few reportValues and then leaveAction
	testAction->reportValue("DoubleValue", 3.141592654);
	testAction->reportValue("IntValue", 42);
	testAction->reportValue("StringValue", "nice value!");
	testAction->leaveAction();
	testAction->leaveAction();
}

TEST_F(RootActionTest, leaveActionSetsEndTimeBeforeAddingToBeacon)
{
	// expect
	{
		testing::InSequence s;

		EXPECT_CALL(*mockBeacon, getCurrentTimestamp()).Times(1); 	// root action constructor
		EXPECT_CALL(*mockBeacon, createSequenceNumber()).Times(1); 	// root action constructor

		EXPECT_CALL(*mockBeacon, getCurrentTimestamp()).Times(2); 	// check if action is ended + set end timestamp
		EXPECT_CALL(*mockBeacon, createSequenceNumber()).Times(1); 	// set end sequence number
		EXPECT_CALL(*mockBeacon, mockAddAction(testing::Matcher<RootAction_sp>(testing::_)));
	}

	// given
	auto target = std::make_shared<RootAction_t>(logger, mockBeacon, core::UTF8String("root action"), session);

	// when
	target->leaveAction();
}

TEST_F(RootActionTest, leaveActionLeavesChildActionsBeforeSettingEndTime)
{
	// expect
	{
		testing::InSequence s;
		EXPECT_CALL(*mockBeacon, getCurrentTimestamp());	// constructor root action
		EXPECT_CALL(*mockBeacon, createSequenceNumber());	// constructor root action

		EXPECT_CALL(*mockBeacon, getCurrentTimestamp());	// constructor child action
		EXPECT_CALL(*mockBeacon, createSequenceNumber());	// constructor child action


		EXPECT_CALL(*mockBeacon, getCurrentTimestamp());	// check if root action is ended
		EXPECT_CALL(*mockBeacon, getCurrentTimestamp());	// check if child action is ended
		EXPECT_CALL(*mockBeacon, getCurrentTimestamp());	// set end time on child action
		EXPECT_CALL(*mockBeacon, createSequenceNumber());	// set end sequence number on child action
		EXPECT_CALL(*mockBeacon, mockAddAction(testing::Matcher<Action_sp>(testing::_)));
		EXPECT_CALL(*mockBeacon, getCurrentTimestamp());	// set end time on root action
		EXPECT_CALL(*mockBeacon, createSequenceNumber());	// set end sequence number on root action
		EXPECT_CALL(*mockBeacon, mockAddAction(testing::Matcher<RootAction_sp>(testing::_)));
	}

	// given
	auto target = std::make_shared<RootAction_t>(logger, mockBeacon, core::UTF8String("root action"), session);
	target->enterAction("child action");

	// when
	target->leaveAction();
}

TEST_F(RootActionTest, verifySequenceNumbersParents)
{
	EXPECT_CALL(*mockBeacon, createSequenceNumber())
		.WillOnce(testing::Return((int32_t)1))
		.WillOnce(testing::Return((int32_t)2))
		.WillOnce(testing::Return((int32_t)3))
		.WillRepeatedly(testing::Return((int32_t)4));

	//create two actions
	auto testAction1 = std::make_shared<RootAction_t>(logger, mockBeacon, Utf8String_t("test action 1"), session);
	ASSERT_EQ(testAction1->getStartSequenceNo(), 1);
	ASSERT_EQ(testAction1->getEndSequenceNo(), -1);
	auto testAction2 = std::make_shared<RootAction_t>(logger, mockBeacon, Utf8String_t("test action 2"), session);
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
	auto testAction1 = std::make_shared<RootAction_t>(logger, mockBeacon, Utf8String_t("test action 1"), session);
	ASSERT_EQ(testAction1->getStartSequenceNo(), 1);
	ASSERT_EQ(testAction1->getEndSequenceNo(), -1);
	auto testAction2 = std::make_shared<RootAction_t>(logger, mockBeacon, Utf8String_t("test action 2"), session);
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
	auto testAction = std::make_shared<RootAction_t>(logger, mockBeacon, Utf8String_t("test action"), session);

	//then
	ASSERT_EQ(testAction->getID(), 1);
	ASSERT_EQ(testAction->getName(), Utf8String_t("test action"));
}

TEST_F(RootActionTest, aNewlyCreatedActionIsNotLeft)
{
	//given
	auto testAction = std::make_shared<RootAction_t>(logger, mockBeacon, Utf8String_t("test action"), session);

	//then
	ASSERT_FALSE(testAction->isActionLeft());
}

TEST_F(RootActionTest, afterLeavingAnActionItIsLeft)
{
	//given
	auto testAction = std::make_shared<RootAction_t>(logger, mockBeacon, Utf8String_t("test action"), session);

	//when
	testAction->leaveAction();

	//then
	ASSERT_TRUE(testAction->isActionLeft());
}

TEST_F(RootActionTest, reportEventDoesNothingIfActionIsLeft)
{
	//given
	auto testAction = std::make_shared<RootAction_t>(logger, mockBeacon, Utf8String_t("test action"), session);
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
	auto testAction = std::make_shared<RootAction_t>(logger, mockBeacon, Utf8String_t("test action"), session);
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
	auto testAction = std::make_shared<RootAction_t>(logger, mockBeacon, Utf8String_t("test action"), session);
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
	auto testAction = std::make_shared<RootAction_t>(logger, mockBeacon, Utf8String_t("test action"), session);
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
	auto testAction = std::make_shared<RootAction_t>(logger, mockBeacon, Utf8String_t("test action"), session);
	testAction->leaveAction();
	mockBeacon->clearData();

	//when
	auto obtained = testAction->reportError("teapot", 418, "I'm a teapot");

	//then
	ASSERT_TRUE(mockBeacon->isEmpty());
	ASSERT_EQ(testAction, obtained);
}