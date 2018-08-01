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

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

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
		/// Construct a response given a response code
		/// @param[in] responseCode a numerical code for the status of a request
		/// @param[in] responseHeaders a map of key-value pairs containing the response headers and values.
		///
		Response(int32_t responseCode, const ResponseHeaders& responseHeaders);

		///
		/// Destructor
		///
		virtual ~Response() {}

		///
		/// Return a boolean indicating whether this is an erroneous response or not.
		/// @remarks A response is considered to be erroneous, if the getResponseCode() returns >= 400.
		/// @return @c true if this response is erroneous, @c false otherwise.
		///
		bool isErroneousResponse() const;

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

	private:
		
		/// numerical response code
		int32_t mResponseCode;

		/// response headers
		ResponseHeaders mResponseHeaders;
	};
}
#endif