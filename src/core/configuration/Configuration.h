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

#ifndef _CORE_CONFIGURATION_CONFIGURATION_H
#define _CORE_CONFIGURATION_CONFIGURATION_H

#include "providers/ISessionIDProvider.h"
#include "HTTPClientConfiguration.h"
#include "OpenKitType.h"
#include "Device.h"
#include "protocol/StatusResponse.h"
#include "BeaconCacheConfiguration.h"
#include "BeaconConfiguration.h"

#include <memory>
#include <atomic>

namespace core
{
	namespace configuration
	{
		///
		/// The Configuration class holds all configuration settings, both provided by the user and the Dynatrace/AppMon server.
		///
		class Configuration
		{
		public:
			///
			/// Construct a Configuration given a @ref configuration::HTTPClientConfiguration
			/// @param[in] device device configuration
			/// @param[in] openKitType AppMon or dynatrace configuration @ref configuration::OpenKitType
			/// @param[in] applicationName applicationName application name
			/// @param[in] applicationVersion version of the application
			/// @param[in] applicationID application id
			/// @param[in] deviceID device id
			/// @param[in] endpointURL beacon endpoint URL
			/// @param[in] sessionIDProvider provider for session IDs
			/// @param[in] sslTrustManager the openkit::ISSLTrustManager instance to use
			/// @param[in] beaconCacheConfiguration beacon cache configuration
			/// @param[in] beaconConfiguration beacon configuration
			///
			Configuration
			(
				std::shared_ptr<Device> device,
				OpenKitType openKitType,
				const core::UTF8String& applicationName,
				const core::UTF8String& applicationVersion,
				const core::UTF8String& applicationID,
				int64_t deviceID,
				const core::UTF8String& origDeviceID,
				const core::UTF8String& endpointURL,
				std::shared_ptr<providers::ISessionIDProvider> sessionIDProvider,
				std::shared_ptr<openkit::ISSLTrustManager> sslTrustManager,
				std::shared_ptr<BeaconCacheConfiguration> beaconCacheConfiguration,
				std::shared_ptr<BeaconConfiguration> beaconConfiguration
			);

			virtual ~Configuration() {}

			///
			/// Return the  HTTPClientConfiguration to use when constructing a  HTTPClient
			/// @returns the  HTTPClientConfiguration stored in the  Configuration
			///
			std::shared_ptr<HTTPClientConfiguration> getHTTPClientConfiguration() const;

			///
			/// Update settings based on a status response
			/// @param[in] statusResponse the status response received from the server
			///
			void updateSettings(std::shared_ptr<protocol::StatusResponse> statusResponse);

			///
			/// Enable capturing
			///
			void enableCapture();

			///
			/// Disable capturing
			///
			void disableCapture();

			///
			/// Returns a flag if capturing is enabled
			/// @returns @c true if capturing is enabled, @c false if capturing is disabled
			///
			bool isCapture() const;

			///
			/// Return next session number
			/// @returns session number
			///
			int32_t createSessionNumber();

			///
			/// Return the OpenKit type as string
			/// @returns the OpenKit type as string
			///
			const char* getOpenKitType() const;

			///
			/// Returns the application name
			/// @returns the application name
			///
			const core::UTF8String& getApplicationName() const;

			///
			/// Returns the application id
			/// @returns the application id
			///
			const core::UTF8String& getApplicationID() const;

			///
			/// Returns the percent-encoded application id
			/// @returns the application id
			///
			const core::UTF8String& getApplicationIDPercentEncoded() const;

			///
			/// Returns the application version
			/// @returns the application version
			///
			const core::UTF8String& getApplicationVersion() const;

			///
			/// Returns the endpoint URL (=beacon URL)
			/// @returns the endpoint URL (=beacon URL)
			///
			const core::UTF8String& getEndpointURL() const;

			///
			/// Returns the device id
			/// @returns the device id
			///
			int64_t getDeviceID() const;

			///
			/// Returns the device id like it looked before it was parsed/hashed
			///
			const core::UTF8String& getOrigDeviceID() const;

			///
			/// Returns the send interval
			/// @returns the send interval
			///
			int64_t getSendInterval() const;

			///
			/// Sets the send interval.
			/// This method is intended for unit testing.
			/// @param[in] sendInterval
			///
			void setSendInterval(int64_t sendInterval);

			///
			/// Returns the maximum beacon size
			/// @returns the maximum beacon size
			///
			int32_t getMaxBeaconSize() const;

			///
			/// Returns a flag if errors are captured
			/// @returns @c true if errors are captured @c false if errors are not captured
			///
			bool isCaptureErrors() const;

			///
			/// Returns a flag if crashes are captured
			/// @returns @c true if crashes are captured @c false if crashes are not captured
			///
			bool isCaptureCrashes() const;

			///
			/// Returns the device configuration
			/// @returns the device configuration
			///
			std::shared_ptr<configuration::Device> getDevice() const;

			///
			/// Returns the beacon cache configuration
			/// @returns the beacon cache configuration
			///
			std::shared_ptr<configuration::BeaconCacheConfiguration> getBeaconCacheConfiguration() const;

			///
			/// Return the beacon configuration
			/// @returns the beacon configuration
			///
			std::shared_ptr<configuration::BeaconConfiguration> getBeaconConfiguration() const;

		private:
			/// HTTP client configuration
			std::shared_ptr<HTTPClientConfiguration> mHTTPClientConfiguration;

			/// session ID provider
			std::shared_ptr<providers::ISessionIDProvider> mSessionIDProvider;

			/// flag if capturing is enabled
			std::atomic<bool> mIsCapture;

			/// the send interval
			int64_t mSendInterval;

			/// maximum beacon size
			int32_t mMaxBeaconSize;

			/// flag if to capture errors
			bool mCaptureErrors;

			/// flag if to capture crashes
			bool mCaptureCrashes;

			/// OpenKit type
			OpenKitType mOpenKitType;

			/// application name
			core::UTF8String mApplicationName;

			/// application id
			core::UTF8String mApplicationID;

			/// percent encoded id
			core::UTF8String mApplicationIDPercentEncoded;

			/// the application version
			core::UTF8String mApplicationVersion;

			/// endpoint url
			core::UTF8String mEndpointURL;

			///device ID
			int64_t mDeviceID;

			/// original device ID
			core::UTF8String mOrigDeviceID;

			/// device information
			std::shared_ptr<configuration::Device> mDevice;

			/// configuration options for the beacon caching
			std::shared_ptr<configuration::BeaconCacheConfiguration> mBeaconCacheConfiguration;

			/// configuration options for @ref protocol::Beacon
			std::shared_ptr<configuration::BeaconConfiguration> mBeaconConfiguration;
		};
	}
}

#endif
