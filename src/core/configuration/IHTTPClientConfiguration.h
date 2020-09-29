/**
 * Copyright 2018-2020 Dynatrace LLC
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

#ifndef _CORE_CONFIGURATION_IHTTPCLIENTCONFIGURATION_H
#define _CORE_CONFIGURATION_IHTTPCLIENTCONFIGURATION_H

#include "OpenKit/ISSLTrustManager.h"
#include "core/UTF8String.h"

#include <cstdint>
#include <memory>

namespace core
{
	namespace configuration
	{
		class IHTTPClientConfiguration
		{
		public:

			virtual ~IHTTPClientConfiguration() = default;

			///
			/// Returns the base URL for the HTTP client
			///
			virtual const core::UTF8String& getBaseURL() const = 0;

			///
			/// Returns the server ID to be used for the HTTP client.
			///
			virtual int32_t getServerID() const = 0;

			///
			/// Returns the application ID for the HTTP client.
			///
			virtual const core::UTF8String& getApplicationID() const = 0;

			///
			/// Returns the trust manager which defines how trust in SSL shall be handled.
			///
			virtual std::shared_ptr<openkit::ISSLTrustManager> getSSLTrustManager() const = 0;
		};
	}
}

#endif
