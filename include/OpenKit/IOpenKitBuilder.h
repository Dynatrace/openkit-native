/**
 * Copyright 2018-2021 Dynatrace LLC
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

#ifndef _OPENKIT_IOPENKITBUILDER_H
#define _OPENKIT_IOPENKITBUILDER_H

#include "CrashReportingLevel.h"
#include "DataCollectionLevel.h"
#include "ILogger.h"
#include "LogLevel.h"
#include "ISSLTrustManager.h"

#include <cstdint>
#include <memory>
#include <string>

namespace openkit
{
	class OPENKIT_EXPORT IOpenKitBuilder
	{
	public:

		virtual ~IOpenKitBuilder() = default;

		///
		/// Returns the string identifying the OpenKit type that gets created by the builder.
		///
		/// @par
		/// The only real purpose is for logging reasons.
		///
		/// @return some string identifying the OpenKit's type
		///
		virtual const std::string& getOpenKitType() const = 0;

		///
		/// Returns the application identifier for which the OpenKit reports data.
		///
		virtual const std::string& getApplicationID() const = 0;

		///
		/// Returns the application's name.
		///
		/// @par
		/// It depends on the concrete builder whether the application name is configurable or not.
		/// In any case, the derived classes have to return a string that is neither @c nullptr nor empty.
		///
		virtual const std::string& getApplicationName() const = 0;

		///
		/// Returns the ID of the server to communicate with.
		///
		/// @par
		/// This might be changed based on the OpenKit type
		///
		virtual int32_t getDefaultServerID() const = 0;

		///
		/// Returns the application version that was set to this builder.
		///
		/// @par
		/// If no version was set, the @ref openkit::OpenKitConstants::DEFAULT_APPLICATION_VERSION is returned.
		///
		virtual const std::string& getApplicationVersion() const = 0;

		///
		/// Returns the operating system that was set to this builder
		///
		/// @par
		/// If no operating system was set, the @ref openkit::OpenKitConstants::DEFAULT_OPERATING_SYSTEM is returned.
		///
		virtual const std::string& getOperatingSystem() const = 0;

		///
		/// Returns the manufacturer that was set to this builder.
		///
		/// @par
		/// If no manufacturer was set, the @ref openkit::OpenKitConstants::DEFAULT_MANUFACTURER is returned.
		///
		virtual const std::string& getManufacturer() const = 0;

		///
		/// Returns the model identifier that was set to this builder.
		///
		/// @par
		/// If no model ID was set, the @ref openkit::OpenKitConstants::DEFAULT_MODEL_ID is returned.
		///
		virtual const std::string& getModelID() const = 0;

		///
		/// Returns the endpoint URL that was set to this builder.
		///
		/// @par
		/// The endpoint URL is where the beacon data is sent to.
		///
		virtual const std::string& getEndpointURL() const = 0;

		///
		/// Returns the device identifier that was set to this builder.
		///
		/// @par
		/// The device identifier is a unique numeric value that identifies the device or the installation.
		/// The user of the OpenKit library is responsible for providing a unique value which stays consistent per
		/// device/installation.
		///
		virtual int64_t getDeviceID() const = 0;

		///
		/// Returns the device ID as it was passed (not hashed) to the builder.
		///
		virtual const std::string& getOrigDeviceID() const = 0;

		///
		/// Returns the SSL trust manager that was set to this builder.
		///
		/// @par
		/// The SSL trust manager implementation is responsible for checking the X509 certificate chain and also to
		/// reject untrusted/invalid certificates.
		/// The default implementation rejects every untrusted/invalid (including self-signed) certificate.
		///
		virtual std::shared_ptr<openkit::ISSLTrustManager> getTrustManager() const = 0;

		///
		/// Returns the maximum beacon cache age that was set to this builder.
		///
		/// @par
		/// If no max age was set, the @ref core::configuration::ConfigurationDefaults::DEFAULT_MAX_RECORD_AGE_IN_MILLIS
		/// is returned.
		///
		virtual int64_t getBeaconCacheMaxRecordAge() const = 0;

		///
		/// Returns the beacon cache's lower memory boundary that was set to this builder.
		///
		/// @par
		/// If no lower memory boundary was set, the @ref core::configuration::ConfigurationDefaults::DEFAULT_LOWER_MEMORY_BOUNDARY_IN_BYTES
		/// is returned.
		///
		virtual int64_t getBeaconCacheLowerMemoryBoundary() const = 0;

		///
		/// Returns the beacon cache's upper memory boundary that was set to this builder.
		///
		/// @par
		/// If no upper memory boundary was set, the @ref core::configuration::ConfigurationDefaults::DEFAULT_UPPER_MEMORY_BOUNDARY_IN_BYTES
		/// is returned.
		///
		virtual int64_t getBeaconCacheUpperMemoryBoundary() const = 0;

		///
		/// Returns the data collection level that was set on this builder.
		///
		/// @par
		/// If no data collection level was set, the @ref core::configuration::ConfigurationDefaults::DEFAULT_DATA_COLLECTION_LEVEL
		/// is returned.
		///
		virtual openkit::DataCollectionLevel getDataCollectionLevel() const = 0;

		///
		/// Returns the crash reporting level that was set to this builder.
		///
		/// @par
		/// If no crash reporting level was set, the @ref core::configuration::ConfigurationDefaults::DEFAULT_CRASH_REPORTING_LEVEL
		/// is returned.
		///
		virtual openkit::CrashReportingLevel getCrashReportingLevel() const = 0;

		///
		/// Returns the log level that was set on this builder
		///
		/// @par
		/// If no log lwevel was set, the default @ref openkit::LogLevel::LOG_LEVEL_WARN is returned.
		///
		virtual openkit::LogLevel getLogLevel() const = 0;

		///
		/// Returns the logger that was set on this builder.
		///
		/// @par
		/// If no logger was set, a default logger instance is returned.
		///
		virtual std::shared_ptr<openkit::ILogger> getLogger() const = 0;
	};
}

#endif
