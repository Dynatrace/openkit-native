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

#ifndef _PROTOCOL_HTTPRESPONSEPARSER_H
#define _PROTOCOL_HTTPRESPONSEPARSER_H

#include "Response.h"

#include <string>
#include <vector>

namespace protocol
{
	///
	/// Class responsible for parsing HTTP responses.
	/// @remarks This class can handle both the response header data and the response body data.
	///
	class HTTPResponseParser
	{
	public:

		///
		/// Method called when new header line is ready to be parsed.
		/// @remarks The deprecated header folding (having one long header split up into multiple lines)
		///          is not supported.
		///          Furthermore do not assume that @c buffer is 0-terminated.
		/// @param[in] buffer The header line to parse.
		/// @param[in] elementSize The size of one element pointed to by buffer.
		/// @param[in] numberOfElements The number of elements in @c buffer.
		/// @return The number of bytes handled.
		///
		size_t responseHeaderData(const char *buffer, size_t elementSize, size_t numberOfElements);

		///
		/// Method called when new response data is ready to be read.
		/// @param[in] buffer The header line to parse.
		/// @param[in] elementSize The size of one element pointed to by buffer.
		/// @param[in] numberOfElements The number of elements in @c buffer.
		/// @return The number of bytes handled.
		///
		size_t responseBodyData(const char* buffer, size_t elementSize, size_t numberOfElements);

		///
		/// Get the response headers.
		/// @return Response headers parsed so far.
		/// 
		const Response::ResponseHeaders& getResponseHeaders() const;

		///
		/// Get the response body.
		/// @return Response body received so far.
		///
		const std::string& getResponseBody() const;

	private:

		///
		/// Gives a boolean indicating whether given character is a whitespace as defined in RFC 7230 section 3.2.2 Whitespace.
		/// @remarks Since HTTP defines only SP (space) and HTAB as optional whitespace, plus
		///          CR (carriage return) and LF (line feed) as line delimiters, only those
		///          four characters are considered as whitespace.
		/// @param[in] c The character to check if it is a whitespace character or not.
		/// @return @c true If given character @c is a whitespace character, @c false otherwise.
		///
		static bool isWhitespace(char c);

		///
		/// Strip leading and trailing whitespace characters from the string.
		/// @param[in,out] The string to strip.
		///
		static void stripWhitespaces(std::string& str);

		/// Response headers
		Response::ResponseHeaders mResponseHeaders;

		/// Response body
		std::string mResponseBody;
	};
}

#endif
