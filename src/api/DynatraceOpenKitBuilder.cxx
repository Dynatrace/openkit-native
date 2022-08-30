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

#include "OpenKit/DynatraceOpenKitBuilder.h"
#include "OpenKit/DynatraceOpenKitBuilder.h"
#include "OpenKit/OpenKitConstants.h"
#include "core/configuration/ConfigurationDefaults.h"
#include "core/util/DefaultLogger.h"
#include "core/util/StringUtil.h"
#include "core/objects/OpenKitInitializer.h"
#include "core/objects/OpenKit.h"
#include "protocol/ssl/SSLStrictTrustManager.h"
#include "protocol/http/NullHttpRequestInterceptor.h"
#include "protocol/http/NullHttpResponseInterceptor.h"

#include <cstring>

using namespace openkit;

DynatraceOpenKitBuilder::DynatraceOpenKitBuilder(const char* endpointURL, const char* applicationID, int64_t deviceID)
	: mApplicationID(applicationID)
	, mOpenKitType(OPENKIT_TYPE)
	, mLogLevel(LogLevel::LOG_LEVEL_WARN)
	, mLogger(nullptr)
	, mApplicationVersion(DEFAULT_APPLICATION_VERSION)
	, mOperatingSystem(DEFAULT_OPERATING_SYSTEM)
	, mManufacturer(DEFAULT_MANUFACTURER)
	, mModelID(DEFAULT_MODEL_ID)
	, mEndpointURL(endpointURL)
	, mDeviceID(deviceID)
	, mOrigDeviceID(core::util::StringUtil::toInvariantString(deviceID).c_str())
	, mTrustManager(std::make_shared<protocol::SSLStrictTrustManager>())
	, mBeaconCacheMaxRecordAge(core::configuration::DEFAULT_MAX_RECORD_AGE_IN_MILLIS)
	, mBeaconCacheLowerMemoryBoundary(core::configuration::DEFAULT_LOWER_MEMORY_BOUNDARY_IN_BYTES)
	, mBeaconCacheUpperMemoryBoundary(core::configuration::DEFAULT_UPPER_MEMORY_BOUNDARY_IN_BYTES)
	, mDataCollectionLevel(core::configuration::DEFAULT_DATA_COLLECTION_LEVEL)
	, mCrashReportingLevel(core::configuration::DEFAULT_CRASH_REPORTING_LEVEL)
	, mHttpRequestInterceptor(protocol::NullHttpRequestInterceptor::instance())
	, mHttpResponseInterceptor(protocol::NullHttpResponseInterceptor::instance())
{
}

DynatraceOpenKitBuilder& DynatraceOpenKitBuilder::withLogLevel(openkit::LogLevel logLevel)
{
	mLogLevel = logLevel;
	return *this;
}

DynatraceOpenKitBuilder& DynatraceOpenKitBuilder::withLogger(std::shared_ptr<ILogger> logger)
{
	mLogger = logger;
	return *this;
}

DynatraceOpenKitBuilder& DynatraceOpenKitBuilder::withApplicationVersion(const char* applicationVersion)
{
	if (applicationVersion != nullptr && strlen(applicationVersion) > 0)
	{
		mApplicationVersion = applicationVersion;
	}
	return *this;
}

DynatraceOpenKitBuilder& DynatraceOpenKitBuilder::withTrustManager(std::shared_ptr<openkit::ISSLTrustManager> trustManager)
{
	if (trustManager != nullptr)
	{
		mTrustManager = trustManager;
	}
	return *this;
}

DynatraceOpenKitBuilder& DynatraceOpenKitBuilder::withOperatingSystem(const char* operatingSystem)
{
	if (operatingSystem != nullptr && strlen(operatingSystem) > 0)
	{
		mOperatingSystem = operatingSystem;
	}
	return *this;
}

DynatraceOpenKitBuilder& DynatraceOpenKitBuilder::withManufacturer(const char* manufacturer)
{
	if (manufacturer != nullptr && strlen(manufacturer) > 0)
	{
		mManufacturer = manufacturer;
	}
	return *this;
}

DynatraceOpenKitBuilder& DynatraceOpenKitBuilder::withModelID(const char* modelID)
{
	if (modelID != nullptr && strlen(modelID) > 0)
	{
		mModelID = modelID;
	}
	return *this;
}

DynatraceOpenKitBuilder& DynatraceOpenKitBuilder::withBeaconCacheMaxRecordAge(int64_t maxRecordAgeInMilliseconds)
{
	mBeaconCacheMaxRecordAge = maxRecordAgeInMilliseconds;
	return *this;
}

DynatraceOpenKitBuilder& DynatraceOpenKitBuilder::withBeaconCacheLowerMemoryBoundary(int64_t lowerMemoryBoundaryInBytes)
{
	mBeaconCacheLowerMemoryBoundary = lowerMemoryBoundaryInBytes;
	return *this;
}

DynatraceOpenKitBuilder& DynatraceOpenKitBuilder::withBeaconCacheUpperMemoryBoundary(int64_t upperMemoryBoundaryInBytes)
{
	mBeaconCacheUpperMemoryBoundary = upperMemoryBoundaryInBytes;
	return *this;
}

DynatraceOpenKitBuilder& DynatraceOpenKitBuilder::withDataCollectionLevel(DataCollectionLevel dataCollectionLevel)
{
	mDataCollectionLevel = dataCollectionLevel;
	return *this;
}

DynatraceOpenKitBuilder& DynatraceOpenKitBuilder::withCrashReportingLevel(CrashReportingLevel crashReportingLevel)
{
	mCrashReportingLevel = crashReportingLevel;
	return *this;
}

DynatraceOpenKitBuilder& DynatraceOpenKitBuilder::withHttpRequestInterceptor(std::shared_ptr<openkit::IHttpRequestInterceptor> httpRequestIntercetpor)
{
	if (httpRequestIntercetpor != nullptr)
	{
		mHttpRequestInterceptor = httpRequestIntercetpor;
	}
	return *this;
}

DynatraceOpenKitBuilder& DynatraceOpenKitBuilder::withHttpResponseInterceptor(std::shared_ptr<openkit::IHttpResponseInterceptor> httpResponseInterceptor)
{
	if (httpResponseInterceptor != nullptr)
	{
		mHttpResponseInterceptor = httpResponseInterceptor;
	}
	return *this;
}

std::shared_ptr<openkit::IOpenKit> DynatraceOpenKitBuilder::build()
{
	core::objects::OpenKitInitializer initializer(*this);
	auto openKit = std::make_shared<core::objects::OpenKit>(initializer);
	openKit->initialize();

	return openKit;
}

const std::string& DynatraceOpenKitBuilder::getApplicationVersion() const
{
	return mApplicationVersion;
}

const std::string& DynatraceOpenKitBuilder::getOperatingSystem() const
{
	return mOperatingSystem;
}

const std::string& DynatraceOpenKitBuilder::getManufacturer() const
{
	return mManufacturer;
}

const std::string& DynatraceOpenKitBuilder::getModelID() const
{
	return mModelID;
}

const std::string& DynatraceOpenKitBuilder::getEndpointURL() const
{
	return mEndpointURL;
}

int64_t DynatraceOpenKitBuilder::getDeviceID() const
{
	return mDeviceID;
}

const std::string& DynatraceOpenKitBuilder::getOrigDeviceID() const
{
	return mOrigDeviceID;
}

std::shared_ptr<openkit::ISSLTrustManager> DynatraceOpenKitBuilder::getTrustManager() const
{
	return mTrustManager;
}

int64_t DynatraceOpenKitBuilder::getBeaconCacheMaxRecordAge() const
{
	return mBeaconCacheMaxRecordAge;
}

int64_t DynatraceOpenKitBuilder::getBeaconCacheLowerMemoryBoundary() const
{
	return mBeaconCacheLowerMemoryBoundary;
}

int64_t DynatraceOpenKitBuilder::getBeaconCacheUpperMemoryBoundary() const
{
	return mBeaconCacheUpperMemoryBoundary;
}

openkit::DataCollectionLevel DynatraceOpenKitBuilder::getDataCollectionLevel() const
{
	return mDataCollectionLevel;
}

openkit::CrashReportingLevel DynatraceOpenKitBuilder::getCrashReportingLevel() const
{
	return mCrashReportingLevel;
}

std::shared_ptr<openkit::IHttpRequestInterceptor> DynatraceOpenKitBuilder::getHttpRequestInterceptor() const
{
	return mHttpRequestInterceptor;
}

std::shared_ptr<openkit::IHttpResponseInterceptor> DynatraceOpenKitBuilder::getHttpResponseInterceptor() const
{
	return mHttpResponseInterceptor;
}

openkit::LogLevel DynatraceOpenKitBuilder::getLogLevel() const
{
	return mLogLevel;
}

std::shared_ptr<openkit::ILogger> DynatraceOpenKitBuilder::getLogger() const
{
	if (mLogger != nullptr)
	{
		return mLogger;
	}
	return std::make_shared<core::util::DefaultLogger>(mLogLevel);
}


const std::string& DynatraceOpenKitBuilder::getOpenKitType() const
{
	return mOpenKitType;
}

const std::string& DynatraceOpenKitBuilder::getApplicationID() const
{
	return mApplicationID;
}

int32_t DynatraceOpenKitBuilder::getDefaultServerID() const
{
	return DEFAULT_SERVER_ID;
}
