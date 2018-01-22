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

#include "utf8string.h"
#include "memory.h"

#include <stdio.h>

utf8string* init_string(const char* string_data)
{
	if (string_data != NULL)
	{
		utf8string* s = (utf8string*)memory_malloc(sizeof(utf8string));
		if (s != NULL)
		{
			s->data = NULL;

			validate_string(s, string_data);

			return s;
		}
	}
	return NULL;
}

///
/// Return if current byte is part of a multi-byte UTF8 character
///
int32_t is_part_of_previous_utf8_multibyte(const unsigned char character)
{
	//only highest bit set -> belongs to a multibyte utf character 
	// |---|---|---|---|---|---|---|---|
	// | 1 | 1 | 0 | 0 | 0 | 0 | 0 | 0 |  -> 0xC0
	// |---|---|---|---|---|---|---|---|
	// &
	// |---|---|---|---|---|---|---|---|
	// | 1 | 0 | # | # | # | # | # | # |  ?
	// |---|---|---|---|---|---|---|---|
	// =
	// |---|---|---|---|---|---|---|---|
	// | 1 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |  -> 0x80
	// |---|---|---|---|---|---|---|---|

	if ((character & 0xC0) == 0x80)
	{
		return 1;
	}
	return 0;
}

int32_t get_byte_width_of_character(const unsigned char character)
{
	if ((character & 0x80) == 0) //valid single byte US-ASCII character only using the lower seven bytes
	{
		// |---|---|---|---|---|---|---|---|
		// | 1 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |  -> 0x80
		// |---|---|---|---|---|---|---|---|
		// &
		// |---|---|---|---|---|---|---|---|
		// | 0 | # | # | # | # | # | # | # |  ?
		// |---|---|---|---|---|---|---|---|
		// =
		// |---|---|---|---|---|---|---|---|
		// | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |  -> 0
		// |---|---|---|---|---|---|---|---|
		return 1;
	}

	else if ((character & 0xE0) == 0xC0)//2 highest bits set -> marks a 2 byte character)
	{
		// |---|---|---|---|---|---|---|---|
		// | 1 | 1 | 1 | 0 | 0 | 0 | 0 | 0 |  -> 0xE0
		// |---|---|---|---|---|---|---|---|
		// &
		// |---|---|---|---|---|---|---|---|
		// | 1 | 1 | 0 | # | # | # | # | # |  ?
		// |---|---|---|---|---|---|---|---|
		// =
		// |---|---|---|---|---|---|---|---|
		// | 1 | 1 | 0 | 0 | 0 | 0 | 0 | 0 |  -> 0xC0
		// |---|---|---|---|---|---|---|---|
		return 2;
	}
	else if ((character & 0xF0) == 0xE0)//3 highest bits set -> marks a 3 byte character
	{
		// |---|---|---|---|---|---|---|---|
		// | 1 | 1 | 1 | 1 | 0 | 0 | 0 | 0 |  -> 0xF0
		// |---|---|---|---|---|---|---|---|
		// &
		// |---|---|---|---|---|---|---|---|
		// | 1 | 1 | 1 | 0 | # | # | # | # |  ?
		// |---|---|---|---|---|---|---|---|
		// =
		// |---|---|---|---|---|---|---|---|
		// | 1 | 1 | 1 | 0 | 0 | 0 | 0 | 0 |  -> 0xE0
		// |---|---|---|---|---|---|---|---|
		return 3;
	}
	else if ((character & 0xF8) == 0xF0)//4 highest bits set -> marks a 4 byte character
	{
		// |---|---|---|---|---|---|---|---|
		// | 1 | 1 | 1 | 1 | 1 | 0 | 0 | 0 |  -> 0xF8
		// |---|---|---|---|---|---|---|---|
		// &
		// |---|---|---|---|---|---|---|---|
		// | 1 | 1 | 1 | 1 | 0 | # | # | # |  ?
		// |---|---|---|---|---|---|---|---|
		// =
		// |---|---|---|---|---|---|---|---|
		// | 1 | 1 | 1 | 1 | 0 | 0 | 0 | 0 |  -> 0xF0
		// |---|---|---|---|---|---|---|---|
		return 4;
	}
	return -1;
}

//s->string_data
int32_t validate_string(utf8string* target, const char* string_data)
{
	char replacement_character_ascii = '?';
	if (target != NULL && string_data != NULL)
	{
		uint32_t bytelen = 0;

		uint32_t index = 0;
		while (string_data[index++] != '\0')
		{
			bytelen++;
		}
		bytelen++;//also count null terminating character

		target->byte_length = bytelen;
		target->string_length = -1;

		target->data = (char*)memory_calloc(target->byte_length , sizeof(char));

		uint32_t i = 0;
		int32_t multibyte_sequence_length = -1;
		int32_t multibyte_sequence_pos = -1;

		size_t strlen = 0;
		char* write_pointer = &target->data[0];
		unsigned char character_at_pos = 0;
		for (i; i < target->byte_length - 1; i++)//omit \0 at the end of the array
		{
			character_at_pos = (unsigned char)string_data[i];
			int32_t bytes_used_for_current_character = get_byte_width_of_character(character_at_pos);

			if (is_part_of_previous_utf8_multibyte(character_at_pos) == 1)
			{
				multibyte_sequence_pos++;
				if (multibyte_sequence_pos > multibyte_sequence_length - 1)//more follow up characters than expectesd
				{
					*write_pointer = replacement_character_ascii;
					write_pointer++;
					strlen++;

					multibyte_sequence_length = -1;
					multibyte_sequence_pos = -1;
				}
				else if (multibyte_sequence_pos == multibyte_sequence_length - 1)
				{
					memory_copy(write_pointer, &string_data[i - multibyte_sequence_length + 1], multibyte_sequence_length);
					write_pointer += multibyte_sequence_length;

					multibyte_sequence_length = -1;
					multibyte_sequence_pos = -1;

					strlen++;
				}
			} 
			else if (bytes_used_for_current_character == 1)//valid single byte US-ASCII character only using the lower seven bytes
			{
				if (multibyte_sequence_length >= 0)
				{
					*write_pointer = replacement_character_ascii;
					write_pointer++;
					strlen++;
				}
	
				*write_pointer = string_data[i]; // copy single byte character
				write_pointer++;

				multibyte_sequence_length = -1;
				multibyte_sequence_pos = -1;
				strlen++;
			}
			else if (bytes_used_for_current_character > 1)//start a new multi-byte character
			{
				if (multibyte_sequence_length != -1)//in the middle of another multi-byte character -> previous character invalid
				{
					*write_pointer = replacement_character_ascii;
					write_pointer++;
					strlen++;

					multibyte_sequence_length = -1;
					multibyte_sequence_pos = -1;
				}

				multibyte_sequence_length =bytes_used_for_current_character;
				multibyte_sequence_pos = 0;
			}
		}

		target->string_length = strlen;
		target->byte_length = write_pointer - target->data; // byte length can change in case of invalid UTF8 characters

		target->data[target->byte_length] = '\0';
		target->byte_length++;//include '\0' at the end
	}
}

utf8string* duplicate_string(const utf8string* other)
{

}

int32_t destroy_string(utf8string* s)
{
	if (s != NULL)
	{
		if (s->data)
		{
			memory_free(s->data);
		}
		memory_free(s);
	}
}

int32_t compare(const utf8string* s1, const utf8string* s2)
{

}

int32_t concatenate_char_pointer(utf8string* string, const char* other)
{

}

int32_t concatenate_string(utf8string* s, const utf8string* other)
{

}

//character can be multi-byte
int32_t index_of(const utf8string* string, const char* comparison_character, size_t offset)
{
	if (offset < 0 && offset >= string->string_length)
	{
		return -1;
	}

	size_t number_of_bytes_compare = get_byte_width_of_character((unsigned char)(*comparison_character));

	char* current_character = &(string->data[offset]);
	int32_t i;
	for (i = offset; i < string->string_length; i++)
	{
		size_t number_of_bytes = get_byte_width_of_character((unsigned char)(*current_character));

		//compare length
		if (number_of_bytes_compare == number_of_bytes)
		{
			if (number_of_bytes == 1 && (*current_character) == (*comparison_character))
			{
				return i;
			}
			if (number_of_bytes > 1 && memory_compare(current_character, comparison_character, number_of_bytes) == 0)
			{
				return i;
			}
		}
		current_character += number_of_bytes;
	}
	return -1;
}

utf8string* substring(const utf8string* string, size_t start, size_t end)
{

}