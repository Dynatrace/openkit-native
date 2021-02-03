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

#ifndef _PROTOCOL_STATUSRESPONSE_H
#define _PROTOCOL_STATUSRESPONSE_H

#include "IStatusResponse.h"
#include "core/UTF8String.h"
#include "OpenKit/ILogger.h"

#include <memory>

namespace protocol
{
	///
	/// Implements a status response which is sent for the request types status check and beacon send.
	/// A status response encapsulates settings contained in the status response
	///
	class StatusResponse
		: public IStatusResponse
	{
	public:

		/// Response code sent by HTTP server to indicate success.
		static constexpr int32_t HTTP_OK = 200;
		
		/// Value that is sent if response status indicates an error.
		/// The status is part of the payload, see class ResponseAttributes.
		static const core::UTF8String RESPONSE_STATUS_ERROR;
		
		///
		/// Creates a success StatusResponse.
		/// @param[in] logger The logger to write traces to
		/// @param[in] responseAttributes the response attributes
		/// @param[in] responseCode a numerical response code
		/// @param[in] responseHeaders HTTP response headers
		///
		static std::shared_ptr<StatusResponse> createSuccessResponse(
			std::shared_ptr<openkit::ILogger> logger,
			std::shared_ptr<IResponseAttributes> responseAttributes,
			int32_t responseCode,
			const ResponseHeaders& responseHeaders);

		///
		/// Creates an erroneous StatusResponse.
		/// @param[in] logger The logger to write traces to
		/// @param[in] responseCode a numerical response code
		///
		static std::shared_ptr<StatusResponse> createErrorResponse(
			std::shared_ptr<openkit::ILogger> logger,
			int32_t responseCode
		);

		///
		/// Creates an erroneous StatusResponse.
		/// @param[in] logger The logger to write traces to
		/// @param[in] responseCode a numerical response code
		/// @param[in] responseHeaders HTTP response headers
		///
		static std::shared_ptr<StatusResponse> createErrorResponse(
			std::shared_ptr<openkit::ILogger> logger,
			int32_t responseCode,
			const ResponseHeaders& responseHeaders
		);

		///
		/// Return a boolean indicating whether this is an erroneous response or not.
		/// @remarks A response is considered to be erroneous, if the getResponseCode() returns >= 400.
		/// @return @c true if this response is erroneous, @c false otherwise.
		///
		bool isErroneousResponse() const override;

		///
		/// Return a boolean indicating whether this is a "too many requests" response or not.
		/// @remarks A response is a "too many requests" response, if the getResponseCode() returns @c 429.
		/// @return @c true if this response is a "too many requests" response, @c false otherwise.
		///
		bool isTooManyRequestsResponse() const override;

		///
		/// Return the response code
		/// @returns the response code
		///
		int32_t getResponseCode() const override;

		///
		/// Return the HTTP response headers
		/// @returns the response headers
		///
		const ResponseHeaders& getResponseHeaders() const override;

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
		int64_t getRetryAfterInMilliseconds() const override;

		///
		/// Return the response attributes
		///
		std::shared_ptr<IResponseAttributes> getResponseAttributes() const override;

	private:

		///
		/// Construct a status response using a response code and string
		/// @param[in] logger The logger to write traces to
		/// @param[in] response the response string obtained from the server
		/// @param[in] responseCode a numerical response code
		/// @param[in] responseHeaders HTTP response headers
		///
		StatusResponse
		(
			std::shared_ptr<openkit::ILogger> logger,
			std::shared_ptr<IResponseAttributes> responseAttributes,
			int32_t responseCode,
			const ResponseHeaders& responseHeaders
		);

	private:

		/// Logger to write traces to
		std::shared_ptr<openkit::ILogger> mLogger;

		/// response attributes
		std::shared_ptr<IResponseAttributes> mResponseAttributes;

		/// numerical response code
		int32_t mResponseCode;

		/// response headers
		ResponseHeaders mResponseHeaders;
	};
}

#endif