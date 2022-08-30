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

#include "mock/MockILogger.h"
#include "mock/MockISslTrustManager.h"
#include "mock/MockIHttpRequestInterceptor.h"
#include "mock/MockIHttpResponseInterceptor.h"
#include "../DefaultValues.h"

#include "OpenKit/DynatraceOpenKitBuilder.h"
#include "OpenKit/CrashReportingLevel.h"
#include "OpenKit/DataCollectionLevel.h"
#include "OpenKit/LogLevel.h"
#include "OpenKit/OpenKitConstants.h"
#include "core/configuration/ConfigurationDefaults.h"
#include "core/util/DefaultLogger.h"
#include "core/util/StringUtil.h"
#include "protocol/ssl/SSLStrictTrustManager.h"
#include "protocol/http/NullHttpRequestInterceptor.h"
#include "protocol/http/NullHttpResponseInterceptor.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace test;

using DynatraceOpenKitBuilder_t = openkit::DynatraceOpenKitBuilder;
using CrashReportingLevel_t = openkit::CrashReportingLevel;
using DataCollectionLevel_t = openkit::DataCollectionLevel;
using DefaultLogger_t = core::util::DefaultLogger;
using LogLevel_t = openkit::LogLevel;
using SSLStrictTrustManager_t = protocol::SSLStrictTrustManager;
using NullHttpRequestInterceptor_t = protocol::NullHttpRequestInterceptor;
using NullHttpResponseInterceptor_t = protocol::NullHttpResponseInterceptor;
using StringUtil_t = core::util::StringUtil;

constexpr char ENDPOINT_URL[] = "https://localhost";
constexpr char APPLICATION_ID[] = "the-application-identifier";
constexpr char APPLICATION_NAME[] = "the-application-name";
constexpr int64_t DEVICE_ID = 777;
constexpr char APPLICATION_VERSION[] = "application-version";
constexpr char OPERATING_SYSTEM[] = "ultimate-operating-system";
constexpr char MANUFACTURER[] = "ACME Inc.";
constexpr char MODEL_ID[] = "the-model-identifier";
constexpr int64_t MAX_RECORD_AGE_IN_MILLIS = 42000;
constexpr int64_t LOWER_MEMORY_BOUNDARY_IN_BYTES = 999;
constexpr int64_t UPPER_MEMORY_BOUNDARY_IN_BYTES = 9999;

class DynatraceOpenKitBuilderTest : public testing::Test
{
};

class StubOpenKitBuilder : public DynatraceOpenKitBuilder_t
{
public:
	StubOpenKitBuilder(const char* endpointUrl, const char* applicationId, int64_t deviceId)
		: DynatraceOpenKitBuilder_t(endpointUrl, applicationId, deviceId)
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

TEST_F(DynatraceOpenKitBuilderTest, constructorInitializesApplicationId)
{
	// given, when
	DynatraceOpenKitBuilder_t target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// then
	ASSERT_THAT(target.getApplicationID(), testing::Eq(APPLICATION_ID));
}

TEST_F(DynatraceOpenKitBuilderTest, getOpenKitTypeGivesAppropriateValue)
{
	// given, when
	DynatraceOpenKitBuilder_t target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// then
	ASSERT_THAT(target.getOpenKitType(), testing::Eq(DynatraceOpenKitBuilder_t::OPENKIT_TYPE));
}

TEST_F(DynatraceOpenKitBuilderTest, getDefaultServerIdGivesAppropriateValue)
{
	// given, when
	DynatraceOpenKitBuilder_t target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// then
	ASSERT_THAT(target.getDefaultServerID(), testing::Eq(DynatraceOpenKitBuilder_t::DEFAULT_SERVER_ID));
}

TEST_F(DynatraceOpenKitBuilderTest, constructorInitializesEndpointUrl)
{
	// given, when
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// then
	ASSERT_THAT(target.getEndpointURL(), testing::Eq(ENDPOINT_URL));
}

TEST_F(DynatraceOpenKitBuilderTest, constructorInitializesDeviceId)
{
	// given, when
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// then
	ASSERT_THAT(target.getDeviceID(), testing::Eq(DEVICE_ID));
}

TEST_F(DynatraceOpenKitBuilderTest, constructorIntitializesNumericDeviceIdString)
{
	//given
	const int64_t deviceId = 42;
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, deviceId);

	// when, then
	ASSERT_THAT(target.getDeviceID(), testing::Eq(deviceId));
	ASSERT_THAT(target.getOrigDeviceID(), testing::Eq(std::to_string(deviceId)));
}

TEST_F(DynatraceOpenKitBuilderTest, getLoggerGivesADefaultImplementationIfNoneHasBeenProvided)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	auto obtained = target.getLogger();

	// then
	ASSERT_THAT(obtained, testing::NotNull());

	ASSERT_THAT(std::dynamic_pointer_cast<DefaultLogger_t>(obtained), testing::NotNull());
}

TEST_F(DynatraceOpenKitBuilderTest, defaultLoggerHasErrorAndWarningLevelEnabled)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	auto obtained = target.getLogger();

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->isErrorEnabled(), testing::Eq(true));
	ASSERT_THAT(obtained->isWarningEnabled(), testing::Eq(true));
	ASSERT_THAT(obtained->isInfoEnabled(), testing::Eq(false));
	ASSERT_THAT(obtained->isDebugEnabled(), testing::Eq(false));
}

TEST_F(DynatraceOpenKitBuilderTest, withLogLevelAppliesLogLevelToDefaultLogger)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

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

TEST_F(DynatraceOpenKitBuilderTest, getLoggerReturnsPreviouslySetLogger)
{
	// given
	auto logger = MockILogger::createNice();
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	target.withLogger(logger);
	auto obtained = target.getLogger();

	// then
	ASSERT_THAT(obtained, testing::Eq(logger));
}

TEST_F(DynatraceOpenKitBuilderTest, getApplicationVersionUsesDefaultIfNotSet)
{
	// given, when
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// then
	ASSERT_THAT(target.getApplicationVersion(), testing::Eq(openkit::DEFAULT_APPLICATION_VERSION));
}

TEST_F(DynatraceOpenKitBuilderTest, applicationVersionCanBeChanged)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	target.withApplicationVersion(APPLICATION_VERSION);

	// then
	ASSERT_THAT(target.getApplicationVersion(), testing::Eq(APPLICATION_VERSION));
}

TEST_F(DynatraceOpenKitBuilderTest, applicationVersionCannotBeChangedToNull)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	target.withApplicationVersion(nullptr);

	// then
	ASSERT_THAT(target.getApplicationVersion(), testing::Eq(openkit::DEFAULT_APPLICATION_VERSION));
}

TEST_F(DynatraceOpenKitBuilderTest, applicationVersionCannotBeChangedToEmptyString)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	target.withApplicationVersion("");

	// then
	ASSERT_THAT(target.getApplicationVersion(), testing::Eq(openkit::DEFAULT_APPLICATION_VERSION));
}

TEST_F(DynatraceOpenKitBuilderTest, getTrustManagerGivesStrictTrustManagerByDefault)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	auto obtained = target.getTrustManager();

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(std::dynamic_pointer_cast<SSLStrictTrustManager_t>(obtained), testing::NotNull());
}

TEST_F(DynatraceOpenKitBuilderTest, getTrustManagerGivesPreviouslySetTrustManager)
{
	// given
	auto trustManager = MockISslTrustManager::createNice();
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	target.withTrustManager(trustManager);
	auto obtained = target.getTrustManager();

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained, testing::Eq(trustManager));
}

TEST_F(DynatraceOpenKitBuilderTest, trustManagerCannotBeChangedToNull)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	target.withTrustManager(nullptr);
	auto obtained = target.getTrustManager();

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(std::dynamic_pointer_cast<SSLStrictTrustManager_t>(obtained), testing::NotNull());
}

TEST_F(DynatraceOpenKitBuilderTest, getOperatingSystemReturnsADefaultValue)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	auto obtained = target.getOperatingSystem();

	// then
	ASSERT_THAT(obtained, testing::Eq(openkit::DEFAULT_OPERATING_SYSTEM));
}

TEST_F(DynatraceOpenKitBuilderTest, getOperatingSystemGivesChangedOperatingSystem)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	target.withOperatingSystem(OPERATING_SYSTEM);
	auto obtained = target.getOperatingSystem();

	// then
	ASSERT_THAT(obtained, testing::Eq(OPERATING_SYSTEM));
}

TEST_F(DynatraceOpenKitBuilderTest, operatingSystemCannotBeChangedToNull)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	target.withOperatingSystem(nullptr);
	auto obtained = target.getOperatingSystem();

	// then
	ASSERT_THAT(obtained, testing::Eq(openkit::DEFAULT_OPERATING_SYSTEM));
}

TEST_F(DynatraceOpenKitBuilderTest, operatingSystemCannotBeChangedToEmptyString)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	target.withOperatingSystem("");
	auto obtained = target.getOperatingSystem();

	// then
	ASSERT_THAT(obtained, testing::Eq(openkit::DEFAULT_OPERATING_SYSTEM));
}

TEST_F(DynatraceOpenKitBuilderTest, getManufacturerReturnsADefaultValue)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	auto obtained = target.getManufacturer();

	// then
	ASSERT_THAT(obtained, testing::Eq(openkit::DEFAULT_MANUFACTURER));
}

TEST_F(DynatraceOpenKitBuilderTest, getManufacturerGivesChangedManufacturer)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	target.withManufacturer(MANUFACTURER);
	auto obtained = target.getManufacturer();

	// then
	ASSERT_THAT(obtained, testing::Eq(MANUFACTURER));
}

TEST_F(DynatraceOpenKitBuilderTest, manufacturerCannotBeChangedToNull)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	target.withManufacturer(nullptr);
	auto obtained = target.getManufacturer();

	// then
	ASSERT_THAT(obtained, testing::Eq(openkit::DEFAULT_MANUFACTURER));
}

TEST_F(DynatraceOpenKitBuilderTest, manufacturerCannotBeChangedToEmptyString)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	target.withManufacturer("");
	auto obtained = target.getManufacturer();

	// then
	ASSERT_THAT(obtained, testing::Eq(openkit::DEFAULT_MANUFACTURER));
}

TEST_F(DynatraceOpenKitBuilderTest, getModelIdReturnsADefaultValue)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	auto obtained = target.getModelID();

	// then
	ASSERT_THAT(obtained, testing::Eq(openkit::DEFAULT_MODEL_ID));
}

TEST_F(DynatraceOpenKitBuilderTest, getModelIdGivesChangedModelId)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	target.withModelID(MODEL_ID);
	auto obtained = target.getModelID();

	// then
	ASSERT_THAT(obtained, testing::Eq(MODEL_ID));
}

TEST_F(DynatraceOpenKitBuilderTest, modelIdCannotBeChangedToNull)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	target.withModelID(nullptr);
	auto obtained = target.getModelID();

	// then
	ASSERT_THAT(obtained, testing::Eq(openkit::DEFAULT_MODEL_ID));
}

TEST_F(DynatraceOpenKitBuilderTest, modelIdCannotBeChangedToEmptyString)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	target.withModelID("");
	auto obtained = target.getModelID();

	// then
	ASSERT_THAT(obtained, testing::Eq(openkit::DEFAULT_MODEL_ID));
}

TEST_F(DynatraceOpenKitBuilderTest, getBeaconCacheMaxRecordAgeReturnsADefaultValue)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	auto obtained = target.getBeaconCacheMaxRecordAge();

	// then
	ASSERT_THAT(obtained, testing::Eq(core::configuration::DEFAULT_MAX_RECORD_AGE_IN_MILLIS));
}

TEST_F(DynatraceOpenKitBuilderTest, getBeaconCacheMaxRecordAgeGivesChangedValue)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	target.withBeaconCacheMaxRecordAge(MAX_RECORD_AGE_IN_MILLIS);
	auto obtained = target.getBeaconCacheMaxRecordAge();

	// then
	ASSERT_THAT(obtained, testing::Eq(MAX_RECORD_AGE_IN_MILLIS));
}

TEST_F(DynatraceOpenKitBuilderTest, getBeaconCacheLowerMemoryBoundaryReturnsADefaultValue)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	auto obtained = target.getBeaconCacheLowerMemoryBoundary();

	// then
	ASSERT_THAT(obtained, testing::Eq(core::configuration::DEFAULT_LOWER_MEMORY_BOUNDARY_IN_BYTES));
}

TEST_F(DynatraceOpenKitBuilderTest, getBeaconCacheLowerMemoryBoundaryGivesChangedValue)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	target.withBeaconCacheLowerMemoryBoundary(LOWER_MEMORY_BOUNDARY_IN_BYTES);
	auto obtained = target.getBeaconCacheLowerMemoryBoundary();

	// then
	ASSERT_THAT(obtained, testing::Eq(LOWER_MEMORY_BOUNDARY_IN_BYTES));
}

TEST_F(DynatraceOpenKitBuilderTest, getBeaconCacheUpperMemoryBoundaryReturnsADefaultValue)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	auto obtained = target.getBeaconCacheUpperMemoryBoundary();

	// then
	ASSERT_THAT(obtained, testing::Eq(core::configuration::DEFAULT_UPPER_MEMORY_BOUNDARY_IN_BYTES));
}

TEST_F(DynatraceOpenKitBuilderTest, getBeaconCacheUpperMemoryBoundaryGivesChangedValue)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	target.withBeaconCacheUpperMemoryBoundary(UPPER_MEMORY_BOUNDARY_IN_BYTES);
	auto obtained = target.getBeaconCacheUpperMemoryBoundary();

	// then
	ASSERT_THAT(obtained, testing::Eq(UPPER_MEMORY_BOUNDARY_IN_BYTES));
}

TEST_F(DynatraceOpenKitBuilderTest, defaultDatacollectionLevelIsUserBehavior)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	auto obtained = target.getDataCollectionLevel();

	// then
	ASSERT_THAT(obtained, testing::Eq(DataCollectionLevel_t::USER_BEHAVIOR));
}

TEST_F(DynatraceOpenKitBuilderTest, getDataCollectionLevelReturnsChangedDatacCollectionLevel)
{
	// given
	const DataCollectionLevel_t dataCollectionLevel = DataCollectionLevel_t::PERFORMANCE;
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	target.withDataCollectionLevel(dataCollectionLevel);
	auto obtained = target.getDataCollectionLevel();

	// then
	ASSERT_THAT(obtained, testing::Eq(dataCollectionLevel));
}

TEST_F(DynatraceOpenKitBuilderTest, defaultCrashReportingLevelIsOptInCrashes)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	auto obtained = target.getCrashReportingLevel();

	// then
	ASSERT_THAT(obtained, testing::Eq(CrashReportingLevel_t::OPT_IN_CRASHES));
}

TEST_F(DynatraceOpenKitBuilderTest, getCrashReportingLevelReturnsChangedCrashReportingLevel)
{
	// given
	const CrashReportingLevel_t crashReportingLevel = CrashReportingLevel_t::OPT_OUT_CRASHES;
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	target.withCrashReportingLevel(crashReportingLevel);
	auto obtained = target.getCrashReportingLevel();

	// then
	ASSERT_THAT(obtained, testing::Eq(crashReportingLevel));
}

TEST_F(DynatraceOpenKitBuilderTest, getHttpRequestInterceptorGivesNullHttpRequestInterceptorByDefault)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	auto obtained = target.getHttpRequestInterceptor();

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(std::dynamic_pointer_cast<NullHttpRequestInterceptor_t>(obtained), testing::NotNull());
}

TEST_F(DynatraceOpenKitBuilderTest, getHttpRequestInterceptorGivesPreviouslySetHttpRequestInterceptor)
{
	// given
	auto httpRequestInterceptor = MockIHttpRequestInterceptor::createNice();
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	target.withHttpRequestInterceptor(httpRequestInterceptor);
	auto obtained = target.getHttpRequestInterceptor();

	// then
	ASSERT_THAT(obtained, testing::Eq(httpRequestInterceptor));
}

TEST_F(DynatraceOpenKitBuilderTest, getHttpRequestInterceptorCannotBeChangedToNullptr)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	target.withHttpRequestInterceptor(nullptr);
	auto obtained = target.getHttpRequestInterceptor();

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(std::dynamic_pointer_cast<NullHttpRequestInterceptor_t>(obtained), testing::NotNull());
}

TEST_F(DynatraceOpenKitBuilderTest, getHttpResponseInterceptorGivesNullHttpResponseInterceptorByDefault)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	auto obtained = target.getHttpResponseInterceptor();

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(std::dynamic_pointer_cast<NullHttpResponseInterceptor_t>(obtained), testing::NotNull());
}

TEST_F(DynatraceOpenKitBuilderTest, getHttpResponseInterceptorGivesPreviouslySetHttpResponseInterceptor)
{
	// given
	auto httpResponseInterceptor = MockIHttpResponseInterceptor::createNice();
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	target.withHttpResponseInterceptor(httpResponseInterceptor);
	auto obtained = target.getHttpResponseInterceptor();

	// then
	ASSERT_THAT(obtained, testing::Eq(httpResponseInterceptor));
}

TEST_F(DynatraceOpenKitBuilderTest, getHttpResponseInterceptorCannotBeChangedToNullptr)
{
	// given
	StubOpenKitBuilder target(ENDPOINT_URL, APPLICATION_ID, DEVICE_ID);

	// when
	target.withHttpResponseInterceptor(nullptr);
	auto obtained = target.getHttpResponseInterceptor();

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(std::dynamic_pointer_cast<NullHttpResponseInterceptor_t>(obtained), testing::NotNull());
}