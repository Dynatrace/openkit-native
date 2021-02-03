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

#ifndef _UTIL_JSON_LEXER_JSONLEXER_H
#define _UTIL_JSON_LEXER_JSONLEXER_H

#include "util/json/lexer/JsonLexerState.h"
#include "util/json/lexer/JsonToken.h"
#include "util/json/lexer/JsonLexerException.h"
#include "util/json/reader/IResettableReader.h"

#include <string>
#include <sstream>
#include <memory>

namespace util
{
	namespace json
	{
		namespace lexer
		{
			class JsonLexer
			{
				using ReaderPtr = std::shared_ptr<util::json::reader::IResettableReader>;
				using JsonTokenPtr = std::shared_ptr<JsonToken>;

			public: // functions

				///
				/// Destructor
				///
				virtual ~JsonLexer();

				///
				/// Constructor taking the JSON string
				///
				/// @param input JSON string for lexical analysis
				///
				JsonLexer(const std::string& input);

				///
				/// Constructor taking a @ref IResettableReader from where to read the JSON data
				///
				/// @param input a reader @ref IResettableReader "reader" instance from where to read the JSON data
				///
				JsonLexer(const ReaderPtr input);

				///
				/// Returns the next token, or @c nullptr if no next token is available.
				///
				/// @return the next @ref JsonToken or @c nullptr if there is no such token.
				///
				virtual const JsonTokenPtr nextToken();

			private: // members

				///
				/// current state of the lexer
				///
				JsonLexerState mLexerState = JsonLexerState::INITIAL;

				///
				/// the reader from where to read the JSON input
				///
				ReaderPtr mReader;

			private: // functions

				///
				/// Parses a token after all whitespace characters were consumed.
				///
				/// @par
				///  It must be guaranteed that the first character is
				///  @li a non-whitespace character
				///  @li not EOF (integer value -1)
				///
				/// @return the next token
				///
				const JsonTokenPtr doParseNextToken();

				///
				/// Tries to parse a boolean literal, which is either true or false with all lower case characters.
				///
				/// @par
				///   The definition of a boolean literal follows the specs from https://tools.ietf.org/html/rfc8259#section-3
				///
				/// @return the parsed @ref JsonToken
				///
				const JsonTokenPtr tryParseBooleanLiteral();

				///
				/// Try to parse a null literal
				///
				/// @par the definition of a null literal follows the spec from https://tools.ietf.org/html/rfc8259#section-3
				///
				/// @return the parsed @ref JsonToken
				///
				const JsonTokenPtr tryParseNullLiteral();

				///
				/// Try to parse a JSON string token
				///
				/// @return the parsed string token
				///
				const JsonTokenPtr tryParseStringToken();

				///
				/// Tries to parse an escape sequence
				///
				void tryParseEscapeSequence(std::stringstream& literalString);

				///
				/// Tries to parse a unicode escape sequence.
				///
				void tryParseUnicodeEscapeSequence(std::stringstream& literalString);

				///
				/// Tries to parse a low surrogate UTF-16 character
				///
				/// @param[out] isSuccess indicates if the parsed character is a low surrogate character
				/// @return the low surrogate character or @c nullptr if the first two characters are not the start of
				///   a unicode escape sequence.
				///
				int32_t tryParseLowSurrogateChar(bool& isSuccess);

				///
				/// Reads a unicode escape sequence and returns the read data
				///
				/// @par
				///   A unicode escape sequence is a 4 character long sequence where each of those 4 characters is a
				///   hex (base-16) character. The characters a-f may be lower, upper or mixed case.
				///
				std::string readUnicodeEscapeSequence();

				///
				/// Tries to parse a number token.
				///
				/// @return the parsed number token
				///
				JsonTokenPtr tryParseNumberToken();

				///
				/// Parses a literal
				///
				/// @par
				///  A literal is considered to be terminated by eiter
				///  @li one of the four whitespace characters
				///  @li one of the six structural characters
				///
				/// @return the parsed literal string
				///
				const std::string parseLiteral();

				///
				/// Consumes all whitespace characters form the @ref IResettableReader until the first non-whitespace is
				/// encountered.
				///
				/// @return @c true if EOF (end of file) is reached, @c false otherwise.
				///
				bool consumeWhitespaceCharacters();

				///
				/// Tests if the given character is a JSON whitespace character according to RFC 8259
				///
				/// @param chr the character to test whether it is a whitespace character or not
				///
				/// @return @c true if it is a structural character, @c false otherwise.
				///
				static bool isJsonWhitespaceCharacter(int32_t chr);

				///
				/// Tests if the given character is a JSON structural character according to RFC 8259
				///
				/// @param chr the character to test whether it is a structural character or not.
				///
				/// @ return @c true if it is a structural character, @c false otherwise
				///
				static bool isJsonStructuralCharacter(int32_t chr);

				///
				/// Tests if the given character is a digit or a minus character
				///
				/// @param chr the character to be tested if it is a a minos or digit character
				///
				/// @return @c true if it is a digit or a minus character, @c false otherwise
				///
				static bool isDigitOrMinus(int32_t chr);

				///
				/// Tests if the given character is the escape character to escape other characters in a JSON string
				///
				/// @param chr the character to test
				///
				/// @return @c true if it is the escape character, @c false otherwise
				///
				static bool isEscapeCharacter(int32_t chr);

				///
				/// Tests if the given character needs escaping (according to https://tools.ietf.org/html/rfc8259#section-7)
				///
				/// @par
				/// Characters that need to be escaped according to RFC:
				/// @li quotation mark
				/// @li reverse solidus
				/// @li control characters [U+0000, U+001F]
				///
				/// @param chr the character to test
				/// @return @c true if the character needs to be escaped in a JSON string, @c false otherwise.
				///
				static bool isCharacterThatNeedsEscaping(int32_t chr);

				///
				/// Helper method to test if the given character is a hex character
				///
				/// @par
				/// A character is considered to be a hex character, if
				/// @li is in range 0-9
				/// @li is in range a-f
				/// @li is in range A-F
				///
				/// @param chr the character to test
				/// @return @c true if the character is a hex character, @c false otherwise.
				///
				static bool isHexCharacter(int32_t chr);

				///
				/// Helper method to return the "unexpected literal" exception message
				///
				/// @par
				/// This method is a helper to avoid code duplication
				///
				/// @param literalToken the unexpected literal token
				/// @return an error message for an exception
				///
				static std::string unexpectedLiteralTokenMessage(std::string& literalToken);
			};
		}
	}
}

#endif //_UTIL_JSON_LEXER_JSONLEXER_H
