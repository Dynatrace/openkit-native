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

#ifndef _PROTOCOL_IBEACON_H
#define _PROTOCOL_IBEACON_H

#include "core/UTF8String.h"
#include "core/configuration/IServerConfiguration.h"
#include "core/configuration/IBeaconConfiguration.h"
#include "core/objects/IActionCommon.h"
#include "core/objects/IWebRequestTracerInternals.h"
#include "protocol/IAdditionalQueryParameters.h"
#include "protocol/IStatusResponse.h"
#include "providers/IHTTPClientProvider.h"

#include <memory>
#include <cstdint>

namespace protocol
{
	class IBeacon
	{
	public:

		///
		/// Destructor
		///
		virtual ~IBeacon() = default;

		///
		/// Create unique sequence number
		/// The sequence number returned is only unique per Beacon.
		/// Calling this method on two different Beacon instances, might give the same result.
		/// @returns a unique sequence number;
		///
		virtual int32_t createSequenceNumber() = 0;

		///
		/// Get the current timestamp in milliseconds
		///
		virtual int64_t getCurrentTimestamp() const = 0;

		///
		/// Returns the time when the session was started (in milliseconds).
		///
		virtual int64_t getSessionStartTime() const = 0;

		///
		/// Create a unique identifier.
		/// -The identifier returned is only unique per Beacon.
		/// -Calling this method on two different Beacon instances, might give the same result.
		/// @returns a unique identifier
		///
		virtual int32_t createID() = 0;

		///
		/// Create a web request tag
		/// Web request tags can be attached as HTTP header for web request tracing.
		/// @param[in] parentActionID The ID of the @ref core::objects::RootAction or @ref core::objects::LeafAction for
		///   which to create a web request tag.
		/// @param[in] sequenceNumber Sequence number of the @ref core::objects::WebRequestTracer
		/// @returns A web request tracer tag
		///
		virtual core::UTF8String createTag(int32_t parentActionID, int32_t sequenceNumber) = 0;

		///
		/// Add @ref core::objects::IActionCommon to Beacon
		/// The serialized data is added to the Beacon
		/// @param[in] action action to add to the Beacon
		///
		virtual void addAction(std::shared_ptr<core::objects::IActionCommon> action) = 0;

		///
		/// Add sessionStart to Beacon
		///
		virtual void startSession() = 0;

		///
		/// Add @ref core::objects::Session to Beacon when session is ended.
		/// @param[in] session ended session that is added to the Beacon
		///
		virtual void endSession() = 0;

		///
		/// Add key-value-pair to Beacon.
		///
		/// The serialized data is added to @ref core::caching::BeaconCache.
		///
		/// @param actionID The id of the @ref core::objects::RootAction or @ref core::objects::LeafAction on which
		///   this value was reported.
		/// @param valueName Value's name.
		/// @param value Actual value to report.
		///
		virtual void reportValue(int32_t actionID, const core::UTF8String& valueName, int32_t value) = 0;

		///
		/// Add key-value-pair to Beacon.
		///
		/// The serialized data is added to @ref core::caching::BeaconCache.
		///
		/// @param actionID The id of the @ref core::objects::RootAction or @ref core::objects::LeafAction on which
		///   this value was reported.
		/// @param valueName Value's name.
		/// @param value Actual value to report.
		///
		virtual void reportValue(int32_t actionID, const core::UTF8String& valueName, int64_t value) = 0;

		///
		/// Add key-value-pair to Beacon.
		///
		/// The serialized data is added to @ref caching::BeaconCache.
		///
		/// @param actionID The id of the @ref core::objects::RootAction or @ref core::objects::LeafAction on which
		///   this value was reported.
		/// @param valueName Value's name.
		/// @param value Actual value to report.
		///
		virtual void reportValue(int32_t actionID, const core::UTF8String& valueName, double value) = 0;

		///
		/// Add key-value-pair to Beacon.
		///
		/// The serialized data is added to @ref caching::BeaconCache.
		///
		/// @param actionID The id of the @ref core::objects::RootAction or @ref core::objects::LeafAction on which
		///   this value was reported.
		/// @param valueName Value's name.
		/// @param value Actual value to report.
		///
		virtual void reportValue(int32_t actionID, const core::UTF8String& valueName, const core::UTF8String& value) = 0;

		///
		/// Add event (aka. named event) to Beacon.
		///
		/// The serialized data is added to @ref caching::BeaconCache.
		///
		/// @param actionID The id of the @ref core::objects::Action on which this event was reported.
		/// @param eventName Event's name.
		///
		virtual void reportEvent(int32_t actionID, const core::UTF8String& eventName) = 0;

		///
		/// Add error to Beacon.
		///
		/// The serialized data is added to @ref core::caching::BeaconCache.
		///
		/// @param actionID The id of the @ref core::objects::RootAction or @ref core::objects::LeafAction on which
		///   this value was reported.
		/// @param errorName Error's name.
		/// @param errorCode Some error code.
		/// @param reason Reason for that error.
		///
		virtual void reportError(int32_t actionID, const core::UTF8String& errorName, int32_t error, const core::UTF8String& reason) = 0;

		///
		/// Add crash to Beacon
		/// The serialized data is added to @ref core::caching::BeaconCache
		/// @param[in] errorName Error's name.
		/// @param[in] reason Reason for that error.
		/// @param[in] stacktrace Crash stacktrace.
		///
		virtual void reportCrash(const core::UTF8String& errorName, const core::UTF8String& reason, const core::UTF8String& stacktrace) = 0;

		///
		/// Add the given web request tracer to the Beacon
		/// The serialized data is added to @ref core::caching::BeaconCache
		/// @param actionID The id of the @ref core::objects::RootAction or @ref core::objects::LeafAction on which
		///   this value was reported.
		/// @param[in] webRequestTracer @ref core::objects::IWebRequestTracerInternals to serialize
		///
		virtual void addWebRequest(
			int32_t parentActionID,
			std::shared_ptr<core::objects::IWebRequestTracerInternals> webRequestTracer
		) = 0;

		///
		/// Add user identification to Beacon.
		/// The serialized data is added to @ref core::caching::BeaconCache
		/// @param[in] userTag User tag containing data to serialize.
		///
		virtual void identifyUser(const core::UTF8String& userTag) = 0;

		///
		/// Sends the current Beacon state
		/// @param[in] clientProvider the @ref providers::IHTTPClientProvider to use for sending
		/// @param[in] additionalParameters 
		/// @returns the status response returned for the Beacon data
		///
		virtual std::shared_ptr<IStatusResponse> send(
			std::shared_ptr<providers::IHTTPClientProvider> clientProvider,
			const protocol::IAdditionalQueryParameters& additionalParameters
		) = 0;

		///
		/// Checks if the Beacon is empty
		///
		/// A beacon is considered to be empty, if it does not contain any action or event data.
		/// @returns @c true if the beacon is empty, @c false otherwise
		///
		virtual bool isEmpty() const = 0;

		///
		/// Clears all previously collected data for this Beacon.
		///
		/// This only affects the so far serialized data, which gets removed from the cache.
		///
		virtual void clearData() = 0;

		///
		/// Returns the session number.
		/// @returns session number
		///
		virtual int32_t getSessionNumber() const = 0;

		///
		/// Returns the sequence number of the session.
		/// @par
		/// The session sequence number is a consecutive number which is increased when a session is split due to
        /// exceeding the maximum number of allowed events. The split session will have the same session number but an
        /// increased session sequence number.
		///
		virtual int32_t getSessionSequenceNumber() const = 0;

		///
		/// Returns the device id
		/// @returns device id
		///
		virtual int64_t getDeviceID() const = 0;

		///
		/// Get boolean flag indicating if client IP should be used (sent) or not.
		/// @returns @c true if client IP should be used, @c false otherwise.
		///
		virtual bool useClientIPAddress() const = 0;

		///
		/// Get the client IP address.
		/// @returns The client's IP address.
		///
		virtual const core::UTF8String& getClientIPAddress() const = 0;

		///
		/// Initializes the beacon with the given IServerConfiguration.
		///
		/// @param serverConfiguration the server configuration which will be used for initialization.
		///
		virtual void initializeServerConfiguration(std::shared_ptr<core::configuration::IServerConfiguration> serverConfiguration) = 0;

		///
		/// Updates this beacon with the given server configuration
		///
		/// @param serverConfig the server configuration which will be used to update this beacon.
		///
		virtual void updateServerConfiguration(std::shared_ptr<core::configuration::IServerConfiguration> serverConfig) = 0;

		///
		/// Indicates whether a server configuration is set on this beacon or not.
		///
		virtual bool isServerConfigurationSet() = 0;

		///
		/// Indicates whether action reporting is allowed by privacy settings or not.
		///
		virtual bool isActionReportingAllowedByPrivacySettings() = 0;

		///
		/// Indicates whether data capturing for this beacon is currently enabled or not.
		///
		virtual bool isDataCapturingEnabled() = 0;

		///
		/// Enables capturing for this beacon.
		///
		/// @par
		/// This will implicitly cause @ref isServerConfigurationSet() to return @c true
		///
		virtual void enableCapture() = 0;

		///
		/// Disables capturing for this beacon.
		///
		/// @par
		/// This will implicitly cause @ref isServerConfigurationSet() to return @c true
		///
		virtual void disableCapture() = 0;

		///
		/// Set the callback being invoked when the server configuration is updated.
		///
		/// @param serverConfigurationUpdateCallback Method being invoked
		///
		virtual void setServerConfigurationUpdateCallback(core::configuration::ServerConfigurationUpdateCallback serverConfigurationUpdateCallback) = 0;
	};
}

#endif
