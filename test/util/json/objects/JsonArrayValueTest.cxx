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

#include "util/json/objects/JsonArrayValue.h"
#include "util/json/objects/JsonBooleanValue.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace util::json::objects;


class JsonArrayValueTest : public testing::Test
{
};

TEST_F(JsonArrayValueTest, isArrayType)
{
	// given
	auto emptyList = std::make_shared<std::list<std::shared_ptr<JsonValue>>>();

	// when, then
	ASSERT_THAT(JsonArrayValue::fromList(emptyList)->getValueType(), testing::Eq(JsonValueType::ARRAY_VALUE));
}

TEST_F(JsonArrayValueTest, sizeReturnsSizeOfUnderlyingList)
{
	// given
	auto jsonValues = std::make_shared<JsonArrayValue::JsonValueList>();
	jsonValues->push_back(JsonBooleanValue::trueValue());

	auto target = JsonArrayValue::fromList(jsonValues);

	// when
	auto obtained = target->size();

	// then
	ASSERT_THAT(obtained, testing::Eq(size_t(1)));

	// and when
	jsonValues->push_back(JsonBooleanValue::falseValue());
	target = JsonArrayValue::fromList(jsonValues);

	obtained = target->size();

	// then
	ASSERT_THAT(obtained, testing::Eq(size_t(2)));
}

TEST_F(JsonArrayValueTest, beginReturnsIteratorOfUnderlyingList)
{
	// given
	auto jsonValues = std::make_shared<JsonArrayValue::JsonValueList>();
	jsonValues->push_back(JsonBooleanValue::trueValue());
	jsonValues->push_back(JsonBooleanValue::falseValue());

	auto target = JsonArrayValue::fromList(jsonValues);

	// when
	auto obtained = target->begin();

	// then
	ASSERT_THAT(*obtained, testing::Eq(JsonBooleanValue::trueValue()));

	// and when
	obtained++;

	// then
	ASSERT_THAT(*obtained, testing::Eq(JsonBooleanValue::falseValue()));

	// and when
	obtained++;

	// then
	ASSERT_THAT(obtained, testing::Eq(target->end()));
}