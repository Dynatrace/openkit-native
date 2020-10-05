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

#include "core/objects/NullRootAction.h"
#include "core/objects/NullSession.h"
#include "core/objects/NullWebRequestTracer.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using NullRootAction_t = core::objects::NullRootAction;
using NullSession_t = core::objects::NullSession;
using NullWebRequestTracer_t = core::objects::NullWebRequestTracer;

class NullSessionTest : public testing::Test
{
};

TEST_F(NullSessionTest, enterActionReturnsNullRootAction)
{
	// given
	auto target = NullSession_t::instance();

	// when
	auto obtained = target->enterAction("action name");

	// then
	auto nullRootAction = std::dynamic_pointer_cast<NullRootAction_t>(obtained);
	ASSERT_THAT(nullRootAction, testing::NotNull());
	ASSERT_THAT(nullRootAction, testing::Eq(NullRootAction_t::instance()));
}

TEST_F(NullSessionTest, traceWebRequestReturnsNullWebRequestTracer)
{
	// given
	auto target = NullSession_t::instance();

	// when
	auto obtained = target->traceWebRequest("https://localhost");

	// then
	auto nullTracer = std::dynamic_pointer_cast<NullWebRequestTracer_t>(obtained);
	ASSERT_THAT(nullTracer, testing::NotNull());
	ASSERT_THAT(nullTracer, testing::Eq(NullWebRequestTracer_t::instance()));
}