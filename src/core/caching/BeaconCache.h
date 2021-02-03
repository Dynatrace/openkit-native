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

#ifndef _CORE_CACHING_BEACONCACHE_H
#define _CORE_CACHING_BEACONCACHE_H

#include "OpenKit/ILogger.h"
#include "IBeaconCache.h"
#include "BeaconCacheEntry.h"

#include "core/util/ScopedReadLock.h"
#include "core/util/ScopedWriteLock.h"

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <atomic>
#include <list>

namespace core
{
	namespace caching
	{
		///
		/// Class used in OpenKit to cache serialized @ref protocol::Beacon data.
		///
		/// This cache needs to deal with high concurrency, since it's possible that a lot of threads insert new data concurrently.
		/// Furthermore two OpenKit internal threads are also accessing the cache.
		///
		class BeaconCache : public IBeaconCache
		{
		public:
			///
			/// Constructor
			///
			BeaconCache(std::shared_ptr<openkit::ILogger> logger);

			///
			/// destructor
			///
			~BeaconCache() override = default;

			///
			/// Delete the copy constructor
			///
			BeaconCache(const BeaconCache&) = delete;

			///
			/// Delete the assignment operator
			///
			BeaconCache& operator = (const BeaconCache &) = delete;

			void addObserver(IObserver* observer) override;

			void addEventData(const BeaconKey& beaconKey, int64_t timestamp, const core::UTF8String& data) override;

			void addActionData(const BeaconKey& beaconKey, int64_t timestamp, const core::UTF8String& data) override;

			void deleteCacheEntry(const BeaconKey& beaconKey) override;

			const core::UTF8String getNextBeaconChunk(const BeaconKey& beaconKey, const core::UTF8String& chunkPrefix, int32_t maxSize, const core::UTF8String& delimiter) override;

			void removeChunkedData(const BeaconKey& beaconKey) override;

			void resetChunkedData(const BeaconKey& beaconKey) override;

			///
			/// Get a deep copy of events collected so far.
			///
			/// Although this method is intended for debugging purposes only, it still does appropriate locking.
			///
			/// @param[in] beaconKey The beacon key for which to retrieve the events.
			/// @return List of event data.
			///
			const std::vector<core::UTF8String> getEvents(const BeaconKey& beaconKey);

			///
			/// Get a deep copy of events that are about to be sent.
			///
			/// This method is only intended for internal unit tests.
			///
			/// @param[in] beaconKey The beacon key for which to retrieve the events.
			/// @return List of event data.
			///
			const std::list<BeaconCacheRecord> getEventsBeingSent(const BeaconKey& beaconKey);

			///
			/// Get a deep copy of actions collected so far.
			///
			/// Although this method is intended for debugging purposes only, it still does appropriate locking.
			///
			/// @param[in] beaconKey The beacon key for which to retrieve the actions.
			/// @return List of action data.
			///
			const std::vector<core::UTF8String> getActions(const BeaconKey& beaconKey);

			///
			/// Get a deep copy of actions that are about to be sent.
			///
			/// This method is only intended for internal unit tests.
			///
			/// @param[in] beaconKey The beacon key for which to retrieve the actions.
			/// @return List of action data.
			///
			const std::list<BeaconCacheRecord> getActionsBeingSent(const BeaconKey& beaconKey);

			const std::unordered_set<BeaconKey, BeaconKey::Hash> getBeaconKeys() override;

			uint32_t evictRecordsByAge(const BeaconKey& beaconKey, int64_t minTimestamp) override;

			uint32_t evictRecordsByNumber(const BeaconKey& beaconKey, uint32_t numRecords) override;

			int64_t getNumBytesInCache() const override;

			bool isEmpty(const BeaconKey& beaconKey) override;

		private:
			///
			/// Get cached @ref BeaconCacheEntry or insert new one if nothing exists for given BeaconKey.
			/// 
			/// @param beaconKey[in] The beacon key to search for.
			/// @return The already cached entry or newly created one.
			///
			std::shared_ptr<BeaconCacheEntry> getCachedEntryOrInsert(const BeaconKey& beaconKey);

			///
			/// Get cached @ref BeaconCacheEntry or @c nullptr if nothing exists for given BeaconKey.
			/// 
			/// @param beaconKey[in] The beacon key to search for.
			/// @return The cached entry or @c nullptr.
			///
			std::shared_ptr<BeaconCacheEntry> getCachedEntry(const BeaconKey& beaconKey);

			///
			/// Helper method to extract the data from the provided records.
			/// 
			/// @param[in] eventData the records from which to extract the data
			/// @return the extracted data
			///
			static std::vector<core::UTF8String> extractData(const std::list<BeaconCacheRecord>& eventData);

			///
			/// Call this method when something was added (size of cache increased).
			///
			void onDataAdded();

		private:
			/// Logger to write traces to
			std::shared_ptr<openkit::ILogger> mLogger;

			/// Observers to be notified about data being added
			std::vector<IObserver*> observers;

			/// Locks the cache for read and write access
			core::util::ReadWriteLock mGlobalCacheLock;

			/// The central part of the cache are the beacons (key=beaconID, value=
			std::unordered_map<BeaconKey, std::shared_ptr<BeaconCacheEntry>, BeaconKey::Hash> mBeacons;

			/// Sum of all record's data size estimation.
			std::atomic<int64_t> mCacheSizeInBytes;
		};
	}
}

#endif
