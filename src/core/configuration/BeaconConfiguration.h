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

#ifndef _CORE_CONFIGURATION_BEACONCONFIGURATION_H
#define _CORE_CONFIGURATION_BEACONCONFIGURATION_H

#include "IBeaconConfiguration.h"
#include "IHTTPClientConfiguration.h"
#include "IOpenKitConfiguration.h"
#include "IPrivacyConfiguration.h"
#include "IServerConfiguration.h"

#include <mutex>

namespace core
{
	namespace configuration
	{
		///
		/// Configuration for beacon cache.
		///
		class BeaconConfiguration
			: public IBeaconConfiguration
		{
		public:

			BeaconConfiguration(
					std::shared_ptr<IOpenKitConfiguration> openKitConfig,
					std::shared_ptr<IPrivacyConfiguration> privacyConfig,
					int32_t serverId
			);

			~BeaconConfiguration() override = default;

			///
			/// Creates a @ref IBeaconConfiguration from the given @ref IOpenKitConfiguration and
			/// @ref IPrivacyConfiguration.
			///
			/// @param openKitConfig application related configuration
			/// @param privacyConfig privacy related configuration.
			/// @return @c nullptr if any of the given arguments is @c nullptr, otherwise a new @ref IBeaconConfiguration.
			static std::shared_ptr<IBeaconConfiguration> from(
				std::shared_ptr<IOpenKitConfiguration> openKitConfig,
				std::shared_ptr<IPrivacyConfiguration> privacyConfig,
				int32_t serverId
			);

			std::shared_ptr<IOpenKitConfiguration> getOpenKitConfiguration() const override;

			std::shared_ptr<IPrivacyConfiguration> getPrivacyConfiguration() const override;

			std::shared_ptr<IHTTPClientConfiguration> getHTTPClientConfiguration() const override;

			std::shared_ptr<IServerConfiguration> getServerConfiguration() override;

			void initializeServerConfiguration(std::shared_ptr<IServerConfiguration> initialServerConfiguration) override;

			void enableCapture() override;

			void disableCapture() override;

			void updateServerConfiguration(std::shared_ptr<IServerConfiguration> newServerConfiguration) override;

			bool isServerConfigurationSet() override;

			void setServerConfigurationUpdateCallback(ServerConfigurationUpdateCallback serverConfigurationUpdateCallback) override;

		private:

			///
			/// Enables/disables capture according to the given state
			///
			/// @param captureState the state to which capture will be set.
			///
			void updateCaptureWith(bool captureState);

			std::shared_ptr<IServerConfiguration> getServerConfigurationOrDefault();

			/// application related configuration
			const std::shared_ptr<IOpenKitConfiguration> mOpenKitConfiguration;

			/// privacy related configuration
			const std::shared_ptr<IPrivacyConfiguration> mPrivacyConfiguration;

			/// HTTP client configuration
			const std::shared_ptr<IHTTPClientConfiguration> mHTTPClientConfiguration;

			/// server related configuration
			std::shared_ptr<IServerConfiguration> mServerConfiguration;

			/// indicator if server configuration was set or not
			bool mIsServerConfigurationSet;

			/// Callback invoked when server configuration is updated
			ServerConfigurationUpdateCallback mServerConfigurationUpdateCallback;

			/// synchronization
			std::recursive_mutex mMutex;
		};
	}
}

#endif
