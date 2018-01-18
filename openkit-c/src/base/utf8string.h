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

#include <stdint.h>
#include <stddef.h>
///
/// struct encapsulating a string which can either be pure ASCII or multibyte UTF8
///
typedef struct _utf8string
{
	//internal storage with UTF8 compliant string
	char* data;

	//number of characters in string
	size_t string_length;

	//number of bytes consumed by string, due to multi-byte characters possibly different from string length
	size_t byte_length;

	//pointer to location last token operation ended
	char* last_token_end;
} utf8string;

///
/// Using a user-provided char sequence initialize this string. Either UTF8 multibyte
/// data or plain US-ASCII can be used to initialize strings.
/// @param[in] string_data the string data used to initialize this string
/// @return a new string initialized to the provided value
///
utf8string* init_string(const char* string_data);

///
/// Create a new string by duplicating another string
/// @param[in] other the string to duplicate
/// @return a new string initialized with data from another string
///
utf8string* duplicate_string(const utf8string* other);

///
/// Destroy a string and free associated resources
/// @param[in] s the string object to destroy
/// @return 0 in case of success, -1 if problems occured
///
int32_t destroy_string(utf8string* s);

///
/// Check for invalid codepoints
/// -replace invalid UTF8 codepoints
/// @param[in,out] target the string where to copy the valid string to
/// @param[in] s the string to validate
/// @returns 0 on sucess of validation or if already valid string, non-0 values in all other cases
///
int32_t validate_string(utf8string* target, const char* s);

///
/// Compare two string
/// @param[in] s1 first string
/// @param[in] s2 second string
/// @return 0 if strings are equal, -1/1 if not equal
///
int32_t compare(const utf8string* s1, const utf8string* s2);

///
/// Concatenate two strings
/// @param[in] string the current string
/// @param[in] s add content of s to current string
/// @return 0 in case of success, other values indicate issues during operation
///
int32_t concatenate_char_pointer(utf8string* s, const char* other);

///
/// Concatenate two strings
/// @param[in] string the current string
/// @param[in] s add content of second string to current string
/// @return 0 in case of success, other values indicate issues during operation
///
int32_t concatenate(utf8string* s, const utf8string* other);

///
/// Find first occurence of character. Indices do not refer to bytes, instead they refer to actual
/// characters. The reason is that UTF8 characters can span multiple bytes.
/// @param[in] s the string to use
/// @param[in] character the character to search in the string, character also supports multi-byte UTF8 characters
/// @param[in] offset start search for character at the given offset
/// @return the index of 
///
int32_t index_of(const utf8string* s, const char* character, size_t offset);

///
/// Create a substring by using the range \c begin to \c end. Indices do not refer to bytes, instead they refer to actual
/// characters. The reason is that UTF8 characters can span multiple bytes.
/// @param[in] s the string to use
/// @param[in] start start index of substring
/// @param[in] end end end index of the substring
/// @returns a substring created from this string with the range from begin to end
///
utf8string* substring(const utf8string* s, size_t start, size_t end);

#endif