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
#include "util/json/lexer/JsonLexerException.h"
#include "util/json/constants/JsonLiterals.h"

#include <sstream>

using namespace util::json::lexer;
using namespace util::json::constants;


const std::shared_ptr<JsonToken> JsonToken::BOOLEAN_TRUE_TOKEN =
		std::shared_ptr<JsonToken>(new JsonToken(JsonTokenType::LITERAL_BOOLEAN, JsonLiterals::BOOLEAN_TRUE_LITERAL));

const std::shared_ptr<JsonToken> JsonToken::BOOLEAN_FALSE_TOKEN =
		std::shared_ptr<JsonToken>(new JsonToken(JsonTokenType::LITERAL_BOOLEAN, JsonLiterals::BOOLEAN_FALSE_LITERAL));

const std::shared_ptr<JsonToken> JsonToken::NULL_TOKEN =
		std::shared_ptr<JsonToken>(new JsonToken(JsonTokenType::LITERAL_NULL, JsonLiterals::NULL_LITERAL));

const std::shared_ptr<JsonToken> JsonToken::LEFT_BRACE_TOKEN =
		std::shared_ptr<JsonToken>(new JsonToken(JsonTokenType::LEFT_BRACE));

const std::shared_ptr<JsonToken> JsonToken::RIGHT_BRACE_TOKEN =
		std::shared_ptr<JsonToken>(new JsonToken(JsonTokenType::RIGHT_BRACE));

const std::shared_ptr<JsonToken> JsonToken::LEFT_SQUARE_BRACKET_TOKEN =
		std::shared_ptr<JsonToken>(new JsonToken(JsonTokenType::LEFT_SQUARE_BRACKET));

const std::shared_ptr<JsonToken> JsonToken::RIGHT_SQUARE_BRACKET_TOKEN =
		std::shared_ptr<JsonToken>(new JsonToken(JsonTokenType::RIGHT_SQUARE_BRACKET));

const std::shared_ptr<JsonToken> JsonToken::COMMA_TOKEN =
		std::shared_ptr<JsonToken>(new JsonToken(JsonTokenType::COMMA));

const std::shared_ptr<JsonToken> JsonToken::COLON_TOKEN =
		std::shared_ptr<JsonToken>(new JsonToken(JsonTokenType::COLON));


JsonToken::JsonToken(JsonTokenType tokenType)
	: JsonToken(tokenType, "")
{
}

JsonToken::JsonToken(const JsonTokenType tokenType, const std::string& value)
	: mTokenType(tokenType), mValue(value)
{
}

std::shared_ptr<JsonToken> JsonToken::createStringToken(const std::string& stringValue)
{
	return std::shared_ptr<JsonToken>(new JsonToken(JsonTokenType::VALUE_STRING, stringValue));
}

std::shared_ptr<JsonToken> JsonToken::createNumberToken(const std::string& numericValue)
{
	return std::shared_ptr<JsonToken>(new JsonToken(JsonTokenType::VALUE_NUMBER, numericValue));
}

JsonTokenType JsonToken::getTokenType() const
{
	return mTokenType;
}

const std::string& JsonToken::getValue() const
{
	return mValue;
}

const std::string JsonToken::toString() const
{
	std::stringstream stream;
	stream << "JsonToken {tokenType=" << tokenTypeToString(mTokenType) << ", value=" << mValue << "}";

	return stream.str();
}

const std::string JsonToken::tokenTypeToString(JsonTokenType tokeType)
{
	switch (tokeType)
	{
		case JsonTokenType::VALUE_NUMBER:
			return "NUMBER";
		case JsonTokenType::VALUE_STRING:
			return "STRING";
		case JsonTokenType::LITERAL_BOOLEAN:
			return "BOOLEAN";
		case JsonTokenType::LITERAL_NULL:
			return JsonLiterals::NULL_LITERAL;
		case JsonTokenType::LEFT_BRACE:
			return "{";
		case JsonTokenType::RIGHT_BRACE:
			return "}";
		case JsonTokenType::LEFT_SQUARE_BRACKET:
			return "[";
		case JsonTokenType::RIGHT_SQUARE_BRACKET:
			return "]";
		case JsonTokenType::COMMA:
			return ",";
		case JsonTokenType::COLON:
			return ":";
	}

	throw JsonLexerException("Unknown token type " + static_cast<std::underlying_type<JsonTokenType>::type>(tokeType));
}