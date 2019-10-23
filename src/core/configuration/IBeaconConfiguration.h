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

#ifndef _CORE_CONFIGURATION_IBEACONCONFIGURATION_H
#define _CORE_CONFIGURATION_IBEACONCONFIGURATION_H

#include "OpenKit/CrashReportingLevel.h"
#include "OpenKit/DataCollectionLevel.h"

#include <cstdint>

namespace core
{
	namespace configuration
	{
		///
		/// Provides configuration for the beacon.
		///
		class IBeaconConfiguration
		{
		public:

			virtual ~IBeaconConfiguration() {}

			///
			/// Returns the data collection level
			///
			virtual openkit::DataCollectionLevel getDataCollectionLevel() const = 0;

			///
			/// Returns the crash reporting level
			///
			virtual openkit::CrashReportingLevel getCrashReportingLevel() const = 0;

			///
			/// Returns the multiplicity
			///
			virtual int32_t getMultiplicity() const = 0;
		};
	}
}

#endif
