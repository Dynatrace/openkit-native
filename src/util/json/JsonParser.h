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

#ifndef _UTIL_JSON_JSONPARSER_H
#define _UTIL_JSON_JSONPARSER_H

#include "util/json/lexer/JsonLexer.h"
#include "util/json/lexer/JsonToken.h"
#include "util/json/objects/JsonValue.h"
#include "util/json/objects/JsonArrayValue.h"
#include "util/json/objects/JsonObjectValue.h"
#include "util/json/parser/JsonParserState.h"

#include <string>
#include <list>
#include <stack>
#include <unordered_map>

namespace util
{
	namespace json
	{
		///
		/// JSON parser class for parsing a JSON input string
		///
		class JsonParser
		{
			using JsonLexerPtr = std::shared_ptr<util::json::lexer::JsonLexer>;
			using JsonTokenPtr = std::shared_ptr<util::json::lexer::JsonToken>;
			using JsonValuePtr = std::shared_ptr<util::json::objects::JsonValue>;
			using JsonArrayValuePtr = std::shared_ptr<util::json::objects::JsonArrayValue>;
			using JsonObjectValuePtr = std::shared_ptr<util::json::objects::JsonObjectValue>;

		public: // functions

			///
			/// Constructor taking the JSON input string
			///
			/// @param input JSON input string
			///
			JsonParser(const std::string& input);

			///
			/// Internal constructor taking the lexical analyzer.
			///
			/// @par
			/// This constructor can be used in tests, when there is the need to mock the lexer
			///
			/// @param lexer lexical analyzer
			///
			JsonParser(const JsonLexerPtr lexer);

			///
			/// Parses the JSON string passed in the constructor and returns the parsed JSON value object.
			///
			/// @return the parsed JSON value object
			///
			/// @throws @ref JsonParserException if there is an error while parsing the input string
			///
			const JsonValuePtr parse();

			///
			/// Function for retrieving the current parser state
			///
			/// @par
			/// This method is only intended to be used in unit tests to properly retrieve the parser's current state
			///
			/// @return returns the current state of the parser
			///
			util::json::parser::JsonParserState getState() const;

		private: // functions

			///
			/// Parses the JSON string passed in the constructor and returns the parsed JSON value object
			///
			/// @return parsed JSON value object.
			///
			/// @throws @ref JsonLexerException if there is an error during lexical analysis of the JSON string
			/// @throws @ref JsonParserException if there is an error while parsing the input string
			///
			const JsonValuePtr doParse();

			///
			/// Parses the given token in the initial state.
			///
			/// @par
			/// This state is the state right after starting parsing the JSON string.
			/// Valid and expected tokens in this state are simple value tokens or start of a compound value.
			///
			/// @param[in] token the token to parse
			///
			/// @throws @ref JsonParserException if there is an error while parsing the token
			///
			void parseInitState(const JsonTokenPtr token);

			///
			/// Parses the given token when an array was started.
			///
			/// @param[in] token the token to parse
			///
			/// @throws @ref JsonParserException if there is an error while parsing the token
			///
			void parseInArrayStartState(const JsonTokenPtr token) ;

			///
			/// Parses the given token in state when an array and a value were previously parsed
			///
			/// @param[in] token the token to parse
			///
			void parseInArrayValueState(const JsonTokenPtr token);

			///
			/// Parses the given token in state when inside an array and a delimiter was previously parsed.
			///
			/// @param[in] token the token to parse
			///
			/// @throws @ref JsonParserException if there is an error while parsing the token
			///
			void parseInArrayDelimiterState(const JsonTokenPtr token);

			///
			/// Utility function to parse the start of a nested object
			///
			/// @par
			/// This function is called if the left brace token is encountered
			///
			void parseStartOfNestedObject();

			///
			/// Utility function to parse the start of a nested array
			///
			/// @par
			/// this function is called if the left square bracket token is encountered
			///
			void parseStartOfNestedArray();

			///
			/// Parses the given token in the state right after a JSON object was started.
			///
			/// @param[in] token the token to parse
			///
			/// @throws @ref JsonParserException if there is an error while parsing the token
			///
			void parseInObjectStartState(const JsonTokenPtr token);

			///
			/// Parses a token in the state right after a JSON key token was parsed.
			///
			/// @param[in] token the token to parse
			///
			/// @throws @ref JsonParserException if there is an error while parsing the token
			///
			void parseInObjectKeyState(const JsonTokenPtr token);

			///
			/// Parses the given token in the state right after a JSON key-value delimiter(":") was parsed.
			///
			/// @param[in] token the token to parse
			///
			/// @throws @ref JsonParserException if there is an error while parsing the token
			///
			void parseInObjectColonState(const JsonTokenPtr token);

			///
			/// Parses the given token in the state right after a JSON object value was parsed.
			///
			/// @par
			/// Note for now: an object can have more than one key-value pair, but the value must be a simple type.
			///
			/// @param[in] token the token to parse
			///
			/// @throws @ref JsonParserException if there is an error while parsing the token
			///
			void parseInObjectValueState(const JsonTokenPtr token);

			///
			/// Parses the given token in the state right after a delimiter was parsed.
			///
			/// @par
			/// Note for now: not supported yet.
			///
			/// @param[in] token the token to parse
			///
			/// @throws @ref JsonParserException if there is an error while parsing the token
			///
			void parseInObjectDelimiterState(const JsonTokenPtr token);

			///
			/// Parses the token when already encountered the end state
			///
			/// @param[in] token the token to parse
			///
			/// @throws @ref JsonParserException if there is an error while parsing the token
			///
			void parseEndState(const JsonTokenPtr token);

			///
			/// Helper function to remove the top level JSON value / state from the value / state stack.
			///
			/// @throws @ref JsonParserException in case of an inconsistent state of the respective stacks
			///
			void closeCompositeJsonValueAndRestoreState();

			///
			/// Puts the last parsed key-value pair as top level onto the value stack
			///
			/// @par
			/// some sanity checks are performed to ensure consistency
			///
			/// @throws @ref JsonParserException in case the stack is inconsistent or the parser is in an inconsistent state
			///
			void putLastParsedKeyValuePairIntoObject();

			///
			/// Helper function for converting a JSON token to a JSON value
			///
			/// @par
			/// Only simple JSON values are supported
			///
			/// @param[in] token the token to convert to a JSON value
			///
			/// @return the converted JSON value
			///
			/// @throws @ref JsonParserException in case the given token is not a simple JSON value
			///
			const JsonValuePtr tokenToSimpleJsonValue(const JsonTokenPtr token) const;

			///
			/// Ensures that the given @c token is not @c nullptr
			///
			/// @par
			/// If the given @c token is @c nullptr a @ref JsonParserException is thrown.
			///
			/// @param[in] token the token to be checked for null
			/// @param[in] exceptionMessage the message passed to the @ref JsonParserException
			///
			/// @throws @ref JsonParserException if the given token is @c nullptr
			///
			void ensureTokenIsNotNull(const JsonTokenPtr token, const char* exceptionMessage);

			///
			/// Ensures that the value container stack's top element is OK so that a JSON array can be parsed.
			///
			/// @throws @ref JsonParserException in case the stack is inconsistent
			///
			void ensureTopLevelElementIsAJsonArray() const;

			///
			/// Ensures that the value container stack's top element is OK so that a JSON object can be parsed.
			///
			/// @throws @ref JsonParserException in case the stack is inconsistent
			///
			void ensureTopLevelElementIsAJsonObject() const;

			///
			/// Ensures that a key-value pair was previously parsed.
			///
			/// @throws @ref JsonParserException if no key-value pair was parsed before.
			///
			void ensureKeyValuePairWasParsed() const;

			///
			/// Helper function to ensure that the value container stack is not empty
			///
			/// @throws @ref JsonParserException the value stack is empty
			///
			void ensureValueContainerStackIsNotEmpty() const;

			///
			/// Helper function for creating the internal parser error text
			///
			/// @param[in] state the current parser state
			/// @param[in] suffix the suffix to append to the message;
			///
			static const std::string internalParserErrorMessage(const util::json::parser::JsonParserState state, const char* suffix);

			///
			/// Helper method for creating the unexpected token error text
			///
			/// @param[in] token the unexpected token
			/// @param[in] suffix the suffix to append to the message
			///
			static const std::string unexpectedTokenErrorMessage(const JsonTokenPtr token, const char* suffix);

		private: // structures

			struct JsonValueContainer
			{
				///
				/// the JSON value to store
				///
				const JsonValuePtr mJsonValue;

				///
				/// backing list which is non-null if and only if @c mJsonValue is a @ref JsonArrayValue
				///
				const std::shared_ptr<std::list<JsonValuePtr>> mBackingList;

				///
				/// backing map which is non-null if and only if @c mJsonValue is a @ref JsonObjectValue
				///
				const std::shared_ptr<std::unordered_map<std::string, JsonValuePtr>> mBackingMap;

				///
				/// refers to the last parsed key of an object
				///
				std::string mLastParsedObjectKey = std::string();

				///
				/// refers to the last parsed value of an object
				///
				JsonValuePtr mLastParsedObjectValue = nullptr;

				///
				/// Constructs a @ref JsonValueContainer with the given JSON value
				///
				/// @par
				/// any backing container classes are set to @c nullptr, therefore use this constructor only with simple
				/// values.
				/// @param[in] jsonValue a simple @ref JsonValue to initialize this container with
				JsonValueContainer(const JsonValuePtr jsonValue);

				///
				/// Constructs a @ref JsonValueContainer with a JSON array value
				///
				/// @param[in] jsonArrayValue the JSON array value
				/// @param[in] backingList the backing list for the JSON array value
				///
				JsonValueContainer(const JsonArrayValuePtr jsonArrayValue, const std::shared_ptr<std::list<JsonValuePtr>> backingList);

				///
				/// Construct a @ref JsonValueContainer with a JSON array value
				///
				/// @param jsonObjecValue the JSON aray value.
				/// @param backingMap the backing list for the JSON array value.
				///
				JsonValueContainer(const JsonObjectValuePtr jsonObjecValue, const std::shared_ptr<std::unordered_map<std::string, JsonValuePtr>> backingMap);
			};

		private: // members
			using JsonValueContainerPtr = std::shared_ptr<JsonValueContainer>;

			///
			/// error message used for exception when a JSON array is not terminated
			///
			static constexpr const char* UNTERMINATED_JSON_ARRAY_ERROR = "Unterminated JSON array";

			///
			/// error message used for exception when a JSON object is not terminated
			///
			static constexpr const char* UNTERMINATED_JSON_OBJECT_ERROR = "Unterminated JSON object";

			///
			/// lexical analyzer
			///
			const JsonLexerPtr mLexer;

			///
			/// current parser state
			///
			util::json::parser::JsonParserState mState = util::json::parser::JsonParserState::INIT;

			///
			/// the parsed JSON value object
			///
			JsonValuePtr mParsedValue = nullptr;

			///
			/// stack storing the parser state. required to parse nested objects
			///
			std::stack<util::json::parser::JsonParserState> mStateStack = std::stack<util::json::parser::JsonParserState>();

			///
			/// stack storing the JSON values. required for nested values.
			///
			std::stack<JsonValueContainerPtr> mValueStack = std::stack<JsonValueContainerPtr>();
		};
	}
}

#endif //_UTIL_JSON_JSONPARSER_H
