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
#include <type_traits>
#include <sstream>
#include <iomanip>
#include <locale>
#include <limits>

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

			///
			/// Converts the given UTF-16 (2-byte character) string to a corresponding UTF-8 (single byte character)
			/// string representation
			///
			/// @param utf16String the string to be converted
			///
			/// @return the converted UTF-8 string
			///
			static std::string convertUtf16StringToUtf8String(std::u16string& utf16String);

			///
			/// Determines if the given character is a unicode high-surrogate (leading surrogate) character.
			///
			/// @par
			/// Surrogate characters not represent characters by themselves, but are used in UTF-16 encoding when
			/// representing supplemental characters.
			///
			/// @param character the character to be checked
			/// @return @c if the given character represents a high surrogate character, @c false otherwise.
			///
			static bool isHighSurrogateCharacter(int32_t character);

			///
			/// Determines if the given character is a unicode low-surrogate (trailing surrogate) character.
			///
			/// @par
			/// Surrogate characters not represent characters by themselves, but are used in UTF-16 encoding when
			/// representing supplemental characters.
			///
			/// @param character the character to be checked
			/// @return @c true if the given character represents a low surrogate character, @c false otherwise.
			///
			static bool isLowSurrogateCharacter(int32_t character);

			///
			/// Method to convert an integral type into a string with invariant culture.
			///
			/// @par
			/// As the std::to_string method is locale aware and std::to_chars is introduced
			/// with C++17, this wrapper is offering an alternative.
			///
			/// @param intValue the integral value to convert
			/// @return converted string value of @c intValue
			///
			template <typename Integer>
			typename std::enable_if<std::is_integral<Integer>::value, std::string>::type static toInvariantString(Integer intValue)
			{
				std::ostringstream oss;
				oss.imbue(std::locale::classic());
				oss << intValue;

				return oss.str();
			}

			///
			/// Method to convert a floating point type into a string with invariant culture.
			///
			/// @par
			/// As the std::to_string method is locale aware and std::to_chars is introduced
			/// with C++17, this wrapper is offering an alternative.
			///
			/// @par
			/// The precision used is large enough to guarantee a correct parsing again.
			/// The output written is as short as possible, but guaranteed to be parseable by Dynatrace cluster.
			///
			/// @param floatValue the floating point value to convert
			/// @return converted string value of @c floatValue
			///
			template <typename Float>
			typename std::enable_if<std::is_floating_point<Float>::value, std::string>::type static toInvariantString(Float floatValue)
			{
				std::ostringstream oss;
				oss.imbue(std::locale::classic());
				oss << std::setprecision(std::numeric_limits<Float>::max_digits10) << floatValue;

				return oss.str();
			}

		private:
			/// utility class, not instantiable
			StringUtil();
		};
	}
}

#endif //_CORE_UTIL_STRINGUTIL_H
