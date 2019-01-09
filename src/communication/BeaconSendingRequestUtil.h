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

#ifndef _COMMUNICATION_BEACONSENDINGREQUESTUTIL_H
#define _COMMUNICATION_BEACONSENDINGREQUESTUTIL_H

#include <memory>

#include "BeaconSendingContext.h"
#include  "protocol/StatusResponse.h"

namespace communication
{
	///
	/// Utilities to be called as static methods to do actual requests
	///
	class BeaconSendingRequestUtil
	{
	public:

		///
		/// Send a status request and capture the response
		/// @param[in] context the BeaconSendingContext containing HTTPClient and configuration
		/// @param[in] numRetries number of retries when failing
		/// @param[in] initialRetryDelayInMillis if retries are necesarry this in the initial time to sleep between two retries, it is doubled every time
		///
		static std::shared_ptr<protocol::StatusResponse> sendStatusRequest(BeaconSendingContext& context, uint32_t numRetries, uint64_t initialRetryDelayInMillis);

	private:

		///
		/// Default constructor.
		/// @remarks This constructor is removed, since this class is used as static utility class.
		///
		BeaconSendingRequestUtil() = delete;
	};
}

#endif