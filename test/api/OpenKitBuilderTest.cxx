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

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <stdint.h>
#include <memory>

#include "OpenKit/DynatraceOpenKitBuilder.h"
#include "OpenKit/AppMonOpenKitBuilder.h"

#include "configuration/Configuration.h"
#include "OpenKit/OpenKitConstants.h"
#include "protocol/ssl/SSLStrictTrustManager.h"
#include "core/util/DefaultLogger.h"

#include "../protocol/TestSSLTrustManager.h"

using namespace openkit;

class OpenKitBuilderTest : public testing::Test
{
protected:

	void SetUp()
	{
		testSSLTrustManager = std::make_shared<test::TestSSLTrustManager>();
	}

	static constexpr const char* DEFAULT_ENDPOINT_URL = "https://localhost:12345";
	static constexpr const char* DEFAULT_APPLICATION_ID = "asdf123";
	static constexpr int64_t DEFAULT_DEVICE_ID = 123L;
	std::shared_ptr<openkit::ISSLTrustManager> testSSLTrustManager;
	static constexpr const char* TEST_APPLICATION_VERSION = "1.2.3.4";
	static constexpr const char* TEST_OPERATING_SYSTEM = "Some OS";
	static constexpr const char* TEST_MANUFACTURER = "ACME";
	static constexpr const char* TEST_MODEL_ID = "abc.123";
	static constexpr int64_t TEST_CACHE_MAX_RECORD_AGE = 123456L;
	static constexpr int64_t TEST_CACHE_LOWER_MEMORY_BOUNDARY = 42 * 1024;
	static constexpr int64_t TEST_CACHE_UPPER_MEMORY_BOUNDARY = 144 * 1024;
};

constexpr const char* OpenKitBuilderTest::DEFAULT_ENDPOINT_URL;
constexpr const char* OpenKitBuilderTest::DEFAULT_APPLICATION_ID;
constexpr int64_t OpenKitBuilderTest::DEFAULT_DEVICE_ID;
constexpr const char* OpenKitBuilderTest::TEST_APPLICATION_VERSION;
constexpr const char* OpenKitBuilderTest::TEST_OPERATING_SYSTEM;
constexpr const char* OpenKitBuilderTest::TEST_MANUFACTURER;
constexpr const char* OpenKitBuilderTest::TEST_MODEL_ID;
constexpr int64_t OpenKitBuilderTest::TEST_CACHE_MAX_RECORD_AGE;
constexpr int64_t OpenKitBuilderTest::TEST_CACHE_LOWER_MEMORY_BOUNDARY;
constexpr int64_t OpenKitBuilderTest::TEST_CACHE_UPPER_MEMORY_BOUNDARY;

TEST_F(OpenKitBuilderTest, defaultsAreSetForAppMon)
{
	auto configuration = AppMonOpenKitBuilder(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID).buildConfiguration();

	ASSERT_TRUE(configuration->getApplicationVersion().equals(openkit::DEFAULT_APPLICATION_VERSION));
	auto device = configuration->getDevice();
	ASSERT_TRUE(device->getManufacturer().equals(openkit::DEFAULT_MANUFACTURER));
	ASSERT_TRUE(device->getOperatingSystem().equals(openkit::DEFAULT_OPERATING_SYSTEM));
	ASSERT_TRUE(device->getModelID().equals(openkit::DEFAULT_MODEL_ID));

	auto trustManagerCast = std::dynamic_pointer_cast<protocol::SSLStrictTrustManager>(configuration->getHTTPClientConfiguration()->getSSLTrustManager());
	ASSERT_TRUE(trustManagerCast != nullptr);

	auto beaconCacheConfiguration = configuration->getBeaconCacheConfiguration();
	ASSERT_TRUE(beaconCacheConfiguration != nullptr);
	ASSERT_EQ(beaconCacheConfiguration->getMaxRecordAge(), configuration::BeaconCacheConfiguration::DEFAULT_MAX_RECORD_AGE_IN_MILLIS.count());
	ASSERT_EQ(beaconCacheConfiguration->getCacheSizeUpperBound(), configuration::BeaconCacheConfiguration::DEFAULT_UPPER_MEMORY_BOUNDARY_IN_BYTES);
	ASSERT_EQ(beaconCacheConfiguration->getCacheSizeLowerBound(), configuration::BeaconCacheConfiguration::DEFAULT_LOWER_MEMORY_BOUNDARY_IN_BYTES);

	auto beaconConfiguration = configuration->getBeaconConfiguration();
	ASSERT_EQ(beaconConfiguration->getDataCollectionLevel(), configuration::BeaconConfiguration::DEFAULT_DATA_COLLECTION_LEVEL);
	ASSERT_EQ(beaconConfiguration->getCrashReportingLevel(), configuration::BeaconConfiguration::DEFAULT_CRASH_REPORTING_LEVEL);
}

TEST_F(OpenKitBuilderTest, defaultsAreSetForDynatrace)
{
	auto configuration = DynatraceOpenKitBuilder(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID).buildConfiguration();

	ASSERT_TRUE(configuration->getApplicationVersion().equals(openkit::DEFAULT_APPLICATION_VERSION));
	auto device = configuration->getDevice();
	ASSERT_TRUE(device->getManufacturer().equals(openkit::DEFAULT_MANUFACTURER));
	ASSERT_TRUE(device->getOperatingSystem().equals(openkit::DEFAULT_OPERATING_SYSTEM));
	ASSERT_TRUE(device->getModelID().equals(openkit::DEFAULT_MODEL_ID));

	auto trustManagerCast = std::dynamic_pointer_cast<protocol::SSLStrictTrustManager>(configuration->getHTTPClientConfiguration()->getSSLTrustManager());
	ASSERT_TRUE(trustManagerCast != nullptr);

	auto beaconCacheConfiguration = configuration->getBeaconCacheConfiguration();
	ASSERT_TRUE(beaconCacheConfiguration != nullptr);
	ASSERT_EQ(beaconCacheConfiguration->getMaxRecordAge(), configuration::BeaconCacheConfiguration::DEFAULT_MAX_RECORD_AGE_IN_MILLIS.count());
	ASSERT_EQ(beaconCacheConfiguration->getCacheSizeUpperBound(), configuration::BeaconCacheConfiguration::DEFAULT_UPPER_MEMORY_BOUNDARY_IN_BYTES);
	ASSERT_EQ(beaconCacheConfiguration->getCacheSizeLowerBound(), configuration::BeaconCacheConfiguration::DEFAULT_LOWER_MEMORY_BOUNDARY_IN_BYTES);
}

TEST_F(OpenKitBuilderTest, applicationNameIsSetCorrectlyForAppMon)
{
	auto configuration = AppMonOpenKitBuilder(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID).buildConfiguration();
	ASSERT_TRUE(configuration->getApplicationName() == DEFAULT_APPLICATION_ID);
	ASSERT_EQ(configuration->getApplicationName(), configuration->getApplicationID());
}

TEST_F(OpenKitBuilderTest, canOverrideTrustManagerForAppMon)
{
	auto configuration = AppMonOpenKitBuilder(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withTrustManager(testSSLTrustManager)
		.buildConfiguration();

	ASSERT_EQ(configuration->getHTTPClientConfiguration()->getSSLTrustManager(), testSSLTrustManager);
}

TEST_F(OpenKitBuilderTest, cannotOverrideNullTrustManagerForAppMon)
{
	auto configuration = AppMonOpenKitBuilder(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withTrustManager(testSSLTrustManager) // first call, set a known & valid trust manager
		.withTrustManager(nullptr)
		.buildConfiguration();

	ASSERT_EQ(configuration->getHTTPClientConfiguration()->getSSLTrustManager(), testSSLTrustManager);
}

TEST_F(OpenKitBuilderTest, canOverrideTrustManagerForDynatrace)
{
	auto configuration = DynatraceOpenKitBuilder(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withTrustManager(testSSLTrustManager)
		.buildConfiguration();

	ASSERT_EQ(configuration->getHTTPClientConfiguration()->getSSLTrustManager(), testSSLTrustManager);
}

TEST_F(OpenKitBuilderTest, cannotOverrideNullTrustManagerForDynatrace)
{
	auto configuration = DynatraceOpenKitBuilder(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withTrustManager(testSSLTrustManager) // first call, set a known & valid trust manager
		.withTrustManager(nullptr)
		.buildConfiguration();

	ASSERT_EQ(configuration->getHTTPClientConfiguration()->getSSLTrustManager(), testSSLTrustManager);
}

TEST_F(OpenKitBuilderTest, canSetApplicationVersionForAppMon)
{
	auto configuration = AppMonOpenKitBuilder(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withApplicationVersion(TEST_APPLICATION_VERSION)
		.buildConfiguration();

	ASSERT_EQ(configuration->getApplicationVersion(), TEST_APPLICATION_VERSION);
}

TEST_F(OpenKitBuilderTest, canSetApplicationVersionForDynatrace)
{
	auto configuration = DynatraceOpenKitBuilder(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withApplicationVersion(TEST_APPLICATION_VERSION)
		.buildConfiguration();

	ASSERT_EQ(configuration->getApplicationVersion(), TEST_APPLICATION_VERSION);
}

TEST_F(OpenKitBuilderTest, canSetOperatingSystemForAppMon)
{
	auto configuration = AppMonOpenKitBuilder(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withOperatingSystem(TEST_OPERATING_SYSTEM)
		.buildConfiguration();

	ASSERT_EQ(configuration->getDevice()->getOperatingSystem(), TEST_OPERATING_SYSTEM);
}

TEST_F(OpenKitBuilderTest, canSetOperatingSystemForDynatrace)
{
	auto configuration = DynatraceOpenKitBuilder(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withOperatingSystem(TEST_OPERATING_SYSTEM)
		.buildConfiguration();

	ASSERT_EQ(configuration->getDevice()->getOperatingSystem(), TEST_OPERATING_SYSTEM);
}

TEST_F(OpenKitBuilderTest, canSetManufacturerForAppMon)
{
	auto configuration = AppMonOpenKitBuilder(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withManufacturer(TEST_MANUFACTURER)
		.buildConfiguration();

	ASSERT_EQ(configuration->getDevice()->getManufacturer(), TEST_MANUFACTURER);
}

TEST_F(OpenKitBuilderTest, canSetManufactureForDynatrace)
{
	auto configuration = DynatraceOpenKitBuilder(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withManufacturer(TEST_MANUFACTURER)
		.buildConfiguration();

	ASSERT_EQ(configuration->getDevice()->getManufacturer(), TEST_MANUFACTURER);
}

TEST_F(OpenKitBuilderTest, canSetModelIDForAppMon)
{
	auto configuration = AppMonOpenKitBuilder(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withModelID(TEST_MODEL_ID)
		.buildConfiguration();

	ASSERT_EQ(configuration->getDevice()->getModelID(), TEST_MODEL_ID);
}

TEST_F(OpenKitBuilderTest, canSetModelIDForDynatrace)
{
	auto configuration = DynatraceOpenKitBuilder(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withModelID(TEST_MODEL_ID)
		.buildConfiguration();

	ASSERT_EQ(configuration->getDevice()->getModelID(), TEST_MODEL_ID);
}

TEST_F(OpenKitBuilderTest, defaultLoggerIsUsedByDefault)
{
	auto logger = DynatraceOpenKitBuilder(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withLogLevel(openkit::LogLevel::LOG_LEVEL_DEBUG)
		.getLogger();

	auto typeCastLogger = std::dynamic_pointer_cast<core::util::DefaultLogger>(logger);

	ASSERT_TRUE(typeCastLogger != nullptr);
}

TEST_F(OpenKitBuilderTest, verboseIsUsedInDefaultLogger)
{
	auto logger = DynatraceOpenKitBuilder(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withLogLevel(openkit::LogLevel::LOG_LEVEL_DEBUG)
		.getLogger();

	auto typeCastLogger = std::dynamic_pointer_cast<core::util::DefaultLogger>(logger);

	ASSERT_TRUE(typeCastLogger != nullptr);
	ASSERT_TRUE(typeCastLogger->isDebugEnabled());
	ASSERT_TRUE(typeCastLogger->isInfoEnabled());
}

TEST_F(OpenKitBuilderTest, canSetCustomMaxBeaconRecordAgeForAppMon)
{
	auto configuration = AppMonOpenKitBuilder(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withBeaconCacheMaxRecordAge(TEST_CACHE_MAX_RECORD_AGE)
		.buildConfiguration();

	ASSERT_EQ(configuration->getBeaconCacheConfiguration()->getMaxRecordAge(), TEST_CACHE_MAX_RECORD_AGE);
}

TEST_F(OpenKitBuilderTest, canSetCustomMaxBeaconRecordAgeForDynatrace)
{
	auto configuration = DynatraceOpenKitBuilder(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withBeaconCacheMaxRecordAge(TEST_CACHE_MAX_RECORD_AGE)
		.buildConfiguration();

	ASSERT_EQ(configuration->getBeaconCacheConfiguration()->getMaxRecordAge(), TEST_CACHE_MAX_RECORD_AGE);
}


TEST_F(OpenKitBuilderTest, canSetBeaconCacheLowerMemoryBoundaryForAppMon)
{
	auto configuration = AppMonOpenKitBuilder(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withBeaconCacheLowerMemoryBoundary(TEST_CACHE_LOWER_MEMORY_BOUNDARY)
		.buildConfiguration();

	ASSERT_EQ(configuration->getBeaconCacheConfiguration()->getCacheSizeLowerBound(), TEST_CACHE_LOWER_MEMORY_BOUNDARY);
}

TEST_F(OpenKitBuilderTest, canSetBeaconCacheLowerMemoryBoundaryForDynatrace)
{
	auto configuration = DynatraceOpenKitBuilder(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withBeaconCacheLowerMemoryBoundary(TEST_CACHE_LOWER_MEMORY_BOUNDARY)
		.buildConfiguration();

	ASSERT_EQ(configuration->getBeaconCacheConfiguration()->getCacheSizeLowerBound(), TEST_CACHE_LOWER_MEMORY_BOUNDARY);
}

TEST_F(OpenKitBuilderTest, canSetBeaconCacheUpperMemoryBoundaryForAppMon)
{
	auto configuration = AppMonOpenKitBuilder(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withBeaconCacheUpperMemoryBoundary(TEST_CACHE_UPPER_MEMORY_BOUNDARY)
		.buildConfiguration();

	ASSERT_EQ(configuration->getBeaconCacheConfiguration()->getCacheSizeUpperBound(), TEST_CACHE_UPPER_MEMORY_BOUNDARY);
}

TEST_F(OpenKitBuilderTest, canSetBeaconCacheUpperMemoryBoundaryForDynatrace)
{
	auto configuration = DynatraceOpenKitBuilder(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withBeaconCacheUpperMemoryBoundary(TEST_CACHE_UPPER_MEMORY_BOUNDARY)
		.buildConfiguration();

	ASSERT_EQ(configuration->getBeaconCacheConfiguration()->getCacheSizeUpperBound(), TEST_CACHE_UPPER_MEMORY_BOUNDARY);
}

TEST_F(OpenKitBuilderTest, canSetDataCollectionLevelForDynatrace)
{
	auto configuration = DynatraceOpenKitBuilder(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withDataCollectionLevel(DataCollectionLevel::PERFORMANCE)
		.buildConfiguration();

	ASSERT_EQ(configuration->getBeaconConfiguration()->getDataCollectionLevel(), DataCollectionLevel::PERFORMANCE);
}

TEST_F(OpenKitBuilderTest, canSetDataCollectionLevelForAppMon)
{
	auto configuration = AppMonOpenKitBuilder(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withDataCollectionLevel(DataCollectionLevel::PERFORMANCE)
		.buildConfiguration();

	ASSERT_EQ(configuration->getBeaconConfiguration()->getDataCollectionLevel(), DataCollectionLevel::PERFORMANCE);
}

TEST_F(OpenKitBuilderTest, canSetCrashReportingLevelForDynatrace)
{
	auto configuration = DynatraceOpenKitBuilder(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withCrashReportingLevel(CrashReportingLevel::OPT_IN_CRASHES)
		.buildConfiguration();

	ASSERT_EQ(configuration->getBeaconConfiguration()->getCrashReportingLevel(), CrashReportingLevel::OPT_IN_CRASHES);
}

TEST_F(OpenKitBuilderTest, canSetCrashReportingLevelForAppMon)
{
	auto configuration = AppMonOpenKitBuilder(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withCrashReportingLevel(CrashReportingLevel::OPT_IN_CRASHES)
		.buildConfiguration();

	ASSERT_EQ(configuration->getBeaconConfiguration()->getCrashReportingLevel(), CrashReportingLevel::OPT_IN_CRASHES);
}
