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

#ifndef _CONFIGURATION_OBJECTS_HTTPCLIENTCONFIGURATION_H
#define _CONFIGURATION_OBJECTS_HTTPCLIENTCONFIGURATION_H

#include "OpenKit/ISSLTrustManager.h"
#include "core/UTF8String.h"
#include "core/configuration/IHTTPClientConfiguration.h"
#include "core/configuration/IOpenKitConfiguration.h"

#include <memory>


namespace core
{
	namespace configuration
	{
		///
		/// The HTTPClientConfiguration holds all http client related settings
		///
		class HTTPClientConfiguration
			: public IHTTPClientConfiguration
		{
		public:

			class Builder
			{
			public:

				Builder();

				///
				/// Creates a builder initialized from the given OpenKit configuration.
				///
				/// @param openKitConfig the configuration from which the builder will be initialized.
				///
				Builder(std::shared_ptr<core::configuration::IOpenKitConfiguration> openKitConfig);

				///
				/// Creates a builder initialized from the given HTTP client configuration.
				///
				/// @param httpClientConfig the configuration from which the builder will be initialized.
				///
				Builder(std::shared_ptr<core::configuration::IHTTPClientConfiguration> httpClientConfig);

				const core::UTF8String& getBaseURL() const;

				Builder& withBaseURL(const core::UTF8String& baseURL);

				int32_t getServerID() const;

				Builder& withServerID(int32_t serverID);

				const core::UTF8String& getApplicationID() const;

				Builder& withApplicationID(const core::UTF8String& applicationID);

				std::shared_ptr<openkit::ISSLTrustManager> getTrustManager() const;

				Builder& withTrustManager(std::shared_ptr<openkit::ISSLTrustManager> trustManager);

				std::shared_ptr<core::configuration::IHTTPClientConfiguration> build();

			private:

				core::UTF8String mBaseURL;

				int32_t mServerID;

				core::UTF8String mApplicationID;

				std::shared_ptr<openkit::ISSLTrustManager> mTrustManager;
			};

			///
			/// Default constructor
			/// @param[in] url the beacon URL
			/// @param[in] serverID server id
			/// @param[in] applicationID the application id
			/// @param[in] sslTrustManager optional
			///
			HTTPClientConfiguration(Builder& builder);

			///
			/// Returns the base url for the http client
			/// @returns the base url
			///
			const core::UTF8String& getBaseURL() const override;

			///
			/// Returns the server id to be used for the http client
			/// @returns the server id
			///
			int32_t getServerID() const override;

			///
			/// The application id for the http client
			/// @returns the application id
			///
			const core::UTF8String& getApplicationID() const override;

			///
			/// Returns the trust manager which defines how trust in SSL shall be handled
			/// @returns the trust manager which defines how trust in SSL shall be handled
			///
			std::shared_ptr<openkit::ISSLTrustManager> getSSLTrustManager() const override;

		private:
			/// the beacon URL
			const core::UTF8String mBaseURL;

			/// the server ID
			const int32_t mServerID;

			/// the application id
			const core::UTF8String mApplicationID;

			/// how the peer's TSL/SSL certificate and the hostname shall be trusted
			const std::shared_ptr<openkit::ISSLTrustManager> mSSLTrustManager;
		};
	}
}
#endif