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

#include "core/objects/NullWebRequestTracer.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using NullWebRequestTracer_t = core::objects::NullWebRequestTracer;

class NullWebRequestTracerTest : public testing::Test
{
};

TEST_F(NullWebRequestTracerTest, getTagReturnsEmptyString)
{
	// given
	auto target = NullWebRequestTracer_t::INSTANCE;

	// when
	auto obtained = target->getTag();

	// then
	ASSERT_THAT(obtained, testing::StrEq(""));
}

TEST_F(NullWebRequestTracerTest, setResponseCodeReturnsSelf)
{
	// given
	auto target = NullWebRequestTracer_t::INSTANCE;

	// when
	auto obtained = target->setResponseCode(200);

	// then
	auto nullTracer = std::dynamic_pointer_cast<NullWebRequestTracer_t>(obtained);
	ASSERT_THAT(nullTracer, testing::NotNull());
	ASSERT_THAT(nullTracer, testing::Eq(target));
}

TEST_F(NullWebRequestTracerTest, setBytesSentReturnsSelf)
{
	// given
	auto target = NullWebRequestTracer_t::INSTANCE;

	// when
	auto obtained = target->setBytesSent(37);

	// then
	auto nullTracer = std::dynamic_pointer_cast<NullWebRequestTracer_t>(obtained);
	ASSERT_THAT(nullTracer, testing::NotNull());
	ASSERT_THAT(nullTracer, testing::Eq(target));
}

TEST_F(NullWebRequestTracerTest, setBytesReceivedReturnsSelf)
{
	// given
	auto target = NullWebRequestTracer_t::INSTANCE;

	// when
	auto obtained = target->setBytesReceived(73);

	// then
	auto nullTracer = std::dynamic_pointer_cast<NullWebRequestTracer_t>(obtained);
	ASSERT_THAT(nullTracer, testing::NotNull());
	ASSERT_THAT(nullTracer, testing::Eq(target));
}

TEST_F(NullWebRequestTracerTest, startReturnsSelf)
{
	// given
	auto target = NullWebRequestTracer_t::INSTANCE;

	// when
	auto obtained = target->start();

	// then
	auto nullTracer = std::dynamic_pointer_cast<NullWebRequestTracer_t>(obtained);
	ASSERT_THAT(nullTracer, testing::NotNull());
	ASSERT_THAT(nullTracer, testing::Eq(target));
}