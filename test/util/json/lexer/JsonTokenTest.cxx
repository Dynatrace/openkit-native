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

#include "util/json/lexer/JsonToken.h"
#include "util/json/lexer/JsonTokenType.h"
#include "util/json/constants/JsonLiterals.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace util::json::lexer;
using namespace util::json::constants;

class JsonTokenTest : public testing::Test
{
};

TEST_F(JsonTokenTest, tokenTypesOfPredefinedTokensAreCorrect)
{
	// when boolean true token, then
	ASSERT_THAT(JsonToken::BOOLEAN_TRUE_TOKEN->getTokenType(), testing::Eq(JsonTokenType::LITERAL_BOOLEAN));

	// when boolean false token, then
	ASSERT_THAT(JsonToken::BOOLEAN_FALSE_TOKEN->getTokenType(), testing::Eq(JsonTokenType::LITERAL_BOOLEAN));

	// when null token, then
	ASSERT_THAT(JsonToken::NULL_TOKEN->getTokenType(), testing::Eq(JsonTokenType::LITERAL_NULL));

	// when left brace token, then
	ASSERT_THAT(JsonToken::LEFT_BRACE_TOKEN->getTokenType(), testing::Eq(JsonTokenType::LEFT_BRACE));

	// when right brace token, then
	ASSERT_THAT(JsonToken::RIGHT_BRACE_TOKEN->getTokenType(), testing::Eq(JsonTokenType::RIGHT_BRACE));

	// when left square bracket token, then
	ASSERT_THAT(JsonToken::LEFT_SQUARE_BRACKET_TOKEN->getTokenType(), testing::Eq(JsonTokenType::LEFT_SQUARE_BRACKET));

	// when right square bracket token,then
	ASSERT_THAT(JsonToken::RIGHT_SQUARE_BRACKET_TOKEN->getTokenType(), testing::Eq(JsonTokenType::RIGHT_SQUARE_BRACKET));

	// when comma token, then
	ASSERT_THAT(JsonToken::COMMA_TOKEN->getTokenType(), testing::Eq(JsonTokenType::COMMA));

	// when colon token, then
	ASSERT_THAT(JsonToken::COLON_TOKEN->getTokenType(), testing::Eq(JsonTokenType::COLON));
}

TEST_F(JsonTokenTest, tokenValuesOfPredefinedTokensAreCorrect)
{
	// when boolean true token, then
	ASSERT_THAT(JsonToken::BOOLEAN_TRUE_TOKEN->getValue(), testing::Eq(JsonLiterals::BOOLEAN_TRUE_LITERAL));

	// when boolean false token, then
	ASSERT_THAT(JsonToken::BOOLEAN_FALSE_TOKEN->getValue(), testing::Eq(JsonLiterals::BOOLEAN_FALSE_LITERAL));

	// when null token, then
	ASSERT_THAT(JsonToken::NULL_TOKEN->getValue(), testing::Eq(JsonLiterals::NULL_LITERAL));

	// when left brace token, then
	ASSERT_THAT(JsonToken::LEFT_BRACE_TOKEN->getValue(), testing::Eq(""));

	// when right brace token, then
	ASSERT_THAT(JsonToken::RIGHT_BRACE_TOKEN->getValue(), testing::Eq(""));

	// when left square bracket token, then
	ASSERT_THAT(JsonToken::LEFT_SQUARE_BRACKET_TOKEN->getValue(), testing::Eq(""));

	// when right square bracket token, then
	ASSERT_THAT(JsonToken::RIGHT_SQUARE_BRACKET_TOKEN->getValue(), testing::Eq(""));

	// when comma token, then
	ASSERT_THAT(JsonToken::COMMA_TOKEN->getValue(), testing::Eq(""));

	// when colon token, then
	ASSERT_THAT(JsonToken::COLON_TOKEN->getValue(), testing::Eq(""));
}

TEST_F(JsonTokenTest, createStringTokenReturnsAppropriateJsonToken)
{
	// when
	auto obtained = JsonToken::createStringToken("foobar");

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getTokenType(), testing::Eq(JsonTokenType::VALUE_STRING));
	ASSERT_THAT(obtained->getValue(), testing::Eq("foobar"));
}

TEST_F(JsonTokenTest, createNumberTokenReturnsAppropriateJsonToken)
{
	// when
	auto obtained = JsonToken::createNumberToken("12345");

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getTokenType(), testing::Eq(JsonTokenType::VALUE_NUMBER));
	ASSERT_THAT(obtained->getValue(), testing::Eq("12345"));
}

TEST_F(JsonTokenTest, tokenTypeToStringReturnsAppropriateStringRepresentation)
{
	// when called with number, then
	ASSERT_THAT(JsonToken::tokenTypeToString(JsonTokenType::VALUE_NUMBER), testing::Eq("NUMBER"));

	// when called with string, then
	ASSERT_THAT(JsonToken::tokenTypeToString(JsonTokenType::VALUE_STRING), testing::Eq("STRING"));

	// when called with boolean, then
	ASSERT_THAT(JsonToken::tokenTypeToString(JsonTokenType::LITERAL_BOOLEAN), testing::Eq("BOOLEAN"));

	// when called with null, then
	ASSERT_THAT(JsonToken::tokenTypeToString(JsonTokenType::LITERAL_NULL), testing::Eq(JsonLiterals::NULL_LITERAL));

	// when called with left brace, then
	ASSERT_THAT(JsonToken::tokenTypeToString(JsonTokenType::LEFT_BRACE), testing::Eq("{"));

	// when called with right brace, then
	ASSERT_THAT(JsonToken::tokenTypeToString(JsonTokenType::RIGHT_BRACE), testing::Eq("}"));

	// when called with left square bracket, then
	ASSERT_THAT(JsonToken::tokenTypeToString(JsonTokenType::LEFT_SQUARE_BRACKET), testing::Eq("["));

	// when called with right square bracket, then
	ASSERT_THAT(JsonToken::tokenTypeToString(JsonTokenType::RIGHT_SQUARE_BRACKET), testing::Eq("]"));

	// when called with comma, then
	ASSERT_THAT(JsonToken::tokenTypeToString(JsonTokenType::COMMA), testing::Eq(","));

	// when called with colon, then
	ASSERT_THAT(JsonToken::tokenTypeToString(JsonTokenType::COLON), testing::Eq(":"));
}

TEST_F(JsonTokenTest, toStringForTokenWithoutValueGivesAppropriateStringRepresentation)
{
	// given a token that does not store a value
	auto target = JsonToken::COLON_TOKEN;

	// when
	auto obtained = target->toString();

	// then
	ASSERT_THAT(obtained, testing::Eq("JsonToken {tokenType=:, value=}"));
}

TEST_F(JsonTokenTest, toStringForTokenWithValueGivesAppropriateStringRepresentation)
{
	// given
	auto target = JsonToken::createNumberToken("12345");

	// when
	auto obtained = target->toString();

	// then
	ASSERT_THAT(obtained, testing::Eq("JsonToken {tokenType=NUMBER, value=12345}"));
}