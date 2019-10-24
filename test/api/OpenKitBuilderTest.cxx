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

#include "mock/MockISslTrustManager.h"

#include "OpenKit/AppMonOpenKitBuilder.h"
#include "OpenKit/CrashReportingLevel.h"
#include "OpenKit/DataCollectionLevel.h"
#include "OpenKit/DynatraceOpenKitBuilder.h"
#include "OpenKit/LogLevel.h"
#include "OpenKit/OpenKitConstants.h"
#include "core/configuration/Configuration.h"
#include "core/configuration/ConfigurationDefaults.h"
#include "core/util/DefaultLogger.h"
#include "core/util/StringUtil.h"
#include "protocol/ssl/SSLStrictTrustManager.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <stdint.h>
#include <memory>

using namespace test;

using AppMonOpenKitBuilder_t = openkit::AppMonOpenKitBuilder;
using CrashReportingLevel_t = openkit::CrashReportingLevel;
using DataCollectionLevel_t = openkit::DataCollectionLevel;
using DefaultLogger_t = core::util::DefaultLogger;
using DynatraceOpenKitBuilder_t = openkit::DynatraceOpenKitBuilder;
using LogLevel_t = openkit::LogLevel;
using MockNiceISslTrustManager_sp = std::shared_ptr<testing::NiceMock<MockISslTrustManager>>;
using SslStrictTrustManager_t = protocol::SSLStrictTrustManager;

class OpenKitBuilderTest : public testing::Test
{
protected:

	void SetUp()
	{
		mockTrustManager = MockISslTrustManager::createNice();
	}

	static constexpr const char* DEFAULT_ENDPOINT_URL = "https://localhost:12345";
	static constexpr const char* DEFAULT_APPLICATION_ID = "asdf123";
	static constexpr int64_t DEFAULT_DEVICE_ID = 123L;
	MockNiceISslTrustManager_sp mockTrustManager;
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
	auto configuration = AppMonOpenKitBuilder_t(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID).buildConfiguration();

	ASSERT_TRUE(configuration->getApplicationVersion().equals(openkit::DEFAULT_APPLICATION_VERSION));
	auto device = configuration->getDevice();
	ASSERT_TRUE(device->getManufacturer().equals(openkit::DEFAULT_MANUFACTURER));
	ASSERT_TRUE(device->getOperatingSystem().equals(openkit::DEFAULT_OPERATING_SYSTEM));
	ASSERT_TRUE(device->getModelID().equals(openkit::DEFAULT_MODEL_ID));

	auto trustManagerCast = std::dynamic_pointer_cast<SslStrictTrustManager_t>(configuration->getHTTPClientConfiguration()->getSSLTrustManager());
	ASSERT_TRUE(trustManagerCast != nullptr);

	auto beaconCacheConfiguration = configuration->getBeaconCacheConfiguration();
	ASSERT_TRUE(beaconCacheConfiguration != nullptr);
	ASSERT_EQ(beaconCacheConfiguration->getMaxRecordAge(), core::configuration::DEFAULT_MAX_RECORD_AGE_IN_MILLIS.count());
	ASSERT_EQ(beaconCacheConfiguration->getCacheSizeUpperBound(), core::configuration::DEFAULT_UPPER_MEMORY_BOUNDARY_IN_BYTES);
	ASSERT_EQ(beaconCacheConfiguration->getCacheSizeLowerBound(), core::configuration::DEFAULT_LOWER_MEMORY_BOUNDARY_IN_BYTES);

	auto beaconConfiguration = configuration->getBeaconConfiguration();
	ASSERT_EQ(beaconConfiguration->getDataCollectionLevel(), core::configuration::DEFAULT_DATA_COLLECTION_LEVEL);
	ASSERT_EQ(beaconConfiguration->getCrashReportingLevel(), core::configuration::DEFAULT_CRASH_REPORTING_LEVEL);
}

TEST_F(OpenKitBuilderTest, appMonOpenKitBuilderTakesStringDeviceID)
{
    // when
    auto deviceIdString = std::string("some text");
    auto configuration = AppMonOpenKitBuilder_t(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, deviceIdString.c_str()).buildConfiguration();

    // when
    auto hashedDeviceId = core::util::StringUtil::to64BitHash(deviceIdString);

    // then
    ASSERT_EQ(configuration->getDeviceID(), hashedDeviceId);
}

TEST_F(OpenKitBuilderTest, appMonOpenKitBuilderTakesOverNumericDeviceIdString)
{
    // given
    auto configuration = AppMonOpenKitBuilder_t(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, "42").buildConfiguration();

    // when, then
    ASSERT_EQ(configuration->getDeviceID(), 42);
}

TEST_F(OpenKitBuilderTest, appMonOpenKitBuilderTrimsDeviceIdString)
{
    // given
    const char* deviceIdString = " 42 ";
    auto configuration = AppMonOpenKitBuilder_t(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, deviceIdString).buildConfiguration();

    // when, then
    ASSERT_EQ(configuration->getDeviceID(), 42);
}

TEST_F(OpenKitBuilderTest, appMonKitBuilderTakesNumericDeviceId)
{
    //given
    auto deviceId = 42;
    auto configuration = AppMonOpenKitBuilder_t(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, deviceId).buildConfiguration();

    // when, then
    ASSERT_EQ(configuration->getDeviceID(), deviceId);
}

TEST_F(OpenKitBuilderTest, defaultsAreSetForDynatrace)
{
	auto configuration = DynatraceOpenKitBuilder_t(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID).buildConfiguration();

	ASSERT_TRUE(configuration->getApplicationVersion().equals(openkit::DEFAULT_APPLICATION_VERSION));
	auto device = configuration->getDevice();
	ASSERT_TRUE(device->getManufacturer().equals(openkit::DEFAULT_MANUFACTURER));
	ASSERT_TRUE(device->getOperatingSystem().equals(openkit::DEFAULT_OPERATING_SYSTEM));
	ASSERT_TRUE(device->getModelID().equals(openkit::DEFAULT_MODEL_ID));

	auto trustManagerCast = std::dynamic_pointer_cast<protocol::SSLStrictTrustManager>(configuration->getHTTPClientConfiguration()->getSSLTrustManager());
	ASSERT_TRUE(trustManagerCast != nullptr);

	auto beaconCacheConfiguration = configuration->getBeaconCacheConfiguration();
	ASSERT_TRUE(beaconCacheConfiguration != nullptr);
	ASSERT_EQ(beaconCacheConfiguration->getMaxRecordAge(), core::configuration::DEFAULT_MAX_RECORD_AGE_IN_MILLIS.count());
	ASSERT_EQ(beaconCacheConfiguration->getCacheSizeUpperBound(), core::configuration::DEFAULT_UPPER_MEMORY_BOUNDARY_IN_BYTES);
	ASSERT_EQ(beaconCacheConfiguration->getCacheSizeLowerBound(), core::configuration::DEFAULT_LOWER_MEMORY_BOUNDARY_IN_BYTES);
}

TEST_F(OpenKitBuilderTest, dynatraceOpenKitBuilderTakesStringDeviceId)
{
    // given
    auto deviceIdString = std::string("some text");
    auto configuration = DynatraceOpenKitBuilder_t(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, deviceIdString.c_str()).buildConfiguration();

    // when
    auto hashedDeviceId = core::util::StringUtil::to64BitHash(deviceIdString);

    // then
    ASSERT_EQ(configuration->getDeviceID(), hashedDeviceId);
}

TEST_F(OpenKitBuilderTest, dynatraceOpenKitBuilderTakesOverNumericDeviceIdString)
{
    // given
    auto configuration = DynatraceOpenKitBuilder_t(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, "42").buildConfiguration();

    // when, then
    ASSERT_EQ(configuration->getDeviceID(), 42);
}

TEST_F(OpenKitBuilderTest, dynatraceOpenKitBuilderTrimsDeviceIdString)
{
    // given
    const char* deviceIdString = " 42 ";
    auto configuration = DynatraceOpenKitBuilder_t(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, deviceIdString).buildConfiguration();

    // when, then
    ASSERT_EQ(configuration->getDeviceID(), 42);
}

TEST_F(OpenKitBuilderTest, dynatraceOpenKitBuilderTakesNumericDeviceId)
{
    //given
    auto deviceId = 42;
    auto configuration = DynatraceOpenKitBuilder_t(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, deviceId).buildConfiguration();

    // when, then
    ASSERT_EQ(configuration->getDeviceID(), deviceId);
}

TEST_F(OpenKitBuilderTest, applicationNameIsSetCorrectlyForAppMon)
{
	auto configuration = AppMonOpenKitBuilder_t(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID).buildConfiguration();
	ASSERT_TRUE(configuration->getApplicationName() == DEFAULT_APPLICATION_ID);
	ASSERT_EQ(configuration->getApplicationName(), configuration->getApplicationID());
}

TEST_F(OpenKitBuilderTest, canOverrideTrustManagerForAppMon)
{
	auto configuration = AppMonOpenKitBuilder_t(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withTrustManager(mockTrustManager)
		.buildConfiguration();

	ASSERT_EQ(configuration->getHTTPClientConfiguration()->getSSLTrustManager(), mockTrustManager);
}

TEST_F(OpenKitBuilderTest, cannotOverrideNullTrustManagerForAppMon)
{
	auto configuration = AppMonOpenKitBuilder_t(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withTrustManager(mockTrustManager) // first call, set a known & valid trust manager
		.withTrustManager(nullptr)
		.buildConfiguration();

	ASSERT_EQ(configuration->getHTTPClientConfiguration()->getSSLTrustManager(), mockTrustManager);
}

TEST_F(OpenKitBuilderTest, canOverrideTrustManagerForDynatrace)
{
	auto configuration = DynatraceOpenKitBuilder_t(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withTrustManager(mockTrustManager)
		.buildConfiguration();

	ASSERT_EQ(configuration->getHTTPClientConfiguration()->getSSLTrustManager(), mockTrustManager);
}

TEST_F(OpenKitBuilderTest, cannotOverrideNullTrustManagerForDynatrace)
{
	auto configuration = DynatraceOpenKitBuilder_t(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withTrustManager(mockTrustManager) // first call, set a known & valid trust manager
		.withTrustManager(nullptr)
		.buildConfiguration();

	ASSERT_EQ(configuration->getHTTPClientConfiguration()->getSSLTrustManager(), mockTrustManager);
}

TEST_F(OpenKitBuilderTest, canSetApplicationVersionForAppMon)
{
	auto configuration = AppMonOpenKitBuilder_t(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withApplicationVersion(TEST_APPLICATION_VERSION)
		.buildConfiguration();

	ASSERT_EQ(configuration->getApplicationVersion(), TEST_APPLICATION_VERSION);
}

TEST_F(OpenKitBuilderTest, canSetApplicationVersionForDynatrace)
{
	auto configuration = DynatraceOpenKitBuilder_t(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withApplicationVersion(TEST_APPLICATION_VERSION)
		.buildConfiguration();

	ASSERT_EQ(configuration->getApplicationVersion(), TEST_APPLICATION_VERSION);
}

TEST_F(OpenKitBuilderTest, canSetOperatingSystemForAppMon)
{
	auto configuration = AppMonOpenKitBuilder_t(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withOperatingSystem(TEST_OPERATING_SYSTEM)
		.buildConfiguration();

	ASSERT_EQ(configuration->getDevice()->getOperatingSystem(), TEST_OPERATING_SYSTEM);
}

TEST_F(OpenKitBuilderTest, canSetOperatingSystemForDynatrace)
{
	auto configuration = DynatraceOpenKitBuilder_t(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withOperatingSystem(TEST_OPERATING_SYSTEM)
		.buildConfiguration();

	ASSERT_EQ(configuration->getDevice()->getOperatingSystem(), TEST_OPERATING_SYSTEM);
}

TEST_F(OpenKitBuilderTest, canSetManufacturerForAppMon)
{
	auto configuration = AppMonOpenKitBuilder_t(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withManufacturer(TEST_MANUFACTURER)
		.buildConfiguration();

	ASSERT_EQ(configuration->getDevice()->getManufacturer(), TEST_MANUFACTURER);
}

TEST_F(OpenKitBuilderTest, canSetManufactureForDynatrace)
{
	auto configuration = DynatraceOpenKitBuilder_t(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withManufacturer(TEST_MANUFACTURER)
		.buildConfiguration();

	ASSERT_EQ(configuration->getDevice()->getManufacturer(), TEST_MANUFACTURER);
}

TEST_F(OpenKitBuilderTest, canSetModelIDForAppMon)
{
	auto configuration = AppMonOpenKitBuilder_t(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withModelID(TEST_MODEL_ID)
		.buildConfiguration();

	ASSERT_EQ(configuration->getDevice()->getModelID(), TEST_MODEL_ID);
}

TEST_F(OpenKitBuilderTest, canSetModelIDForDynatrace)
{
	auto configuration = DynatraceOpenKitBuilder_t(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withModelID(TEST_MODEL_ID)
		.buildConfiguration();

	ASSERT_EQ(configuration->getDevice()->getModelID(), TEST_MODEL_ID);
}

TEST_F(OpenKitBuilderTest, defaultLoggerIsUsedByDefault)
{
	auto logger = DynatraceOpenKitBuilder_t(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withLogLevel(LogLevel_t::LOG_LEVEL_DEBUG)
		.getLogger();

	auto typeCastLogger = std::dynamic_pointer_cast<DefaultLogger_t>(logger);

	ASSERT_TRUE(typeCastLogger != nullptr);
}

TEST_F(OpenKitBuilderTest, verboseIsUsedInDefaultLogger)
{
	auto logger = DynatraceOpenKitBuilder_t(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withLogLevel(LogLevel_t::LOG_LEVEL_DEBUG)
		.getLogger();

	auto typeCastLogger = std::dynamic_pointer_cast<DefaultLogger_t>(logger);

	ASSERT_TRUE(typeCastLogger != nullptr);
	ASSERT_TRUE(typeCastLogger->isDebugEnabled());
	ASSERT_TRUE(typeCastLogger->isInfoEnabled());
}

TEST_F(OpenKitBuilderTest, canSetCustomMaxBeaconRecordAgeForAppMon)
{
	auto configuration = AppMonOpenKitBuilder_t(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withBeaconCacheMaxRecordAge(TEST_CACHE_MAX_RECORD_AGE)
		.buildConfiguration();

	ASSERT_EQ(configuration->getBeaconCacheConfiguration()->getMaxRecordAge(), TEST_CACHE_MAX_RECORD_AGE);
}

TEST_F(OpenKitBuilderTest, canSetCustomMaxBeaconRecordAgeForDynatrace)
{
	auto configuration = DynatraceOpenKitBuilder_t(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withBeaconCacheMaxRecordAge(TEST_CACHE_MAX_RECORD_AGE)
		.buildConfiguration();

	ASSERT_EQ(configuration->getBeaconCacheConfiguration()->getMaxRecordAge(), TEST_CACHE_MAX_RECORD_AGE);
}


TEST_F(OpenKitBuilderTest, canSetBeaconCacheLowerMemoryBoundaryForAppMon)
{
	auto configuration = AppMonOpenKitBuilder_t(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withBeaconCacheLowerMemoryBoundary(TEST_CACHE_LOWER_MEMORY_BOUNDARY)
		.buildConfiguration();

	ASSERT_EQ(configuration->getBeaconCacheConfiguration()->getCacheSizeLowerBound(), TEST_CACHE_LOWER_MEMORY_BOUNDARY);
}

TEST_F(OpenKitBuilderTest, canSetBeaconCacheLowerMemoryBoundaryForDynatrace)
{
	auto configuration = DynatraceOpenKitBuilder_t(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withBeaconCacheLowerMemoryBoundary(TEST_CACHE_LOWER_MEMORY_BOUNDARY)
		.buildConfiguration();

	ASSERT_EQ(configuration->getBeaconCacheConfiguration()->getCacheSizeLowerBound(), TEST_CACHE_LOWER_MEMORY_BOUNDARY);
}

TEST_F(OpenKitBuilderTest, canSetBeaconCacheUpperMemoryBoundaryForAppMon)
{
	auto configuration = AppMonOpenKitBuilder_t(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withBeaconCacheUpperMemoryBoundary(TEST_CACHE_UPPER_MEMORY_BOUNDARY)
		.buildConfiguration();

	ASSERT_EQ(configuration->getBeaconCacheConfiguration()->getCacheSizeUpperBound(), TEST_CACHE_UPPER_MEMORY_BOUNDARY);
}

TEST_F(OpenKitBuilderTest, canSetBeaconCacheUpperMemoryBoundaryForDynatrace)
{
	auto configuration = DynatraceOpenKitBuilder_t(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withBeaconCacheUpperMemoryBoundary(TEST_CACHE_UPPER_MEMORY_BOUNDARY)
		.buildConfiguration();

	ASSERT_EQ(configuration->getBeaconCacheConfiguration()->getCacheSizeUpperBound(), TEST_CACHE_UPPER_MEMORY_BOUNDARY);
}

TEST_F(OpenKitBuilderTest, canSetDataCollectionLevelForDynatrace)
{
	auto configuration = DynatraceOpenKitBuilder_t(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withDataCollectionLevel(DataCollectionLevel_t::PERFORMANCE)
		.buildConfiguration();

	ASSERT_EQ(configuration->getBeaconConfiguration()->getDataCollectionLevel(), DataCollectionLevel_t::PERFORMANCE);
}

TEST_F(OpenKitBuilderTest, canSetDataCollectionLevelForAppMon)
{
	auto configuration = AppMonOpenKitBuilder_t(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withDataCollectionLevel(DataCollectionLevel_t::PERFORMANCE)
		.buildConfiguration();

	ASSERT_EQ(configuration->getBeaconConfiguration()->getDataCollectionLevel(), DataCollectionLevel_t::PERFORMANCE);
}

TEST_F(OpenKitBuilderTest, canSetCrashReportingLevelForDynatrace)
{
	auto configuration = DynatraceOpenKitBuilder_t(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withCrashReportingLevel(CrashReportingLevel_t::OPT_IN_CRASHES)
		.buildConfiguration();

	ASSERT_EQ(configuration->getBeaconConfiguration()->getCrashReportingLevel(), CrashReportingLevel_t::OPT_IN_CRASHES);
}

TEST_F(OpenKitBuilderTest, canSetCrashReportingLevelForAppMon)
{
	auto configuration = AppMonOpenKitBuilder_t(DEFAULT_ENDPOINT_URL, DEFAULT_APPLICATION_ID, DEFAULT_DEVICE_ID)
		.withCrashReportingLevel(CrashReportingLevel_t::OPT_IN_CRASHES)
		.buildConfiguration();

	ASSERT_EQ(configuration->getBeaconConfiguration()->getCrashReportingLevel(), CrashReportingLevel_t::OPT_IN_CRASHES);
}
