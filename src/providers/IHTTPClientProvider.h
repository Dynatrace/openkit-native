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

#ifndef _PROVIDERS_IHTTPCLIENTPROVIDER_H
#define _PROVIDERS_IHTTPCLIENTPROVIDER_H

#include "protocol/HTTPClient.h"

#include <memory>

#include "OpenKit/ILogger.h"
#include "configuration/HTTPClientConfiguration.h"

namespace providers
{
	///
	/// Interface for providing an HTTP client. Mostly needed for testing purposes.
	///
	class IHTTPClientProvider
	{
	public:

		///
		/// Destructor
		///
		virtual ~IHTTPClientProvider() {}

		///
		/// Returns an HTTPClient based on the provided configuration.
		/// @param[in] logger to write traces to
		/// @param[in] configuration configuration parameters for the HTTP connection
		///
		virtual std::shared_ptr<protocol::IHTTPClient> createClient(std::shared_ptr<openkit::ILogger> logger, std::shared_ptr<configuration::HTTPClientConfiguration> configuration) = 0;
	};
}

#endif