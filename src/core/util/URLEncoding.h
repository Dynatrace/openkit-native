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

#ifndef _CORE_UTIL_URLENCODING_H
#define _CORE_UTIL_URLENCODING_H

#include "core/UTF8String.h"

#include <unordered_set>

namespace core
{
	namespace util
	{
		///
		/// This class is used to escape reserved characters in URL strings that can contain
		/// characters that are not valid in the context of an URL
		///
		class URLEncoding
		{
		public:
			///
			/// URL-Encode the given string
			/// @returns url-encoded version of the current string
			///
			static core::UTF8String urlencode(const core::UTF8String& string);

			///
			/// URL-Encode the given string, taking additional characters into account
			/// which are treated as reserved characters.
			///
			/// @param string The string to encode.
			/// @param additionalReservedCharacters Additional characters to consider as reserved.
			/// @returns url-encoded version of the current string
			///
			static core::UTF8String urlencode(const core::UTF8String& string,
											  const std::unordered_set<char>& additionalReservedCharacters);

			///
			/// URL-Decode the given string
			/// @returns url-decoded version of the current string
			///
			static core::UTF8String urldecode(const core::UTF8String& string);
		private:
			/// list of characters that don't need escaping
			static const std::unordered_set<unsigned char> sUnreservedCharactersRFC3986;
		};
	}
}

#endif