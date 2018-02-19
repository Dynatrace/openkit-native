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

#ifndef _PROTOCOL_HTTPCLIENT_H
#define _PROTOCOL_HTTPCLIENT_H

#include <memory>

#include "protocol/StatusResponse.h"
#include "configuration/HTTPClientConfiguration.h"

namespace protocol {
	///
	/// HTTP client helper which abstracts the 3 basic request types:
	/// - status check
	/// - beacon send
	/// - time sync
	///
	class HTTPClient
	{
	public:
		///
		/// Default constructor
		/// @param[in] configuration configuration parameters for the HTTPClient
		///
		HTTPClient(std::shared_ptr<configuration::HTTPClientConfiguration> configuration);

		///
		/// sends a status check request and returns a status response
		/// @returns a status response with the response data for the request
		///
		virtual std::unique_ptr<StatusResponse> sendStatusRequest();

		virtual ~HTTPClient() {};
	};
}

#endif