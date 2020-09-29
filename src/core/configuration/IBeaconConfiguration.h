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

#ifndef _CORE_CONFIGURATION_IBEACONCONFIGURATION_H
#define _CORE_CONFIGURATION_IBEACONCONFIGURATION_H

#include "OpenKit/CrashReportingLevel.h"
#include "OpenKit/DataCollectionLevel.h"
#include "core/configuration/IHTTPClientConfiguration.h"
#include "core/configuration/IOpenKitConfiguration.h"
#include "core/configuration/IPrivacyConfiguration.h"
#include "core/configuration/IServerConfiguration.h"

#include <cstdint>
#include <functional>

namespace core
{
	namespace configuration
	{
		///
		/// Specifies a callback which will be invoked when the server configuration is updated.
		///
		using ServerConfigurationUpdateCallback = std::function<void(std::shared_ptr<IServerConfiguration>)>;

		///
		/// Provides configuration for the beacon.
		///
		class IBeaconConfiguration
		{
		public:

			virtual ~IBeaconConfiguration() = default;

			///
			/// Returns the object holding application relevant configuration details.
			///
			virtual std::shared_ptr<IOpenKitConfiguration> getOpenKitConfiguration() const = 0;

			///
			/// Returns the object holding privacy relevant configuration details.
			///
			virtual std::shared_ptr<IPrivacyConfiguration> getPrivacyConfiguration() const = 0;

			///
			/// Returns the object holding HTTP related configuration details.
			///
			virtual std::shared_ptr<IHTTPClientConfiguration> getHTTPClientConfiguration() const = 0;

			///
			/// Returns the object holding server relevant configuration details.
			///
			virtual std::shared_ptr<IServerConfiguration> getServerConfiguration() = 0;

			///
			/// Enables the capturing and implicitly sets @ref isServerConfigurationSet()
			///
			virtual void enableCapture() = 0;

			///
			/// Disables capturing and implicitly sets @ref isServerConfigurationSet()
			///
			virtual void disableCapture() = 0;

			///
			/// Updates the server configuration object.
			///
			/// @par
			/// The first call to this method takes over the given configuration as is. Subsequent calls will merge the
			/// given configuration with the one already stored.
			///
			/// @param newServerConfiguration the new configuration as received by the server.
			///
			virtual void updateServerConfiguration(std::shared_ptr<IServerConfiguration> newServerConfiguration) = 0;

			///
			/// Returns a boolean indicating whether the server configuration has been set before or not.
			///
			virtual bool isServerConfigurationSet() = 0;

			///
			/// Set the callback being invoked when the server configuration is updated.
			///
			/// @param serverConfigurationUpdateCallback Method being invoked
			///
			virtual void setServerConfigurationUpdateCallback(ServerConfigurationUpdateCallback serverConfigurationUpdateCallback) = 0;
		};
	}
}

#endif
