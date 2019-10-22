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

#ifndef _PROTOCOL_IBEACON_H
#define _PROTOCOL_IBEACON_H

#include "IStatusResponse.h"
#include "core/UTF8String.h"
#include "core/configuration/BeaconConfiguration.h"
#include "core/objects/RootAction.h"
#include "core/objects/Session.h"

#include <memory>

namespace core
{
	namespace objects
	{
		class IActionCommon;
		class WebRequestTracer;
	}
}

namespace protocol
{
	class IBeacon
	{
	public:

		///
		/// Destructor
		///
		virtual ~IBeacon() {}

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
		/// Add @ref core::WebRequestTracer to Beacon
		/// The serialized data is added to @ref core::caching::BeaconCache
		/// @param actionID The id of the @ref core::objects::RootAction or @ref core::objects::LeafAction on which
		///   this value was reported.
		/// @param[in] webRequestTracer @ref core::objects::WebRequestBase to serialize
		///
		virtual void addWebRequest(int32_t parentActionID, std::shared_ptr<core::objects::WebRequestTracer> webRequestTracer) = 0;

		///
		/// Add user identification to Beacon.
		/// The serialized data is added to @ref core::caching::BeaconCache
		/// @param[in] userTag User tag containing data to serialize.
		///
		virtual void identifyUser(const core::UTF8String& userTag) = 0;

		///
		/// Sends the current Beacon state
		/// @param[in] clientProvider the @ref providers::IHTTPClientProvider to use for sending
		/// @returns the status response returned for the Beacon data
		///
		virtual std::shared_ptr<IStatusResponse> send(std::shared_ptr<providers::IHTTPClientProvider> clientProvider) = 0;

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
		/// Returns the device id
		/// @returns device id
		///
		virtual int64_t getDeviceID() const = 0;

		///
		/// Sets the beacon configuration on the Beacon
		/// @param[in] beaconConfiguration the beacon configuration to apply to this Beacon
		///
		virtual void setBeaconConfiguration(std::shared_ptr<core::configuration::BeaconConfiguration> beaconConfiguration) = 0;

		///
		/// Return the beacon configuration
		/// @returns the beacon configuration
		///
		virtual std::shared_ptr<core::configuration::BeaconConfiguration> getBeaconConfiguration() const = 0;

		///
		/// Get the client IP address.
		/// @returns The client's IP address.
		///
		virtual const core::UTF8String& getClientIPAddress() const = 0;
	};
}

#endif
