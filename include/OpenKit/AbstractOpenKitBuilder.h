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

#ifndef _OPENKIT_ABSTRACTOPENKITBUILDER_H
#define _OPENKIT_ABSTRACTOPENKITBUILDER_H

#include "OpenKit_export.h"
#include "OpenKit/IOpenKit.h"
#include "OpenKit/ILogger.h"
#include "OpenKit/ISSLTrustManager.h"
#include "OpenKit/DataCollectionLevel.h"
#include "OpenKit/CrashReportingLevel.h"

#include <cstdint>
#include <memory>

#ifndef DOXYGEN_HIDE_FROM_DOC
namespace configuration
{
	class OPENKIT_EXPORT Configuration;
}
#endif

namespace openkit
{
	///
	/// abstract base class for OpenKitBuilders
	///
	class OPENKIT_EXPORT AbstractOpenKitBuilder
	{
		public:

			///
			/// Destructor
			///
			virtual ~AbstractOpenKitBuilder() {}

			///
			/// Enables verbose mode. Verbose mode is only enabled if the the default logger is used.
			/// If a custom logger is provided (by calling @ref withLogger(std::shared_ptr<openkit::ILogger>)) debug and info log output
			/// depends on the values returned by @ref openkit::ILogger::isDebugEnabled() and @ref openkit::ILogger::isInfoEnabled().
 			/// @return @c this for fluent usage
			///
			AbstractOpenKitBuilder& enableVerbose();

			///
			/// Sets the logger. If no logger is set the default console logger is used. For the default
			/// logger verbose mode is enabled by calling @c enableVerbose.
			/// @param[in] logger the logger
			/// @return @c this for fluent usage
			///
			AbstractOpenKitBuilder& withLogger(std::shared_ptr<openkit::ILogger> logger);

			///
			/// Defines the version of the application. The value is only set if it is neither null nor empty.
			///
			/// @param[in] applicationVersion the application version
			/// @returns @c this for fluent usage
			///
			AbstractOpenKitBuilder& withApplicationVersion(const char* applicationVersion);

			///
			/// Sets the trust manager, if it's not @c nullptr.
			/// @remarks Overrides the default trust manager which is only accepting valid SSL certificates.
			///
			/// @param[in] trustManager trust manager implementation
			/// @returns @c this for fluent usage
			///
			AbstractOpenKitBuilder& withTrustManager(std::shared_ptr<openkit::ISSLTrustManager> trustManager);

			///
			/// Sets the operating system information. The value is only set if it is neither null nor empty.
			///
			/// @param[in] operatingSystem the operating system
			/// @returns @c this for fluent usage
			///
			AbstractOpenKitBuilder& withOperatingSystem(const char* operatingSystem);

			///
			/// Sets the manufacturer information. The value is only set if it is neither null nor empty.
			///
			/// @param[in] manufacturer the manufacturer
			/// @returns @c this for fluent usage
			///
			AbstractOpenKitBuilder& withManufacturer(const char* manufacturer);

			///
			/// Sets the model id. The value is only set if it is neither null nor empty.
			///
			/// @param[in] modelID the model id
			/// @returns @c this for fluent usage
			///
			AbstractOpenKitBuilder& withModelID(const char* modelID);

			///
			/// Sets the maximum beacon record age of beacon data in cache.
			///
			/// @param[in] maxRecordAgeInMilliseconds The maximum beacon record age in milliseconds, or unbounded if negative.
			/// @returns @c this for fluent usage
			///
			AbstractOpenKitBuilder& withBeaconCacheMaxRecordAge(int64_t maxRecordAgeInMilliseconds);

			///
			/// Sets the lower memory boundary of the beacon cache.
			///
			/// When this is set to a positive value the memory based eviction strategy clears the collected data,
			/// until the data size in the cache falls below the configured limit.
			/// @param[in] lowerMemoryBoundaryInBytes The lower boundary of the beacon cache or negative if unlimited.
			/// @returns @c this
			///
			AbstractOpenKitBuilder& withBeaconCacheLowerMemoryBoundary(int64_t lowerMemoryBoundaryInBytes);

			///
			/// Sets the upper memory boundary of the beacon cache.
			///
			/// When this is set to a positive value the memory based eviction strategy starts to clear
			/// data from the beacon cache when the cache size exceeds this setting.
			/// @param[in] upperMemoryBoundaryInBytes The upper boundary of the beacon cache or negative if unlimited.
			/// @returns @c this
			///
			AbstractOpenKitBuilder& withBeaconCacheUpperMemoryBoundary(int64_t upperMemoryBoundaryInBytes);

			///
			/// Sets the data collection level used
			///
			/// Default behavior is the level @ref openkit::DataCollectionLevel::OFF
			/// @param[in] dataCollectionLevel data collection level to use
			/// @returns @c this
			///
			AbstractOpenKitBuilder& withDataCollectionLevel(openkit::DataCollectionLevel dataCollectionLevel);

			///
			/// Sets the crash reporting level used
			///
			/// Default behavior is the level @ref openkit::CrashReportingLevel::OFF
			/// @param[in] crashReportingLevel crash reporting level to use
			/// @returns @c this
			///
			AbstractOpenKitBuilder& withCrashReportingLevel(openkit::CrashReportingLevel crashReportingLevel);

			///
			/// Builds an @ref openkit::IOpenKit instance
			/// @return an @ref openkit::IOpenKit instance
			///
			virtual std::shared_ptr<openkit::IOpenKit> build();

			///
			/// Builds the configuration for the OpenKit instance
			/// @returns the configuration build using the parameters in this class
			///
			virtual std::shared_ptr<configuration::Configuration> buildConfiguration() = 0;

		protected:

			///
			/// Constructor
			/// @param[in] endpointURL endpoint OpenKit connects to
			/// @param[in] deviceID unique device id
			///
			AbstractOpenKitBuilder(const char* endpointURL, int64_t deviceID);

			///
			/// Constructor
			/// @param[in] endpointURL endpoint OpenKit connects to
			/// @param[in] deviceID unique device id
			///
			AbstractOpenKitBuilder(const char* endpointURL, const char* deviceID);

			///
			/// Returns the application version
			/// @returns the application version
			///
			const std::string& getApplicationVersion() const;

			///
			/// Returns the operating system 
			/// @returns the operating system
			///
			const std::string& getOperatingSystem() const;

			///
			/// Returns the manufacturer
			/// @returns the manufacturer
			///
			const std::string& getManufacturer() const;

			///
			/// Returns the model ID
			/// @returns the model ID
			///
			const std::string& getModelID() const;

			///
			/// Returns the endpoint URL
			/// @returns the endpoint URL
			///
			const std::string& getEndpointURL()const;

			///
			/// Returns the device ID
			/// @returns the device ID
			///
			const std::string& getDeviceID() const;

			///
			/// Returns the SSL trust manager
			/// @returns the SSL trust manager
			///
			std::shared_ptr<openkit::ISSLTrustManager> getTrustManager() const;

			///
			/// Returns the maximum record age
			/// @returns the maximum record age, negative values declare that there are no bounds
			///
			int64_t getBeaconCacheMaxRecordAge() const;

			///
			/// Returns the lower memory boundary
			/// @returns the lower memory boundary, negative values declare that there are no bounds
			///
			int64_t getBeaconCacheLowerMemoryBoundary() const;

			///
			/// Returns the upper memory boundary
			/// @returns the upper memory boundary, negative values declare that there are no bounds
			///
			int64_t getBeaconCacheUpperMemoryBoundary() const;

			///
			/// Returns the data collection level
			/// @returns the data collection level
			///
			DataCollectionLevel getDataCollectionLevel() const;

			///
			/// Returns the crash reporting level
			/// @returns the crash reporting level
			///
			CrashReportingLevel getCrashReportingLevel() const;

		public:
			///
			/// Returns a @ref openkit::ILogger. If no logger is set, when building the OpenKit with @ref build(),
			/// the default logger is returned.
			/// @return a logger
			///
			std::shared_ptr<openkit::ILogger> getLogger();

		private:
			/// Flag to enable INFO and DEBUG logs
			bool mVerbose;

			/// The logger used to log traces
			std::shared_ptr<ILogger> mLogger;

			/// application version
			std::string mApplicationVersion;

			/// operating system
			std::string mOperatingSystem;

			/// manufacturer
			std::string mManufacturer;

			/// model ID
			std::string mModelID;

			/// endpoint URL
			std::string mEndpointURL;

			/// device ID
			std::string mDeviceID;

			/// SSL trust manager
			std::shared_ptr<openkit::ISSLTrustManager> mTrustManager;

			/// maximum record age inside beacon cache
			int64_t mBeaconCacheMaxRecordAge;

			/// lower memory boundary of beacon cache
			int64_t mBeaconCacheLowerMemoryBoundary;

			/// upper memory boundary of beacon cache
			int64_t mBeaconCacheUpperMemoryBoundary;

			/// data collection level
			openkit::DataCollectionLevel mDataCollectionLevel;

			/// crash reporting level
			openkit::CrashReportingLevel mCrashReportingLevel;
	};
}

#endif
