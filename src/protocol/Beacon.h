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

#include "core/UTF8String.h"
#include "providers/ITimingProvider.h"
#include "providers/IThreadIDProvider.h"
#include "configuration/Configuration.h"
#include "core/Action.h"
#include "core/Session.h"
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
		/// @param[in] configuration Configuration object
		/// @param[in] clientIPAddress IP Address of the client
		/// @param[in] timingProvider timing provider used to retrieve timestamps
		///
		Beacon(std::shared_ptr<configuration::Configuration> configuration, const core::UTF8String clientIPAddress, std::shared_ptr<providers::IThreadIDProvider> threadIDProvider , std::shared_ptr<providers::ITimingProvider> timingProvider);

		///
		/// Create unique sequence number
		/// The sequence number returned is only unique per Beacon.
		/// Calling this method on two different Beacon instances, might give the same result.
		/// @returns a unique sequencen number;
		///
		int32_t createSequenceNumber();

		///
		/// Get the current timestamp in milliseconds by delegating to TimingProvider
		/// @returns Current timestamp in milliseconds
		///
		int64_t getCurrentTimestamp() const;

		///
		/// Create a unique identifier.
		/// -The identifier returned is only unique per Beacon.
		/// -Calling this method on two different Beacon instances, might give the same result.
		/// @returns a unique identifier
		///
		int32_t createID();

		///
		/// Add Action to Beacon
		/// The serialized data is added to the Beacon
		/// @param[in] action action to add to the Beacon
		///
		void addAction(std::shared_ptr<core::Action> action);

		///
		/// Add Session to Beacon when session is ended.
		/// @param[in] session ended session that is added to the Beacon
		///
		void endSession(std::shared_ptr<core::Session> session);
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
		void storeAction(int64_t timestamp, const core::UTF8String& actionData);

		///
		/// Add previously serialized event data to the beacon list
		/// @param[in] timestamp The timestamp when the event data occurred.
		/// @param[in] actionData Contains the serialized event data.
		///
		void storeEvent(int64_t timestamp, const core::UTF8String& eventData);

	private:
		/// configuration
		std::shared_ptr<configuration::Configuration> mConfiguration;

		/// client IP Address
		const core::UTF8String mClientIPAddress;

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

		/// container for action data
		std::map<int64_t, core::UTF8String> mActionDataList;

		/// container for event data
		std::map<int64_t, core::UTF8String> mEventDataList;
	};
}
#endif