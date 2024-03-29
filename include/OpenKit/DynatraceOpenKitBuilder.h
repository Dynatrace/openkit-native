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

#ifndef _OPENKIT_DYNATRACEOPENKITBUILDER_H
#define _OPENKIT_DYNATRACEOPENKITBUILDER_H

#include "OpenKit/OpenKitExports.h"
#include "IOpenKit.h"
#include "IOpenKitBuilder.h"
#include "ILogger.h"
#include "ISSLTrustManager.h"
#include "DataCollectionLevel.h"
#include "CrashReportingLevel.h"
#include "IHttpRequestInterceptor.h"
#include "IHttpResponseInterceptor.h"

#include <cstdint>
#include <memory>
#include <string>

#ifndef DOXYGEN_HIDE_FROM_DOC
namespace core {
	namespace configuration
	{
		class OPENKIT_EXPORT Configuration;
	}
}
#endif

namespace openkit
{
	///
	/// Builder for OpenKit instances connected to Dynatrace
	///
	class OPENKIT_EXPORT DynatraceOpenKitBuilder : public openkit::IOpenKitBuilder
	{
	public:

		static constexpr int32_t DEFAULT_SERVER_ID = 1;

		static constexpr const char* OPENKIT_TYPE = "DynatraceOpenKit";

		///
		/// Constructor
		/// @param[in] endpointURL endpoint OpenKit connects to
		/// @param[in] applicationID unique application id
		/// @param[in] deviceID unique device id
		///
		DynatraceOpenKitBuilder(const char* endpointURL, const char* applicationID, int64_t deviceID);

		///
		/// Destructor
		///
		~DynatraceOpenKitBuilder() = default;

		///
			/// Sets the log level if the default logger is used.
			/// If a custom logger is provided by calling @ref withLogger, debug and info log output
			/// depends on the values returned by @ref ILogger::isDebugEnabled and @ref ILogger::isInfoEnabled.
			/// @param logLevel The logLevel for the default logger
			///
		DynatraceOpenKitBuilder& withLogLevel(openkit::LogLevel logLevel);

		///
		/// Sets the logger. If no logger is set the default console logger is used. For the default
		/// logger verbose mode is enabled by calling @c withLogLevel.
		/// @param[in] logger the logger
		/// @return @c this for fluent usage
		///
		DynatraceOpenKitBuilder& withLogger(std::shared_ptr<openkit::ILogger> logger);

		///
		/// Defines the version of the application. The value is only set if it is neither null nor empty.
		///
		/// @param[in] applicationVersion the application version
		/// @returns @c this for fluent usage
		///
		DynatraceOpenKitBuilder& withApplicationVersion(const char* applicationVersion);

		///
		/// Sets the trust manager, if it's not @c nullptr.
		/// @remarks Overrides the default trust manager which is only accepting valid SSL certificates.
		///
		/// @param[in] trustManager trust manager implementation
		/// @returns @c this for fluent usage
		///
		DynatraceOpenKitBuilder& withTrustManager(std::shared_ptr<openkit::ISSLTrustManager> trustManager);

		///
		/// Sets the operating system information. The value is only set if it is neither null nor empty.
		///
		/// @param[in] operatingSystem the operating system
		/// @returns @c this for fluent usage
		///
		DynatraceOpenKitBuilder& withOperatingSystem(const char* operatingSystem);

		///
		/// Sets the manufacturer information. The value is only set if it is neither null nor empty.
		///
		/// @param[in] manufacturer the manufacturer
		/// @returns @c this for fluent usage
		///
		DynatraceOpenKitBuilder& withManufacturer(const char* manufacturer);

		///
		/// Sets the model id. The value is only set if it is neither null nor empty.
		///
		/// @param[in] modelID the model id
		/// @returns @c this for fluent usage
		///
		DynatraceOpenKitBuilder& withModelID(const char* modelID);

		///
		/// Sets the maximum beacon record age of beacon data in cache.
		///
		/// @param[in] maxRecordAgeInMilliseconds The maximum beacon record age in milliseconds, or unbounded if negative.
		/// @returns @c this for fluent usage
		///
		DynatraceOpenKitBuilder& withBeaconCacheMaxRecordAge(int64_t maxRecordAgeInMilliseconds);

		///
		/// Sets the lower memory boundary of the beacon cache.
		///
		/// When this is set to a positive value the memory based eviction strategy clears the collected data,
		/// until the data size in the cache falls below the configured limit.
		/// @param[in] lowerMemoryBoundaryInBytes The lower boundary of the beacon cache or negative if unlimited.
		/// @returns @c this
		///
		DynatraceOpenKitBuilder& withBeaconCacheLowerMemoryBoundary(int64_t lowerMemoryBoundaryInBytes);

		///
		/// Sets the upper memory boundary of the beacon cache.
		///
		/// When this is set to a positive value the memory based eviction strategy starts to clear
		/// data from the beacon cache when the cache size exceeds this setting.
		/// @param[in] upperMemoryBoundaryInBytes The upper boundary of the beacon cache or negative if unlimited.
		/// @returns @c this
		///
		DynatraceOpenKitBuilder& withBeaconCacheUpperMemoryBoundary(int64_t upperMemoryBoundaryInBytes);

		///
		/// Sets the data collection level used
		///
		/// Depending on the chosen level the amount and granularity of data sent is controlled.
		/// <ul>
		///   <li> @ref openkit::DataCollectionLevel::OFF - no data collected
		///   <li> @ref openkit::DataCollectionLevel::PERFORMANCE - only performance related data is collected
		///   <li> @ref openkit::DataCollectionLevel::USER_BEHAVIOR - all available RUM data including performance related data is collected
		/// </ul>
		///
		/// Default behavior is the level @ref openkit::DataCollectionLevel::USER_BEHAVIOR
		///
		/// @param[in] dataCollectionLevel data collection level to apply.
		/// @returns @c this
		///
		DynatraceOpenKitBuilder& withDataCollectionLevel(openkit::DataCollectionLevel dataCollectionLevel);

		///
		/// Sets the crash reporting level used
		///
		/// <ul>
		///   <li> @ref openkit::CrashReportingLevel::OFF - Crashes are not sent to the server
		///   <li> @ref openkit::CrashReportingLevel::OPT_OUT_CRASHES - Crashes are not sent to the server
		///   <li> @ref openkit::CrashReportingLevel::OPT_IN_CRASHES - Crashes are sent to the server
		/// </ul>
		///
		/// Default behavior is the level @ref openkit::CrashReportingLevel::OFF
		/// @param[in] crashReportingLevel crash reporting level to use
		/// @returns @c this
		///
		DynatraceOpenKitBuilder& withCrashReportingLevel(openkit::CrashReportingLevel crashReportingLevel);

		///
		/// Sets a custom openkit::IHttpResponseInterceptor
		///
		/// @param[in] httpRequestInterceptor Interceptor for intercepting requests to Dynatrace backends.
		/// @return @c this
		///
		DynatraceOpenKitBuilder& withHttpRequestInterceptor(std::shared_ptr<openkit::IHttpRequestInterceptor> httpRequestIntercetpor);

		///
		/// Sets a custom openkit::IHttpResponseInterceptor
		///
		/// @param[in] httpResponseInterceptor Interceptor for intercepting responses received from Dynatrace backends.
		/// @return @c this
		///
		DynatraceOpenKitBuilder& withHttpResponseInterceptor(std::shared_ptr<openkit::IHttpResponseInterceptor> httpResponseInterceptor);

		///
		/// Builds an @ref openkit::IOpenKit instance
		/// @return an @ref openkit::IOpenKit instance
		///
		std::shared_ptr<openkit::IOpenKit> build();

		const std::string& getOpenKitType() const override;

		const std::string& getApplicationID() const override;

		int32_t getDefaultServerID() const override;

		const std::string& getApplicationVersion() const override;

		const std::string& getOperatingSystem() const override;

		const std::string& getManufacturer() const override;

		const std::string& getModelID() const override;

		const std::string& getEndpointURL()const override;

		int64_t getDeviceID() const override;

		const std::string& getOrigDeviceID() const override;

		std::shared_ptr<openkit::ISSLTrustManager> getTrustManager() const override;

		int64_t getBeaconCacheMaxRecordAge() const override;

		int64_t getBeaconCacheLowerMemoryBoundary() const override;

		int64_t getBeaconCacheUpperMemoryBoundary() const override;

		DataCollectionLevel getDataCollectionLevel() const override;

		CrashReportingLevel getCrashReportingLevel() const override;

		std::shared_ptr<openkit::IHttpRequestInterceptor> getHttpRequestInterceptor() const override;

		std::shared_ptr<openkit::IHttpResponseInterceptor> getHttpResponseInterceptor() const override;

		openkit::LogLevel getLogLevel() const override;

		std::shared_ptr<openkit::ILogger> getLogger() const override;

	private:

		///
		/// Constructor
		/// @param[in] endpointURL endpoint OpenKit connects to
		/// @param[in] deviceID unique device id
		/// @param[in] origDeviceID device id before it was hashed
		///
		DynatraceOpenKitBuilder(const char* endpointURL, int64_t deviceID, const char* origDeviceID);

		/// application id
		std::string mApplicationID;

		/// openkit type
		std::string mOpenKitType;

		/// Log level used for the default logger
		LogLevel mLogLevel;

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
		int64_t mDeviceID;

		/// original device ID (before hashing)
		std::string mOrigDeviceID;

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

		/// Used for intercepting HTTP requests to the Dynatrace backend
		std::shared_ptr<openkit::IHttpRequestInterceptor> mHttpRequestInterceptor;

		/// Used for intercepting HTTP responses from Dynatrace backend
		std::shared_ptr<openkit::IHttpResponseInterceptor> mHttpResponseInterceptor;
	};
}

#endif
