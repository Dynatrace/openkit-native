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

#ifndef _UTIL_JSON_CONSTANTS_JSONLITERALS_H
#define _UTIL_JSON_CONSTANTS_JSONLITERALS_H

#include <string>
#include <regex>

namespace util
{
	namespace json
	{
		namespace constants
		{
			class JsonLiterals
			{
			public:

				///
				/// boolean true literal
				///
				static constexpr const char* BOOLEAN_TRUE_LITERAL = "true";

				///
				/// boolean false literal
				///
				static constexpr const char* BOOLEAN_FALSE_LITERAL = "false";

				///
				/// null literal
				///
				static constexpr const char* NULL_LITERAL = "null";

				///
				/// regex pattern for parsing number literals
				///
				static constexpr const char* NUMBER_PATTERN_STRING = "^-?(0|[1-9]\\d*)(\\.\\d+)?([eE][+-]?\\d+)?$";

				///
				/// Get sole instance of JSON number pattern regex.
				///
				static const std::regex& getNumberPattern();
			};
		}
	}
}

#endif //_UTIL_JSON_CONSTANTS_JSONLITERALS_H
