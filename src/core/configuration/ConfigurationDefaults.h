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

#ifndef _CORE_CONFIGURATION_CONFIGURATIONDEFAULTS_H
#define _CORE_CONFIGURATION_CONFIGURATIONDEFAULTS_H

#include "OpenKit/CrashReportingLevel.h"
#include "OpenKit/DataCollectionLevel.h"

#include <chrono>

namespace core
{
	namespace configuration
	{
		///
		/// The default @ref BeaconCacheConfiguration when user does not override it.
		/// Default settings allow beacons which are max 2 hours old and unbounded memory limits.
		///
		static const std::chrono::milliseconds DEFAULT_MAX_RECORD_AGE_IN_MILLIS = std::chrono::minutes(105); // 1 hour and 45 minutes.

		///
		/// Defines the default upper memory bound of the beacon cache
		///
		/// @par
		/// The upper boundary is the size limit at which the beacon cache will start evicting records.
		/// The default upper boundary is 100 MB
		///
		static constexpr int64_t DEFAULT_UPPER_MEMORY_BOUNDARY_IN_BYTES = 100 * 1024 * 1024;		// 100MiB

		///
		/// Defines the default lower memory boundary of the beacon cache
		///
		/// @par
		/// The lower boundary is the size until which the beacon cache will evict records once the upper boundary was
		/// exceeded.
		/// The default lower boundary is 80 MB
		static const int64_t DEFAULT_LOWER_MEMORY_BOUNDARY_IN_BYTES = 80 * 1024 * 1024;				// 80MiB

		///
		/// Default data collection level used, if no other value was specified.
		///
		static constexpr openkit::DataCollectionLevel DEFAULT_DATA_COLLECTION_LEVEL = openkit::DataCollectionLevel::USER_BEHAVIOR;

		///
		/// Default crash reporting level used, if no other value was specified.
		///
		static constexpr openkit::CrashReportingLevel DEFAULT_CRASH_REPORTING_LEVEL = openkit::CrashReportingLevel::OPT_IN_CRASHES;

		///
		/// Specifies the default multiplicity.
		///
		static constexpr int32_t DEFAULT_MULTIPLICITY = 1;
	}
}

#endif
