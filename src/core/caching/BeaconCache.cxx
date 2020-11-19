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

#include "BeaconCache.h"

#include <mutex>
#include <inttypes.h> // for PRId64 macro

using namespace core::caching;

BeaconCache::BeaconCache(std::shared_ptr<openkit::ILogger> logger)
	: mLogger(logger)
	, observers()
	, mGlobalCacheLock()
	, mBeacons()
	, mCacheSizeInBytes(0)
{

}

void BeaconCache::addObserver(IObserver* observer)
{
	if (observer != nullptr)
	{
		observers.push_back(observer);
	}
}

void BeaconCache::addEventData(const BeaconKey& beaconKey, int64_t timestamp, const core::UTF8String& data)
{
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("BeaconCache addEventData(sn=%d, seq=%d, timestamp=%" PRId64 ", data='%s')",
			beaconKey.getBeaconId(), beaconKey.getBeaconSequenceNumber(), timestamp, data.getStringData().c_str());
	}

	// get a reference to the cache entry
	auto entry = getCachedEntryOrInsert(beaconKey);

	BeaconCacheRecord record(timestamp, data);

	std::unique_lock<std::mutex> lock(entry->getLock());
	entry->addEventData(record);
	lock.unlock();

	// update cache stats
	mCacheSizeInBytes += record.getDataSizeInBytes();

	// notify observers
	onDataAdded();
}

void BeaconCache::addActionData(const BeaconKey& beaconKey, int64_t timestamp, const core::UTF8String& data)
{
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("BeaconCache addActionData(sn=%d, seq=%d, timestamp=%" PRId64 ", data='%s')",
			beaconKey.getBeaconId(), beaconKey.getBeaconSequenceNumber(), timestamp, data.getStringData().c_str());
	}

	// get a reference to the cache entry
	auto entry = getCachedEntryOrInsert(beaconKey);

	BeaconCacheRecord record(timestamp, data);
	std::unique_lock<std::mutex> lock(entry->getLock());
	entry->addActionData(record);
	lock.unlock();

	// update cache stats
	mCacheSizeInBytes += record.getDataSizeInBytes();

	// notify observers
	onDataAdded();
}

void BeaconCache::deleteCacheEntry(const BeaconKey& beaconKey)
{
	core::util::ScopedWriteLock lock(mGlobalCacheLock);
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("BeaconCache deleteCacheEntry(sn=%d, seq=%d)",
			beaconKey.getBeaconId(), beaconKey.getBeaconSequenceNumber());
	}

	auto it = mBeacons.find(beaconKey);
	if (it != mBeacons.end())
	{
		mCacheSizeInBytes -= it->second->getTotalNumberOfBytes();
		mBeacons.erase(it);
	}

	lock.unlock();
}

const core::UTF8String BeaconCache::getNextBeaconChunk(const BeaconKey& beaconKey, const core::UTF8String& chunkPrefix, int32_t maxSize, const core::UTF8String& delimiter)
{
	auto entry = getCachedEntry(beaconKey);
	if (entry == nullptr)
	{
		// a cache entry for the given beaconID does not exist
		return core::UTF8String();
	}

	if (entry->needsDataCopyBeforeChunking())
	{
		// both entries are null, prepare data for sending
		int64_t numBytes = 0;
		std::unique_lock<std::mutex> lock(entry->getLock());
		numBytes = entry->getTotalNumberOfBytes();
		entry->copyDataForChunking();
		lock.unlock();

		// assumption: sending will work fine, and everything we copied will be removed quite soon
		mCacheSizeInBytes -= numBytes;
	}

	// data for chunking is available
	return entry->getChunk(chunkPrefix, maxSize, delimiter);
}

void BeaconCache::removeChunkedData(const BeaconKey& beaconKey)
{
	auto entry = getCachedEntry(beaconKey);
	if (entry == nullptr)
	{
		// a cache entry for the given beaconID does not exist
		return;
	}

	entry->removeDataMarkedForSending();
}

void BeaconCache::resetChunkedData(const BeaconKey& beaconKey)
{
	auto entry = getCachedEntry(beaconKey);
	if (entry == nullptr)
	{
		// a cache entry for the given beaconID does not exist
		return;
	}

	int64_t numBytes = 0;
	std::unique_lock<std::mutex> lock(entry->getLock());
	int64_t oldSize = entry->getTotalNumberOfBytes();
	entry->resetDataMarkedForSending();
	int64_t newSize = entry->getTotalNumberOfBytes();
	numBytes = newSize - oldSize;
	lock.unlock();

	mCacheSizeInBytes += numBytes;

	// notify observers
	onDataAdded();
}

std::shared_ptr<BeaconCacheEntry> BeaconCache::getCachedEntryOrInsert(const BeaconKey& beaconKey)
{
	auto entry = getCachedEntry(beaconKey);
	if (entry == nullptr)
	{
		// does not exist, and needs to be inserted
		core::util::ScopedWriteLock lock(mGlobalCacheLock);

		// double check since this could have been added in the mean time
		auto it = mBeacons.find(beaconKey);
		if (it == mBeacons.end())
		{
			entry = std::make_shared<BeaconCacheEntry>();
			mBeacons.insert(std::make_pair(beaconKey, entry));
		}
		else
		{
			entry = it->second;
		}

		lock.unlock();
	}

	return entry;
}

const std::vector<core::UTF8String> BeaconCache::getEvents(const BeaconKey& beaconKey)
{
	auto entry = getCachedEntry(beaconKey);
	if (entry == nullptr)
	{
		// entry not found
		return {};
	}

	std::vector<core::UTF8String> events;
	std::unique_lock<std::mutex> lock(entry->getLock());
	events = extractData(entry->getEventData());
	lock.unlock();
	return events;
}

const std::list<BeaconCacheRecord> BeaconCache::getEventsBeingSent(const BeaconKey& beaconKey)
{
	auto entry = getCachedEntry(beaconKey);
	if (entry == nullptr)
	{
		// entry not found
		return {};
	}

	return entry->getEventDataBeingSent();
}

const std::vector<core::UTF8String> BeaconCache::getActions(const BeaconKey& beaconKey)
{
	auto entry = getCachedEntry(beaconKey);
	if (entry == nullptr)
	{
		// entry not found
		return {};
	}

	std::vector<core::UTF8String> actions;
	std::unique_lock<std::mutex> lock(entry->getLock());
	actions = extractData(entry->getActionData());
	lock.unlock();
	return actions;
}

const std::list<BeaconCacheRecord> BeaconCache::getActionsBeingSent(const BeaconKey& beaconKey)
{
	auto entry = getCachedEntry(beaconKey);
	if (entry == nullptr)
	{
		// entry not found
		return {};
	}

	return entry->getActionDataBeingSent();
}

std::vector<core::UTF8String> BeaconCache::extractData(const std::list<BeaconCacheRecord>& eventData)
{
	std::vector<core::UTF8String> result;
	for (auto const& value : eventData)
	{
		result.push_back(value.getData());
	}
	return result;
}

std::shared_ptr<BeaconCacheEntry> BeaconCache::getCachedEntry(const BeaconKey& beaconKey)
{
	std::shared_ptr<BeaconCacheEntry> entry = nullptr;

	// acquire read lock and get the entry
	core::util::ScopedReadLock lock(mGlobalCacheLock);
	auto it = mBeacons.find(beaconKey);
	if (it != mBeacons.end())
	{
		entry = it->second;
	}
	lock.unlock();

	return entry;
}

const std::unordered_set<BeaconKey, BeaconKey::Hash> BeaconCache::getBeaconKeys()
{
	std::unordered_set<BeaconKey, BeaconKey::Hash> result;

	core::util::ScopedReadLock lock(mGlobalCacheLock);
	for (auto const& beacon : mBeacons)
	{
		result.insert(beacon.first);
	}
	lock.unlock();

	return result;
}

uint32_t BeaconCache::evictRecordsByAge(const BeaconKey& beaconKey, int64_t minTimestamp)
{
	auto entry = getCachedEntry(beaconKey);
	if (entry == nullptr)
	{
		// already removed
		return 0;
	}

	std::unique_lock<std::mutex> lock(entry->getLock());
	uint32_t numRecordsRemoved = entry->removeRecordsOlderThan(minTimestamp);
	lock.unlock();

	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("BeaconCache evictRecordsByAge(sn=%d, seq=%d, minTimestamp=%" PRId64 ") has evicted %u records", 
			beaconKey.getBeaconId(), beaconKey.getBeaconSequenceNumber(), minTimestamp, numRecordsRemoved);
	}

	return numRecordsRemoved;
}

uint32_t BeaconCache::evictRecordsByNumber(const BeaconKey& beaconKey, uint32_t numRecords)
{
	auto entry = getCachedEntry(beaconKey);
	if (entry == nullptr)
	{
		// already removed
		return 0;
	}

	std::unique_lock<std::mutex> lock(entry->getLock());
	uint32_t numRecordsRemoved = entry->removeOldestRecords(numRecords);
	lock.unlock();

	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("BeaconCache evictRecordsByNumber(sn=%d, seq=%d, numRecords=%u) has evicted %u records",
			beaconKey.getBeaconId(), beaconKey.getBeaconSequenceNumber(), numRecords, numRecordsRemoved);
	}

	return numRecordsRemoved;
}

int64_t BeaconCache::getNumBytesInCache() const
{
	return mCacheSizeInBytes;
}

void BeaconCache::onDataAdded()
{
	for (auto iter = observers.begin(); iter != observers.end(); ++iter)
	{
		(*iter)->update();
	}
}

bool BeaconCache::isEmpty(const BeaconKey& beaconKey)
{
	auto entry = getCachedEntry(beaconKey);
	if (entry == nullptr)
	{
		// already removed
		return true;
	}

	std::unique_lock<std::mutex> lock(entry->getLock());
	bool isEmpty = entry->getTotalNumberOfBytes() == 0;
	lock.unlock();

	return isEmpty;
}