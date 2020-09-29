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

#include <providers/FixedSessionIDProvider.h>

#include "mock/MockISessionIDProvider.h"

#include <memory>

#include <gtest/gtest.h>

using MockISessionIDProvider_t = test::MockISessionIDProvider;
using FixedSessionIDProvider_t = providers::FixedSessionIDProvider;

class FixedSessionIDProviderTest : public testing::Test
{
};

TEST_F(FixedSessionIDProviderTest, getNextSessionIdReturnsAlwaysTheSameNumber)
{
	// given
	const int32_t sessionId = 777;

	auto sessionIdProviderMock = MockISessionIDProvider_t::createNice();
	EXPECT_CALL(*sessionIdProviderMock, getNextSessionID())
		.Times(1)
		.WillOnce(testing::Return(sessionId))
		.WillRepeatedly(testing::Return(1));

	FixedSessionIDProvider_t target(sessionIdProviderMock);

	// when, then
	for (auto i = 0; i < 100; i++)
	{
		auto obtained = target.getNextSessionID();
		ASSERT_THAT(obtained, testing::Eq(sessionId));
	}
}
