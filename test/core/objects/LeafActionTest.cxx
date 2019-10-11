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
#include "../../api/MockTypes.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>

using namespace test::types;

class LeafActionTest : public testing::Test
{
protected:

	MockStrictIActionCommon_sp mockActionImpl;
	MockStrictIRootAction_sp mockRootAction;

	void SetUp()
	{
		mockActionImpl = std::make_shared<MockStrictIActionCommon_t>();
		mockRootAction = std::make_shared<MockStrictIRootAction_t>();
	}

	void TearDown()
	{
	}

	LeafAction_sp createAction()
	{
		return std::make_shared<LeafAction_t>(
			mockActionImpl,
			mockRootAction
		);
	}
};

TEST_F(LeafActionTest, reportEventDelegatesToCommonImpl)
{
	// with
	const char* eventName = "event name";

	// expect
	EXPECT_CALL(*mockActionImpl, reportEvent(eventName)).Times(testing::Exactly(1));

	// given
	auto target = createAction();

	// when
	auto obtained = target->reportEvent(eventName);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained, testing::Eq(target));
}

TEST_F(LeafActionTest, reportValueIntDelegatesToCommonImpl)
{
	// with
	const char* valueName = "IntValue";
	const int32_t value = 42;

	// expect
	EXPECT_CALL(*mockActionImpl, reportValueInt(valueName, value)).Times(testing::Exactly(1));

	// given
	auto target = createAction();

	// when
	auto obtained = target->reportValue(valueName, value);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained, testing::Eq(target));
}

TEST_F(LeafActionTest, reportValueDoubleDelegatesToCommonImpl)
{
	// with
	const char* valueName = "DoubleValue";
	const double value = 42.1337;

	// expect
	EXPECT_CALL(*mockActionImpl, reportValueDouble(valueName, value)).Times(testing::Exactly(1));

	// given
	auto target = createAction();

	// when
	auto obtained = target->reportValue(valueName, value);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained, testing::Eq(target));
}

TEST_F(LeafActionTest, reportValueStringDelegatesToCommonImpl)
{
	// with
	const char* valueName = "StringValue";
	const char* value = "string value";

	// expect
	EXPECT_CALL(*mockActionImpl, reportValueString(valueName, value)).Times(testing::Exactly(1));

	// given
	auto target = createAction();

	// when
	auto obtained = target->reportValue(valueName, value);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained, testing::Eq(target));
}

TEST_F(LeafActionTest, reportErrorDelegatsToCommonImpl)
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
	auto obtained = target->reportError(errorName, errorCode, errorReason);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained, testing::Eq(target));
}

TEST_F(LeafActionTest, traceWebRequestDelegatesToCommonImpl)
{
	// with
	const char* url = "https::localhost:9999/1";
	auto tracer = std::make_shared<MockStrictIWebRequestTracer_t>();

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

TEST_F(LeafActionTest, leaveActionDelegatesToCommonImpl)
{
	// expect
	EXPECT_CALL(*mockActionImpl, leaveAction()).Times(testing::Exactly(1));

	// given
	auto target = createAction();

	// when
	auto obtained = target->leaveAction();

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained, testing::Eq(mockRootAction));
}