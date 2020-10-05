/**
 * Copyright 2018-2020 Dynatrace LLC
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


const std::shared_ptr<JsonToken> JsonToken::booleanTrueToken()
{
	static const auto booleanTrueToken
		= std::shared_ptr<JsonToken>(new JsonToken(JsonTokenType::LITERAL_BOOLEAN, JsonLiterals::BOOLEAN_TRUE_LITERAL));

	return booleanTrueToken;
}

const std::shared_ptr<JsonToken> JsonToken::booleanFalseToken()
{
	static const auto booleanFalseToken
		= std::shared_ptr<JsonToken>(new JsonToken(JsonTokenType::LITERAL_BOOLEAN, JsonLiterals::BOOLEAN_FALSE_LITERAL));

	return booleanFalseToken;
}

const std::shared_ptr<JsonToken> JsonToken::nullToken()
{
	static const auto nullToken
		= std::shared_ptr<JsonToken>(new JsonToken(JsonTokenType::LITERAL_NULL, JsonLiterals::NULL_LITERAL));

	return nullToken;
}

const std::shared_ptr<JsonToken> JsonToken::leftBraceToken()
{
	static const auto leftBraceToken
		= std::shared_ptr<JsonToken>(new JsonToken(JsonTokenType::LEFT_BRACE));

	return leftBraceToken;
}

const std::shared_ptr<JsonToken> JsonToken::rightBraceToken()
{
	static const auto rightBraceToken
		= std::shared_ptr<JsonToken>(new JsonToken(JsonTokenType::RIGHT_BRACE));

	return rightBraceToken;
}

const std::shared_ptr<JsonToken> JsonToken::leftSquareBracketToken()
{
	static const auto leftSquareBracketToken
		= std::shared_ptr<JsonToken>(new JsonToken(JsonTokenType::LEFT_SQUARE_BRACKET));

	return leftSquareBracketToken;
}

const std::shared_ptr<JsonToken> JsonToken::rightSquareBracketToken()
{
	static const auto rightSquareBracketToken
		= std::shared_ptr<JsonToken>(new JsonToken(JsonTokenType::RIGHT_SQUARE_BRACKET));

	return rightSquareBracketToken;
}

const std::shared_ptr<JsonToken> JsonToken::commaToken()
{
	static const auto commaToken
		= std::shared_ptr<JsonToken>(new JsonToken(JsonTokenType::COMMA));

	return commaToken;
}

const std::shared_ptr<JsonToken> JsonToken::colonToken()
{
	static const auto colonToken
		= std::shared_ptr<JsonToken>(new JsonToken(JsonTokenType::COLON));

	return colonToken;
}


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