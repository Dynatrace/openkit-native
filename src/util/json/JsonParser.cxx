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

#include "util/json/JsonParser.h"
#include "util/json/parser/JsonParserException.h"
#include "util/json/lexer/JsonLexerException.h"
#include "util/json/lexer/JsonTokenType.h"
#include "util/json/objects/JsonArrayValue.h"
#include "util/json/objects/JsonObjectValue.h"
#include "util/json/objects/JsonNullValue.h"
#include "util/json/objects/JsonBooleanValue.h"
#include "util/json/objects/JsonStringValue.h"
#include "util/json/objects/JsonNumberValue.h"

#include <sstream>

using namespace util::json;
using namespace util::json::parser;

using JsonTokenType = util::json::lexer::JsonTokenType;


JsonParser::JsonParser(const std::string& input)
	: JsonParser(std::make_shared<util::json::lexer::JsonLexer>(input))
{
}

JsonParser::JsonParser(const util::json::JsonParser::JsonLexerPtr lexer)
	: mLexer(lexer)
{
}

const JsonParser::JsonValuePtr JsonParser::parse()
{
	if (mState == JsonParserState::END)
	{
		// return the already parsed object if parse was called before
		return mParsedValue;
	}

	if (mState == JsonParserState::ERROR)
	{
		throw JsonParserException("JSON parser is in erroneous state");
	}

	try
	{
		mParsedValue = doParse();
	}
	catch (util::json::lexer::JsonLexerException&)
	{
		mState = JsonParserState::ERROR;
		std::throw_with_nested(JsonParserException("Caught exception from lexical analysis"));
	}

	return mParsedValue;
}

JsonParserState JsonParser::getState() const
{
	return mState;
}

const JsonParser::JsonValuePtr JsonParser::doParse()
{
	std::shared_ptr<util::json::lexer::JsonToken> token;

	do
	{
		token = mLexer->nextToken();
		switch (mState)
		{
			case JsonParserState::INIT:
				parseInitState(token);
				break;
			case JsonParserState::IN_ARRAY_START:
				parseInArrayStartState(token);
				break;
			case JsonParserState::IN_ARRAY_VALUE:
				parseInArrayValueState(token);
				break;
			case JsonParserState::IN_ARRAY_DELIMITER:
				parseInArrayDelimiterState(token);
				break;
			case JsonParserState::IN_OBJECT_START:
				parseInObjectStartState(token);
				break;
			case JsonParserState::IN_OBJECT_KEY:
				parseInObjectKeyState(token);
				break;
			case JsonParserState::IN_OBJECT_COLON:
				parseInObjectColonState(token);
				break;
			case JsonParserState::IN_OBJECT_VALUE:
				parseInObjectValueState(token);
				break;
			case JsonParserState::IN_OBJECT_DELIMITER:
				parseInObjectDelimiterState(token);
				break;
			case JsonParserState::END:
				parseEndState(token);
				break;
			case JsonParserState::ERROR:
				// this should never be reached since whenever there is a transition into the error state an exception
				// is thrown right afterwards. this is just a precaution
				throw JsonParserException(unexpectedTokenErrorMessage(token, "in error state"));
			default:
				// precaution in case a new state got added
				throw JsonParserException(internalParserErrorMessage(mState, "Unexpected JsonParserState"));
		}
	}
	while (token != nullptr);

	ensureValueContainerStackIsNotEmpty();

	return mValueStack.top()->mJsonValue;
}

void JsonParser::parseInitState(const JsonTokenPtr token)
{
	ensureTokenIsNotNull(token, "No JSON object could be decoded");

	// parse the token
	switch (token->getTokenType())
	{
		case JsonTokenType::LITERAL_NULL:		// fallthrough
		case JsonTokenType::LITERAL_BOOLEAN:	// fallthrough
		case JsonTokenType::VALUE_STRING:		// fallthrough
		case JsonTokenType::VALUE_NUMBER:
		{
			mValueStack.push(std::shared_ptr<JsonValueContainer>(new JsonValueContainer(tokenToSimpleJsonValue(token))));

			mState = JsonParserState::END;
			break;
		}
		case JsonTokenType::LEFT_SQUARE_BRACKET:
		{
			auto jsonValueList = std::make_shared<std::list<JsonValuePtr>>();
			auto jsonArrayValue = objects::JsonArrayValue::fromList(jsonValueList);
			mValueStack.push(std::shared_ptr<JsonValueContainer>(new JsonValueContainer(jsonArrayValue, jsonValueList)));

			mState = JsonParserState::IN_ARRAY_START;
			break;
		}
		case JsonTokenType::LEFT_BRACE:
		{
			auto jsonObjectMap = std::make_shared<std::unordered_map<std::string, JsonValuePtr>>();
			auto jsonObjectValue = objects::JsonObjectValue::fromMap(jsonObjectMap);
			mValueStack.push(std::shared_ptr<JsonValueContainer>(new JsonValueContainer(jsonObjectValue, jsonObjectMap)));

			mState = JsonParserState::IN_OBJECT_START;
			break;
		}
		default:
		{
			mState = JsonParserState::ERROR;
			throw JsonParserException(unexpectedTokenErrorMessage(token, "at start of input"));
		}
	}
}

void JsonParser::parseInArrayStartState(const JsonTokenPtr token)
{
	ensureTokenIsNotNull(token, UNTERMINATED_JSON_ARRAY_ERROR);

	switch (token->getTokenType())
	{
		case JsonTokenType::LITERAL_NULL:		// fallthrough
		case JsonTokenType::LITERAL_BOOLEAN:	// fallthrough
		case JsonTokenType::VALUE_STRING:		// fallthrough
		case JsonTokenType::VALUE_NUMBER:
			ensureTopLevelElementIsAJsonArray();
			mValueStack.top()->mBackingList->push_back(tokenToSimpleJsonValue(token));

			mState = JsonParserState::IN_ARRAY_VALUE;
			break;
		case JsonTokenType::LEFT_SQUARE_BRACKET:
			// start nested array as first element in array
			parseStartOfNestedArray();
			break;
		case JsonTokenType::LEFT_BRACE:
			// start nested object as first element in array
			parseStartOfNestedObject();
			break;
		case JsonTokenType::RIGHT_SQUARE_BRACKET:
			closeCompositeJsonValueAndRestoreState();
			break;
		default:
			mState = JsonParserState::ERROR;
			throw JsonParserException(unexpectedTokenErrorMessage(token, "at beginning of array"));
	}
}

void JsonParser::parseInArrayValueState(const JsonTokenPtr token)
{
	ensureTokenIsNotNull(token, UNTERMINATED_JSON_ARRAY_ERROR);

	switch (token->getTokenType())
	{
		case JsonTokenType::COMMA:
			mState = JsonParserState::IN_ARRAY_DELIMITER;
			break;
		case JsonTokenType::RIGHT_SQUARE_BRACKET:
			closeCompositeJsonValueAndRestoreState();
			break;
		default:
			mState = JsonParserState::ERROR;
			throw JsonParserException(unexpectedTokenErrorMessage(token, "in array after value was parsed"));
	}
}

void JsonParser::parseInArrayDelimiterState(const JsonTokenPtr token)
{
	ensureTokenIsNotNull(token,UNTERMINATED_JSON_ARRAY_ERROR);

	switch (token->getTokenType())
	{
		case JsonTokenType::LITERAL_NULL:		// fallthrough
		case JsonTokenType::LITERAL_BOOLEAN:	// fallthrough
		case JsonTokenType::VALUE_STRING:		// fallthrough
		case JsonTokenType::VALUE_NUMBER:
			ensureTopLevelElementIsAJsonArray();
			mValueStack.top()->mBackingList->push_back(tokenToSimpleJsonValue(token));

			mState = JsonParserState::IN_ARRAY_VALUE;
			break;
		case JsonTokenType::LEFT_SQUARE_BRACKET:
			// start nested array as element in array
			parseStartOfNestedArray();
			break;
		case JsonTokenType::LEFT_BRACE:
			// start nested object as first element in array
			parseStartOfNestedObject();
			break;
		default:
			mState = JsonParserState::ERROR;
			throw JsonParserException(unexpectedTokenErrorMessage(token, "in array after delimiter"));
	}
}

void JsonParser::parseStartOfNestedObject()
{
	mStateStack.push(JsonParserState::IN_ARRAY_VALUE);
	auto jsonObjectMap = std::make_shared<std::unordered_map<std::string, JsonValuePtr>>();
	auto jsonObjectValue = objects::JsonObjectValue::fromMap(jsonObjectMap);
	mValueStack.push(std::shared_ptr<JsonValueContainer>(new JsonValueContainer(jsonObjectValue, jsonObjectMap)));

	mState = JsonParserState::IN_OBJECT_START;
}

void JsonParser::parseStartOfNestedArray()
{
	mStateStack.push(JsonParserState::IN_ARRAY_VALUE);
	auto jsonValueList = std::make_shared<std::list<JsonValuePtr>>();
	auto jsonArrayValue = objects::JsonArrayValue::fromList(jsonValueList);
	mValueStack.push(std::shared_ptr<JsonValueContainer>(new JsonValueContainer(jsonArrayValue, jsonValueList)));

	mState = JsonParserState::IN_ARRAY_START;
}

void JsonParser::parseInObjectStartState(const JsonTokenPtr token)
{
	ensureTokenIsNotNull(token, UNTERMINATED_JSON_OBJECT_ERROR);
	ensureTopLevelElementIsAJsonObject();

	switch (token->getTokenType())
	{
		case JsonTokenType::RIGHT_BRACE:
			// object is closed right after it was started
			closeCompositeJsonValueAndRestoreState();
			break;
		case JsonTokenType::VALUE_STRING:
			mValueStack.top()->mLastParsedObjectKey = token->getValue();
			mState = JsonParserState::IN_OBJECT_KEY;
			break;
		default:
			mState = JsonParserState::ERROR;
			throw JsonParserException(unexpectedTokenErrorMessage(token, "encountered - object key expected"));
	}
}

void JsonParser::parseInObjectKeyState(const JsonTokenPtr token)
{
	ensureTokenIsNotNull(token,UNTERMINATED_JSON_OBJECT_ERROR);

	if (token->getTokenType() == JsonTokenType::COLON)
	{
		// got key-value delimiter as expected
		mState = JsonParserState::IN_OBJECT_COLON;
	}
	else
	{
		// expected key-value delimiter (":") but got something different instead
		mState = JsonParserState::ERROR;
		throw JsonParserException(unexpectedTokenErrorMessage(token,"encountered - key-value delimiter expected"));
	}
}

void JsonParser::parseInObjectColonState(const JsonTokenPtr token)
{
	ensureTokenIsNotNull(token, UNTERMINATED_JSON_OBJECT_ERROR);
	ensureTopLevelElementIsAJsonObject();

	switch (token->getTokenType())
	{
		case JsonTokenType::LITERAL_NULL:		// fallthrough
		case JsonTokenType::LITERAL_BOOLEAN:	// fallthrough
		case JsonTokenType::VALUE_STRING:		// fallthrough
		case JsonTokenType::VALUE_NUMBER:
		{
			// simple JSON value as object value
			mValueStack.top()->mLastParsedObjectValue = tokenToSimpleJsonValue(token);

			mState = JsonParserState::IN_OBJECT_VALUE;
			break;
		}
		case JsonTokenType::LEFT_BRACE:
		{
			// value is an object
			auto jsonObjectMap = std::make_shared<std::unordered_map<std::string, JsonValuePtr>>();
			auto jsonObjectValue = objects::JsonObjectValue::fromMap(jsonObjectMap);
			mValueStack.push(std::shared_ptr<JsonValueContainer>(new JsonValueContainer(jsonObjectValue, jsonObjectMap)));
			mStateStack.push(JsonParserState::IN_OBJECT_VALUE);

			mState = JsonParserState::IN_OBJECT_START;
			break;
		}
		case JsonTokenType::LEFT_SQUARE_BRACKET:
		{
			// value is an array
			auto jsonValueList = std::make_shared<std::list<JsonValuePtr>>();
			auto jsonArrayValue = objects::JsonArrayValue::fromList(jsonValueList);
			mValueStack.push(std::shared_ptr<JsonValueContainer>(new JsonValueContainer(jsonArrayValue, jsonValueList)));
			mStateStack.push(JsonParserState::IN_OBJECT_VALUE);

			mState = JsonParserState::IN_ARRAY_START;
			break;
		}
		default:
		{
			// any other token
			throw JsonParserException(unexpectedTokenErrorMessage(token,"after key-value pair encountered"));
		}
	}
}

void JsonParser::parseInObjectValueState(const JsonTokenPtr token)
{
	ensureTokenIsNotNull(token, UNTERMINATED_JSON_OBJECT_ERROR);
	ensureTopLevelElementIsAJsonObject();

	switch (token->getTokenType())
	{
		case JsonTokenType::RIGHT_BRACE:
		{
			// object is closed right after some value. push last parsed key/value into map
			ensureKeyValuePairWasParsed();

			auto firstElement = mValueStack.top();
			auto key = firstElement->mLastParsedObjectKey;
			auto value = firstElement->mLastParsedObjectValue;
			auto backingMap = firstElement->mBackingMap;
			(*backingMap)[key] = value;

			closeCompositeJsonValueAndRestoreState();
			break;
		}
		case JsonTokenType::COMMA:
		{
			// expecting more entries in the current object. push existing and make state transition
			putLastParsedKeyValuePairIntoObject();

			mState = JsonParserState::IN_OBJECT_DELIMITER;
			break;
		}
		default:
		{
			// any other token
			mState = JsonParserState::ERROR;
			throw JsonParserException(unexpectedTokenErrorMessage(token, "after key-value pair encountered"));
		}
	}
}

void JsonParser::parseInObjectDelimiterState(JsonTokenPtr token)
{
	ensureTokenIsNotNull(token, UNTERMINATED_JSON_OBJECT_ERROR);
	ensureTopLevelElementIsAJsonObject();

	if (token->getTokenType() == JsonTokenType::VALUE_STRING)
	{
		mValueStack.top()->mLastParsedObjectKey = token->getValue();
		mState = JsonParserState::IN_OBJECT_KEY;
	}
	else
	{
		mState = JsonParserState::ERROR;
		throw JsonParserException(unexpectedTokenErrorMessage(token, "encountered - object key expected"));
	}
}

void JsonParser::parseEndState(JsonTokenPtr token)
{
	if (!token)
	{
		// end of input. regular terminal state
		return;
	}

	mState = JsonParserState::ERROR;
	throw JsonParserException(unexpectedTokenErrorMessage(token, "at end of input"));
}

void JsonParser::closeCompositeJsonValueAndRestoreState()
{
	ensureValueContainerStackIsNotEmpty();

	if (mValueStack.size() != mStateStack.size() +1)
	{
		// sanity check. cannot happen unless there is a programming error
		throw JsonParserException(internalParserErrorMessage(mState, "mValueStack and mStateStack sizes mismatch"));
	}
	if(mValueStack.size() == 1)
	{
		// the outermost array is terminated. do not remove anything from the stack
		mState = JsonParserState::END;
		return;
	}

	auto firstValueStackElement = mValueStack.top();
	mValueStack.pop();

	auto currentValue = firstValueStackElement->mJsonValue;

	// ensure that there is a new top level element which is a composite value (object or array)
	ensureValueContainerStackIsNotEmpty();
	firstValueStackElement = mValueStack.top();
	if (firstValueStackElement->mJsonValue->getValueType() == objects::JsonValueType::ARRAY_VALUE)
	{
		if (!firstValueStackElement->mBackingList)
		{
			// precaution to ensure we did not do something wrong
			throw JsonParserException(internalParserErrorMessage(mState, "backing list is null"));
		}
		firstValueStackElement->mBackingList->push_back(currentValue);
	}
	else if (firstValueStackElement->mJsonValue->getValueType() == objects::JsonValueType::OBJECT_VALUE)
	{
		firstValueStackElement->mLastParsedObjectValue = currentValue;
	}
	else
	{
		// unexpected top level object - this should not happen unless there is a programming error
		throw JsonParserException(internalParserErrorMessage(mState, "not a composite top level object"));
	}

	mState = mStateStack.top();
	mStateStack.pop();
}

void JsonParser::putLastParsedKeyValuePairIntoObject()
{
	ensureKeyValuePairWasParsed();
	ensureTopLevelElementIsAJsonObject();

	auto firstValueStackElement = mValueStack.top();
	auto key = firstValueStackElement->mLastParsedObjectKey;
	auto value = firstValueStackElement->mLastParsedObjectValue;
	auto backingMap = firstValueStackElement->mBackingMap;
	(*backingMap)[key] = value;

	firstValueStackElement->mLastParsedObjectKey = std::string();
	firstValueStackElement->mLastParsedObjectValue = nullptr;
}

const JsonParser::JsonValuePtr JsonParser::tokenToSimpleJsonValue(const JsonTokenPtr token) const
{
	switch (token->getTokenType())
	{
		case JsonTokenType::LITERAL_NULL:
			return objects::JsonNullValue::nullValue();
		case JsonTokenType::LITERAL_BOOLEAN:
			return objects::JsonBooleanValue::fromLiteral(token->getValue());
		case JsonTokenType::VALUE_STRING:
			return objects::JsonStringValue::fromString(token->getValue());
		case JsonTokenType::VALUE_NUMBER:
			return objects::JsonNumberValue::fromNumberLiteral((token->getValue()));
		default:
			throw JsonParserException("Internal parser error: Unexpected JSON token \"" + token->toString() + "\"");
	}
}

void JsonParser::ensureTokenIsNotNull(const JsonTokenPtr token, const char* exceptionMessage)
{
	if (!token)
	{
		mState = JsonParserState::ERROR;
		throw JsonParserException(exceptionMessage);
	}
}

void JsonParser::ensureTopLevelElementIsAJsonArray() const
{
	ensureValueContainerStackIsNotEmpty();

	auto firstStackElement = mValueStack.top();
	if (firstStackElement->mJsonValue->getValueType() != objects::JsonValueType::ARRAY_VALUE)
	{
		// sanity check. cannot happen unless there is a programming error
		throw JsonParserException(internalParserErrorMessage(mState, "top level element is not a JSON array"));
	}
	if (!firstStackElement->mBackingList)
	{
		// sanity check. cannot happen unless there is a programming error
		throw JsonParserException(internalParserErrorMessage(mState, "backing list is null"));
	}
}

void JsonParser::ensureTopLevelElementIsAJsonObject() const
{
	ensureValueContainerStackIsNotEmpty();

	auto firstStackElement = mValueStack.top();
	if (firstStackElement->mJsonValue->getValueType() != objects::JsonValueType::OBJECT_VALUE)
	{
		// sanity check. cannot happen unless there is a programming error
		throw JsonParserException(internalParserErrorMessage(mState, "top level element is not a JSON object"));
	}
	if (!firstStackElement->mBackingMap)
	{
		// sanity check. cannot happen unless there is a programming error
		throw JsonParserException(internalParserErrorMessage(mState, "backing map is null"));
	}
}

void JsonParser::ensureKeyValuePairWasParsed() const
{
	ensureValueContainerStackIsNotEmpty();

	auto firstStackElement = mValueStack.top();
	if (firstStackElement->mLastParsedObjectKey.empty())
	{
		throw JsonParserException(internalParserErrorMessage(mState, "mLastParsedObjectKey is not set"));
	}
	if (!firstStackElement->mLastParsedObjectValue)
	{
		throw JsonParserException(internalParserErrorMessage(mState, "mLastParsedObjectValue is null"));
	}
}

void JsonParser::ensureValueContainerStackIsNotEmpty() const
{
	if (mValueStack.empty())
	{
		// sanity check. cannot happen unless there is a programming error
		throw JsonParserException(internalParserErrorMessage(mState, "mValueStack is empty"));
	}
}

const std::string JsonParser::internalParserErrorMessage(const JsonParserState state, const char* suffix)
{
	std:: stringstream stream;
	stream << "Internal parser error: [state=\""
		<< static_cast<std::underlying_type<util::json::parser::JsonParserState>::type>(state) <<  "\"] " << suffix;
	return stream.str();
}

const std::string JsonParser::unexpectedTokenErrorMessage(const JsonTokenPtr token, const char* suffix)
{
	return "Unexpected token \"" + token->toString() + "\" " + suffix;
}

//
// nested class JsonValueContainer -------------------------------------------------------------------------------------
//

JsonParser::JsonValueContainer::JsonValueContainer(JsonValuePtr jsonValue)
	: mJsonValue(jsonValue)
	, mBackingList(nullptr)
	, mBackingMap(nullptr)
{
}

JsonParser::JsonValueContainer::JsonValueContainer(JsonArrayValuePtr jsonArrayValue,
	std::shared_ptr<std::list<JsonValuePtr>> backingList)
	: mJsonValue(jsonArrayValue)
	, mBackingList(backingList)
	, mBackingMap(nullptr)
{
}

JsonParser::JsonValueContainer::JsonValueContainer(JsonObjectValuePtr jsonObjecValue,
	std::shared_ptr<std::unordered_map<std::string, JsonValuePtr>> backingMap)
	: mJsonValue(jsonObjecValue)
	, mBackingList(nullptr)
	, mBackingMap(backingMap)
{
}