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

#ifndef _UTIL_JSON_LEXER_JSONTOKENTYPE_H
#define _UTIL_JSON_LEXER_JSONTOKENTYPE_H


namespace util
{
	namespace json
	{
		namespace lexer
		{
			///
			/// Specifies the type of a @ref util::json::lexer::JsonToken "JSON token"
			///
			enum class JsonTokenType
			{
				///
				/// Numeric value, the value is the number text.
				///
				VALUE_NUMBER,

				///
				/// String value, the value does not include leading and trailing " characters
				///
				VALUE_STRING,

				///
				/// Boolean literal, the value is either true or false
				///
				LITERAL_BOOLEAN,

				///
				/// null literal, value will be "null"
				///
				LITERAL_NULL,

				///
				/// {
				///
				LEFT_BRACE,

				///
				/// }
				///
				RIGHT_BRACE,

				///
				/// [
				///
				LEFT_SQUARE_BRACKET,

				///
				/// ]
				///
				RIGHT_SQUARE_BRACKET,

				///
				/// ,
				///
				COMMA,

				///
				/// :
				///
				COLON
			};
		}
	}
}

#endif //_UTIL_JSON_LEXER_JSONTOKENTYPE_H
