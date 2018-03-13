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

const std::unordered_set<unsigned char> URLEncoding::sUnreservedCharactersRFC3986 = std::unordered_set<unsigned char>({ 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
																					'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i',
																					'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1',
																					'2', '3', '4', '5', '6', '7', '8', '9', '-', '_', '.', '~' });

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
			if (stringData.end() - (it + 1) >= 2)//check if there is enough data for the current percent sign
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