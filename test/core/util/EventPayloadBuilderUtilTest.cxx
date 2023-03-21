/**
 * Copyright 2018-2023 Dynatrace LLC
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

#include "../../api/mock/MockILogger.h"
#include "core/util/EventPayloadBuilderUtil.h"
#include "core/objects/EventPayloadBuilder.h"
#include "OpenKit/json/JsonNumberValue.h"
#include "OpenKit/json/JsonStringValue.h"

#include "gtest/gtest.h"

using EPBUtil_t = core::util::EventPayloadBuilderUtil;
using MockILogger_sp = std::shared_ptr<test::MockILogger>;

class EventPayloadBuilderUtilTest : public testing::Test
{
protected:
	MockILogger_sp mockLogger;

	void SetUp() override
	{
		mockLogger = test::MockILogger::createNice();
	}
};

TEST_F(EventPayloadBuilderUtilTest, emptyAttributes)
{
	// given
	auto emptyMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	auto builder = std::make_shared<core::objects::EventPayloadBuilder>(emptyMap, mockLogger);

	// then
	ASSERT_THAT(EPBUtil_t::isEventContainingNonFiniteNumericValues(builder), testing::Eq(false));
}

TEST_F(EventPayloadBuilderUtilTest, sampleAttributesWithoutNfn)
{
	// given
	auto sampleMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	auto nestedMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	auto nestedMap2 = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();

	auto jsonArray = std::make_shared<openkit::json::JsonArrayValue::JsonValueList>();
	jsonArray->push_back(openkit::json::JsonNumberValue::fromLong(6));
	auto outsideArray = openkit::json::JsonArrayValue::fromList(jsonArray);

	auto jsonArray2 = std::make_shared<openkit::json::JsonArrayValue::JsonValueList>();
	jsonArray2->push_back(openkit::json::JsonNumberValue::fromDouble(5.1));
	jsonArray2->push_back(openkit::json::JsonStringValue::fromString("Value"));
	auto nestedArray = openkit::json::JsonArrayValue::fromList(jsonArray2);

	nestedMap2->insert(std::make_pair("g", openkit::json::JsonStringValue::fromString("Value")));

	nestedMap->insert(std::make_pair("d", openkit::json::JsonNumberValue::fromLong(4)));
	nestedMap->insert(std::make_pair("e", nestedArray));
	nestedMap->insert(std::make_pair("f", openkit::json::JsonObjectValue::fromMap(nestedMap2)));

	sampleMap->insert(std::make_pair("a", openkit::json::JsonStringValue::fromString("Value")));
	sampleMap->insert(std::make_pair("b", openkit::json::JsonNumberValue::fromLong(3)));
	sampleMap->insert(std::make_pair("c", openkit::json::JsonObjectValue::fromMap(nestedMap)));
	sampleMap->insert(std::make_pair("h", outsideArray));

	auto builder = std::make_shared<core::objects::EventPayloadBuilder>(sampleMap, mockLogger);

	// then
	ASSERT_THAT(EPBUtil_t::isEventContainingNonFiniteNumericValues(builder), testing::Eq(false));
}

TEST_F(EventPayloadBuilderUtilTest, mapWithNanValue)
{
	// given
	auto sampleMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	sampleMap->insert(std::make_pair("custom", openkit::json::JsonNumberValue::fromDouble(nan(""))));
	auto builder = std::make_shared<core::objects::EventPayloadBuilder>(sampleMap, mockLogger);

	// then
	ASSERT_THAT(EPBUtil_t::isEventContainingNonFiniteNumericValues(builder), testing::Eq(true));
}

TEST_F(EventPayloadBuilderUtilTest, mapWithInfiniteValue)
{
	// given
	auto sampleMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	sampleMap->insert(std::make_pair("custom", openkit::json::JsonNumberValue::fromDouble(std::numeric_limits<float>::infinity())));
	auto builder = std::make_shared<core::objects::EventPayloadBuilder>(sampleMap, mockLogger);

	// then
	ASSERT_THAT(EPBUtil_t::isEventContainingNonFiniteNumericValues(builder), testing::Eq(true));
}

TEST_F(EventPayloadBuilderUtilTest, mapWithArrayThatContainsNanValue)
{
	// given
	auto sampleMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();

	auto jsonValues = std::make_shared<openkit::json::JsonArrayValue::JsonValueList>();
	jsonValues->push_back(openkit::json::JsonNumberValue::fromDouble(nan("")));

	auto sampleArray = openkit::json::JsonArrayValue::fromList(jsonValues);

	sampleMap->insert(std::make_pair("custom", sampleArray));
	auto builder = std::make_shared<core::objects::EventPayloadBuilder>(sampleMap, mockLogger);

	// then
	ASSERT_THAT(EPBUtil_t::isEventContainingNonFiniteNumericValues(builder), testing::Eq(true));
}

TEST_F(EventPayloadBuilderUtilTest, mapWithArrayThatContainsInfiniteValue)
{
	// given
	auto sampleMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();

	auto jsonValues = std::make_shared<openkit::json::JsonArrayValue::JsonValueList>();
	jsonValues->push_back(openkit::json::JsonNumberValue::fromDouble(std::numeric_limits<float>::infinity()));

	auto sampleArray = openkit::json::JsonArrayValue::fromList(jsonValues);

	sampleMap->insert(std::make_pair("custom", sampleArray));
	auto builder = std::make_shared<core::objects::EventPayloadBuilder>(sampleMap, mockLogger);

	// then
	ASSERT_THAT(EPBUtil_t::isEventContainingNonFiniteNumericValues(builder), testing::Eq(true));
}

TEST_F(EventPayloadBuilderUtilTest, mapWithNestedMapThatContainsNanValue)
{
	// given
	auto sampleMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	auto nestedMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	nestedMap->insert(std::make_pair("custom", openkit::json::JsonNumberValue::fromDouble(nan(""))));
	sampleMap->insert(std::make_pair("custom", openkit::json::JsonObjectValue::fromMap(nestedMap)));
	
	auto builder = std::make_shared<core::objects::EventPayloadBuilder>(sampleMap, mockLogger);

	// then
	ASSERT_THAT(EPBUtil_t::isEventContainingNonFiniteNumericValues(builder), testing::Eq(true));
}

TEST_F(EventPayloadBuilderUtilTest, mapWithNestedMapThatContainsInfiniteValue)
{
	// given
	auto sampleMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	auto nestedMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	nestedMap->insert(std::make_pair("custom", openkit::json::JsonNumberValue::fromDouble(std::numeric_limits<float>::infinity())));
	sampleMap->insert(std::make_pair("custom", openkit::json::JsonObjectValue::fromMap(nestedMap)));

	auto builder = std::make_shared<core::objects::EventPayloadBuilder>(sampleMap, mockLogger);

	// then
	ASSERT_THAT(EPBUtil_t::isEventContainingNonFiniteNumericValues(builder), testing::Eq(true));
}

TEST_F(EventPayloadBuilderUtilTest, mapWithNestedMapThatContainsArrayWithNanValue)
{
	// given
	auto sampleMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	auto nestedMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();

	auto jsonValuesArray = std::make_shared<openkit::json::JsonArrayValue::JsonValueList>();
	jsonValuesArray->push_back(openkit::json::JsonStringValue::fromString("Test"));
	jsonValuesArray->push_back(openkit::json::JsonNumberValue::fromDouble(nan("")));
	auto nestedArray = openkit::json::JsonArrayValue::fromList(jsonValuesArray);

	nestedMap->insert(std::make_pair("custom", nestedArray));
	sampleMap->insert(std::make_pair("custom", openkit::json::JsonObjectValue::fromMap(nestedMap)));

	auto builder = std::make_shared<core::objects::EventPayloadBuilder>(sampleMap, mockLogger);

	// then
	ASSERT_THAT(EPBUtil_t::isEventContainingNonFiniteNumericValues(builder), testing::Eq(true));
}

TEST_F(EventPayloadBuilderUtilTest, mapWithNestedMapThatContainsArrayWithInfiniteValue)
{
	// given
	auto sampleMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	auto nestedMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();

	auto jsonValuesArray = std::make_shared<openkit::json::JsonArrayValue::JsonValueList>();
	jsonValuesArray->push_back(openkit::json::JsonStringValue::fromString("Test"));
	jsonValuesArray->push_back(openkit::json::JsonNumberValue::fromDouble(std::numeric_limits<float>::infinity()));
	auto nestedArray = openkit::json::JsonArrayValue::fromList(jsonValuesArray);

	nestedMap->insert(std::make_pair("custom", nestedArray));
	sampleMap->insert(std::make_pair("custom", openkit::json::JsonObjectValue::fromMap(nestedMap)));

	auto builder = std::make_shared<core::objects::EventPayloadBuilder>(sampleMap, mockLogger);

	// then
	ASSERT_THAT(EPBUtil_t::isEventContainingNonFiniteNumericValues(builder), testing::Eq(true));
}