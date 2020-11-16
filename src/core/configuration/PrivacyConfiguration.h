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

#ifndef _CORE_CONFIGURATION_PRIVACYCONFIGURATION_H
#define _CORE_CONFIGURATION_PRIVACYCONFIGURATION_H

#include "OpenKit/CrashReportingLevel.h"
#include "OpenKit/DataCollectionLevel.h"
#include "OpenKit/IOpenKitBuilder.h"
#include "core/configuration/IPrivacyConfiguration.h"

namespace core
{
	namespace configuration
	{
		///
		/// Configuration class storing user configured privacy settings.
		///
		class PrivacyConfiguration
			: public core::configuration::IPrivacyConfiguration
		{
		public:

			///
			/// Constructs the privacy configuration
			///
			/// @param builder instance providing privacy relevant configuration details.
			///
			PrivacyConfiguration(
				const openkit::IOpenKitBuilder& builder
			);

			~PrivacyConfiguration() override = default;

			///
			/// Creates a privacy configuration instance from the given OpenKit builder.
			///
			/// @param builder the OpenKit builder from which to create the privacy configuration instance.
			/// @return a newly created privacy configuration instance or @c nullptr if the given argument is @c nullptr
			///
			static std::shared_ptr<IPrivacyConfiguration> from(const openkit::IOpenKitBuilder& builder);

			///
			/// Returns the data collection level which was set in the constructor.
			///
			openkit::DataCollectionLevel getDataCollectionLevel() const override;

			///
			/// Returns the crash reporting level which was set in the constructor.
			///
			openkit::CrashReportingLevel getCrashReportingLevel() const override;

			///
			/// Returns a boolean indicating whether sending the device identifier is allowed or not.
			///
			bool isDeviceIdSendingAllowed() const override;

			///
			/// Returns a boolean indicating whether sending the session number is allowed or not.
			///
			bool isSessionNumberReportingAllowed() const override;

			///
			/// Returns a boolean indicating whether tracing web requests is allowed or not.
			///
			bool isWebRequestTracingAllowed() const override;

			///
			/// Returns a boolean indicating whether reporting ended sessions is allowed or not.
			///
			bool isSessionReportingAllowed() const override;

			///
			/// Returns a boolean indicating whether reporting actions is allowed or not.
			///
			bool isActionReportingAllowed() const override;

			///
			/// Returns a boolean indicating whether reporting values is allowed or not.
			///
			bool isValueReportingAllowed() const override;

			///
			/// Returns a boolean indicating whether reporting events is allowed or not.
			///
			bool isEventReportingAllowed() const override;

			///
			/// Returns a boolean indicating whether reporting errors is allowed or not.
			///
			bool isErrorReportingAllowed() const override;

			///
			/// Returns a boolean indicating whether reporting crashes is allowed or not.
			///
			bool isCrashReportingAllowed() const override;

			///
			/// Returns a boolean indicating whether user identification is allowed or not.
			///
			bool isUserIdentificationAllowed() const override;

		private:

			openkit::DataCollectionLevel mDataCollectionLevel;

			openkit::CrashReportingLevel mCrashReportingLevel;
		};
	}
}

#endif
