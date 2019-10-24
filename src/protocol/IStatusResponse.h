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

#ifndef _PROTOCOL_ISTATUSRESPONSE_H
#define _PROTOCOL_ISTATUSRESPONSE_H

#include "core/UTF8String.h"

#include <unordered_map>
#include <vector>

namespace protocol
{
	class IStatusResponse
	{
	public:

		/// Alias for HTTP response headers
		using ResponseHeaders = std::unordered_map<std::string, std::vector<std::string>>;

		///
		/// Destructor
		///
		virtual ~IStatusResponse() {}

		///
		/// Get a flag if capturing is enabled by the cluster
		/// @returns @c true if capturing is enabled, @c false is capturing is not enabled
		///
		virtual bool isCapture() const = 0;

		///
		/// Get the send interval
		/// @returns send interval in seconds
		///
		virtual int32_t getSendInterval() const = 0;

		///
		/// Get the monitor name
		/// @returns the monitor name
		///
		virtual const core::UTF8String& getMonitorName() const = 0;

		///
		/// Get the server id
		/// @returns the server id
		///
		virtual int32_t getServerID() const = 0;

		///
		/// Get the maximum beacon size
		/// @returns the maximum beacon size in kilobytes
		///
		virtual int32_t getMaxBeaconSize() const = 0;

		///
		/// Get a flag if errors should be reported
		/// @returns @c true if errors are reported to the cluster, @c false if errors are not reported
		///
		virtual bool isCaptureErrors() const = 0;

		///
		/// Get a flag if crashes should be reported
		/// @returns @c true if errors are reported to the cluster, @c false if errors are not reported
		///
		virtual bool isCaptureCrashes() const = 0;

		///
		/// Get the multiplicity
		/// @returns the multiplicity factor
		///
		virtual int32_t getMultiplicity() const = 0;

		///
		/// Return a boolean indicating whether this is an erroneous response or not.
		/// @remarks A response is considered to be erroneous, if the getResponseCode() returns >= 400.
		/// @return @c true if this response is erroneous, @c false otherwise.
		///
		virtual bool isErroneousResponse() const = 0;

		///
		/// Return a boolean indicating whether this is a "too many requests" response or not.
		/// @remarks A response is a "too many requests" response, if the getResponseCode() returns @c 429.
		/// @return @c true if this response is a "too many requests" response, @c false otherwise.
		///
		virtual bool isTooManyRequestsResponse() const = 0;

		///
		/// Return the response code
		/// @returns the response code
		///
		virtual int32_t getResponseCode() const = 0;

		///
		/// Return the HTTP response headers
		/// @returns the response headers
		///
		virtual const ResponseHeaders& getResponseHeaders() const = 0;

		///
		/// Get Retry-After response header value in milliseconds.
		///
		/// @remarks According to RFC 7231 Section 7.1.3 (https://tools.ietf.org/html/rfc7231#section-7.1.3)
		/// Retry-After response header's value can either be an HTTP date, or a positive integer representing
		/// a delay in seconds.
		/// Currently this method only parses the delay in seconds and returns the value in milliseconds.
		/// If any parsing error occurs, it's logged and a default value of 10 minutes (expressed in milliseconds)
		/// is returned.
		///
		/// @return Returns the parsed Retry-After response header value or the default value in case of parsing errors.
		///
		virtual int64_t getRetryAfterInMilliseconds() const = 0;
	};
}

#endif
