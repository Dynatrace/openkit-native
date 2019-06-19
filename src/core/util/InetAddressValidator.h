/**
* Copyright 2018-2019 Dynatrace LLC
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

#ifndef _CORE_UTIL_INETADDRESSVALIDATOR_H
#define _CORE_UTIL_INETADDRESSVALIDATOR_H

#include "core/UTF8String.h"

namespace core
{
	namespace util
	{
		///
		/// This class provides static methods to check for valid Inet addresses in IPv4, IPv6 or
		/// mixed notation.
		///
		class InetAddressValidator
		{
		public:
			///
			/// Check if input is a valid IPv4 or IPv6 address.
			/// @param[in] ipAddress ip-address to check
			/// @returns @c true if ipAddress is in correct IPv4 or IPv6 notation, else @c false is returned
			///
			static bool IsValidIP(const core::UTF8String& ipAddress);

		private:	
			///
			/// checks if ipAddress is a valid IPv4Address
			/// The format is 'xxx.xxx.xxx.xxx'.Four blocks of integer numbers ranging from 0 to 255
			/// are required.Letters are not allowed.
			/// @param[in] ipAddress ip-address to check
			/// @returns @c true if ipAddress is in correct IPv4 notation, else @c false is returned
			///
			static bool IsIPv4Address(const core::UTF8String& ipAddress);

			///
			/// checks if ipAddress is a valid  IPv6Address
			/// Possible notations for valid IPv6 are :
			/// Standard IPv6 address
			///	 -Hex - compressed IPv6 address
			///	 -Link - local IPv6 address
			///	 -IPv4 - mapped - to - IPV6 address
			///	 -IPv6 mixed address
			/// @param[in] ipAddress ip-address to check
			/// @returns @c true if ipAddress is in correct IPv6 notation, else @c false is returned
			///
			static bool IsIPv6Address(const core::UTF8String& ipAddress);

			///
			///  Check if the given address is a valid IPv6 address in the standard format
			/// The format is 'xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx'.Eight blocks of hexadecimal digits
			/// are required.
			/// @param[in] ipAddress ip-address to check
			/// @returns @c true if ipAddress is in correct IPv4 notation, else @c false is returned
			///
			static bool IsIPv6StdAddress(const core::UTF8String& ipAddress);

			///
			/// Check if the given address is a valid IPv6 address in the hex-compressed notation
			/// The format is 'xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx'.If all digits in a block are '0'
			/// the block can be left empty.
			/// @param[in] ipAddress ip-address to check
			/// @returns @c true if ipAddress is in correct IPv6 standard notation, else @c false is returned
			///
			static bool IsIPv6HexCompressedAddress(const core::UTF8String& ipAddress);

			///
			/// Check if the given address is a valid IPv6 address in the mixed-standard or mixed-compressed notation.
			/// @returns @c true if ipAddress is in correct IPv6 (mixed-standard or mixed-compressed) notation, else @c false is returned
			///
			static bool IsIPv6MixedAddress(const core::UTF8String& ipAddress);

			///
			/// Check if the given address is a link local IPv6 address starting with "fe80:" and containing
			/// a zone index with "%xxx".The zone index will not be checked.
			/// @param[in] ipAddress ip-address to check
			/// @returns @c true if ipAddress is in correct IPv6 notation with zone index, else @c false is returned
			///
			static bool IsLinkLocalIPv6WithZoneIndex(const core::UTF8String& ipAddress);
		};
	}
}
#endif