/**
* Copyright 2018 Dynatrace LLC
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

#ifndef _PROTOCOL_BEACON_H
#define _PROTOCOL_BEACON_H

#include "OpenKit/ILogger.h"
#include "core/UTF8String.h"
#include "providers/ITimingProvider.h"
#include "providers/IThreadIDProvider.h"
#include "configuration/Configuration.h"
#include "configuration/HTTPClientConfiguration.h"
#include "core/Action.h"
#include "core/RootAction.h"
#include "core/Session.h"
#include "core/WebRequestTracerBase.h"
#include "caching/BeaconCache.h"
#include "EventType.h"

#include <memory>
#include <map>

namespace protocol
{
	///
	/// The Beacon class holds all the beacon data and the beacon protocol implementation.
	///
	class Beacon
	{
	public:
		///
		/// Constructor for Beacon
		/// @param[in] logger to write traces to
		/// @param[in] beaconCache Cache storing beacon related data.
		/// @param[in] configuration Configuration object
		/// @param[in] clientIPAddress IP Address of the client
		/// @param[in] timingProvider timing provider used to retrieve timestamps
		///
		Beacon(std::shared_ptr<openkit::ILogger> logger, std::shared_ptr<caching::IBeaconCache> beaconCache, std::shared_ptr<configuration::Configuration> configuration, const core::UTF8String clientIPAddress, std::shared_ptr<providers::IThreadIDProvider> threadIDProvider , std::shared_ptr<providers::ITimingProvider> timingProvider);

		///
		/// Destructor 
		///
		virtual ~Beacon() {}

		///
		/// Create unique sequence number
		/// The sequence number returned is only unique per Beacon.
		/// Calling this method on two different Beacon instances, might give the same result.
		/// @returns a unique sequencen number;
		///
		virtual int32_t createSequenceNumber();

		///
		/// Get the current timestamp in milliseconds by delegating to TimingProvider
		/// @returns Current timestamp in milliseconds
		///
		virtual int64_t getCurrentTimestamp() const;

		///
		/// Create a unique identifier.
		/// -The identifier returned is only unique per Beacon.
		/// -Calling this method on two different Beacon instances, might give the same result.
		/// @returns a unique identifier
		///
		int32_t createID();

		///
		/// Create a web request tag
		/// Web request tags can be attached as HTTP header for web request tracing.
		/// @param[in] parentActionID The ID of the @ref Action for which to create a web request tag. 
		/// @param[in] sequenceNumber Sequence number of the @ref WebRequestTracer
		/// @returns A web request tracer tag
		///
		virtual core::UTF8String createTag(int32_t parentActionID, int32_t sequenceNumber);

		///
		/// Add Action to Beacon
		/// The serialized data is added to the Beacon
		/// @param[in] action action to add to the Beacon
		///
		void addAction(std::shared_ptr<core::Action> action);

		///
		/// Add RootAction to Beacon
		/// The serialized data is added to the Beacon
		/// @param[in] action root action to add to the Beacon
		///
		void addAction(std::shared_ptr<core::RootAction> action);

		///
		/// Add Session to Beacon when session is ended.
		/// @param[in] session ended session that is added to the Beacon
		///
		virtual void endSession(std::shared_ptr<core::Session> session);

		///
		/// Add key-value-pair to Beacon.
		///
		/// The serialized data is added to @ref BeaconCache.
		///
		/// @param parentAction The @ref Action on which this value was reported.
		/// @param valueName Value's name.
		/// @param value Actual value to report.
		///
		virtual void reportValue(std::shared_ptr<core::Action> parentAction, const core::UTF8String& valueName, int32_t value);

		///
		/// Add key-value-pair to Beacon.
		///
		/// The serialized data is added to @ref BeaconCache.
		///
		/// @param parentAction The @ref Action on which this value was reported.
		/// @param valueName Value's name.
		/// @param value Actual value to report.
		///
		virtual void reportValue(std::shared_ptr<core::Action> parentAction, const core::UTF8String& valueName, double value);

		///
		/// Add key-value-pair to Beacon.
		///
		/// The serialized data is added to @ref BeaconCache.
		///
		/// @param parentAction The @ref Action on which this value was reported.
		/// @param valueName Value's name.
		/// @param value Actual value to report.
		///
		virtual void reportValue(std::shared_ptr<core::Action> parentAction, const core::UTF8String& valueName, const core::UTF8String& value);

		///
		/// Add event (aka. named event) to Beacon.
		///
		/// The serialized data is added to @ref BeaconCache.
		///
		/// @param parentAction The @ref Action on which this event was reported.
		/// @param eventName Event's name.
		///
		virtual void reportEvent(std::shared_ptr<core::Action> parentAction, const core::UTF8String& eventName);

		///
		/// Add error to Beacon.
		///
		/// The serialized data is added to @ref BeaconCache.
		///
		/// @param parentAction The @ref Action on which this error was reported.
		/// @param errorName Error's name.
		/// @param errorCode Some error code.
		/// @param reason Reason for that error.
		///
		virtual void reportError(std::shared_ptr<core::Action> parentAction, const core::UTF8String& errorName, int32_t errorCode, const core::UTF8String& reason);

		///
		/// Add key-value-pair to Beacon.
		///
		/// The serialized data is added to @ref BeaconCache.
		///
		/// @param parentAction The @ref Action on which this value was reported.
		/// @param valueName Value's name.
		/// @param value Actual value to report.
		///
		virtual void reportValue(std::shared_ptr<core::RootAction> parentAction, const core::UTF8String& valueName, int32_t value);

		///
		/// Add key-value-pair to Beacon.
		///
		/// The serialized data is added to @ref BeaconCache.
		///
		/// @param parentAction The @ref RootAction on which this value was reported.
		/// @param valueName Value's name.
		/// @param value Actual value to report.
		///
		virtual void reportValue(std::shared_ptr<core::RootAction> parentAction, const core::UTF8String& valueName, double value);

		///
		/// Add key-value-pair to Beacon.
		///
		/// The serialized data is added to @ref BeaconCache.
		///
		/// @param parentAction The @ref RootAction on which this value was reported.
		/// @param valueName Value's name.
		/// @param value Actual value to report.
		///
		virtual void reportValue(std::shared_ptr<core::RootAction> parentAction, const core::UTF8String& valueName, const core::UTF8String& value);

		///
		/// Add event (aka. named event) to Beacon.
		///
		/// The serialized data is added to @ref BeaconCache.
		///
		/// @param parentAction The @ref RootAction on which this event was reported.
		/// @param eventName Event's name.
		///
		virtual void reportEvent(std::shared_ptr<core::RootAction> parentAction, core::UTF8String eventName);

		///
		/// Add error to Beacon.
		///
		/// The serialized data is added to @ref BeaconCache.
		///
		/// @param parentAction The @ref RootAction on which this error was reported.
		/// @param errorName Error's name.
		/// @param errorCode Some error code.
		/// @param reason Reason for that error.
		///
		virtual void reportError(std::shared_ptr<core::RootAction> parentAction, const core::UTF8String& errorName, int32_t errorCode, const core::UTF8String& reason);

		///
		/// Add crash to Beacon
		/// The serialized data is added to @ref BeaconCache
		/// @param[in] errorName Error's name.
		/// @param[in] reason Reason for that error.
		/// @param[in] stacktrace Crash stacktrace.
		///
		virtual void reportCrash(const core::UTF8String& errorName, const core::UTF8String& reason, const core::UTF8String& stacktrace);

		///
		/// Add web request to Beacon
		/// The serialized data is added to @ref BeaconCache
		/// @param[in] The @ref Action on which the web request was reported
		/// @param[in] @ref WebRequestTracer to serialize
		///
		virtual void addWebRequest(int32_t parentActionID, std::shared_ptr<core::WebRequestTracerBase> webRequestTracer);

		///
		/// Add user identification to Beacon.
		/// The serialized data is added to @ref BeaconCache
		/// @param[in] userTag User tag containing data to serialize.
		///
		virtual void identifyUser(const core::UTF8String& userTag);

		/// 
		/// Sends the current Beacon state
		/// @param[in] clientProvider the IHTTPClientProvider to use for sending
		/// @returns the status response returned for the Beacon data
		///
		virtual std::unique_ptr<protocol::StatusResponse> send(std::shared_ptr<providers::IHTTPClientProvider> clientProvider);

		///
		/// Tests if the Beacon is empty
		/// 
		/// A beacon is considered to be empty, if it does not contain any action or event data.
		/// @returns @c true if the beacon is empty, @c false otherwise
		///
		bool isEmpty() const;

		///
		/// Clears all previously collected data for this Beacon.
		///
		/// This only affects the so far serialized data, which gets removed from the cache.
		///
		void clearData();
	private:
		///
		/// Serialization helper method for creating basic beacon protocol data.
		/// @returns Serialized data
		///
		core::UTF8String createBasicBeaconData();

		///
		/// Serialization helper method for creating basic event data
		/// @return Serialized data
		///
		core::UTF8String createBasicEventData(EventType eventType, const core::UTF8String& eventName);

		///
		/// Serialization helper method for creating basic timestamp data.
		/// @return Serialized data
		///
		core::UTF8String createTimestampData();

		///
		/// Serialization helper for event data.
		/// @param[in] name The event's type.
		/// @param[in] name Event name
		/// @param[in] parentActionID The ID of the action on which this event was reported.
		/// @param[inout] eventTimestamp uint64_t var that will be filled with the event timestamp
		/// @return The timestamp associated with the event(timestamp since session start time).
		///
		core::UTF8String buildEvent(EventType eventType, const core::UTF8String& name, int32_t parentActionID, uint64_t& eventTimestamp);

		///
		/// Serialization helper method for appending a key.
		/// @param[in] s reference to string containing serialized data
		/// @param[in] key key to append to string
		///
		void appendKey(core::UTF8String& s, const core::UTF8String& key);

		///
		/// Serialization helper method for adding key/value pairs with string values
		/// @param[in] s reference to string containing serialized data
		/// @param[in] key key to append to string
		/// @param[in] value the string value to add
		/// @returs the serialization data including the new key value pair
		///
		void addKeyValuePair(core::UTF8String& s, const core::UTF8String& key, const core::UTF8String& value);

		///
		/// Serialization helper method for adding key/value pairs with int32 values
		/// @param[in] s reference to string containing serialized data
		/// @param[in] key key to append to string
		/// @param[in] value the integer value to add
		/// @returs the serialization data including the new key value pair
		///
		void addKeyValuePair(core::UTF8String& s, const core::UTF8String& key, int32_t value);

		///
		/// Serialization helper method for adding key/value pairs with int64 values
		/// @param[in] s reference to string containing serialized data
		/// @param[in] key key to append to string
		/// @param[in] value the long value to add
		/// @returs the serialization data including the new key value pair
		///
		void addKeyValuePair(core::UTF8String& s, const core::UTF8String& key, int64_t value);

		///
		/// Serialization helper method for adding key/value pairs with double values
		/// @param[in] s reference to string containing serialized data
		/// @param[in] key key to append to string
		/// @param[in] value the double value to add
		/// @returs the serialization data including the new key value pair
		///
		void addKeyValuePair(core::UTF8String& s, const core::UTF8String& key, double value);

		///
		/// helper method for truncating name at max name size
		/// see @c MAX_NAME_LEN for the actual length
		/// @param[in] string string to trunctate
		/// @returns the trunctated string
		///
		core::UTF8String trunctate(const core::UTF8String& string);

		///
		/// Get a timestamp relative to the time this session (aka. beacon) was created.
		/// @param[in] timestamp The absolute timestamp for which to get a relative one.
		/// @return relative timestamp
		///
		int64_t getTimeSinceSessionStartTime(int64_t timestamp);

		///
		/// Add previously serialized action data to the beacon list
		/// @param[in] timestamp The timestamp when the action data occurred.
		/// @param[in] actionData Contains the serialized action data.
		///
		void addActionData(int64_t timestamp, const core::UTF8String& actionData);

		///
		/// Add previously serialized event data to the beacon list
		/// @param[in] timestamp The timestamp when the event data occurred.
		/// @param[in] actionData Contains the serialized event data.
		///
		void addEventData(int64_t timestamp, const core::UTF8String& eventData);

	private:
		/// Logger to write traces to
		std::shared_ptr<openkit::ILogger> mLogger;

		/// configuration
		std::shared_ptr<configuration::Configuration> mConfiguration;

		/// client IP Address
		core::UTF8String mClientIPAddress;

		/// timing provider
		std::shared_ptr<providers::ITimingProvider> mTimingProvider;

		/// thread id provider
		std::shared_ptr<providers::IThreadIDProvider> mThreadIDProvider;

		/// sequence number
		std::atomic<int32_t> mSequenceNumber;

		/// id
		std::atomic<int32_t> mID;

		/// session number
		uint32_t mSessionNumber;

		/// session start time
		int64_t mSessionStartTime;

		/// basic beacon data
		core::UTF8String mBasicBeaconData;

		///cache for beacons
		std::shared_ptr<caching::IBeaconCache> mBeaconCache;

		/// HTTP client configuration
		std::shared_ptr<configuration::HTTPClientConfiguration> mHTTPClientConfiguration;
	};
}
#endif