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

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <stdint.h>
#include <memory>

#include "OpenKit/DynatraceOpenKitBuilder.h"
#include "OpenKit/AppMonOpenKitBuilder.h"

#include "OpenKit/OpenKitConstants.h"
#include "protocol/ssl/SSLStrictTrustManager.h"
#include "core/util/DefaultLogger.h"

#include "../protocol/TestSSLTrustManager.h"

using namespace openkit;

class OpenKitBuilderTest : public testing::Test
{
public:

	void SetUp()
	{
		testSSLTrustManager = std::make_shared<test::TestSSLTrustManager>();
	}

	const char* defaultEndpointURL = "https://localhost:12345";
	const char* defaultApplicationID = "asdf123";
	const int64_t defaultDeviceID = 123L;
	std::shared_ptr<protocol::ISSLTrustManager> testSSLTrustManager;
	const char* testApplicationVersion = "1.2.3.4";
	const char* testOperatingSystem = "Some OS";
	const char* testManufacturer = "ACME";
	const char* testModelID = "abc.123";
	int64_t testCacheMaxRecordAge = 123456L;
	int64_t testCacheLowerMemoryBoundary = 42 * 1024;
	int64_t testCacheUpperMemoryBoundary = 144 * 1024;
};

TEST_F(OpenKitBuilderTest, defaultsAreSetForAppMon)
{
	auto configuration = AppMonOpenKitBuilder(defaultEndpointURL, defaultApplicationID, defaultDeviceID).buildConfiguration();

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

TEST_F(OpenKitBuilderTest, defaultsAreSetForDynatrace)
{
	auto configuration = DynatraceOpenKitBuilder(defaultEndpointURL, defaultApplicationID, defaultDeviceID).buildConfiguration();

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
	auto configuration = AppMonOpenKitBuilder(defaultEndpointURL, defaultApplicationID, defaultDeviceID).buildConfiguration();
	ASSERT_TRUE(configuration->getApplicationName() == defaultApplicationID);
	ASSERT_EQ(configuration->getApplicationName(), configuration->getApplicationID());
}

TEST_F(OpenKitBuilderTest, canOverrideTrustManagerForAppMon)
{
	auto configuration = AppMonOpenKitBuilder(defaultEndpointURL, defaultApplicationID, defaultDeviceID)
		.withTrustManager(testSSLTrustManager)
		.buildConfiguration();

	ASSERT_EQ(configuration->getHTTPClientConfiguration()->getSSLTrustManager(), testSSLTrustManager);
}

TEST_F(OpenKitBuilderTest, canOverrideTrustManagerForDynatrace)
{
	auto configuration = DynatraceOpenKitBuilder(defaultEndpointURL, defaultApplicationID, defaultDeviceID)
		.withTrustManager(testSSLTrustManager)
		.buildConfiguration();

	ASSERT_EQ(configuration->getHTTPClientConfiguration()->getSSLTrustManager(), testSSLTrustManager);
}

TEST_F(OpenKitBuilderTest, canSetApplicationVersionForAppMon)
{
	auto configuration = AppMonOpenKitBuilder(defaultEndpointURL, defaultApplicationID, defaultDeviceID)
		.withApplicationVersion(testApplicationVersion)
		.buildConfiguration();

	ASSERT_EQ(configuration->getApplicationVersion(), testApplicationVersion);
}

TEST_F(OpenKitBuilderTest, canSetApplicationVersionForDynatrace)
{
	auto configuration = DynatraceOpenKitBuilder(defaultEndpointURL, defaultApplicationID, defaultDeviceID)
		.withApplicationVersion(testApplicationVersion)
		.buildConfiguration();

	ASSERT_EQ(configuration->getApplicationVersion(), testApplicationVersion);
}

TEST_F(OpenKitBuilderTest, canSetOperatingSystemForAppMon)
{
	auto configuration = AppMonOpenKitBuilder(defaultEndpointURL, defaultApplicationID, defaultDeviceID)
		.withOperatingSystem(testOperatingSystem)
		.buildConfiguration();

	ASSERT_EQ(configuration->getDevice()->getOperatingSystem(), testOperatingSystem);
}

TEST_F(OpenKitBuilderTest, canSetOperatingSystemForDynatrace)
{
	auto configuration = DynatraceOpenKitBuilder(defaultEndpointURL, defaultApplicationID, defaultDeviceID)
		.withOperatingSystem(testOperatingSystem)
		.buildConfiguration();

	ASSERT_EQ(configuration->getDevice()->getOperatingSystem(), testOperatingSystem);
}

TEST_F(OpenKitBuilderTest, canSetManufacturerForAppMon)
{
	auto configuration = AppMonOpenKitBuilder(defaultEndpointURL, defaultApplicationID, defaultDeviceID)
		.withManufacturer(testManufacturer)
		.buildConfiguration();

	ASSERT_EQ(configuration->getDevice()->getManufacturer(), testManufacturer);
}

TEST_F(OpenKitBuilderTest, canSetManufactureForDynatrace)
{
	auto configuration = DynatraceOpenKitBuilder(defaultEndpointURL, defaultApplicationID, defaultDeviceID)
		.withManufacturer(testManufacturer)
		.buildConfiguration();

	ASSERT_EQ(configuration->getDevice()->getManufacturer(), testManufacturer);
}

TEST_F(OpenKitBuilderTest, canSetModelIDForAppMon)
{
	auto configuration = AppMonOpenKitBuilder(defaultEndpointURL, defaultApplicationID, defaultDeviceID)
		.withModelID(testModelID)
		.buildConfiguration();

	ASSERT_EQ(configuration->getDevice()->getModelID(), testModelID);
}

TEST_F(OpenKitBuilderTest, canSetModelIDForDynatrace)
{
	auto configuration = DynatraceOpenKitBuilder(defaultEndpointURL, defaultApplicationID, defaultDeviceID)
		.withModelID(testModelID)
		.buildConfiguration();

	ASSERT_EQ(configuration->getDevice()->getModelID(), testModelID);
}

TEST_F(OpenKitBuilderTest, defaultLoggerIsUsedByDefault)
{
	auto logger = DynatraceOpenKitBuilder(defaultEndpointURL, defaultApplicationID, defaultDeviceID)
		.enableVerbose()
		.getLogger();

	auto typeCastLogger = std::dynamic_pointer_cast<core::util::DefaultLogger>(logger);

	ASSERT_TRUE(typeCastLogger != nullptr);
}

TEST_F(OpenKitBuilderTest, verboseIsUsedInDefaultLogger)
{
	auto logger = DynatraceOpenKitBuilder(defaultEndpointURL, defaultApplicationID, defaultDeviceID)
		.enableVerbose()
		.getLogger();

	auto typeCastLogger = std::dynamic_pointer_cast<core::util::DefaultLogger>(logger);

	ASSERT_TRUE(typeCastLogger != nullptr);
	ASSERT_TRUE(typeCastLogger->isDebugEnabled());
	ASSERT_TRUE(typeCastLogger->isInfoEnabled());
}

TEST_F(OpenKitBuilderTest, canSetCustomMaxBeaconRecordAgeForAppMon)
{
	auto configuration = AppMonOpenKitBuilder(defaultEndpointURL, defaultApplicationID, defaultDeviceID)
		.withBeaconCacheMaxRecordAge(testCacheMaxRecordAge)
		.buildConfiguration();

	ASSERT_EQ(configuration->getBeaconCacheConfiguration()->getMaxRecordAge(), testCacheMaxRecordAge);
}

TEST_F(OpenKitBuilderTest, canSetCustomMaxBeaconRecordAgeForDynatrace)
{
	auto configuration = DynatraceOpenKitBuilder(defaultEndpointURL, defaultApplicationID, defaultDeviceID)
		.withBeaconCacheMaxRecordAge(testCacheMaxRecordAge)
		.buildConfiguration();

	ASSERT_EQ(configuration->getBeaconCacheConfiguration()->getMaxRecordAge(), testCacheMaxRecordAge);
}


TEST_F(OpenKitBuilderTest, canSetBeaconCacheLowerMemoryBoundaryForAppMon)
{
	auto configuration = AppMonOpenKitBuilder(defaultEndpointURL, defaultApplicationID, defaultDeviceID)
		.withBeaconCacheLowerMemoryBoundary(testCacheLowerMemoryBoundary)
		.buildConfiguration();

	ASSERT_EQ(configuration->getBeaconCacheConfiguration()->getCacheSizeLowerBound(), testCacheLowerMemoryBoundary);
}

TEST_F(OpenKitBuilderTest, canSetBeaconCacheLowerMemoryBoundaryForDynatrace)
{
	auto configuration = DynatraceOpenKitBuilder(defaultEndpointURL, defaultApplicationID, defaultDeviceID)
		.withBeaconCacheLowerMemoryBoundary(testCacheLowerMemoryBoundary)
		.buildConfiguration();

	ASSERT_EQ(configuration->getBeaconCacheConfiguration()->getCacheSizeLowerBound(), testCacheLowerMemoryBoundary);
}

TEST_F(OpenKitBuilderTest, canSetBeaconCacheUpperMemoryBoundaryForAppMon)
{
	auto configuration = AppMonOpenKitBuilder(defaultEndpointURL, defaultApplicationID, defaultDeviceID)
		.withBeaconCacheUpperMemoryBoundary(testCacheUpperMemoryBoundary)
		.buildConfiguration();

	ASSERT_EQ(configuration->getBeaconCacheConfiguration()->getCacheSizeUpperBound(), testCacheUpperMemoryBoundary);
}

TEST_F(OpenKitBuilderTest, canSetBeaconCacheUpperMemoryBoundaryForDynatrace)
{
	auto configuration = DynatraceOpenKitBuilder(defaultEndpointURL, defaultApplicationID, defaultDeviceID)
		.withBeaconCacheUpperMemoryBoundary(testCacheUpperMemoryBoundary)
		.buildConfiguration();

	ASSERT_EQ(configuration->getBeaconCacheConfiguration()->getCacheSizeUpperBound(), testCacheUpperMemoryBoundary);
}
