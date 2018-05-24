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

#include "InetAddressValidator.h"

#include <regex>
#include <string>

using namespace core::util;


bool InetAddressValidator::IsValidIP(const core::UTF8String& ipAddress)
{
	return IsIPv4Address(ipAddress) || IsIPv6Address(ipAddress);
}

bool InetAddressValidator::IsIPv6Address(const core::UTF8String& ipAddress)
{
	return IsIPv6StdAddress(ipAddress)
		|| IsIPv6HexCompressedAddress(ipAddress)
		|| IsIPv6MixedAddress(ipAddress)
		|| IsLinkLocalIPv6WithZoneIndex(ipAddress);
}
static const std::regex ipv4Regex("^"											// start of string
							      "(25[0-5]|2[0-4]\\d|[0-1]?\\d?\\d)"			// first block - a number from 0-255
								  "(\\.(25[0-5]|2[0-4]\\d|[0-1]?\\d?\\d)){3}"	// three more blocks - numbers from 0-255 - each prepended by a point character '.'
								  "$"											// end of string
	, std::regex::optimize | std::regex::ECMAScript);

bool InetAddressValidator::IsIPv4Address(const core::UTF8String& ipAddress)
{
	std::smatch matches;
	std::regex_match(ipAddress.getStringData(), matches, ipv4Regex);

	return !matches.empty();
}

static const std::regex ipv6StdRegex("^"										// start of string
									 "(?:[0-9A-Fa-f]{1,4}:){7}"					// 7 blocks of a 1 to 4 digit hex number followed by double colon ':'
									 "[0-9A-Fa-f]{1,4}"							// one more block of a 1 to 4 digit hex number
									 "$"										// end of string
	, std::regex::optimize | std::regex::ECMAScript);

bool InetAddressValidator::IsIPv6StdAddress(const core::UTF8String& ipAddress)
{
	std::smatch matches;
	std::regex_match(ipAddress.getStringData(), matches, ipv6StdRegex);

	return !matches.empty();
}

static const std::regex ipv6HexCompressedRegex("^"												// start of string
											   "("												// 1st group
											   "(?:[0-9A-Fa-f]{1,4}"							// at least one block of a 1 to 4 digit hex number
											   "(?::[0-9A-Fa-f]{1,4})*)?"						// optinional further blocks, any number
											   ")"
											   "::"												// in the middle of the expression the two occurences of ':' are neccessary
											   "("												// 2nd group
											   "(?:[0-9A-Fa-f]{1,4}"							// at least one block of a 1 to 4 digit hex number
											   "(?::[0-9A-Fa-f]{1,4})*)?"						// optinional further blocks, any number
											   ")"
											   "$"												// end of string
	, std::regex::optimize | std::regex::ECMAScript);

bool InetAddressValidator::IsIPv6HexCompressedAddress(const core::UTF8String& ipAddress)
{
	std::smatch matches;
	std::regex_match(ipAddress.getStringData(), matches, ipv6HexCompressedRegex);

	return !matches.empty();
}

static const std::regex ipv6MixedCompressedRegex("^"							// start of string
												"("                             // 1st group
												"(?:[0-9A-Fa-f]{1,4}"           // at least one block of a 1 to 4 digit hex number
												"(?::[0-9A-Fa-f]{1,4})*)?"      // optional further blocks, any number
												")"
												"::"                            // in the middle of the expression the two occurences of ':' are neccessary
												"("                             // 2nd group
												"(?:[0-9A-Fa-f]{1,4}:"          // at least one block of a 1 to 4 digit hex number followed by a ':' character
												"(?:[0-9A-Fa-f]{1,4}:)*)?"      // optional further blocks, any number, all succeeded by ':' character
												")"
												"$"     						// end of string
												, std::regex::optimize | std::regex::ECMAScript);

static const std::regex ipv6MixedNonCompressedRegex("^"							  // start of string
													"(?:[0-9a-fA-F]{1,4}:){6}"    // 6 blocks of a 1 to 4 digit hex number followed by double colon ':'
													"$"  						  // end of string
													, std::regex::optimize | std::regex::ECMAScript);

bool InetAddressValidator::IsIPv6MixedAddress(const core::UTF8String& ipAddress)
{
	size_t splitIndex = std::string::npos;
	const char splitCharacter = ':';
	uint32_t runs = 0;

	while (runs < 1 ||  splitIndex < ipAddress.getStringLength())
	{
		auto characterFoundIndex = ipAddress.getIndexOf(&splitCharacter, runs==0 ? 0 : splitIndex + 1);
		if (characterFoundIndex != std::string::npos)
		{
			splitIndex = characterFoundIndex;
		}
		else
		{
			break;
		}
		runs++;
	}

	if (splitIndex == std::string::npos)
	{
		return false;
	}

	auto ipv4PartIsValid = IsIPv4Address(ipAddress.substring(splitIndex + 1));
	core::UTF8String ipV6Part = ipAddress.substring(0, splitIndex + 1);
	if (ipV6Part.equals("::"))
	{
		return ipv4PartIsValid;
	}

	
	std::smatch matchesNonCompressedIPv6;
	std::regex_match(ipV6Part.getStringData(), matchesNonCompressedIPv6, ipv6MixedNonCompressedRegex);

	std::smatch matchesCompressedIPv6;
	std::regex_match(ipV6Part.getStringData(), matchesCompressedIPv6, ipv6MixedCompressedRegex);

	return ipv4PartIsValid && (!matchesNonCompressedIPv6.empty() || !matchesCompressedIPv6.empty());
}

bool InetAddressValidator::IsLinkLocalIPv6WithZoneIndex(const core::UTF8String& ipAddress)
{
	size_t positionOfZoneIndex = ipAddress.getIndexOf("%", 5);
	if (ipAddress.getStringLength() > 5
		&& positionOfZoneIndex != std::string::npos && positionOfZoneIndex < ipAddress.getStringLength() -1 )
	{
		core::UTF8String ipPart = ipAddress.substring(0, positionOfZoneIndex);
		return IsIPv6StdAddress(ipPart) || IsIPv6HexCompressedAddress(ipPart);
	}
	return false;
}