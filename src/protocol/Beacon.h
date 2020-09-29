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

#ifndef _PROTOCOL_BEACON_H
#define _PROTOCOL_BEACON_H

#include "IBeacon.h"
#include "OpenKit/ILogger.h"
#include "core/UTF8String.h"
#include "providers/ISessionIDProvider.h"
#include "providers/ITimingProvider.h"
#include "providers/IThreadIDProvider.h"
#include "providers/IPRNGenerator.h"
#include "core/configuration/IBeaconConfiguration.h"
#include "core/configuration/IHTTPClientConfiguration.h"
#include "core/configuration/IPrivacyConfiguration.h"
#include "core/objects/IActionCommon.h"
#include "core/objects/Session.h"
#include "core/objects/IWebRequestTracerInternals.h"
#include "core/caching/BeaconCache.h"
#include "protocol/IStatusResponse.h"
#include "EventType.h"

#include <memory>
#include <map>

namespace protocol
{
	///
	/// The Beacon class holds all the beacon data and the beacon protocol implementation.
	///
	class Beacon
		: public IBeacon
	{
	public:

		///
		/// Constructor for Beacon
		/// @param[in] logger to write traces to
		/// @param[in] beaconCache Cache storing beacon related data.
		/// @param[in] configuration Configuration object
		/// @param[in] clientIPAddress IP Address of the client
		/// @param[in] sessionIDProvider provider for retrieving a unique session number
		/// @param[in] threadIDProvider provider for thread ids
		/// @param[in] timingProvider timing provider used to retrieve timestamps
		///
		Beacon(
			std::shared_ptr<openkit::ILogger> logger,
			std::shared_ptr<core::caching::IBeaconCache> beaconCache,
			std::shared_ptr<core::configuration::IBeaconConfiguration> configuration,
			const char* clientIPAddress,
			std::shared_ptr<providers::ISessionIDProvider> sessionIDProvider,
			std::shared_ptr<providers::IThreadIDProvider> threadIDProvider,
			std::shared_ptr<providers::ITimingProvider> timingProvider
		);

		///
		/// Constructor for Beacon
		/// @param[in] logger to write traces to
		/// @param[in] beaconCache Cache storing beacon related data.
		/// @param[in] configuration Configuration object
		/// @param[in] clientIPAddress IP Address of the client
		/// @param[in] sessionIDProvider provider for retrieving a unique session number
		/// @param[in] threadIDProvider provider for thread ids
		/// @param[in] timingProvider timing provider used to retrieve timestamps
		/// @param[in] randomGenerator random number generator
		///
		Beacon(
			std::shared_ptr<openkit::ILogger> logger,
			std::shared_ptr<core::caching::IBeaconCache> beaconCache,
			std::shared_ptr<core::configuration::IBeaconConfiguration> configuration,
			const char* clientIPAddress,
			std::shared_ptr<providers::ISessionIDProvider> sessionIDProvider,
			std::shared_ptr<providers::IThreadIDProvider> threadIDProvider,
			std::shared_ptr<providers::ITimingProvider> timingProvider,
			std::shared_ptr<providers::IPRNGenerator> randomGenerator
		);

		///
		/// Destructor
		///
		~Beacon() override = default;

		int32_t createSequenceNumber() override;

		int64_t getCurrentTimestamp() const override;

		int32_t createID() override;

		core::UTF8String createTag(int32_t parentActionID, int32_t sequenceNumber) override;

		void addAction(std::shared_ptr<core::objects::IActionCommon> action) override;

		void startSession() override;

		void endSession() override;

		void reportValue(int32_t actionID, const core::UTF8String& valueName, int32_t value) override;

		void reportValue(int32_t actionID, const core::UTF8String& valueName, double value) override;

		void reportValue(int32_t actionID, const core::UTF8String& valueName, const core::UTF8String& value) override;

		void reportEvent(int32_t actionID, const core::UTF8String& eventName) override;

		void reportError
		(
			int32_t actionID,
			const core::UTF8String& errorName,
			int32_t errorCode,
			const core::UTF8String& reason
		) override;

		void reportCrash(
			const core::UTF8String& errorName,
			const core::UTF8String& reason,
			const core::UTF8String& stacktrace
		) override;

		void addWebRequest(
			int32_t parentActionID,
			std::shared_ptr<core::objects::IWebRequestTracerInternals> webRequestTracer
		) override;

		void identifyUser(const core::UTF8String& userTag) override;

		std::shared_ptr<protocol::IStatusResponse> send
		(
			std::shared_ptr<providers::IHTTPClientProvider> clientProvider
		) override;

		bool isEmpty() const override;

		void clearData() override;

		int32_t getSessionNumber() const override;

		int64_t getDeviceID() const override;

		const core::UTF8String& getClientIPAddress() const override;

		void updateServerConfiguration(std::shared_ptr<core::configuration::IServerConfiguration> serverConfig) override;

		bool isServerConfigurationSet() override;

		bool isCaptureEnabled() override;

		void enableCapture() override;

		void disableCapture() override;

		void setServerConfigurationUpdateCallback(core::configuration::ServerConfigurationUpdateCallback serverConfigurationUpdateCallback) override;

	private:
		///
		/// Serialization helper method for creating basic beacon protocol data.
		/// @returns Serialized data
		///
		core::UTF8String createImmutableBeaconData();

		///
		/// Serialization helper method for creating basic event data
		/// @returns Serialized data
		///
		core::UTF8String createBasicEventData(EventType eventType, const core::UTF8String& eventName);

		///
		/// Serialization helper method for creating basic timestamp data.
		/// @returns Serialized data
		///
		core::UTF8String createTimestampData();

		///
		/// Serialization helper for event data.
		/// @param[in] eventType The event's type.
		/// @param[in] name Event name
		/// @param[in] parentActionID The ID of the action on which this event was reported.
		/// @param[inout] eventTimestamp uint64_t var that will be filled with the event timestamp
		/// @returns The timestamp associated with the event(timestamp since session start time).
		///
		core::UTF8String buildEvent(
			EventType eventType,
			const core::UTF8String& name,
			int32_t parentActionID,
			uint64_t& eventTimestamp
		);

		///
		/// Serialization helper method for appending a key.
		/// @param[in] s reference to string containing serialized data
		/// @param[in] key key to append to string
		///
		void appendKey(core::UTF8String& s, const core::UTF8String& key);

		///
		/// Serialization helper method for adding key/value pairs with string values
		/// @param[in,out] s reference to string containing serialized data
		/// @param[in] key key to append to string
		/// @param[in] value the string value to add
		/// @returns the serialization data including the new key value pair
		///
		void addKeyValuePair(core::UTF8String& s, const core::UTF8String& key, const core::UTF8String& value);

		///
		/// Serialization helper method for adding key/value pairs with string values in case the given value is not empty
		/// @param s reference to string containing serialized data
		/// @param key  key to append to string
		/// @param value the string value to add
		///
		void addKeyValuePairIfNotEmpty(core::UTF8String& s, const core::UTF8String& key, const core::UTF8String & value);

		///
		/// Serialization helper method for adding key/value pairs with int32 values
		/// @param[in,out] s reference to string containing serialized data
		/// @param[in] key key to append to string
		/// @param[in] value the integer value to add
		/// @returns the serialization data including the new key value pair
		///
		void addKeyValuePair(core::UTF8String& s, const core::UTF8String& key, int32_t value);

		///
		/// Serialization helper method for adding key/value pairs with int64 values
		/// @param[in,out] s reference to string containing serialized data
		/// @param[in] key key to append to string
		/// @param[in] value the long value to add
		/// @returns the serialization data including the new key value pair
		///
		void addKeyValuePair(core::UTF8String& s, const core::UTF8String& key, int64_t value);

		///
		/// Serialization helper method for adding key/value pairs with double values
		/// @param[in,out] s reference to string containing serialized data
		/// @param[in] key key to append to string
		/// @param[in] value the double value to add
		/// @returns the serialization data including the new key value pair
		///
		void addKeyValuePair(core::UTF8String& s, const core::UTF8String& key, double value);

		///
		/// helper method for truncating name at max name size
		/// see @c MAX_NAME_LEN for the actual length
		/// @param[in] string string to truncate
		/// @returns the truncated string
		///
		static core::UTF8String truncate(const core::UTF8String& string);

		///
		/// Get a timestamp relative to the time this session (aka. beacon) was created.
		/// @param[in] timestamp The absolute timestamp for which to get a relative one.
		/// @returns relative timestamp
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
		/// @param[in] eventData Contains the serialized event data.
		///
		void addEventData(int64_t timestamp, const core::UTF8String& eventData);

		///
		/// Generate serialization for the mutable part of the beaon
		/// e.g. multiplicity and timestamp
		/// @returns the mutable beacon data
		///
		core::UTF8String getMutableBeaconData();

		///
		/// Generate multiplicity data
		/// @returns the multiplicity data
		///
		core::UTF8String createMultiplicityData();

	private:
		/// Logger to write traces to
		const std::shared_ptr<openkit::ILogger> mLogger;

		///cache for beacons
		const std::shared_ptr<core::caching::IBeaconCache> mBeaconCache;

		/// configuration object required for this beacon
		const std::shared_ptr<core::configuration::IBeaconConfiguration> mBeaconConfiguration;

		/// client IP Address
		core::UTF8String mClientIPAddress;

		/// thread id provider
		const std::shared_ptr<providers::IThreadIDProvider> mThreadIDProvider;

		/// timing provider
		const std::shared_ptr<providers::ITimingProvider> mTimingProvider;

		///random generator
		const std::shared_ptr<providers::IPRNGenerator> mRandomGenerator;

		/// device id
		int64_t mDeviceID;

		/// sequence number
		std::atomic<int32_t> mSequenceNumber;

		/// id
		std::atomic<int32_t> mID;

		/// beacon ID internally used to identifiy the session/beacon (might be equal to mSessionNumber)
		int32_t mBeaconId;

		/// session number
		int32_t mSessionNumber;

		/// session start time
		int64_t mSessionStartTime;

		/// basic beacon data
		core::UTF8String mImmutableBasicBeaconData;
	};
}
#endif
