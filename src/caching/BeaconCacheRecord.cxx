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

#include "BeaconCacheRecord.h"

using namespace caching;

BeaconCacheRecord::BeaconCacheRecord(int64_t timestamp, const core::UTF8String& data)
	: mTimestamp(timestamp)
	, mData(data)
	, mMarkedForSending(false)
{

}

int64_t BeaconCacheRecord::getTimestamp() const
{
	return mTimestamp;
}

const core::UTF8String& BeaconCacheRecord::getData() const
{
	return mData;
}

int64_t BeaconCacheRecord::getDataSizeInBytes() const
{
	if (mData.empty())
	{
		return 0;
	}
	return mData.getStringData().size();
}

bool BeaconCacheRecord::isMarkedForSending() const
{
	return mMarkedForSending;
}

void BeaconCacheRecord::markForSending()
{
	mMarkedForSending = true;
}

void BeaconCacheRecord::unsetSending()
{
	mMarkedForSending = false;
}