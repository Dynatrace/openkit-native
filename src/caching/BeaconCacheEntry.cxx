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

#include "BeaconCacheEntry.h"

using namespace caching;

BeaconCacheEntry::BeaconCacheEntry()
	: mEventData()
	, mActionData()
	, mMutex()
	, mEventDataBeingSent()
	, mActionDataBeingSent()
	, mTotalNumBytes(0)
{

}

std::mutex& BeaconCacheEntry::getLock()
{
	return mMutex;
}

void BeaconCacheEntry::addEventData(const BeaconCacheRecord& record)
{
	mEventData.push_back(record);
	mTotalNumBytes += record.getDataSizeInBytes();
}

void BeaconCacheEntry::addActionData(const BeaconCacheRecord& record)
{
	mActionData.push_back(record);
	mTotalNumBytes += record.getDataSizeInBytes();
}

bool BeaconCacheEntry::needsDataCopyBeforeChunking() const
{
	// no data currently being sent AND some data available
	return mActionDataBeingSent.empty() && mEventDataBeingSent.empty() && (!mEventData.empty() || !mActionData.empty());
}

void BeaconCacheEntry::copyDataForChunking()
{
	mActionDataBeingSent.splice(mActionDataBeingSent.begin(), mActionData);
	mEventDataBeingSent.splice(mEventDataBeingSent.begin(), mEventData);

	mTotalNumBytes = 0;
}

const core::UTF8String BeaconCacheEntry::getChunk(const core::UTF8String& chunkPrefix, size_t maxSize, const core::UTF8String& delimiter)
{
	if (!hasDataToSend())
	{
		// nothing to send - reset lists, so next time lists get copied again
		mEventDataBeingSent.clear();
		mActionDataBeingSent.clear();
		return core::UTF8String();
	}
	return getNextChunk(chunkPrefix, maxSize, delimiter);
}

bool BeaconCacheEntry::hasDataToSend() const
{
	return !mEventDataBeingSent.empty() || !mActionDataBeingSent.empty();
}

const core::UTF8String BeaconCacheEntry::getNextChunk(const core::UTF8String& chunkPrefix, size_t maxSize, const core::UTF8String& delimiter)
{
	core::UTF8String chunk;

	// append the chunk prefix
	chunk.concatenate(chunkPrefix);

	// append data from both lists
	// note the order is currently important -> event data goes first, then action data
	chunkifyDataList(chunk, mEventDataBeingSent, maxSize, delimiter);
	chunkifyDataList(chunk, mActionDataBeingSent, maxSize, delimiter);

	return chunk;
}

void BeaconCacheEntry::chunkifyDataList(core::UTF8String& chunk, std::list<BeaconCacheRecord>& dataBeingSent, size_t maxSize, const core::UTF8String& delimiter)
{
	auto it = dataBeingSent.begin();
	while (it != dataBeingSent.end() && chunk.getStringLength() <= maxSize)
	{
		// mark the record for sending
		it->markForSending();

		// append delimiter & data
		chunk.concatenate(delimiter);
		chunk.concatenate(it->getData());

		it++;
	}
}

void BeaconCacheEntry::removeDataMarkedForSending()
{
	if (!hasDataToSend())
	{
		// data has not been copied yet
		return;
	}

	auto it = mEventDataBeingSent.begin();
	while (it != mEventDataBeingSent.end())
	{
		if (it->isMarkedForSending())
		{
			it = mEventDataBeingSent.erase(it);
		}
		else
		{
			++it;
		}
	}

	if (it == mEventDataBeingSent.end())
	{
		// only check action data, if all event data has been traversed, otherwise it's just waste of cpu time
		it = mActionDataBeingSent.begin();
		while (it != mActionDataBeingSent.end())
		{
			if (it->isMarkedForSending())
			{
				it = mActionDataBeingSent.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
}

void BeaconCacheEntry::resetDataMarkedForSending()
{
	if (!hasDataToSend())
	{
		// data has not been copied yet
		return;
	}

	// reset the "sending marks" and in the same traversal count the bytes which are added back
	int64_t numBytes = 0;
	for (auto it = mEventDataBeingSent.begin(); it != mEventDataBeingSent.end(); ++it)
	{
		it->unsetSending();
		numBytes += it->getDataSizeInBytes();
	}

	for (auto it = mActionDataBeingSent.begin(); it != mActionDataBeingSent.end(); ++it)
	{
		it->unsetSending();
		numBytes += it->getDataSizeInBytes();
	}

	// merge data
	mEventData.splice(mEventData.begin(), mEventDataBeingSent);
	mActionData.splice(mActionData.begin(), mActionDataBeingSent);

	mTotalNumBytes += numBytes;
}

int64_t BeaconCacheEntry::getTotalNumberOfBytes() const
{
	return mTotalNumBytes;
}

int32_t BeaconCacheEntry::removeRecordsOlderThan(int64_t minTimestamp)
{
	int32_t numRecordsRemoved = removeRecordsOlderThan(mEventData, minTimestamp);
	numRecordsRemoved += removeRecordsOlderThan(mActionData, minTimestamp);

	return numRecordsRemoved;
}

int32_t BeaconCacheEntry::removeRecordsOlderThan(std::list<BeaconCacheRecord>& records, int64_t minTimestamp)
{
	int32_t numRecordsRemoved = 0;
	auto it = records.begin();
	while (it != records.end())
	{
		auto record = *it;
		if (record.getTimestamp() < minTimestamp)
		{
			it = records.erase(it);
			numRecordsRemoved++;
		}
		else
		{
			++it;
		}
	}

	return numRecordsRemoved;
}

int32_t BeaconCacheEntry::removeOldestRecords(int32_t numRecords)
{
	int32_t numRecordsRemoved = 0;

	auto eventsIterator = mEventData.begin();
	auto actionsIterator = mActionData.begin();

	while (numRecordsRemoved < numRecords && (eventsIterator != mEventData.end() || actionsIterator != mActionData.end()))
	{
		if (eventsIterator == mEventData.end())
		{
			// actions is not empty -> remove action
			actionsIterator = mActionData.erase(actionsIterator);
		}
		else if (actionsIterator == mActionData.end())
		{
			// events is not empty -> remove event
			eventsIterator = mEventData.erase(eventsIterator);
		}
		else
		{
			// both are not empty -> compare by timestamp and take the older one
			if ((*actionsIterator).getTimestamp() < (*eventsIterator).getTimestamp())
			{
				// first action is older than first event
				actionsIterator = mActionData.erase(actionsIterator);
			}
			else
			{
				// first event is older than first action
				eventsIterator = mEventData.erase(eventsIterator);
			}
		}

		numRecordsRemoved++;
	}

	return numRecordsRemoved;
}

const std::list<BeaconCacheRecord> BeaconCacheEntry::getEventData() const
{
	std::list<BeaconCacheRecord> result = mEventData;
	return result;
}

const std::list<BeaconCacheRecord> BeaconCacheEntry::getActionData() const
{
	std::list<BeaconCacheRecord> result = mActionData;
	return result;
}

const std::list<BeaconCacheRecord> BeaconCacheEntry::getEventDataBeingSent() const
{
	return mEventDataBeingSent;
}

const std::list<BeaconCacheRecord> BeaconCacheEntry::getActionDataBeingSent() const
{
	return mActionDataBeingSent;
}
