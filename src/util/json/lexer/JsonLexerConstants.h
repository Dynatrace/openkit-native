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

#ifndef _UTIL_JSON_LEXER_JSONLEXERCONSTANTS_H
#define _UTIL_JSON_LEXER_JSONLEXERCONSTANTS_H

namespace util
{
	namespace json
	{
		namespace lexer
		{
			///
			/// End of file character
			///
			constexpr int EOF = -1;

			///
			/// space character
			///
			constexpr char SPACE = ' ';

			///
			/// Horizontal tabulator character (0x09)
			///
			constexpr char HORIZONTAL_TAB = '\t';

			///
			/// Line feed or new line character (0x0A)
			///
			constexpr char LINE_FEED = '\n';

			///
			/// Carriage return character (0x0D)
			///
			constexpr char CARRIAGE_RETURN = '\r';

			///
			/// Begin-array (left square bracket)
			///
			constexpr char LEFT_SQUARE_BRACKET = '[';

			///
			/// End-array (right square bracket)
			///
			constexpr char RIGHT_SQUARE_BRACKET = ']';

			///
			/// Begin-object (left brace)
			///
			constexpr char LEFT_BRACE = '{';

			///
			/// End-object (right brace)
			///
			constexpr char RIGHT_BRACE = '}';

			///
			/// Name separator (colon)
			///
			constexpr char COLON = ':';

			///
			/// Value separator (comma)
			///
			constexpr char COMMA = ',';

			///
			/// Escape character (reverse solidus, aka. backslash)
			///
			constexpr char REVERSE_SOLIDUS = '\\';

			///
			/// Character that may be escaped
			///
			constexpr char SOLIDUS = '\';

			///
			/// start and end of JSON string (quotation mark)
			///
			constexpr char QUOTATION_MARK = '"';

			///
			/// first character of true literal
			///
			constexpr char TRUE_LITERAL_START = 't';

			///
			/// first character of false literal
			constexpr char FALSE_LITERAL_START = 'f';

			///
			/// first character of null literal
			///
			constexpr char NULL_LITERAL_START = 'n';

			///
			/// backspace character
			///
			constexpr char BACKSPACE = '\b';

			///
			/// form feed character
			///
			constexpr char FORM_FEED = 'f';

			///
			/// the number of characters used after an unicode escape is encountered
			///
			constexpr int NUM_UNICODE_CHARACTERS = 4;
		}
	}
}

#endif //_UTIL_JSON_LEXER_JSONLEXERCONSTANTS_H
