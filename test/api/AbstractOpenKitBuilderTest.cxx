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

#include "mock/MockILogger.h"
#include "mock/MockISslTrustManager.h"
#include "../DefaultValues.h"

#include "OpenKit/AbstractOpenKitBuilder.h"
#include "OpenKit/CrashReportingLevel.h"
#include "OpenKit/DataCollectionLevel.h"
#include "OpenKit/LogLevel.h"
#include "OpenKit/OpenKitConstants.h"
#include "core/configuration/ConfigurationDefaults.h"
#include "core/util/DefaultLogger.h"
#include "core/util/StringUtil.h"
#include "protocol/ssl/SSLStrictTrustManager.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace test;

using AbstractOpenKitBuilder_t = openkit::AbstractOpenKitBuilder;
using CrashReportingLevel_t = openkit::CrashReportingLevel;
using DataCollectionLevel_t = openkit::DataCollectionLevel;
using DefaultLogger_t = core::util::DefaultLogger;
using LogLevel_t = openkit::LogLevel;
using SSLStrictTrustManager_t = protocol::SSLStrictTrustManager;
using StringUtil_t = core::util::StringUtil;

constexpr char ENDPOINT_URL[] = "https://localhost:9999/1";
constexpr int64_t DEVICE_ID = 777;
constexpr char APPLICATION_VERSION[] = "application-version";
constexpr char OPERATING_SYSTEM[] = "ultimate-operating-system";
constexpr char MANUFACTURER[] = "ACME Inc.";
constexpr char MODEL_ID[] = "the-model-identifier";
constexpr int64_t MAX_RECORD_AGE_IN_MILLIS = 42000;
constexpr int64_t LOWER_MEMORY_BOUNDARY_IN_BYTES = 999;
constexpr int64_t UPPER_MEMORY_BOUNDARY_IN_BYTES = 9999;

class AbstractOpenKitBuilderTest : public testing::Test
{
};

class StubOpenKitBuilder : public AbstractOpenKitBuilder_t
{
public:
	StubOpenKitBuilder(const char* endpointUrl, int64_t deviceId)
		: AbstractOpenKitBuilder_t(endpointUrl, deviceId)
	{
	}

	StubOpenKitBuilder(const char* endpointUrl, const char* deviceId)
		: AbstractOpenKitBuilder_t(endpointUrl, deviceId)
	{
	}

	const std::string& getOpenKitType() const
	{
		return  DefaultValues::EMPTY_STRING;
	}

	const std::string& getApplicationID() const
	{
		return DefaultValues::EMPTY_STRING;
	}

	const std::string& getApplicationName() const
	{
		return DefaultValues::EMPTY_STRING;
	}

	int32_t getDefaultServerID() const
	{
		return 0;
	}
};

TEST_F(AbstractOpenKitBuilderTest, constructorInitializesEndpointUrl)
{
	// given, when
	StubOpenKitBuilder target(ENDPOINT_URL, DEVICE_ID);

	// then
	ASSERT_THAT(target.getEndpointURL(), testing::Eq(ENDPOINT_URL));
}

TEST_F(AbstractOpenKitBuilderTest, constructorInitializesDeviceId)
{
	// given, when
	StubOpenKitBuilder target(ENDPOINT_URL, DEVICE_ID);

	// then
	ASSERT_THAT(target.getDeviceID(), testing::Eq(DEVICE_ID));
}

TEST_F(AbstractOpenKitBuilderTest, constructorIntializesAndHashesStringDeviceId)
{
	// given
	const std::string deviceIdAsString = "stringDeviceID";
	StubOpenKitBuilder target(ENDPOINT_URL, deviceIdAsString.c_str());

	// when,then
	auto hashedDeviceId = StringUtil_t::to64BitHash(deviceIdAsString);
	ASSERT_THAT(target.getDeviceID(), testing::Eq(hashedDeviceId));
	ASSERT_THAT(target.getOrigDeviceID(), testing::Eq(deviceIdAsString));
}

TEST_F(AbstractOpenKitBuilderTest, constructorIntitializesNumericDeviceIdString)
{
	//given
	const int64_t deviceId = 42;
	StubOpenKitBuilder target(ENDPOINT_URL, deviceId);

	// when, then
	ASSERT_THAT(target.getDeviceID(), testing::Eq(deviceId));
	ASSERT_THAT(target.getOrigDeviceID(), testing::Eq(std::to_string(deviceId)));
}

TEST_F(AbstractOpenKitBuilderTest, constructorTrimsDeviceIdString)
{
	// given
	const std::string deviceIdAsString = " 42 ";
	StubOpenKitBuilder target(ENDPOINT_URL, deviceIdAsString.c_str());

	// when, then
	ASSERT_THAT(target.getDeviceID(), testing::Eq(42));
	ASSERT_THAT(target.getOrigDeviceID(), testing::Eq(deviceIdAsString));
}

TEST_F(AbstractOpenKitBuilderTest, getLoggerGivesADefaultImplementationIfNoneHasBeenProvided)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, DEVICE_ID);

	// when
	auto obtained = target.getLogger();

	// then
	ASSERT_THAT(obtained, testing::NotNull());

	ASSERT_THAT(std::dynamic_pointer_cast<DefaultLogger_t>(obtained), testing::NotNull());
}

TEST_F(AbstractOpenKitBuilderTest, defaultLoggerHasErrorAndWarningLevelEnabled)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, DEVICE_ID);

	// when
	auto obtained = target.getLogger();

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->isErrorEnabled(), testing::Eq(true));
	ASSERT_THAT(obtained->isWarningEnabled(), testing::Eq(true));
	ASSERT_THAT(obtained->isInfoEnabled(), testing::Eq(false));
	ASSERT_THAT(obtained->isDebugEnabled(), testing::Eq(false));
}

TEST_F(AbstractOpenKitBuilderTest, whenEnabelingVerboseAllLogLevelsAreEnabledInDefaultLogger)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, DEVICE_ID);

	// when
	target.enableVerbose();
	auto obtained = target.getLogger();

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->isErrorEnabled(), testing::Eq(true));
	ASSERT_THAT(obtained->isWarningEnabled(), testing::Eq(true));
	ASSERT_THAT(obtained->isInfoEnabled(), testing::Eq(true));
	ASSERT_THAT(obtained->isDebugEnabled(), testing::Eq(true));
}

TEST_F(AbstractOpenKitBuilderTest, withLogLevelAppliesLogLevelToDefaultLogger)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, DEVICE_ID);

	// when
	target.withLogLevel(LogLevel_t::LOG_LEVEL_INFO);
	auto obtained = target.getLogger();

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->isErrorEnabled(), testing::Eq(true));
	ASSERT_THAT(obtained->isWarningEnabled(), testing::Eq(true));
	ASSERT_THAT(obtained->isInfoEnabled(), testing::Eq(true));
	ASSERT_THAT(obtained->isDebugEnabled(), testing::Eq(false));
}

TEST_F(AbstractOpenKitBuilderTest, getLoggerReturnsPreviouslySetLogger)
{
	// given
	auto logger = MockILogger::createNice();
	StubOpenKitBuilder target(ENDPOINT_URL, DEVICE_ID);

	// when
	target.withLogger(logger);
	auto obtained = target.getLogger();

	// then
	ASSERT_THAT(obtained, testing::Eq(logger));
}

TEST_F(AbstractOpenKitBuilderTest, getApplicationVersionUsesDefaultIfNotSet)
{
	// given, when
	StubOpenKitBuilder target(ENDPOINT_URL, DEVICE_ID);

	// then
	ASSERT_THAT(target.getApplicationVersion(), testing::Eq(openkit::DEFAULT_APPLICATION_VERSION));
}

TEST_F(AbstractOpenKitBuilderTest, applicationVersionCanBeChanged)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, DEVICE_ID);

	// when
	target.withApplicationVersion(APPLICATION_VERSION);

	// then
	ASSERT_THAT(target.getApplicationVersion(), testing::Eq(APPLICATION_VERSION));
}

TEST_F(AbstractOpenKitBuilderTest, applicationVersionCannotBeChangedToNull)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, DEVICE_ID);

	// when
	target.withApplicationVersion(nullptr);

	// then
	ASSERT_THAT(target.getApplicationVersion(), testing::Eq(openkit::DEFAULT_APPLICATION_VERSION));
}

TEST_F(AbstractOpenKitBuilderTest, applicationVersionCannotBeChangedToEmptyString)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, DEVICE_ID);

	// when
	target.withApplicationVersion("");

	// then
	ASSERT_THAT(target.getApplicationVersion(), testing::Eq(openkit::DEFAULT_APPLICATION_VERSION));
}

TEST_F(AbstractOpenKitBuilderTest, getTrustManagerGivesStrictTrustManagerByDefault)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, DEVICE_ID);

	// when
	auto obtained = target.getTrustManager();

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(std::dynamic_pointer_cast<SSLStrictTrustManager_t>(obtained), testing::NotNull());
}

TEST_F(AbstractOpenKitBuilderTest, getTrustManagerGivesPreviouslySetTrustManager)
{
	// given
	auto trustManager = MockISslTrustManager::createNice();
	StubOpenKitBuilder target(ENDPOINT_URL, DEVICE_ID);

	// when
	target.withTrustManager(trustManager);
	auto obtained = target.getTrustManager();

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained, testing::Eq(trustManager));
}

TEST_F(AbstractOpenKitBuilderTest, trustManagerCannotBeChangedToNull)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, DEVICE_ID);

	// when
	target.withTrustManager(nullptr);
	auto obtained = target.getTrustManager();

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(std::dynamic_pointer_cast<SSLStrictTrustManager_t>(obtained), testing::NotNull());
}

TEST_F(AbstractOpenKitBuilderTest, getOperatingSystemReturnsADefaultValue)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, DEVICE_ID);

	// when
	auto obtained = target.getOperatingSystem();

	// then
	ASSERT_THAT(obtained, testing::Eq(openkit::DEFAULT_OPERATING_SYSTEM));
}

TEST_F(AbstractOpenKitBuilderTest, getOperatingSystemGivesChangedOperatingSystem)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, DEVICE_ID);

	// when
	target.withOperatingSystem(OPERATING_SYSTEM);
	auto obtained = target.getOperatingSystem();

	// then
	ASSERT_THAT(obtained, testing::Eq(OPERATING_SYSTEM));
}

TEST_F(AbstractOpenKitBuilderTest, operatingSystemCannotBeChangedToNull)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, DEVICE_ID);

	// when
	target.withOperatingSystem(nullptr);
	auto obtained = target.getOperatingSystem();

	// then
	ASSERT_THAT(obtained, testing::Eq(openkit::DEFAULT_OPERATING_SYSTEM));
}

TEST_F(AbstractOpenKitBuilderTest, operatingSystemCannotBeChangedToEmptyString)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, DEVICE_ID);

	// when
	target.withOperatingSystem("");
	auto obtained = target.getOperatingSystem();

	// then
	ASSERT_THAT(obtained, testing::Eq(openkit::DEFAULT_OPERATING_SYSTEM));
}

TEST_F(AbstractOpenKitBuilderTest, getManufacturerReturnsADefaultValue)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, DEVICE_ID);

	// when
	auto obtained = target.getManufacturer();

	// then
	ASSERT_THAT(obtained, testing::Eq(openkit::DEFAULT_MANUFACTURER));
}

TEST_F(AbstractOpenKitBuilderTest, getManufacturerGivesChangedManufacturer)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, DEVICE_ID);

	// when
	target.withManufacturer(MANUFACTURER);
	auto obtained = target.getManufacturer();

	// then
	ASSERT_THAT(obtained, testing::Eq(MANUFACTURER));
}

TEST_F(AbstractOpenKitBuilderTest, manufacturerCannotBeChangedToNull)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, DEVICE_ID);

	// when
	target.withManufacturer(nullptr);
	auto obtained = target.getManufacturer();

	// then
	ASSERT_THAT(obtained, testing::Eq(openkit::DEFAULT_MANUFACTURER));
}

TEST_F(AbstractOpenKitBuilderTest, manufacturerCannotBeChangedToEmptyString)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, DEVICE_ID);

	// when
	target.withManufacturer("");
	auto obtained = target.getManufacturer();

	// then
	ASSERT_THAT(obtained, testing::Eq(openkit::DEFAULT_MANUFACTURER));
}

TEST_F(AbstractOpenKitBuilderTest, getModelIdReturnsADefaultValue)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, DEVICE_ID);

	// when
	auto obtained = target.getModelID();

	// then
	ASSERT_THAT(obtained, testing::Eq(openkit::DEFAULT_MODEL_ID));
}

TEST_F(AbstractOpenKitBuilderTest, getModelIdGivesChangedModelId)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, DEVICE_ID);

	// when
	target.withModelID(MODEL_ID);
	auto obtained = target.getModelID();

	// then
	ASSERT_THAT(obtained, testing::Eq(MODEL_ID));
}

TEST_F(AbstractOpenKitBuilderTest, modelIdCannotBeChangedToNull)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, DEVICE_ID);

	// when
	target.withModelID(nullptr);
	auto obtained = target.getModelID();

	// then
	ASSERT_THAT(obtained, testing::Eq(openkit::DEFAULT_MODEL_ID));
}

TEST_F(AbstractOpenKitBuilderTest, modelIdCannotBeChangedToEmptyString)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, DEVICE_ID);

	// when
	target.withModelID("");
	auto obtained = target.getModelID();

	// then
	ASSERT_THAT(obtained, testing::Eq(openkit::DEFAULT_MODEL_ID));
}

TEST_F(AbstractOpenKitBuilderTest, getBeaconCacheMaxRecordAgeReturnsADefaultValue)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, DEVICE_ID);

	// when
	auto obtained = target.getBeaconCacheMaxRecordAge();

	// then
	ASSERT_THAT(obtained, testing::Eq(core::configuration::DEFAULT_MAX_RECORD_AGE_IN_MILLIS));
}

TEST_F(AbstractOpenKitBuilderTest, getBeaconCacheMaxRecordAgeGivesChangedValue)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, DEVICE_ID);

	// when
	target.withBeaconCacheMaxRecordAge(MAX_RECORD_AGE_IN_MILLIS);
	auto obtained = target.getBeaconCacheMaxRecordAge();

	// then
	ASSERT_THAT(obtained, testing::Eq(MAX_RECORD_AGE_IN_MILLIS));
}

TEST_F(AbstractOpenKitBuilderTest, getBeaconCacheLowerMemoryBoundaryReturnsADefaultValue)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, DEVICE_ID);

	// when
	auto obtained = target.getBeaconCacheLowerMemoryBoundary();

	// then
	ASSERT_THAT(obtained, testing::Eq(core::configuration::DEFAULT_LOWER_MEMORY_BOUNDARY_IN_BYTES));
}

TEST_F(AbstractOpenKitBuilderTest, getBeaconCacheLowerMemoryBoundaryGivesChangedValue)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, DEVICE_ID);

	// when
	target.withBeaconCacheLowerMemoryBoundary(LOWER_MEMORY_BOUNDARY_IN_BYTES);
	auto obtained = target.getBeaconCacheLowerMemoryBoundary();

	// then
	ASSERT_THAT(obtained, testing::Eq(LOWER_MEMORY_BOUNDARY_IN_BYTES));
}

TEST_F(AbstractOpenKitBuilderTest, getBeaconCacheUpperMemoryBoundaryReturnsADefaultValue)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, DEVICE_ID);

	// when
	auto obtained = target.getBeaconCacheUpperMemoryBoundary();

	// then
	ASSERT_THAT(obtained, testing::Eq(core::configuration::DEFAULT_UPPER_MEMORY_BOUNDARY_IN_BYTES));
}

TEST_F(AbstractOpenKitBuilderTest, getBeaconCacheUpperMemoryBoundaryGivesChangedValue)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, DEVICE_ID);

	// when
	target.withBeaconCacheUpperMemoryBoundary(UPPER_MEMORY_BOUNDARY_IN_BYTES);
	auto obtained = target.getBeaconCacheUpperMemoryBoundary();

	// then
	ASSERT_THAT(obtained, testing::Eq(UPPER_MEMORY_BOUNDARY_IN_BYTES));
}

TEST_F(AbstractOpenKitBuilderTest, defaultDatacollectionLevelIsUserBehavior)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, DEVICE_ID);

	// when
	auto obtained = target.getDataCollectionLevel();

	// then
	ASSERT_THAT(obtained, testing::Eq(DataCollectionLevel_t::USER_BEHAVIOR));
}

TEST_F(AbstractOpenKitBuilderTest, getDataCollectionLevelReturnsChangedDatacCollectionLevel)
{
	// given
	const DataCollectionLevel_t dataCollectionLevel = DataCollectionLevel_t::PERFORMANCE;
	StubOpenKitBuilder target(ENDPOINT_URL, DEVICE_ID);

	// when
	target.withDataCollectionLevel(dataCollectionLevel);
	auto obtained = target.getDataCollectionLevel();

	// then
	ASSERT_THAT(obtained, testing::Eq(dataCollectionLevel));
}

TEST_F(AbstractOpenKitBuilderTest, defaultCrashReportingLevelIsOptInCrashes)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, DEVICE_ID);

	// when
	auto obtained = target.getCrashReportingLevel();

	// then
	ASSERT_THAT(obtained, testing::Eq(CrashReportingLevel_t::OPT_IN_CRASHES));
}

TEST_F(AbstractOpenKitBuilderTest, getCrashReportingLevelReturnsChangedCrashReportingLevel)
{
	// given
	const CrashReportingLevel_t crashReportingLevel = CrashReportingLevel_t::OPT_OUT_CRASHES;
	StubOpenKitBuilder target(ENDPOINT_URL, DEVICE_ID);

	// when
	target.withCrashReportingLevel(crashReportingLevel);
	auto obtained = target.getCrashReportingLevel();

	// then
	ASSERT_THAT(obtained, testing::Eq(crashReportingLevel));
}

