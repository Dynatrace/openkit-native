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


#ifndef _CORE_UTF8STRING_H
#define _CORE_UTF8STRING_H

#include <string>

namespace core
{
	///
	/// A string class based on std::string able to work with mixed UTF8/ASCII strings
	///
	class UTF8String
	{
	public:

		///type definition for size based on std::string:size
		typedef std::string::size_type size_type;

		///
		/// Default constructor building an empty string
		///
		UTF8String();
			
		///
		/// Using a user-provided char sequence initialize this string. Either UTF8 multibyte
		/// data or plain US-ASCII can be used to initialize strings.
		/// @param[in] stringData the string data used to initialize this string
		/// @return a new string initialized to the provided value
		///
		UTF8String(const char* stringData);

		///
		/// Destructor
		///
		~UTF8String();

		///
		/// Returns the string size. For UTF8 this is not necessarily the number of bytes
		/// @returns the number of characters
		///
		size_type getStringLength() const;

		///
		/// Returns the string data as vector of chars
		/// @returns the string data.
		///
		const std::string& getStringData() const;


		///
		/// Check for invalid codepoints
		/// -replace invalid UTF8 codepoints
		/// @param[in] stringData the string data to validate
		///
		void validateString(const char* stringData);

		///
		/// Compare two strings
		/// @param[in] other string to compare this instance against
		/// @return @c true if strings are equal, @c false if not equal
		///
		bool compare(const UTF8String& other) const;

		///
		/// Compare two strings
		/// @param[in] other string to compare this instance against
		/// @return @c true if strings are equal, @c false if not equal
		///
		bool compare(const char* other) const;

		///
		/// Concatenate two strings
		/// @param[in] data add @c data to the current string
		///
		void concatenate(const UTF8String& data);

		///
		/// Concatenate two strings
		/// @param[in] data add content of @c data to current string
		///
		void concatenate(const char* data);

		///
		/// Find first occurence of character. Indices do not refer to bytes, instead they refer to actual
		/// characters. The reason is that UTF8 characters can span multiple bytes.
		/// @param[in] comparisonCharacter the character to search in the string, character also supports multi-byte UTF8 characters
		/// @param[in] offset start search for character at the given offset
		/// @return the index of 
		///
		size_t getIndexOf(const char* comparisonCharacter, size_t offset) const;

		///
		/// Create a substring by using the range @c start to @c end. Indices do not refer to bytes, instead they refer to actual
		/// characters. The reason is that UTF8 characters can span multiple bytes.
		/// @param[in] start start index of substring
		/// @param[in] end end end index of the substring
		/// @returns a substring created from this string with the range from begin to end
		///
		UTF8String substring(size_t start, size_t end) const;

	private:

		///
		/// Checks if the given character belongs to a previous UTF8 multibyte character
		/// NOTE: The bit mask @s '10xxxxxx' determines if a character belongs to a
		///       previous character.
		/// @param[in] character codepoint to check 
		/// @returns flag if character belongs to a previous charater
		///
		bool isPartOfPreviousUtf8Multibyte(const unsigned char character) const;

		///
		/// Calculate the width of UTF8 multi byte characters. ASCII character return 1.
		/// UTF8 characters return sizes between 2 and 4 bytes.
		/// @param[in] character codepoint to check
		/// @returns the number of bytes taken by the current character
		///
		size_t getByteWidthOfCharacter(const unsigned char character) const;

	private:

		//internal storage with UTF8 compliant string
		std::string mData;

		//number of characters in string
		size_type mStringLength;

	};
}
#endif