/**
* Copyright 2018 Dynatrace LLC
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


#ifndef _UTF8STRING_H
#define _UTF8STRING_H

#include <vector>
#include <stdint.h>

namespace base
{

	class UTF8String
	{
	public:

		///
		/// Default constructor building an empty string
		///
		UTF8String();
			
		///
		/// Using a user-provided char sequence initialize this string. Either UTF8 multibyte
		/// data or plain US-ASCII can be used to initialize strings.
		/// @param[in] string_data the string data used to initialize this string
		/// @return a new string initialized to the provided value
		///
		UTF8String(const char* string_data);

		///
		/// Create a new string by duplicating another string ( copy constructor)
		/// @param[in] other the string to duplicate
		/// @return a new string initialized with data from another string
		///
		UTF8String(const UTF8String& other);

		///
		/// Destructor
		///
		~UTF8String();

		///
		/// Returns the string size. For UTF8 this is not necessarily the number of bytes
		/// @returns the number of characters
		///
		int32_t getStringLength();

		///
		/// Returns the string data as vector of chars
		/// @returns the string data.
		///
		std::vector<char>& getStringData();


		///
		/// Check for invalid codepoints
		/// -replace invalid UTF8 codepoints
		/// @param[in] stringData the string data to validate
		/// @returns 0 on sucess of validation or if already valid string, non-0 values in all other cases
		///
		void validateString(const char* stringData);

		///
		/// Compare two strings
		/// @param[other] other string to compare this instance against
		/// @return 0 if strings are equal, -1/1 if not equal
		///
		int32_t compare(const UTF8String& other);

		///
		/// Compare two strings
		/// @param[other] other string to compare this instance against
		/// @return 0 if strings are equal, -1/1 if not equal
		///
		int32_t compare(const char* other);

		///
		/// Concatenate two strings
		/// @param[in] string the current string
		/// @param[in]  add content of second string to current string
		///
		void concatenate(const UTF8String& data);

		///
		/// Concatenate two strings
		/// @param[in] s add content of s to current string
		///
		void concatenate(const char* data);

		///
		/// Find first occurence of character. Indices do not refer to bytes, instead they refer to actual
		/// characters. The reason is that UTF8 characters can span multiple bytes.
		/// @param[in] character the character to search in the string, character also supports multi-byte UTF8 characters
		/// @param[in] offset start search for character at the given offset
		/// @return the index of 
		///
		int32_t getIndexOf(const char* character, size_t offset);

		///
		/// Create a substring by using the range \c begin to \c end. Indices do not refer to bytes, instead they refer to actual
		/// characters. The reason is that UTF8 characters can span multiple bytes.
		/// @param[in] s the string to use
		/// @param[in] start start index of substring
		/// @param[in] end end end index of the substring
		/// @returns a substring created from this string with the range from begin to end
		///
		UTF8String* substring(size_t start, size_t end);

	private:

		///
		/// Checks if the given character belongs to a previous UTF8 multibyte character
		/// NOTE: The bit mask @s '10xxxxxx' determines if a character belongs to a
		///       previous character.
		/// @param[in] character codepoint to check 
		/// @returns flag if character belongs to a previous charater
		///
		bool isPartOfPreviousUtf8Multibyte(const unsigned char character);

		///
		/// Calculate the width of UTF8 multi byte characters. ASCII character return 1.
		/// UTF8 characters return sizes between 2 and 4 bytes.
		/// @param[in] character codepoint to check
		/// @returns the number of bytes taken by the current character
		///
		size_t getByteWidthOfCharacter(const unsigned char character);

		//internal storage with UTF8 compliant string
		std::vector<char> mData;

		//number of characters in string
		size_t mStringLength;

	};
}
#endif