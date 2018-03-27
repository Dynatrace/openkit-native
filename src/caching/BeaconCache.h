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

#ifndef _CACHING_BEACONCACHE_H
#define _CACHING_BEACONCACHE_H

#include "caching/IObserver.h"
#include "caching/IBeaconCache.h"
#include "core/util/ScopedReadLock.h"
#include "core/util/ScopedWriteLock.h"
#include "caching/BeaconCacheEntry.h"

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <atomic>
#include <list>

namespace caching
{
	///
	/// Class used in OpenKit to cache serialized @ref Beacon data.
	///
	/// This cache needs to deal with high concurrency, since it's possible that a lot of threads insert new data concurrently.
	/// Furthermore two OpenKit internal threads are also accessing the cache.
	///
	class BeaconCache : public IBeaconCache
	{
	public:
		///
		/// Default constructor
		///
		BeaconCache();

		///
		/// destructor
		///
		virtual ~BeaconCache() {}

		///
		/// Delete the copy constructor
		///
		BeaconCache(const BeaconCache&) = delete;

		///
		/// Delete the assignment operator
		///
		BeaconCache& operator = (const BeaconCache &) = delete;

		virtual void addObserver(IObserver* observer) override;

		virtual void addEventData(int32_t beaconID, int64_t timestamp, const core::UTF8String& data) override;

		virtual void addActionData(int32_t beaconID, int64_t timestamp, const core::UTF8String& data) override;

		virtual void deleteCacheEntry(int32_t beaconID) override;

		virtual const core::UTF8String getNextBeaconChunk(int32_t beaconID, const core::UTF8String& chunkPrefix, int32_t maxSize, const core::UTF8String& delimiter) override;

		virtual void removeChunkedData(int32_t beaconID) override;

		virtual void resetChunkedData(int32_t beaconID) override;

		///
		/// Get a deep copy of events collected so far.
		///
		/// Although this method is intended for debugging purposes only, it still does appropriate locking.
		///
		/// @param[in] beaconID The beacon id for which to retrieve the events.
		/// @return List of event data.
		///
		const std::vector<core::UTF8String> getEvents(int32_t beaconID);

		///
		/// Get a deep copy of events that are about to be sent.
		///
		/// This method is only intended for internal unit tests.
		///
		/// @param[in] beaconID The beacon id for which to retrieve the events.
		/// @return List of event data.
		///
		const std::list<BeaconCacheRecord> getEventsBeingSent(int32_t beaconID);

		///
		/// Get a deep copy of actions collected so far.
		///
		/// Although this method is intended for debugging purposes only, it still does appropriate locking.
		///
		/// @param[in] beaconID The beacon id for which to retrieve the actions.
		/// @return List of action data.
		///
		const std::vector<core::UTF8String> getActions(int32_t beaconID);
		
		///
		/// Get a deep copy of actions that are about to be sent.
		///
		/// This method is only intended for internal unit tests.
		///
		/// @param[in] beaconID The beacon id for which to retrieve the actions.
		/// @return List of action data.
		///
		const std::list<BeaconCacheRecord> getActionsBeingSent(int32_t beaconID);

		virtual const std::unordered_set<int32_t> getBeaconIDs() override;

		virtual uint32_t evictRecordsByAge(int32_t beaconID, int64_t minTimestamp) override;

		virtual uint32_t evictRecordsByNumber(int32_t beaconID, uint32_t numRecords) override;

		virtual const int64_t getNumBytesInCache() const override;

		virtual bool isEmpty(int32_t beaconID) override;

	private:
		///
		/// Get cached @ref BeaconCacheEntry or insert new one if nothing exists for given @c beaconID.
		/// @param beaconID The beacon id to search for.
		/// @return The already cached entry or newly created one.
		///
		std::shared_ptr<BeaconCacheEntry> getCachedEntryOrInsert(int beaconID);

		///
		/// Get cached @ref BeaconCacheEntry or @c nullptr if nothing exists for given @c beaconID.
		/// @param beaconID The beacon id to search for.
		/// @return The cached entry or @c nullptr.
		///
		std::shared_ptr<BeaconCacheEntry> getCachedEntry(int32_t beaconID);

		///
		/// Helper method to extract the data from the provided records.
		/// @param[in] eventData the records from which to extract the data
		/// @return the extracted data
		///
		static std::vector<core::UTF8String> extractData(const std::list<BeaconCacheRecord>& eventData);

		///
		/// Call this method when something was added (size of cache increased).
		///
		void onDataAdded();

	private:
		/// Observers to be notified about data being added
		std::vector<IObserver*> observers;

		/// Locks the cache for read and write access
		core::util::ReadWriteLock mGlobalCacheLock;

		/// The central part of the cache are the beacons (key=beaconID, value=
		std::unordered_map<int32_t, std::shared_ptr<BeaconCacheEntry>> mBeacons;

		/// Sum of all record's data size estimation.
		std::atomic<int64_t> mCacheSizeInBytes;
	};
}

#endif
