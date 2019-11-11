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

#include "mock/MockIActionCommon.h"
#include "mock/MockIWebRequestTracerInternals.h"

#include "core/objects/RootAction.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <memory>

using namespace test;

using MockStrictIActionCommon_t = testing::StrictMock<MockIActionCommon>;
using MockStrictIActionCommon_sp = std::shared_ptr<MockStrictIActionCommon_t>;
using RootAction_t = core::objects::RootAction;
using RootAction_sp = std::shared_ptr<RootAction_t>;

class RootActionTest : public testing::Test
{
protected:

	MockStrictIActionCommon_sp mockActionImpl;

	void SetUp() override
	{
		mockActionImpl = std::make_shared<MockStrictIActionCommon_t>();
	}

	RootAction_sp createAction()
	{
		return std::make_shared<RootAction_t>(mockActionImpl);
	}
};

TEST_F(RootActionTest, enterActionDelegatesToCommonImpl)
{
	// with
	const char* actionName = "root action";

	// expect
	EXPECT_CALL(*mockActionImpl, enterAction(testing::_, actionName)).Times(testing::Exactly(1));

	// given
	auto target = createAction();

	// when
	target->enterAction(actionName);
}

TEST_F(RootActionTest, reportEventDelegatesToCommonImpl)
{
	// with
	const char* eventName = "event name";

	// expect
	EXPECT_CALL(*mockActionImpl, reportEvent(eventName)).Times(testing::Exactly(1));

	// given
	auto target = createAction();

	// when
	target->reportEvent(eventName);
}

TEST_F(RootActionTest, reportValueIntDelegatsToCommonImpl)
{
	// with
	const char* valueName = "IntValue";
	int32_t value = 42;

	// expect
	EXPECT_CALL(*mockActionImpl, reportValue(testing::Eq(valueName), testing::TypedEq<int32_t>(value)))
		.Times(testing::Exactly(1));

	// given
	auto target = createAction();

	// when
	target->reportValue(valueName, value);
}

TEST_F(RootActionTest, reportValueDoubleDelegatsToCommonImpl)
{
	// with
	const char* valueName = "DoubleValue";
	double value = 42.1337;

	// expect
	EXPECT_CALL(*mockActionImpl, reportValue(testing::Eq(valueName), testing::TypedEq<double>(value)))
		.Times(testing::Exactly(1));

	// given
	auto target = createAction();

	// when
	target->reportValue(valueName, value);
}

TEST_F(RootActionTest, reportValueStringDelegatsToCommonImpl)
{
	// with
	const char* valueName = "StringValue";
	const char* value = "string value";

	// expect
	EXPECT_CALL(*mockActionImpl, reportValue(testing::Eq(valueName), testing::TypedEq<const char*>(value)))
		.Times(testing::Exactly(1));

	// given
	auto target = createAction();

	// when
	target->reportValue(valueName, value);
}

TEST_F(RootActionTest, reportErrorDelegatsToCommonImpl)
{
	// with
	const char* errorName = "error name";
	const int32_t errorCode = 42;
	const char* errorReason = "some reason";

	// expect
	EXPECT_CALL(*mockActionImpl, reportError(errorName, errorCode, errorReason)).Times(testing::Exactly(1));

	// given
	auto target = createAction();

	// when
	target->reportError(errorName, errorCode, errorReason);
}

TEST_F(RootActionTest, traceWebRequestDelegatesToCommonImpl)
{
	// with
	const char* url = "https::localhost:9999/1";
	auto tracer = MockIWebRequestTracerInternals::createStrict();

	// expect
	EXPECT_CALL(*mockActionImpl, traceWebRequest(url)).Times(testing::Exactly(1));

	// given
	ON_CALL(*mockActionImpl, traceWebRequest(testing::_))
		.WillByDefault(testing::Return(tracer));

	auto target = createAction();

	// when
	auto obtained = target->traceWebRequest(url);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained, testing::Eq(tracer));
}

TEST_F(RootActionTest, leaveActionDelegatesToCommonImpl)
{
	// expect
	EXPECT_CALL(*mockActionImpl, leaveAction()).Times(testing::Exactly(1));

	// given
	auto target = createAction();

	// when
	target->leaveAction();
}