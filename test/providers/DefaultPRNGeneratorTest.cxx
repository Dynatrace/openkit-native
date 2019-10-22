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

#include "providers/DefaultPRNGenerator.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>


using DefaultPrnGenerator_t = providers::DefaultPRNGenerator;

class DefaultPRNGeneratorTest : public testing::Test
{
protected:
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
	DefaultPrnGenerator_t randomGenerator;
};

TEST_F(DefaultPRNGeneratorTest, DefaultPRNGeneratorProvidesPositiveInt32)
{
	// when
	auto randomNumber = randomGenerator.nextInt32(std::numeric_limits<int32_t>::max());

	// then
	EXPECT_THAT(randomNumber, testing::AllOf(testing::Ge(int32_t(0)), testing::Lt(std::numeric_limits<int32_t>::max())));
}


TEST_F(DefaultPRNGeneratorTest, DefaultPRNGeneratorProvidesPositiveUInt64)
{
	// when
	auto randomNumber = randomGenerator.nextInt64(std::numeric_limits<int64_t>::max());

	// then
	EXPECT_THAT(randomNumber, testing::AllOf(testing::Ge(int64_t(0)), testing::Lt(std::numeric_limits<int64_t>::max())));
}
