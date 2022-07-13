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

#include "OpenKit/json/JsonStringValue.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace openkit::json;

class JsonStringValueTest : public testing::Test
{
};

TEST_F(JsonStringValueTest, isStringType)
{
	ASSERT_THAT(JsonStringValue::fromString(std::string(""))->getValueType(), testing::Eq(JsonValueType::STRING_VALUE));
}

TEST_F(JsonStringValueTest, getValueReturnsValueOfFactoryMethodArgument)
{
	ASSERT_THAT(JsonStringValue::fromString(std::string(""))->getValue(), testing::Eq(std::string("")));
	ASSERT_THAT(JsonStringValue::fromString(std::string("a"))->getValue(), testing::Eq(std::string("a")));
	ASSERT_THAT(JsonStringValue::fromString(std::string("foobar"))->getValue(), testing::Eq(std::string("foobar")));
}

TEST_F(JsonStringValueTest, toString)
{
	ASSERT_THAT(JsonStringValue::fromString(std::string("Value"))->toString(), testing::Eq(std::string("\"Value\"")));
}