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

#ifndef _CONFIGURATION_BEACONCACHECONFIGURATION_H
#define _CONFIGURATION_BEACONCACHECONFIGURATION_H

#include <cstdint>
#include <chrono>

namespace configuration
{
	///
	/// Configuration for beacon cache.
	///
	class BeaconCacheConfiguration
	{
	public:
		///
		/// Constructor
		/// @param[in] maxRecordAge Maximum record age
		/// @param[in] cacheSizeLowerBound lower memory limit for cache
		/// @param[in] cacheSizeUpperBound upper memory limit for cache
		///
		BeaconCacheConfiguration(int64_t maxRecordAge, int64_t cacheSizeLowerBound, int64_t cacheSizeUpperBound);

		///
		/// Get maximum record age.
		///
		int64_t getMaxRecordAge() const;

		///
		/// Get lower memory limit for the cache.
		///
		int64_t getCacheSizeLowerBound() const;
		

		///
		/// Get upper memory limit for the cache.
		///
		int64_t getCacheSizeUpperBound() const;

	private:
		/// maximum record age
		int64_t mMaxRecordAge;

		/// lower memory limit for the cache
		int64_t mCacheSizeLowerBound;

		/// upper memory limit for the cache
		int64_t mCacheSizeUpperBound;

	public:
	
		//default value for maximum record age
		static const std::chrono::milliseconds DEFAULT_MAX_RECORD_AGE_IN_MILLIS;

		//default value for upper memory boundary
		static const int64_t DEFAULT_UPPER_MEMORY_BOUNDARY_IN_BYTES;

		//default value for lower memory boundary
		static const int64_t DEFAULT_LOWER_MEMORY_BOUNDARY_IN_BYTES;
	};
}

#endif