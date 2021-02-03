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

#include "OpenKit/AbstractOpenKitBuilder.h"
#include "OpenKit/OpenKitConstants.h"
#include "core/configuration/ConfigurationDefaults.h"
#include "core/util/DefaultLogger.h"
#include "core/util/StringUtil.h"
#include "core/objects/OpenKitInitializer.h"
#include "core/objects/OpenKit.h"
#include "protocol/ssl/SSLStrictTrustManager.h"

#include <cstring>

using namespace openkit;

AbstractOpenKitBuilder::AbstractOpenKitBuilder(const char* endpointURL, const char* deviceID)
	: AbstractOpenKitBuilder(endpointURL, core::util::StringUtil::toNumericOr64BitHash(deviceID), deviceID)
{
}

AbstractOpenKitBuilder::AbstractOpenKitBuilder(const char* endpointURL, int64_t deviceID)
	: AbstractOpenKitBuilder(endpointURL, deviceID, core::util::StringUtil::toInvariantString(deviceID).c_str())
{
}

AbstractOpenKitBuilder::AbstractOpenKitBuilder(const char* endpointURL, int64_t deviceID, const char* origDeviceID)
	: mLogLevel(LogLevel::LOG_LEVEL_WARN)
	, mLogger(nullptr)
	, mApplicationVersion(DEFAULT_APPLICATION_VERSION)
	, mOperatingSystem(DEFAULT_OPERATING_SYSTEM)
	, mManufacturer(DEFAULT_MANUFACTURER)
	, mModelID(DEFAULT_MODEL_ID)
	, mEndpointURL(endpointURL)
	, mDeviceID(deviceID)
	, mOrigDeviceID(origDeviceID)
	, mTrustManager(std::make_shared<protocol::SSLStrictTrustManager>())
	, mBeaconCacheMaxRecordAge(core::configuration::DEFAULT_MAX_RECORD_AGE_IN_MILLIS)
	, mBeaconCacheLowerMemoryBoundary(core::configuration::DEFAULT_LOWER_MEMORY_BOUNDARY_IN_BYTES)
	, mBeaconCacheUpperMemoryBoundary(core::configuration::DEFAULT_UPPER_MEMORY_BOUNDARY_IN_BYTES)
	, mDataCollectionLevel(core::configuration::DEFAULT_DATA_COLLECTION_LEVEL)
	, mCrashReportingLevel(core::configuration::DEFAULT_CRASH_REPORTING_LEVEL)
{
}

AbstractOpenKitBuilder& AbstractOpenKitBuilder::enableVerbose()
{
	return withLogLevel(LogLevel::LOG_LEVEL_DEBUG);
}

AbstractOpenKitBuilder& AbstractOpenKitBuilder::withLogLevel(openkit::LogLevel logLevel)
{
	mLogLevel = logLevel;
	return *this;
}

AbstractOpenKitBuilder& AbstractOpenKitBuilder::withLogger(std::shared_ptr<ILogger> logger)
{
	mLogger = logger;
	return *this;
}

AbstractOpenKitBuilder& AbstractOpenKitBuilder::withApplicationVersion(const char* applicationVersion)
{
	if (applicationVersion != nullptr && strlen(applicationVersion) > 0)
	{
		mApplicationVersion = applicationVersion;
	}
	return *this;
}

AbstractOpenKitBuilder& AbstractOpenKitBuilder::withTrustManager(std::shared_ptr<openkit::ISSLTrustManager> trustManager)
{
	if (trustManager != nullptr)
	{
		mTrustManager = trustManager;
	}
	return *this;
}

AbstractOpenKitBuilder& AbstractOpenKitBuilder::withOperatingSystem(const char* operatingSystem)
{
	if (operatingSystem != nullptr && strlen(operatingSystem) > 0)
	{
		mOperatingSystem = operatingSystem;
	}
	return *this;
}

AbstractOpenKitBuilder& AbstractOpenKitBuilder::withManufacturer(const char* manufacturer)
{
	if (manufacturer != nullptr && strlen(manufacturer) > 0)
	{
		mManufacturer = manufacturer;
	}
	return *this;
}

AbstractOpenKitBuilder& AbstractOpenKitBuilder::withModelID(const char* modelID)
{
	if (modelID != nullptr && strlen(modelID) > 0)
	{
		mModelID = modelID;
	}
	return *this;
}

AbstractOpenKitBuilder& AbstractOpenKitBuilder::withBeaconCacheMaxRecordAge(int64_t maxRecordAgeInMilliseconds)
{
	mBeaconCacheMaxRecordAge = maxRecordAgeInMilliseconds;
	return *this;
}

AbstractOpenKitBuilder& AbstractOpenKitBuilder::withBeaconCacheLowerMemoryBoundary(int64_t lowerMemoryBoundaryInBytes)
{
	mBeaconCacheLowerMemoryBoundary = lowerMemoryBoundaryInBytes;
	return *this;
}

AbstractOpenKitBuilder& AbstractOpenKitBuilder::withBeaconCacheUpperMemoryBoundary(int64_t upperMemoryBoundaryInBytes)
{
	mBeaconCacheUpperMemoryBoundary = upperMemoryBoundaryInBytes;
	return *this;
}

AbstractOpenKitBuilder& AbstractOpenKitBuilder::withDataCollectionLevel(DataCollectionLevel dataCollectionLevel)
{
	mDataCollectionLevel = dataCollectionLevel;
	return *this;
}

AbstractOpenKitBuilder& AbstractOpenKitBuilder::withCrashReportingLevel(CrashReportingLevel crashReportingLevel)
{
	mCrashReportingLevel = crashReportingLevel;
	return *this;
}

std::shared_ptr<openkit::IOpenKit> AbstractOpenKitBuilder::build()
{
	core::objects::OpenKitInitializer initializer(*this);
	auto openKit = std::make_shared<core::objects::OpenKit>(initializer);
	openKit->initialize();

	return openKit;
}

const std::string& AbstractOpenKitBuilder::getApplicationVersion() const
{
	return mApplicationVersion;
}

const std::string& AbstractOpenKitBuilder::getOperatingSystem() const
{
	return mOperatingSystem;
}

const std::string& AbstractOpenKitBuilder::getManufacturer() const
{
	return mManufacturer;
}

const std::string& AbstractOpenKitBuilder::getModelID() const
{
	return mModelID;
}

const std::string& AbstractOpenKitBuilder::getEndpointURL() const
{
	return mEndpointURL;
}

int64_t AbstractOpenKitBuilder::getDeviceID() const
{
	return mDeviceID;
}

const std::string& AbstractOpenKitBuilder::getOrigDeviceID() const
{
	return mOrigDeviceID;
}

std::shared_ptr<openkit::ISSLTrustManager> AbstractOpenKitBuilder::getTrustManager() const
{
	return mTrustManager;
}

int64_t AbstractOpenKitBuilder::getBeaconCacheMaxRecordAge() const
{
	return mBeaconCacheMaxRecordAge;
}

int64_t AbstractOpenKitBuilder::getBeaconCacheLowerMemoryBoundary() const
{
	return mBeaconCacheLowerMemoryBoundary;
}

int64_t AbstractOpenKitBuilder::getBeaconCacheUpperMemoryBoundary() const
{
	return mBeaconCacheUpperMemoryBoundary;
}

openkit::DataCollectionLevel AbstractOpenKitBuilder::getDataCollectionLevel() const
{
	return mDataCollectionLevel;
}

openkit::CrashReportingLevel AbstractOpenKitBuilder::getCrashReportingLevel() const
{
	return mCrashReportingLevel;
}

openkit::LogLevel AbstractOpenKitBuilder::getLogLevel() const
{
	return mLogLevel;
}

std::shared_ptr<openkit::ILogger> AbstractOpenKitBuilder::getLogger() const
{
	if (mLogger != nullptr)
	{
		return mLogger;
	}
	return std::make_shared<core::util::DefaultLogger>(mLogLevel);
}
