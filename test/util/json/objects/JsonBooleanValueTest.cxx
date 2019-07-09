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

#include "util/json/objects/JsonBooleanValue.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace util::json::objects;

class JsonBooleanValueTest : public testing::Test
{
};

TEST_F(JsonBooleanValueTest, isBooleanType)
{
	// when, then
	ASSERT_THAT(JsonBooleanValue::TRUE->getValueType(), testing::Eq(JsonValueType::BOOLEAN_VALUE));
}

TEST_F(JsonBooleanValueTest, getValueReturnsAppropriateBooleanValue)
{
	// when,then
	ASSERT_THAT(JsonBooleanValue::TRUE->getValue(), testing::Eq(true));
	ASSERT_THAT(JsonBooleanValue::FALSE->getValue(), testing::Eq(false));
}

TEST_F(JsonBooleanValueTest, fromValueReturnsTrueSingletonValue)
{
	// when
	auto obtained = JsonBooleanValue::fromValue(true);

	// then the singleton TRUE instance is returned
	ASSERT_THAT(obtained, testing::Eq(JsonBooleanValue::TRUE));
}

TEST_F(JsonBooleanValueTest, fromValueReturnsFalseSingletonValue)
{
	// when
	auto obtained = JsonBooleanValue::fromValue(false);

	// then the singleton FALSE instance is returned
	ASSERT_THAT(obtained, testing::Eq(JsonBooleanValue::FALSE));
}

TEST_F(JsonBooleanValueTest, fromLiteralReturnsTrueSingletonValueForTrueLiteral)
{
	// given
	auto trueLiteral = std::string("true");

	// when
	auto obtained = JsonBooleanValue::fromLiteral(trueLiteral);

	// then the singleton TRUE instance is returned
	ASSERT_THAT(obtained, testing::Eq(JsonBooleanValue::TRUE));
}

TEST_F(JsonBooleanValueTest, fromLiteralReturnsFalseSingletonValueForFalseLiteral)
{
	// given
	auto falseLiteral = std::string("false");

	// when
	auto obtained = JsonBooleanValue::fromLiteral(falseLiteral);

	// then the singleton FALSE instance is returned
	ASSERT_THAT(obtained, testing::Eq(JsonBooleanValue::FALSE));
}

TEST_F(JsonBooleanValueTest, fromLiteralReturnsNullForNonBooleanLiterals)
{
	// when wrong casing is used, then
	auto upperCaseTrueLiteral = std::string("TRUE");
	ASSERT_THAT(JsonBooleanValue::fromLiteral(upperCaseTrueLiteral), testing::IsNull());

	auto upperCaseFalseLiteral = std::string("FALSE");
	ASSERT_THAT(JsonBooleanValue::fromLiteral(upperCaseFalseLiteral), testing::IsNull());

	// and when it is actually a number, then
	auto numberLiteral = std::string("1234");
	ASSERT_THAT(JsonBooleanValue::fromLiteral(numberLiteral), testing::IsNull());

	// and when passing an empty string then,
	auto emptyString = std::string("");
	ASSERT_THAT(JsonBooleanValue::fromLiteral(emptyString), testing::IsNull());
}
