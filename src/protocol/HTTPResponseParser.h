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

#ifndef _PROTOCOL_HTTPRESPONSEPARSER_H
#define _PROTOCOL_HTTPRESPONSEPARSER_H

#include "protocol/http/HttpHeaderCollection.h"

#include <string>
#include <cstdint>

namespace protocol
{
	///
	/// Class responsible for parsing HTTP responses from CURL.
	/// @remarks This class can handle both the response header data and the response body data.
	///
	class HTTPResponseParser
	{
	public:

		///
		/// Construct a HTTPResponseParser instance.
		///
		HTTPResponseParser();

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
		/// Method called when new response body data is ready to be read.
		/// @param[in] buffer The header line to parse.
		/// @param[in] elementSize The size of one element pointed to by buffer.
		/// @param[in] numberOfElements The number of elements in @c buffer.
		/// @return The number of bytes handled.
		///
		size_t responseBodyData(const char* buffer, size_t elementSize, size_t numberOfElements);

		///
		/// Get HTTP response status code or @c -1 if parsing failed.
		///
		int32_t getResponseStatus() const;

		///
		/// Get HTTP response reason phrase or empty string if parsing failed or server did not send it.
		///
		const std::string& getReasonPhrase() const;

		///
		/// Get the response headers.
		///
		const HttpHeaderCollection& getResponseHeaders() const;

		///
		/// Get the response body.
		///
		const std::string& getResponseBody() const;

	private:

		///
		/// Method for resetting the data.
		///
		void reset();

		/// HTTP response status code
		int32_t mResponseStatus;

		/// optional HTTP response reason phrase
		std::string mReasonPhrase;

		/// Response headers
		HttpHeaderCollection mResponseHeaders;

		/// Response body
		std::string mResponseBody;
	};
}

#endif
