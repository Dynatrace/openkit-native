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

#ifndef _UTIL_JSON_LEXER_JSONTOKEN_H
#define _UTIL_JSON_LEXER_JSONTOKEN_H

#include "util/json/lexer/JsonTokenType.h"

#include <string>
#include <memory>

namespace util
{
	namespace json
	{
		namespace lexer
		{
			///
			/// Container class representing a token
			///
			class JsonToken
			{

			public: // members

				///
				/// @ref JsonToken to be used for boolean @c true literal
				///
				static const std::shared_ptr<JsonToken> BOOLEAN_TRUE_TOKEN;

				///
				/// @ref JsonToken to be used for boolean @c false literal
				///
				static const std::shared_ptr<JsonToken> BOOLEAN_FALSE_TOKEN;

				///
				/// @ref JsonToken to be used for null literal
				///
				static const std::shared_ptr<JsonToken> NULL_TOKEN;

				///
				/// @ref JsonToken to be used for left brace
				///
				static const std::shared_ptr<JsonToken> LEFT_BRACE_TOKEN;

				///
				/// @ref JsonToken to be used for right brace
				///
				static const std::shared_ptr<JsonToken> RIGHT_BRACE_TOKEN;

				///
				/// @ref JsonToken to be used for left square bracket
				///
				static const std::shared_ptr<JsonToken> LEFT_SQUARE_BRACKET_TOKEN;

				///
				/// @ref JsonToken to be used for right square bracket
				///
				static const std::shared_ptr<JsonToken> RIGHT_SQUARE_BRACKET_TOKEN;

				///
				/// @ref JsonToken to be used for comma
				///
				static const std::shared_ptr<JsonToken> COMMA_TOKEN;

				///
				/// @ref JsonToken to be used for colon
				///
				static const std::shared_ptr<JsonToken> COLON_TOKEN;

			public: // functions

				///
				/// Create a  @ref JsonToken of type @ref JsonTokenType::VALUE_STRING and the given value.
				/// @param stringValue  the value to be used for the token
				/// @return the created token
				///
				static std::shared_ptr<JsonToken> createStringToken(const std::string& stringValue);

				///
				/// Create a new @ref JsonToken of type @ref JsonTokenType::VALUE_NUMBER and the given value.
				/// @param numericValue the value to be used for the token
				/// @return the newly created token
				///
				static std::shared_ptr<JsonToken> createNumberToken(const std::string& numericValue);

				///
				/// Returns the type of this token
				///
				JsonTokenType getTokenType() const;

				///
				/// Returns the value of this token
				///
				/// @par
				/// this function is only relevant for number, boolean and string tokens
				///
				/// @return the token value as string
				///
				const std::string& getValue() const;

				///
				/// Returns a string representation of this @ref JsonToken
				///
				const std::string toString() const;

				///
				/// Returns a string representation of the given @ref TokenType
				/// @param tokenType the @ref TokenType for which to obtain the string representation
				/// @return a string representation of the given @ref TokenType
				///
				static const std::string tokenTypeToString(const JsonTokenType tokenType);

			private: // members

				///
				/// the type of this token
				///
				const JsonTokenType mTokenType;

				///
				/// token value for primitive tokens
				///
				const std::string mValue;

			private: // functions

				///
				/// Construct the token with the type only and set the value to @c nullptr
				/// @param tokenType type of this token
				///
				JsonToken(const JsonTokenType tokenType);

				///
				/// Construct the token with type and value
				///
				/// @par
				/// Instead of using this constructor use either one of the following
				/// @li the predefined instances
				/// @li @ref ::createStringToken
				///
				/// @param tokenType the type of this token
				/// @param value value of this token, if it has a value.
				///
				JsonToken(const JsonTokenType tokenType, const std::string& value);
			};
		}
	}
}

#endif //_UTIL_JSON_LEXER_JSONTOKEN_H
