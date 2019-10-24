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
			const core::UTF8String& response,
			int32_t responseCode,
			const ResponseHeaders& responseHeaders
		);

		///
		/// Destructor
		///
		virtual ~StatusResponse() {}

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
		virtual int32_t getResponseCode() const override;

		///
		/// Return the HTTP response headers
		/// @returns the response headers
		///
		const IStatusResponse::ResponseHeaders& getResponseHeaders() const override;

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
		/// Get a flag if capturing is enabled by the cluster
		/// @returns @c true if capturing is enabled, @c false is capturing is not enabled
		///
		bool isCapture() const override;

		///
		/// Get the send interval
		/// @returns send interval in seconds
		///
		int32_t getSendInterval() const override;

		///
		/// Get the monitor name
		/// @returns the monitor name
		///
		const core::UTF8String& getMonitorName() const override;

		///
		/// Get the server id
		/// @returns the server id
		///
		int32_t getServerID() const override;

		///
		/// Get the maximum beacon size
		/// @returns the maximum beacon size in kilobytes
		///
		int32_t getMaxBeaconSize() const override;

		///
		/// Get a flag if errors should be reported
		/// @returns @c true if errors are reported to the cluster, @c false if errors are not reported
		///
		bool isCaptureErrors() const override;

		///
		/// Get a flag if crashes should be reported
		/// @returns @c true if errors are reported to the cluster, @c false if errors are not reported
		///
		bool isCaptureCrashes() const override;

		///
		/// Get the multiplicity
		/// @returns the multiplicity factor
		///
		int32_t getMultiplicity() const override;

	private:
		///
		/// Parse the response string for further details
		/// @param[in] response the response string obtained from the server
		///
		void parseResponse(const core::UTF8String& response);
	private:

		/// Logger to write traces to
		std::shared_ptr<openkit::ILogger> mLogger;

		/// numerical response code
		int32_t mResponseCode;

		/// response headers
		ResponseHeaders mResponseHeaders;

		/// capture on/off
		bool mCapture;

		/// send interval
		int32_t mSendInterval;

		/// monitor name
		core::UTF8String mMonitorName;

		/// server id
		int32_t mServerID;

		/// maximum beacon size
		int32_t mMaxBeaconSize;

		/// capture errors
		bool mCaptureErrors;

		/// capture crashes
		bool mCaptureCrashes;

		/// multiplicity
		int32_t mMultiplicity;
	};
}

#endif