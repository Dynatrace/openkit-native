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

using namespace base;

UTF8String::UTF8String()
{
	mStringLength = 0;
}

UTF8String::UTF8String(const char* string_data)
{
	if (string_data != NULL)
	{
		validateString(string_data);
	}
}

UTF8String::UTF8String(const UTF8String& other)
{
	mStringLength = other.mStringLength;
	mData.insert(mData.begin(), other.mData.begin(), other.mData.end());
}

UTF8String::~UTF8String()
{
	mData.clear();
}

int32_t UTF8String::getStringLength() const
{
	return mStringLength;
}

const std::string UTF8String::getStringData() const
{
	return mData;
}

bool UTF8String::isPartOfPreviousUtf8Multibyte(const unsigned char character) const
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

size_t UTF8String::getByteWidthOfCharacter(const unsigned char character) const
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

void UTF8String::validateString(const char* string_data)
{
	char replacement_character_ascii = '?';
	if (string_data != NULL)
	{
		uint32_t bytelen = 0;

		uint32_t index = 0;
		while (string_data[index++] != '\0')
		{
			bytelen++;
		}
		if (bytelen > 0)
		{
			bytelen++;//count null terminating character in case of a string with at least 1 character
		}
		else
		{
			this->mStringLength = 0;
			return;
		}
		
		mStringLength = 0;

		mData.clear();

		int32_t multibyte_sequence_length = -1;
		int32_t multibyte_sequence_pos = -1;

		size_t strlen = 0;
		unsigned char character_at_pos = 0;
		for (int32_t i = 0; i < bytelen - 1; i++)//omit \0 at the end of the array
		{
			character_at_pos = (unsigned char)string_data[i];
			int32_t bytes_used_for_current_character = getByteWidthOfCharacter(character_at_pos);

			if (isPartOfPreviousUtf8Multibyte(character_at_pos) == 1)
			{
				multibyte_sequence_pos++;
				if (multibyte_sequence_pos > multibyte_sequence_length - 1)//more follow up characters than expectesd
				{
					this->mData.push_back(replacement_character_ascii);
					strlen++;

					multibyte_sequence_length = -1;
					multibyte_sequence_pos = -1;
				}
				else if (multibyte_sequence_pos == multibyte_sequence_length - 1)
				{
					size_t offset = i - multibyte_sequence_length + 1;
					for (int32_t j = 0; j < multibyte_sequence_length; j++)
					{
						this->mData.push_back(string_data[offset + j]);
					}

					multibyte_sequence_length = -1;
					multibyte_sequence_pos = -1;

					strlen++;
				}
			} 
			else if (bytes_used_for_current_character == 1)//valid single byte US-ASCII character only using the lower seven bytes
			{
				if (multibyte_sequence_length >= 0)
				{
					this->mData.push_back(replacement_character_ascii);
					strlen++;
				}
	
				this->mData.push_back(string_data[i]);

				multibyte_sequence_length = -1;
				multibyte_sequence_pos = -1;
				strlen++;
			}
			else if (bytes_used_for_current_character > 1)//start a new multi-byte character
			{
				if (multibyte_sequence_length != -1)//in the middle of another multi-byte character -> previous character invalid
				{
					this->mData.push_back(replacement_character_ascii);
					strlen++;

					multibyte_sequence_length = -1;
					multibyte_sequence_pos = -1;
				}

				multibyte_sequence_length =bytes_used_for_current_character;
				multibyte_sequence_pos = 0;
			}
		}

		mStringLength = strlen;
		mData.push_back('\0'); //only append '\0' character for strings with at least one character
	}
}

int32_t UTF8String::compare(const UTF8String& other) const
{
	int result = 0;
	result = (mData.size() < other.mData.size());
	if (result == 0)
	{
		result = memcmp(&(mData[0]), &(other.mData[0]), mData.size());
	}
	return result;
}

int32_t UTF8String::compare(const char* other) const
{
	if (other != NULL)
	{
		UTF8String newString(other);
		return compare(newString);
	}
	return -1;
}

void UTF8String::concatenate(const UTF8String& string)
{
	if (string.getStringLength() > 0)
	{
		mData.insert(mData.end() - 1, string.mData.begin(), string.mData.end() - 1); //write before old string end '\0'
		mStringLength += string.mStringLength;
	}
}

void UTF8String::concatenate(const char* string)
{
	UTF8String concatenateString(string);
	concatenate(concatenateString);
}

//character can be multi-byte
int32_t UTF8String::getIndexOf(const char* comparison_character, size_t offset = 0) const
{
	if (offset < 0 && offset >= mData.size())
	{
		return -1;
	}

	size_t number_of_bytes_compare = getByteWidthOfCharacter((unsigned char)(*comparison_character));

	const char* current_character = &(mData[0]);
	int32_t i;
	for (i = 0; i < mData.size(); i++)
	{
		size_t number_of_bytes = getByteWidthOfCharacter((unsigned char)(*current_character));

		if (i >= offset)
		{
			if (number_of_bytes_compare == number_of_bytes)
			{
				if (number_of_bytes == 1 && (*current_character) == (*comparison_character))
				{
					return i;
				}
				if (number_of_bytes > 1 && memcmp(current_character, comparison_character, number_of_bytes) == 0)
				{
					return i;
				}
			}
		}
		current_character += number_of_bytes;
	}
	return -1;
}

UTF8String* UTF8String::substring(size_t start, size_t end) const
{
	if ( start < 0 || start > mData.size()
		|| end < 0 || end > mData.size()
		|| end <start )
	{
		return NULL;
	}

	size_t byte_offset_start = 0;
	size_t byte_offset_end = 0;

	const char* current_character = &(mData[0]);
	size_t bytepos = 0;
	int32_t i;
	for (i = 0; i < mData.size(); i++)
	{
		size_t number_of_bytes = getByteWidthOfCharacter((unsigned char)(*current_character));

		if (i == start)
		{
			byte_offset_start = bytepos;
		}
		if (i == end)
		{
			byte_offset_end = bytepos + number_of_bytes -1;//in case of a multibyte character num - 1 is non-zero
		}

		current_character += number_of_bytes;
		bytepos += number_of_bytes;
	}

	if (byte_offset_start > 0 && byte_offset_start < byte_offset_end && byte_offset_end < mData.size())
	{
		UTF8String* substring = new UTF8String();
		if (substring != NULL)
		{
			substring->mStringLength = end - (start - 1);
			substring->mData.insert(substring->mData.begin(), mData.begin() + byte_offset_start, mData.begin() + byte_offset_end + 1);
			substring->mData.push_back('\0');

			return substring;
		}
	}
	return NULL;
}