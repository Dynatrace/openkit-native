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

#ifndef _CORE_UTIL_STRINGUTIL_H
#define _CORE_UTIL_STRINGUTIL_H

#include <string>

namespace core
{
	namespace util
	{
		///
		/// Utility class for string operations
		///
		class StringUtil
		{
		public:

			///
			/// Removes the leading whitespaces from the given string.
			/// @param string the string from which leading whitespace characters will be removed.
			///
			static void leftTrim(std::string& string);

			///
			/// Returns copy of the given string without leading whitespaces
			/// @param string the input string from which a trimmed copy will be created.
			static std::string leftTrimToCopy(const std::string& string);

			///
			/// Removes trailing whitespace characters from the given string.
			/// @param string the string from which the trailing whitespace characters will be removed.
			///
			static void rightTrim(std::string& string);

			///
			/// Returns a copy of the given string without trailing whitespaces.
			/// @param string the input string from which a trimmed copy will be created.
			///
			static std::string rightTrimToCopy(const std::string& string);

			///
			/// Removes leading and trailing whitespace characters from the given string
			/// @param[in] string the string from which the whitespace characters will be trimmed.
			///
			static void trim(std::string& string);

			///
			/// Returns a copy f the given string without leading and trailing whitespaces.
			/// @param string the input string from which trimmed copy will be created.
			///
			static std::string trimToCopy(const std::string& string);

			///
			/// Generates a 64 bit hash from the given string
			/// @param string
			/// @return a hashed representation of the given input string, or @c 0 if the given input string is empty.
			///
			static int64_t to64BitHash(const std::string& string);

			///
			/// Tries to convert the given string to a numeric value. In case it is a non numeric value a corresponding
			/// 64 bit hash will be calculated.
			/// @param string the string to be parsed/hashed.
			/// @return either the numeric representation of the given string, or in case the string itself is not numeric,
			///	  a 64 bit hash representation of the given string (@c 0 in case the given string is @c nullptr or empty)
			///
			static int64_t toNumericOr64BitHash(const char* string);

		private:
			/// utility class, not instantiable
			StringUtil();
		};
	}
}

#endif //_CORE_UTIL_STRINGUTIL_H
