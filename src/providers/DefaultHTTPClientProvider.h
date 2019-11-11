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

#ifndef _PROVIDERS_DEFAULTHTTPCLIENTPROVIDER_H
#define _PROVIDERS_DEFAULTHTTPCLIENTPROVIDER_H

#include "core/configuration/IHTTPClientConfiguration.h"
#include "providers/IHTTPClientProvider.h"

namespace providers
{
	///
	/// Implementation of an HTTPClientProvider which creates a HTTP client for executing status check and beacon send requests.
	///
	class DefaultHTTPClientProvider : public IHTTPClientProvider
	{
	public:

		~DefaultHTTPClientProvider() override = default;

		std::shared_ptr<protocol::IHTTPClient> createClient(
			std::shared_ptr<openkit::ILogger> logger,
			std::shared_ptr<core::configuration::IHTTPClientConfiguration> configuration
		) override;
	};
}

#endif
