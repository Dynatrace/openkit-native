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

#include "StringUtil.h"

#include <iterator>
#include <algorithm>
#include <cctype>
#include <codecvt>
#include <locale>

using namespace core::util;


void StringUtil::leftTrim(std::string& string)
{
	auto matcher = [](int ch)
	{ return !isspace(ch); };

	auto firstNonSpace = std::find_if(string.begin(), string.end(), matcher);
	string.erase(string.begin(), firstNonSpace);
}

std::string StringUtil::leftTrimToCopy(const std::string& str)
{
	auto string = std::string(str);
	leftTrim(string);
	return string;
}

void StringUtil::rightTrim(std::string& string)
{
	auto matcher = [](int ch)
	{ return !std::isspace(ch); };

	auto firstNonSpace = std::find_if(string.rbegin(), string.rend(), matcher);
	string.erase(firstNonSpace.base(), string.end());
}

std::string StringUtil::rightTrimToCopy(const std::string& str)
{
	auto string = std::string(str);
	rightTrim(string);
	return string;
}

void StringUtil::trim(std::string& string)
{
	leftTrim(string);
	rightTrim(string);
}

std::string StringUtil::trimToCopy(const std::string& str)
{
	auto string = std::string(str);
	trim(string);
	return string;
}

int64_t StringUtil::to64BitHash(const std::string& str)
{
	auto string = std::string(str);
	trim(string);

	if (string.empty())
	{
		return 0;
	}

	int64_t hash1 = 5381;
	int64_t hash2 = hash1;

	for (auto it = string.begin(); it < string.end(); it += 2)
	{
		hash1 = ((hash1 << 5) + hash1) ^ *it;

		if (it + 1 >= string.end())
		{
			break;
		}

		hash2 = ((hash2 << 5) + hash2) ^ *(it + 1);
	}

	return hash1 + (hash2 * 1566083941);
}

int64_t StringUtil::toNumericOr64BitHash(const char* str)
{
	if (str == nullptr)
	{
		return 0;
	}

	auto string = std::string(str);
	StringUtil::trim(string);

	if (string.empty())
	{
		return 0;
	}

	char* parsedEndPtr = nullptr;
	int64_t parsedDeviceId = std::strtol(string.c_str(), &parsedEndPtr, 10);
	if (*parsedEndPtr == 0)
	{
		return parsedDeviceId;
	}

	return StringUtil::to64BitHash(string);
}

std::string StringUtil::convertUtf16StringToUtf8String(std::u16string& utf16String)
{
#if _WIN32 && _MSC_VER >= 1900
	// a bug in Visual C++ STL library results in a linking error when using std::codecvt.
	// see: https://social.msdn.microsoft.com/Forums/en-US/8f40dcd8-c67f-4eba-9134-a19b9178e481/vs-2015-rc-linker-stdcodecvt-error

	std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> convert;
	auto p = reinterpret_cast<const int16_t*>(utf16String.data());
	return convert.to_bytes(p, p + utf16String.size());
#else
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
	return convert.to_bytes(utf16String);
#endif
}

bool StringUtil::isHighSurrogateCharacter(int32_t character)
{
	return character >= 0xD800 && character <= 0xDBFF;
}

bool StringUtil::isLowSurrogateCharacter(int32_t character)
{
	return character >= 0xDC00 && character <= 0xDFFF;
}