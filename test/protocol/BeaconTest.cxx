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

#include "protocol/Beacon.h"
#include "OpenKit/CrashReportingLevel.h"
#include "OpenKit/DataCollectionLevel.h"
#include "core/UTF8String.h"
#include "core/caching/BeaconCache.h"
#include "core/caching/BeaconKey.h"
#include "core/configuration/ConfigurationDefaults.h"
#include "core/objects/WebRequestTracer.h"
#include "core/util/URLEncoding.h"
#include "protocol/EventType.h"
#include "protocol/ProtocolConstants.h"

#include "builder/TestBeaconBuilder.h"
#include "mock/MockIHTTPClient.h"
#include "../api/mock/MockILogger.h"
#include "../core/caching/mock/MockIBeaconCache.h"
#include "../core/configuration/mock/MockIBeaconConfiguration.h"
#include "../core/configuration/mock/MockIHTTPClientConfiguration.h"
#include "../core/configuration/mock/MockIOpenKitConfiguration.h"
#include "../core/configuration/mock/MockIPrivacyConfiguration.h"
#include "../core/configuration/mock/MockIServerConfiguration.h"
#include "../core/objects/mock/MockIActionCommon.h"
#include "../core/objects/mock/MockIOpenKitComposite.h"
#include "../core/objects/mock/MockSessionInternals.h"
#include "../core/objects/mock/MockIWebRequestTracerInternals.h"
#include "../protocol/mock/MockIAdditionalQueryParameters.h"
#include "../protocol/mock/MockIStatusResponse.h"
#include "../providers/mock/MockIHTTPClientProvider.h"
#include "../providers/mock/MockIPRNGenerator.h"
#include "../providers/mock/MockISessionIDProvider.h"
#include "../providers/mock/MockIThreadIDProvider.h"
#include "../providers/mock/MockITimingProvider.h"



#include <sstream>

using namespace test;

using BeaconBuilder_sp = std::shared_ptr<TestBeaconBuilder>;
using BeaconCache_t = core::caching::BeaconCache;
using BeaconKey_t = core::caching::BeaconKey;
using CrashReportingLevel_t = openkit::CrashReportingLevel;
using DataCollectionLevel_t = openkit::DataCollectionLevel;
using EventType_t = protocol::EventType;
using MockIBeaconConfiguration_sp = std::shared_ptr<MockIBeaconConfiguration>;
using MockILogger_sp = std::shared_ptr<MockILogger>;
using MockIOpenKitComposite_sp = std::shared_ptr<MockIOpenKitComposite>;
using MockIOpenKitConfiguration_sp = std::shared_ptr<MockIOpenKitConfiguration>;
using MockIPrivacyConfiguration_sp = std::shared_ptr<MockIPrivacyConfiguration>;
using MockIPRNGenerator_sp = std::shared_ptr<MockIPRNGenerator>;
using MockIServerConfiguration_sp = std::shared_ptr<MockIServerConfiguration>;
using MockIAdditionalQueryParameters_sp = std::shared_ptr<MockIAdditionalQueryParameters>;
using MockISessionIDProvider_sp = std::shared_ptr<MockISessionIDProvider>;
using MockIThreadIDProvider_sp = std::shared_ptr<MockIThreadIDProvider>;
using MockITimingProvider_sp = std::shared_ptr<MockITimingProvider>;
using MockIBeaconCache_sp = std::shared_ptr<MockIBeaconCache>;
using UrlEncoding_t = core::util::URLEncoding;
using Utf8String_t = core::UTF8String;
using WebRequestTracer_t = core::objects::WebRequestTracer;
using IServerConfiguration_sp = std::shared_ptr<core::configuration::IServerConfiguration>;

const Utf8String_t APP_ID("appID");
const Utf8String_t APP_NAME("appName");
const Utf8String_t APP_VERSION("1.0");
constexpr int32_t ACTION_ID = 17;
constexpr int32_t SERVER_ID = 1;
constexpr int64_t DEVICE_ID = 456;
constexpr int32_t THREAD_ID = 1234567;
constexpr int32_t SESSION_ID = 73;
constexpr int32_t SESSION_SEQUENCE = 13;
const Utf8String_t TRACER_URL("https://localhost");
constexpr int32_t MULTIPLICITY = 1;

class BeaconTest : public testing::Test
{
protected:

	MockIBeaconConfiguration_sp mockBeaconConfiguration;
	MockIOpenKitConfiguration_sp mockOpenKitConfiguration;
	MockIPrivacyConfiguration_sp mockPrivacyConfiguration;
	MockIServerConfiguration_sp mockServerConfiguration;

	MockIAdditionalQueryParameters_sp mockAdditionalQueryParameters;

	MockISessionIDProvider_sp mockSessionIDProvider;
	MockIThreadIDProvider_sp mockThreadIdProvider;
	MockITimingProvider_sp mockTimingProvider;
	MockIOpenKitComposite_sp mockParent;

	MockILogger_sp mockLogger;
	MockIBeaconCache_sp mockBeaconCache;

	void SetUp() override
	{
		mockOpenKitConfiguration = MockIOpenKitConfiguration::createNice();
		ON_CALL(*mockOpenKitConfiguration, getApplicationId())
			.WillByDefault(testing::ReturnRef(APP_ID));
		ON_CALL(*mockOpenKitConfiguration, getApplicationIdPercentEncoded())
			.WillByDefault(testing::ReturnRef(APP_ID));
		ON_CALL(*mockOpenKitConfiguration, getApplicationName())
			.WillByDefault(testing::ReturnRef(APP_NAME));
		ON_CALL(*mockOpenKitConfiguration, getApplicationVersion())
			.WillByDefault(testing::ReturnRef(APP_VERSION));
		ON_CALL(*mockOpenKitConfiguration, getDeviceId())
			.WillByDefault(testing::Return(DEVICE_ID));

		mockPrivacyConfiguration = MockIPrivacyConfiguration::createNice();

		mockServerConfiguration = MockIServerConfiguration::createNice();
		ON_CALL(*mockServerConfiguration, getServerId())
			.WillByDefault(testing::Return(SERVER_ID));
		ON_CALL(*mockServerConfiguration, getBeaconSizeInBytes())
			.WillByDefault(testing::Return(30 * 1024)); // 30 kB
		ON_CALL(*mockServerConfiguration, getMultiplicity())
			.WillByDefault(testing::Return(MULTIPLICITY));

		auto mockHttpClientConfig = MockIHTTPClientConfiguration::createNice();
		ON_CALL(*mockHttpClientConfig, getServerID())
			.WillByDefault(testing::Return(SERVER_ID));

		mockBeaconConfiguration = MockIBeaconConfiguration::createNice();
		ON_CALL(*mockBeaconConfiguration, getOpenKitConfiguration())
			.WillByDefault(testing::Return(mockOpenKitConfiguration));
		ON_CALL(*mockBeaconConfiguration, getPrivacyConfiguration())
			.WillByDefault(testing::Return(mockPrivacyConfiguration));
		ON_CALL(*mockBeaconConfiguration, getServerConfiguration())
			.WillByDefault(testing::Return(mockServerConfiguration));
		ON_CALL(*mockBeaconConfiguration, getHTTPClientConfiguration())
			.WillByDefault(testing::Return(mockHttpClientConfig));

		mockAdditionalQueryParameters = MockIAdditionalQueryParameters::createNice();

		mockSessionIDProvider = MockISessionIDProvider::createNice();
		ON_CALL(*mockSessionIDProvider, getNextSessionID())
			.WillByDefault(testing::Return(SESSION_ID));

		mockThreadIdProvider = MockIThreadIDProvider::createNice();
		ON_CALL(*mockThreadIdProvider, getThreadID())
			.WillByDefault(testing::Return(THREAD_ID));

		mockTimingProvider = MockITimingProvider::createNice();

		mockLogger = MockILogger::createNice();
		mockBeaconCache = MockIBeaconCache::createStrict();

		mockParent = MockIOpenKitComposite::createNice();
	}

	BeaconBuilder_sp createBeacon()
	{
		auto builder = std::make_shared<TestBeaconBuilder>();

		builder->with(mockLogger)
			.with(mockBeaconCache)
			.with(mockBeaconConfiguration)
			.with(mockSessionIDProvider)
			.withSessionSequenceNumber(SESSION_SEQUENCE)
			.with(mockThreadIdProvider)
			.with(mockTimingProvider)
		;

		return builder;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// generic defaults, instance creation and smaller getters/creators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(BeaconTest, defaultBeaconConfigurationDoesNotDisableCapturing)
{
	// given
	auto target = createBeacon()->build();

	// then
	ASSERT_THAT(target->isCaptureEnabled(), testing::Eq(true));
}

TEST_F(BeaconTest, createInstanceWithInvalidIpAddress)
{
	// with
	Utf8String_t ipAddress("invalid");

	// expect
	EXPECT_CALL(*mockLogger, mockWarning("Beacon() - Client IP address validation failed: invalid"))
		.Times(1);

	// given, when
	auto target = createBeacon()
		->withIpAddress(ipAddress)
		.build();

	// then
	ASSERT_THAT(target->getClientIPAddress(), testing::Eq("invalid"));
	ASSERT_THAT(target->useClientIPAddress(), testing::Eq(false));
}

TEST_F(BeaconTest, createInstanceWithNullIpAddress)
{
	// with
	const char* ipAddress = nullptr;

	// expect
	EXPECT_CALL(*mockLogger, mockWarning(testing::_))
		.Times(0);

	// given, when
	auto target = createBeacon()
		->withIpAddress(ipAddress)
		.build();

	// then
	ASSERT_THAT(target->getClientIPAddress(), testing::Eq(""));
	ASSERT_THAT(target->useClientIPAddress(), testing::Eq(false));
}

TEST_F(BeaconTest, createInstanceWithEmptyIpAddress)
{
	//given
	auto target = createBeacon()
		->withIpAddress("")
		.build();

	// when
	auto obtained = target->getClientIPAddress();

	// then
	ASSERT_THAT(obtained, testing::Eq(""));
	ASSERT_THAT(target->useClientIPAddress(), testing::Eq(false));
}

TEST_F(BeaconTest, createInstanceWithValidIpAddress)
{
	//given
	Utf8String_t ipAddress("127.0.0.1");
	auto target = createBeacon()
		->withIpAddress(ipAddress)
		.build();

	// when
	auto obtained = target->getClientIPAddress();

	// then
	ASSERT_THAT(obtained, testing::Eq(ipAddress));
	ASSERT_THAT(target->useClientIPAddress(), testing::Eq(true));
}

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// createTag - creating web request tag tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
		<< "_" << APP_ID.getStringData()			// application ID
		<< "_" << ACTION_ID							// parent action ID
		<< "_" << THREAD_ID							// thread ID
		<< "_" << sequenceNo						// sequence number
	;
	ASSERT_THAT(obtained, testing::Eq(s.str()));
}

TEST_F(BeaconTest, createTagEncodesDeviceIDProperly)
{
	// given
	int32_t deviceId = -42;
	int32_t sequenceNo = 42;

	ON_CALL(*mockOpenKitConfiguration, getDeviceId())
		.WillByDefault(testing::Return(deviceId));

	auto target = createBeacon()->build();

	// when
	auto obtained = target->createTag(ACTION_ID, sequenceNo);

	// then
	std::stringstream s;
	s << "MT" 										// tag prefix
		<< "_" << protocol::PROTOCOL_VERSION		// protocol version
		<< "_" << SERVER_ID							// server ID
		<< "_" << deviceId							// device ID
		<< "_" << SESSION_ID						// session number
		<< "_" << APP_ID.getStringData()			// application ID
		<< "_" << ACTION_ID							// parent action ID
		<< "_" << THREAD_ID							// thread ID
		<< "_" << sequenceNo						// sequence number
	;

	ASSERT_THAT(obtained, testing::Eq(s.str()));
}

TEST_F(BeaconTest, beaconReturnsEmptyTagIfWebRequestTracingDisallowed)
{
	// with
	ON_CALL(*mockPrivacyConfiguration, isWebRequestTracingAllowed())
		.WillByDefault(testing::Return(false));

	//given
	auto target = createBeacon()->build();

	// when
	auto obtained = target->createTag(ACTION_ID, 1);

	//then
	ASSERT_THAT(obtained, testing::Eq(""));
}

TEST_F(BeaconTest, beaconReturnsValidTagIfWebRequestTracingIsAllowed)
{
	// with
	int32_t sequenceNo = 1;

	ON_CALL(*mockPrivacyConfiguration, isWebRequestTracingAllowed())
		.WillByDefault(testing::Return(true));

	auto target = createBeacon()->build();

	// when
	auto tagString = target->createTag(ACTION_ID, sequenceNo);

	//then
	std::stringstream str;
	str << "MT"						// tag prefix
		<< "_" << protocol::PROTOCOL_VERSION	// protocol version
		<< "_" << SERVER_ID						// server ID
		<< "_" << DEVICE_ID						// device ID
		<< "_" << SESSION_ID					// session number
		<< "_" << APP_ID.getStringData()		// application ID
		<< "_" << ACTION_ID						// parent action ID
		<< "_" << THREAD_ID						// thread ID
		<< "_" << sequenceNo					// sequence number
		;

	ASSERT_THAT(tagString.getStringData(), testing::Eq(str.str()));
}

TEST_F(BeaconTest, beaconReturnsValidTagWithSessionNumberIfSessionNumberReportingAllowed)
{
	// with
	int32_t sequenceNo = 1;

	ON_CALL(*mockPrivacyConfiguration, isSessionNumberReportingAllowed())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mockPrivacyConfiguration, isWebRequestTracingAllowed())
		.WillByDefault(testing::Return(true));

	auto target = createBeacon()->build();

	// when
	auto tagString = target->createTag(ACTION_ID, sequenceNo);

	//then
	std::stringstream str;
	str << "MT"						// tag prefix
		<< "_" << protocol::PROTOCOL_VERSION	// protocol version
		<< "_" << SERVER_ID						// server ID
		<< "_" << DEVICE_ID						// device ID
		<< "_" << SESSION_ID					// session number
		<< "_" << APP_ID.getStringData()		// application ID
		<< "_" << ACTION_ID						// parent action ID
		<< "_" << THREAD_ID						// thread ID
		<< "_" << sequenceNo					// sequence number
		;

	ASSERT_THAT(tagString.getStringData(), testing::Eq(str.str()));
}

TEST_F(BeaconTest, beaconReturnsValidTagWithSessionNumberOneIfSessionNumberReportinDisallowed)
{
	// with
	int32_t sequenceNo = 1;

	ON_CALL(*mockPrivacyConfiguration, isSessionNumberReportingAllowed())
		.WillByDefault(testing::Return(false));
	ON_CALL(*mockPrivacyConfiguration, isWebRequestTracingAllowed())
		.WillByDefault(testing::Return(true));

	auto target = createBeacon()->build();

	// when
	auto tagString = target->createTag(ACTION_ID, sequenceNo);

	//then
	std::stringstream str;
	str << "MT"						// tag prefix
		<< "_" << protocol::PROTOCOL_VERSION	// protocol version
		<< "_" << SERVER_ID						// server ID
		<< "_" << DEVICE_ID						// device ID
		<< "_1"									// session number (must always be 1 if session number reporting disallowed)
		<< "_" << APP_ID.getStringData()		// application ID
		<< "_" << ACTION_ID						// parent action ID
		<< "_" << THREAD_ID						// thread ID
		<< "_" << sequenceNo					// sequence number
		;

	ASSERT_THAT(tagString.getStringData(), testing::Eq(str.str()));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// addAction tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// action start time
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->addAction(action);
}

TEST_F(BeaconTest, actionNotReportedIfCapturingIsDisabled)
{
	// with
	ON_CALL(*mockServerConfiguration, isCaptureEnabled())
		.WillByDefault(testing::Return(false));

	Utf8String_t actionName("action");
	auto action = MockIActionCommon::createNice();
	ON_CALL(*action, getName())
		.WillByDefault(testing::ReturnRef(actionName));

	// expect
	EXPECT_CALL(*mockBeaconCache, addActionData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	auto target = createBeacon()->build();

	// when, expect no interaction with beacon cache
	target->addAction(action);
}

TEST_F(BeaconTest, actionNotReportedIfActionReportingDisallowed)
{
	// with
	ON_CALL(*mockPrivacyConfiguration, isActionReportingAllowed())
		.WillByDefault(testing::Return(false));

	auto actionMock = MockIActionCommon::createStrict();

	// expect
	EXPECT_CALL(*mockBeaconCache, addActionData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	auto target = createBeacon()->build();

	// when, expect no interaction with beacon cache
	target->addAction(actionMock);
}

TEST_F(BeaconTest, actionNotReportedIfDataSendingDisallowed)
{
	// with
	ON_CALL(*mockServerConfiguration, isSendingDataAllowed())
		.WillByDefault(testing::Return(false));

	auto actionMock = MockIActionCommon::createStrict();

	// expect
	EXPECT_CALL(*mockBeaconCache, addActionData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	auto target = createBeacon()->build();

	// when, expect no interaction with beacon cache
	target->addAction(actionMock);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// startSession tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(BeaconTest, addStartSessionEvent)
{
	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::SESSION_START)	// event type
		<< "&it=" << THREAD_ID										// thread ID
		<< "&pa=0"													// parent action ID
		<< "&s0=1"													// start session sequence number
		<< "&t0=0"													// session start time
		;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// session end time
		testing::Eq(s.str())
	)).Times(1);

	//given
	auto target = createBeacon()->build();

	// when
	target->startSession();
}

TEST_F(BeaconTest, sessionStartIsReportedRegardlessOfPrivacyConfiguration)
{
	// with
	auto privacyConfig = MockIPrivacyConfiguration::createStrict();

	ON_CALL(*mockBeaconConfiguration, getPrivacyConfiguration())
		.WillByDefault(testing::Return(privacyConfig));

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(1);
	EXPECT_CALL(*privacyConfig, isDeviceIdSendingAllowed())
		.Times(1); // beacon constructor, checking if device ID should be randomized
	EXPECT_CALL(*privacyConfig, isSessionNumberReportingAllowed())
		.Times(1); // beacon constructor, checking if session number can be sent
	EXPECT_CALL(*privacyConfig, getDataCollectionLevel())
		.Times(1); // beacon constructor, creating immutable beacon string
	EXPECT_CALL(*privacyConfig, getCrashReportingLevel())
		.Times(1); // beacon constructor, creating immutable beacon string

	//given
	auto target = createBeacon()->build();

	// when
	target->startSession();
}

TEST_F(BeaconTest, noSessionStartIsReportedIfCapturingDisabled)
{
	//given
	ON_CALL(*mockServerConfiguration, isCaptureEnabled())
		.WillByDefault(testing::Return(false));

	auto target = createBeacon()->build();

	// expect
	EXPECT_CALL(*mockBeaconCache, addActionData(testing::_, testing::_, testing::_))
		.Times(0);

	// when, expect on interaction on beacon cache
	target->startSession();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// endSession tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// session end time
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->endSession();
}

TEST_F(BeaconTest, sessionNotReportedIfCapturingDisabled)
{
	// with
	ON_CALL(*mockServerConfiguration, isCaptureEnabled())
		.WillByDefault(testing::Return(false));

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	auto target = createBeacon()->build();

	// when, expect no interaction with beacon cache
	target->endSession();
}

TEST_F(BeaconTest, sessionNotReportedIfSessionReportingDisallowed)
{
	// with
	ON_CALL(*mockPrivacyConfiguration, isSessionReportingAllowed())
		.WillByDefault(testing::Return(false));

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	//given
	auto target = createBeacon()->build();

	// when, expect no interaction with beacon cache
	target->endSession();
}

TEST_F(BeaconTest, sessionNotReportedIfDataSendingDisallowed)
{
	// with
	ON_CALL(*mockServerConfiguration, isSendingDataAllowed())
		.WillByDefault(testing::Return(false));

	// expect
	EXPECT_CALL(*mockBeaconCache, addActionData(testing::_, testing::_, testing::_))
		.Times(0);

	//given
	auto target = createBeacon()->build();

	// when, expect no interaction with beacon cache
	target->endSession();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// reportValue(int32_t) tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(BeaconTest, reportValidValueInt32)
{
	// with
	Utf8String_t valueName("Int32Value");
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
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when value was reported
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->reportValue(ACTION_ID, valueName, value);
}

TEST_F(BeaconTest, int32ValueIsNotReportedIfCapturingIsDisabled)
{
	// with
	ON_CALL(*mockServerConfiguration, isCaptureEnabled())
		.WillByDefault(testing::Return(false));

	int32_t value = 42;
	Utf8String_t valueName("Int32Value");

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	auto target = createBeacon()->build();

	// when, expect no interaction with beacon cache
	target->reportValue(ACTION_ID, valueName, value);
}

TEST_F(BeaconTest, int32ValueIsNotReportedIfReportValueDisallowed)
{
	// with
	ON_CALL(*mockPrivacyConfiguration, isValueReportingAllowed())
		.WillByDefault(testing::Return(false));

	Utf8String_t valueName("Int32Value");
	int32_t value = 42;

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	//given
	auto target = createBeacon()->build();

	// when, expect no interaction with beacon cache
	target->reportValue(1, valueName, value);
}

TEST_F(BeaconTest, int32ValueIsNotReportedIfDataSendingDisallowed)
{
	// with
	ON_CALL(*mockServerConfiguration, isSendingDataAllowed())
		.WillByDefault(testing::Return(false));

	Utf8String_t valueName("Int32Value");
	int32_t value = 42;

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	//given
	auto target = createBeacon()->build();

	// when, expect no interaction with beacon cache
	target->reportValue(1, valueName, value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// reportValue(int64_t) tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(BeaconTest, reportValidValueInt64)
{
	// with
	Utf8String_t valueName("Int64Value");
	int64_t value = 21;

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
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when value was reported
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->reportValue(ACTION_ID, valueName, value);
}

TEST_F(BeaconTest, int64ValueIsNotReportedIfCapturingIsDisabled)
{
	// with
	ON_CALL(*mockServerConfiguration, isCaptureEnabled())
		.WillByDefault(testing::Return(false));

	int64_t value = 42;
	Utf8String_t valueName("Int64Value");

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	auto target = createBeacon()->build();

	// when, expect no interaction with beacon cache
	target->reportValue(ACTION_ID, valueName, value);
}

TEST_F(BeaconTest, int64ValueIsNotReportedIfReportValueDisallowed)
{
	// with
	ON_CALL(*mockPrivacyConfiguration, isValueReportingAllowed())
		.WillByDefault(testing::Return(false));

	Utf8String_t valueName("Int64Value");
	int64_t value = 42;

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	//given
	auto target = createBeacon()->build();

	// when, expect no interaction with beacon cache
	target->reportValue(1, valueName, value);
}

TEST_F(BeaconTest, int64ValueIsNotReportedIfDataSendingDisallowed)
{
	// with
	ON_CALL(*mockServerConfiguration, isSendingDataAllowed())
		.WillByDefault(testing::Return(false));

	Utf8String_t valueName("Int64Value");
	int64_t value = 42;

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	//given
	auto target = createBeacon()->build();

	// when, expect no interaction with beacon cache
	target->reportValue(1, valueName, value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// reportValue(double) tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(BeaconTest, reportValidValueDouble)
{
	// with
	Utf8String_t valueName("DoubleValue");
	double value = 3.125;

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::VALUE_DOUBLE)	// event type
		<< "&na=" << valueName.getStringData()		// name of reported value
		<< "&it=" << THREAD_ID						// thread ID
		<< "&pa=" << ACTION_ID						// parent action
		<< "&s0=1"									// sequence number of reported value
		<< "&t0=0"									// event time since session start
		<< "&vl=3.125"								// reported value
	;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when value was reported
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->reportValue(ACTION_ID, valueName, value);
}

TEST_F(BeaconTest, doubleValueIsNotReportedIfCapturingIsDisabled)
{
	// with
	ON_CALL(*mockServerConfiguration, isCaptureEnabled())
		.WillByDefault(testing::Return(false));

	double value = 42.1337;
	Utf8String_t valueName("DoubleValue");

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	auto target = createBeacon()->build();

	// when, expect no interaction with beacon cache
	target->reportValue(ACTION_ID, valueName, value);
}

TEST_F(BeaconTest, doubleValueIsNotReportedIfReportValueDisallowed)
{
	// with
	ON_CALL(*mockPrivacyConfiguration, isValueReportingAllowed())
		.WillByDefault(testing::Return(false));

	Utf8String_t valueName("DoubleValue");
	double value = 42.1337;

	// expect
	EXPECT_CALL(*mockBeaconCache, addActionData(testing::_, testing::_, testing::_))
		.Times(0);

	//given
	auto target = createBeacon()->build();

	// when, expect no interaction with beacon cache
	target->reportValue(1, valueName, value);
}

TEST_F(BeaconTest, doubleValueIsNotReportedIfDataSendingDisallowed)
{
	// with
	ON_CALL(*mockServerConfiguration, isSendingDataAllowed())
		.WillByDefault(testing::Return(false));

	Utf8String_t valueName("DoubleValue");
	double value = 42.1337;

	// expect
	EXPECT_CALL(*mockBeaconCache, addActionData(testing::_, testing::_, testing::_))
		.Times(0);

	//given
	auto target = createBeacon()->build();

	// when, expect no interaction with beacon cache
	target->reportValue(1, valueName, value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// reportValue(string) tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
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
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
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
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when value was reported
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->reportValue(ACTION_ID, valueName, value);
}

TEST_F(BeaconTest, stringValueIsNotIfCapturingIsDisabled)
{
	// with
	ON_CALL(*mockServerConfiguration, isCaptureEnabled())
		.WillByDefault(testing::Return(false));

	Utf8String_t value("HelloWorld");
	Utf8String_t valueName("StringValue");

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	auto target = createBeacon()->build();

	// when, expect no interaction with beacon cache
	target->reportValue(ACTION_ID, valueName, value);
}

TEST_F(BeaconTest, stringValueIsNotReportedIfValueReportingDisallowed)
{
	// with
	ON_CALL(*mockPrivacyConfiguration, isValueReportingAllowed())
		.WillByDefault(testing::Return(false));

	Utf8String_t valueName("StringValue");
	Utf8String_t value("HelloWorld");

	// expect
	EXPECT_CALL(*mockBeaconCache, addActionData(testing::_, testing::_, testing::_))
		.Times(0);

	//given
	auto target = createBeacon()->build();

	// when, expect no interaction with beacon cache
	target->reportValue(1, valueName, value);
}

TEST_F(BeaconTest, stringValueIsNotReportedIfDataSendingDisallowed)
{
	// with
	ON_CALL(*mockServerConfiguration, isSendingDataAllowed())
		.WillByDefault(testing::Return(false));

	Utf8String_t valueName("StringValue");
	Utf8String_t value("HelloWorld");

	// expect
	EXPECT_CALL(*mockBeaconCache, addActionData(testing::_, testing::_, testing::_))
		.Times(0);

	//given
	auto target = createBeacon()->build();

	// when, expect no interaction with beacon cache
	target->reportValue(1, valueName, value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// reportEvent tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
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
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// event timestamp
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->reportEvent(ACTION_ID, eventName);
}

TEST_F(BeaconTest, namedEventNotReportedIfCapturingDisabled)
{
	// with
	ON_CALL(*mockServerConfiguration, isCaptureEnabled())
		.WillByDefault(testing::Return(false));

	Utf8String_t eventName("event name");

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	auto target = createBeacon()->build();

	// when, expect no interaction with beacon cache
	target->reportEvent(ACTION_ID, eventName);
}

TEST_F(BeaconTest, namedEventNotReportedIfDataSendingIsDisallowed)
{
	// with
	ON_CALL(*mockServerConfiguration, isSendingDataAllowed())
		.WillByDefault(testing::Return(false));

	Utf8String_t eventName("event name");

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	auto target = createBeacon()->build();

	// when, expect no interaction with beacon cache
	target->reportEvent(ACTION_ID, eventName);
}

TEST_F(BeaconTest, namedEventNotReportedIfEventReportingDisallowed)
{
	// with
	ON_CALL(*mockPrivacyConfiguration, isEventReportingAllowed())
		.WillByDefault(testing::Return(false));

	Utf8String_t eventName("some event");

	// expect
	EXPECT_CALL(*mockBeaconCache, addActionData(testing::_, testing::_, testing::_))
		.Times(0);

	//given
	auto target = createBeacon()->build();

	// when, expect no interaction with beacon cache
	target->reportEvent(ACTION_ID, eventName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// reportError tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
		<< "&tt=c"									// error technology type
	;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
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
		<< "&tt=c"									// error technology type
	;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when error was reported
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->reportError(ACTION_ID, errorName, errorCode, reason);
}

TEST_F(BeaconTest, errorNotReportedIfCapturingDisabled)
{
	// with
	ON_CALL(*mockServerConfiguration, isCaptureEnabled())
		.WillByDefault(testing::Return(false));

	Utf8String_t eventName("event name");
	int32_t errorCode = 123;
	Utf8String_t reason("error reason");

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	auto target = createBeacon()->build();

	// when, expect no interaction with beacon cache
	target->reportError(ACTION_ID, eventName, errorCode, reason);
}

TEST_F(BeaconTest, errorNotReportedIfSendingErrorDataDisallowed)
{
	// with
	ON_CALL(*mockServerConfiguration, isSendingErrorsAllowed())
		.WillByDefault(testing::Return(false));

	Utf8String_t eventName("event name");
	int32_t errorCode = 123;
	Utf8String_t reason("error reason");

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	auto target = createBeacon()->build();

	// when, expect no interaction with beacon cache
	target->reportError(ACTION_ID, eventName, errorCode, reason);
}

TEST_F(BeaconTest, errorNotReportedIfErrorReportingDisallowed)
{
	// with
	ON_CALL(*mockPrivacyConfiguration, isErrorReportingAllowed())
		.WillByDefault(testing::Return(false));

	Utf8String_t eventName("error name");
	int32_t errorCode = 132;
	Utf8String_t reason("error reason");

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	//given
	auto target = createBeacon()->build();

	// when, expect no interaction with beacon cache
	target->reportError(ACTION_ID, eventName, errorCode, reason);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// reportCrash tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
		<< "&tt=c"									// error technology type
	;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
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
		<< "&tt=c"									// error technology type
	;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when crash was reported
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->reportCrash(errorName, reason, stacktrace);
}

TEST_F(BeaconTest, crashNotReportedIfCapturingIsDisabled)
{
	// with
	ON_CALL(*mockServerConfiguration, isCaptureEnabled())
		.WillByDefault(testing::Return(false));

	Utf8String_t eventName("event name");
	Utf8String_t reason("some reason");
	Utf8String_t stacktrace("some stacktrace");

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	auto target = createBeacon()->build();

	// when, expect no interaction with beacon cache
	target->reportCrash(eventName, reason, stacktrace);
}

TEST_F(BeaconTest, crashNotReportedIfSendingCrashDataDisallowed)
{
	// with
	ON_CALL(*mockServerConfiguration, isSendingCrashesAllowed())
		.WillByDefault(testing::Return(false));

	Utf8String_t eventName("event name");
	Utf8String_t reason("some reason");
	Utf8String_t stacktrace("some stacktrace");

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	auto target = createBeacon()->build();

	// when, expect no interaction with beacon cache
	target->reportCrash(eventName, reason, stacktrace);
}

TEST_F(BeaconTest, crashNotReportedIfCrashReportingDisallowed)
{
	// with
	ON_CALL(*mockPrivacyConfiguration, isCrashReportingAllowed())
		.WillByDefault(testing::Return(false));

	Utf8String_t eventName("crash name");
	Utf8String_t reason("some reason");
	Utf8String_t stacktrace("in some dark code segment");

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	//given
	auto target = createBeacon()->build();

	// when, expect no interaction with beacon cache
	target->reportCrash(eventName, reason, stacktrace);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// addWebRequest tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when web request tracer was reported
		testing::Eq(s.str())
	)).Times(1);

	// when
	target->addWebRequest(ACTION_ID, tracer);
}

TEST_F(BeaconTest, canAddSentBytesEqualToZeroToWebRequest)
{
	// given
	int32_t numBytesReceived = 14;
	int32_t responseCode = 15;

	auto target = createBeacon()->build();

	auto tracer = MockIWebRequestTracerInternals::createNice();
	ON_CALL(*tracer, getURL())
		.WillByDefault(testing::Return(TRACER_URL));
	ON_CALL(*tracer, getBytesSent())
		.WillByDefault(testing::Return(0));
	ON_CALL(*tracer, getBytesReceived())
		.WillByDefault(testing::Return(numBytesReceived));
	ON_CALL(*tracer, getResponseCode())
		.WillByDefault(testing::Return(responseCode));

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::WEBREQUEST)	// event type
		<< "&na=" << UrlEncoding_t::urlencode(TRACER_URL, { '_' }).getStringData() // tracer url
		<< "&it=" << THREAD_ID						// thread ID
		<< "&pa=" << ACTION_ID						// parent action
		<< "&s0=0"									// web request start sequence number
		<< "&t0=0"									// web request start time (since session start)
		<< "&s1=0"									// web request end sequence number
		<< "&t1=0"									// web request end time (relative to start time)
		<< "&bs=" << 0								// number of bytes sent
		<< "&br=" << numBytesReceived				// number of bytes received
		<< "&rc=" << responseCode					// response code
		;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when web request tracer was reported
		testing::Eq(s.str())
	)).Times(1);

	// when
	target->addWebRequest(ACTION_ID, tracer);
}

TEST_F(BeaconTest, cannotAddSentBytesLessThanZeroToWebRequest)
{
	// given
	int32_t numBytesReceived = 14;
	int32_t responseCode = 15;

	auto target = createBeacon()->build();

	auto tracer = MockIWebRequestTracerInternals::createNice();
	ON_CALL(*tracer, getURL())
		.WillByDefault(testing::Return(TRACER_URL));
	ON_CALL(*tracer, getBytesSent())
		.WillByDefault(testing::Return(-1));
	ON_CALL(*tracer, getBytesReceived())
		.WillByDefault(testing::Return(numBytesReceived));
	ON_CALL(*tracer, getResponseCode())
		.WillByDefault(testing::Return(responseCode));

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::WEBREQUEST)	// event type
		<< "&na=" << UrlEncoding_t::urlencode(TRACER_URL, { '_' }).getStringData() // tracer url
		<< "&it=" << THREAD_ID						// thread ID
		<< "&pa=" << ACTION_ID						// parent action
		<< "&s0=0"									// web request start sequence number
		<< "&t0=0"									// web request start time (since session start)
		<< "&s1=0"									// web request end sequence number
		<< "&t1=0"									// web request end time (relative to start time)
		<< "&br=" << numBytesReceived				// number of bytes received
		<< "&rc=" << responseCode					// response code
		;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when web request tracer was reported
		testing::Eq(s.str())
	)).Times(1);

	// when
	target->addWebRequest(ACTION_ID, tracer);
}

TEST_F(BeaconTest, canAddReceivedBytesEqualToZeroToWebRequest)
{
	// given
	int32_t numBytesSent = 13;
	int32_t responseCode = 15;

	auto target = createBeacon()->build();

	auto tracer = MockIWebRequestTracerInternals::createNice();
	ON_CALL(*tracer, getURL())
		.WillByDefault(testing::Return(TRACER_URL));
	ON_CALL(*tracer, getBytesSent())
		.WillByDefault(testing::Return(numBytesSent));
	ON_CALL(*tracer, getBytesReceived())
		.WillByDefault(testing::Return(0));
	ON_CALL(*tracer, getResponseCode())
		.WillByDefault(testing::Return(responseCode));

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::WEBREQUEST)	// event type
		<< "&na=" << UrlEncoding_t::urlencode(TRACER_URL, { '_' }).getStringData() // tracer url
		<< "&it=" << THREAD_ID						// thread ID
		<< "&pa=" << ACTION_ID						// parent action
		<< "&s0=0"									// web request start sequence number
		<< "&t0=0"									// web request start time (since session start)
		<< "&s1=0"									// web request end sequence number
		<< "&t1=0"									// web request end time (relative to start time)
		<< "&bs=" << numBytesSent					// number of bytes sent
		<< "&br=" << 0								// number of bytes received
		<< "&rc=" << responseCode					// response code
		;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when web request tracer was reported
		testing::Eq(s.str())
	)).Times(1);

	// when
	target->addWebRequest(ACTION_ID, tracer);
}

TEST_F(BeaconTest, cannotAddReceivedBytesLessThanZeroToWebRequest)
{
	// given
	int32_t numBytesSent = 13;
	int32_t responseCode = 15;

	auto target = createBeacon()->build();

	auto tracer = MockIWebRequestTracerInternals::createNice();
	ON_CALL(*tracer, getURL())
		.WillByDefault(testing::Return(TRACER_URL));
	ON_CALL(*tracer, getBytesSent())
		.WillByDefault(testing::Return(numBytesSent));
	ON_CALL(*tracer, getBytesReceived())
		.WillByDefault(testing::Return(-1));
	ON_CALL(*tracer, getResponseCode())
		.WillByDefault(testing::Return(responseCode));

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::WEBREQUEST)	// event type
		<< "&na=" << UrlEncoding_t::urlencode(TRACER_URL, { '_' }).getStringData() // tracer url
		<< "&it=" << THREAD_ID						// thread ID
		<< "&pa=" << ACTION_ID						// parent action
		<< "&s0=0"									// web request start sequence number
		<< "&t0=0"									// web request start time (since session start)
		<< "&s1=0"									// web request end sequence number
		<< "&t1=0"									// web request end time (relative to start time)
		<< "&bs=" << numBytesSent					// number of bytes sent
		<< "&rc=" << responseCode					// response code
		;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when web request tracer was reported
		testing::Eq(s.str())
	)).Times(1);

	// when
	target->addWebRequest(ACTION_ID, tracer);
}

TEST_F(BeaconTest, canAddResponseCodeEqualToZeroToWebRequest)
{
	// given
	int32_t numBytesSent = 13;
	int32_t numBytesReceived = 14;

	auto target = createBeacon()->build();

	auto tracer = MockIWebRequestTracerInternals::createNice();
	ON_CALL(*tracer, getURL())
		.WillByDefault(testing::Return(TRACER_URL));
	ON_CALL(*tracer, getBytesSent())
		.WillByDefault(testing::Return(numBytesSent));
	ON_CALL(*tracer, getBytesReceived())
		.WillByDefault(testing::Return(numBytesReceived));
	ON_CALL(*tracer, getResponseCode())
		.WillByDefault(testing::Return(0));

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::WEBREQUEST)	// event type
		<< "&na=" << UrlEncoding_t::urlencode(TRACER_URL, { '_' }).getStringData() // tracer url
		<< "&it=" << THREAD_ID						// thread ID
		<< "&pa=" << ACTION_ID						// parent action
		<< "&s0=0"									// web request start sequence number
		<< "&t0=0"									// web request start time (since session start)
		<< "&s1=0"									// web request end sequence number
		<< "&t1=0"									// web request end time (relative to start time)
		<< "&bs=" << numBytesSent					// number of bytes sent
		<< "&br=" << numBytesReceived				// number of bytes received
		<< "&rc=" << 0								// response code
		;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when web request tracer was reported
		testing::Eq(s.str())
	)).Times(1);

	// when
	target->addWebRequest(ACTION_ID, tracer);
}

TEST_F(BeaconTest, cannotAddResponseCodeLessThanZeroToWebRequest)
{
	// given
	int32_t numBytesSent = 13;
	int32_t numBytesReceived = 14;

	auto target = createBeacon()->build();

	auto tracer = MockIWebRequestTracerInternals::createNice();
	ON_CALL(*tracer, getURL())
		.WillByDefault(testing::Return(TRACER_URL));
	ON_CALL(*tracer, getBytesSent())
		.WillByDefault(testing::Return(numBytesSent));
	ON_CALL(*tracer, getBytesReceived())
		.WillByDefault(testing::Return(numBytesReceived));
	ON_CALL(*tracer, getResponseCode())
		.WillByDefault(testing::Return(-1));

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::WEBREQUEST)	// event type
		<< "&na=" << UrlEncoding_t::urlencode(TRACER_URL, { '_' }).getStringData() // tracer url
		<< "&it=" << THREAD_ID						// thread ID
		<< "&pa=" << ACTION_ID						// parent actiond
		<< "&s0=0"									// web request start sequence number
		<< "&t0=0"									// web request start time (since session start)
		<< "&s1=0"									// web request end sequence number
		<< "&t1=0"									// web request end time (relative to start time)
		<< "&bs=" << numBytesSent					// number of bytes sent
		<< "&br=" << numBytesReceived				// number of bytes received
		;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when web request tracer was reported
		testing::Eq(s.str())
	)).Times(1);

	// when
	target->addWebRequest(ACTION_ID, tracer);
}

TEST_F(BeaconTest, webRequestNotReportedIfCapturingDisabled)
{
	// with
	ON_CALL(*mockServerConfiguration, isCaptureEnabled())
		.WillByDefault(testing::Return(false));

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	auto target = createBeacon()
		->build();
	auto tracer = MockIWebRequestTracerInternals::createNice();

	// when, expect no interaction with beacon cache
	target->addWebRequest(ACTION_ID, tracer);
}

TEST_F(BeaconTest, webRequestNotReportedIfWebRequestTracingDisallowed)
{
	// with
	ON_CALL(*mockPrivacyConfiguration, isWebRequestTracingAllowed())
		.WillByDefault(testing::Return(false));

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	auto target = createBeacon()->build();
	auto tracer = MockIWebRequestTracerInternals::createNice();

	// when, expect no interaction with beacon cache
	target->addWebRequest(ACTION_ID, tracer);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// identifyUser tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
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
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp of the user identification event
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->identifyUser(userID);
}

TEST_F(BeaconTest, cannotIdentifyUserIfCapturingDisabled)
{
	// with
	ON_CALL(*mockServerConfiguration, isCaptureEnabled())
		.WillByDefault(testing::Return(false));

	Utf8String_t userID("jane.doe@acmoe.com");

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	auto target = createBeacon()->build();

	// when, expect no interaction with beacon cache
	target->identifyUser(userID);
}

TEST_F(BeaconTest, cannotIdentifyUserIfUserIdentificationDisabled)
{
	// with
	ON_CALL(*mockPrivacyConfiguration, isUserIdentificationAllowed())
		.WillByDefault(testing::Return(false));

	// given
	auto target = createBeacon()->build();

	// when, expect no interaction with beacon cache
	target->identifyUser("jane@doe.com");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// send tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(BeaconTest, canHandleNoDataInBeaconSend)
{
	// expect
	EXPECT_CALL(*mockBeaconCache, getNextBeaconChunk(BeaconKey_t(SESSION_ID, SESSION_SEQUENCE), testing::_, testing::_, testing::_))
		.Times(1);

	// given
	auto httpClient = MockIHTTPClient::createNice();
	auto httpClientProvider = MockIHTTPClientProvider::createNice();
	ON_CALL(*httpClientProvider, createClient(testing::_))
		.WillByDefault(testing::Return(httpClient));

	auto target = createBeacon()->build();

	// when
	auto obtained = target->send(httpClientProvider, *mockAdditionalQueryParameters);

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
	ON_CALL(*httpClient, sendBeaconRequest(testing::_, testing::_, testing::_))
		.WillByDefault(testing::Return(statusResponse));

	auto httpClientProvider = MockIHTTPClientProvider::createNice();
	ON_CALL(*httpClientProvider, createClient(testing::_))
		.WillByDefault(testing::Return(httpClient));

	// expect
	EXPECT_CALL(*httpClient, sendBeaconRequest(testing::Eq(ipAddress), testing::_, testing::Ref(*mockAdditionalQueryParameters)))
		.Times(1);

	// given
	auto target = createBeacon()
		->withIpAddress(ipAddress)
		.with(beaconCache)
		.build();

	// when
	target->reportCrash("errorName", "errorReason", "errorStackTrace");
	auto obtained = target->send(httpClientProvider, *mockAdditionalQueryParameters);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getResponseCode(), testing::Eq(responseCode));
}

TEST_F(BeaconTest, sendDataAndFakeErrorResponse)
{
	// with
	int32_t responseCode = 418;
	Utf8String_t ipAddress("127.0.0.1");
	auto beaconCache = std::make_shared<BeaconCache_t>(mockLogger);

	auto statusResponse = MockIStatusResponse::createNice();
	ON_CALL(*statusResponse, getResponseCode())
		.WillByDefault(testing::Return(responseCode));
	ON_CALL(*statusResponse, isErroneousResponse())
		.WillByDefault(testing::Return(true));

	auto httpClient = MockIHTTPClient::createNice();
	ON_CALL(*httpClient, sendBeaconRequest(testing::_, testing::_, testing::_))
		.WillByDefault(testing::Return(statusResponse));

	auto httpClientProvider = MockIHTTPClientProvider::createNice();
	ON_CALL(*httpClientProvider, createClient(testing::_))
		.WillByDefault(testing::Return(httpClient));

	// expect
	EXPECT_CALL(*httpClient, sendBeaconRequest(testing::Eq(ipAddress), testing::_, testing::Ref(*mockAdditionalQueryParameters)))
		.Times(1);

	// given
	auto target = createBeacon()
		->withIpAddress(ipAddress)
		.with(beaconCache)
		.build();

	// when
	target->reportCrash("errorName", "errorReason", "errorStackTrace");
	auto obtained = target->send(httpClientProvider, *mockAdditionalQueryParameters);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->getResponseCode(), testing::Eq(responseCode));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// misc tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(BeaconTest, clearDataFromBeaconCache)
{
	// given
	auto beaconCache = std::make_shared<BeaconCache_t>(mockLogger);

	auto target = createBeacon()
		->with(beaconCache)
		.build();

	auto action = MockIActionCommon::createNice();

	target->addAction(action);
	target->reportValue(ACTION_ID, "Int32Value", 42);
	target->reportValue(ACTION_ID, "Int64Value", int64_t(42));
	target->reportValue(ACTION_ID, "DoubleValue", 3.1415);
	target->reportValue(ACTION_ID, "StringValue", "HelloWorld");
	target->reportEvent(ACTION_ID, "SomeEvent");
	target->reportError(ACTION_ID, "SomeError", -123, "SomeReason");
	target->reportCrash("SomeCrash", "SomeReason", "SomeStacktrace");
	target->endSession();

	// when
	target->clearData();

	// then
	ASSERT_THAT(target->isEmpty(), testing::Eq(true));
}

TEST_F(BeaconTest, clearDataForwardsCallToBeaconCache)
{
	// expect
	EXPECT_CALL(*mockBeaconCache, deleteCacheEntry(BeaconKey_t(SESSION_ID, SESSION_SEQUENCE)))
		.Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->clearData();
}

TEST_F(BeaconTest, deviceIDIsRandomizedIfDeviceIdSendingDisallowed)
{
	// with
	ON_CALL(*mockPrivacyConfiguration, isDeviceIdSendingAllowed())
		.WillByDefault(testing::Return(false));

	int64_t deviceId = 1337;
	auto mockRandom = MockIPRNGenerator::createNice();
	ON_CALL(*mockRandom, nextPositiveInt64())
		.WillByDefault(testing::Return(deviceId));

	// expect
	EXPECT_CALL(*mockRandom, nextPositiveInt64())
		.Times(1);

	// given
	auto target = createBeacon()
		->with(mockRandom)
		.build();

	// when
	auto obtained = target->getDeviceID();

	// then
	ASSERT_THAT(obtained, testing::Eq(deviceId));
}

TEST_F(BeaconTest, givenDeviceIDIsUsedIfDeviceIdSendingIsAllowed)
{
	// with
	ON_CALL(*mockPrivacyConfiguration, isDeviceIdSendingAllowed())
		.WillByDefault(testing::Return(true));

	auto mockRandomStrict = MockIPRNGenerator::createStrict();

	// given
	auto target = createBeacon()
		->with(mockRandomStrict)
		.build();

	// when
	auto obtained = target->getDeviceID();

	// then
	ASSERT_THAT(obtained, testing::Eq(DEVICE_ID));
}

TEST_F(BeaconTest, sessionIDIsAlwaysValueOneIfSessionNumberReportingDisallowed)
{
	// with
	ON_CALL(*mockPrivacyConfiguration, isSessionNumberReportingAllowed())
		.WillByDefault(testing::Return(false));

	// given
	auto target = createBeacon()->build();

	// when
	auto obtained = target->getSessionNumber();

	// then
	ASSERT_THAT(obtained, testing::Eq(1));
}

TEST_F(BeaconTest, sessionIDIsValueFromSessionIDProviderIfSessionNumberReportingAllowed)
{
	// with
	ON_CALL(*mockPrivacyConfiguration, isSessionNumberReportingAllowed())
		.WillByDefault(testing::Return(true));

	// expect
	EXPECT_CALL(*mockSessionIDProvider, getNextSessionID())
		.Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	auto obtained = target->getSessionNumber();

	// then
	ASSERT_THAT(obtained, testing::Eq(SESSION_ID));
}

TEST_F(BeaconTest, updateServerConfigurationDelegatesToBeaconConfig)
{
	// with
	auto serverConfig = MockIServerConfiguration::createStrict();

	// expect
	EXPECT_CALL(*mockBeaconConfiguration, updateServerConfiguration(testing::Eq(serverConfig)))
		.Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->updateServerConfiguration(serverConfig);
}

TEST_F(BeaconTest, isServerConfigurationSetDelegatesToBeaconConfig)
{
	// with
	EXPECT_CALL(*mockBeaconConfiguration, isServerConfigurationSet())
		.Times(2)
		.WillOnce(testing::Return(false))
		.WillOnce(testing::Return(true));

	// given
	auto target = createBeacon()->build();

	// when
	auto obtained = target->isServerConfigurationSet();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));

	// and when
	obtained = target->isServerConfigurationSet();

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
}

TEST_F(BeaconTest, isCaptureEnabledReturnsValueFromServerConfig)
{
	// expect
	EXPECT_CALL(*mockServerConfiguration, isCaptureEnabled())
		.Times(2)
		.WillOnce(testing::Return(false))
		.WillOnce(testing::Return(true));

	// given
	auto target = createBeacon()->build();

	// when
	auto obtained = target->isCaptureEnabled();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));

	// and when
	obtained = target->isCaptureEnabled();

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
}

TEST_F(BeaconTest, enableCaptureDelegatesToBeaconConfig)
{
	// expect
	EXPECT_CALL(*mockBeaconConfiguration, enableCapture()).Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->enableCapture();
}

TEST_F(BeaconTest, disableCaptureDelegatesToBeaconConfig)
{
	// expect
	EXPECT_CALL(*mockBeaconConfiguration, disableCapture()).Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->disableCapture();
}

TEST_F(BeaconTest, useInternalBeaconIdForAccessingBeaconCacheWhenSessionNumberReportingDisabled)
{
	// with
	int32_t beaconId = 73;
	ON_CALL(*mockSessionIDProvider, getNextSessionID())
		.WillByDefault(testing::Return(beaconId));

	// expect
	EXPECT_CALL(*mockBeaconCache, deleteCacheEntry(BeaconKey_t(beaconId, SESSION_SEQUENCE)))
		.Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->clearData();
}

TEST_F(BeaconTest, onServerConfigurationUpdateAttachesEventOnBeaconConfiguration)
{
	// expect
	core::configuration::ServerConfigurationUpdateCallback serverConfigurationUpdateCallback = [](IServerConfiguration_sp config) {(void)config;};
	EXPECT_CALL(*mockBeaconConfiguration, setServerConfigurationUpdateCallback(testing::_))
		.Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->setServerConfigurationUpdateCallback(serverConfigurationUpdateCallback);
}

TEST_F(BeaconTest, onServerConfigurationUpdateDetachesEventOnBeaconConfiguration)
{
	// expect
	core::configuration::ServerConfigurationUpdateCallback serverConfigurationUpdateCallback = nullptr;
	EXPECT_CALL(*mockBeaconConfiguration, setServerConfigurationUpdateCallback(testing::_))
		.Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->setServerConfigurationUpdateCallback(serverConfigurationUpdateCallback);
}
