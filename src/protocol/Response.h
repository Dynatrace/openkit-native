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

#ifndef _PROTOCOL_RESPONSE_H
#define _PROTOCOL_RESPONSE_H

#include "OpenKit/ILogger.h"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace protocol
{
	///
	/// Abstract base class for a response to one of the 3 request types(status check, beacon send, time sync).
	///
	class Response
	{
	public:

		/// Alias for HTTP response headers
		using ResponseHeaders = std::unordered_map<std::string, std::vector<std::string>>;

		///
		/// Destructor
		///
		virtual ~Response() {}

		///
		/// Return a boolean indicating whether this is a successful response or not.
		/// @remarks A response is considered to be successful, if the getResponseCode() returns < 400.
		/// @return @c true if this response is successful, @c false otherwise.
		///
		bool isSuccessfulResponse() const;
		///
		/// Return a boolean indicating whether this is an erroneous response or not.
		/// @remarks A response is considered to be erroneous, if the getResponseCode() returns >= 400.
		/// @return @c true if this response is erroneous, @c false otherwise.
		///
		bool isErroneousResponse() const;

		///
		/// Return a boolean indicating whether this is a "too many requests" response or not.
		/// @remarks A response is a "too many requests" response, if the getResponseCode() returns @c 429.
		/// @return @c true if this response is a "too many requests" response, @c false otherwise.
		///
		bool isTooManyRequestsResponse() const;

		///
		/// Return the response code
		/// @returns the response code
		///
		virtual int32_t getResponseCode() const;

		///
		/// Return the HTTP response headers
		/// @returns the response headers
		///
		const ResponseHeaders& getResponseHeaders() const;

		///
		/// Get Retry-After response header value in milliseconds.
		///
		/// @remarks According to RFC 7231 Section 7.1.3 (https://tools.ietf.org/html/rfc7231#section-7.1.3)
		/// Retry-After response header's value can either be an HTTP date, or a positive integer representing
		/// a delay in seconds.
		/// Currently this method only parses the delay in seconds and returns the value in milliseoncs.
		/// If any parsing error occurs, it's logged and a default value of 10 minutes (expressed in milliseconds)
		/// is returned.
		///
		/// @return Returns the parsed Retry-After response header value or the default value in case of parsing errors.
		///
		int64_t getRetryAfterInMilliseconds() const;

	protected:

		///
		/// Construct a response given a response code
		/// @remarks This constructor is intentionally protected, to make the class abstract.
		/// @param[in] responseCode a numerical code for the status of a request
		/// @param[in] responseHeaders a map of key-value pairs containing the response headers and values.
		///
		Response(std::shared_ptr<openkit::ILogger> logger, int32_t responseCode, const ResponseHeaders& responseHeaders);

	private:

		/// Logger to write traces to
		std::shared_ptr<openkit::ILogger> mLogger;
		
		/// numerical response code
		int32_t mResponseCode;

		/// response headers
		ResponseHeaders mResponseHeaders;
	};
}
#endif