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

#include "util/json/JsonParser.h"
#include "util/json/parser/JsonParserException.h"
#include "util/json/constants/JsonLiterals.h"
#include "OpenKit/json/JsonNullValue.h"
#include "OpenKit/json/JsonBooleanValue.h"
#include "OpenKit/json/JsonStringValue.h"
#include "OpenKit/json/JsonNumberValue.h"
#include <OpenKit/json/JsonObjectValue.h>
#include <OpenKit/json/JsonArrayValue.h>
#include <OpenKit/json/JsonValueType.h>
#include "lexer/MockJsonLexer.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <sstream>
#include <iomanip>

using namespace openkit::json;
using namespace util::json;
using namespace util::json::parser;

#define CAST_TOKEN(token, tokenClass) std::dynamic_pointer_cast<tokenClass>(token)

#define CAST_BOOLEAN_TOKEN(token) CAST_TOKEN(token, JsonBooleanValue)
#define CAST_NUMBER_TOKEN(token) CAST_TOKEN(token, JsonNumberValue)
#define CAST_STRING_TOKEN(token) CAST_TOKEN(token, JsonStringValue)
#define CAST_ARRAY_TOKEN(token) CAST_TOKEN(token, JsonArrayValue)
#define CAST_OBJECT_TOKEN(token) CAST_TOKEN(token, JsonObjectValue)

MATCHER(isNullToken, "")
{
	(void)result_listener; // result_listener argument is unused, keep compiler quiet

	if (arg == nullptr)
	{
		return false;
	}

	return arg->getValueType() == JsonValueType::NULL_VALUE;
}

MATCHER_P(isBooleanTokenOf, value, "")
{
	(void)result_listener; // result_listener argument is unused, keep compiler quiet

	if (arg == nullptr)
	{
		return false;
	}
	if (arg->getValueType() != JsonValueType::BOOLEAN_VALUE)
	{
		return false;
	}

	auto castedValue = CAST_BOOLEAN_TOKEN(arg);
	if (castedValue == nullptr)
	{
		return false;
	}

	return castedValue->getValue() == value;
}

MATCHER_P(isStringTokenOf, value, "")
{
	(void)result_listener; // result_listener argument is unused, keep compiler quiet

	if (arg == nullptr)
	{
		return false;
	}
	if(arg->getValueType() != JsonValueType::STRING_VALUE)
	{
		return false;
	}

	auto castedValue = CAST_STRING_TOKEN(arg);
	if (castedValue == nullptr)
	{
		return false;
	}

	return castedValue->getValue() == value;
}

MATCHER_P(isNumberLongTokenOf, value, "")
{
	(void)result_listener; // result_listener argument is unused, keep compiler quiet

	if (arg == nullptr)
	{
		return false;
	}
	if (arg->getValueType() != JsonValueType::NUMBER_VALUE)
	{
		return false;
	}

	auto castedValue = CAST_NUMBER_TOKEN(arg);
	if (castedValue == nullptr)
	{
		return false;
	}

	return castedValue->getLongValue() == value;
}

MATCHER_P(isNumberDecimalTokenOf, value, "")
{
	(void)result_listener; // result_listener argument is unused, keep compiler quiet

	if (arg == nullptr)
	{
		return false;
	}
	if (arg->getValueType() != JsonValueType::NUMBER_VALUE)
	{
		return false;
	}

	auto castedValue = CAST_NUMBER_TOKEN(arg);
	if (castedValue == nullptr)
	{
		return false;
	}

	return castedValue->getDoubleValue() == value;
}

MATCHER_P(isArrayOfSize, size, "")
{
	(void)result_listener; // result_listener argument is unused, keep compiler quiet

	if (arg == nullptr)
	{
		return false;
	}
	if (arg->getValueType() != JsonValueType::ARRAY_VALUE)
	{
		return false;
	}

	auto castedValue = CAST_ARRAY_TOKEN(arg);
	if (castedValue == nullptr)
	{
		return false;
	}

	return castedValue->size() == static_cast<size_t>(size);
}

MATCHER_P(isObjectOfSize, size, "")
{
	(void)result_listener; // result_listener argument is unused, keep compiler quiet

	if (arg == nullptr)
	{
		return false;
	}
	if (arg->getValueType() != JsonValueType::OBJECT_VALUE)
	{
		return false;
	}

	auto castedValue = CAST_OBJECT_TOKEN(arg);
	if (castedValue == nullptr)
	{
		return false;
	}

	return castedValue->size() == static_cast<size_t>(size);
}


class JsonParserTest : public testing::Test
{
protected:
	int32_t doublePrecision = 17;
	double pi = 3.14159265358979323846;
	double euler = 2.71828182845904523536;

	static const std::string getNestedExceptionMessage(const std::exception& exception)
	{
		try
		{
			std::rethrow_if_nested(exception);
		}
		catch (const std::exception& e)
		{
			return e.what();
		}

		throw std::logic_error("not a nested exception");
	}

	static const std::shared_ptr<JsonValue> getValueAt(const std::shared_ptr<JsonArrayValue> arrayValue, size_t index)
	{
		size_t currentElementIndex = 0;
		for (auto jsonValue : *arrayValue)
		{
			if (currentElementIndex++ == index)
			{
				return jsonValue;
			}
		}

		return nullptr;
	}

	static void assertParseThrowsParserExceptionWithNested(JsonParser& parser, const std::string& message, const std::string& nestedMessage)
	{
		try
		{
			parser.parse();
			FAIL() << "Expected JsonParserException to be thrown";
		}
		catch (JsonParserException& e)
		{
			ASSERT_THAT(e.getMessage(), testing::Eq(message));

			auto nestedExceptionMsg = getNestedExceptionMessage(e);
			ASSERT_THAT(nestedExceptionMsg, testing::Eq(nestedMessage));
		}
	}

	static void assertParseThrowsParserException(JsonParser& parser, const std::string& message)
	{
		try
		{
			parser.parse();
			FAIL() << "Expected JsonParserException to be thrown";
		}
		catch (JsonParserException& e)
		{
			ASSERT_THAT(e.getMessage(), testing::Eq(message));
		}
	}

	void assertObjectKeys(const std::shared_ptr<JsonObjectValue> objectValue, const std::initializer_list<std::string>& keys)
	{
		ASSERT_THAT(objectValue->size(), testing::Eq(keys.size()));
		for (const auto& key : keys)
		{
			ASSERT_THAT(objectValue->containsKey(key), testing::Eq(true));
		}
	}
};

TEST_F(JsonParserTest, aLexerExceptionIsCaughtAndRethrowAsParserException)
{
	// given
	auto lexerException = lexer::JsonLexerException("dummy");
	auto mockLexer = std::make_shared<testing::NiceMock<test::MockJsonLexer>>("");
	ON_CALL(*mockLexer, nextToken()).WillByDefault(testing::Throw(lexerException));

	auto target = JsonParser(mockLexer);

	// when, then
	assertParseThrowsParserExceptionWithNested(target, "Caught exception from lexical analysis", lexerException.what());
}

TEST_F(JsonParserTest, aJsonParserInErroneousStateThrowsExceptionIfParseIsCalledAgain)
{
	// given
	auto lexerException = lexer::JsonLexerException("dummy");
	auto mockLexer = std::make_shared<testing::NiceMock<test::MockJsonLexer>>("");
	ON_CALL(*mockLexer, nextToken()).WillByDefault(testing::Throw(lexerException));

	auto target = JsonParser(mockLexer);

	// when, then
	assertParseThrowsParserExceptionWithNested(target, "Caught exception from lexical analysis", lexerException.what());
	assertParseThrowsParserException(target, "JSON parser is in erroneous state");
}

TEST_F(JsonParserTest, parsingAnEmptyJsonInputStringThrowsAnException)
{
	// given
	auto target = JsonParser("");

	// when, then
	assertParseThrowsParserException(target, "No JSON object could be decoded");

	// and the parser is also an error state
	ASSERT_THAT(target.getState(), testing::Eq(JsonParserState::ERROR));
}

TEST_F(JsonParserTest, parsingNullLiteralValueWorks)
{
	// given
	auto target = JsonParser(constants::JsonLiterals::NULL_LITERAL);

	// when
	auto obtained = target.parse();

	// then
	ASSERT_THAT(obtained,testing::NotNull());
	ASSERT_THAT(obtained->getValueType(), testing::Eq(JsonValueType::NULL_VALUE));

	// and the parser is in end state
	ASSERT_THAT(target.getState(), testing::Eq(JsonParserState::END));
}

TEST_F(JsonParserTest,parsingBooleanFalseLiteralValueWorks)
{
	// given
	auto target = JsonParser(constants::JsonLiterals::BOOLEAN_FALSE_LITERAL);

	// when
	auto obtained = target.parse();

	// then
	ASSERT_THAT(obtained, isBooleanTokenOf(false));

	// and the parser is in end state
	ASSERT_THAT(target.getState(), testing::Eq(JsonParserState::END));
}

TEST_F(JsonParserTest, parsingBooleanTrueLiteralWorks)
{
	// given
	auto target = JsonParser(constants::JsonLiterals::BOOLEAN_TRUE_LITERAL);

	// when
	auto obtained = target.parse();

	// then
	ASSERT_THAT(obtained, isBooleanTokenOf(true));

	// and the parser is in end state
	ASSERT_THAT(target.getState(), testing::Eq(JsonParserState::END));
}

TEST_F(JsonParserTest, parsingStringValueWorks)
{
	// given
	auto target = JsonParser("\"foobar\"");

	// when
	auto obtained = target.parse();

	// then
	ASSERT_THAT(obtained, isStringTokenOf("foobar"));

	// and the parser is in end state
	ASSERT_THAT(target.getState(), testing::Eq(JsonParserState::END));
}

TEST_F(JsonParserTest, parsingIntegerNumberValueWorks)
{
	// given
	auto target = JsonParser("1234567890");

	// when
	auto obtained = target.parse();

	// then
	ASSERT_THAT(obtained, isNumberLongTokenOf(1234567890L));

	// and the parser is in end state
	ASSERT_THAT(target.getState(), testing::Eq(JsonParserState::END));
}

TEST_F(JsonParserTest, parsingFloatingPointNumberValueWorks)
{
	// given
	auto target = JsonParser("5.125");

	// when
	auto obtained = target.parse();

	// then
	ASSERT_THAT(obtained, isNumberDecimalTokenOf(5.125));

	// and the parser is end state
	ASSERT_THAT(target.getState(), testing::Eq(JsonParserState::END));
}

TEST_F(JsonParserTest, callingParseSubsequentlyReturnsAlreadyParsedObject)
{
	// given
	auto target = JsonParser(constants::JsonLiterals::NULL_LITERAL);

	// when
	auto obtained1 = target.parse();
	auto obtained2 = target.parse();

	// then
	ASSERT_THAT(obtained1, testing::Eq(obtained2));
}

TEST_F(JsonParserTest, parsingMultipleJsonValuesFails)
{
	// given
	std::stringstream input;
	input << constants::JsonLiterals::NULL_LITERAL <<  " " << constants::JsonLiterals::BOOLEAN_TRUE_LITERAL;
	auto target = JsonParser(input.str());

	// when then
	assertParseThrowsParserException(target, "Unexpected token \"JsonToken {tokenType=BOOLEAN, value=true}\" at end of input");

	// and also check transition to error state
	ASSERT_THAT(target.getState(), testing::Eq(JsonParserState::ERROR));
}

TEST_F(JsonParserTest, parsingRightSquareBracketAsFirstTokenThrowsAnException)
{
	// given
	auto target = JsonParser("]");

	// when, then
	assertParseThrowsParserException(target, "Unexpected token \"JsonToken {tokenType=], value=}\" at start of input");

	// and also check transition to error state
	ASSERT_THAT(target.getState(), testing::Eq(JsonParserState::ERROR));
}

TEST_F(JsonParserTest, parsingRightBraceAsFirstTokenThrowsAnException)
{
	// given
	auto target = JsonParser("}");

	// when, then
	assertParseThrowsParserException(target, "Unexpected token \"JsonToken {tokenType=}, value=}\" at start of input");

	// and also check transition into error state
	ASSERT_THAT(target.getState(), testing::Eq(JsonParserState::ERROR));
}

TEST_F(JsonParserTest, parsingCommaAsFirstTokenThrowsAnException)
{
	// given
	auto target = JsonParser(",");

	// when, then
	assertParseThrowsParserException(target, "Unexpected token \"JsonToken {tokenType=,, value=}\" at start of input");

	// and also check transition to error state
	ASSERT_THAT(target.getState(), testing::Eq(JsonParserState::ERROR));
}

TEST_F(JsonParserTest, parsingColonAsFirstTokenThrowsAnException)
{
	// given
	auto target = JsonParser(":");

	// when, then
	assertParseThrowsParserException(target, "Unexpected token \"JsonToken {tokenType=:, value=}\" at start of input");

	// and also check transition to error state
	ASSERT_THAT(target.getState(), testing::Eq(JsonParserState::ERROR));
}

TEST_F(JsonParserTest, parsingUnterminatedEmptyArrayThrowsAnException)
{
	// given
	auto target = JsonParser("[");

	// when, then
	assertParseThrowsParserException(target, "Unterminated JSON array");

	// and also check transition to error state
	ASSERT_THAT(target.getState(), testing::Eq(JsonParserState::ERROR));
}

TEST_F(JsonParserTest, parsingEmptyArrayWorks)
{
	// given
	auto target = JsonParser("[]");

	// when
	auto obtained = target.parse();

	// then
	ASSERT_THAT(obtained, isArrayOfSize(0));
}

TEST_F(JsonParserTest, parsingArrayWithSingleTrueValueWorks)
{
	// given
	std::stringstream input;
	input << "[" << constants::JsonLiterals::BOOLEAN_TRUE_LITERAL << "]";
	auto target = JsonParser(input.str());

	// when
	auto obtained = target.parse();

	// then
	ASSERT_THAT(obtained, isArrayOfSize(1));
	auto jsonArray = CAST_ARRAY_TOKEN(obtained);

	auto obtainedArrayValue = getValueAt(jsonArray, 0);
	ASSERT_THAT(obtainedArrayValue, isBooleanTokenOf(true));
}

TEST_F(JsonParserTest, parsingArrayWithSingleFalseValueWorks)
{
	// given
	std::stringstream input;
	input << "[" << constants::JsonLiterals::BOOLEAN_FALSE_LITERAL << "]";
	auto target = JsonParser(input.str());

	// when
	auto obtained = target.parse();

	// then
	ASSERT_THAT(obtained, isArrayOfSize(1));
	auto jsonArray = CAST_ARRAY_TOKEN(obtained);

	auto obtainedArrayValue = getValueAt(jsonArray,0);
	ASSERT_THAT(obtainedArrayValue, isBooleanTokenOf(false));
}

TEST_F(JsonParserTest, parsingArrayWithSingleNumberValueWorks)
{
	// given
	std::stringstream input;
	input << "[" << std::setprecision(17) << pi << "]";
	auto target = JsonParser(input.str());

	// when
	auto obtained = target.parse();

	// then
	ASSERT_THAT(obtained, isArrayOfSize(1));
	auto jsonArray = CAST_ARRAY_TOKEN(obtained);

	auto obtainedArrayValue = getValueAt(jsonArray, 0);
	ASSERT_THAT(obtainedArrayValue, isNumberDecimalTokenOf(pi));
}

TEST_F(JsonParserTest, parsingArrayWithSingleStringworks)
{
	// given
	auto target = JsonParser("[\"foobar\"]");

	// when
	auto obtained = target.parse();

	// then
	ASSERT_THAT(obtained, isArrayOfSize(1));
	auto jsonArray = CAST_ARRAY_TOKEN(obtained);

	auto obtainedArrayValue = getValueAt(jsonArray, 0);
	ASSERT_THAT(obtainedArrayValue, isStringTokenOf("foobar"));
}

TEST_F(JsonParserTest, parsingArrayWithMultipleSimpleValuesWorks)
{
	// given
	std::stringstream input;
	input << "["
		<< constants::JsonLiterals::NULL_LITERAL
		<< ", " << constants::JsonLiterals::BOOLEAN_TRUE_LITERAL
		<< ", " << constants::JsonLiterals::BOOLEAN_FALSE_LITERAL
		<< ", " << std::setprecision(doublePrecision) << euler
		<< ", " << "\"Hello World!\"]";
	auto target = JsonParser(input.str());

	// when
	auto obtained = target.parse();

	// then

	ASSERT_THAT(obtained, isArrayOfSize(5));
	auto obtainedArray = CAST_ARRAY_TOKEN(obtained);

	auto obtainedFirstValue = getValueAt(obtainedArray,0);
	ASSERT_THAT(obtainedFirstValue, isNullToken());

	auto obtainedSecondValue = getValueAt(obtainedArray, 1);
	ASSERT_THAT(obtainedSecondValue, isBooleanTokenOf(true));

	auto obtainedThirdValue = getValueAt(obtainedArray, 2);
	ASSERT_THAT(obtainedThirdValue, isBooleanTokenOf(false));

	auto obtainedFourthValue = getValueAt(obtainedArray, 3);
	ASSERT_THAT(obtainedFourthValue, isNumberDecimalTokenOf(euler));

	auto obtainedFifthValue = getValueAt(obtainedArray, 4);
	ASSERT_THAT(obtainedFifthValue, isStringTokenOf("Hello World!"));
}

TEST_F(JsonParserTest, parsingCommaRightAfterArrayStartThrowsAnException)
{
	// given
	auto target = JsonParser("[,");

	// when, then
	assertParseThrowsParserException(target, "Unexpected token \"JsonToken {tokenType=,, value=}\" at beginning of array");

	// and also check transition to error state
	ASSERT_THAT(target.getState(), testing::Eq(JsonParserState::ERROR));
}

TEST_F(JsonParserTest, parsingColonRightAfterArrayStartThrowsAnException)
{
	// given
	auto target = JsonParser("[:");

	// when, then
	assertParseThrowsParserException(target,"Unexpected token \"JsonToken {tokenType=:, value=}\" at beginning of array" );

	// and also check transition to error state
	ASSERT_THAT(target.getState(), testing::Eq(JsonParserState::ERROR));
}

TEST_F(JsonParserTest, parsingUnterminatedArrayAfterValueThrowsAnException)
{
	// given
	auto target = JsonParser("[42");

	// when, then
	assertParseThrowsParserException(target, "Unterminated JSON array");

	// and also check transition to error state
	ASSERT_THAT(target.getState(), testing::Eq(JsonParserState::ERROR));
}

TEST_F(JsonParserTest, parsingUnterminatedArrayAfterValueDelimiterThrowsAnException)
{
	// given
	auto target = JsonParser("[42, ");

	// when, then
	assertParseThrowsParserException(target, "Unterminated JSON array");

	// and also check transition to error state
	ASSERT_THAT(target.getState(), testing::Eq(JsonParserState::ERROR));
}

TEST_F(JsonParserTest, parsingMultipleArrayValuesWithoutDelimiterThrowsAnException)
{
	// given
	auto target = JsonParser("[42 45]");

	// when, then
	assertParseThrowsParserException(target, "Unexpected token \"JsonToken {tokenType=NUMBER, value=45}\" in array after value was parsed");

	// and also check transition to error state
	ASSERT_THAT(target.getState(), testing::Eq(JsonParserState::ERROR));
}

TEST_F(JsonParserTest, parsingValueDelimiterAsLastArrayTokenThrowsAnException)
{
	// given
	auto target = JsonParser("[42, 45,]");

	// when, then
	assertParseThrowsParserException(target, "Unexpected token \"JsonToken {tokenType=], value=}\" in array after delimiter");

	// and also check transition to error state
	ASSERT_THAT(target.getState(), testing::Eq(JsonParserState::ERROR));
}

TEST_F(JsonParserTest, parsingNestedEmptyArrayWorks)
{
	// given
	auto target = JsonParser("[[]]");

	// when
	auto obtained = target.parse();

	// then
	ASSERT_THAT(obtained, isArrayOfSize(1));
	auto obtainedArray = CAST_ARRAY_TOKEN(obtained);

	auto arrayValue = getValueAt(obtainedArray, 0);
	ASSERT_THAT(arrayValue, isArrayOfSize(0));
}

TEST_F(JsonParserTest, parsingMultipleEmptyArraysWorks)
{
	// given
	auto target = JsonParser("[[], []]");

	// when
	auto obtained = target.parse();

	// then
	ASSERT_THAT(obtained, isArrayOfSize(2));
	auto obtainedArray = CAST_ARRAY_TOKEN(obtained);

	auto firstArrayValue = getValueAt(obtainedArray, 0);
	ASSERT_THAT(firstArrayValue, isArrayOfSize(0));

	auto secondArrayValue = getValueAt(obtainedArray, 1);
	ASSERT_THAT(secondArrayValue, isArrayOfSize(0));
}

TEST_F(JsonParserTest, parsingNestedArrayWithValuesWorks)
{
	// given
	std::stringstream input;
	input << "[[" << constants::JsonLiterals::NULL_LITERAL
		<< ", " << constants::JsonLiterals::BOOLEAN_TRUE_LITERAL
		<< ", " << constants::JsonLiterals::BOOLEAN_FALSE_LITERAL
		<< ", " << std::setprecision(doublePrecision) << euler
		<< ", \"Hello World!\"]]";
	auto target = JsonParser(input.str());

	// when
	auto obtained = target.parse();

	// then
	ASSERT_THAT(obtained, isArrayOfSize(1));
	auto obtainedArray = CAST_ARRAY_TOKEN(obtained);

	auto arrayValue = getValueAt(obtainedArray, 0);
	ASSERT_THAT(arrayValue, isArrayOfSize(5));

	auto innerArray = CAST_ARRAY_TOKEN(arrayValue);
	auto innerFirstValue = getValueAt(innerArray, 0);
	ASSERT_THAT(innerFirstValue,isNullToken());

	auto innerSecondValue = getValueAt(innerArray, 1);
	ASSERT_THAT(innerSecondValue, isBooleanTokenOf(true));

	auto innerThirdValue = getValueAt(innerArray, 2);
	ASSERT_THAT(innerThirdValue,isBooleanTokenOf(false));

	auto innerFourthValue = getValueAt(innerArray, 3);
	ASSERT_THAT(innerFourthValue, isNumberDecimalTokenOf(euler));

	auto innerFifthValue = getValueAt(innerArray, 4);
	ASSERT_THAT(innerFifthValue, isStringTokenOf("Hello World!"));
}

TEST_F(JsonParserTest, parsingMultipleNestedArrayWithValuesWorks)
{
	// given
	std::stringstream input;
	input << "[" << constants::JsonLiterals::NULL_LITERAL
		<< ", [" << constants::JsonLiterals::BOOLEAN_TRUE_LITERAL
		<< ", [" << constants::JsonLiterals::BOOLEAN_FALSE_LITERAL
		<< ", " << std::setprecision(doublePrecision) << euler
		<< ", \"Hello World!\"]]]";
	auto target = JsonParser(input.str());

	// when
	auto obtained = target.parse();

	// then
	ASSERT_THAT(obtained, isArrayOfSize(2));
	auto obtainedArray = CAST_ARRAY_TOKEN(obtained);

	auto levelOneValueOne = getValueAt(obtainedArray, 0);
	ASSERT_THAT(levelOneValueOne, isNullToken());

	auto levelOneValueTwo = getValueAt(obtainedArray, 1);
	ASSERT_THAT(levelOneValueTwo, isArrayOfSize(2));
	auto levelOneValueTwoArray = CAST_ARRAY_TOKEN(levelOneValueTwo);

	auto levelTwoValueOne = getValueAt(levelOneValueTwoArray, 0);
	ASSERT_THAT(levelTwoValueOne, isBooleanTokenOf(true));

	auto levelTwoValueTwo = getValueAt(levelOneValueTwoArray, 1);
	ASSERT_THAT(levelTwoValueTwo, isArrayOfSize(3));
	auto levelTwoValueTwoArray = CAST_ARRAY_TOKEN(levelTwoValueTwo);

	auto levelThreeValueOne = getValueAt(levelTwoValueTwoArray, 0);
	ASSERT_THAT(levelThreeValueOne, isBooleanTokenOf(false));

	auto levelThreeValueTwo = getValueAt(levelTwoValueTwoArray, 1);
	ASSERT_THAT(levelThreeValueTwo, isNumberDecimalTokenOf(euler));

	auto levelThreeValueThree = getValueAt(levelTwoValueTwoArray, 2);
	ASSERT_THAT(levelThreeValueThree, isStringTokenOf("Hello World!"));
}

TEST_F(JsonParserTest, parsingUnterminatedNestedArrayThrowsAnException)
{
	// given
	std::stringstream input;
	input << "[[" << constants::JsonLiterals::NULL_LITERAL
		<< ", " << constants::JsonLiterals::BOOLEAN_TRUE_LITERAL
		<< ", " << constants::JsonLiterals::BOOLEAN_FALSE_LITERAL
		<< ", " << std::setprecision(doublePrecision) << euler
		<< ", \"Hello World!\"]";
	auto target = JsonParser(input.str());

	// when, then
	assertParseThrowsParserException(target, "Unterminated JSON array");

	// and also check transition to error state
	ASSERT_THAT(target.getState(), testing::Eq(JsonParserState::ERROR));
}

TEST_F(JsonParserTest, parsingUnterminatedEmptyObjectThrowsAnException)
{
	// given
	auto target = JsonParser("{");

	// when, then
	assertParseThrowsParserException(target, "Unterminated JSON object");

	// and also check transition to error state
	ASSERT_THAT(target.getState(), testing::Eq(JsonParserState::ERROR));
}

TEST_F(JsonParserTest, parsingEmptyObjectWorks)
{
	// given
	auto target = JsonParser("{}");

	// when
	auto obtained = target.parse();

	// then
	ASSERT_THAT(obtained, isObjectOfSize(0));
}

TEST_F(JsonParserTest, parsingObjectWithNullLiteralAsKeyThrowsAnException)
{
	// given
	auto target = JsonParser(R"({null: "foo"})");

	// when, then
	assertParseThrowsParserException(target, "Unexpected token \"JsonToken {tokenType=null, value=null}\" encountered - object key expected");

	// and also check transition to error state
	ASSERT_THAT(target.getState(), testing::Eq(JsonParserState::ERROR));
}

TEST_F(JsonParserTest, parsingObjectWithBooleanTrueLiteralAsKeyThrowsAnException)
{
	// given
	auto target = JsonParser(R"({true: "foo"})");

	// when, then
	assertParseThrowsParserException(target, "Unexpected token \"JsonToken {tokenType=BOOLEAN, value=true}\" encountered - object key expected");

	// and also check transition to error state
	ASSERT_THAT(target.getState(), testing::Eq(JsonParserState::ERROR));
}

TEST_F(JsonParserTest, parsingObjectWithBooleanFalseLiteralAsKeyThrowsAnException)
{
	// given
	auto target = JsonParser(R"({false: "foo"})");

	// when, then
	assertParseThrowsParserException(target, "Unexpected token \"JsonToken {tokenType=BOOLEAN, value=false}\" encountered - object key expected");

	// and also check transition to error state
	ASSERT_THAT(target.getState(), testing::Eq(JsonParserState::ERROR));
}

TEST_F(JsonParserTest, parsingObjectWithNumberAsKeyThrowsAnException)
{
	// given
	auto target = JsonParser(R"({42: "foo"})");

	// when, then
	assertParseThrowsParserException(target, "Unexpected token \"JsonToken {tokenType=NUMBER, value=42}\" encountered - object key expected");

	// and also check transition to error state
	ASSERT_THAT(target.getState(), testing::Eq(JsonParserState::ERROR));
}

TEST_F(JsonParserTest, parsingObjectWithArrayAsKeyThrowsAnException)
{
	// given
	auto target = JsonParser(R"({[]: "foo"})");

	// when,then
	assertParseThrowsParserException(target, "Unexpected token \"JsonToken {tokenType=[, value=}\" encountered - object key expected");

	// and also check transition to error state
	ASSERT_THAT(target.getState(), testing::Eq(JsonParserState::ERROR));
}

TEST_F(JsonParserTest, parsingObjectWithObjectWasKeyThrowsAnException)
{
	// given
	auto target = JsonParser(R"({{}: "foo"})");

	// when, then
	assertParseThrowsParserException(target, "Unexpected token \"JsonToken {tokenType={, value=}\" encountered - object key expected");

	// and check also transition to error state
	ASSERT_THAT(target.getState(), testing::Eq(JsonParserState::ERROR));
}

TEST_F(JsonParserTest, parsingObjectWithoutKeyTokenThrowsAnException)
{
	// given
	auto target = JsonParser(R"({: "foo"})");

	// when, then
	assertParseThrowsParserException(target, "Unexpected token \"JsonToken {tokenType=:, value=}\" encountered - object key expected");

	// and also check transition to error state
	ASSERT_THAT(target.getState(), testing::Eq(JsonParserState::ERROR));
}

TEST_F(JsonParserTest, parsingObjectWithCommaAsKeyThrowsAnException)
{
	// given
	auto target = JsonParser(R"({, "foo"})");

	// when, then
	assertParseThrowsParserException(target, "Unexpected token \"JsonToken {tokenType=,, value=}\" encountered - object key expected");

	// and also check transition to error state
	ASSERT_THAT(target.getState(), testing::Eq(JsonParserState::ERROR));
}

TEST_F(JsonParserTest, parsingObjectWithValidKeyButNoKeyValueDelimiterThrowsAnException)
{
	// given
	auto target = JsonParser(R"({"foo" "bar"})");

	// when, then
	assertParseThrowsParserException(target,"Unexpected token \"JsonToken {tokenType=STRING, value=bar}\" encountered - key-value delimiter expected" );

	// and also check transition to error state
	ASSERT_THAT(target.getState(), testing::Eq(JsonParserState::ERROR));
}

TEST_F(JsonParserTest, parsingObjectWithStringKeyAndNullValueWorks)
{
	// given
	auto target = JsonParser(R"({"foo": null})");

	// when
	auto obtained = target.parse();

	// then
	ASSERT_THAT(obtained, isObjectOfSize(1));
	auto obtainedObject = CAST_OBJECT_TOKEN(obtained);
	assertObjectKeys(obtainedObject, {"foo"});

	auto objectValue = (*obtainedObject)["foo"];
	ASSERT_THAT(objectValue, isNullToken());
}

TEST_F(JsonParserTest, parsingObjectWithStringKeyAndBooleanTrueValueWorks)
{
	// given
	auto target = JsonParser(R"({"foo": true})");

	// when
	auto obtained = target.parse();

	// then
	ASSERT_THAT(obtained, isObjectOfSize(1));
	auto obtainedObject = CAST_OBJECT_TOKEN(obtained);
	assertObjectKeys(obtainedObject, {"foo"});

	auto objectValue = (*obtainedObject)["foo"];
	ASSERT_THAT(objectValue, isBooleanTokenOf(true));
}

TEST_F(JsonParserTest, parsingObjectWithStringKeyAndBooleanFalseWorks)
{
	// given
	auto target = JsonParser(R"({"foo": false})");

	// when
	auto obtained = target.parse();

	// then
	ASSERT_THAT(obtained, isObjectOfSize(1));
	auto obtainedObject = CAST_OBJECT_TOKEN(obtained);
	assertObjectKeys(obtainedObject, {"foo"});

	auto objectValue = (*obtainedObject)["foo"];
	ASSERT_THAT(objectValue, isBooleanTokenOf(false));
}

TEST_F(JsonParserTest, parsingObjectWithStringKeyAndNumericValueWorks)
{
	// given
	auto target = JsonParser(R"({"foo": 42})");

	// when
	auto obtained = target.parse();

	// then
	ASSERT_THAT(obtained, isObjectOfSize(1));
	auto obtainedObject = CAST_OBJECT_TOKEN(obtained);
	assertObjectKeys(obtainedObject, {"foo"});

	auto objectValue = (*obtainedObject)["foo"];
	ASSERT_THAT(objectValue,isNumberLongTokenOf(42));
}

TEST_F(JsonParserTest, parsingObjectWithStringKeyAndStringValueWorks)
{
	// given
	auto target = JsonParser(R"({"foo": "bar"})");

	// when
	auto obtained = target.parse();

	// then
	ASSERT_THAT(obtained, isObjectOfSize(1));
	auto obtainedObject = CAST_OBJECT_TOKEN(obtained);
	assertObjectKeys(obtainedObject, {"foo"});

	auto objectValue = (*obtainedObject)["foo"];
	ASSERT_THAT(objectValue, isStringTokenOf("bar"));
}

TEST_F(JsonParserTest, parsingUnterminatedObjectAfterKeyHasBeenParsedThrowsAnException)
{
	// given
	auto target = JsonParser(R"({"foo")");

	// when, then
	assertParseThrowsParserException(target, "Unterminated JSON object");

	// and also check transition to error state
	ASSERT_THAT(target.getState(), testing::Eq(JsonParserState::ERROR));
}

TEST_F(JsonParserTest, parsingUnterminatedObjectAfterKeyValueDelimiterHasBeenParsedThrowsAnException)
{
	// given
	auto target = JsonParser(R"({"foo": )");

	// when, then
	assertParseThrowsParserException(target, "Unterminated JSON object");

	// and also check transition to error state
	ASSERT_THAT(target.getState(), testing::Eq(JsonParserState::ERROR));
}

TEST_F(JsonParserTest, parsingUnterminatedObjectAfterValueHasBeenParsedThrowsAnException)
{
	// given
	auto target = JsonParser(R"({"foo": "bar")");

	// when, then
	assertParseThrowsParserException(target, "Unterminated JSON object");

	// and also check transition to error state
	ASSERT_THAT(target.getState(), testing::Eq(JsonParserState::ERROR));
}

TEST_F(JsonParserTest, parsingMultipleKeyValuePairsWorks)
{
	// given
	auto target = JsonParser(R"({"a": null, "b": false, "c": true, "d": 123.5, "e": "foobar"})");

	// when
	auto obtained = target.parse();

	// then
	ASSERT_THAT(obtained, isObjectOfSize(5));
	auto obtainedObject = CAST_OBJECT_TOKEN(obtained);
	assertObjectKeys(obtainedObject,{"a", "b", "c", "d","e"});

	auto firstValue = (*obtainedObject)["a"];
	ASSERT_THAT(firstValue, isNullToken());

	auto secondValue = (*obtainedObject)["b"];
	ASSERT_THAT(secondValue, isBooleanTokenOf(false));

	auto thirdValue = (*obtainedObject)["c"];
	ASSERT_THAT(thirdValue, isBooleanTokenOf(true));

	auto fourthVale = (*obtainedObject)["d"];
	ASSERT_THAT(fourthVale, isNumberDecimalTokenOf(123.5));

	auto fifthValue = (*obtainedObject)["e"];
	ASSERT_THAT(fifthValue, isStringTokenOf("foobar"));
}

TEST_F(JsonParserTest, parsingJsonObjectOverwritesValuesIfKeyIsDuplicate)
{
	// given
	auto target = JsonParser(R"({"a": null, "b": false, "a": true, "c": 123.5, "a": "foobar"})");

	// when
	auto obtained = target.parse();

	// then
	ASSERT_THAT(obtained, isObjectOfSize(3));
	auto obtainedObject = CAST_OBJECT_TOKEN(obtained);
	assertObjectKeys(obtainedObject,{"a", "b","c"});

	auto firstValue = (*obtainedObject)["a"];
	ASSERT_THAT(firstValue, isStringTokenOf("foobar"));

	auto secondValue = (*obtainedObject)["b"];
	ASSERT_THAT(secondValue, isBooleanTokenOf(false));

	auto thirdValue = (*obtainedObject)["c"];
	ASSERT_THAT(thirdValue, isNumberDecimalTokenOf(123.5));
}

TEST_F(JsonParserTest, parsingJsonObjectWithIllegalTokenAfterKeyValuePairThrowsAnException)
{
	// given
	auto target = JsonParser(R"({"foo": "bar" :})");

	// when, then
	assertParseThrowsParserException(target, "Unexpected token \"JsonToken {tokenType=:, value=}\" after key-value pair encountered" );

	// and also check transition to error state
	ASSERT_THAT(target.getState(), testing::Eq(JsonParserState::ERROR));
}

TEST_F(JsonParserTest, parsingUnterminatedObjectAfterCommaThrowsAnException)
{
	// given
	auto target = JsonParser(R"({"foo": "bar", )");

	// when, then
	assertParseThrowsParserException(target, "Unterminated JSON object");

	// and also check transition to error state
	ASSERT_THAT(target.getState(), testing::Eq(JsonParserState::ERROR));
}

TEST_F(JsonParserTest, parsingNonStringTokenAsSecondKeyThrowsAnException)
{
	// given
	auto target = JsonParser(R"({"foo": "bar", 123: 456})");

	// when, then
	assertParseThrowsParserException(target, "Unexpected token \"JsonToken {tokenType=NUMBER, value=123}\" encountered - object key expected");

	// and also check transition to error state
	ASSERT_THAT(target.getState(), testing::Eq(JsonParserState::ERROR));
}

TEST_F(JsonParserTest, parsingEmptyJsonArrayAsObjectValueWorks)
{
	// given
	auto target = JsonParser(R"({"a": [], "b": []})");

	// when
	auto obtained = target.parse();

	// then
	ASSERT_THAT(obtained, isObjectOfSize(2));
	auto obtainedObject = CAST_OBJECT_TOKEN(obtained);
	assertObjectKeys(obtainedObject,{"a", "b"});

	auto firstValue = (*obtainedObject)["a"];
	ASSERT_THAT(firstValue, isArrayOfSize(0));

	auto secondValue = (*obtainedObject)["b"];
	ASSERT_THAT(secondValue, isArrayOfSize(0));
}

TEST_F(JsonParserTest, parsingNonEmptyJsonArrayAsObjectValueWorks)
{
	// given
	auto target = JsonParser(R"({"a": [null, true], "b": [false, 42.25, "foobar"]})");

	// when
	auto obtained = target.parse();

	// then
	ASSERT_THAT(obtained, isObjectOfSize(2));
	auto obtainedObject = CAST_OBJECT_TOKEN(obtained);
	assertObjectKeys(obtainedObject, {"a", "b"});

	auto objectFirstValue = (*obtainedObject)["a"];
	ASSERT_THAT(objectFirstValue, isArrayOfSize(2));
	auto objectFirstArray = CAST_ARRAY_TOKEN(objectFirstValue);

	auto firstArrayFirstValue = getValueAt(objectFirstArray, 0);
	ASSERT_THAT(firstArrayFirstValue, isNullToken());

	auto firstArraySecondValue = getValueAt(objectFirstArray, 1);
	ASSERT_THAT(firstArraySecondValue, isBooleanTokenOf(true));

	auto objectSecondValue = (*obtainedObject)["b"];
	ASSERT_THAT(objectSecondValue, isArrayOfSize(3));
	auto objectSecondArray = CAST_ARRAY_TOKEN(objectSecondValue);

	auto secondArrayFirstValue = getValueAt(objectSecondArray, 0);
	ASSERT_THAT(secondArrayFirstValue, isBooleanTokenOf(false));

	auto secondArraySecondValue = getValueAt(objectSecondArray, 1);
	ASSERT_THAT(secondArraySecondValue, isNumberDecimalTokenOf(42.25));

	auto secondArrayThirdValue = getValueAt(objectSecondArray, 2);
	ASSERT_THAT(secondArrayThirdValue, isStringTokenOf("foobar"));
}

TEST_F(JsonParserTest, parsingEmptyJsonObjectAsObjectValueWorks)
{
	// given
	auto target = JsonParser(R"({"a": {}, "b": {}})");

	// when
	auto obtained = target.parse();

	// then
	ASSERT_THAT(obtained, isObjectOfSize(2));
	auto obtainedObject = CAST_OBJECT_TOKEN(obtained);
	assertObjectKeys(obtainedObject, {"a", "b"});

	auto firstValue = (*obtainedObject)["a"];
	ASSERT_THAT(firstValue, isObjectOfSize(0));

	auto secondValue = (*obtainedObject)["b"];
	ASSERT_THAT(secondValue, isObjectOfSize(0));
}

TEST_F(JsonParserTest, parsingNonEmptyJsonObjectAsObjectValueWorks)
{
	// given
	auto target = JsonParser(R"({"a": {"a" : null, "b": true, "c": [false, 42.25, "foobar"]}})");

	// when
	auto obtained = target.parse();

	// then
	ASSERT_THAT(obtained, isObjectOfSize(1));
	auto obtainedObject = CAST_OBJECT_TOKEN(obtained);
	assertObjectKeys(obtainedObject, {"a"});

	auto firstObjectFirstValue = (*obtainedObject)["a"];
	ASSERT_THAT(firstObjectFirstValue, isObjectOfSize(3));
	auto innerObject = CAST_OBJECT_TOKEN(firstObjectFirstValue);
	assertObjectKeys(innerObject, {"a", "b", "c"});

	auto innerObjectFirstValue = (*innerObject)["a"];
	ASSERT_THAT(innerObjectFirstValue, isNullToken());

	auto innerObjectSecondValue = (*innerObject)["b"];
	ASSERT_THAT(innerObjectSecondValue, isBooleanTokenOf(true));

	auto innerObjectThirdValue = (*innerObject)["c"];
	ASSERT_THAT(innerObjectThirdValue, isArrayOfSize(3));
	auto innerArray = CAST_ARRAY_TOKEN(innerObjectThirdValue);

	auto innerArrayFirstValue = getValueAt(innerArray, 0);
	ASSERT_THAT(innerArrayFirstValue, isBooleanTokenOf(false));

	auto innerArraySecondValue = getValueAt(innerArray, 1);
	ASSERT_THAT(innerArraySecondValue, isNumberDecimalTokenOf(42.25));

	auto innerArrayThirdValue = getValueAt(innerArray, 2);
	ASSERT_THAT(innerArrayThirdValue, isStringTokenOf("foobar"));
}

TEST_F(JsonParserTest, parsingEmptyJsonObjectsAsArrayValueWorks)
{
	// given
	auto target = JsonParser("[{}, {}]");

	// when
	auto obtained = target.parse();

	// then
	ASSERT_THAT(obtained, isArrayOfSize(2));
	auto obtainedArray = CAST_ARRAY_TOKEN(obtained);

	auto firstValue = getValueAt(obtainedArray, 0);
	ASSERT_THAT(firstValue, isObjectOfSize(0));

	auto secondValue = getValueAt(obtainedArray, 0);
	ASSERT_THAT(secondValue, isObjectOfSize(0));
}

TEST_F(JsonParserTest, parsingNonEmptyJsonObjectAsArrayValueWorks)
{
	// given
	auto target = JsonParser(R"([{"a": null, "b": 42}, {"x": [true, false]}])");

	// when
	auto obtained = target.parse();

	// then
	ASSERT_THAT(obtained, isArrayOfSize(2));
	auto obtainedArray = CAST_ARRAY_TOKEN(obtained);

	auto arrayFirstValue = getValueAt(obtainedArray, 0);
	ASSERT_THAT(arrayFirstValue, isObjectOfSize(2));
	auto firstObject = CAST_OBJECT_TOKEN(arrayFirstValue);
	assertObjectKeys(firstObject, {"a", "b"});

	auto firstObjectFirstValue = (*firstObject)["a"];
	ASSERT_THAT(firstObjectFirstValue, isNullToken());

	auto firstObjectSecondValue = (*firstObject)["b"];
	ASSERT_THAT(firstObjectSecondValue, isNumberLongTokenOf(42));

	auto arraySecondValue = getValueAt(obtainedArray, 1);
	ASSERT_THAT(arraySecondValue, isObjectOfSize(1));
	auto secondObject = CAST_OBJECT_TOKEN(arraySecondValue);
	assertObjectKeys(secondObject, {"x"});

	auto secondObjectFirstValue = (*secondObject)["x"];
	ASSERT_THAT(secondObjectFirstValue, isArrayOfSize(2));
	auto innerArray = CAST_ARRAY_TOKEN(secondObjectFirstValue);

	auto innerArrayFirstValue = getValueAt(innerArray, 0);
	ASSERT_THAT(innerArrayFirstValue, isBooleanTokenOf(true));

	auto innerArraySecondValue = getValueAt(innerArray, 1);
	ASSERT_THAT(innerArraySecondValue, isBooleanTokenOf(false));
}