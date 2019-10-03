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

#ifndef _CORE_CONFIGURATION_BEACONCONFIGURATION_H
#define _CORE_CONFIGURATION_BEACONCONFIGURATION_H

#include "OpenKit/CrashReportingLevel.h"
#include "OpenKit/DataCollectionLevel.h"

namespace core
{
	namespace configuration
	{
		///
		/// Configuration for beacon cache.
		///
		class BeaconConfiguration
		{
		public:
			///
			/// Constructor
			/// @param[in] dataLevel data collection level
			/// @param[in] crashLevel crash reporting level
			///
			BeaconConfiguration(int32_t multiplicty, openkit::DataCollectionLevel dataLevel, openkit::CrashReportingLevel crashLevel);

			///
			/// Default constructor returning the default config
			///
			BeaconConfiguration();

			///
			/// Get the data collection level
			/// @return the data collection level
			///
			openkit::DataCollectionLevel getDataCollectionLevel() const;

			///
			/// Get the crash reporting level
			/// @return the crash reporting level
			///
			openkit::CrashReportingLevel getCrashReportingLevel() const;

			///
			/// Get the multiplicity
			/// @return the multiplicity
			///
			int32_t getMultiplicity() const;

		private:
			/// multiplicity controlling how many sessions are actually send
			int32_t mMulitplicity;

			/// data collection level
			openkit::DataCollectionLevel mDataCollectionLevel;

			/// crash reporting level
			openkit::CrashReportingLevel mCrashReportingLevel;



		public:
			/// default multiplicity
			static const int32_t DEFAULT_MULTIPLICITY;

			/// default value for data collection level
			static const openkit::DataCollectionLevel DEFAULT_DATA_COLLECTION_LEVEL;

			/// default value for crash reporting level
			static const openkit::CrashReportingLevel DEFAULT_CRASH_REPORTING_LEVEL;
		};
	}
}

#endif
