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

#ifndef _CORE_CONFIGURATION_IPRIVACYCONFIGURATION_H
#define _CORE_CONFIGURATION_IPRIVACYCONFIGURATION_H

#include "OpenKit/CrashReportingLevel.h"
#include "OpenKit/DataCollectionLevel.h"

namespace core
{
	namespace configuration
	{
		class IPrivacyConfiguration
		{
		public:

			///
			/// Destructor
			///
			virtual ~IPrivacyConfiguration() {}

			///
			/// Returns the data collection level which was set in the constructor.
			///
			virtual openkit::DataCollectionLevel getDataCollectionLevel() const = 0;

			///
			/// Returns the crash reporting level which was set in the constructor.
			///
			virtual openkit::CrashReportingLevel getCrashReportingLevel() const = 0;

			///
			/// Returns a boolean indicating whether sending the device identifier is allowed or not.
			///
			virtual bool isDeviceIdSendingAllowed() const = 0;

			///
			/// Returns a boolean indicating whether sending the session number is allowed or not.
			///
			virtual bool isSessionNumberReportingAllowed() const = 0;

			///
			/// Returns a boolean indicating whether tracing web requests is allowed or not.
			///
			virtual bool isWebRequestTracingAllowed() const = 0;

			///
			/// Returns a boolean indicating whether reporting ended sessions is allowed or not.
			///
			virtual bool isSessionReportingAllowed() const = 0;

			///
			/// Returns a boolean indicating whether reporting actions is allowed or not.
			///
			virtual bool isActionReportingAllowed() const = 0;

			///
			/// Returns a boolean indicating whether reporting values is allowed or not.
			///
			virtual bool isValueReportingAllowed() const = 0;

			///
			/// Returns a boolean indicating whether reporting events is allowed or not.
			///
			virtual bool isEventReportingAllowed() const = 0;

			///
			/// Returns a boolean indicating whether reporting errors is allowed or not.
			///
			virtual bool isErrorReportingAllowed() const = 0;

			///
			/// Returns a boolean indicating whether reporting crashes is allowed or not.
			///
			virtual bool isCrashReportingAllowed() const = 0;

			///
			/// Returns a boolean indicating whether user identification is allowed or not.
			///
			virtual bool isUserIdentificationAllowed() const = 0;
		};
	}
}

#endif
