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

#ifndef _PROTOCOL_TIMESYNCRESPONSE_H
#define _PROTOCOL_TIMESYNCRESPONSE_H

#include "Response.h"
#include "core/UTF8String.h"

namespace protocol
{
	///
	/// Implements a timesync response which is sent for time sync requests.
	/// A timesync response encapsulates settings contained in the timesync response
	///
	class TimeSyncResponse : public Response
	{
	public:

		///
		/// Construct a timesync response using a response code and string
		/// @param[in] response the response string obtained from the server
		/// @param[in] responseCode a numerical response code
		/// @param[in] responseHeaders HTTP response headers
		///
		TimeSyncResponse(const core::UTF8String& response, int32_t responseCode, const Response::ResponseHeaders& responseHeaders);

	public:

		///
		/// Get the receive timestamp contained in the time sync request
		/// @returns the receive timestamp contained in the time sync request
		///
		int64_t getRequestReceiveTime() const;

		///
		/// Get the send timestamp contained in the time sync response
		/// @returns the send timestamp contained in the time sync response
		///
		int64_t getResponseSendTime() const;

	private:

		///
		/// Parse the response string for further details
		/// @param[in] response the response string obtained from the server
		///
		void parseResponse(const core::UTF8String& response);

	private:

		/// receive timestamp contained in the time sync request
		int64_t mRequestReceiveTime;

		/// send timestamp contained in the time sync response
		int64_t mResponseSendTime;
	};
}

#endif