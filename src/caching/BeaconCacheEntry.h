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

#ifndef _CACHING_BEACONCACHEENTRY_H
#define _CACHING_BEACONCACHEENTRY_H

#include "core/UTF8String.h"
#include "caching/BeaconCacheRecord.h"

#include <cstdint>
#include <vector>
#include <memory>
#include <list>
#include <mutex>

namespace caching
{
	///
	/// Represents an entry in the @ref BeaconCache.
	///
	/// The caller is responsible to lock this element, before the first method is invoked using
	/// @ref lock() and after the last operation is invoked @ref unlock() must be called.
	///
	class BeaconCacheEntry
	{
	public:
		///
		/// Default constructor
		///
		BeaconCacheEntry();


		std::mutex& getLock();

		///
		/// Lock this @c BeaconCacheEntry for reading & writing.
		/// When locking is no longer required, @ref unlock() must be called.
		///
		//void lock();

		///
		/// Release this @c BeaconCacheEntry lock, so that other threads can access this object.
		///
		/// When calling this method ensure @ref lock() was called before.
		///
		//void unlock();

		///
		/// Add new event data record to cache.
		///
		/// @param[in] record The new record to add.
		///
		void addEventData(const BeaconCacheRecord& record);

		///
		/// Add new action data record to the cache.
		///
		/// @param[in] record The new record to add.
		///
		void addActionData(const BeaconCacheRecord& record);

		///
		/// Test if data shall be copied, before creating chunks for sending.
		///
		/// @return @c true if data must be copied, @c false otherwise.
		///
		bool needsDataCopyBeforeChunking() const;

		///
		/// Copy data for sending.
		///
		void copyDataForChunking();

		///
		/// Get next data chunk to send to the Dynatrace backend system.
		///
		/// This method is called from beacon sending thread.
		///
		/// @param[in] chunkPrefix The prefix to add to each chunk.
		/// @param[in] maxSize     The maximum size in characters for one chunk.
		/// @param[in] delimiter   The delimiter between data chunks.
		/// @return The string to send or an empty string if there is no more data to send.
		///
		core::UTF8String getChunk(const core::UTF8String& chunkPrefix, int32_t maxSize, const char* delimiter);

		///
		/// Remove data that was previously marked for sending when getNextChunk was called.
		///
		void removeDataMarkedForSending();

		///
		/// This method removes the marked for sending and prepends the copied data back to the data.
		///
		void resetDataMarkedForSending();

		///
		/// Get total number of bytes used.
		///
		/// Note: The number of bytes is calculated from the lists where active records are added.
		/// Data that is currently being sent is not taken into account, since we assume sending is
		/// successful and therefore this data is just temporarily stored.
		///
		/// @return Sum of data size in bytes for each @ref BeaconCacheRecord.
		///
		int64_t getTotalNumberOfBytes() const;

		///
		/// Remove all @ref BeaconCacheRecord from event and action data which are older than given minTimestamp
		///
		/// Records which are currently being sent are not evicted.
		///
		/// @param[in] minTimestamp The minimum timestamp allowed.
		/// @return The total number of removed records.
		///
		int32_t removeRecordsOlderThan(int64_t minTimestamp);

		///
		/// Remove up to @c numRecords records from event & action data, compared by their age.
		///
		/// Note not all event/action data entries are traversed, only the first action data & first event
		/// data is removed and compared against each other, which one to remove first. If the first action's timestamp and
		/// first event's timestamp are equal, the first event is removed.
		///
		/// @param[in] numRecords The number of records.
		/// @return Number of actually removed records.
		///
		int32_t removeOldestRecords(int32_t numRecords);

		///
		/// Get a deep copy of event data.
		///
		/// This method shall only be used for testing purposes.
		///
		const std::list<BeaconCacheRecord> getEventData() const;

		///
		/// Get a deep copy of action data.
		///
		/// This method shall only be used for testing purposes.
		///
		const std::list<BeaconCacheRecord> getActionData() const;

		///
		/// Get a deep copy of event data being sent.
		///
		/// This method shall only be used for testing purposes.
		///
		const std::list<BeaconCacheRecord> getEventDataBeingSent() const;

		///
		/// Get a deep copy of action data being sent.
		///
		/// This method shall only be used for testing purposes.
		///
		const std::list<BeaconCacheRecord> getActionDataBeingSent() const;

	private:
		///
		/// Test if there is more data to send (to chunk).
		///
		/// @return @c true if there is more data, @c false otherwise.
		///
		bool hasDataToSend() const;

		///
		/// Get the next chunk.
		/// @param[in] chunkPrefix The prefix to add to each chunk.
		/// @param[in] maxSize     The maximum size in characters for one chunk.
		/// @param[in] delimiter   The delimiter between data chunks.
		/// @return The string to send or an empty string if there is no more data to send.
		///
		core::UTF8String getNextChunk(const core::UTF8String& chunkPrefix, uint32_t maxSize, const char* delimiter);

		///
		/// Iterates (up to the @c maxSize) the provided @c dataBeingSent list and appends the data together with the @c delimiter to the provided @c chunk.
		/// param[in,out] chunk the chunk to which the data is appended
		/// param[in] dataBeingSent the list of record containing the data to append
		/// param[in] maxSize in characters for one chunk. Up to this size data (if available) is appended
		/// param[in] delimiter the delimiter between data chunks 
		///
		static void chunkifyDataList(core::UTF8String& chunk, std::list<BeaconCacheRecord>& dataBeingSent, uint32_t maxSize, const char* delimiter);

		///
		/// Remove all @ref BeaconCacheRecord from @c records.
		/// @param[in] minTimestamp The minimum timestamp allowed.
		/// @return The number of records removed from {@code records}.
		///
		static int32_t removeRecordsOlderThan(std::list<BeaconCacheRecord>& records, int64_t minTimestamp);

	private:

		///	List storing all active event data.
		std::list<BeaconCacheRecord> mEventData;

		///	List storing all active action data.
		std::list<BeaconCacheRecord> mActionData;

		/// Lock object for locking access to session & event data.
		std::mutex mMutex;

		///	List storing all event data being sent.
		std::list<BeaconCacheRecord> mEventDataBeingSent;

		///	List storing all action data being sent.
		std::list<BeaconCacheRecord> mActionDataBeingSent;

		int64_t mTotalNumBytes;
	};
}

#endif
