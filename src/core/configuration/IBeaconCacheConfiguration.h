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

#ifndef _CORE_CONFIGURATION_IBEACONCACHECONFIGURATION_H
#define _CORE_CONFIGURATION_IBEACONCACHECONFIGURATION_H

#include <cstdint>

namespace core
{
	namespace configuration
	{
		///
		/// Provides relevant configuration for the beacon cache
		///
		class IBeaconCacheConfiguration
		{
		public:

			///
			/// Destructor
			///
			virtual ~IBeaconCacheConfiguration() {}

			///
			/// Returns the maximum record age (in milliseconds) which a record is allowed to have before it gets evicted.
			///
			virtual int64_t getMaxRecordAge() const = 0;

			///
			/// Returns the lower cache size (in bytes) until which records will be evicted, once the
			/// @ref getCacheSizeUpperBound() upper limit was exceeded.
			///
			virtual int64_t getCacheSizeLowerBound() const = 0;

			///
			/// Returns the upper cache size which, upon exceeding, will start the space eviction strategy.
			///
			virtual int64_t getCacheSizeUpperBound() const = 0;
		};
	}
}

#endif
