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
static const std::regex IPV4_REGEX("^"                                            // start of string
                                   "(25[0-5]|2[0-4]\\d|[0-1]?\\d?\\d)"            // first block - a number from 0-255
                                   "(\\.(25[0-5]|2[0-4]\\d|[0-1]?\\d?\\d)){3}"    // three more blocks - numbers from 0-255 - each prepended by a point character '.'
                                   "$"                                            // end of string
                                   , std::regex::optimize | std::regex::ECMAScript);

bool InetAddressValidator::IsIPv4Address(const core::UTF8String& ipAddress)
{
    std::smatch matches;
    std::regex_match(ipAddress.getStringData(), matches, IPV4_REGEX);

    return !matches.empty();
}

static const std::regex IPV6_STD_REGEX("^"                                        // start of string
                                       "(?:[0-9A-Fa-f]{1,4}:){7}"                 // 7 blocks of a 1 to 4 digit hex number followed by double colon ':'
                                       "[0-9A-Fa-f]{1,4}"                         // one more block of a 1 to 4 digit hex number
                                       "$"                                        // end of string
                                       , std::regex::optimize | std::regex::ECMAScript);

bool InetAddressValidator::IsIPv6StdAddress(const core::UTF8String& ipAddress)
{
    std::smatch matches;
    std::regex_match(ipAddress.getStringData(), matches, IPV6_STD_REGEX);

    return !matches.empty();
}

static const std::regex IPV6_HEX_COMPRESSED_REGEX ("^"                                              // start of string
                                                   "("                                              // 1st group
                                                   "(?:[0-9A-Fa-f]{1,4}"                            // at least one block of a 1 to 4 digit hex number
                                                   "(?::[0-9A-Fa-f]{1,4})*)?"                       // optional further blocks, any number
                                                   ")"
                                                   "::"                                             // in the middle of the expression the two occurences of ':' are neccessary
                                                   "("                                              // 2nd group
                                                   "(?:[0-9A-Fa-f]{1,4}"                            // at least one block of a 1 to 4 digit hex number
                                                   "(?::[0-9A-Fa-f]{1,4})*)?"                       // optional further blocks, any number
                                                   ")"
                                                   "$"                                              // end of string
                                                   , std::regex::optimize | std::regex::ECMAScript);

bool InetAddressValidator::IsIPv6HexCompressedAddress(const core::UTF8String& ipAddress)
{
    std::smatch matches;
    std::regex_match(ipAddress.getStringData(), matches, IPV6_HEX_COMPRESSED_REGEX);

    return !matches.empty();
}

static const std::regex IPV6_MIXED_COMPRESSED_REGEX("^"                            // start of string
                                                    "("                            // 1st group
                                                    "(?:[0-9A-Fa-f]{1,4}"          // at least one block of a 1 to 4 digit hex number
                                                    "(?::[0-9A-Fa-f]{1,4})*)?"     // optional further blocks, any number
                                                    ")"
                                                    "::"                           // in the middle of the expression the two occurences of ':' are neccessary
                                                    "("                            // 2nd group
                                                    "(?:[0-9A-Fa-f]{1,4}:"         // at least one block of a 1 to 4 digit hex number followed by a ':' character
                                                    "(?:[0-9A-Fa-f]{1,4}:)*)?"     // optional further blocks, any number, all succeeded by ':' character
                                                    ")"
                                                    "$"                            // end of string
                                                    , std::regex::optimize | std::regex::ECMAScript);

static const std::regex IPV6_MIXED_NON_COMPRESSED_REGEX("^"                       // start of string
                                                    "(?:[0-9a-fA-F]{1,4}:){6}"    // 6 blocks of a 1 to 4 digit hex number followed by double colon ':'
                                                    "$"                           // end of string
                                                    , std::regex::optimize | std::regex::ECMAScript);

//  IPV6 Mixed mode consists of two parts, the first 96 bits (up to 6 blocks of 4 hex digits) are IPv6
// the IPV6 part can be either compressed or uncompressed
// the second block is a full IPv4 address
// e.g. '0:0:0:0:0:0:172.12.55.18'
bool InetAddressValidator::IsIPv6MixedAddress(const core::UTF8String& ipAddress)
{
    size_t splitIndex = ipAddress.getStringData().find_last_of(':');
    if (splitIndex == std::string::npos)
    {
        return false;
    }

    auto ipv4PartIsValid = IsIPv4Address(ipAddress.substring(splitIndex + 1));
    auto ipV6Part = ipAddress.substring(0, splitIndex + 1);
    if (ipV6Part.equals("::"))
    {
        return ipv4PartIsValid;
    }

    
    std::smatch matchesNonCompressedIPv6;
    std::regex_match(ipV6Part.getStringData(), matchesNonCompressedIPv6, IPV6_MIXED_NON_COMPRESSED_REGEX);

    std::smatch matchesCompressedIPv6;
    std::regex_match(ipV6Part.getStringData(), matchesCompressedIPv6, IPV6_MIXED_COMPRESSED_REGEX);

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