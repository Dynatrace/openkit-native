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
#include "MockTypes.h"
#include "../Types.h"
#include "../caching/Types.h"
#include "../configuration/Types.h"
#include "../../api/MockTypes.h"
#include "../../protocol/MockTypes.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <sstream>

using namespace test::types;

class ActionCommonImplTest : public testing::Test
{
protected:
	MockNiceILogger_sp mockNiceLogger;
	MockNiceBeacon_sp mockNiceBeacon;
	MockNiceIOpenKitComposite_sp mockParent;
	Utf8String_t ACTION_NAME = "TestAction";
	int32_t ACTION_ID= 1234;

	void SetUp()
	{
		mockNiceLogger = std::make_shared<MockNiceILogger_t>();
		ON_CALL(*mockNiceLogger, isInfoEnabled())
				.WillByDefault(testing::Return(true));
		ON_CALL(*mockNiceLogger, isDebugEnabled())
				.WillByDefault(testing::Return(true));

		mockParent = std::make_shared<MockNiceIOpenKitComposite_t>();

		auto beaconCache = std::make_shared<BeaconCache_t>(mockNiceLogger);
		auto device = std::make_shared<Device_t>("", "", "");
		auto sessionIdProvider = std::make_shared<DefaultSessionIdProvider_t>();
		auto beaconCacheConfig = std::make_shared<BeaconCacheConfiguration_t>(-1, -1, -1);
		auto beaconConfig = std::make_shared<BeaconConfiguration_t>();
		auto trustManager = std::make_shared<SslStrictTrustManager_t>();

		int64_t deviceId = 73;

		auto config = std::make_shared<Configuration_t>(
			device,
			OpenKitType_t::Type::DYNATRACE,
			"name",
			"version",
			"appId",
			deviceId,
			std::to_string(deviceId),
			"",
			sessionIdProvider,
			trustManager,
			beaconCacheConfig,
			beaconConfig
		);

		auto threadIdProvider = std::make_shared<DefaultThreadIdProvider_t>();
		auto timingProvider = std::make_shared<DefaultTimingProvider_t>();

		mockNiceBeacon = std::make_shared<MockNiceBeacon_t>(
			mockNiceLogger,
			beaconCache,
			config,
			"127.0.0.1",
			threadIdProvider,
			timingProvider
		);

		ON_CALL(*mockNiceBeacon, createID())
			.WillByDefault(testing::Return(ACTION_ID));
	}

	ActionCommonImpl_sp createAction()
	{
		ActionCommonImpl_sp action = std::make_shared<ActionCommonImpl_t>(
			mockNiceLogger,
			mockNiceBeacon,
			mockParent,
			ACTION_NAME,
			ACTION_NAME.getStringData()
		);
		return action;
	}
};

TEST_F(ActionCommonImplTest, reportEvent)
{
	// with
	const Utf8String_t eventName("Test Event");
	ON_CALL(*mockNiceBeacon, createID())
			.WillByDefault(testing::Return(ACTION_ID));

	// expect
	EXPECT_CALL(*mockNiceBeacon, reportEvent(testing::Eq(ACTION_ID), eventName))
		.Times(testing::Exactly(1));

	// given
	auto target = createAction();

	// when
	target->reportEvent(eventName.getStringData().c_str());
}

TEST_F(ActionCommonImplTest, reportEventDoesNothingIfEventNameIsNull)
{
	// given
	const char* eventName = nullptr;
	auto target = createAction();

	// expect
	std::stringstream stream;
	stream << target->toString() << " reportEvent: eventName must not be null or empty";
	EXPECT_CALL(*mockNiceLogger, mockWarning(stream.str()))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockNiceBeacon, reportEvent(testing::_, testing::_))
		.Times(testing::Exactly(0));

	// when
	target->reportEvent(eventName);
}

TEST_F(ActionCommonImplTest, reportEventDoesNothingIfEventNameIsEmpty)
{
	// given
	const char* eventName = "";
	auto target = createAction();

	// expect
	std::stringstream stream;
	stream << target->toString() << " reportEvent: eventName must not be null or empty";
	EXPECT_CALL(*mockNiceLogger, mockWarning(stream.str()))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockNiceBeacon, reportEvent(testing::_, testing::_))
		.Times(testing::Exactly(0));

	// when
	target->reportEvent(eventName);
}

TEST_F(ActionCommonImplTest, reportValueIntWithNullNameDoesNotReportValue)
{
	// given
	const char* eventName = nullptr;
	const int32_t value = 42;
	auto target = createAction();

	// expect
	std::stringstream stream;
	stream << target->toString() << " reportValue (int): valueName must not be null or empty";
	EXPECT_CALL(*mockNiceLogger, mockWarning(stream.str()))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockNiceBeacon, reportValueInt32(testing::_, testing::_, testing::_))
		.Times(testing::Exactly(0));

	// when
	target->reportValue(eventName, value);
}

TEST_F(ActionCommonImplTest, reportValueIntWithEmptyNameDoesNotReportValue)
{
	// given
	const char* eventName = "";
	const int32_t value = 42;
	auto target = createAction();

	// expect
	std::stringstream stream;
	stream << target->toString() << " reportValue (int): valueName must not be null or empty";
	EXPECT_CALL(*mockNiceLogger, mockWarning(stream.str()))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockNiceBeacon, reportValueInt32(testing::_, testing::_, testing::_))
		.Times(testing::Exactly(0));

	// when
	target->reportValue(eventName, value);
}

TEST_F(ActionCommonImplTest, reportValueIntWithValidValue)
{
	// with
	const char* eventName = "IntegerValue";
	const int32_t value = 42;

	// expect
	EXPECT_CALL(*mockNiceBeacon, reportValueInt32(testing::Eq(ACTION_ID), testing::Eq(eventName), testing::Eq(value)))
		.Times(testing::Exactly(1));

	// given
	auto target = createAction();

	// when
	target->reportValue(eventName, value);
}

TEST_F(ActionCommonImplTest, reportValueDoubleWithNullNameDoesNotReportValue)
{
	// given
	const char* eventName = nullptr;
	const double value = 42.1337;
	auto target = createAction();

	//expect
	std::stringstream stream;
	stream << target->toString() << " reportValue (double): valueName must not be null or empty";
	EXPECT_CALL(*mockNiceLogger, mockWarning(stream.str()))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockNiceBeacon, reportValueDouble(testing::_, testing::_, testing::_))
		.Times(testing::Exactly(0));

	//when
	target->reportValue(eventName, value);
}

TEST_F(ActionCommonImplTest, reportValueDoubleWithEmptyNameDoesNotReportValue)
{
	// given
	const char* eventName = "";
	const double value = 42.1337;
	auto target = createAction();

	//expect
	std::stringstream stream;
	stream << target->toString() << " reportValue (double): valueName must not be null or empty";
	EXPECT_CALL(*mockNiceLogger, mockWarning(stream.str()))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockNiceBeacon, reportValueDouble(testing::_, testing::_, testing::_))
		.Times(testing::Exactly(0));

	//when
	target->reportValue(eventName, value);
}

TEST_F(ActionCommonImplTest, reportValueDoubleWithValidValue)
{
	// with
	const char* eventName = "IntegerValue";
	const double value = 42.1337;

	// expect
	EXPECT_CALL(*mockNiceBeacon, reportValueDouble(testing::Eq(ACTION_ID), testing::Eq(eventName), testing::Eq(value)))
		.Times(testing::Exactly(1));

	// given
	auto target = createAction();

	// when
	target->reportValue(eventName, value);
}

TEST_F(ActionCommonImplTest, reportValueStringWithNullNameDoesNotReportValue)
{
	// given
	const char* eventName = nullptr;
	const char* value = "This is a string";
	auto target = createAction();

	// expect
	std::stringstream stream;
	stream << target->toString() << " reportValue (string): valueName must not be null or empty";
	EXPECT_CALL(*mockNiceLogger, mockWarning(stream.str()))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockNiceBeacon, reportValueString(testing::_, testing::_, testing::_))
		.Times(testing::Exactly(0));

	//when
	target->reportValue(eventName, value);
}

TEST_F(ActionCommonImplTest, reportValueStringWithValidValue)
{
	// with
	const char* eventName = "StringValue";
	const char* value = "This is a string";

	// expect
	EXPECT_CALL(*mockNiceBeacon, reportValueString(testing::Eq(ACTION_ID), testing::Eq(eventName), testing::Eq(value)))
		.Times(testing::Exactly(1));

	// given
	auto target = createAction();

	//when
	target->reportValue(eventName, value);
}

TEST_F(ActionCommonImplTest, reportValueStringWithValueNull)
{
	// with
	const char* eventName = "StringValue";
	const char* value = nullptr;

	// expect
	EXPECT_CALL(*mockNiceBeacon, reportValueString(testing::Eq(ACTION_ID), testing::Eq(eventName), testing::Eq(value)))
		.Times(testing::Exactly(1));

	// given
	auto target = createAction();

	//when
	target->reportValue(eventName, value);
}

TEST_F(ActionCommonImplTest, reportErrorWithAllValuesSet)
{
	// with
	const char* errorName = "FATAL Error";
	const int32_t errorCode = 0x8005037;
	const char* errorReason = "Some reason for this fatal error";

	// expect
	EXPECT_CALL(*mockNiceBeacon, reportError(testing::Eq(ACTION_ID), testing::Eq(errorName), testing::Eq(errorCode), testing::Eq(errorReason)))
		.Times(testing::Exactly(1));

	// given
	auto target = createAction();

	//when
	target->reportError(errorName, errorCode, errorReason);
}

TEST_F(ActionCommonImplTest, reportErrorWithNullErrorNameDoesNotReportTheError)
{
	// given
	const char* errorName = nullptr;
	const int32_t errorCode = 0x8005037;
	const char* errorReason = "Some reason for this fatal error";
	auto target = createAction();

	// expect
	std::stringstream stream;
	stream << target->toString() << " reportError: errorName must not be null or empty";
	EXPECT_CALL(*mockNiceLogger, mockWarning(stream.str()))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockNiceBeacon, reportError(testing::_, testing::_, testing::_, testing::_))
		.Times(testing::Exactly(0));

	//when
	target->reportError(errorName, errorCode, errorReason);
}

TEST_F(ActionCommonImplTest, reportErrorWithEmptyErrorNameDoesNotReportTheError)
{
	// given
	const char* errorName = "";
	const int32_t errorCode = 0x8005037;
	const char* errorReason = "Some reason for this fatal error";
	auto target = createAction();

	// expect
	std::stringstream stream;
	stream << target->toString() << " reportError: errorName must not be null or empty";
	EXPECT_CALL(*mockNiceLogger, mockWarning(stream.str()))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockNiceBeacon, reportError(testing::_, testing::_, testing::_, testing::_))
		.Times(testing::Exactly(0));

	//when
	target->reportError(errorName, errorCode, errorReason);
}

TEST_F(ActionCommonImplTest, reportErrorWithEmptyNullErrorReasonDoesReport)
{
	// with
	const char* errorName = "FATAL error";
	const int32_t errorCode = 0x8005037;
	const char* errorReason = nullptr;

	// expect
	EXPECT_CALL(*mockNiceBeacon, reportError(testing::Eq(ACTION_ID), testing::Eq(errorName), testing::Eq(errorCode), testing::Eq(errorReason)))
		.Times(testing::Exactly(1));

	// given
	auto target = createAction();

	//when
	target->reportError(errorName, errorCode, errorReason);
}

TEST_F(ActionCommonImplTest, traceWebRequestWithValidUrlStringGivesAppropriateTracer)
{
	// with
	const char* url = "http://example.com/pages/";

	// given
	auto target = createAction();

	// when
	auto obtained = target->traceWebRequest(url);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(std::dynamic_pointer_cast<WebRequestTracer_t>(obtained), testing::NotNull());

	// break dependency cycle: obtained contained in child objects of target
	obtained->stop(0);
}

TEST_F(ActionCommonImplTest, traceWebRequestWithUrlContainingParameters)
{
	// with
	Utf8String_t url("http://example.com/pages/");
	Utf8String_t parameters("someParameter=hello&someOtherParameter=world");
	Utf8String_t urlWithParameters(url);
	urlWithParameters.concatenate("?");
	urlWithParameters.concatenate(parameters);

	// given
	auto target = createAction();

	// when
	auto obtained = target->traceWebRequest(urlWithParameters.getStringData().c_str());

	// then
	ASSERT_THAT(std::static_pointer_cast<WebRequestTracer_t>(obtained)->getURL(), testing::Eq(url));

	// break dependency cycle: obtained contained in child objects of target
	obtained->stop(0);
}

TEST_F(ActionCommonImplTest, onChildClosedRemovesChildFromList)
{
	// with
	auto childObject = std::make_shared<MockNiceIOpenKitObject_t>();

	// given
	auto target = createAction();
	target->storeChildInList(childObject);

	// when
	target->onChildClosed(childObject);

	// then
	auto childObjects = target->getCopyOfChildObjects();
	ASSERT_THAT(childObjects.size(), testing::Eq(0));
}

TEST_F(ActionCommonImplTest, tracingANullStringWebRequestIsNotAllowed)
{
	// given
	const char* url = nullptr;
	auto target = createAction();

	// expect
	std::stringstream stream;
	stream << target->toString() << " traceWebRequest (string): url must not be null or empty";
	EXPECT_CALL(*mockNiceLogger, mockWarning(stream.str()))
		.Times(testing::Exactly(1));

	// when
	auto obtained = target->traceWebRequest(url);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(std::dynamic_pointer_cast<NullWebRequestTracer_t>(obtained), testing::NotNull());

	// break dependency cycle: obtained contained in child objects of target
	obtained->stop(0);
}

TEST_F(ActionCommonImplTest, tracingAnEmptyStringWebRequestIsNotAllowed)
{
	// given
	const char* url = "";
	auto target = createAction();

	// expect
	std::stringstream stream;
	stream << target->toString() << " traceWebRequest (string): url must not be null or empty";
	EXPECT_CALL(*mockNiceLogger, mockWarning(stream.str()))
		.Times(testing::Exactly(1));


	// when
	auto obtained = target->traceWebRequest(url);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(std::dynamic_pointer_cast<NullWebRequestTracer_t>(obtained), testing::NotNull());

	// break dependency cycle: obtained contained in child objects of target
	obtained->stop(0);
}

TEST_F(ActionCommonImplTest, traceWebRequestWithInvalidUrlIsNotAllowed)
{
	// with
	// given
	const char* url = "foobar/://";
	auto target = createAction();

	// expect
	std::stringstream stream;
	stream << target->toString() << " traceWebRequest (string): url \"" << url << "\" does not have a valid scheme";
	EXPECT_CALL(*mockNiceLogger, mockWarning(stream.str()))
		.Times(testing::Exactly(1));


	// when
	auto obtained = target->traceWebRequest(url);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(std::dynamic_pointer_cast<NullWebRequestTracer_t>(obtained), testing::NotNull());

	// break dependency cycle: obtained contained in child objects of target
	obtained->stop(0);
}

TEST_F(ActionCommonImplTest, parentActionIdIsInitializedInTheConstructor)
{
	// with
	const int32_t parentId = 666;

	// given
	ON_CALL(*mockParent, getActionId())
		.WillByDefault(testing::Return(parentId));

	// when
	auto target = createAction();

	// then
	ASSERT_THAT(target->getParentID(), testing::Eq(parentId));
}

TEST_F(ActionCommonImplTest, idIsIntializedInTheConstructor)
{
	// with
	const int32_t id = 777;

	// given
	ON_CALL(*mockNiceBeacon, createID())
		.WillByDefault(testing::Return(id));

	// when
	auto target = createAction();

	// then
	ASSERT_THAT(target->getID(), testing::Eq(id));
}

TEST_F(ActionCommonImplTest, nameIsInitializedInTheConstructor)
{
	// given, when
	auto target = createAction();

	// then
	ASSERT_THAT(target->getName(), testing::Eq(ACTION_NAME));
}

TEST_F(ActionCommonImplTest, startTimeIsInitializedInTheConstructor)
{
	// with
	const int64_t timestamp = 123456;

	// expect
	EXPECT_CALL(*mockNiceBeacon, getCurrentTimestamp())
		.Times(testing::Exactly(1));

	// given
	ON_CALL(*mockNiceBeacon, getCurrentTimestamp())
		.WillByDefault(testing::Return(timestamp));

	// when
	auto target = createAction();

	// then
	ASSERT_THAT(target->getStartTime(), testing::Eq(timestamp));
}

TEST_F(ActionCommonImplTest, endTimeIsMinusOneForNewlyCreatedAction)
{
	// given, when
	auto target = createAction();

	// then
	ASSERT_THAT(target->getEndTime(), testing::Eq(-1));
}

TEST_F(ActionCommonImplTest, startSequenceNumberIsInitializedInTheConstructor)
{
	// with
	int32_t sequenceNumber = 1234;

	// expect
	EXPECT_CALL(*mockNiceBeacon, createSequenceNumber())
		.Times(testing::Exactly(1));

	// given
	ON_CALL(*mockNiceBeacon, createSequenceNumber())
		.WillByDefault(testing::Return(sequenceNumber));

	// when
	auto target = createAction();

	// then
	ASSERT_THAT(target->getStartSequenceNumber(), testing::Eq(sequenceNumber));
}

TEST_F(ActionCommonImplTest, endSequenceNumberIsMinusOneForNewlyCreatedAction)
{
	// given, when
	auto target = createAction();

	// then
	ASSERT_THAT(target->getEndSequenceNumber(), testing::Eq(-1));
}

TEST_F(ActionCommonImplTest, aNewlyCreatedActionIsNotLeft)
{
	// given, when
	auto target = createAction();

	// then
	ASSERT_THAT(target->isActionLeft(), testing::Eq(false));
}

TEST_F(ActionCommonImplTest, afterLeavingAnActionItIsLeft)
{
	// given
	auto target = createAction();

	// when
	target->leaveAction();

	// then
	ASSERT_THAT(target->isActionLeft(), testing::Eq(true));
}

TEST_F(ActionCommonImplTest, leavingAnActionSetsTheEndTime)
{
	// with
	int64_t endTime = 1234;

	// expect
	EXPECT_CALL(*mockNiceBeacon, getCurrentTimestamp())
		.Times(testing::Exactly(2));

	// given
	ON_CALL(*mockNiceBeacon, getCurrentTimestamp())
		.WillByDefault(testing::Return(endTime));
	auto target = createAction();

	// when
	target->leaveAction();

	// then
	ASSERT_THAT(target->getEndTime(), testing::Eq(endTime));
}

TEST_F(ActionCommonImplTest, leavingAnActionSetsTheEndSequenceNumber)
{
	// with
	int64_t endSequenceNumber = 1234;

	// expect
	EXPECT_CALL(*mockNiceBeacon, createSequenceNumber())
		.Times(testing::Exactly(2));

	// given
	ON_CALL(*mockNiceBeacon, createSequenceNumber())
		.WillByDefault(testing::Return(endSequenceNumber));
	auto target = createAction();

	// when
	target->leaveAction();

	// then
	ASSERT_THAT(target->getEndSequenceNumber(), testing::Eq(endSequenceNumber));
}

TEST_F(ActionCommonImplTest, leavingAnActionSerializesItself)
{
	// expect
	EXPECT_CALL(*mockNiceBeacon, mockAddAction(testing::_))
		.Times(testing::Exactly(1));

	// given
	auto target = createAction();

	// when
	target->leaveAction();
}

TEST_F(ActionCommonImplTest, leavingAnActionClosesAllChildObjects)
{
	// with
	auto childObjectOne = std::make_shared<MockNiceIOpenKitObject_t>();
	auto childObjectTwo = std::make_shared<MockNiceIOpenKitObject_t>();

	// expect
	EXPECT_CALL(*childObjectOne, close()).Times(testing::Exactly(1));
	EXPECT_CALL(*childObjectTwo, close()).Times(testing::Exactly(1));

	// given
	auto target = createAction();
	target->storeChildInList(childObjectOne);
	target->storeChildInList(childObjectTwo);

	// when
	target->leaveAction();
}

TEST_F(ActionCommonImplTest, leavingAnActionNotifiesTheParentCompositeObject)
{
	// expect
	EXPECT_CALL(*mockParent, onChildClosed(testing::_)).Times(testing::Exactly(1));

	// given
	auto target = createAction();

	// when
	target->leaveAction();
}

TEST_F(ActionCommonImplTest, leavingAnActionReturnsTrueIfNotYetLeft)
{
	// given
	auto target = createAction();

	// when
	auto obtained = target->leaveAction();

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
}

TEST_F(ActionCommonImplTest, leavingAnActionReturnsFalseIfAlreadyLeft)
{
	// given
	auto target = createAction();
	target->leaveAction();

	// when
	auto obtained = target->leaveAction();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(ActionCommonImplTest, leavingAnAlreadyLeftActionReturnsImmediately)
{
	// expect
	EXPECT_CALL(*mockNiceBeacon, mockAddAction(testing::_)).Times(testing::Exactly(1));

	// given
	auto target = createAction();
	target->leaveAction();

	// when
	target->leaveAction();
}

TEST_F(ActionCommonImplTest, leaveActionRemovesItSelfFromParent)
{
	// with
	auto rootAction = std::make_shared<MockStrictIRootAction_t>();

	// given
	auto target = createAction();

	// when
	auto obtained = target->enterAction(rootAction, "child Action");

	auto childObjects = target->getCopyOfChildObjects();
	ASSERT_THAT(childObjects.size(), testing::Eq(1));

	// and when
	obtained->leaveAction();

	// then
	childObjects = target->getCopyOfChildObjects();
	ASSERT_THAT(childObjects.size(), testing::Eq(0));
}

TEST_F(ActionCommonImplTest, leaveActionSetsEndTimeBeforeAddingToBeacon)
{
	// expect
	{
		testing::InSequence s;

		EXPECT_CALL(*mockNiceBeacon, getCurrentTimestamp()).Times(1); 	// root action constructor
		EXPECT_CALL(*mockNiceBeacon, createSequenceNumber()).Times(1); 	// root action constructor

		EXPECT_CALL(*mockNiceBeacon, getCurrentTimestamp()).Times(1); 	// set end timestamp
		EXPECT_CALL(*mockNiceBeacon, createSequenceNumber()).Times(1); 	// set end sequence number
		EXPECT_CALL(*mockNiceBeacon, mockAddAction(testing::_));
	}

	// given
	auto target = createAction();

	// when
	target->leaveAction();
}

TEST_F(ActionCommonImplTest, leaveActionLeavesChildActionsBeforeSettingEndTime)
{
	// expect
	{
		testing::InSequence s;
		EXPECT_CALL(*mockNiceBeacon, getCurrentTimestamp());	// constructor root action
		EXPECT_CALL(*mockNiceBeacon, createSequenceNumber());	// constructor root action

		EXPECT_CALL(*mockNiceBeacon, getCurrentTimestamp());	// constructor child action
		EXPECT_CALL(*mockNiceBeacon, createSequenceNumber());	// constructor child action


		EXPECT_CALL(*mockNiceBeacon, getCurrentTimestamp());	// set end time on child action
		EXPECT_CALL(*mockNiceBeacon, createSequenceNumber());	// set end sequence number on child action
		EXPECT_CALL(*mockNiceBeacon, mockAddAction(testing::_));
		EXPECT_CALL(*mockNiceBeacon, getCurrentTimestamp());	// set end time on root action
		EXPECT_CALL(*mockNiceBeacon, createSequenceNumber());	// set end sequence number on root action
		EXPECT_CALL(*mockNiceBeacon, mockAddAction(testing::_));
	}

	// given
	auto rootAction =  std::make_shared<MockStrictIRootAction_t>();
	auto target = createAction();
	target->enterAction(rootAction, "child action");

	// when
	target->leaveAction();
}

TEST_F(ActionCommonImplTest, reportEventDoesNothingIfActionIsLeft)
{
	// with
	const char* eventName = "eventName";
	const int64_t timestamp = 1234;
	const int32_t sequenceNumber = 73;

	// expect
	EXPECT_CALL(*mockNiceBeacon, reportEvent(testing::_, testing::_))
		.Times(testing::Exactly(0));

	//given
	ON_CALL(*mockNiceBeacon, getCurrentTimestamp())
		.WillByDefault(testing::Return(timestamp));
	ON_CALL(*mockNiceBeacon, createSequenceNumber())
		.WillByDefault(testing::Return(sequenceNumber));

	auto target = createAction();
	target->leaveAction();

	//when
	target->reportEvent(eventName);
}

TEST_F(ActionCommonImplTest, reportIntValueDoesNothingIfActionIsLeft)
{
	// with
	const char* eventName = "intValue";
	const int64_t timestamp = 1234;
	const int32_t sequenceNumber = 73;
	const int32_t value = 42;

	// expect
	EXPECT_CALL(*mockNiceBeacon, reportValueInt32(testing::_, testing::_, testing::_))
		.Times(testing::Exactly(0));

	//given
	ON_CALL(*mockNiceBeacon, getCurrentTimestamp())
		.WillByDefault(testing::Return(timestamp));
	ON_CALL(*mockNiceBeacon, createSequenceNumber())
		.WillByDefault(testing::Return(sequenceNumber));

	auto target = createAction();
	target->leaveAction();

	//when
	target->reportValue(eventName, value);
}

TEST_F(ActionCommonImplTest, reportDoubleValueDoesNothingIfActionIsLeft)
{
	// with
	const char* eventName = "doubleValue";
	const int64_t timestamp = 1234;
	const int32_t sequenceNumber = 73;
	const double value = 42.1337;

	// expect
	EXPECT_CALL(*mockNiceBeacon, reportValueDouble(testing::_, testing::_, testing::_))
		.Times(testing::Exactly(0));

	//given
	ON_CALL(*mockNiceBeacon, getCurrentTimestamp())
		.WillByDefault(testing::Return(timestamp));
	ON_CALL(*mockNiceBeacon, createSequenceNumber())
		.WillByDefault(testing::Return(sequenceNumber));

	auto target = createAction();
	target->leaveAction();

	//when
	target->reportValue(eventName, value);
}

TEST_F(ActionCommonImplTest, reportStringValueDoesNothingIfActionIsLeft)
{
	// with
	const char* eventName = "doubleValue";
	const int64_t timestamp = 1234;
	const int32_t sequenceNumber = 73;
	const char* value = "42";

	// expect
	EXPECT_CALL(*mockNiceBeacon, reportValueString(testing::_, testing::_, testing::_))
		.Times(testing::Exactly(0));

	//given
	ON_CALL(*mockNiceBeacon, getCurrentTimestamp())
		.WillByDefault(testing::Return(timestamp));
	ON_CALL(*mockNiceBeacon, createSequenceNumber())
		.WillByDefault(testing::Return(sequenceNumber));

	auto target = createAction();
	target->leaveAction();

	//when
	target->reportValue(eventName, value);
}


TEST_F(ActionCommonImplTest, reportErrorDoesNothingIfActionIsLeft)
{
	// with
	const char* errorName = "teapot";
	const int32_t errorCode = 418;
	const char* errorReason = "I'm a teapot";
	const int64_t timestamp = 1234;
	const int32_t sequenceNumber = 73;

	// expect
	EXPECT_CALL(*mockNiceBeacon, reportError(testing::_, testing::_, testing::_, testing::_))
		.Times(testing::Exactly(0));

	//given
	ON_CALL(*mockNiceBeacon, getCurrentTimestamp())
		.WillByDefault(testing::Return(timestamp));
	ON_CALL(*mockNiceBeacon, createSequenceNumber())
		.WillByDefault(testing::Return(sequenceNumber));

	auto target = createAction();
	target->leaveAction();

	//when
	target->reportError(errorName, errorCode, errorReason);
}

TEST_F(ActionCommonImplTest, closeActionLeavesTheAction)
{
	// with
	const int64_t timestamp = 1234;
	const int32_t sequenceNumber = 42;

	// expect
	EXPECT_CALL(*mockNiceBeacon, mockAddAction(testing::_))
		.Times(testing::Exactly(1));

	// given
	ON_CALL(*mockNiceBeacon, getCurrentTimestamp()).WillByDefault(testing::Return(timestamp));
	ON_CALL(*mockNiceBeacon, createSequenceNumber()).WillByDefault(testing::Return(sequenceNumber));

	auto target = createAction();

	// when
	target->close();

	// then
	ASSERT_THAT(target->getEndTime(), testing::Eq(timestamp));
	ASSERT_THAT(target->getEndSequenceNumber(), testing::Eq(sequenceNumber));
}

TEST_F(ActionCommonImplTest, enterActionWithNullNameGivesNullAction)
{
	// given
	const char* actionName = nullptr;
	auto rootAction = std::make_shared<MockStrictIRootAction_t>();
	auto target = createAction();

	// expect
	std::stringstream stream;
	stream << target->toString() << " enterAction: actionName must not be null or empty";
	EXPECT_CALL(*mockNiceLogger, mockWarning(stream.str()))
		.Times(testing::Exactly(1));


	// when
	auto obtained = target->enterAction(rootAction, actionName);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	auto nullAction = std::dynamic_pointer_cast<NullAction_t>(obtained);
	ASSERT_THAT(nullAction, testing::NotNull());

	// and when
	auto obtainedRootAction = nullAction->leaveAction(); // breaks also dependency cycle: obtained contained in child objects of target
	ASSERT_THAT(obtainedRootAction, testing::NotNull());
	ASSERT_THAT(std::dynamic_pointer_cast<IRootAction_t>(obtainedRootAction), testing::Eq(rootAction));
}

TEST_F(ActionCommonImplTest, enterActionWithEmptyNameGivesNullAction)
{
	// given
	const char* actionName = "";
	auto rootAction = std::make_shared<MockStrictIRootAction_t>();
	auto target = createAction();

	// expect
	std::stringstream stream;
	stream << target->toString() << " enterAction: actionName must not be null or empty";
	EXPECT_CALL(*mockNiceLogger, mockWarning(stream.str()))
		.Times(testing::Exactly(1));

	// when
	auto obtained = target->enterAction(rootAction, actionName);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	auto nullAction = std::dynamic_pointer_cast<NullAction_t>(obtained);
	ASSERT_THAT(nullAction, testing::NotNull());

	// and when
	auto obtainedRootAction = nullAction->leaveAction(); // breaks also dependency cycle: obtained contained in child objects of target
	ASSERT_THAT(obtainedRootAction, testing::NotNull());
	ASSERT_THAT(std::dynamic_pointer_cast<IRootAction_t>(obtainedRootAction), testing::Eq(rootAction));
}

TEST_F(ActionCommonImplTest, enterActionGivesLeafActionInstance)
{
	// with
	const char* actionName = "child action";
	auto rootAction = std::make_shared<MockStrictIRootAction_t>();

	// given
	auto target = createAction();

	// when
	auto obtained = target->enterAction(rootAction, actionName);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(std::dynamic_pointer_cast<LeafAction_t>(obtained), testing::NotNull());

	// break dependency cycle: obtained contained in child objects of target
	obtained->leaveAction();
}

TEST_F(ActionCommonImplTest, enterActionAddsLeafActionToListOfChildObjects)
{
	// with
	const char* actionName = "child action";
	auto rootAction = std::make_shared<MockStrictIRootAction_t>();

	// given
	auto target = createAction();

	// when
	auto obtained = target->enterAction(rootAction, actionName);

	// then
	auto childObjects = target->getCopyOfChildObjects();
	ASSERT_THAT(childObjects.size(), testing::Eq(1));
	auto leafAction = std::dynamic_pointer_cast<LeafAction_t>(obtained);
	ASSERT_THAT(leafAction, testing::NotNull());

	auto actionImpl = std::dynamic_pointer_cast<IActionCommon_t>(*childObjects.begin());
	ASSERT_THAT(actionImpl, testing::NotNull());
	ASSERT_THAT(leafAction->getActionImpl(), testing::Eq(actionImpl));

	// break dependency cycle: obtained contained in child objects of target
	obtained->leaveAction();
}

TEST_F(ActionCommonImplTest, enterActionGivesNullActionIfAlreadyLeft)
{
	// with
	const char* actionName = "child action";
	auto rootAction = std::make_shared<MockStrictIRootAction_t>();

	// given
	auto target = createAction();
	target->leaveAction();

	// when
	auto obtained = target->enterAction(rootAction, actionName);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	auto nullAction = std::dynamic_pointer_cast<NullAction_t>(obtained);
	ASSERT_THAT(nullAction, testing::NotNull());

	// and when
	auto obtainedRootAction = nullAction->leaveAction();
	ASSERT_THAT(obtainedRootAction, testing::NotNull());
	ASSERT_THAT(std::dynamic_pointer_cast<IRootAction_t>(obtainedRootAction), testing::Eq(rootAction));
}

TEST_F(ActionCommonImplTest, toStringReturnsAppropriateResult)
{
	// with
	const int32_t sessionNumber = 21;
	const int32_t actionId = 42;
	const int32_t parentId = 73;

	// given
	ON_CALL(*mockNiceBeacon, getSessionNumber())
		.WillByDefault(testing::Return(sessionNumber));
	ON_CALL(*mockNiceBeacon, createID())
		.WillByDefault(testing::Return(actionId));
	ON_CALL(*mockParent, getActionId())
		.WillByDefault(testing::Return(parentId));

	auto target = createAction();

	// when
	auto obtained = target->toString();

	// then
	std::stringstream ss;
	ss << ACTION_NAME.getStringData()
		<< " [sn=" << sessionNumber
		<< ", id=" << actionId
		<< ", name=" << ACTION_NAME.getStringData()
		<< ", pa=" << parentId
		<< "]";
	ASSERT_THAT(obtained, testing::Eq(ss.str()));
}