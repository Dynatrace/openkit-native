/**
 * Copyright 2018-2021 Dynatrace LLC
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

#ifndef _CORE_CONFIGURATION_IOPENKITCONFIGURATION_H
#define _CORE_CONFIGURATION_IOPENKITCONFIGURATION_H

#include "OpenKit/ISSLTrustManager.h"
#include "OpenKit/IHttpRequestInterceptor.h"
#include "OpenKit/IHttpResponseInterceptor.h"
#include "core/UTF8String.h"

#include <memory>

namespace core
{
	namespace configuration
	{
		class IOpenKitConfiguration
		{
		public:

			virtual ~IOpenKitConfiguration() = default;

			///
			/// Returns the endpoint URL to which beacon data will be sent.
			///
			virtual const core::UTF8String& getEndpointUrl() const = 0;

			///
			/// Returns the identifier of the device for which this configuration is used.
			///
			virtual int64_t getDeviceId() const = 0;

			///
			/// Returns the original (not hashed) device identifier.
			///
			virtual const core::UTF8String& getOrigDeviceId() const = 0;

			///
			/// Returns the type of this configuration.
			///
			virtual const std::string& getOpenKitType() const = 0;

			///
			/// Returns the identifier of the application for which this configuration is used.
			///
			virtual const core::UTF8String& getApplicationId() const = 0;

			///
			/// Returns a percent encoded representation of the application identifier.
			///
			virtual const core::UTF8String& getApplicationIdPercentEncoded() const = 0;

			///
			/// Returns the version of the application for which this configuration is used.
			///
			virtual const core::UTF8String& getApplicationVersion() const = 0;

			///
			/// Returns the device's operating system.
			///
			virtual const core::UTF8String& getOperatingSystem() const = 0;

			///
			/// Returns the device's manufacturer
			///
			virtual const core::UTF8String& getManufacturer() const = 0;

			///
			/// Returns the device's model ID.
			///
			virtual const core::UTF8String& getModelId() const = 0;

			///
			/// Returns the default identifier of the Dynatrace server to communicate with.
			///
			virtual int32_t getDefaultServerId() const = 0;

			///
			/// Returns the SSL trust manager
			///
			virtual std::shared_ptr<openkit::ISSLTrustManager> getTrustManager() const = 0;

			///
			/// Returns the openkit::IHttpRequestInterceptor
			///
			virtual std::shared_ptr<openkit::IHttpRequestInterceptor> getHttpRequestInterceptor() const = 0;

			///
			/// Returns the openkit::IHttpResponseInterceptor
			///
			virtual std::shared_ptr<openkit::IHttpResponseInterceptor> getHttpResponseInterceptor() const = 0;
		};
	}
}

#endif
