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

#include <providers/FixedPRNGenerator.h>

#include "mock/MockIPRNGenerator.h"

#include <gtest/gtest.h>

using FixedPRNGenerator_t = providers::FixedPRNGenerator;
using MockIPRNGenerator_t = test::MockIPRNGenerator;

class FixedPRNGeneratorTest : public testing::Test
{
};

TEST_F(FixedPRNGeneratorTest, nextPositiveInt64AlwaysReturnsTheSameNumber)
{
	// given
	const int64_t randomInt64 = 12345;

	auto rngMock = MockIPRNGenerator_t::createNice();
	EXPECT_CALL(*rngMock, nextPositiveInt64())
		.Times(1)
		.WillOnce(testing::Return(randomInt64));

	FixedPRNGenerator_t target(rngMock);

	// when, then
	for (auto i = 0; i < 100; i++)
	{
		auto obtained = target.nextPositiveInt64();
		ASSERT_THAT(obtained, testing::Eq(randomInt64));
	}
}

TEST_F(FixedPRNGeneratorTest, nextPercentageValueReturnsAlwaysTheSameNumber)
{
	// given
	const int32_t randomInt32 = 12345;

	auto rngMock = MockIPRNGenerator_t::createNice();
	EXPECT_CALL(*rngMock, nextPercentageValue())
		.Times(1)
		.WillOnce(testing::Return(randomInt32));

	FixedPRNGenerator_t target(rngMock);

	// when, then
	for (auto i = 0; i < 100; i++)
	{
		auto obtained = target.nextPercentageValue();
		ASSERT_THAT(obtained, testing::Eq(randomInt32));
	}
}
