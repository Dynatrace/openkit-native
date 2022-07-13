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

#include "util/json/JsonWriter.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace openkit::json;

class JsonWriterTest : public testing::Test
{
};

TEST_F(JsonWriterTest, checkOpenArrayCharacter)
{
	// given
	auto target = JsonWriter();
	target.openArray();

	// then
	ASSERT_THAT(target.toString(), testing::Eq(std::string("[")));
}

TEST_F(JsonWriterTest, checkCloseArrayCharacter)
{
	// given
	auto target = JsonWriter();
	target.closeArray();

	// then
	ASSERT_THAT(target.toString(), testing::Eq(std::string("]")));
}

TEST_F(JsonWriterTest, checkOpenObjectCharacter)
{
	// given
	auto target = JsonWriter();
	target.openObject();

	// then
	ASSERT_THAT(target.toString(), testing::Eq(std::string("{")));
}

TEST_F(JsonWriterTest, checkCloseObjectCharacter)
{
	// given
	auto target = JsonWriter();
	target.closeObject();

	// then
	ASSERT_THAT(target.toString(), testing::Eq(std::string("}")));
}

TEST_F(JsonWriterTest, checkElementSeperatorCharacter)
{
	// given
	auto target = JsonWriter();
	target.insertElementSeperator();

	// then
	ASSERT_THAT(target.toString(), testing::Eq(std::string(",")));
}

TEST_F(JsonWriterTest, checkKeyValueSeperatorCharacter)
{
	// given
	auto target = JsonWriter();
	target.insertKeyValueSeperator();

	// then
	ASSERT_THAT(target.toString(), testing::Eq(std::string(":")));
}

TEST_F(JsonWriterTest, checkValueFormatting)
{
	// given
	auto target = JsonWriter();
	target.insertValue(std::string("false"));

	// then
	ASSERT_THAT(target.toString(), testing::Eq(std::string("false")));
}

TEST_F(JsonWriterTest, checkStringValueFormatting)
{
	// given
	auto target = JsonWriter();
	target.insertStringValue(std::string("false"));

	// then
	ASSERT_THAT(target.toString(), testing::Eq(std::string("\"false\"")));
}

TEST_F(JsonWriterTest, checkKeyFormatting)
{
	// given
	auto target = JsonWriter();
	target.insertKey(std::string("Key"));

	// then
	ASSERT_THAT(target.toString(), testing::Eq(std::string("\"Key\"")));
}