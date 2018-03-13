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

#include "URLEncoding.h"

#include <sstream>
#include <iomanip>
#include <cctype>
#include <cstdint>

using namespace core::util;

constexpr char unreserved[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_.~";
const std::unordered_set<unsigned char> URLEncoding::sUnreservedCharactersRFC3986 = std::unordered_set<unsigned char>(unreserved, unreserved + sizeof(unreserved)/sizeof(unsigned char));

static std::string escapeHexValueInPercentEncoding(unsigned char c)
{
	std::stringstream ss;
	ss << "%" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << static_cast<int32_t>(c);
	return ss.str();
}

 core::UTF8String URLEncoding::urlencode(const core::UTF8String& string)
 {
	std::string encoded;
	encoded.reserve(string.getStringLength());

	auto stringData = string.getStringData();

	for (auto it = stringData.begin(); it < stringData.end(); it++)
	{
		auto character = *it;
		if (sUnreservedCharactersRFC3986.find(character) !=sUnreservedCharactersRFC3986.end()) //character is in the list of unreserved characters -> copy
		{
			encoded += character;
		}
		else // character must be escaped
		{
			auto escaped = escapeHexValueInPercentEncoding(character);
			encoded += escaped.c_str();
		}
	}

	return encoded;
}



core::UTF8String URLEncoding::urldecode(const core::UTF8String& string)
{
	std::string decoded;
	decoded.reserve(string.getStringLength());

	auto stringData = string.getStringData();
	for (auto it = stringData.begin(); it < stringData.end(); it++)
	{
		auto character = *it;
		if (character != '%')//character is in the list of unreserved characters -> copy
		{
			decoded += character;
		}
		else // character must be 'un'-escaped, the two next characters are hex-encoded byte
		{
			if (static_cast<size_t>(it - stringData.begin()) < stringData.length() - 2)//check if there is enough data for the current percent sign
			{
				std::string bytes(it + 1, it + 3);

				if (std::isxdigit(bytes[0]) && std::isxdigit(bytes[1]))
				{
					int32_t codepoint = stoi(bytes, nullptr, 16);
					decoded += static_cast<int8_t>(codepoint);
				}
				else
				{
					decoded += "?";
					decoded += bytes;
				}

				it += 2;
			}
			else
			{
				break;
			}
		}
	}

	return UTF8String(decoded);
}