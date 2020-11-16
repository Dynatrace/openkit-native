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

#ifndef _CORE_CONFIGURATION_OPENKITCONFIGURATION_H
#define _CORE_CONFIGURATION_OPENKITCONFIGURATION_H

#include "OpenKit/IOpenKitBuilder.h"
#include "core/UTF8String.h"
#include "core/configuration/IOpenKitConfiguration.h"

#include <memory>

namespace core
{
	namespace configuration
	{
		///
		/// Holds all application relevant configuration settings, both provided by the user and the Dynatrace/AppMon server.
		///
		class OpenKitConfiguration
			: public core::configuration::IOpenKitConfiguration
		{
		public:

			///
			/// Constructor taking over the values from the given builder
			///
			OpenKitConfiguration(const openkit::IOpenKitBuilder& builder);

			///
			/// Destructor
			///
			~OpenKitConfiguration() override = default;

			///
			/// Creates an OpenKit configuration instance from the given builder.
			///
			/// @param builder the OpenKit builder for which to create an OpenKit configuration instance.
			/// @return a newly created OpenKit configuration instance.
			///
			static std::shared_ptr<core::configuration::IOpenKitConfiguration> from(const openkit::IOpenKitBuilder& builder);

			const core::UTF8String& getEndpointUrl() const override;

			int64_t getDeviceId() const override;

			const core::UTF8String& getOrigDeviceId() const override;

			const std::string& getOpenKitType() const override;

			const core::UTF8String& getApplicationId() const override;

			const core::UTF8String& getApplicationIdPercentEncoded() const override;

			const core::UTF8String& getApplicationName() const override;

			const core::UTF8String& getApplicationVersion() const override;

			const core::UTF8String& getOperatingSystem() const override;

			const core::UTF8String& getManufacturer() const override;

			const core::UTF8String& getModelId() const override;

			int32_t getDefaultServerId() const override;

			std::shared_ptr<openkit::ISSLTrustManager> getTrustManager() const override;

		private:

			/// endpoint URL to send data to
			const core::UTF8String mEndpointUrl;

			/// unique device/installation identifier
			const int64_t mDeviceId;

			/// unique device/installation identifier in a not-hashed representation (as it was passed to the OpenKit builder)
			const core::UTF8String mOrigDeviceId;

			/// OpenKit type string which can be used for the purpose of logging
			const std::string mOpenKitType;

			/// application identifier for which to report data
			const core::UTF8String mApplicationId;

			/// application identifier in percent encoded representation
			const core::UTF8String mPercentEncodedApplicationId;

			/// name of the application for which to report data
			const core::UTF8String mApplicationName;

			/// version of the application for which to report data
			const core::UTF8String mApplicationVersion;

			/// the operating system on which the application is running
			const core::UTF8String mOperatingSystem;

			/// the manufacturer of the device/installation
			const core::UTF8String mManufacturer;

			/// the identifier of the device's model
			const core::UTF8String mModelId;

			/// default server ID to communicate with
			const int32_t mDefaultServerId;

			/// configured SSL trust manager
			const std::shared_ptr<openkit::ISSLTrustManager> mTrustManager;
		};
	}
}

#endif
