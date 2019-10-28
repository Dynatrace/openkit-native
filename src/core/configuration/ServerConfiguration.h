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

#ifndef _CORE_CONFIGURATION_SERVERCONFIGURATION_H
#define _CORE_CONFIGURATION_SERVERCONFIGURATION_H

#include "core/configuration/IServerConfiguration.h"
#include "protocol/IStatusResponse.h"

namespace core
{
	namespace configuration
	{
		class ServerConfiguration
			: public IServerConfiguration
		{
		public:

			class Builder
			{
			public:

				///
				/// Default constructor
				///
				Builder();

				///
				/// Creates a new builder instance with pre-initialized fields from the given status response.
				///
				/// @param statusResponse status response used for initializing the builder.
				///
				Builder(std::shared_ptr<protocol::IStatusResponse> statusResponse);

				///
				/// Creates a new builder instance with pre-initialized fields from the given server configuration.
				///
				/// @param serverConfiguration  server configuration for initializing the builder.
				///
				Builder(std::shared_ptr<core::configuration::IServerConfiguration> serverConfiguration);

				bool isCaptureEnabled();

				///
				/// Enables/disables capturing by setting @ref isCaptureEnabled to the corresponding value.
				///
				/// @param capture the capture state to set.
				/// @return @c this
				///
				Builder& withCapture(bool capture);

				bool isCrashReportingEnabled();

				///
				/// Enables/disables crash reporting by setting @ref isCrashReportingEnabled to the corresponding value.
				///
				/// @param crashReportingState the crash reporting state to set.
				/// @return @c this
				Builder& withCrashReporting(bool crashReportingState);

				bool isErrorReportingEnabled();

				///
				/// Enables/disables error reporting by setting @c isErrorReportingEnabled to the corresponding value.
				///
				/// @param errorReportingState the error reporting state to set.
				/// @return @c this
				///
				Builder& withErrorReporting(bool errorReportingState);

				int32_t getSendIntervalInMilliseconds();

				///
				/// Configures the send interval.
				///
				/// @param sendIntervalInMilliseconds the send interval in milliseconds.
				/// @return @c this
				///
				Builder& withSendIntervalInMilliseconds(int32_t sendIntervalInMilliseconds);

				int32_t getServerId();

				///
				/// Configures the server ID.
				///
				/// @param serverId the ID of the server to communicate with.
				/// @return @c this
				Builder& withServerId(int32_t serverId);

				int32_t getBeaconSizeInBytes();

				///
				/// Configures the beacon size in bytes.
				///
				/// @param beaconSize the maximum allowed beacon size in bytes.
				/// @return @c this
				///
				Builder& withBeaconSizeInBytes(int32_t beaconSize);

				int32_t getMultiplicity();

				///
				/// Configures the multiplicity factor.
				///
				/// @param multiplicity multiplicity factor.
				/// @return @ this
				Builder& withMultiplicity(int32_t multiplicity);

				///
				/// Creates a new instance of @ref IServerConfiguration
				///
				/// @return a newly created IServerConfiguration instance.
				///
				std::shared_ptr<core::configuration::IServerConfiguration> build();

			private:

				bool mCaptureState;
				bool mCrashReportingState;
				bool mErrorReportingState;
				int32_t mSendIntervalInMilliseconds;
				int32_t mServerId;
				int32_t mBeaconSizeInBytes;
				int32_t mMultiplicity;
			};

			///
			/// Creates a server configuration from the given builder.
			///
			ServerConfiguration(ServerConfiguration::Builder& builder);

			/// Default server configuration instance.
			static const std::shared_ptr<const core::configuration::IServerConfiguration> DEFAULT;

			/// by default capturing is enabled
			static constexpr bool DEFAULT_CAPTURE_ENABLED = true;

			/// by default crash reporting is enabled
			static constexpr bool DEFAULT_CRASH_REPORTING_ENABLED = true;

			/// by default error reporting is enabled
			static constexpr bool DEFAULT_ERROR_REPORTING_ENABLED = true;

			/// default send interval is not defined
			static constexpr int32_t DEFAULT_SEND_INTERVAL = -1;

			/// default server ID depends on the backend
			static constexpr int32_t DEFAULT_SERVER_ID = -1;

			/// default beacon size is not defined
			static constexpr int32_t DEFAULT_BEACON_SIZE = -1;

			// default multiplicity is 1
			static constexpr int32_t DEFAULT_MULTIPLICITY = 1;

			~ServerConfiguration() override = default;

			///
			/// Creates a new serer configuration from the given status response.
			///
			/// @param statusResponse the status response from which to create the server configuration.
			/// @return a newly creates server configuration.
			///
			static std::shared_ptr<core::configuration::IServerConfiguration> from(
					std::shared_ptr<protocol::IStatusResponse> statusResponse
			);

			bool isCaptureEnabled() const override;

			bool isCrashReportingEnabled() const override;

			bool isErrorReportingEnabled() const override;

			int32_t getSendIntervalInMilliseconds() const override;

			int32_t getServerId() const override;

			int32_t getBeaconSizeInBytes() const override;

			int32_t getMultiplicity() const override;

			bool isSendingDataAllowed() const override;

			bool isSendingCrashesAllowed() const override;

			bool isSendingErrorsAllowed() const override;

			std::shared_ptr<core::configuration::IServerConfiguration> merge(
					std::shared_ptr<core::configuration::IServerConfiguration> other
			) const override;

		private:

			/// indicator whether capturing is enabled by backend or not.
			const bool mIsCaptureEnabled;

			/// indicator whether crash reporting is enabled by backend or not.
			const bool mIsCrashReportingEnabled;

			/// indicator whether error reporting is enabled by backend or not.
			const bool mIsErrorReportingEnabled;

			/// value specifying the send interval in milliseconds
			const int32_t mSendIntervalInMilliseconds;

			/// the server ID to send future requests to
			const int32_t mServerId;

			/// the maximum allowed beacon size (post body size) in bytes
			const int32_t mBeaconSizeInBytes;

			/// the multiplicity value
			const int32_t mMultiplicity;
		};
	}
}

#endif
