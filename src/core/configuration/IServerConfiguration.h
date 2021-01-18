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

#ifndef _CORE_CONFIGURATION_ISERVERCONFIGURATION_H
#define _CORE_CONFIGURATION_ISERVERCONFIGURATION_H

#include <cstdint>
#include <memory>

namespace core
{
	namespace configuration
	{
		class IServerConfiguration
		{
		public:

			virtual ~IServerConfiguration() = default;

			///
			/// Indicates whether capturing is enabled in Dynatrace/AppMon or not.
			///
			/// @return @c true if capturing is enabled, @c false otherwise.
			///
			virtual bool isCaptureEnabled() const = 0;

			///
			/// Indicates whether crash reporting is enabled by Dynatrace/AppMon or not.
			///
			/// @return @c true if crash reporting is enabled, @c false otherwise.
			///
			virtual bool isCrashReportingEnabled() const = 0;

			///
			/// Indicates whether error reporting is enabled by Dynatrace/AppMon or not.
			///
			/// @return @c true if error reporting is enabled, @c false otherwise.
			///
			virtual bool isErrorReportingEnabled() const = 0;

			///
			/// Returns the ID of the server which is communicated with.
			///
			virtual int32_t getServerId() const = 0;

			///
			/// Returns the maximum beacon size, that is the post body, in bytes.
			///
			virtual int32_t getBeaconSizeInBytes() const = 0;

			///
			/// Returns the multiplicity value.
			///
			/// @par
			/// Multiplicity is a factor on the server side, which is greater than 1 to prevent overload situations.
			/// This value comes from the server and has to be sent back to the server.
			///
			virtual int32_t getMultiplicity() const = 0;

			///
			/// Returns the send interval in milliseconds.
			///
			virtual int32_t getSendIntervalInMilliseconds() const = 0;

			///
			/// Returns the maximum duration in milliseconds after which a session is to be split.
			///
			virtual int32_t getMaxSessionDurationInMilliseconds() const = 0;

			///
			/// Returns @c true if session splitting when exceeding the maximum session duration is enabled,
			/// @c false otherwise.
			///
			virtual bool isSessionSplitBySessionDurationEnabled() const = 0;

			///
			/// Returns the maximum number of top level actions after which a session is to be split.
			///
			virtual int32_t getMaxEventsPerSession() const = 0;
			
			///
			/// Returns @c true if session splitting when exceeding the maximum number of top level events is
			/// enabled, @c false otherwise.
			///
			virtual bool isSessionSplitByEventsEnabled() const = 0;

			///
			/// Returns the idle timeout after which a session is to be split.
			///
			virtual int32_t getSessionTimeoutInMilliseconds() const = 0;

			///
			/// Returns @c true if session splitting by exceeding the idle timeout is enabled, @c false otherwise.
			///
			virtual bool isSessionSplitByIdleTimeoutEnabled() const = 0;

			///
			/// Returns the version of the visit store that is being used.
			///
			virtual int32_t getVisitStoreVersion() const = 0;

			///
			/// Indicates whether sending arbitrary data to the server is allowed or not.
			///
			/// @par
			/// Sending data is allowed if all of the following conditions evaluate to true:
			/// @li @c isCaptureEnabled is @c true
			/// @li @c getMultiplicity is greater than @c 0
			///
			/// To check if sending errors is allowed, use @c isSendingErrorsAllowed.
			/// To check if sending crashes is allowed, use @c isSendingCrashesAllowed.
			///
			/// @return @c true if data sending is allowed, @c false otherwise.
			///
			virtual bool isSendingDataAllowed() const = 0;

			///
			/// Indicates whether sending crashes to the server is allowed or not.
			///
			/// @par
			/// Sending crashes is only allowed if all of the following conditions evaluate to true:
			/// @li @c isSendingDataAllowed is @c true
			/// @li @c isCrashReportingEnabled is @c true
			///
			/// @return @c true if sending crashes is allowed, @c false otherwise
			///
			virtual bool isSendingCrashesAllowed() const = 0;

			///
			/// Indicates whether sending errors to the server is allowed or not.
			///
			/// @par
			/// Sending errors is only allowed if all of the following conditions evaluate to true:
			/// @li @c isSendingDataAllowed is @c true
			/// @li @c isErrorReportingAllowed is @c true
			///
			/// @return @c true if sending errors is allowed, @c false otherwise
			///
			virtual bool isSendingErrorsAllowed() const = 0;

			///
			/// Merges the given server configuration with this server configuration instance and returns the newly
			/// created merged instance.
			///
			/// @par
			/// Most fields are taken from the other server configuration, expect for the multiplicity and the server ID.
			///
			/// @param other the server configuration to be merged
			/// @return a newly created server configuration consisting of a merge between the given and this server
			///  configuration.
			///
			virtual std::shared_ptr<IServerConfiguration> merge(std::shared_ptr<IServerConfiguration> other) const = 0;
		};
	}
}

#endif
