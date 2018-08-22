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

#ifndef _COMMUNICATION_BEACONSENDINGRESPONSEUTIL_H
#define _COMMUNICATION_BEACONSENDINGRESPONSEUTIL_H

#include "protocol/Response.h"

#include <memory>

namespace communication
{
	///
	/// Utility class for responses.
	///
	class BeaconSendingResponseUtil
	{
	public:

		///
		/// Test if the given Response is a successful response.
		/// @param response The given response to check whether it is successful or not.
		/// @return @c true if response is successful, @c false otherwise.
		///
		static bool isSuccessfulResponse(std::shared_ptr<protocol::Response> response);

		///
		/// Test if the given {@link Response} is a "too many requests" response.
		/// @remarks A "too many requests" response is an HTTP response with response code 429.
		/// @param response The given response to check whether it is a "too many requests" response or not.
		/// @return @c true if response indicates too many requests, @c false otherwise.
		///
		static bool isTooManyRequestsResponse(std::shared_ptr<protocol::Response> response);

	private:

		///
		/// Default constructor.
		/// @remarks This constructor is private since the class is handled as static class.
		///
		BeaconSendingResponseUtil() = delete;
	};
}

#endif
