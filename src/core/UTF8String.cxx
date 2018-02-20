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

#include "UTF8String.h"
#include "memory.h"

#include <stdio.h>

using namespace core;

UTF8String::UTF8String()
	: mStringLength(0)
{
}

UTF8String::UTF8String(const char* string_data)
	: mStringLength(0)
{
	if (string_data != nullptr)
	{
		validateString(string_data);
	}
}

UTF8String::~UTF8String()
{
	mData.clear();
}

UTF8String::size_type UTF8String::getStringLength() const
{
	return mStringLength;
}

const std::string& UTF8String::getStringData() const
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
		return true;
	}
	return false;
}

size_t UTF8String::getByteWidthOfCharacter(const unsigned char character) const
{
	if ((character & 0x80) == 0) //valid single byte US-ASCII character only using the lower seven bits
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
	return 0;
}

void UTF8String::validateString(const char* stringData)
{
	auto replacementCharacterASCII = "\xEF\xBF\xBD";
	if (stringData == nullptr || stringData[0] == '\0')
	{
		mStringLength = 0;
		return;
	}

	auto byteLength = 0;

	while (*(stringData + byteLength) != '\0')
	{
		byteLength++;
	}

	if (byteLength <= 0)
	{
		return;
	}
		
	mData.clear();

	auto multibyteSeqenceLength = -1;
	auto multibyteSequencePosition = -1;

	auto characterCount = 0; //number of characters, either UTF8 multibyte or ASCII single byte
	for (auto i = 0; i < byteLength; i++)//omit \0 at the end of the array
	{
		auto byteWidthOfCurrentCharacter = getByteWidthOfCharacter(static_cast<unsigned char>(stringData[i]));

		if (isPartOfPreviousUtf8Multibyte(static_cast<unsigned char>(stringData[i])))
		{
			multibyteSequencePosition++;
			if (multibyteSequencePosition > multibyteSeqenceLength - 1)//more follow up characters than expected
			{
				this->mData.append(replacementCharacterASCII);
				characterCount ++;

				multibyteSeqenceLength = -1;
				multibyteSequencePosition = -1;
			}
			else if (multibyteSequencePosition == multibyteSeqenceLength - 1)
			{
				auto offset = i - multibyteSeqenceLength + 1;
				for (auto j = 0; j < multibyteSeqenceLength; j++)
				{
					this->mData.push_back(stringData[offset + j]);
				}

				multibyteSeqenceLength = -1;
				multibyteSequencePosition = -1;

				characterCount++;
			}
		} 
		else if (byteWidthOfCurrentCharacter == 1)//valid single byte US-ASCII character only using the lower seven bytes
		{
			if (multibyteSeqenceLength >= 0)
			{
				this->mData.append(replacementCharacterASCII);
				characterCount ++;
			}
	
			this->mData.push_back(stringData[i]);

			multibyteSeqenceLength = -1;
			multibyteSequencePosition = -1;
			characterCount++;
		}
		else if (byteWidthOfCurrentCharacter > 1)//start a new multi-byte character
		{
			if (multibyteSeqenceLength != -1)//in the middle of another multi-byte character -> previous character invalid
			{
				this->mData.append(replacementCharacterASCII);
				characterCount++;

				multibyteSeqenceLength = -1;
				multibyteSequencePosition = -1;
			}

			multibyteSeqenceLength = byteWidthOfCurrentCharacter;
			multibyteSequencePosition = 0;
		}
	}

	mStringLength = characterCount;
}

bool UTF8String::compare(const UTF8String& other) const
{
	return mData.compare(other.mData) == 0;
}

bool UTF8String::compare(const char* other) const
{
	if (other != nullptr)
	{
		UTF8String newString(other);
		return compare(newString);
	}
	return false;
}

void UTF8String::concatenate(const UTF8String& string)
{
	if (string.getStringLength() > 0)
	{
		mData.insert(mData.end(), string.mData.begin(), string.mData.end());
		mStringLength += string.mStringLength;
	}
}

void UTF8String::concatenate(const char* string)
{
	UTF8String concatenateString(string);
	concatenate(concatenateString);
}

//character can be multi-byte
UTF8String::size_type UTF8String::getIndexOf(const char* comparisonCharacter, size_t offset = 0) const
{
	if (offset < 0 && offset >= mData.size())
	{
		return std::string::npos;
	}

	size_t characterByteWidth = getByteWidthOfCharacter((unsigned char)(*comparisonCharacter));
	if (characterByteWidth < 0)//error
	{
		return std::string::npos;
	}

	auto currentCharacter = &(mData[0]);

	//this for loop takes multi byte characters into account correctly
	for (auto i = 0; i < mData.size(); i++)
	{
		auto numberOfBytesCurrent = getByteWidthOfCharacter((unsigned char)(*currentCharacter));

		if (i >= offset)
		{
			if (characterByteWidth == numberOfBytesCurrent)
			{
				if (numberOfBytesCurrent == 1 && (*currentCharacter) == (*comparisonCharacter))
				{
					return i;
				}
				if (numberOfBytesCurrent > 1 && memcmp(currentCharacter, comparisonCharacter, numberOfBytesCurrent) == 0)
				{
					return i;
				}
			}
		}
		currentCharacter += numberOfBytesCurrent;
	}
	return std::string::npos;
}

UTF8String UTF8String::substring(size_t start, size_t end) const
{
	if ( start < 0 || start > mData.size()
		|| end < 0 || end > mData.size()
		|| end <start )
	{
		return UTF8String();
	}

	auto byteOffsetStart = 0;
	auto byteOffsetEnd = 0;

	auto index = 0;
	auto characterCounter = 0;

	//collect byte positions correctly counting multi byte characters
	while(index < mData.size() && ( byteOffsetStart == 0 || byteOffsetEnd == 0))
	{
		auto currentCharacter = mData[index];
		auto numberOfBytes = getByteWidthOfCharacter(currentCharacter);

		if (characterCounter == start)
		{
			byteOffsetStart = index;
		}
		if (characterCounter == end)
		{
			byteOffsetEnd = index + numberOfBytes -1;//in case of a multibyte character num - 1 is non-zero
		}

		characterCounter++;
		index += numberOfBytes;
	}

	//cut the new string using the indices
	if (byteOffsetStart > 0 && byteOffsetStart < byteOffsetEnd && byteOffsetEnd < mData.size())
	{
		UTF8String substring;
		substring.mStringLength = end - (start - 1);
		substring.mData.insert(substring.mData.begin(), mData.begin() + byteOffsetStart, mData.begin() + byteOffsetEnd + 1);
		substring.mData.push_back('\0');

		return substring;
	}
	return UTF8String();
}

bool UTF8String::empty() const
{
	return mStringLength == 0;
}