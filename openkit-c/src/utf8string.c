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
	utf8string* s = (utf8string*)memory_malloc(sizeof(utf8string));
	if (s != NULL)
	{
		s->data = NULL;

		validate_string(s, string_data);

		return s;
	}
	return NULL;
}

int32_t validate_string(utf8string* target, const char* s)
{
	char replacement_character_ascii = '?';
	if (target != NULL && s != NULL)
	{
		uint32_t bytelen = 0;

		uint32_t index = 0;
		while (s[index++] != '\0')
		{
			bytelen++;
		}
		bytelen++;//also count null terminating character

		target->byte_length = bytelen;
		target->string_length = -1;

		target->data = (char*)memory_calloc(target->byte_length , sizeof(char));
		//memory_copy(target->data, s, target->byte_length);
		//target->data[target->byte_length] = '\0';

		uint32_t i = 0;
		int32_t multibyte_sequence_length = -1;
		int32_t multibyte_sequence_pos = -1;

		size_t strlen = 0;
		char* write_pointer = &target->data[0];
		unsigned char character_at_pos = 0;
		for (i; i < target->byte_length - 1; i++)//omit \0 at the end of the array
		{
			character_at_pos = (unsigned char)s[i];
			if ((character_at_pos & 0x80) == 0)//valid single byte US-ASCII character only using the lower seven bytes
			{
				if (multibyte_sequence_length >= 0)
				{
					*write_pointer = replacement_character_ascii;
					write_pointer++;
					strlen++;
				}
	
				*write_pointer = s[i]; // copy single byte character
				write_pointer++;

				multibyte_sequence_length = -1;
				multibyte_sequence_pos = -1;
				strlen++;
			}
			else if ((character_at_pos & 0xC0) == 0x80)//only highest bit set -> belongs to a multibyte utf character 
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
					memory_copy(write_pointer, &s[i - multibyte_sequence_length + 1], multibyte_sequence_length);
					write_pointer += multibyte_sequence_length;

					multibyte_sequence_length = -1;
					multibyte_sequence_pos = -1;

					strlen++;
				}
			}
			else if ((character_at_pos & 0xE0) == 0xC0)//2 highest bits set -> start a new character with a length of two bytes
			{
				if (multibyte_sequence_length != -1)//in the middle of another character -> previous character invalid
				{
					*write_pointer = replacement_character_ascii;
					write_pointer++;
					strlen++;

					multibyte_sequence_length = -1;
					multibyte_sequence_pos = -1;
				}

				multibyte_sequence_length = 2;
				multibyte_sequence_pos = 0;
			}
			else if ((character_at_pos & 0xF0) == 0xE0)//3 highest bits set -> start a new character with a length of three bytes
			{
				if (multibyte_sequence_length != -1)//in the middle of another character -> previous character invalid
				{
					*write_pointer = replacement_character_ascii;
					write_pointer++;
					strlen++;

					multibyte_sequence_length = -1;
					multibyte_sequence_pos = -1;
				}

				multibyte_sequence_length = 3;
				multibyte_sequence_pos = 0;
			}
			else if ((character_at_pos & 0xF8) == 0xF0)//4 highest bits set -> start a new character with a length of four bytes
			{
				if (multibyte_sequence_length != -1)//in the middle of another character -> previous character invalid
				{
					*write_pointer = replacement_character_ascii;
					write_pointer++;
					strlen++;

					multibyte_sequence_length = -1;
					multibyte_sequence_pos = -1;
				}

				multibyte_sequence_length = 4;
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

int32_t concatenate_char_pointer(utf8string* s, const char* other)
{

}

int32_t concatenate_string(utf8string* s, const utf8string* other)
{

}

int32_t index_of(const utf8string* s, const char* character, size_t offset)
{

}

utf8string* substring(const utf8string* s, size_t start, size_t end)
{

}