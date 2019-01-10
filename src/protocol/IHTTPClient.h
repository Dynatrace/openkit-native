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

#ifndef _PROTOCOL_IHTTPCLIENT_H
#define _PROTOCOL_IHTTPCLIENT_H

#include <memory>

#include "protocol/StatusResponse.h"
#include "protocol/TimeSyncResponse.h"
#include "configuration/HTTPClientConfiguration.h"
#include "core/UTF8String.h"

namespace protocol
{
	///
	/// HTTP client which abstracts the 3 basic request types:
	/// - status check
	/// - beacon send
	/// - time sync
	///
	class IHTTPClient
	{
	public:

		///
		/// Destructor
		///
		virtual ~IHTTPClient() {}

		///
		/// sends a status check request and returns a status response
		/// @returns a status response with the response data for the request or @c nullptr on error
		///
		virtual std::shared_ptr<StatusResponse> sendStatusRequest() = 0;

		///
		/// sends a beacon send request and returns a status response
		/// @param[in] clientIPAddress the client IP address
		/// @param[in] beaconData the beacon payload
		/// @returns a status response with the response data for the request or @c nullptr on error
		///
		virtual std::shared_ptr<StatusResponse> sendBeaconRequest(const core::UTF8String& clientIPAddress, const core::UTF8String& beaconData) = 0;

		///
		/// sends a timesync request and returns a timesync response
		/// @returns a timesync response with the response data for the request or @c nullptr on error
		///
		virtual std::shared_ptr<TimeSyncResponse> sendTimeSyncRequest() = 0;

		///
		/// sends a new session request and returns a status response
		/// @returns a status response with the response data for the request or @c nullptr on error
		///
		virtual std::shared_ptr<StatusResponse> sendNewSessionRequest() = 0;
	};
}
#endif