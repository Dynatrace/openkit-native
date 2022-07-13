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

#include "OpenKit/json/JsonNullValue.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace openkit::json;


class JsonNullValueTest : public ::testing::Test
{
};

TEST_F(JsonNullValueTest, isNullType)
{
	// when, then
	ASSERT_THAT(JsonNullValue::nullValue()->getValueType(), testing::Eq(JsonValueType::NULL_VALUE));
}

TEST_F(JsonNullValueTest, toString)
{
	// when, then
	ASSERT_THAT(JsonNullValue::nullValue()->toString(), testing::Eq(std::string("null")));
}