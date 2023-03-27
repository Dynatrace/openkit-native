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

#include "../../api/mock/MockILogger.h"
#include <core/objects/EventPayloadBuilder.h>
#include <OpenKit/json/JsonObjectValue.h>
#include <OpenKit/json/JsonStringValue.h>
#include <OpenKit/json/JsonNumberValue.h>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <memory>

using namespace core::objects;

using MockILogger_sp = std::shared_ptr<test::MockILogger>;

class EventPayloadBuilderTest : public testing::Test
{
protected:
	MockILogger_sp mockLogger;

	void SetUp() override
	{
		mockLogger = test::MockILogger::createNice();
	}
};

MATCHER_P2(AttributeIsOverridden, attributeName, attributeContent, "Attribute should be overridden")
{
	auto mainString = std::string(arg);

	auto result = mainString.rfind("\"" + std::string(attributeName) + "\":\"" + std::string(attributeContent) + "\"") != std::string::npos;

	if (!result)
	{
		*result_listener << "Attribute named \"" << attributeName << "\" was not overridden";
	}

	return result;
}

TEST_F(EventPayloadBuilderTest, createEmptyPayloadBuilder)
{
	// given
	auto emptyMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	EventPayloadBuilder eventPayloadBuilder(emptyMap, mockLogger);

	// then
	ASSERT_THAT(eventPayloadBuilder.build(), testing::Eq("{}"));
}

TEST_F(EventPayloadBuilderTest, removingInternalReservedValues)
{
	// given
	auto map = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	map->insert(std::make_pair("dt", openkit::json::JsonStringValue::fromString("Removed")));
	map->insert(std::make_pair("dt.hello", openkit::json::JsonStringValue::fromString("Removed")));
	map->insert(std::make_pair("event.kind", openkit::json::JsonStringValue::fromString("Okay")));

	EventPayloadBuilder eventPayloadBuilder(map, mockLogger);
	eventPayloadBuilder.cleanReservedInternalAttributes();

	// then
	ASSERT_THAT(eventPayloadBuilder.build(), testing::Eq("{\"event.kind\":\"Okay\"}"));
}

TEST_F(EventPayloadBuilderTest, addNonOverridableAttributeWhichIsAlreadyAvailable)
{
	// given
	auto map = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	map->insert(std::make_pair("dt.rum.sid", openkit::json::JsonStringValue::fromString("MySession")));

	EventPayloadBuilder eventPayloadBuilder(map, mockLogger);
	eventPayloadBuilder.addNonOverridableAttribute("dt.rum.sid", openkit::json::JsonStringValue::fromString("ComingFromAgent"));

	// then
	ASSERT_THAT(eventPayloadBuilder.build(), testing::Eq("{\"dt.rum.sid\":\"ComingFromAgent\"}"));
}

TEST_F(EventPayloadBuilderTest, addNonOverridableAttributeWhichIsNotAvailable)
{
	// given
	auto map = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();

	EventPayloadBuilder eventPayloadBuilder(map, mockLogger);
	eventPayloadBuilder.addNonOverridableAttribute("NonOverridable", openkit::json::JsonStringValue::fromString("Changed"));

	// then
	ASSERT_THAT(eventPayloadBuilder.build(), testing::Eq("{\"NonOverridable\":\"Changed\"}"));
}

TEST_F(EventPayloadBuilderTest, addOverridableAttributeWhichIsAlreadyAvailable)
{
	// given
	auto map = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	map->insert(std::make_pair("timestamp", openkit::json::JsonStringValue::fromString("Changed")));

	EventPayloadBuilder eventPayloadBuilder(map, mockLogger);
	eventPayloadBuilder.addOverridableAttribute("timestamp", openkit::json::JsonStringValue::fromString("ComingFromAgent"));

	// then
	ASSERT_THAT(eventPayloadBuilder.build(), AttributeIsOverridden("timestamp", "Changed"));
}

TEST_F(EventPayloadBuilderTest, addOverridableAttributeWhichIsNotAvailable)
{
	// given
	auto map = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();

	EventPayloadBuilder eventPayloadBuilder(map, mockLogger);
	eventPayloadBuilder.addOverridableAttribute("Overridable", openkit::json::JsonStringValue::fromString("Changed"));

	// then
	ASSERT_THAT(eventPayloadBuilder.build(), testing::Eq("{\"Overridable\":\"Changed\"}"));
}

TEST_F(EventPayloadBuilderTest, buildMapWithNanValue)
{
	// given
	auto sampleMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	sampleMap->insert(std::make_pair("custom", openkit::json::JsonNumberValue::fromDouble(nan(""))));

	EventPayloadBuilder eventPayloadBuilder(sampleMap, mockLogger);

	// then
	ASSERT_THAT(eventPayloadBuilder.build(), testing::Eq("{\"custom\":null}"));
}

TEST_F(EventPayloadBuilderTest, buildMapWithInfiniteValue)
{
	// given
	auto sampleMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	sampleMap->insert(std::make_pair("custom", openkit::json::JsonNumberValue::fromDouble(std::numeric_limits<float>::infinity())));
	EventPayloadBuilder eventPayloadBuilder(sampleMap, mockLogger);

	// then
	ASSERT_THAT(eventPayloadBuilder.build(), testing::Eq("{\"custom\":null}"));
}

TEST_F(EventPayloadBuilderTest, buildMapWithArrayThatContainsNanValue)
{
	// given
	auto sampleMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();

	auto jsonValues = std::make_shared<openkit::json::JsonArrayValue::JsonValueList>();
	jsonValues->push_back(openkit::json::JsonNumberValue::fromDouble(nan("")));

	auto sampleArray = openkit::json::JsonArrayValue::fromList(jsonValues);

	sampleMap->insert(std::make_pair("custom", sampleArray));
	EventPayloadBuilder eventPayloadBuilder(sampleMap, mockLogger);

	// then
	ASSERT_THAT(eventPayloadBuilder.build(), testing::Eq("{\"custom\":[null]}"));
}

TEST_F(EventPayloadBuilderTest, buildMapWithArrayThatContainsInfiniteValue)
{
	// given
	auto sampleMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();

	auto jsonValues = std::make_shared<openkit::json::JsonArrayValue::JsonValueList>();
	jsonValues->push_back(openkit::json::JsonNumberValue::fromDouble(std::numeric_limits<float>::infinity()));

	auto sampleArray = openkit::json::JsonArrayValue::fromList(jsonValues);

	sampleMap->insert(std::make_pair("custom", sampleArray));
	EventPayloadBuilder eventPayloadBuilder(sampleMap, mockLogger);

	// then
	ASSERT_THAT(eventPayloadBuilder.build(), testing::Eq("{\"custom\":[null]}"));
}

TEST_F(EventPayloadBuilderTest, buildMapWithNestedMapThatContainsNanValue)
{
	// given
	auto sampleMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	auto nestedMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	nestedMap->insert(std::make_pair("custom", openkit::json::JsonNumberValue::fromDouble(nan(""))));
	sampleMap->insert(std::make_pair("custom", openkit::json::JsonObjectValue::fromMap(nestedMap)));

	EventPayloadBuilder eventPayloadBuilder(sampleMap, mockLogger);

	// then
	ASSERT_THAT(eventPayloadBuilder.build(), testing::Eq("{\"custom\":{\"custom\":null}}"));
}

TEST_F(EventPayloadBuilderTest, buildMapWithNestedMapThatContainsInfiniteValue)
{
	// given
	auto sampleMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	auto nestedMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	nestedMap->insert(std::make_pair("custom", openkit::json::JsonNumberValue::fromDouble(std::numeric_limits<float>::infinity())));
	sampleMap->insert(std::make_pair("custom", openkit::json::JsonObjectValue::fromMap(nestedMap)));

	EventPayloadBuilder eventPayloadBuilder(sampleMap, mockLogger);

	// then
	ASSERT_THAT(eventPayloadBuilder.build(), testing::Eq("{\"custom\":{\"custom\":null}}"));
}

TEST_F(EventPayloadBuilderTest, buildMapWithNestedMapThatContainsArrayWithNanValue)
{
	// given
	auto sampleMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	auto nestedMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();

	auto jsonValuesArray = std::make_shared<openkit::json::JsonArrayValue::JsonValueList>();
	jsonValuesArray->push_back(openkit::json::JsonStringValue::fromString("Test"));
	jsonValuesArray->push_back(openkit::json::JsonNumberValue::fromLong(1));
	jsonValuesArray->push_back(openkit::json::JsonNumberValue::fromDouble(nan("")));
	jsonValuesArray->push_back(openkit::json::JsonNumberValue::fromLong(2));
	auto nestedArray = openkit::json::JsonArrayValue::fromList(jsonValuesArray);

	nestedMap->insert(std::make_pair("custom", nestedArray));
	sampleMap->insert(std::make_pair("custom", openkit::json::JsonObjectValue::fromMap(nestedMap)));

	EventPayloadBuilder eventPayloadBuilder(sampleMap, mockLogger);

	// then
	ASSERT_THAT(eventPayloadBuilder.build(), testing::Eq("{\"custom\":{\"custom\":[\"Test\",1,null,2]}}"));
}

TEST_F(EventPayloadBuilderTest, buildMapWithNestedMapThatContainsArrayWithInfiniteValue)
{
	// given
	auto sampleMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	auto nestedMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();

	auto jsonValuesArray = std::make_shared<openkit::json::JsonArrayValue::JsonValueList>();
	jsonValuesArray->push_back(openkit::json::JsonStringValue::fromString("Test"));
	jsonValuesArray->push_back(openkit::json::JsonNumberValue::fromLong(1));
	jsonValuesArray->push_back(openkit::json::JsonNumberValue::fromDouble(std::numeric_limits<float>::infinity()));
	jsonValuesArray->push_back(openkit::json::JsonNumberValue::fromLong(2));
	auto nestedArray = openkit::json::JsonArrayValue::fromList(jsonValuesArray);

	nestedMap->insert(std::make_pair("custom", nestedArray));
	sampleMap->insert(std::make_pair("custom", openkit::json::JsonObjectValue::fromMap(nestedMap)));

	EventPayloadBuilder eventPayloadBuilder(sampleMap, mockLogger);

	// then
	ASSERT_THAT(eventPayloadBuilder.build(), testing::Eq("{\"custom\":{\"custom\":[\"Test\",1,null,2]}}"));
}