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

#ifndef _UTIL_JSON_PARSER_JSONPARSERSTATE_H
#define _UTIL_JSON_PARSER_JSONPARSERSTATE_H

namespace util
{
	namespace json
	{
		namespace parser
		{
			enum class JsonParserState
			{
				///
				/// initial state of the JSON parser
				///
				INIT,

				///
				/// state when start of an array was encountered
				///
				IN_ARRAY_START,

				///
				/// state in array after a value was parsed
				///
				IN_ARRAY_VALUE,

				///
				/// state in array after a delimiter was parsed
				///
				IN_ARRAY_DELIMITER,

				///
				/// state when start of an object was encountered
				///
				IN_OBJECT_START,

				///
				/// state in object after a key was parsed
				///
				IN_OBJECT_KEY,

				///
				/// state in object after a key value delimiter (":") was parsed
				///
				IN_OBJECT_COLON,

				///
				/// state in object after a value was parsed
				///
				IN_OBJECT_VALUE,

				///
				/// state in object after comma delimiter was parsed
				///
				IN_OBJECT_DELIMITER,

				///
				/// end state of the JSON parser
				///
				END,

				///
				/// error state
				///
				ERROR
			};
		}
	}
}

#endif //_UTIL_JSON_PARSER_JSONPARSERSTATE_H
