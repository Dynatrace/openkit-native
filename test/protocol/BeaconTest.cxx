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

#include "mock/MockIHTTPClient.h"
#include "../api/MockILogger.h"
#include "../core/caching/mock/MockIBeaconCache.h"

#include "core/caching/BeaconCache.h"
#include "core/caching/IBeaconCache.h"

#include "Types.h"
#include "../api/Types.h"
#include "../core/objects/Types.h"
#include "../core/objects/MockTypes.h"
#include "../core/util/Types.h"
#include "../providers/Types.h"
#include "../providers/MockTypes.h"

using namespace test;
using namespace test::types;

using BeaconCache_t = core::caching::BeaconCache;
using IBeaconCache_sp = std::shared_ptr<core::caching::IBeaconCache>;
using MockNiceIHTTPClient_sp = std::shared_ptr<testing::NiceMock<MockIHTTPClient>>;
using MockNiceILogger_sp = std::shared_ptr<testing::NiceMock<MockILogger>>;

static const char APP_ID[] = "appID";
static const char APP_NAME[] = "appName";
static const int64_t DEVICE_ID = 42;

class BeaconTest : public testing::Test
{
protected:
	void SetUp()
	{
		logger = MockILogger::createNice();
		threadIDProvider = std::make_shared<DefaultThreadIdProvider_t>();
		timingProvider = std::make_shared<DefaultTimingProvider_t>();

		trustManager = std::make_shared<SslStrictTrustManager_t>();

		device = std::make_shared<Device_t>(Utf8String_t(""), Utf8String_t(""), Utf8String_t(""));

		beaconCacheConfiguration = std::make_shared<BeaconCacheConfiguration_t>(-1, -1, -1);

		beaconCache = std::make_shared<BeaconCache_t>(logger);

		randomGeneratorMock = std::make_shared<MockNicePrnGenerator_t>();
		sessionIDProviderMock = std::make_shared<MockNiceSessionIdProvider_t>();
		mockTimingProvider = std::make_shared<MockNiceTimingProvider_t>();

		mockParent = std::make_shared<MockNiceOpenKitComposite_t>();
	}

	Beacon_sp buildBeaconWithDefaultConfig()
	{
		return buildBeacon(BeaconConfiguration_t::DEFAULT_DATA_COLLECTION_LEVEL, BeaconConfiguration_t::DEFAULT_CRASH_REPORTING_LEVEL);
	}

	Beacon_sp buildBeacon(DataCollectionLevel_t dl, CrashReportingLevel_t cl)
	{
		return buildBeacon(dl, cl, DEVICE_ID, Utf8String_t(APP_ID));
	}

	Beacon_sp buildBeacon(DataCollectionLevel_t dl, CrashReportingLevel_t cl, int64_t deviceID, const Utf8String_t& appID)
	{
		return buildBeacon(dl, cl, deviceID, appID, "127.0.0.1");
	}

	Beacon_sp buildBeacon(DataCollectionLevel_t dl, CrashReportingLevel_t cl, int64_t deviceID, const Utf8String_t& appID, const char* clientIPAddress)
	{
		auto beaconConfiguration = std::make_shared<BeaconConfiguration_t>(BeaconConfiguration_t::DEFAULT_MULTIPLICITY, dl, cl);

		configuration = std::make_shared<Configuration_t>(
			device,
			OpenKitType_t::Type::DYNATRACE,
			Utf8String_t(APP_NAME),
			"",
			appID,
			deviceID,
			std::to_string(deviceID).c_str(),
			"",
			sessionIDProviderMock,
			trustManager,
			beaconCacheConfiguration,
			beaconConfiguration
		);
		configuration->enableCapture();

		return std::make_shared<Beacon_t>(logger, beaconCache, configuration, clientIPAddress, threadIDProvider, mockTimingProvider, randomGeneratorMock);
	}

	MockNiceWebRequestTracer_sp createMockedWebRequestTracer(Beacon_sp beacon)
	{
		return std::make_shared<MockNiceWebRequestTracer_t>(logger, mockParent, beacon, "");
	}

	MockNiceSession_sp createMockedSession()
	{
		return std::make_shared<MockNiceSession_t>(logger);
	}

	MockNiceIActionCommon_sp createMockedAction()
	{
		return std::make_shared<MockNiceIActionCommon_t>();
	}

	MockNicePrnGenerator_sp getMockedRandomGenerator()
	{
		return randomGeneratorMock;
	}

	MockNiceSessionIdProvider_sp getSessionIDProviderMock()
	{
		return sessionIDProviderMock;
	}

	MockNiceTimingProvider_sp getTimingProviderMock()
	{
		return mockTimingProvider;
	}

	Configuration_sp getConfiguration()
	{
		return configuration;
	}

	ILogger_sp getLogger()
	{
		return logger;
	}

	void TearDown()
	{

	}

protected:
	ILogger_sp logger;
	IThreadIdProvider_sp threadIDProvider;
	ITimingProvider_sp timingProvider;

	ISslTrustManager_sp trustManager;

	Device_sp device;
	BeaconCacheConfiguration_sp beaconCacheConfiguration;
	IBeaconCache_sp beaconCache;

	MockNicePrnGenerator_sp randomGeneratorMock;
	MockNiceSessionIdProvider_sp sessionIDProviderMock;
	Configuration_sp configuration;
	MockNiceTimingProvider_sp mockTimingProvider;

	MockNiceOpenKitComposite_sp mockParent;
};

TEST_F(BeaconTest, noWebRequestIsReportedForDataCollectionLevel0)
{
	//given
	auto target = buildBeacon(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF);
	auto mockWebRequestTracer = std::make_shared<MockNiceWebRequestTracer_t>(getLogger(), mockParent, target, "");

	ON_CALL(*mockWebRequestTracer, getBytesSent())
		.WillByDefault(testing::Return(123));
	ON_CALL(*mockWebRequestTracer, getBytesReceived())
		.WillByDefault(testing::Return(45));
	ON_CALL(*mockWebRequestTracer, getResponseCode())
		.WillByDefault(testing::Return(400));

	// verify / then
	EXPECT_CALL(*mockWebRequestTracer, getBytesSent())
		.Times(0);

	EXPECT_CALL(*mockWebRequestTracer, getBytesReceived())
		.Times(0);

	EXPECT_CALL(*mockWebRequestTracer, getResponseCode())
		.Times(0);

	// when
	target->addWebRequest(123, mockWebRequestTracer);

	ASSERT_TRUE(target->isEmpty());
}

TEST_F(BeaconTest, webRequestIsReportedForDataCollectionLevel1)
{
	//given
	auto target = buildBeacon(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF);
	auto mockWebRequestTracer = createMockedWebRequestTracer(target);

	ON_CALL(*mockWebRequestTracer, getBytesSent())
		.WillByDefault(testing::Return(123));
	ON_CALL(*mockWebRequestTracer, getBytesReceived())
		.WillByDefault(testing::Return(45));
	ON_CALL(*mockWebRequestTracer, getResponseCode())
		.WillByDefault(testing::Return(400));

	// verify / then
	EXPECT_CALL(*mockWebRequestTracer, getBytesSent())
		.Times(1);

	EXPECT_CALL(*mockWebRequestTracer, getBytesReceived())
		.Times(1);

	EXPECT_CALL(*mockWebRequestTracer, getResponseCode())
		.Times(1);

	// when
	target->addWebRequest(123, mockWebRequestTracer);

	ASSERT_FALSE(target->isEmpty());
}

TEST_F(BeaconTest, webRequestIsReportedForDataCollectionLevel2)
{
	//given
	auto target = buildBeacon(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF);
	auto mockWebRequestTracer = createMockedWebRequestTracer(target);

	ON_CALL(*mockWebRequestTracer, getBytesSent())
		.WillByDefault(testing::Return(123));
	ON_CALL(*mockWebRequestTracer, getBytesReceived())
		.WillByDefault(testing::Return(45));
	ON_CALL(*mockWebRequestTracer, getResponseCode())
		.WillByDefault(testing::Return(400));

	// verify / then
	EXPECT_CALL(*mockWebRequestTracer, getBytesSent())
		.Times(1);

	EXPECT_CALL(*mockWebRequestTracer, getBytesReceived())
		.Times(1);

	EXPECT_CALL(*mockWebRequestTracer, getResponseCode())
		.Times(1);

	// when
	target->addWebRequest(123, mockWebRequestTracer);

	//then
	ASSERT_FALSE(target->isEmpty());
}

TEST_F(BeaconTest, createTagReturnsEmptyStringForDataCollectionLevel0)
{
	//given
	auto target = buildBeacon(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF);

	// when
	auto tagString = target->createTag(1,1);

	//then
	ASSERT_EQ(tagString.getStringLength(), 0);
}

TEST_F(BeaconTest, createTagReturnsTagStringForDataCollectionLevel1)
{
	//given
	int64_t deviceId = 37;
	int64_t ignoredDeviceId = 999;
	auto appId = std::string("appID");
	auto mockRandom = getMockedRandomGenerator();
	ON_CALL(*mockRandom, nextInt64(testing::_)).WillByDefault(testing::Return(deviceId));

	auto target = buildBeacon(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF, ignoredDeviceId, appId);

	// when
	auto tagString = target->createTag(1, 1);

	//then
	std::stringstream str;
	str << "MT"						// tag prefix
		<< "_3"						// protocol version
		<< "_1"						// server ID
		<< "_" << deviceId			// device ID
		<< "_1" 					// session number (must always be 1 for data collection level performance)
		<< "_appID"					// application ID
		<< "_1"						// parent action ID
		<< "_" << threadIDProvider->getThreadID() // thread ID
		<< "_1"						// sequence number
	;

	ASSERT_THAT(tagString.getStringData(), testing::Eq(str.str()));
}

TEST_F(BeaconTest, createTagReturnsTagStringForDataCollectionLevel2)
{
	//given
	int64_t deviceId = 37;
	int32_t sessionId = 73;
	auto appId = std::string("appID");
	auto mockSessionIdProvider = getSessionIDProviderMock();
	ON_CALL(*mockSessionIdProvider, getNextSessionID()).WillByDefault(testing::Return(sessionId));

	auto target = buildBeacon(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF, deviceId, appId);

	// when
	auto tagString = target->createTag(1, 1);

	// then
	std::stringstream str;
	str << "MT"						// tag prefix
		<< "_3"						// protocol version
		<< "_1"						// server ID
		<< "_" << deviceId			// device ID
		<< "_" << sessionId			// session number (must always be 1 for data collection level performance)
		<< "_" << appId				// application ID
		<< "_1"						// parent action ID
		<< "_" << threadIDProvider->getThreadID() // thread ID
		<< "_1"						// sequence number
	;
	ASSERT_THAT(tagString.getStringData(), str.str());
}

TEST_F(BeaconTest, createTagEncodesDeviceIDPropperly)
{
	// given
	auto target = buildBeacon(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF, -42, APP_ID);

	// when
	auto tagString = target->createTag(1, 1);

	ASSERT_EQ(tagString, std::string("MT_3_1_-42_0_")
							+ APP_ID
							+ std::string("_1_")
							+ std::to_string(threadIDProvider->getThreadID())
							+ std::string("_1"));
}

TEST_F(BeaconTest, createTagUsesEncodedAppID)
{
	// given
	auto target = buildBeacon(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF, 42, "app_ID_");

	// when
	auto tagString = target->createTag(1, 1);

	ASSERT_EQ(tagString, std::string("MT_3_1_42_0_")
		+ std::string("app%5FID%5F")
		+ std::string("_1_")
		+ std::to_string(threadIDProvider->getThreadID())
		+ std::string("_1"));
}

TEST_F(BeaconTest, deviceIDIsRandomizedOnDataCollectionLevel0)
{
	auto mockRandomGenerator = getMockedRandomGenerator();

	// then / verify
	EXPECT_CALL(*mockRandomGenerator, nextInt64(testing::_))
		.Times(1);

	//when/given
	auto target = buildBeacon(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF);
}

TEST_F(BeaconTest, deviceIDIsRandomizedOnDataCollectionLevel1)
{
	auto mockRandomGenerator = getMockedRandomGenerator();

	// then / verify
	EXPECT_CALL(*mockRandomGenerator, nextInt64(testing::_))
		.Times(1);

	//given
	auto target = buildBeacon(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF);
}

TEST_F(BeaconTest, givenDeviceIDIsUsedOnDataCollectionLevel2)
{
	auto mockRandomGenerator = getMockedRandomGenerator();

	// then / verify
	EXPECT_CALL(*mockRandomGenerator, nextInt64(testing::_))
		.Times(0);

	//given
	auto target = buildBeacon(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF);

	// when
	auto deviceID = target->getDeviceID();

	// then
	EXPECT_EQ(getConfiguration()->getDeviceID(), deviceID);
}

TEST_F(BeaconTest, randomDeviceIDCannotBeNegativeOnDataCollectionLevel0)
{
	// given
	auto target = buildBeacon(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF);

	// when
	auto deviceID = target->getDeviceID();

	// then
	EXPECT_THAT(deviceID, testing::AllOf(testing::Ge(int64_t(0)), testing::Lt(std::numeric_limits<int64_t>::max())));
}

TEST_F(BeaconTest, randomDeviceIDCannotBeNegativeOnDataCollectionLevel1)
{
	// given
	auto target = buildBeacon(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF);

	// when
	auto deviceID = target->getDeviceID();

	// then
	EXPECT_THAT(deviceID, testing::AllOf(testing::Ge(int64_t(0)), testing::Lt(std::numeric_limits<int64_t>::max())));
}

TEST_F(BeaconTest, sessionIDIsAlwaysValue1OnDataCollectionLevel0)
{
	// given
	auto target = buildBeacon(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF);

	// when
	auto sessionNumber = target->getSessionNumber();

	// then
	EXPECT_EQ(sessionNumber, 1);
}

TEST_F(BeaconTest, sessionIDIsAlwaysValue1OnDataCollectionLevel1)
{
	//given
	auto target = buildBeacon(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF);

	// when
	auto sessionNumber = target->getSessionNumber();

	// then
	EXPECT_EQ(sessionNumber, 1);
}

TEST_F(BeaconTest, sessionIDIsValueFromSessionIDProviderOnDataCollectionLevel2)
{
	constexpr int32_t THE_ANSWER = 42;

	//given

	auto mockSessionIDProvider = getSessionIDProviderMock();

	ON_CALL(*mockSessionIDProvider, getNextSessionID())
		.WillByDefault(testing::Return(THE_ANSWER));

	EXPECT_CALL(*mockSessionIDProvider, getNextSessionID())
		.Times(1);

	auto target = buildBeacon(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF);

	// when
	auto sessionNumber = target->getSessionNumber();

	// then
	EXPECT_EQ(sessionNumber, THE_ANSWER);
}

TEST_F(BeaconTest, actionNotReportedForDataCollectionLevel0)
{
	// given
	auto target = buildBeacon(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF);
	auto actionMock = createMockedAction();

	//verify
	EXPECT_CALL(*actionMock, getID())
		.Times(0);

	// when
	target->addAction(actionMock);

	//then
	ASSERT_TRUE(target->isEmpty());
}

TEST_F(BeaconTest, actionReportedForDataCollectionLevel1)
{
	//given
	auto target = buildBeacon(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF);
	auto actionMock = createMockedAction();

	//verify
	EXPECT_CALL(*actionMock, getID())
		.Times(1);

	// when
	target->addAction(actionMock);

	//then
	ASSERT_FALSE(target->isEmpty());
}

TEST_F(BeaconTest, actionReportedForDataCollectionLevel2)
{
	//given
	auto target = buildBeacon(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF);
	auto actionMock = createMockedAction();

	//verify
	EXPECT_CALL(*actionMock, getID())
		.Times(1);

	// when
	target->addAction(actionMock);

	//then
	ASSERT_FALSE(target->isEmpty());
}

TEST_F(BeaconTest, sessionNotReportedForDataCollectionLevel0)
{
	//given
	auto target = buildBeacon(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF);
	auto sessionMock = createMockedSession();

	//verify
	EXPECT_CALL(*sessionMock, getEndTime())
		.Times(0);

	// when
	target->endSession(sessionMock);

	//then
	ASSERT_TRUE(target->isEmpty());
}

TEST_F(BeaconTest, sessionReportedForDataCollectionLevel1)
{
	//given
	auto target = buildBeacon(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF);
	auto sessionMock = createMockedSession();

	//verify
	EXPECT_CALL(*sessionMock, getEndTime())
		.Times(2);

	// when
	target->endSession(sessionMock);

	//then
	ASSERT_FALSE(target->isEmpty());
}

TEST_F(BeaconTest, sessionReportedForDataCollectionLevel2)
{
	//given
	auto target = buildBeacon(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF);
	auto sessionMock = createMockedSession();

	//verify
	EXPECT_CALL(*sessionMock, getEndTime())
		.Times(2);

	// when
	target->endSession(sessionMock);

	//then
	ASSERT_FALSE(target->isEmpty());
}

TEST_F(BeaconTest, identifyUserNotAllowedToReportOnDataCollectionLevel0)
{
	//given
	auto target = buildBeacon(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF);
	auto timingProviderMock = getTimingProviderMock();

	EXPECT_CALL(*timingProviderMock, provideTimestampInMilliseconds())
		.Times(0);

	// when
	target->identifyUser(Utf8String_t("testUser"));

	//then
	ASSERT_TRUE(target->isEmpty());
}

TEST_F(BeaconTest, identifyUserNotAllowedToReportOnDataCollectionLevel1)
{
	//given
	auto target = buildBeacon(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF);
	auto timingProviderMock = getTimingProviderMock();

	EXPECT_CALL(*timingProviderMock, provideTimestampInMilliseconds())
		.Times(0);

	// when
	target->identifyUser(Utf8String_t("testUser"));

	//then
	ASSERT_TRUE(target->isEmpty());
}

TEST_F(BeaconTest, identifyUserReportsOnDataCollectionLevel2)
{
	//given
	auto target = buildBeacon(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF);
	auto timingProviderMock = getTimingProviderMock();

	EXPECT_CALL(*timingProviderMock, provideTimestampInMilliseconds())
		.Times(1);

	// when
	target->identifyUser(Utf8String_t("testUser"));

	//then
	ASSERT_FALSE(target->isEmpty());
}

TEST_F(BeaconTest, reportErrorDoesNotReportOnDataCollectionLevel0)
{
	//given
	auto target = buildBeacon(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF);
	auto timingProviderMock = getTimingProviderMock();

	EXPECT_CALL(*timingProviderMock, provideTimestampInMilliseconds())
		.Times(0);

	// when
	target->reportError(1, Utf8String_t("test error name"), 132, Utf8String_t("no reason detected"));

	//then
	ASSERT_TRUE(target->isEmpty());
}

TEST_F(BeaconTest, reportErrorDoesReportOnDataCollectionLevel1)
{
	//given
	auto target = buildBeacon(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF);
	auto timingProviderMock = getTimingProviderMock();

	EXPECT_CALL(*timingProviderMock, provideTimestampInMilliseconds())
		.Times(1);

	// when
	target->reportError(1, Utf8String_t("test error name"), 132, Utf8String_t("no reason detected"));

	//then
	ASSERT_FALSE(target->isEmpty());
}

TEST_F(BeaconTest, reportErrorDoesReportOnDataCollectionLevel2)
{
	//given
	auto target = buildBeacon(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF);
	auto timingProviderMock = getTimingProviderMock();

	EXPECT_CALL(*timingProviderMock, provideTimestampInMilliseconds())
		.Times(1);

	// when
	target->reportError(1, Utf8String_t("test error name"), 132, Utf8String_t("no reason detected"));


	//then
	ASSERT_FALSE(target->isEmpty());
}

TEST_F(BeaconTest, reportCrashDoesNotReportOnDataCollectionLevel0)
{
	//given
	auto target = buildBeacon(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF);
	auto timingProviderMock = getTimingProviderMock();

	EXPECT_CALL(*timingProviderMock, provideTimestampInMilliseconds())
		.Times(0);

	// when
	target->reportCrash(Utf8String_t("OutOfMemory exception"), Utf8String_t("insufficient memory"), Utf8String_t("stacktrace:123"));

	//then
	ASSERT_TRUE(target->isEmpty());
}

TEST_F(BeaconTest, reportCrashDoesNotReportOnDataCollectionLevel1)
{
	//given
	auto target = buildBeacon(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OPT_OUT_CRASHES);
	auto timingProviderMock = getTimingProviderMock();

	EXPECT_CALL(*timingProviderMock, provideTimestampInMilliseconds())
		.Times(0);

	// when
	target->reportCrash(Utf8String_t("OutOfMemory exception"), Utf8String_t("insufficient memory"), Utf8String_t("stacktrace:123"));

	//then
	ASSERT_TRUE(target->isEmpty());
}

TEST_F(BeaconTest, reportCrashDoesReportOnCrashReportingLevel2)
{
	//given
	auto target = buildBeacon(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OPT_IN_CRASHES);
	auto timingProviderMock = getTimingProviderMock();

	EXPECT_CALL(*timingProviderMock, provideTimestampInMilliseconds())
		.Times(1);

	// when
	target->reportCrash(Utf8String_t("OutOfMemory exception"), Utf8String_t("insufficient memory"), Utf8String_t("stacktrace:123"));

	//then
	ASSERT_FALSE(target->isEmpty());
}

TEST_F(BeaconTest, intValueNotReportedForDataCollectionLevel0)
{
	//given
	auto target = buildBeacon(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF);
	auto timingProviderMock = getTimingProviderMock();

	EXPECT_CALL(*timingProviderMock, provideTimestampInMilliseconds())
		.Times(0);

	// when
	target->reportValue(1, "the answer", 42);

	//then
	ASSERT_TRUE(target->isEmpty());
}

TEST_F(BeaconTest, intValueNotReportedForDataCollectionLevel1)
{
	//given
	auto target = buildBeacon(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF);
	auto timingProviderMock = getTimingProviderMock();

	EXPECT_CALL(*timingProviderMock, provideTimestampInMilliseconds())
		.Times(0);

	// when
	target->reportValue(1, "the answer", 42);

	//then
	ASSERT_TRUE(target->isEmpty());
}


TEST_F(BeaconTest, intValueReportedForDataCollectionLevel2)
{
	//given
	auto target = buildBeacon(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF);
	auto timingProviderMock = getTimingProviderMock();

	EXPECT_CALL(*timingProviderMock, provideTimestampInMilliseconds())
		.Times(1);

	// when
	target->reportValue(1, "the answer", 42);

	//then
	ASSERT_FALSE(target->isEmpty());
}

TEST_F(BeaconTest, doubleValueNotReportedForDataCollectionLevel0)
{
	//given
	auto target = buildBeacon(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF);
	auto timingProviderMock = getTimingProviderMock();

	EXPECT_CALL(*timingProviderMock, provideTimestampInMilliseconds())
		.Times(0);

	// when
	target->reportValue(1, "the answer", 42.0);

	//then
	ASSERT_TRUE(target->isEmpty());
}

TEST_F(BeaconTest, doubleValueNotReportedForDataCollectionLevel1)
{
	//given
	auto target = buildBeacon(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF);
	auto timingProviderMock = getTimingProviderMock();

	EXPECT_CALL(*timingProviderMock, provideTimestampInMilliseconds())
		.Times(0);

	// when
	target->reportValue(1, "the answer", 42.0);

	//then
	ASSERT_TRUE(target->isEmpty());
}


TEST_F(BeaconTest, doubleValueReportedForDataCollectionLevel2)
{
	//given
	auto target = buildBeacon(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF);
	auto timingProviderMock = getTimingProviderMock();

	EXPECT_CALL(*timingProviderMock, provideTimestampInMilliseconds())
		.Times(1);

	// when
	target->reportValue(1, "the answer", 42.0);

	//then
	ASSERT_FALSE(target->isEmpty());
}

TEST_F(BeaconTest, stringValueNotReportedForDataCollectionLevel0)
{
	//given
	auto target = buildBeacon(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF);
	auto timingProviderMock = getTimingProviderMock();

	EXPECT_CALL(*timingProviderMock, provideTimestampInMilliseconds())
		.Times(0);

	// when
	target->reportValue(1, "the answer", "the answer is 42");

	//then
	ASSERT_TRUE(target->isEmpty());
}

TEST_F(BeaconTest, stringValueNotReportedForDataCollectionLevel1)
{
	//given
	auto target = buildBeacon(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF);
	auto timingProviderMock = getTimingProviderMock();

	EXPECT_CALL(*timingProviderMock, provideTimestampInMilliseconds())
		.Times(0);

	// when
	target->reportValue(1, "the answer", "the answer is 42");

	//then
	ASSERT_TRUE(target->isEmpty());
}


TEST_F(BeaconTest, stringValueReportedForDataCollectionLevel2)
{
	//given
	auto target = buildBeacon(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF);
	auto timingProviderMock = getTimingProviderMock();

	EXPECT_CALL(*timingProviderMock, provideTimestampInMilliseconds())
		.Times(1);

	// when
	target->reportValue(1, "the answer", "the answer is 42");

	//then
	ASSERT_FALSE(target->isEmpty());
}

TEST_F(BeaconTest, namedEventNotReportedForDataCollectionLevel0)
{
	//given
	auto target = buildBeacon(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF);
	auto timingProviderMock = getTimingProviderMock();

	EXPECT_CALL(*timingProviderMock, provideTimestampInMilliseconds())
		.Times(0);

	// when
	target->reportEvent(1, "2 equals to 3 for large values of 2");

	//then
	ASSERT_TRUE(target->isEmpty());
}

TEST_F(BeaconTest, namedEventNotReportedForDataCollectionLevel1)
{
	//given
	auto target = buildBeacon(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF);
	auto timingProviderMock = getTimingProviderMock();

	EXPECT_CALL(*timingProviderMock, provideTimestampInMilliseconds())
		.Times(0);

	// when
	target->reportEvent(1, "2 equals to 3 for large values of 2");

	//then
	ASSERT_TRUE(target->isEmpty());
}


TEST_F(BeaconTest, namedEventNotReportedForDataCollectionLevel2)
{
	//given
	auto target = buildBeacon(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF);
	auto timingProviderMock = getTimingProviderMock();

	EXPECT_CALL(*timingProviderMock, provideTimestampInMilliseconds())
		.Times(1);

	// when
	target->reportEvent(1, "2 equals to 3 for large values of 2");

	//then
	ASSERT_FALSE(target->isEmpty());
}

TEST_F(BeaconTest, sessionStartIsReported)
{
	//given
	auto target = buildBeaconWithDefaultConfig();

	// when
	target->startSession();

	//then
	ASSERT_FALSE(target->isEmpty());
}

TEST_F(BeaconTest, sessionStartIsReportedForDataCollectionLevel0)
{
	//given
	auto target = buildBeacon(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF);

	// when
	target->startSession();

	//then
	ASSERT_FALSE(target->isEmpty());
}

TEST_F(BeaconTest, sessionStartIsReportedForDataCollectionLevel1)
{
	//given
	auto target = buildBeacon(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF);

	// when
	target->startSession();

	//then
	ASSERT_FALSE(target->isEmpty());
}

TEST_F(BeaconTest, sessionStartIsReportedForDataCollectionLevel2)
{
	//given
	auto target = buildBeacon(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF);

	// when
	target->startSession();

	//then
	ASSERT_FALSE(target->isEmpty());
}

TEST_F(BeaconTest, clientIPAddressCanBeANullptr)
{
	//given
	auto target = buildBeacon(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF, DEVICE_ID, Utf8String_t(APP_ID), nullptr);

	// when, then
	ASSERT_EQ(0, target->getClientIPAddress().getStringLength());
}

TEST_F(BeaconTest, clientIPAddressCanBeAnEmptyString)
{
	//given
	auto target = buildBeacon(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF, DEVICE_ID, Utf8String_t(APP_ID), "");

	// when, then
	ASSERT_EQ(0, target->getClientIPAddress().getStringLength());
}

TEST_F(BeaconTest, validClientIpIsStored)
{
	//given
	auto target = buildBeacon(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF, DEVICE_ID, Utf8String_t(APP_ID), "127.0.0.1");

	// when, then
	ASSERT_STREQ("127.0.0.1", target->getClientIPAddress().getStringData().c_str());
}

TEST_F(BeaconTest, invalidClientIPIsConvertedToEmptyString)
{
	//given
	auto target = buildBeacon(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF, DEVICE_ID, Utf8String_t(APP_ID), "asdf");

	// when, then
	ASSERT_EQ(0, target->getClientIPAddress().getStringLength());
}

TEST_F(BeaconTest, useInternalBeaconIdForAccessingBeaconCacheWhenSessionNumberReportingDisabled)
{
	// given
	auto mockBeaconCache = MockIBeaconCache::createNice();
	beaconCache = mockBeaconCache;

	int32_t beaconId = 73;
	auto mockSessionIdProvider = getSessionIDProviderMock();
	ON_CALL(*mockSessionIdProvider, getNextSessionID()).WillByDefault(testing::Return(beaconId));

	auto target = buildBeacon(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OPT_IN_CRASHES);

	// expect
	EXPECT_CALL(*mockBeaconCache, deleteCacheEntry(beaconId)).Times(1);

	// when
	target->clearData();

}