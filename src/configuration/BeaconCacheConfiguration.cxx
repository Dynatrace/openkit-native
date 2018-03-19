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

#include "configuration/BeaconCacheConfiguration.h"

#include <chrono>

using namespace configuration;

///
/// The default @ref BeaconCacheConfiguration when user does not override it.
/// Default settings allow beacons which are max 2 hours old and unbounded memory limits.
///
constexpr std::chrono::milliseconds DEFAULT_MAX_RECORD_AGE_IN_MILLIS = std::chrono::minutes(105);	// 1hour and 45 minutes
constexpr int64_t DEFAULT_UPPER_MEMORY_BOUNDARY_IN_BYTES = 100 * 1024 * 1024;			// 100 MiB
constexpr int64_t DEFAULT_LOWER_MEMORY_BOUNDARY_IN_BYTES = 80 * 1024 * 1024;			// 80 MiB

BeaconCacheConfiguration::BeaconCacheConfiguration(int64_t maxRecordAge, int64_t cacheSizeLowerBound, int64_t cacheSizeUpperBound)
	: mMaxRecordAge(maxRecordAge)
	, mCacheSizeLowerBound(cacheSizeLowerBound)
	, mCacheSizeUpperBound(cacheSizeUpperBound)
{

}

int64_t BeaconCacheConfiguration::getMaxRecordAge() const
{
	return mMaxRecordAge;
}

int64_t BeaconCacheConfiguration::getCacheSizeLowerBound() const
{
	return mCacheSizeLowerBound;
}

int64_t BeaconCacheConfiguration::getCacheSizeUpperBound() const
{
	return mCacheSizeUpperBound;
}