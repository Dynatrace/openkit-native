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

				bool isCaptureEnabled() const;

				///
				/// Enables/disables capturing by setting @ref isCaptureEnabled to the corresponding value.
				///
				/// @param capture the capture state to set.
				/// @return @c this
				///
				Builder& withCapture(bool capture);

				bool isCrashReportingEnabled() const;

				///
				/// Enables/disables crash reporting by setting @ref isCrashReportingEnabled to the corresponding value.
				///
				/// @param crashReportingState the crash reporting state to set.
				/// @return @c this
				Builder& withCrashReporting(bool crashReportingState);

				bool isErrorReportingEnabled() const;

				///
				/// Enables/disables error reporting by setting @c isErrorReportingEnabled to the corresponding value.
				///
				/// @param errorReportingState the error reporting state to set.
				/// @return @c this
				///
				Builder& withErrorReporting(bool errorReportingState);

				int32_t getSendIntervalInMilliseconds() const;

				///
				/// Configures the send interval.
				///
				/// @param sendIntervalInMilliseconds the send interval in milliseconds.
				/// @return @c this
				///
				Builder& withSendIntervalInMilliseconds(int32_t sendIntervalInMilliseconds);

				int32_t getServerId() const;

				///
				/// Configures the server ID.
				///
				/// @param serverId the ID of the server to communicate with.
				/// @return @c this
				Builder& withServerId(int32_t serverId);

				int32_t getBeaconSizeInBytes() const;

				///
				/// Configures the beacon size in bytes.
				///
				/// @param beaconSize the maximum allowed beacon size in bytes.
				/// @return @c this
				///
				Builder& withBeaconSizeInBytes(int32_t beaconSize);

				int32_t getMultiplicity() const;

				///
				/// Configures the multiplicity factor.
				///
				/// @param multiplicity multiplicity factor.
				/// @return @ this
				Builder& withMultiplicity(int32_t multiplicity);

				int32_t getMaxSessionDurationInMilliseconds() const;

				///
				/// Configures the maximum duration after which the session gets split.
				///
				/// @param maxSessionDurationInMilliseconds the maximum duration of a session in milliseconds
				/// @return @c this
				Builder& withMaxSessionDurationInMilliseconds(int32_t maxSessionDurationInMilliseconds);

				int32_t getMaxEventsPerSession() const;

				///
				/// Configures the maximum number of events after which the session gets split.
				///
				/// @param maxEventsPerSession the maximum number of top level actions after which a session gets split.
				/// @return @c this
				Builder& withMaxEventsPerSession(int maxEventsPerSession);

				int32_t getSessionTimeoutInMilliseconds() const;

				///
				/// Configures the idle timeout after which a session gets split.
				///
				/// @param sessionTimeoutInMilliseconds the idle timeout in milliseconds after which a session gets split.
				/// @return @c this
				Builder& withSessionTimeoutInMilliseconds(int32_t sessionTimeoutInMilliseconds);

				int32_t getVisitStoreVersion() const;

				///
				/// Configures the version of the visit store that is to be used.
				///
				/// @param visitStoreVersion the version of the visit store to be used.
				/// @return @c this
				Builder& withVisitStoreVersion(int32_t visitStoreVersion);

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
				int32_t mMaxSessionDurationInMilliseconds;
				int32_t mMaxEventsPerSession;
				int32_t mSessionIdleTimeout;
				int32_t mVisitStoreVersion;
			};

			///
			/// Creates a server configuration from the given builder.
			///
			ServerConfiguration(ServerConfiguration::Builder& builder);

			/// Default server configuration instance.
			static const std::shared_ptr<core::configuration::IServerConfiguration> DEFAULT;

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

			// by default split by session duration is disabled, thus value is -1
			static constexpr int32_t DEFAULT_MAX_SESSION_DURATION = -1;

			// by default split by events is disabled,thus value is -1
			static constexpr int32_t DEFAULT_MAX_EVENTS_PER_SESSION = -1;

			// by default session split by timeout is disabled, thus value is -1
			static constexpr int32_t DEFAULT_SESSION_TIMEOUT = -1;

			// default visit store version is 1
			static constexpr int32_t DEFAULT_VISIT_STORE_VERSION = 1;

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

			int32_t getMaxSessionDurationInMilliseconds() const override;

			int32_t getMaxEventsPerSession() const override;

			int32_t getSessionTimeoutInMilliseconds() const override;

			int32_t getVisitStoreVersion() const override;

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

			/// the maximum duration of a session after which it gets split
			const int32_t mMaxSessionDurationInMilliseconds;

			/// the maximum number of events after which a session gets split
			const int32_t mMaxEventsPerSession;

			/// the idle timeout after which a session gets split
			const int32_t mSessionTimeoutInMilliseconds;

			/// the version of the visit store being used.
			const int32_t mVisitStoreVersion;
		};
	}
}

#endif
