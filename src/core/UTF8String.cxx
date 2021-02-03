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

#include "UTF8String.h"

#include <cstring>
#include <stdio.h>
#include <sstream>

using namespace core;

UTF8String::UTF8String()
	: mData()
	, mStringLength(0)
{
}

UTF8String::UTF8String(const char* stringData)
	: UTF8String()
{
	if (stringData != nullptr)
	{
		validateString(stringData);
	}
}

UTF8String::UTF8String(const std::string& stringData)
	: UTF8String(stringData.c_str())
{
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

	while (!isStringTerminationCharacter(stringData, byteLength))
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

inline bool UTF8String::isStringTerminationCharacter(const char* stringData, size_t offset) const
{
	auto character = *(stringData + offset);
	if (character == '\0') // standard null terminated string
	{
		return true;
	}

	if (character == '\xC0' && *(stringData + offset + 1) == '\x80') // modified UTF-8 terminated string
	{
		return true;
	}

	return false;
}

bool UTF8String::equals(const UTF8String& other) const
{
	return mData.compare(other.mData) == 0;
}

bool UTF8String::equals(const char* other) const
{
	if (other != nullptr)
	{
		UTF8String newString(other);
		return equals(newString);
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
UTF8String::size_type UTF8String::getIndexOf(const char* comparisonCharacter, size_t offset) const
{
	if (offset == std::string::npos && offset >= mData.size())
	{
		return std::string::npos;
	}

	size_t characterByteWidth = getByteWidthOfCharacter((unsigned char)(*comparisonCharacter));
	if (characterByteWidth == std::string::npos)//error
	{
		return std::string::npos;
	}

	auto currentCharacter = &(mData[0]);

	//this for loop takes multi byte characters into account correctly
	for (size_t i = 0; i < mData.size(); i++)
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

UTF8String UTF8String::substring(size_t start, size_t length) const
{
	// Sanity
	if (length == 0 || start > mData.size())
	{
		return UTF8String();
	}

	size_t byteIndex = 0;
	size_t byteOffsetStart = 0;
	size_t byteOffsetEnd = 0;
	size_t characterIndex = 0;
	size_t characterCounter = 0;

	//collect byte positions correctly counting multi byte characters
	while (byteIndex < mData.size() && characterCounter < length)
	{
		auto currentCharacter = mData[byteIndex];
		auto numberOfBytes = getByteWidthOfCharacter(currentCharacter);
		if (characterIndex == start)
		{
			byteOffsetStart = byteIndex;
		}
		if (characterIndex >= start)
		{
			characterCounter++;
		}
		byteOffsetEnd = byteIndex + numberOfBytes - 1;
		byteIndex += numberOfBytes;
		characterIndex++;
	}

	//cut the new string using the indices
	if (byteOffsetStart != std::string::npos && byteOffsetStart <= byteOffsetEnd && byteOffsetEnd < mData.size())
	{
		UTF8String substring;
		substring.mStringLength = characterCounter;
		substring.mData.insert(substring.mData.begin(), mData.begin() + byteOffsetStart, mData.begin() + byteOffsetEnd + 1);

		return substring;
	}
	return UTF8String();
}


bool UTF8String::empty() const
{
	return mStringLength == 0;
}


UTF8String::size_type UTF8String::size() const
{
	return mData.size();
}

std::vector<UTF8String> UTF8String::split(char delimiter) const
{
	std::vector<UTF8String> parts;
	std::string item;
	std::stringstream ss(mData);
	while (std::getline(ss, item, delimiter))
	{
		parts.push_back(UTF8String(item));
	}
	return parts;
}

bool UTF8String::operator ==(const UTF8String& other) const
{
	return this->equals(other);
}

bool UTF8String::operator !=(const UTF8String& other) const
{
	return !this->equals(other);
}
