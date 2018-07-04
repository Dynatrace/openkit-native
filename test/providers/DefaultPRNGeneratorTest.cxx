/**
* Copyright 2018 Dynatrace LLC
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

#include "providers/DefaultPRNGenerator.h"
#include <gtest/gtest.h>

#include <limits>

using namespace providers;

class DefaultPRNGeneratorTest : public testing::Test
{
public:
	DefaultPRNGeneratorTest()
	{
	}

	void SetUp()
	{
	}

	void TearDown()
	{
	}

public:
	/// the test target
	providers::DefaultPRNGenerator randomGenerator;
};

TEST_F(DefaultPRNGeneratorTest, DefaultPRNGeneratorProvidesPositiveUInt32)
{
	// when
	auto randomNumber = randomGenerator.nextUInt32(std::numeric_limits<uint32_t>::max());

	// then
	EXPECT_GE(randomNumber, 0u);
	EXPECT_LT(randomNumber, std::numeric_limits<uint32_t>::max());
}


TEST_F(DefaultPRNGeneratorTest, DefaultPRNGeneratorProvidesPositiveUInt64)
{
	// when
	auto randomNumber = randomGenerator.nextUInt64(std::numeric_limits<uint64_t>::max());

	// then
	EXPECT_GE(randomNumber, 0u);
	EXPECT_LT(randomNumber, std::numeric_limits<uint64_t>::max());
}
