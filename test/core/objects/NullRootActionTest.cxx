/**
 * Copyright 2018-2020 Dynatrace LLC
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

#include "core/objects/NullAction.h"
#include "core/objects/NullRootAction.h"
#include "core/objects/NullWebRequestTracer.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using NullAction_t = core::objects::NullAction;
using NullRootAction_t = core::objects::NullRootAction;
using NullWebRequestTracer_t = core::objects::NullWebRequestTracer;

class NullRootActionTest : public testing::Test
{
};

TEST_F(NullRootActionTest, enterActionReturnsNewNullAction)
{
	// given
	auto target = NullRootAction_t::instance();

	// when
	auto obtained = target->enterAction("action name");

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(std::dynamic_pointer_cast<NullAction_t>(obtained), testing::NotNull());
}

TEST_F(NullRootActionTest, enteredActionHasNullRootActionAsParent)
{
	// given
	auto target = NullRootAction_t::instance();

	// when
	auto action = target->enterAction("action name");
	auto obtained = action->leaveAction();

	// then
	auto nullRootAction = std::dynamic_pointer_cast<NullRootAction_t>(obtained);
	ASSERT_THAT(nullRootAction, testing::NotNull());
	ASSERT_THAT(nullRootAction, testing::Eq(target));
}

TEST_F(NullRootActionTest, reportEventReturnsSelf)
{
	// given
	auto target = NullRootAction_t::instance();

	// when
	auto obtained = target->reportEvent("event name");

	// then
	auto nullRootAction = std::dynamic_pointer_cast<NullRootAction_t>(obtained);
	ASSERT_THAT(nullRootAction, testing::NotNull());
	ASSERT_THAT(nullRootAction, testing::Eq(target));
}

TEST_F(NullRootActionTest, reportInt32ValueReturnsSelf)
{
	// given
	auto target = NullRootAction_t::instance();

	// when
	auto obtained = target->reportValue("value name", int32_t(12));

	// then
	auto nullRootAction = std::dynamic_pointer_cast<NullRootAction_t>(obtained);
	ASSERT_THAT(nullRootAction, testing::NotNull());
	ASSERT_THAT(nullRootAction, testing::Eq(target));
}

TEST_F(NullRootActionTest, reportInt64ValueReturnsSelf)
{
	// given
	auto target = NullRootAction_t::instance();

	// when
	auto obtained = target->reportValue("value name", int64_t(12));

	// then
	auto nullRootAction = std::dynamic_pointer_cast<NullRootAction_t>(obtained);
	ASSERT_THAT(nullRootAction, testing::NotNull());
	ASSERT_THAT(nullRootAction, testing::Eq(target));
}

TEST_F(NullRootActionTest, reportDoubleValueReturnsSelf)
{
	// given
	auto target = NullRootAction_t::instance();

	// when
	auto obtained = target->reportValue("value name", 37.73);

	// then
	auto nullRootAction = std::dynamic_pointer_cast<NullRootAction_t>(obtained);
	ASSERT_THAT(nullRootAction, testing::NotNull());
	ASSERT_THAT(nullRootAction, testing::Eq(target));
}

TEST_F(NullRootActionTest, reportStringValueReturnsSelf)
{
	// given
	auto target = NullRootAction_t::instance();

	// when
	auto obtained = target->reportValue("value name", "value");

	// then
	auto nullRootAction = std::dynamic_pointer_cast<NullRootAction_t>(obtained);
	ASSERT_THAT(nullRootAction, testing::NotNull());
	ASSERT_THAT(nullRootAction, testing::Eq(target));
}

TEST_F(NullRootActionTest, traceWebRequestReturnsNullWebRequestTracer)
{
	// given
	auto target = NullRootAction_t::instance();

	// when
	auto obtained = target->traceWebRequest("https://localhost");

	// then
	auto nullRootAction = std::dynamic_pointer_cast<NullWebRequestTracer_t>(obtained);
	ASSERT_THAT(nullRootAction, testing::NotNull());
	ASSERT_THAT(nullRootAction, testing::Eq(NullWebRequestTracer_t::instance()));
}