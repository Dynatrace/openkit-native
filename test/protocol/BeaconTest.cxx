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

#include "builder/TestBeaconBuilder.h"
#include "mock/MockIHTTPClient.h"
#include "../api/mock/MockILogger.h"
#include "../api/mock/MockISslTrustManager.h"
#include "../core/caching/mock/MockIBeaconCache.h"
#include "../core/configuration/mock/MockIBeaconCacheConfiguration.h"
#include "../core/configuration/mock/MockIBeaconConfiguration.h"
#include "../core/objects/mock/MockIActionCommon.h"
#include "../core/objects/mock/MockIOpenKitComposite.h"
#include "../core/objects/mock/MockSessionInternals.h"
#include "../core/objects/mock/MockIWebRequestTracerInternals.h"
#include "../protocol/mock/MockIStatusResponse.h"
#include "../providers/mock/MockIHTTPClientProvider.h"
#include "../providers/mock/MockIPRNGenerator.h"
#include "../providers/mock/MockISessionIDProvider.h"
#include "../providers/mock/MockIThreadIDProvider.h"
#include "../providers/mock/MockITimingProvider.h"

#include "OpenKit/CrashReportingLevel.h"
#include "OpenKit/DataCollectionLevel.h"
#include "core/UTF8String.h"
#include "core/caching/BeaconCache.h"
#include "core/configuration/Configuration.h"
#include "core/configuration/ConfigurationDefaults.h"
#include "core/configuration/Device.h"
#include "core/configuration/OpenKitType.h"
#include "core/objects/WebRequestTracer.h"
#include "core/util/URLEncoding.h"
#include "protocol/Beacon.h"
#include "protocol/EventType.h"
#include "protocol/ProtocolConstants.h"

#include <sstream>

using namespace test;

using Beacon_t = protocol::Beacon;
using Beacon_sp = std::shared_ptr<Beacon_t>;
using BeaconBuilder_sp = std::shared_ptr<TestBeaconBuilder>;
using BeaconCache_t = core::caching::BeaconCache;
using Configuration_t = core::configuration::Configuration;
using Configuration_sp = std::shared_ptr<Configuration_t>;
using CrashReportingLevel_t = openkit::CrashReportingLevel;
using DataCollectionLevel_t = openkit::DataCollectionLevel;
using Device_t = core::configuration::Device;
using EventType_t = protocol::EventType;
using MockNiceIHTTPClient_sp = std::shared_ptr<testing::NiceMock<MockIHTTPClient>>;
using MockNiceILogger_sp = std::shared_ptr<testing::NiceMock<MockILogger>>;
using MockNiceIOpenKitComposite_sp = std::shared_ptr<testing::NiceMock<MockIOpenKitComposite>>;
using MockNiceIPRNGenerator_sp = std::shared_ptr<testing::NiceMock<MockIPRNGenerator>>;
using MockNiceISessionIDProvider_sp = std::shared_ptr<testing::NiceMock<MockISessionIDProvider>>;
using MockNiceIThreadIDProvider_sp = std::shared_ptr<testing::NiceMock<MockIThreadIDProvider>>;
using MockNiceITimingProvider_sp = std::shared_ptr<testing::NiceMock<MockITimingProvider>>;
using MockNiceSession_t = testing::NiceMock<MockSessionInternals>;
using MockNiceSession_sp = std::shared_ptr<MockNiceSession_t>;
using MockStrictIBeaconCache_sp = std::shared_ptr<testing::StrictMock<MockIBeaconCache>>;
using OpenKitType_t = core::configuration::OpenKitType;
using UrlEncoding_t = core::util::URLEncoding;
using Utf8String_t = core::UTF8String;
using WebRequestTracer_t = core::objects::WebRequestTracer;

constexpr char APP_ID[] = "appID";
constexpr char APP_NAME[] = "appName";
constexpr int32_t ACTION_ID = 17;
constexpr int32_t SERVER_ID = 1;
constexpr int64_t DEVICE_ID = 456;
constexpr int32_t THREAD_ID = 1234567;
constexpr int32_t SESSION_ID = 73;
constexpr char TRACER_URL[] = "https://localhost";

class BeaconTest : public testing::Test
{
protected:

	MockNiceILogger_sp mockLogger;
	MockNiceIThreadIDProvider_sp mockThreadIdProvider;

	MockStrictIBeaconCache_sp mockBeaconCache;

	MockNiceIPRNGenerator_sp mockRandom;
	MockNiceISessionIDProvider_sp mockSessionIDProvider;
	Configuration_sp configuration;
	MockNiceITimingProvider_sp mockTimingProvider;

	MockNiceIOpenKitComposite_sp mockParent;

	void SetUp()
	{
		mockLogger = MockILogger::createNice();

		mockThreadIdProvider = MockIThreadIDProvider::createNice();
		ON_CALL(*mockThreadIdProvider, getThreadID())
			.WillByDefault(testing::Return(THREAD_ID));

		mockBeaconCache = MockIBeaconCache::createStrict();
		mockRandom = MockIPRNGenerator::createNice();

		mockSessionIDProvider = MockISessionIDProvider::createNice();
		ON_CALL(*mockSessionIDProvider, getNextSessionID())
			.WillByDefault(testing::Return(SESSION_ID));

		mockTimingProvider = MockITimingProvider::createNice();
		mockParent = MockIOpenKitComposite::createNice();
	}

	BeaconBuilder_sp createBeacon()
	{
		return createBeacon(
			core::configuration::DEFAULT_DATA_COLLECTION_LEVEL,
			core::configuration::DEFAULT_CRASH_REPORTING_LEVEL
		);
	}

	BeaconBuilder_sp createBeacon(DataCollectionLevel_t dl, CrashReportingLevel_t cl)
	{
		return createBeacon(dl, cl, DEVICE_ID, Utf8String_t(APP_ID));
	}

	BeaconBuilder_sp createBeacon(DataCollectionLevel_t dl, CrashReportingLevel_t cl, int64_t deviceID, const Utf8String_t& appID)
	{
		auto device = std::make_shared<Device_t>(Utf8String_t(""), Utf8String_t(""), Utf8String_t(""));
		auto beaconConfiguration = MockIBeaconConfiguration::createNice();
		ON_CALL(*beaconConfiguration, getCrashReportingLevel())
			.WillByDefault(testing::Return(cl));
		ON_CALL(*beaconConfiguration, getDataCollectionLevel())
			.WillByDefault(testing::Return(dl));

		configuration = std::make_shared<Configuration_t>(
			device,
			OpenKitType_t::Type::DYNATRACE,
			Utf8String_t(APP_NAME),
			"",
			appID,
			deviceID,
			std::to_string(deviceID).c_str(),
			"",
			mockSessionIDProvider,
			MockISslTrustManager::createNice(),
			MockIBeaconCacheConfiguration::createNice(),
			beaconConfiguration
		);
		configuration->enableCapture();

		return std::make_shared<TestBeaconBuilder>(configuration)
			->with(mockLogger)
			->with(mockBeaconCache)
			->with(mockThreadIdProvider)
			->with(mockTimingProvider)
			->with(mockRandom);
	}
};

TEST_F(BeaconTest, createIDs)
{
	// given
	auto target = createBeacon()->build();

	for (auto i = 1; i < 4; i++)
	{
		// when
		auto id = target->createID();

		// then
		ASSERT_THAT(id, testing::Eq(i));
	}
}

TEST_F(BeaconTest, getCurrentTimestamp)
{
	// with
	int64_t timestamp = 42;
	ON_CALL(*mockTimingProvider, provideTimestampInMilliseconds())
		.WillByDefault(testing::Return(timestamp));

	// expect
	EXPECT_CALL(*mockTimingProvider, provideTimestampInMilliseconds())
		.Times(2);

	// given
	auto target = createBeacon()->build();

	// when
	auto obtained = target->getCurrentTimestamp();

	// then
	ASSERT_THAT(obtained, testing::Eq(timestamp));
}

TEST_F(BeaconTest, createSequenceNumbers)
{
	// given
	auto target = createBeacon()->build();

	for(auto i = 1; i < 4; i++)
	{
		// when
		auto id = target->createSequenceNumber();

		// then
		ASSERT_THAT(id, testing::Eq(i));
	}
}

TEST_F(BeaconTest, createWebRequestTag)
{
	// given
	int32_t sequenceNo = 42;
	auto target = createBeacon()->build();

	// when
	auto obtained = target->createTag(ACTION_ID, sequenceNo);

	// then
	std::stringstream s;
	s << "MT" 										// tag prefix
		<< "_" << protocol::PROTOCOL_VERSION		// protocol version
		<< "_" << SERVER_ID							// server ID
		<< "_" << DEVICE_ID							// device ID
		<< "_" << SESSION_ID						// session number
		<< "_" << APP_ID							// application ID
		<< "_" << ACTION_ID							// parent action ID
		<< "_" << THREAD_ID							// thread ID
		<< "_" << sequenceNo						// sequence number
	;
	ASSERT_THAT(obtained, testing::Eq(s.str()));
}

TEST_F(BeaconTest, createTagEncodesDeviceIDPropperly)
{
	// given
	int32_t deviceId = -42;
	int32_t sequenceNo = 42;
	auto target = createBeacon(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF, deviceId, APP_ID)->build();

	// when
	auto obtained = target->createTag(ACTION_ID, sequenceNo);

	// then
	std::stringstream s;
	s << "MT" 										// tag prefix
		<< "_" << protocol::PROTOCOL_VERSION		// protocol version
		<< "_" << SERVER_ID							// server ID
		<< "_" << deviceId							// device ID
		<< "_" << SESSION_ID						// session number
		<< "_" << APP_ID							// application ID
		<< "_" << ACTION_ID							// parent action ID
		<< "_" << THREAD_ID							// thread ID
		<< "_" << sequenceNo						// sequence number
	;

	ASSERT_THAT(obtained, testing::Eq(s.str()));
}

TEST_F(BeaconTest, createTagUsesEncodedAppID)
{
	// given
	Utf8String_t appID("app_ID_");
	int32_t sequenceNo = 42;
	auto target = createBeacon(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF, DEVICE_ID, appID)->build();

	// when
	auto obtained = target->createTag(ACTION_ID, sequenceNo);

	// then
	std::stringstream s;
	s << "MT" 										// tag prefix
		<< "_" << protocol::PROTOCOL_VERSION		// protocol version
		<< "_" << SERVER_ID							// server ID
		<< "_" << DEVICE_ID							// device ID
		<< "_" << SESSION_ID						// session number
		<< "_" << UrlEncoding_t::urlencode(appID, {'_'}).getStringData()	// application ID
		<< "_" << ACTION_ID							// parent action ID
		<< "_" << THREAD_ID							// thread ID
		<< "_" << sequenceNo						// sequence number
	;

	ASSERT_THAT(obtained, testing::Eq(s.str()));
}

TEST_F(BeaconTest, addValidActionEvent)
{
	// with
	int32_t parentID = 13;
	Utf8String_t actionName("MyAction");
	auto action = MockIActionCommon::createNice();
	ON_CALL(*action, getID())
		.WillByDefault(testing::Return(ACTION_ID));
	ON_CALL(*action, getParentID())
		.WillByDefault(testing::Return(parentID));
	ON_CALL(*action, getName())
		.WillByDefault(testing::ReturnRef(actionName));

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::ACTION)	// event type
		<< "&na=" << actionName.getStringData()		// action name
		<< "&it=" << THREAD_ID						// thread ID
		<< "&ca=" << ACTION_ID						// action ID
		<< "&pa=" << parentID						// parent action ID
		<< "&s0=0"									// action start sequence number
		<< "&t0=0"									// action start time (relative to session start)
		<< "&s1=0"									// action end sequence number
		<< "&t1=0"									// action duration (time from action start to end)
	;
	EXPECT_CALL(*mockBeaconCache, addActionData(
		SESSION_ID,									// session ID
		0,											// action start time
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->addAction(action);
}

TEST_F(BeaconTest, addEndSessionEvent)
{
	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::SESSION_END)	// event type
		<< "&it=" << THREAD_ID						// thread ID
		<< "&pa=0"									// parent action
		<< "&s0=1"									// end sequence number
		<< "&t0=0"									// session end time
	;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		SESSION_ID,									// session ID
		0,											// session end time
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->endSession();
}

TEST_F(BeaconTest, reportValidValueInt)
{
	// with
	Utf8String_t valueName("IntValue");
	int32_t value = 42;

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::VALUE_INT)	// event type
		<< "&na=" << valueName.getStringData()		// name of reported value
		<< "&it=" << THREAD_ID						// thread ID
		<< "&pa=" << ACTION_ID						// parent action
		<< "&s0=1"									// sequence number of reported value
		<< "&t0=0"									// event time since session start
		<< "&vl=" << value							// reported value
	;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		SESSION_ID,									// session ID
		0,											// timestamp when value was reported
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->reportValue(ACTION_ID, valueName, value);
}

TEST_F(BeaconTest, reportValidValueDouble)
{
	// with
	Utf8String_t valueName("DoubleValue");
	double value = 3.14;

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::VALUE_DOUBLE)	// event type
		<< "&na=" << valueName.getStringData()		// name of reported value
		<< "&it=" << THREAD_ID						// thread ID
		<< "&pa=" << ACTION_ID						// parent action
		<< "&s0=1"									// sequence number of reported value
		<< "&t0=0"									// event time since session start
		<< "&vl=" << std::to_string(value)			// reported value
	;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		SESSION_ID,									// session ID
		0,											// timestamp when value was reported
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->reportValue(ACTION_ID, valueName, value);
}

TEST_F(BeaconTest, reportValidValueString)
{
	// with
	Utf8String_t valueName("StringValue");
	Utf8String_t value("HelloWorld");

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::VALUE_STRING)	// event type
		<< "&na=" << valueName.getStringData()		// name of reported value
		<< "&it=" << THREAD_ID						// thread ID
		<< "&pa=" << ACTION_ID						// parent action
		<< "&s0=1"									// sequence number of reported value
		<< "&t0=0"									// event time since session start
		<< "&vl=" << value.getStringData()			// reported value
	;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		SESSION_ID,									// session ID
		0,											// timestamp when value was reported
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->reportValue(ACTION_ID, valueName, value);
}

TEST_F(BeaconTest, reportValueStringWithValueNull)
{
	// with
	Utf8String_t valueName("StringValue");
	Utf8String_t value(nullptr);

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::VALUE_STRING)	// event type
		<< "&na=" << valueName.getStringData()		// name of reported value
		<< "&it=" << THREAD_ID						// thread ID
		<< "&pa=" << ACTION_ID						// parent action
		<< "&s0=1"									// sequence number of reported value
		<< "&t0=0"									// event time since session start
	;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		SESSION_ID,									// session ID
		0,											// timestamp when value was reported
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->reportValue(ACTION_ID, valueName, value);
}

TEST_F(BeaconTest, reportValueStringWithValueNullAndNameNull)
{
	// with
	Utf8String_t valueName(nullptr);
	Utf8String_t value(nullptr);

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::VALUE_STRING)	// event type
		<< "&it=" << THREAD_ID						// thread ID
		<< "&pa=" << ACTION_ID						// parent action
		<< "&s0=1"									// sequence number of reported value
		<< "&t0=0"									// event time since session start
	;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		SESSION_ID,									// session ID
		0,											// timestamp when value was reported
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->reportValue(ACTION_ID, valueName, value);
}

TEST_F(BeaconTest, reportValidEvent)
{
	// with
	Utf8String_t eventName("someEvent");

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::NAMED_EVENT)	// event type
		<< "&na=" << eventName.getStringData()		// name of event
		<< "&it=" << THREAD_ID						// thread ID
		<< "&pa=" << ACTION_ID						// parent action
		<< "&s0=1"									// sequence number of reported event
		<< "&t0=0"									// event time since session start
	;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		SESSION_ID,									// session ID
		0,											// event timestamp
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->reportEvent(ACTION_ID, eventName);
}

TEST_F(BeaconTest, reportEventWithNameNull)
{
	// with
	Utf8String_t eventName(nullptr);

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::NAMED_EVENT)	// event type
		<< "&it=" << THREAD_ID						// thread ID
		<< "&pa=" << ACTION_ID						// parent action
		<< "&s0=1"									// sequence number of reported event
		<< "&t0=0"									// event time since session start
	;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		SESSION_ID,									// session ID
		0,											// event timestamp
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->reportEvent(ACTION_ID, eventName);
}

TEST_F(BeaconTest, reportError)
{
	// with
	Utf8String_t errorName("someEvent");
	int32_t errorCode = -123;
	Utf8String_t reason("someReason");

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::FAILURE_ERROR)	// event type
		<< "&na=" << errorName.getStringData()		// name of error event
		<< "&it=" << THREAD_ID						// thread ID
		<< "&pa=" << ACTION_ID						// parent action
		<< "&s0=1"									// sequence number of reported error
		<< "&t0=0"									// event time since session start
		<< "&ev=" << errorCode						// reported error value
		<< "&rs=" << reason.getStringData()			// reported reason
	;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		SESSION_ID,									// session ID
		0,											// timestamp when error was reported
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->reportError(ACTION_ID, errorName, errorCode, reason);
}

TEST_F(BeaconTest, reportErrorWithoutName)
{
	// with
	Utf8String_t errorName(nullptr);
	int32_t errorCode = -123;
	Utf8String_t reason(nullptr);

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::FAILURE_ERROR)	// event type
		<< "&it=" << THREAD_ID						// thread ID
		<< "&pa=" << ACTION_ID						// parent action
		<< "&s0=1"									// sequence number of reported error
		<< "&t0=0"									// event time since session start
		<< "&ev=" << errorCode						// reported error value
	;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		SESSION_ID,									// session ID
		0,											// timestamp when error was reported
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->reportError(ACTION_ID, errorName, errorCode, reason);
}

TEST_F(BeaconTest, reportValidCrash)
{
	// with
	Utf8String_t errorName("someEvent");
	Utf8String_t reason("someReason");
	Utf8String_t stacktrace("someStacktrace");

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::FAILURE_CRASH)	// event type
		<< "&na=" << errorName.getStringData()		// reported crash name
		<< "&it=" << THREAD_ID						// thread ID
		<< "&pa=0"									// parent action
		<< "&s0=1"									// sequence number of reported crash
		<< "&t0=0"									// event time since session start
		<< "&rs=" << reason.getStringData()			// reported reason
		<< "&st=" << stacktrace.getStringData()		// reported stacktrace
	;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		SESSION_ID,									// session ID
		0,											// timestamp when crash was reported
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->reportCrash(errorName, reason, stacktrace);
}

TEST_F(BeaconTest, reportCrashWithDetailsNull)
{
	// with
	Utf8String_t errorName("someEvent");
	Utf8String_t reason(nullptr);
	Utf8String_t stacktrace(nullptr);

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::FAILURE_CRASH)	// event type
		<< "&na=" << errorName.getStringData()		// reported crash name
		<< "&it=" << THREAD_ID						// thread ID
		<< "&pa=0"									// parent action
		<< "&s0=1"									// sequence number of reported crash
		<< "&t0=0"									// event time since session start
	;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		SESSION_ID,									// session ID
		0,											// timestamp when crash was reported
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->reportCrash(errorName, reason, stacktrace);
}

TEST_F(BeaconTest, addWebRequest)
{
	// given
	int32_t numBytesSent = 13;
	int32_t numBytesReceived = 14;
	int32_t responseCode = 15;

	auto target = createBeacon()->build();

	auto tracer = MockIWebRequestTracerInternals::createNice();
	ON_CALL(*tracer, getURL())
		.WillByDefault(testing::Return(TRACER_URL));
	ON_CALL(*tracer, getBytesSent())
		.WillByDefault(testing::Return(numBytesSent));
	ON_CALL(*tracer, getBytesReceived())
		.WillByDefault(testing::Return(numBytesReceived));
	ON_CALL(*tracer, getResponseCode())
		.WillByDefault(testing::Return(responseCode));

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::WEBREQUEST)	// event type
		<< "&na=" << UrlEncoding_t::urlencode(TRACER_URL, {'_'}).getStringData() // tracer url
		<< "&it=" << THREAD_ID						// thread ID
		<< "&pa=" << ACTION_ID						// parent action
		<< "&s0=0"									// web request start sequence number
		<< "&t0=0"									// web request start time (since session start)
		<< "&s1=0"									// web request end sequence number
		<< "&t1=0"									// web request end time (relative to start time)
		<< "&bs=" << numBytesSent					// number of bytes sent
		<< "&br=" << numBytesReceived				// number of bytes received
		<< "&rc=" << responseCode					// response code
	;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		SESSION_ID,									// session ID
		0,											// timestamp when web request tracer was reported
		testing::Eq(s.str())
	)).Times(1);

	// when
	target->addWebRequest(ACTION_ID, tracer);
}

TEST_F(BeaconTest, addUserIdentifyEvent)
{
	// with
	Utf8String_t userID("myTestUser");

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::IDENTIFY_USER)	// event type
		<< "&na=" << userID.getStringData()			// reported user ID
		<< "&it=" << THREAD_ID						// thread ID
		<< "&pa=0"									// parent action
		<< "&s0=1"									// identify user sequence number
		<< "&t0=0"									// event timestamp since session start
	;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		SESSION_ID,									// session ID
		0,											// timestamp of the user identifaction event
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->identifyUser(userID);
}

TEST_F(BeaconTest, addUserIdentifyWithNullUserIDEvent)
{
	// with
	Utf8String_t userID(nullptr);

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::IDENTIFY_USER)	// event type
		<< "&it=" << THREAD_ID						// thread ID
		<< "&pa=0"									// parent action
		<< "&s0=1"									// identify user sequence number
		<< "&t0=0"									// event timestamp since session start
	;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		SESSION_ID,									// session ID
		0,											// timestamp of the user identification event
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->identifyUser(userID);
}

TEST_F(BeaconTest, canAddSentBytesToWebRequestTracer)
{
	// with
	Utf8String_t url("https://localhost");
	int32_t numBytesSent = 12321;

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::WEBREQUEST)	// event type
		<< "&na=" << UrlEncoding_t::urlencode(url, {'_'}).getStringData() // reported URL
		<< "&it=" << THREAD_ID						// thread ID
		<< "&pa=0"									// parent action
		<< "&s0=1"									// web request start sequence number
		<< "&t0=0"									// web request start timestamp (relative to session start)
		<< "&s1=2"									// web request end sequence number
		<< "&t1=0"									// web request end timestamp (relative to start time)
		<< "&bs=" << numBytesSent					// number of bytes sent
	;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		SESSION_ID,									// session ID
		0,											// timestamp of web request tracer event
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	auto tracer = std::make_shared<WebRequestTracer_t>(mockLogger, mockParent, target, url);

	// when
	tracer->start()->setBytesSent(numBytesSent)->stop(-1); // will add the web request to the beacon
}

TEST_F(BeaconTest, canAddSentBytesValueZeroToWebRequestTracer)
{
	// with
	Utf8String_t url("https://localhost");
	int32_t numBytesSent = 0;

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::WEBREQUEST)	// event type
		<< "&na=" << UrlEncoding_t::urlencode(url, {'_'}).getStringData() // reported URL
		<< "&it=" << THREAD_ID						// thread ID
		<< "&pa=0"									// parent action
		<< "&s0=1"									// web request start sequence number
		<< "&t0=0"									// web request start timestamp (relative to session start)
		<< "&s1=2"									// web request end sequence number
		<< "&t1=0"									// web request end timestamp (relative to start time)
		<< "&bs=" << numBytesSent					// number of bytes sent
	;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		SESSION_ID,									// session ID
		0,											// timestamp of web request tracer event
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	auto tracer = std::make_shared<WebRequestTracer_t>(mockLogger, mockParent, target, url);

	// when
	tracer->start()->setBytesSent(numBytesSent)->stop(-1); // will add the web request to the beacon
}

TEST_F(BeaconTest, cannotAddSentBytesWithInvalidValueSmallerZeroToWebRequestTracer)
{
	// with
	Utf8String_t url("https://localhost");
	int32_t numBytesSent = -5;

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::WEBREQUEST)	// event type
		<< "&na=" << UrlEncoding_t::urlencode(url, {'_'}).getStringData() // reported URL
		<< "&it=" << THREAD_ID						// thread ID
		<< "&pa=0"									// parent action
		<< "&s0=1"									// web request start sequence number
		<< "&t0=0"									// web request start timestamp (relative to session start)
		<< "&s1=2"									// web request end sequence number
		<< "&t1=0"									// web request end timestamp (relative to start time)
	;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		SESSION_ID,									// session ID
		0,											// timestamp of web request tracer event
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	auto tracer = std::make_shared<WebRequestTracer_t>(mockLogger, mockParent, target, url);

	// when
	tracer->start()->setBytesSent(numBytesSent)->stop(-1); // will add the web request to the beacon
}

TEST_F(BeaconTest, canAddReceivedBytesToWebRequestTracer)
{
	// with
	Utf8String_t url("https://localhost");
	int32_t numBytesReceived = 12321;

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::WEBREQUEST)	// event type
		<< "&na=" << UrlEncoding_t::urlencode(url, {'_'}).getStringData() // reported URL
		<< "&it=" << THREAD_ID						// thread ID
		<< "&pa=0"									// parent action
		<< "&s0=1"									// web request start sequence number
		<< "&t0=0"									// web request start timestamp (relative to session start)
		<< "&s1=2"									// web request end sequence number
		<< "&t1=0"									// web request end timestamp (relative to start time)
		<< "&br=" << numBytesReceived				// number of received bytes
	;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		SESSION_ID,									// session ID
		0,											// timestamp of web request tracer event
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	auto tracer = std::make_shared<WebRequestTracer_t>(mockLogger, mockParent, target, url);

	// when
	tracer->start()->setBytesReceived(numBytesReceived)->stop(-1); // will add the web request to the beacon
}

TEST_F(BeaconTest, cannAddReceivedBytesValueZeroToWebRequestTracer)
{
	// with
	Utf8String_t url("https://localhost");
	int32_t numBytesReceived = 0;

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::WEBREQUEST)	// event type
		<< "&na=" << UrlEncoding_t::urlencode(url, {'_'}).getStringData() // reported URL
		<< "&it=" << THREAD_ID						// thread ID
		<< "&pa=0"									// parent action
		<< "&s0=1"									// web request start sequence number
		<< "&t0=0"									// web request start timestamp (relative to session start)
		<< "&s1=2"									// web request end sequence number
		<< "&t1=0"									// web request end timestamp (relative to start time)
		<< "&br=" << numBytesReceived				// number of received bytes
	;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		SESSION_ID,									// session ID
		0,											// timestamp of web request tracer event
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	auto tracer = std::make_shared<WebRequestTracer_t>(mockLogger, mockParent, target, url);

	// when
	tracer->start()->setBytesReceived(numBytesReceived)->stop(-1); // will add the web request to the beacon
}

TEST_F(BeaconTest, cannotAddReceivedBytesWithInvalidValueSmallerZeroToWebRequestTracer)
{
	// with
	Utf8String_t url("https://localhost");
	int32_t numBytesReceived = -5;

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::WEBREQUEST)	// event type
		<< "&na=" << UrlEncoding_t::urlencode(url, {'_'}).getStringData() // reported URL
		<< "&it=" << THREAD_ID						// thread ID
		<< "&pa=0"									// parent action
		<< "&s0=1"									// web request start sequence number
		<< "&t0=0"									// web request start timestamp (relative to session start)
		<< "&s1=2"									// web request end sequence number
		<< "&t1=0"									// web request end timestamp (relative to start time)
	;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		SESSION_ID,									// session ID
		0,											// timestamp of web request tracer event
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	auto tracer = std::make_shared<WebRequestTracer_t>(mockLogger, mockParent, target, url);

	// when
	tracer->start()->setBytesReceived(numBytesReceived)->stop(-1); // will add the web request to the beacon
}

TEST_F(BeaconTest, canAddBothSentBytesAndReceivedBytesToWebRequestTracer)
{
	// with
	Utf8String_t url("https://localhost");
	int32_t numBytesSent = 12321;
	int32_t numBytesReceived = 123;

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::WEBREQUEST)	// event type
		<< "&na=" << UrlEncoding_t::urlencode(url, {'_'}).getStringData() // reported URL
		<< "&it=" << THREAD_ID						// thread ID
		<< "&pa=0"									// parent action
		<< "&s0=1"									// web request start sequence number
		<< "&t0=0"									// web request start timestamp (relative to session start)
		<< "&s1=2"									// web request end sequence number
		<< "&t1=0"									// web request end timestamp (relative to start time)
		<< "&bs=" << numBytesSent					// number of received bytes
		<< "&br=" << numBytesReceived				// number of received bytes
	;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		SESSION_ID,									// session ID
		0,											// timestamp of web request tracer event
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	auto tracer = std::make_shared<WebRequestTracer_t>(mockLogger, mockParent, target, url);

	// when
	tracer->start()
		->setBytesSent(numBytesSent)
		->setBytesReceived(numBytesReceived)
		->stop(-1); // will add the web request to the beacon
}

TEST_F(BeaconTest, canAddActionIfDataSendingIsAllowed)
{
	// with
	Utf8String_t actionName("action");
	auto action = MockIActionCommon::createNice();
	ON_CALL(*action, getName())
		.WillByDefault(testing::ReturnRef(actionName));

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::ACTION)	// event type
		<< "&na=" << actionName.getStringData()		// action name
		<< "&it=" << THREAD_ID						// thread ID
		<< "&ca=0"									// action ID
		<< "&pa=0"									// parent action
		<< "&s0=0"									// action start sequence number
		<< "&t0=0"									// action start time (relative to session start)
		<< "&s1=0"									// action end sequence number
		<< "&t1=0"									// action end time (relative to start time)
	;
	EXPECT_CALL(*mockBeaconCache, addActionData(
		SESSION_ID,									// session ID
		0,											// timestamp of web request tracer event
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->addAction(action);
}

TEST_F(BeaconTest, cannotAddActionIfCapturingIsDisabled)
{
	// with
	Utf8String_t actionName("action");
	auto action = MockIActionCommon::createNice();
	ON_CALL(*action, getName())
		.WillByDefault(testing::ReturnRef(actionName));

	// given
	auto target = createBeacon()->build();
	configuration->disableCapture();

	// when, expect no interaction with beacon cache
	target->addAction(action);
}

TEST_F(BeaconTest, canHandleNoDataInBeaconSend)
{
	// expect
	EXPECT_CALL(*mockBeaconCache, getNextBeaconChunk(SESSION_ID, testing::_, testing::_, testing::_))
		.Times(1);

	// given
	auto httpClient = MockIHTTPClient::createNice();
	auto httpClientProvider = MockIHTTPClientProvider::createNice();
	ON_CALL(*httpClientProvider, createClient(testing::_, testing::_))
		.WillByDefault(testing::Return(httpClient));

	auto target = createBeacon()->build();

	// when
	auto obtained = target->send(httpClientProvider);

	// then
	ASSERT_THAT(obtained, testing::IsNull());
}

TEST_F(BeaconTest, sendValidData)
{
	// with
	Utf8String_t ipAddress("127.0.0.1");
	auto beaconCache = std::make_shared<BeaconCache_t>(mockLogger);
	int32_t responseCode = 200;

	auto statusResponse = MockIStatusResponse ::createNice();
	ON_CALL(*statusResponse, getResponseCode())
		.WillByDefault(testing::Return(responseCode));

	auto httpClient = MockIHTTPClient::createNice();
	ON_CALL(*httpClient, sendBeaconRequest(testing::_, testing::_))
		.WillByDefault(testing::Return(statusResponse));

	auto httpClientProvider = MockIHTTPClientProvider::createNice();
	ON_CALL(*httpClientProvider, createClient(testing::_, testing::_))
		.WillByDefault(testing::Return(httpClient));

	// expect
	EXPECT_CALL(*httpClient, sendBeaconRequest(testing::Eq(ipAddress), testing::_))
		.Times(1);

	auto target = createBeacon()
			->withIpAddress(ipAddress)
			->with(beaconCache)
			->build();

	// when
	target->reportCrash("errorName", "errorReason", "errorStackTrace");
	auto obtained = target->send(httpClientProvider);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getResponseCode(), testing::Eq(responseCode));
}

TEST_F(BeaconTest, clearDataForwardsCallToBeaconCache)
{
	// expect
	EXPECT_CALL(*mockBeaconCache, deleteCacheEntry(SESSION_ID))
		.Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->clearData();
}

TEST_F(BeaconTest, noSessionIsAddedIfCapturingDisabled)
{
	// given
	auto target = createBeacon()->build();
	configuration->disableCapture();

	// when, expect no interaction with beacon cache
	target->endSession();
}

TEST_F(BeaconTest, noActionIsAddedIfCapturingDisabled)
{
	// with
	auto action = MockIActionCommon::createNice();
	ON_CALL(*action, getID())
		.WillByDefault(testing::Return(ACTION_ID));

	// given
	auto target = createBeacon()->build();
	configuration->disableCapture();

	// when, expect no interaction with beacon cache
	target->addAction(action);
}

TEST_F(BeaconTest, noIntValueIsReportedIfCapturingIsDisabled)
{
	// with
	int32_t value = 42;
	Utf8String_t valueName("IntValue");

	// given
	auto target = createBeacon()->build();
	configuration->disableCapture();

	// when, expect no interaction with beacon cache
	target->reportValue(ACTION_ID, valueName, value);
}

TEST_F(BeaconTest, noDoubleValueIsReportedIfCapturingIsDisabled)
{
	// with
	double value = 42.1337;
	Utf8String_t valueName("DoubleValue");

	// given
	auto target = createBeacon()->build();
	configuration->disableCapture();

	// when, expect no interaction with beacon cache
	target->reportValue(ACTION_ID, valueName, value);
}

TEST_F(BeaconTest, noStringValueIsReportedIfCapturingIsDisabled)
{
	// with
	Utf8String_t value("HelloWorld");
	Utf8String_t valueName("StringValue");

	// given
	auto target = createBeacon()->build();
	configuration->disableCapture();

	// when, expect no interaction with beacon cache
	target->reportValue(ACTION_ID, valueName, value);
}

TEST_F(BeaconTest, noEventIsReportedIfCapturingDisabled)
{
	// with
	Utf8String_t eventName("event name");

	// given
	auto target = createBeacon()->build();
	configuration->disableCapture();

	// when, expect no interaction with beacon cache
	target->reportEvent(ACTION_ID, eventName);
}

TEST_F(BeaconTest, noErrorIsReportedIfCapturingDisabled)
{
	// with
	Utf8String_t eventName("event name");
	int32_t errorCode = 123;
	Utf8String_t reason("error reason");

	// given
	auto target = createBeacon()->build();
	configuration->disableCapture();

	// when, expect no interaction with beacon cache
	target->reportError(ACTION_ID, eventName, errorCode, reason);
}

TEST_F(BeaconTest, noCrashIsReportedIfCapturingIsDisabled)
{
	// with
	Utf8String_t eventName("event name");
	Utf8String_t reason("some reason");
	Utf8String_t stacktrace("some stacktrace");

	// given
	auto target = createBeacon()->build();
	configuration->disableCapture();

	// when, expect no interaction with beacon cache
	target->reportCrash(eventName, reason, stacktrace);
}

TEST_F(BeaconTest, noWebRequestIsReportedIfCapturingDisabled)
{
	// given
	auto target = createBeacon()
			->build();
	auto tracer = MockIWebRequestTracerInternals::createNice();

	configuration->disableCapture();

	// when, expect no interaction with beacon cache
	target->addWebRequest(ACTION_ID, tracer);
}

TEST_F(BeaconTest, noUserIdentificationIsReportedIfCapturingDisabled)
{
	// with
	Utf8String_t userID("jane.doe@acmoe.com");

	// given
	auto target = createBeacon()->build();
	configuration->disableCapture();

	// when, expect no interaction with beacon cache
	target->identifyUser(userID);
}

TEST_F(BeaconTest, noWebRequestIsReportedForDataCollectionLevel0)
{
	// given
	auto target = createBeacon(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF)->build();
	auto tracer = MockIWebRequestTracerInternals::createNice();

	// when, expect no interaction with beacon cache
	target->addWebRequest(ACTION_ID, tracer);
}

TEST_F(BeaconTest, webRequestIsReportedForDataCollectionLevel1)
{
	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(SESSION_ID, 0, testing::_))
		.Times(1);

	//given
	auto target = createBeacon(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF)->build();
	auto mockWebRequestTracer = MockIWebRequestTracerInternals::createNice();

	// when
	target->addWebRequest(ACTION_ID, mockWebRequestTracer);
}

TEST_F(BeaconTest, webRequestIsReportedForDataCollectionLevel2)
{
	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(SESSION_ID, 0, testing::_))
		.Times(1);

	//given
	auto target = createBeacon(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF)->build();
	auto mockWebRequestTracer = MockIWebRequestTracerInternals::createNice();

	// when
	target->addWebRequest(ACTION_ID, mockWebRequestTracer);
}

TEST_F(BeaconTest, beaconReturnsEmptyTagForDataCollectionLevel0)
{
	//given
	auto target = createBeacon(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF)->build();

	// when
	auto obtained = target->createTag(ACTION_ID, 1);

	//then
	ASSERT_THAT(obtained, testing::Eq(""));
}

TEST_F(BeaconTest, beaconReturnsValidTagForDataCollectionLevel1)
{
	// with
	int32_t sequenceNo = 73;
	int64_t deviceId = 37;
	int64_t ignoredDeviceId = 999;
	ON_CALL(*mockRandom, nextInt64(testing::_))
		.WillByDefault(testing::Return(deviceId));

	auto target = createBeacon(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF, ignoredDeviceId, APP_ID)->build();

	// when
	auto tagString = target->createTag(ACTION_ID, sequenceNo);

	//then
	std::stringstream str;
	str << "MT"						// tag prefix
		<< "_" << protocol::PROTOCOL_VERSION // protocol version
		<< "_" << SERVER_ID			// server ID
		<< "_" << deviceId			// device ID
		<< "_1"						// session number (must always be 1 for data collection level performance)
		<< "_" << APP_ID			// application ID
		<< "_" << ACTION_ID			// parent action ID
		<< "_" << THREAD_ID			// thread ID
		<< "_" << sequenceNo		// sequence number
	;

	ASSERT_THAT(tagString.getStringData(), testing::Eq(str.str()));
}

TEST_F(BeaconTest, createTagReturnsTagStringForDataCollectionLevel2)
{
	//given
	int64_t deviceId = 37;
	int32_t sequenceNo = 73;
	auto target = createBeacon(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF, deviceId, APP_ID)->build();

	// when
	auto tagString = target->createTag(ACTION_ID, sequenceNo);

	// then
	std::stringstream str;
	str << "MT"						// tag prefix
		<< "_" << protocol::PROTOCOL_VERSION // protocol version
		<< "_" << SERVER_ID			// server ID
		<< "_" << deviceId			// device ID
		<< "_" << SESSION_ID		// session number
		<< "_" << APP_ID			// application ID
		<< "_" << ACTION_ID			// parent action ID
		<< "_" << THREAD_ID			// thread ID
		<< "_" << sequenceNo		// sequence number
	;
	ASSERT_THAT(tagString.getStringData(), str.str());
}

TEST_F(BeaconTest, deviceIDIsRandomizedOnDataCollectionLevel0)
{
	// with
	int64_t deviceId = 1337;
	ON_CALL(*mockRandom, nextInt64(testing::_))
		.WillByDefault(testing::Return(deviceId));

	// expect
	EXPECT_CALL(*mockRandom, nextInt64(testing::_))
		.Times(1);

	// given
	auto target = createBeacon(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF)->build();

	// when
	auto obtained = target->getDeviceID();

	// then
	ASSERT_THAT(obtained, testing::Eq(deviceId));
}

TEST_F(BeaconTest, deviceIDIsRandomizedOnDataCollectionLevel1)
{
	// with
	int64_t deviceId = 1337;
	ON_CALL(*mockRandom, nextInt64(testing::_))
		.WillByDefault(testing::Return(deviceId));

	// expect
	EXPECT_CALL(*mockRandom, nextInt64(testing::_))
		.Times(1);

	// given
	auto target = createBeacon(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF)->build();

	// when
	auto obtained = target->getDeviceID();

	// then
	ASSERT_THAT(obtained, testing::Eq(deviceId));
}

TEST_F(BeaconTest, givenDeviceIDIsUsedOnDataCollectionLevel2)
{
	// with
	auto mockRandomStrict = MockIPRNGenerator::createStrict();

	// given
	auto target = createBeacon(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF)
			->with(mockRandomStrict)
			->build();

	// when
	auto obtained = target->getDeviceID();

	// then
	ASSERT_THAT(obtained, testing::Eq(DEVICE_ID));
}

TEST_F(BeaconTest, randomDeviceIDCannotBeNegativeOnDataCollectionLevel0)
{
	// given
	auto target = createBeacon(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF)->build();

	// when
	auto obtained = target->getDeviceID();

	// then
	EXPECT_THAT(obtained, testing::AllOf(testing::Ge(int64_t(0)), testing::Lt(std::numeric_limits<int64_t>::max())));
}

TEST_F(BeaconTest, randomDeviceIDCannotBeNegativeOnDataCollectionLevel1)
{
	// given
	auto target = createBeacon(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF)->build();

	// when
	auto obtained = target->getDeviceID();

	// then
	EXPECT_THAT(obtained, testing::AllOf(testing::Ge(int64_t(0)), testing::Lt(std::numeric_limits<int64_t>::max())));
}

TEST_F(BeaconTest, sessionIDIsAlwaysValue1OnDataCollectionLevel0)
{
	// given
	auto target = createBeacon(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF)->build();

	// when
	auto obtained = target->getSessionNumber();

	// then
	ASSERT_THAT(obtained, testing::Eq(1));
}

TEST_F(BeaconTest, sessionIDIsAlwaysValue1OnDataCollectionLevel1)
{
	//given
	auto target = createBeacon(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF)->build();

	// when
	auto obtained = target->getSessionNumber();

	// then
	ASSERT_THAT(obtained, testing::Eq(1));
}

TEST_F(BeaconTest, sessionIDIsValueFromSessionIDProviderOnDataCollectionLevel2)
{
	// expect
	EXPECT_CALL(*mockSessionIDProvider, getNextSessionID())
		.Times(1);

	// given
	auto target = createBeacon(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF)->build();

	// when
	auto obtained = target->getSessionNumber();

	// then
	ASSERT_THAT(obtained, testing::Eq(SESSION_ID));
}

TEST_F(BeaconTest, actionNotReportedForDataCollectionLevel0)
{
	// with
	auto actionMock = MockIActionCommon::createStrict();

	// given
	auto target = createBeacon(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF)->build();

	// when, expect no interaction with beacon cache
	target->addAction(actionMock);
}

TEST_F(BeaconTest, actionReportedForDataCollectionLevel1)
{
	// with
	auto actionMock = MockIActionCommon::createNice();

	// expect
	EXPECT_CALL(*mockBeaconCache, addActionData(testing::_, testing::_, testing::_))
		.Times(1);
	EXPECT_CALL(*actionMock, getID())
		.Times(1);

	//given
	auto target = createBeacon(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF)
			->build();

	// when
	target->addAction(actionMock);
}

TEST_F(BeaconTest, actionReportedForDataCollectionLevel2)
{
	// with
	auto actionMock = MockIActionCommon::createNice();

	// expect
	EXPECT_CALL(*mockBeaconCache, addActionData(testing::_, testing::_, testing::_))
		.Times(1);
	EXPECT_CALL(*actionMock, getID())
		.Times(1);

	//given
	auto target = createBeacon(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF)
			->build();

	// when
	target->addAction(actionMock);
}

TEST_F(BeaconTest, sessionNotReportedForDataCollectionLevel0)
{
	//given
	auto target = createBeacon(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF)->build();

	// when, expect no interaction with beacon cache
	target->endSession();
}

TEST_F(BeaconTest, sessionReportedForDataCollectionLevel1)
{
	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(1);

	//given
	auto target = createBeacon(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF)->build();

	// when
	target->endSession();
}

TEST_F(BeaconTest, sessionReportedForDataCollectionLevel2)
{
	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(1);

	//given
	auto target = createBeacon(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF)->build();

	// when
	target->endSession();
}

TEST_F(BeaconTest, identifyUserNotAllowedToReportOnDataCollectionLevel0)
{
	//given
	auto target = createBeacon(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF)->build();

	// when, expect no interaction with beacon cache
	target->identifyUser("testUser");
}

TEST_F(BeaconTest, identifyUserNotAllowedToReportOnDataCollectionLevel1)
{
	//given
	auto target = createBeacon(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF)->build();

	// when, expect no interaction with beacon cache
	target->identifyUser("testUser");
}

TEST_F(BeaconTest, identifyUserReportsOnDataCollectionLevel2)
{
	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(1);

	//given
	auto target = createBeacon(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF)->build();

	// when
	target->identifyUser(Utf8String_t("testUser"));
}

TEST_F(BeaconTest, reportErrorDoesNotReportOnDataCollectionLevel0)
{
	// with
	Utf8String_t eventName("error name");
	int32_t errorCode = 132;
	Utf8String_t reason("error reason");

	//given
	auto target = createBeacon(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF)->build();

	// when, expect no interaction with beacon cache
	target->reportError(ACTION_ID, eventName, errorCode, reason);
}

TEST_F(BeaconTest, reportErrorDoesReportOnDataCollectionLevel1)
{
	// with
	Utf8String_t eventName("error name");
	int32_t errorCode = 132;
	Utf8String_t reason("error reason");

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(1);

	//given
	auto target = createBeacon(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF)->build();

	// when
	target->reportError(ACTION_ID, eventName, errorCode, reason);
}

TEST_F(BeaconTest, reportErrorDoesReportOnDataCollectionLevel2)
{
	// with
	Utf8String_t eventName("error name");
	int32_t errorCode = 132;
	Utf8String_t reason("error reason");

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(1);

	//given
	auto target = createBeacon(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF)
			->build();

	// when
	target->reportError(ACTION_ID, eventName, errorCode, reason);
}

TEST_F(BeaconTest, reportCrashDoesNotReportOnDataCollectionLevel0)
{
	// with
	Utf8String_t eventName("crash name");
	Utf8String_t reason("some reason");
	Utf8String_t stacktrace("in some dark code segment");

	//given
	auto target = createBeacon(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF)->build();

	// when, expect no interaction with beacon cache
	target->reportCrash(eventName, reason, stacktrace);
}

TEST_F(BeaconTest, reportCrashDoesNotReportOnDataCollectionLevel1)
{
	// with
	Utf8String_t eventName("crash name");
	Utf8String_t reason("some reason");
	Utf8String_t stacktrace("in some dark code segment");

	//given
	auto target = createBeacon(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OPT_OUT_CRASHES)->build();

	// when, expect no interaction with beacon cache
	target->reportCrash(eventName, reason, stacktrace);
}

TEST_F(BeaconTest, reportCrashDoesReportOnCrashReportingLevel2)
{
	// with
	Utf8String_t eventName("crash name");
	Utf8String_t reason("some reason");
	Utf8String_t stacktrace("in some dark code segment");

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(1);

	//given
	auto target = createBeacon(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OPT_IN_CRASHES)->build();

	// when
	target->reportCrash(eventName, reason, stacktrace);
}

TEST_F(BeaconTest, intValueNotReportedForDataCollectionLevel0)
{
	// with
	Utf8String_t valueName("IntValue");
	int32_t value = 42;

	//given
	auto target = createBeacon(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF)->build();

	// when, expect no interaction with beacon cache
	target->reportValue(1, valueName, value);
}

TEST_F(BeaconTest, intValueNotReportedForDataCollectionLevel1)
{
	// with
	Utf8String_t valueName("IntValue");
	int32_t value = 42;

	//given
	auto target = createBeacon(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF)->build();

	// when, expect no interaction with beacon cache
	target->reportValue(1, valueName, value);
}


TEST_F(BeaconTest, intValueReportedForDataCollectionLevel2)
{
	// with
	Utf8String_t valueName("IntValue");
	int32_t value = 42;

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(1);

	//given
	auto target = createBeacon(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF)
			->build();

	// when
	target->reportValue(1, valueName, value);
}

TEST_F(BeaconTest, doubleValueNotReportedForDataCollectionLevel0)
{
	// with
	Utf8String_t valueName("DoubleValue");
	double value = 42.1337;

	//given
	auto target = createBeacon(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF)->build();

	// when, expect no interaction with beacon cache
	target->reportValue(1, valueName, value);
}

TEST_F(BeaconTest, doubleValueNotReportedForDataCollectionLevel1)
{
	// with
	Utf8String_t valueName("DoubleValue");
	double value = 42.1337;

	//given
	auto target = createBeacon(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF)->build();

	// when, expect no interaction with beacon cache
	target->reportValue(1, valueName, value);
}


TEST_F(BeaconTest, doubleValueReportedForDataCollectionLevel2)
{
	// with
	Utf8String_t valueName("DoubleValue");
	double value = 42.1337;

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(1);

	//given
	auto target = createBeacon(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF)
			->build();

	// when
	target->reportValue(1, valueName, value);
}

TEST_F(BeaconTest, stringValueNotReportedForDataCollectionLevel0)
{
	// with
	Utf8String_t valueName("StringValue");
	Utf8String_t value("HelloWorld");

	//given
	auto target = createBeacon(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF)->build();

	// when, expect no interaction with beacon cache
	target->reportValue(1, valueName, value);
}

TEST_F(BeaconTest, stringValueNotReportedForDataCollectionLevel1)
{
	// with
	Utf8String_t valueName("StringValue");
	Utf8String_t value("HelloWorld");

	//given
	auto target = createBeacon(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF)->build();

	// when, expect no interaction with beacon cache
	target->reportValue(1, valueName, value);
}


TEST_F(BeaconTest, stringValueReportedForDataCollectionLevel2)
{
	// with
	Utf8String_t valueName("DoubleValue");
	Utf8String_t value("HelloWorld");

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(1);

	//given
	auto target = createBeacon(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF)
			->build();

	// when
	target->reportValue(1, valueName, value);
}

TEST_F(BeaconTest, namedEventNotReportedForDataCollectionLevel0)
{
	// with
	Utf8String_t eventName("some event");

	//given
	auto target = createBeacon(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF)->build();

	// when, expect no interaction with beacon cache
	target->reportEvent(ACTION_ID, eventName);
}

TEST_F(BeaconTest, namedEventNotReportedForDataCollectionLevel1)
{
	// with
	Utf8String_t eventName("some event");

	//given
	auto target = createBeacon(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF)
			->build();

	// when, expect no interaction with beacon cache
	target->reportEvent(ACTION_ID, eventName);
}


TEST_F(BeaconTest, namedEventNotReportedForDataCollectionLevel2)
{
	// with
	Utf8String_t eventName("some event");

	//given
	auto target = createBeacon(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF)
			->build();

	// when, expect no interaction with beacon cache
	target->reportEvent(ACTION_ID, eventName);
}

TEST_F(BeaconTest, sessionStartIsReported)
{
	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(1);

	//given
	auto target = createBeacon()->build();

	// when
	target->startSession();
}

TEST_F(BeaconTest, sessionStartIsReportedForDataCollectionLevel0)
{
	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(1);

	//given
	auto target = createBeacon(DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF)->build();

	// when
	target->startSession();
}

TEST_F(BeaconTest, sessionStartIsReportedForDataCollectionLevel1)
{
	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(1);

	//given
	auto target = createBeacon(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OFF)->build();

	// when
	target->startSession();
}

TEST_F(BeaconTest, sessionStartIsReportedForDataCollectionLevel2)
{
	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(1);

	//given
	auto target = createBeacon(DataCollectionLevel_t::USER_BEHAVIOR, CrashReportingLevel_t::OFF)->build();

	// when
	target->startSession();
}

TEST_F(BeaconTest, clientIPAddressCanBeANullptr)
{
	//given
	auto target = createBeacon()
			->withIpAddress(nullptr)
			->build();

	// when
	auto obtained = target->getClientIPAddress();

	// then
	ASSERT_THAT(obtained, testing::Eq(""));
}

TEST_F(BeaconTest, clientIPAddressCanBeAnEmptyString)
{
	//given
	auto target = createBeacon()
			->withIpAddress("")
			->build();

	// when
	auto obtained = target->getClientIPAddress();

	// then
	ASSERT_THAT(obtained, testing::Eq(""));
}

TEST_F(BeaconTest, validClientIpIsStored)
{
	//given
	Utf8String_t ipAddress("127.0.0.1");
	auto target = createBeacon()
			->withIpAddress(ipAddress)
			->build();

	// when
	auto obtained = target->getClientIPAddress();

	// then
	ASSERT_THAT(obtained, testing::Eq(ipAddress));
}

TEST_F(BeaconTest, invalidClientIPIsConvertedToEmptyString)
{
	//given
	Utf8String_t ipAddress("asdf");
	auto target = createBeacon()
			->withIpAddress(ipAddress)
			->build();

	// when
	auto obtained = target->getClientIPAddress();

	// then
	ASSERT_THAT(obtained, testing::Eq(""));
}

TEST_F(BeaconTest, useInternalBeaconIdForAccessingBeaconCacheWhenSessionNumberReportingDisabled)
{
	// with
	int32_t beaconId = 73;
	ON_CALL(*mockSessionIDProvider, getNextSessionID())
		.WillByDefault(testing::Return(beaconId));

	// expect
	EXPECT_CALL(*mockBeaconCache, deleteCacheEntry(beaconId)).Times(1);

	// given
	auto target = createBeacon(DataCollectionLevel_t::PERFORMANCE, CrashReportingLevel_t::OPT_IN_CRASHES)->build();

	// when
	target->clearData();
}