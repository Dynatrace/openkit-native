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
#include "protocol/Beacon.h"

#include "OpenKit/DataCollectionLevel.h"
#include "OpenKit/CrashReportingLevel.h"
#include "caching/BeaconCache.h"

#include "core/util/DefaultLogger.h"
#include "providers/DefaultThreadIDProvider.h"
#include "providers/DefaultTimingProvider.h"
#include "protocol/ssl/SSLStrictTrustManager.h"
#include "providers/DefaultSessionIDProvider.h"
#include "providers/DefaultHTTPClientProvider.h"
#include "core/BeaconSender.h"
#include "core/WebRequestTracerStringURL.h"
#include "core/Action.h"
#include "configuration/Configuration.h"

#include "../protocol/MockHTTPClient.h"
#include "../providers/MockHTTPClientProvider.h"
#include "../core/MockWebRequestTracer.h"
#include "../providers/MockPRNGenerator.h"
#include "../providers/MockSessionIDProvider.h"
#include "../core/MockAction.h"
#include "../core/MockRootAction.h"
#include "../core/MockSession.h"
#include "../providers/MockTimingProvider.h"

#include <locale>

using namespace core;
using namespace protocol;

static const char APP_ID[] = "appID";
static const char APP_NAME[] = "appName";

class BeaconTest : public testing::Test
{
public:
	void SetUp()
	{
		logger = std::shared_ptr<openkit::ILogger>(new core::util::DefaultLogger(devNull, true));
		threadIDProvider = std::make_shared<providers::DefaultThreadIDProvider>();
		timingProvider = std::make_shared<providers::DefaultTimingProvider>();
		sessionIDProvider = std::make_shared<providers::DefaultSessionIDProvider>();

		std::shared_ptr<configuration::HTTPClientConfiguration> httpClientConfiguration = std::make_shared<configuration::HTTPClientConfiguration>(core::UTF8String(""), 0, core::UTF8String(""));
		mockHTTPClientProvider = std::make_shared<testing::NiceMock<test::MockHTTPClientProvider>>();
		mockHTTPClient = std::shared_ptr<testing::NiceMock<test::MockHTTPClient>>(new testing::NiceMock<test::MockHTTPClient>(httpClientConfiguration));

		trustManager = std::make_shared<protocol::SSLStrictTrustManager>();

		device = std::shared_ptr<configuration::Device>(new configuration::Device(core::UTF8String(""), core::UTF8String(""), core::UTF8String("")));

		beaconCacheConfiguration = std::make_shared<configuration::BeaconCacheConfiguration>(-1, -1, -1);
		
		beaconCache = std::make_shared<caching::BeaconCache>(logger);

		randomGeneratorMock = std::make_shared<testing::NiceMock<test::MockPRNGenerator>>();
		sessionIDProviderMock = std::make_shared<testing::NiceMock<test::MockSessionIDProvider>>();
		mockTimingProvider = std::make_shared<testing::NiceMock<test::MockTimingProvider>>();

		// manipulate the global locale to a variant of the classic (C/POSIX) one.
		auto newLocale = std::locale(std::locale::classic(), new SpecialNumpunctFacet());
		currentLocale = std::locale::global(newLocale);
	}

	std::shared_ptr<protocol::Beacon> buildBeaconWithDefaultConfig()
	{
		std::shared_ptr<configuration::BeaconConfiguration> beaconConfiguration = std::make_shared<configuration::BeaconConfiguration>();

		configuration = std::shared_ptr<configuration::Configuration>(new configuration::Configuration(device, configuration::OpenKitType::Type::DYNATRACE,
			core::UTF8String(APP_NAME), "", APP_ID, 0, "",
			sessionIDProviderMock, trustManager, beaconCacheConfiguration, beaconConfiguration));
		configuration->enableCapture();

		return std::make_shared<protocol::Beacon>(logger, beaconCache, configuration, nullptr, threadIDProvider, mockTimingProvider, randomGeneratorMock);
	}

	std::shared_ptr<protocol::Beacon> buildBeacon(openkit::DataCollectionLevel dl, openkit::CrashReportingLevel cl)
	{
		return buildBeacon(dl, cl, APP_ID, 0);
	}

	std::shared_ptr<protocol::Beacon> buildBeacon(openkit::DataCollectionLevel dl, openkit::CrashReportingLevel cl, const core::UTF8String& appID, int64_t deviceID)
	{
		return buildBeacon(dl, cl, deviceID, appID, "127.0.0.1");
	}

	std::shared_ptr<protocol::Beacon> buildBeacon(openkit::DataCollectionLevel dl, openkit::CrashReportingLevel cl, int64_t deviceID, const core::UTF8String& appID, const char* clientIPAddress)
	{
		auto beaconConfiguration = std::make_shared<configuration::BeaconConfiguration>(configuration::BeaconConfiguration::DEFAULT_MULTIPLICITY, dl, cl);

		configuration = std::shared_ptr<configuration::Configuration>(new configuration::Configuration(device, configuration::OpenKitType::Type::DYNATRACE,
			core::UTF8String(APP_NAME), "", appID, deviceID, "",
			sessionIDProviderMock, trustManager, beaconCacheConfiguration, beaconConfiguration));
		configuration->enableCapture();

		return std::make_shared<protocol::Beacon>(logger, beaconCache, configuration, clientIPAddress, threadIDProvider, mockTimingProvider, randomGeneratorMock);
	}

	std::shared_ptr<testing::NiceMock<test::MockWebRequestTracer>> createMockedWebRequestTracer(std::shared_ptr<protocol::Beacon> beacon)
	{
		return std::make_shared<testing::NiceMock<test::MockWebRequestTracer>>(logger, beacon);
	}

	std::shared_ptr<testing::NiceMock<test::MockSession>> createMockedSession()
	{
		return std::make_shared<testing::NiceMock<test::MockSession>>(logger);
	}

	std::shared_ptr<testing::NiceMock<test::MockAction>> createMockedAction(std::shared_ptr<protocol::Beacon> beacon)
	{
		return std::make_shared<testing::NiceMock<test::MockAction>>(logger, beacon);
	}

	std::shared_ptr<testing::NiceMock<test::MockRootAction>> createMockedRootAction(std::shared_ptr<protocol::Beacon> beacon)
	{
		return std::make_shared<testing::NiceMock<test::MockRootAction>>(logger, beacon, createMockedSession());
	}

	std::shared_ptr<testing::NiceMock<test::MockPRNGenerator>> getMockedRandomGenerator()
	{
		return randomGeneratorMock;
	}

	std::shared_ptr<testing::NiceMock<test::MockSessionIDProvider>> getSessionIDProviderMock()
	{
		return sessionIDProviderMock;
	}

	std::shared_ptr<testing::NiceMock<test::MockTimingProvider>> getTimingProviderMock()
	{
		return mockTimingProvider;
	}

	std::shared_ptr<configuration::Configuration> getConfiguration()
	{
		return configuration;
	}

	std::shared_ptr<openkit::ILogger> getLogger()
	{
		return logger;
	}

	void TearDown()
	{
		// reset global locale
		std::locale::global(currentLocale);
	}

protected:

	// special numpunct facet for testing purposes
	class SpecialNumpunctFacet : public std::numpunct<char>
	{
		char do_decimal_point() const override
		{
			return '~'; // use tilde as decimal point
		}

		char do_thousands_sep() const override
		{
			return ';'; // use semicolon as thousand separator
		}

		std::string do_grouping() const override
		{
			return "\1"; // group after each character
		}
	};

	std::ostringstream devNull;
	std::locale currentLocale;

	std::shared_ptr<openkit::ILogger> logger;
	std::shared_ptr<providers::IThreadIDProvider> threadIDProvider;
	std::shared_ptr<providers::ITimingProvider> timingProvider;
	std::shared_ptr<providers::ISessionIDProvider> sessionIDProvider;

	std::shared_ptr<testing::NiceMock<test::MockHTTPClient>> mockHTTPClient;
	std::shared_ptr<openkit::ISSLTrustManager> trustManager;

	std::shared_ptr<configuration::Device> device;
	std::shared_ptr<configuration::BeaconCacheConfiguration> beaconCacheConfiguration;
	std::shared_ptr<caching::BeaconCache> beaconCache;

	std::shared_ptr<core::BeaconSender> beaconSender;
	std::shared_ptr<testing::NiceMock<test::MockHTTPClientProvider>> mockHTTPClientProvider;

	std::shared_ptr<testing::NiceMock<test::MockPRNGenerator>> randomGeneratorMock;
	std::shared_ptr<testing::NiceMock<test::MockSessionIDProvider>> sessionIDProviderMock;
	std::shared_ptr<configuration::Configuration> configuration;
	std::shared_ptr<testing::NiceMock<test::MockTimingProvider>> mockTimingProvider;
};

TEST_F(BeaconTest, noWebRequestIsReportedForDataCollectionLevel0)
{
	//given
	auto target = buildBeacon(openkit::DataCollectionLevel::OFF, openkit::CrashReportingLevel::OFF);
	auto mockWebRequestTracer = std::make_shared<testing::NiceMock<test::MockWebRequestTracer>>(getLogger(), target);

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
	auto target = buildBeacon(openkit::DataCollectionLevel::PERFORMANCE, openkit::CrashReportingLevel::OFF);
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
	auto target = buildBeacon(openkit::DataCollectionLevel::USER_BEHAVIOR, openkit::CrashReportingLevel::OFF);
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
	auto target = buildBeacon(openkit::DataCollectionLevel::OFF, openkit::CrashReportingLevel::OFF);

	// when
	auto tagString = target->createTag(1,1);

	//then
	ASSERT_EQ(tagString.getStringLength(), 0);
}

TEST_F(BeaconTest, createTagReturnsTagStringForDataCollectionLevel1)
{
	//given
	auto target = buildBeacon(openkit::DataCollectionLevel::PERFORMANCE, openkit::CrashReportingLevel::OFF);

	// when
	auto tagString = target->createTag(1, 1);

	//then
	ASSERT_GT(tagString.getStringLength(), 0u);
}

TEST_F(BeaconTest, createTagReturnsTagStringForDataCollectionLevel2)
{
	//given
	auto target = buildBeacon(openkit::DataCollectionLevel::USER_BEHAVIOR, openkit::CrashReportingLevel::OFF);

	// when
	auto tagString = target->createTag(1, 1);

	// then
	ASSERT_GT(tagString.getStringLength(), 0u);
}

TEST_F(BeaconTest, createTagUsesEncodedAppID)
{
	// given
	auto target = buildBeacon(openkit::DataCollectionLevel::USER_BEHAVIOR, openkit::CrashReportingLevel::OFF, "app_ID_", 42);

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
	auto target = buildBeacon(openkit::DataCollectionLevel::OFF, openkit::CrashReportingLevel::OFF);
}

TEST_F(BeaconTest, deviceIDIsRandomizedOnDataCollectionLevel1)
{
	auto mockRandomGenerator = getMockedRandomGenerator();

	// then / verify
	EXPECT_CALL(*mockRandomGenerator, nextInt64(testing::_))
		.Times(1);

	//given
	auto target = buildBeacon(openkit::DataCollectionLevel::PERFORMANCE, openkit::CrashReportingLevel::OFF);
}

TEST_F(BeaconTest, givenDeviceIDIsUsedOnDataCollectionLevel2)
{
	auto mockRandomGenerator = getMockedRandomGenerator();

	// then / verify
	EXPECT_CALL(*mockRandomGenerator, nextInt64(testing::_))
		.Times(0);

	//given
	auto target = buildBeacon(openkit::DataCollectionLevel::USER_BEHAVIOR, openkit::CrashReportingLevel::OFF);

	// when
	auto deviceID = target->getDeviceID();

	// then
	EXPECT_EQ(getConfiguration()->getDeviceID(), deviceID);
}

TEST_F(BeaconTest, randomVisitorIDCannotBeNegativeOnDataCollectionLevel0)
{
	// given
	auto target = buildBeacon(openkit::DataCollectionLevel::OFF, openkit::CrashReportingLevel::OFF);

	// when
	auto deviceID = target->getDeviceID();

	// then
	EXPECT_THAT(deviceID, testing::AllOf(testing::Ge(int64_t(0)), testing::Lt(std::numeric_limits<int64_t>::max())));
}

TEST_F(BeaconTest, randomVisitorIDCannotBeNegativeOnDataCollectionLevel1)
{
	// given
	auto target = buildBeacon(openkit::DataCollectionLevel::PERFORMANCE, openkit::CrashReportingLevel::OFF);

	// when
	auto deviceID = target->getDeviceID();

	// then
	EXPECT_THAT(deviceID, testing::AllOf(testing::Ge(int64_t(0)), testing::Lt(std::numeric_limits<int64_t>::max())));
}

TEST_F(BeaconTest, sessionIDIsAlwaysValue1OnDataCollectionLevel0)
{
	// given
	auto target = buildBeacon(openkit::DataCollectionLevel::OFF, openkit::CrashReportingLevel::OFF);

	// when
	auto sessionNumber = target->getSessionNumber();

	// then
	EXPECT_EQ(sessionNumber, 1);
}

TEST_F(BeaconTest, sessionIDIsAlwaysValue1OnDataCollectionLevel1)
{
	//given
	auto target = buildBeacon(openkit::DataCollectionLevel::PERFORMANCE, openkit::CrashReportingLevel::OFF);

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

	auto target = buildBeacon(openkit::DataCollectionLevel::USER_BEHAVIOR, openkit::CrashReportingLevel::OFF);

	// when
	auto sessionNumber = target->getSessionNumber();

	// then
	EXPECT_EQ(sessionNumber, THE_ANSWER);
}

TEST_F(BeaconTest, actionNotReportedForDataCollectionLevel0_Action)
{
	// given
	auto target = buildBeacon(openkit::DataCollectionLevel::OFF, openkit::CrashReportingLevel::OFF);
	auto actionMock = createMockedAction(target);

	//verify
	EXPECT_CALL(*actionMock, getID())
		.Times(0);

	// when
	target->addAction(actionMock);

	//then
	ASSERT_TRUE(target->isEmpty());
}


TEST_F(BeaconTest, actionNotReportedForDataCollectionLevel0_RootAction)
{
	//given
	auto target = buildBeacon(openkit::DataCollectionLevel::OFF, openkit::CrashReportingLevel::OFF);
	auto rootActionMock = createMockedRootAction(target);

	//verify
	EXPECT_CALL(*rootActionMock, getID())
		.Times(0);

	// when
	target->addAction(rootActionMock);

	//then
	ASSERT_TRUE(target->isEmpty());
}

TEST_F(BeaconTest, actionReportedForDataCollectionLevel1_Action)
{
	//given
	auto target = buildBeacon(openkit::DataCollectionLevel::PERFORMANCE, openkit::CrashReportingLevel::OFF);
	auto actionMock = createMockedAction(target);

	//verify
	EXPECT_CALL(*actionMock, getID())
		.Times(1);

	// when
	target->addAction(actionMock);

	//then
	ASSERT_FALSE(target->isEmpty());
}


TEST_F(BeaconTest, actionReportedForDataCollectionLevel1_RootAction)
{
	//given
	auto target = buildBeacon(openkit::DataCollectionLevel::PERFORMANCE, openkit::CrashReportingLevel::OFF);
	auto rootActionMock = createMockedRootAction(target);

	//verify
	EXPECT_CALL(*rootActionMock, getID())
		.Times(1);

	// when
	target->addAction(rootActionMock);

	//then
	ASSERT_FALSE(target->isEmpty());
}

TEST_F(BeaconTest, actionReportedForDataCollectionLevel2_Action)
{
	//given
	auto target = buildBeacon(openkit::DataCollectionLevel::USER_BEHAVIOR, openkit::CrashReportingLevel::OFF);
	auto actionMock = createMockedAction(target);

	//verify
	EXPECT_CALL(*actionMock, getID())
		.Times(1);

	// when
	target->addAction(actionMock);

	//then
	ASSERT_FALSE(target->isEmpty());
}


TEST_F(BeaconTest, actionReportedForDataCollectionLevel2_RootAction)
{
	//given
	auto target = buildBeacon(openkit::DataCollectionLevel::USER_BEHAVIOR, openkit::CrashReportingLevel::OFF);
	auto rootActionMock = createMockedRootAction(target);

	//verify
	EXPECT_CALL(*rootActionMock, getID())
		.Times(1);

	// when
	target->addAction(rootActionMock);

	//then
	ASSERT_FALSE(target->isEmpty());
}

TEST_F(BeaconTest, sessionNotReportedForDataCollectionLevel0)
{
	//given
	auto target = buildBeacon(openkit::DataCollectionLevel::OFF, openkit::CrashReportingLevel::OFF);
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
	auto target = buildBeacon(openkit::DataCollectionLevel::PERFORMANCE, openkit::CrashReportingLevel::OFF);
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
	auto target = buildBeacon(openkit::DataCollectionLevel::USER_BEHAVIOR, openkit::CrashReportingLevel::OFF);
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
	auto target = buildBeacon(openkit::DataCollectionLevel::OFF, openkit::CrashReportingLevel::OFF);
	auto timingProviderMock = getTimingProviderMock();

	EXPECT_CALL(*timingProviderMock, provideTimestampInMilliseconds())
		.Times(0);

	// when
	target->identifyUser(core::UTF8String("testUser"));

	//then
	ASSERT_TRUE(target->isEmpty());
}

TEST_F(BeaconTest, identifyUserNotAllowedToReportOnDataCollectionLevel1)
{
	//given
	auto target = buildBeacon(openkit::DataCollectionLevel::PERFORMANCE, openkit::CrashReportingLevel::OFF);
	auto timingProviderMock = getTimingProviderMock();

	EXPECT_CALL(*timingProviderMock, provideTimestampInMilliseconds())
		.Times(0);

	// when
	target->identifyUser(core::UTF8String("testUser"));

	//then
	ASSERT_TRUE(target->isEmpty());
}

TEST_F(BeaconTest, identifyUserReportsOnDataCollectionLevel2)
{
	//given
	auto target = buildBeacon(openkit::DataCollectionLevel::USER_BEHAVIOR, openkit::CrashReportingLevel::OFF);
	auto timingProviderMock = getTimingProviderMock();

	EXPECT_CALL(*timingProviderMock, provideTimestampInMilliseconds())
		.Times(1);

	// when
	target->identifyUser(core::UTF8String("testUser"));

	//then
	ASSERT_FALSE(target->isEmpty());
}

TEST_F(BeaconTest, reportErrorDoesNotReportOnDataCollectionLevel0)
{
	//given
	auto target = buildBeacon(openkit::DataCollectionLevel::OFF, openkit::CrashReportingLevel::OFF);
	auto timingProviderMock = getTimingProviderMock();

	EXPECT_CALL(*timingProviderMock, provideTimestampInMilliseconds())
		.Times(0);

	// when
	target->reportError(1, core::UTF8String("test error name"), 132, core::UTF8String("no reason detected"));

	//then
	ASSERT_TRUE(target->isEmpty());
}

TEST_F(BeaconTest, reportErrorDoesReportOnDataCollectionLevel1)
{
	//given
	auto target = buildBeacon(openkit::DataCollectionLevel::PERFORMANCE, openkit::CrashReportingLevel::OFF);
	auto timingProviderMock = getTimingProviderMock();

	EXPECT_CALL(*timingProviderMock, provideTimestampInMilliseconds())
		.Times(1);

	// when
	target->reportError(1, core::UTF8String("test error name"), 132, core::UTF8String("no reason detected"));

	//then
	ASSERT_FALSE(target->isEmpty());
}

TEST_F(BeaconTest, reportErrorDoesReportOnDataCollectionLevel2)
{
	//given
	auto target = buildBeacon(openkit::DataCollectionLevel::USER_BEHAVIOR, openkit::CrashReportingLevel::OFF);
	auto timingProviderMock = getTimingProviderMock();

	EXPECT_CALL(*timingProviderMock, provideTimestampInMilliseconds())
		.Times(1);

	// when
	target->reportError(1, core::UTF8String("test error name"), 132, core::UTF8String("no reason detected"));


	//then
	ASSERT_FALSE(target->isEmpty());
}

TEST_F(BeaconTest, reportCrashDoesNotReportOnDataCollectionLevel0)
{
	//given
	auto target = buildBeacon(openkit::DataCollectionLevel::OFF, openkit::CrashReportingLevel::OFF);
	auto timingProviderMock = getTimingProviderMock();

	EXPECT_CALL(*timingProviderMock, provideTimestampInMilliseconds())
		.Times(0);

	// when
	target->reportCrash(core::UTF8String("OutOfMemory exception"), core::UTF8String("insufficient memory"), core::UTF8String("stacktrace:123"));

	//then
	ASSERT_TRUE(target->isEmpty());
}

TEST_F(BeaconTest, reportCrashDoesNotReportOnDataCollectionLevel1)
{
	//given
	auto target = buildBeacon(openkit::DataCollectionLevel::OFF, openkit::CrashReportingLevel::OPT_OUT_CRASHES);
	auto timingProviderMock = getTimingProviderMock();

	EXPECT_CALL(*timingProviderMock, provideTimestampInMilliseconds())
		.Times(0);

	// when
	target->reportCrash(core::UTF8String("OutOfMemory exception"), core::UTF8String("insufficient memory"), core::UTF8String("stacktrace:123"));

	//then
	ASSERT_TRUE(target->isEmpty());
}

TEST_F(BeaconTest, reportCrashDoesReportOnCrashReportingLevel2)
{
	//given
	auto target = buildBeacon(openkit::DataCollectionLevel::OFF, openkit::CrashReportingLevel::OPT_IN_CRASHES);
	auto timingProviderMock = getTimingProviderMock();

	EXPECT_CALL(*timingProviderMock, provideTimestampInMilliseconds())
		.Times(1);

	// when
	target->reportCrash(core::UTF8String("OutOfMemory exception"), core::UTF8String("insufficient memory"), core::UTF8String("stacktrace:123"));

	//then
	ASSERT_FALSE(target->isEmpty());
}

TEST_F(BeaconTest, intValueNotReportedForDataCollectionLevel0)
{
	//given
	auto target = buildBeacon(openkit::DataCollectionLevel::OFF, openkit::CrashReportingLevel::OFF);
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
	auto target = buildBeacon(openkit::DataCollectionLevel::PERFORMANCE, openkit::CrashReportingLevel::OFF);
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
	auto target = buildBeacon(openkit::DataCollectionLevel::USER_BEHAVIOR, openkit::CrashReportingLevel::OFF);
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
	auto target = buildBeacon(openkit::DataCollectionLevel::OFF, openkit::CrashReportingLevel::OFF);
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
	auto target = buildBeacon(openkit::DataCollectionLevel::PERFORMANCE, openkit::CrashReportingLevel::OFF);
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
	auto target = buildBeacon(openkit::DataCollectionLevel::USER_BEHAVIOR, openkit::CrashReportingLevel::OFF);
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
	auto target = buildBeacon(openkit::DataCollectionLevel::OFF, openkit::CrashReportingLevel::OFF);
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
	auto target = buildBeacon(openkit::DataCollectionLevel::PERFORMANCE, openkit::CrashReportingLevel::OFF);
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
	auto target = buildBeacon(openkit::DataCollectionLevel::USER_BEHAVIOR, openkit::CrashReportingLevel::OFF);
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
	auto target = buildBeacon(openkit::DataCollectionLevel::OFF, openkit::CrashReportingLevel::OFF);
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
	auto target = buildBeacon(openkit::DataCollectionLevel::PERFORMANCE, openkit::CrashReportingLevel::OFF);
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
	auto target = buildBeacon(openkit::DataCollectionLevel::USER_BEHAVIOR, openkit::CrashReportingLevel::OFF);
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
	auto target = buildBeacon(openkit::DataCollectionLevel::OFF, openkit::CrashReportingLevel::OFF);

	// when
	target->startSession();

	//then
	ASSERT_FALSE(target->isEmpty());
}

TEST_F(BeaconTest, sessionStartIsReportedForDataCollectionLevel1)
{
	//given
	auto target = buildBeacon(openkit::DataCollectionLevel::PERFORMANCE, openkit::CrashReportingLevel::OFF);

	// when
	target->startSession();

	//then
	ASSERT_FALSE(target->isEmpty());
}

TEST_F(BeaconTest, sessionStartIsReportedForDataCollectionLevel2)
{
	//given
	auto target = buildBeacon(openkit::DataCollectionLevel::USER_BEHAVIOR, openkit::CrashReportingLevel::OFF);

	// when
	target->startSession();

	//then
	ASSERT_FALSE(target->isEmpty());
}

TEST_F(BeaconTest, clientIPAddressCanBeANullptr)
{
	//given
	auto target = buildBeacon(openkit::DataCollectionLevel::USER_BEHAVIOR, openkit::CrashReportingLevel::OFF, 1234, core::UTF8String(APP_ID), nullptr);
	
	// when, then
	ASSERT_EQ(0, target->getClientIPAddress().getStringLength());
}

TEST_F(BeaconTest, clientIPAddressCanBeAnEmptyString)
{
	//given
	auto target = buildBeacon(openkit::DataCollectionLevel::USER_BEHAVIOR, openkit::CrashReportingLevel::OFF, 1234, core::UTF8String(APP_ID), "");

	// when, then
	ASSERT_EQ(0, target->getClientIPAddress().getStringLength());
}

TEST_F(BeaconTest, validClientIpIsStored)
{
	//given
	auto target = buildBeacon(openkit::DataCollectionLevel::USER_BEHAVIOR, openkit::CrashReportingLevel::OFF, 1234, core::UTF8String(APP_ID), "127.0.0.1");

	// when, then
	ASSERT_STREQ("127.0.0.1", target->getClientIPAddress().getStringData().c_str());
}

TEST_F(BeaconTest, invalidClientIPIsConvertedToEmptyString)
{
	//given
	auto target = buildBeacon(openkit::DataCollectionLevel::USER_BEHAVIOR, openkit::CrashReportingLevel::OFF, 1234, core::UTF8String(APP_ID), "asdf");

	// when, then
	ASSERT_EQ(0, target->getClientIPAddress().getStringLength());
}

TEST_F(BeaconTest, createTagReturnsLocaleIndependentTagString)
{
	//given
	auto target = buildBeacon(openkit::DataCollectionLevel::USER_BEHAVIOR, openkit::CrashReportingLevel::OFF);

	// when
	auto tagString = target->createTag(123456, 987654);

	// then
	ASSERT_THAT(tagString.getStringData(), ::testing::StartsWith("MT_3_1_0_0_appID_123456_"));
	ASSERT_THAT(tagString.getStringData(), ::testing::EndsWith("_987654"));
}

TEST_F(BeaconTest, reportedIntValueIsLocaleIndependent)
{
	//given
	auto target = buildBeacon(openkit::DataCollectionLevel::USER_BEHAVIOR, openkit::CrashReportingLevel::OFF);
	auto timingProviderMock = getTimingProviderMock();

	EXPECT_CALL(*timingProviderMock, provideTimestampInMilliseconds())
		.Times(1);

	// when
	target->reportValue(1, "foo", 123456789);

	//then
	ASSERT_THAT(beaconCache->getBeaconIDs().size(), ::testing::Eq(1));
	auto eventData = beaconCache->getEvents(*beaconCache->getBeaconIDs().begin());
	ASSERT_THAT(eventData.size(), ::testing::Eq(1));
	ASSERT_THAT(eventData[0].getStringData(), ::testing::EndsWith("&vl=123456789"));
}

TEST_F(BeaconTest, reportedDoubleValueIsLocaleIndependent)
{
	//given
	auto target = buildBeacon(openkit::DataCollectionLevel::USER_BEHAVIOR, openkit::CrashReportingLevel::OFF);
	auto timingProviderMock = getTimingProviderMock();

	EXPECT_CALL(*timingProviderMock, provideTimestampInMilliseconds())
		.Times(1);

	// when
	target->reportValue(1, "bar", 12345.125);

	//then
	ASSERT_THAT(beaconCache->getBeaconIDs().size(), ::testing::Eq(1));
	auto eventData = beaconCache->getEvents(*beaconCache->getBeaconIDs().begin());
	ASSERT_THAT(eventData.size(), ::testing::Eq(1));
	ASSERT_THAT(eventData[0].getStringData(), ::testing::EndsWith("&vl=12345.125000")); // fixed floating point with 6 digits precision
}
