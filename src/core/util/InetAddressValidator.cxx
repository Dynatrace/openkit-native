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

static const std::regex ipv4Regex(R"(^(25[0-5]|2[0-4]\d|[0-1]?\d?\d)(\.(25[0-5]|2[0-4]\\d|[0-1]?\d?\d)){3}$)"
	, std::regex::optimize | std::regex::ECMAScript);

bool InetAddressValidator::IsIPv4Address(const core::UTF8String& ipAddress)
{
	std::smatch matches;
	std::regex_match(ipAddress.getStringData(), matches, ipv4Regex);

	return !matches.empty();
}

static const std::regex ipv6StdRegex("^(?:[0-9A-Fa-f]{1,4}:){7}[0-9A-Fa-f]{1,4}$"
	, std::regex::optimize | std::regex::ECMAScript);

bool InetAddressValidator::IsIPv6StdAddress(const core::UTF8String& ipAddress)
{
	std::smatch matches;
	std::regex_match(ipAddress.getStringData(), matches, ipv6StdRegex);

	return !matches.empty();
}

static const std::regex ipv6HexCompressedRegex("^((?:[0-9A-Fa-f]{1,4}(?::[0-9A-Fa-f]{1,4})*)?)::((?:[0-9A-Fa-f]{1,4}(?::[0-9A-Fa-f]{1,4})*)?)$"
	, std::regex::optimize | std::regex::ECMAScript);

bool InetAddressValidator::IsIPv6HexCompressedAddress(const core::UTF8String& ipAddress)
{
	std::smatch matches;
	std::regex_match(ipAddress.getStringData(), matches, ipv6HexCompressedRegex);

	return !matches.empty();
}

static const std::regex ipv6MixedStdOrCompressedRegex(
	"^"																// Anchor address
	"(?:"
	"(?:[A-Fa-f0-9]{1,4}:){6}"											// Non-compressed
	"|(?=(?:[A-Fa-f0-9]{0,4}:){2,6}"									// Compressed with 2 to 6 colons
	R"((?:[0-9]{1,3}\.){3}[0-9]{1,3})"								// and 4 bytes
	R"((?![:.\w])))"												//  and anchored
	"(([0-9A-Fa-f]{1,4}:){1,5}|:)((:[0-9A-Fa-f]{1,4}){1,5}:|:)"			//  and at most 1 double colon
	"|::(?:[A-Fa-f0-9]{1,4}:){5}"										// Compressed with 7 colons and 5 numbers
	")"
	R"((?:(?:25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])\.){3})"	// 255.255.255.
	"(?:25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])"				// 255
	"$"																// Anchor address
	, std::regex::optimize | std::regex::ECMAScript
);											

bool InetAddressValidator::IsIPv6MixedAddress(const core::UTF8String& ipAddress)
{
	std::smatch matches;
	std::regex_match(ipAddress.getStringData(), matches, ipv6MixedStdOrCompressedRegex);

	return !matches.empty();
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