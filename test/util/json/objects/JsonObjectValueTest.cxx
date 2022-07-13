﻿/**
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

#include "OpenKit/json/JsonObjectValue.h"
#include "OpenKit/json/JsonBooleanValue.h"
#include "OpenKit/json/JsonArrayValue.h"
#include <OpenKit/json/JsonStringValue.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace openkit::json;

class JsonObjectValueTest : public testing::Test
{
};

TEST_F(JsonObjectValueTest, isObjectType)
{
	// given
	auto emptyMap = std::make_shared<JsonObjectValue::JsonObjectMap>();

	// then
	ASSERT_THAT(JsonObjectValue::fromMap(emptyMap)->getValueType(), testing::Eq(JsonValueType::OBJECT_VALUE));
}

TEST_F(JsonObjectValueTest, sizeReturnsSizeOfUnderlyingMap)
{
	// given
	auto jsonValueMap = std::make_shared<JsonObjectValue::JsonObjectMap>();
	jsonValueMap->insert({"first", JsonBooleanValue::trueValue()});

	auto target = JsonObjectValue::fromMap(jsonValueMap);

	// when
	auto obtained = target->size();

	// then
	ASSERT_THAT(obtained, testing::Eq(size_t(1)));

	// and when
	jsonValueMap->insert({"second", JsonBooleanValue::falseValue()});
	target = JsonObjectValue::fromMap(jsonValueMap);
	obtained = target->size();

	// then
	ASSERT_THAT(obtained, testing::Eq(size_t(2)));
}


TEST_F(JsonObjectValueTest, containsKeyDelegatesTheCallToTheUnderlyingMap)
{
	// given
	auto jsonValueMap = std::make_shared<JsonObjectValue::JsonObjectMap>();
	jsonValueMap->insert({{"first", JsonBooleanValue::trueValue()}, {"second", JsonBooleanValue::falseValue()}});

	auto target = JsonObjectValue::fromMap(jsonValueMap);

	// when
	auto obtained = target->containsKey("first");

	// then
	ASSERT_THAT(obtained, testing::Eq(true));

	// and when
	obtained = target->containsKey("second");

	// then
	ASSERT_THAT(obtained, testing::Eq(true));

	// and when
	obtained = target->containsKey("third");

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(JsonObjectValueTest, findReturnsValueOfUnderlyingMap)
{
	// given
	auto jsonValueMap = std::make_shared<JsonObjectValue::JsonObjectMap>();
	jsonValueMap->insert({{"first", JsonBooleanValue::trueValue()}, {"second", JsonBooleanValue::falseValue()}});

	auto target = JsonObjectValue::fromMap(jsonValueMap);

	// when
	auto obtained = target->find("first");

	// then
	ASSERT_THAT(obtained->second, testing::Eq(JsonBooleanValue::trueValue()));

	// and when
	obtained = target->find("second");

	// then
	ASSERT_THAT(obtained->second, testing::Eq(JsonBooleanValue::falseValue()));

	// and when
	obtained = target->find("third");

	// then
	ASSERT_THAT(obtained, testing::Eq(target->end()));
}

TEST_F(JsonObjectValueTest, indexOperatorReturnsValueOfUnderlyingMap)
{
	// given
	auto jsonValueMap = std::make_shared<JsonObjectValue::JsonObjectMap>();
	jsonValueMap->insert({{"first", JsonBooleanValue::trueValue()}, {"second", JsonBooleanValue::falseValue()}});

	auto target = JsonObjectValue::fromMap(jsonValueMap);

	// when
	auto obtained = (*target)["first"];

	// then
	ASSERT_THAT(obtained, testing::Eq(JsonBooleanValue::trueValue()));

	// and when
	obtained = (*target)["second"];

	// then
	ASSERT_THAT(obtained, testing::Eq(JsonBooleanValue::falseValue()));

	// and when
	obtained = (*target)["third"];

	// then
	ASSERT_THAT(obtained, testing::IsNull());
}

TEST_F(JsonObjectValueTest, beginReturnsIteratorToUnderlyingMap)
{
	// given
	auto jsonValueMap = std::make_shared<JsonObjectValue::JsonObjectMap>();
	jsonValueMap->insert({{"first",  JsonBooleanValue::trueValue()}, {"second", JsonBooleanValue::falseValue()}});

	auto target = JsonObjectValue::fromMap(jsonValueMap);

	// when
	auto obtained = target->begin();

	// then
	ASSERT_THAT(obtained->second, testing::AnyOf(JsonBooleanValue::trueValue(), JsonBooleanValue::falseValue()));

	// and when
	obtained++;

	// then
	ASSERT_THAT(obtained->second, testing::AnyOf(JsonBooleanValue::trueValue(), JsonBooleanValue::falseValue()));

	// and when
	obtained++;

	// then
	ASSERT_THAT(obtained, testing::Eq(target->end()));
}

TEST_F(JsonObjectValueTest, toStringEmptyObject)
{
	// given
	auto jsonValueMap = std::make_shared<JsonObjectValue::JsonObjectMap>();
	auto target = JsonObjectValue::fromMap(jsonValueMap);

	// then
	ASSERT_THAT(target->toString(), testing::Eq(std::string("{}")));
}

TEST_F(JsonObjectValueTest, toStringSingleElement)
{
	// given
	auto jsonValueMap = std::make_shared<JsonObjectValue::JsonObjectMap>();
	jsonValueMap->insert({ {"first",  JsonBooleanValue::trueValue()} });
	auto target = JsonObjectValue::fromMap(jsonValueMap);

	// then
	ASSERT_THAT(target->toString(), testing::Eq(std::string("{\"first\":true}")));
}

TEST_F(JsonObjectValueTest, toStringSingleElementEmptyArray)
{
	// given
	auto jsonValueMap = std::make_shared<JsonObjectValue::JsonObjectMap>();
	auto jsonValues = std::make_shared<JsonArrayValue::JsonValueList>();
	jsonValueMap->insert({ {"first",  JsonArrayValue::fromList(jsonValues)}});
	auto target = JsonObjectValue::fromMap(jsonValueMap);

	// then
	ASSERT_THAT(target->toString(), testing::Eq(std::string("{\"first\":[]}")));
}

TEST_F(JsonObjectValueTest, toStringMultipleElements)
{
	// given
	auto jsonValueMap = std::make_shared<JsonObjectValue::JsonObjectMap>();
	jsonValueMap->insert({"second", JsonBooleanValue::falseValue() });
	jsonValueMap->insert({"first",  JsonBooleanValue::trueValue()});
	auto target = JsonObjectValue::fromMap(jsonValueMap);

	// then
	ASSERT_THAT(target->toString(), testing::HasSubstr(std::string("\"first\":true")));
	ASSERT_THAT(target->toString(), testing::HasSubstr(std::string("\"second\":false")));
}

TEST_F(JsonObjectValueTest, parsingSpecialUnicodeCharacter)
{
	// given
	auto jsonValueMap = std::make_shared<JsonObjectValue::JsonObjectMap>();
	jsonValueMap->insert({{"Test",  JsonStringValue::fromString("/\b\f\n\r\t\"\\ф")}});
	auto target = JsonObjectValue::fromMap(jsonValueMap);

	// then
	ASSERT_THAT(target->toString(), testing::Eq(std::string("{\"Test\":\"\\/\\b\\f\\n\\r\\t\\\"\\\\ф\"}")));
}