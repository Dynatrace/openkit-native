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

#include "../../api/mock/MockIRootAction.h"

#include "core/objects/NullAction.h"
#include "core/objects/NullWebRequestTracer.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <memory>

using namespace test;

using MockIRootAction_sp = std::shared_ptr<MockIRootAction>;
using NullAction_t = core::objects::NullAction;
using NullAction_sp = std::shared_ptr<NullAction_t>;
using NullWebRequestTracer_t = core::objects::NullWebRequestTracer;

class NullActionTest : public testing::Test
{
protected:
	MockIRootAction_sp mockParent;

	void SetUp() override
	{
		mockParent = MockIRootAction::createStrict();
	}

	NullAction_sp createNullAction()
	{
		return std::make_shared<NullAction_t>(mockParent);
	}
};

TEST_F(NullActionTest, createNewInstance)
{
	// given, when, verify zero interaction with parent
	createNullAction();
}

TEST_F(NullActionTest, reportEventReturnsSelf)
{
	// given
	auto target = createNullAction();

	// when
	auto obtained = target->reportEvent("event name");

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	auto nullAction = std::dynamic_pointer_cast<NullAction_t>(obtained);
	ASSERT_THAT(nullAction, testing::NotNull());
	ASSERT_THAT(nullAction, testing::Eq(target));
}

TEST_F(NullActionTest, reportInt32ValueReturnsSelf)
{
	// given
	auto target = createNullAction();

	// when
	auto obtained = target->reportValue("value name", int32_t(12));

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	auto nullAction = std::dynamic_pointer_cast<NullAction_t>(obtained);
	ASSERT_THAT(nullAction, testing::NotNull());
	ASSERT_THAT(nullAction, testing::Eq(target));
}

TEST_F(NullActionTest, reportInt64ValueReturnsSelf)
{
	// given
	auto target = createNullAction();

	// when
	auto obtained = target->reportValue("value name", int64_t(24));

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	auto nullAction = std::dynamic_pointer_cast<NullAction_t>(obtained);
	ASSERT_THAT(nullAction, testing::NotNull());
	ASSERT_THAT(nullAction, testing::Eq(target));
}

TEST_F(NullActionTest, reportDoubleValueReturnsSelf)
{
	// given
	auto target = createNullAction();

	// when
	auto obtained = target->reportValue("value name", 37.73);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	auto nullAction = std::dynamic_pointer_cast<NullAction_t>(obtained);
	ASSERT_THAT(nullAction, testing::NotNull());
	ASSERT_THAT(nullAction, testing::Eq(target));
}

TEST_F(NullActionTest, reportStringValueReturnsSelf)
{
	// given
	auto target = createNullAction();

	// when
	auto obtained = target->reportValue("value name", "value");

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	auto nullAction = std::dynamic_pointer_cast<NullAction_t>(obtained);
	ASSERT_THAT(nullAction, testing::NotNull());
	ASSERT_THAT(nullAction, testing::Eq(target));
}

TEST_F(NullActionTest, reportErrorReturnsSelf)
{
	// given
	auto target = createNullAction();

	// when
	auto obtained = target->reportError("error name", 1337, "something bad");

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	auto nullAction = std::dynamic_pointer_cast<NullAction_t>(obtained);
	ASSERT_THAT(nullAction, testing::NotNull());
	ASSERT_THAT(nullAction, testing::Eq(target));
}

TEST_F(NullActionTest, traceWebRequestReturnsNullWebRequestTracer)
{
	// given
	auto target = createNullAction();

	// when
	auto obtained = target->traceWebRequest("https://localhost");

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	auto nullTracer = std::dynamic_pointer_cast<NullWebRequestTracer_t>(obtained);
	ASSERT_THAT(nullTracer, testing::NotNull());
	ASSERT_THAT(nullTracer, testing::Eq(NullWebRequestTracer_t::instance()));
}

TEST_F(NullActionTest, leaveActionWithNullParent)
{
	// given
	NullAction_t target(nullptr);

	// when
	auto obtained = target.leaveAction();

	// then
	ASSERT_THAT(obtained, testing::IsNull());
}