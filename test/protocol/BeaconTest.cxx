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

#include "OpenKitVersion.h"
#include "protocol/Beacon.h"
#include "OpenKit/CrashReportingLevel.h"
#include "OpenKit/DataCollectionLevel.h"
#include "OpenKit/json/JsonObjectValue.h"
#include "OpenKit/json/JsonStringValue.h"
#include "OpenKit/json/JsonArrayValue.h"
#include "OpenKit/json/JsonNumberValue.h"

#include "core/UTF8String.h"
#include "core/caching/BeaconCache.h"
#include "core/caching/BeaconKey.h"
#include "core/configuration/ConfigurationDefaults.h"
#include "core/objects/WebRequestTracer.h"
#include "core/objects/EventPayloadAttributes.h"
#include "core/util/StringUtil.h"
#include "core/util/URLEncoding.h"
#include "protocol/EventType.h"
#include "protocol/ProtocolConstants.h"
#include "protocol/BeaconProtocolConstants.h"

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
#include "../core/objects/mock/MockSessionInternals.h"
#include "../core/objects/mock/MockIWebRequestTracerInternals.h"
#include "../core/objects/mock/MockISupplementaryBasicData.h"
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
using MockIOpenKitConfiguration_sp = std::shared_ptr<MockIOpenKitConfiguration>;
using MockIPrivacyConfiguration_sp = std::shared_ptr<MockIPrivacyConfiguration>;
using MockIPRNGenerator_sp = std::shared_ptr<MockIPRNGenerator>;
using MockISupplementaryBasicData_sp = std::shared_ptr<MockISupplementaryBasicData>;
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
const Utf8String_t APP_VERSION("1.0");
const Utf8String_t OS_NAME("osName");
const Utf8String_t DEVICE_MANUFACTURER("deviceCompany");
const Utf8String_t MODEL_ID("model");
constexpr int32_t ACTION_ID = 17;
constexpr int32_t SERVER_ID = 1;
constexpr int64_t DEVICE_ID = 456;
constexpr int32_t THREAD_ID = 1234567;
constexpr int32_t SESSION_ID = 73;
constexpr int32_t SESSION_SEQUENCE = 13;
const Utf8String_t TRACER_URL("https://localhost");
const Utf8String_t ACTION_NAME("action name");
constexpr int32_t MULTIPLICITY = 1;

bool compareStr(const core::UTF8String& str1, const core::UTF8String& str2)
{
	return str1.getStringData().compare(str2.getStringData()) < 0;
}

MATCHER_P(ContainsString, includedString, "Should contain string")
{
	auto mainString = core::UTF8String(arg);

	auto result = mainString.getStringData().rfind(includedString) != std::string::npos;

	if (!result)
	{
		*result_listener << "String \"" << arg.getStringData() << "\" does not contain \"" << includedString << "\"";
	}

	return result;
}

MATCHER_P(IsEventMapEqual, expectedMap, "Event Map is matching")
{
	auto actualPayload = core::UTF8String(arg);
	auto expectedPayload = core::UTF8String(expectedMap);

	if (actualPayload.equals(expectedPayload))
	{
		return true;
	}

	if (!(actualPayload.getStringData().rfind("et=98&pl=", 0) == 0 && expectedPayload.getStringData().rfind("et=98&pl=", 0) == 0)) {
		*result_listener << "actualPayload or expectedPayload does not contain \"et=98&pl=\"";
		return false;
	}

	auto expectedPayloadDecoded = core::util::URLEncoding::urldecode(expectedPayload);
	auto actualPayloadDecoded = core::util::URLEncoding::urldecode(actualPayload);
	auto indexPl = expectedPayloadDecoded.getIndexOf("pl=") + 4;
	auto expectedPayloadCleaned = expectedPayloadDecoded.substring(indexPl, expectedPayloadDecoded.size() - 1 - indexPl);
	indexPl = actualPayloadDecoded.getIndexOf("pl=") + 4;
	auto actualPayloadCleaned = actualPayloadDecoded.substring(indexPl, actualPayloadDecoded.size() - 1 - indexPl);
	
	auto expectedMapStrVector = expectedPayloadCleaned.split(',');
	auto actualMapStrVector = actualPayloadCleaned.split(',');

	if (actualMapStrVector.size() != expectedMapStrVector.size())
	{
		*result_listener << "actualMapStrVector and expectedMapStrVector sizes differ "
			<< "(expected = " << expectedMapStrVector.size()
			<< ";actual = " << actualMapStrVector.size() << ")";
		return false;
	}

	std::sort(expectedMapStrVector.begin(), expectedMapStrVector.end(), compareStr);
	std::sort(actualMapStrVector.begin(), actualMapStrVector.end(), compareStr);

	auto comparator = [](const core::UTF8String& left, const core::UTF8String& right) {
		return left.equals(right);
	};

	auto result = std::equal(actualMapStrVector.begin(), actualMapStrVector.end(), expectedMapStrVector.begin(), comparator);
	if (!result)
	{
		*result_listener << "actualMapStrVector and expectedMapStrVector have different contents";
	}

	return result;
}

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
	MockISupplementaryBasicData_sp mockSupplementaryBasicData;

	MockILogger_sp mockLogger;
	MockIBeaconCache_sp mockBeaconCache;

	void SetUp() override
	{
		mockOpenKitConfiguration = MockIOpenKitConfiguration::createNice();
		ON_CALL(*mockOpenKitConfiguration, getApplicationId())
			.WillByDefault(testing::ReturnRef(APP_ID));
		ON_CALL(*mockOpenKitConfiguration, getApplicationIdPercentEncoded())
			.WillByDefault(testing::ReturnRef(APP_ID));
		ON_CALL(*mockOpenKitConfiguration, getApplicationVersion())
			.WillByDefault(testing::ReturnRef(APP_VERSION));
		ON_CALL(*mockOpenKitConfiguration, getDeviceId())
			.WillByDefault(testing::Return(DEVICE_ID));
		ON_CALL(*mockOpenKitConfiguration, getOperatingSystem())
			.WillByDefault(testing::ReturnRef(OS_NAME));
		ON_CALL(*mockOpenKitConfiguration, getManufacturer())
			.WillByDefault(testing::ReturnRef(DEVICE_MANUFACTURER));
		ON_CALL(*mockOpenKitConfiguration, getModelId())
			.WillByDefault(testing::ReturnRef(MODEL_ID));


		mockPrivacyConfiguration = MockIPrivacyConfiguration::createNice();

		mockServerConfiguration = MockIServerConfiguration::createNice();
		ON_CALL(*mockServerConfiguration, getServerId())
			.WillByDefault(testing::Return(SERVER_ID));
		ON_CALL(*mockServerConfiguration, getBeaconSizeInBytes())
			.WillByDefault(testing::Return(30 * 1024)); // 30 kB
		ON_CALL(*mockServerConfiguration, getTrafficControlPercentage())
			.WillByDefault(testing::Return(100)); // 100%
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
		mockSupplementaryBasicData = MockISupplementaryBasicData::createNice();

		mockLogger = MockILogger::createNice();
		mockBeaconCache = MockIBeaconCache::createStrict();
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
			.with(mockTimingProvider);

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
	ASSERT_THAT(target->isDataCapturingEnabled(), testing::Eq(true));
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

TEST_F(BeaconTest, getSessionStartTime)
{
	// given
	const int64_t startTime = 73;
	ON_CALL(*mockTimingProvider, provideTimestampInMilliseconds())
		.WillByDefault(testing::Return(startTime));

	auto target = createBeacon()->build();

	// when
	auto obtained = target->getSessionStartTime();

	// then
	ASSERT_THAT(obtained, testing::Eq(startTime));
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
		<< "&it=" << THREAD_ID						// thread ID
		<< "&na=" << actionName.getStringData()		// action name
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

TEST_F(BeaconTest, actionNotReportedIfDisallowedByTrafficControl)
{
	// expect
	EXPECT_CALL(*mockBeaconCache, addActionData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	auto actionMock = MockIActionCommon::createNice();
	ON_CALL(*actionMock, getName())
		.WillByDefault(testing::ReturnRef(ACTION_NAME));

	const auto trafficControlPercentage = 50;

	auto mockRandomGenerator = MockIPRNGenerator::createNice();
	ON_CALL(*mockRandomGenerator, nextPercentageValue())
		.WillByDefault(testing::Return(trafficControlPercentage));

	ON_CALL(*mockServerConfiguration, getTrafficControlPercentage())
		.WillByDefault(testing::Return(trafficControlPercentage));

	auto target = createBeacon()->with(mockRandomGenerator).build();

	// when
	target->addAction(actionMock);
}

TEST_F(BeaconTest, actionNotReportedIfActionReportingDisallowed)
{
	// with
	auto actionMock = MockIActionCommon::createNice();
	ON_CALL(*actionMock, getName())
		.WillByDefault(testing::ReturnRef(ACTION_NAME));

	ON_CALL(*mockPrivacyConfiguration, isActionReportingAllowed())
		.WillByDefault(testing::Return(false));

	// expect
	EXPECT_CALL(*mockBeaconCache, addActionData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	auto target = createBeacon()->build();

	// when, expect no interaction with beacon cache
	target->addAction(actionMock);
}

TEST_F(BeaconTest, addingNullActionThrowsException)
{
	// given
	auto target = createBeacon()->build();

	// then
	EXPECT_THROW(
		target->addAction(nullptr),
		std::invalid_argument
	);
}

TEST_F(BeaconTest, addingActionWithEmptyNameThrowsException)
{
	// given
	auto target = createBeacon()->build();

	auto actionName = Utf8String_t();
	auto actionMock = MockIActionCommon::createNice();
	ON_CALL(*actionMock, getName())
		.WillByDefault(testing::ReturnRef(actionName));

	// then
	EXPECT_THROW(
		target->addAction(actionMock),
		std::invalid_argument
	);
}

TEST_F(BeaconTest, actionNotReportedIfDataSendingDisallowed)
{
	// with
	auto actionMock = MockIActionCommon::createNice();
	ON_CALL(*actionMock, getName())
		.WillByDefault(testing::ReturnRef(ACTION_NAME));

	ON_CALL(*mockServerConfiguration, isSendingDataAllowed())
		.WillByDefault(testing::Return(false));

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

TEST_F(BeaconTest, noSessionStartIsReportedIfDataSendingDisallowed)
{
	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	//given
	ON_CALL(*mockServerConfiguration, isSendingDataAllowed())
		.WillByDefault(testing::Return(false));

	auto target = createBeacon()->build();

	// when
	target->startSession();
}

TEST_F(BeaconTest, noSessionStartIsReportedIfDisallowedByTrafficControl)
{
	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	const auto trafficControlPercentage = 50;

	auto mockRandomGenerator = MockIPRNGenerator::createNice();
	ON_CALL(*mockRandomGenerator, nextPercentageValue())
		.WillByDefault(testing::Return(trafficControlPercentage));

	ON_CALL(*mockServerConfiguration, getTrafficControlPercentage())
		.WillByDefault(testing::Return(trafficControlPercentage));

	auto target = createBeacon()->with(mockRandomGenerator).build();

	// when
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

TEST_F(BeaconTest, sessionNotAddedIfDataSendingDisallowed)
{
	// with
	ON_CALL(*mockServerConfiguration, isSendingDataAllowed())
		.WillByDefault(testing::Return(false));

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);
    EXPECT_CALL(*mockBeaconCache, addActionData(testing::_, testing::_, testing::_))
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

TEST_F(BeaconTest, sessionNotReportedIfDisallowedByTrafficControl)
{
	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	const auto trafficControlPercentage = 50;

	auto mockRandomGenerator = MockIPRNGenerator::createNice();
	ON_CALL(*mockRandomGenerator, nextPercentageValue())
		.WillByDefault(testing::Return(trafficControlPercentage));

	ON_CALL(*mockServerConfiguration, getTrafficControlPercentage())
		.WillByDefault(testing::Return(trafficControlPercentage));

	auto target = createBeacon()->with(mockRandomGenerator).build();

	// when
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
		<< "&it=" << THREAD_ID						// thread ID
		<< "&na=" << valueName.getStringData()		// name of reported value
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

TEST_F(BeaconTest, reportingInt32ValueWithEmptyValueNameThrowsException)
{
	// given
	auto target = createBeacon()->build();

	auto valueName = Utf8String_t();
	int32_t value = 42;

	EXPECT_THROW(
		target->reportValue(1, valueName, value),
		std::invalid_argument
	);
}

TEST_F(BeaconTest, int32ValueNotReportedIfDisallowedByTrafficControl)
{
	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	Utf8String_t valueName("Int32Value");
	int32_t value = 42;

	const auto trafficControlPercentage = 50;

	auto mockRandomGenerator = MockIPRNGenerator::createNice();
	ON_CALL(*mockRandomGenerator, nextPercentageValue())
		.WillByDefault(testing::Return(trafficControlPercentage));

	ON_CALL(*mockServerConfiguration, getTrafficControlPercentage())
		.WillByDefault(testing::Return(trafficControlPercentage));

	auto target = createBeacon()->with(mockRandomGenerator).build();

	// when
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
		<< "&it=" << THREAD_ID						// thread ID
		<< "&na=" << valueName.getStringData()		// name of reported value
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

TEST_F(BeaconTest, reportingInt64ValueWithEmptyValueNameThrowsException)
{
	// given
	auto target = createBeacon()->build();

	auto valueName = Utf8String_t();
	int64_t value = 42;

	EXPECT_THROW(
		target->reportValue(1, valueName, value),
		std::invalid_argument
	);
}

TEST_F(BeaconTest, int64ValueNotReportedIfDisallowedByTrafficControl)
{
	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	Utf8String_t valueName("Int64Value");
	int64_t value = 42;

	const auto trafficControlPercentage = 50;

	auto mockRandomGenerator = MockIPRNGenerator::createNice();
	ON_CALL(*mockRandomGenerator, nextPercentageValue())
		.WillByDefault(testing::Return(trafficControlPercentage));

	ON_CALL(*mockServerConfiguration, getTrafficControlPercentage())
		.WillByDefault(testing::Return(trafficControlPercentage));

	auto target = createBeacon()->with(mockRandomGenerator).build();

	// when
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
		<< "&it=" << THREAD_ID						// thread ID
		<< "&na=" << valueName.getStringData()		// name of reported value
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

TEST_F(BeaconTest, reportingDoubleValueWithEmptyValueNameThrowsException)
{
	// given
	auto target = createBeacon()->build();

	auto valueName = Utf8String_t();
	double value = 42.1337;

	EXPECT_THROW(
		target->reportValue(1, valueName, value),
		std::invalid_argument
	);
}

TEST_F(BeaconTest, doubleValueNotReportedIfDisallowedByTrafficControl)
{
	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	Utf8String_t valueName("DoubleValue");
	double value = 2.71;

	const auto trafficControlPercentage = 50;

	auto mockRandomGenerator = MockIPRNGenerator::createNice();
	ON_CALL(*mockRandomGenerator, nextPercentageValue())
		.WillByDefault(testing::Return(trafficControlPercentage));

	ON_CALL(*mockServerConfiguration, getTrafficControlPercentage())
		.WillByDefault(testing::Return(trafficControlPercentage));

	auto target = createBeacon()->with(mockRandomGenerator).build();

	// when
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
		<< "&it=" << THREAD_ID						// thread ID
		<< "&na=" << valueName.getStringData()		// name of reported value
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

TEST_F(BeaconTest, reportValueStringWithEmptyValue)
{
	// with
	Utf8String_t valueName("StringValue");
	auto value = Utf8String_t();

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::VALUE_STRING)	// event type
		<< "&it=" << THREAD_ID						// thread ID
		<< "&na=" << valueName.getStringData()		// name of reported value
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

TEST_F(BeaconTest, reportingStringValueWithEmptyValueNameThrowsException)
{
	// given
	auto target = createBeacon()->build();

	auto valueName = Utf8String_t();
	Utf8String_t value("HelloWorld");

	// then
	EXPECT_THROW(
		target->reportValue(1, valueName, value),
		std::invalid_argument
	);
}

TEST_F(BeaconTest, stringValueNotReportedIfDisallowedByTrafficControl)
{
	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	Utf8String_t valueName("StringValue");
	Utf8String_t value("HelloWorld");

	const auto trafficControlPercentage = 50;

	auto mockRandomGenerator = MockIPRNGenerator::createNice();
	ON_CALL(*mockRandomGenerator, nextPercentageValue())
		.WillByDefault(testing::Return(trafficControlPercentage));

	ON_CALL(*mockServerConfiguration, getTrafficControlPercentage())
		.WillByDefault(testing::Return(trafficControlPercentage));

	auto target = createBeacon()->with(mockRandomGenerator).build();

	// when
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
		<< "&it=" << THREAD_ID						// thread ID
		<< "&na=" << eventName.getStringData()		// name of event
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

TEST_F(BeaconTest, reportingEventWithEmptyEventNameThrowsException)
{
	// given
	auto target = createBeacon()->build();

	auto eventName = Utf8String_t();

	// then
	EXPECT_THROW(
		target->reportEvent(1, eventName),
		std::invalid_argument
	);
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

TEST_F(BeaconTest, namedEventNotReportedIfDisallowedByTrafficControl)
{
	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	Utf8String_t eventName("some event");

	const auto trafficControlPercentage = 50;

	auto mockRandomGenerator = MockIPRNGenerator::createNice();
	ON_CALL(*mockRandomGenerator, nextPercentageValue())
		.WillByDefault(testing::Return(trafficControlPercentage));

	ON_CALL(*mockServerConfiguration, getTrafficControlPercentage())
		.WillByDefault(testing::Return(trafficControlPercentage));

	auto target = createBeacon()->with(mockRandomGenerator).build();

	// when
	target->reportEvent(1, eventName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// report mutable supplementary basic data
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(BeaconTest, reportNetworkTechnology)
{
	// expect
	EXPECT_CALL(*mockBeaconCache, prepareDataForSending(testing::_))
		.Times(1);
	EXPECT_CALL(*mockBeaconCache, hasDataForSending(testing::_))
		.Times(1)
		.WillOnce(testing::Return(true));

	// given
	const int32_t sessionSequence = 1213;
	const int32_t visitStoreVersion = 2;
	const auto ipAddress = "192.168.0.1";
	const core::UTF8String appVersion{ "1111" };
	const core::UTF8String os{ "system" };
	const core::UTF8String manufacturer{ "manufacturer" };
	const core::UTF8String modelId{ "model" };
	const core::UTF8String networkTechnology{ "technology" };

	ON_CALL(*mockSupplementaryBasicData, getNetworkTechnology())
		.WillByDefault(testing::ReturnRef(networkTechnology));
	ON_CALL(*mockSupplementaryBasicData, isNetworkTechnologyAvailable())
		.WillByDefault(testing::Return(true));

	ON_CALL(*mockOpenKitConfiguration, getApplicationVersion())
		.WillByDefault(testing::ReturnRef(appVersion));
	ON_CALL(*mockOpenKitConfiguration, getOperatingSystem())
		.WillByDefault(testing::ReturnRef(os));
	ON_CALL(*mockOpenKitConfiguration, getManufacturer())
		.WillByDefault(testing::ReturnRef(manufacturer));
	ON_CALL(*mockOpenKitConfiguration, getModelId())
		.WillByDefault(testing::ReturnRef(modelId));
	ON_CALL(*mockServerConfiguration, getVisitStoreVersion())
		.WillByDefault(testing::Return(visitStoreVersion));

	// expect
	std::stringstream expectedPrefix;
	expectedPrefix << "vv=" << protocol::PROTOCOL_VERSION
		<< "&va=" << protocol::OPENKIT_VERSION
		<< "&ap=" << APP_ID.getStringData()
		<< "&vn=" << appVersion.getStringData()
		<< "&pt=" << protocol::PLATFORM_TYPE_OPENKIT
		<< "&tt=" << protocol::AGENT_TECHNOLOGY_TYPE
		<< "&vi=" << DEVICE_ID
		<< "&sn=" << SESSION_ID
		<< "&ip=" << ipAddress
		<< "&os=system"
		<< "&mf=manufacturer"
		<< "&md=model"
		<< "&dl=2"
		<< "&cl=2"
		<< "&vs=" << visitStoreVersion
		<< "&ss=" << sessionSequence
		<< "&tx=0"
		<< "&tv=0"
		<< "&mp=" << MULTIPLICITY
		<< "&np=" << networkTechnology.getStringData();

	const core::UTF8String expected{ expectedPrefix.str() };
	EXPECT_CALL(*mockBeaconCache, getNextBeaconChunk(testing::_, expected, testing::_, testing::_))
		.Times(1)
		.WillRepeatedly(testing::ReturnNull());

	// when
	auto target = createBeacon()->withIpAddress(ipAddress)
		.with(mockSupplementaryBasicData)
		.withSessionSequenceNumber(sessionSequence)
		.build();

	auto httpClientProvider = MockIHTTPClientProvider::createNice();
	target->send(httpClientProvider, *mockAdditionalQueryParameters);
}

TEST_F(BeaconTest, reportCarrier)
{
	// expect
	EXPECT_CALL(*mockBeaconCache, prepareDataForSending(testing::_))
		.Times(1);
	EXPECT_CALL(*mockBeaconCache, hasDataForSending(testing::_))
		.Times(1)
		.WillOnce(testing::Return(true));

	// given
	const int32_t sessionSequence = 1213;
	const int32_t visitStoreVersion = 2;
	const auto ipAddress = "192.168.0.1";
	const core::UTF8String appVersion{ "1111" };
	const core::UTF8String os{ "system" };
	const core::UTF8String manufacturer{ "manufacturer" };
	const core::UTF8String modelId{ "model" };
	const core::UTF8String carrier{ "carrier" };

	ON_CALL(*mockSupplementaryBasicData, getCarrier())
		.WillByDefault(testing::ReturnRef(carrier));
	ON_CALL(*mockSupplementaryBasicData, isCarrierAvailable())
		.WillByDefault(testing::Return(true));

	ON_CALL(*mockOpenKitConfiguration, getApplicationVersion())
		.WillByDefault(testing::ReturnRef(appVersion));
	ON_CALL(*mockOpenKitConfiguration, getOperatingSystem())
		.WillByDefault(testing::ReturnRef(os));
	ON_CALL(*mockOpenKitConfiguration, getManufacturer())
		.WillByDefault(testing::ReturnRef(manufacturer));
	ON_CALL(*mockOpenKitConfiguration, getModelId())
		.WillByDefault(testing::ReturnRef(modelId));
	ON_CALL(*mockServerConfiguration, getVisitStoreVersion())
		.WillByDefault(testing::Return(visitStoreVersion));

	// expect
	std::stringstream expectedPrefix;
	expectedPrefix << "vv=" << protocol::PROTOCOL_VERSION
		<< "&va=" << protocol::OPENKIT_VERSION
		<< "&ap=" << APP_ID.getStringData()
		<< "&vn=" << appVersion.getStringData()
		<< "&pt=" << protocol::PLATFORM_TYPE_OPENKIT
		<< "&tt=" << protocol::AGENT_TECHNOLOGY_TYPE
		<< "&vi=" << DEVICE_ID
		<< "&sn=" << SESSION_ID
		<< "&ip=" << ipAddress
		<< "&os=system"
		<< "&mf=manufacturer"
		<< "&md=model"
		<< "&dl=2"
		<< "&cl=2"
		<< "&vs=" << visitStoreVersion
		<< "&ss=" << sessionSequence
		<< "&tx=0"
		<< "&tv=0"
		<< "&mp=" << MULTIPLICITY
		<< "&cr=" << carrier.getStringData();

	const core::UTF8String expected{ expectedPrefix.str() };
	EXPECT_CALL(*mockBeaconCache, getNextBeaconChunk(testing::_, expected, testing::_, testing::_))
		.Times(1)
		.WillRepeatedly(testing::ReturnNull());

	// when
	auto target = createBeacon()->withIpAddress(ipAddress)
		.with(mockSupplementaryBasicData)
		.withSessionSequenceNumber(sessionSequence)
		.build();

	auto httpClientProvider = MockIHTTPClientProvider::createNice();
	target->send(httpClientProvider, *mockAdditionalQueryParameters);
}

TEST_F(BeaconTest, reportConnectionType)
{
	// expect
	EXPECT_CALL(*mockBeaconCache, prepareDataForSending(testing::_))
		.Times(1);
	EXPECT_CALL(*mockBeaconCache, hasDataForSending(testing::_))
		.Times(1)
		.WillOnce(testing::Return(true));

	// given
	const int32_t sessionSequence = 1213;
	const int32_t visitStoreVersion = 2;
	const auto ipAddress = "192.168.0.1";
	const core::UTF8String appVersion{ "1111" };
	const core::UTF8String os{ "system" };
	const core::UTF8String manufacturer{ "manufacturer" };
	const core::UTF8String modelId{ "model" };

	ON_CALL(*mockSupplementaryBasicData, getConnectionType())
		.WillByDefault(testing::Return(openkit::ConnectionType::MOBILE));
	ON_CALL(*mockSupplementaryBasicData, isConnectionTypeAvailable())
		.WillByDefault(testing::Return(true));

	ON_CALL(*mockOpenKitConfiguration, getApplicationVersion())
		.WillByDefault(testing::ReturnRef(appVersion));
	ON_CALL(*mockOpenKitConfiguration, getOperatingSystem())
		.WillByDefault(testing::ReturnRef(os));
	ON_CALL(*mockOpenKitConfiguration, getManufacturer())
		.WillByDefault(testing::ReturnRef(manufacturer));
	ON_CALL(*mockOpenKitConfiguration, getModelId())
		.WillByDefault(testing::ReturnRef(modelId));
	ON_CALL(*mockServerConfiguration, getVisitStoreVersion())
		.WillByDefault(testing::Return(visitStoreVersion));

	// expect
	std::stringstream expectedPrefix;
	expectedPrefix << "vv=" << protocol::PROTOCOL_VERSION
		<< "&va=" << protocol::OPENKIT_VERSION
		<< "&ap=" << APP_ID.getStringData()
		<< "&vn=" << appVersion.getStringData()
		<< "&pt=" << protocol::PLATFORM_TYPE_OPENKIT
		<< "&tt=" << protocol::AGENT_TECHNOLOGY_TYPE
		<< "&vi=" << DEVICE_ID
		<< "&sn=" << SESSION_ID
		<< "&ip=" << ipAddress
		<< "&os=system"
		<< "&mf=manufacturer"
		<< "&md=model"
		<< "&dl=2"
		<< "&cl=2"
		<< "&vs=" << visitStoreVersion
		<< "&ss=" << sessionSequence
		<< "&tx=0"
		<< "&tv=0"
		<< "&mp=" << MULTIPLICITY
		<< "&ct=m";

	const core::UTF8String expected{ expectedPrefix.str() };
	EXPECT_CALL(*mockBeaconCache, getNextBeaconChunk(testing::_, expected, testing::_, testing::_))
		.Times(1)
		.WillRepeatedly(testing::ReturnNull());

	// when
	auto target = createBeacon()->withIpAddress(ipAddress)
		.with(mockSupplementaryBasicData)
		.withSessionSequenceNumber(sessionSequence)
		.build();

	auto httpClientProvider = MockIHTTPClientProvider::createNice();
	target->send(httpClientProvider, *mockAdditionalQueryParameters);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// reportError tests (with error code)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(BeaconTest, reportErrorCode)
{
	// with
	Utf8String_t errorName("someEvent");
	int32_t errorCode = -123;

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::FAILURE_ERROR)	// event type
		<< "&it=" << THREAD_ID										// thread ID
		<< "&na=" << errorName.getStringData()						// name of error event
		<< "&pa=" << ACTION_ID										// parent action
		<< "&s0=1"													// sequence number of reported error
		<< "&t0=0"													// event time since session start
		<< "&ev=" << errorCode										// reported error value
		<< "&tt=c"													// error technology type
	;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when error was reported
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->reportError(ACTION_ID, errorName, errorCode);
}

TEST_F(BeaconTest, reportingErrorCodeWithEmptyNameThrowsException)
{
	// given
	auto errorName = Utf8String_t();
	int32_t errorCode = -123;

	auto target = createBeacon()->build();

	// then
	EXPECT_THROW(
		target->reportError(ACTION_ID, errorName, errorCode),
		std::invalid_argument
	);
}

TEST_F(BeaconTest, errorCodeNotReportedIfDataSendingDisallowed)
{
	// with
	ON_CALL(*mockServerConfiguration, isSendingDataAllowed())
		.WillByDefault(testing::Return(false));

	Utf8String_t eventName("event name");
	int32_t errorCode = 123;

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	auto target = createBeacon()->build();

	// when, expect no interaction with beacon cache
	target->reportError(ACTION_ID, eventName, errorCode);
}

TEST_F(BeaconTest, errorCodeNotReportedIfSendingErrorDataDisallowed)
{
	// with
	ON_CALL(*mockServerConfiguration, isSendingErrorsAllowed())
		.WillByDefault(testing::Return(false));

	Utf8String_t eventName("event name");
	int32_t errorCode = 123;

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	auto target = createBeacon()->build();

	// when, expect no interaction with beacon cache
	target->reportError(ACTION_ID, eventName, errorCode);
}

TEST_F(BeaconTest, errorCodeNotReportedIfErrorReportingDisallowed)
{
	// with
	ON_CALL(*mockPrivacyConfiguration, isErrorReportingAllowed())
		.WillByDefault(testing::Return(false));

	Utf8String_t eventName("error name");
	int32_t errorCode = 132;

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	//given
	auto target = createBeacon()->build();

	// when, expect no interaction with beacon cache
	target->reportError(ACTION_ID, eventName, errorCode);
}

TEST_F(BeaconTest, errorCodeNotReportedIfDisallowedByTrafficControl)
{
	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	const auto trafficControlPercentage = 50;

	auto mockRandomGenerator = MockIPRNGenerator::createNice();
	ON_CALL(*mockRandomGenerator, nextPercentageValue())
		.WillByDefault(testing::Return(trafficControlPercentage));

	ON_CALL(*mockServerConfiguration, getTrafficControlPercentage())
		.WillByDefault(testing::Return(trafficControlPercentage));

	auto target = createBeacon()->with(mockRandomGenerator).build();

	// when
	target->reportError(1, "DivByZeroError", 127);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// sendEvent
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(BeaconTest, sendEventWithPayload)
{
	// given
	Utf8String_t eventName("event name");
	auto mapWithName = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	mapWithName->insert({ "custom", openkit::json::JsonStringValue::fromString("CustomValue") });

	auto realMapPayload = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();

	realMapPayload->insert({ "custom", openkit::json::JsonStringValue::fromString("CustomValue") });

	realMapPayload->insert({ core::objects::TIMESTAMP, openkit::json::JsonNumberValue::fromLong(0)});
	realMapPayload->insert({ protocol::EVENT_PAYLOAD_APPLICATION_ID, openkit::json::JsonStringValue::fromString(APP_ID.getStringData()) });
	realMapPayload->insert({ protocol::EVENT_PAYLOAD_INSTANCE_ID, openkit::json::JsonStringValue::fromString(core::util::StringUtil::toInvariantString(DEVICE_ID)) });
	realMapPayload->insert({ protocol::EVENT_PAYLOAD_SESSION_ID, openkit::json::JsonStringValue::fromString(core::util::StringUtil::toInvariantString(SESSION_ID)) });
	realMapPayload->insert({ core::objects::APP_VERSION, openkit::json::JsonStringValue::fromString(APP_VERSION.getStringData()) });
	realMapPayload->insert({ core::objects::OS_NAME, openkit::json::JsonStringValue::fromString(OS_NAME.getStringData())});
	realMapPayload->insert({ core::objects::DEVICE_MANUFACTURER, openkit::json::JsonStringValue::fromString(DEVICE_MANUFACTURER.getStringData()) });
	realMapPayload->insert({ core::objects::DEVICE_MODEL_IDENTIFIER, openkit::json::JsonStringValue::fromString(MODEL_ID.getStringData()) });
	realMapPayload->insert({ protocol::EVENT_SCHEMA_VERSION, openkit::json::JsonStringValue::fromString("1.0") });
	realMapPayload->insert({ core::objects::EVENT_PROVIDER, openkit::json::JsonStringValue::fromString(APP_ID.getStringData()) });

	realMapPayload->insert({ "event.name", openkit::json::JsonStringValue::fromString("event name") });
	realMapPayload->insert({ "event.kind", openkit::json::JsonStringValue::fromString(core::objects::EVENT_KIND_RUM) });

	auto str = openkit::json::JsonObjectValue::fromMap(realMapPayload)->toString();

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::EVENT)	// event type
		<< "&pl=" << core::util::URLEncoding::urlencode(str, { '_' }).getStringData()	// payload
		;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when error was reported
		IsEventMapEqual(s.str())
	)).Times(1);

	auto target = createBeacon()->build();

	// then
	target->sendEvent(eventName, mapWithName);
}

TEST_F(BeaconTest, sendEventWithNameInPayload)
{
	// given
	Utf8String_t eventName("event name");
	auto mapWithName = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	mapWithName->insert({ "event.name", openkit::json::JsonStringValue::fromString("trying to override") });

	auto realMapPayload = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	
	realMapPayload->insert({ core::objects::TIMESTAMP, openkit::json::JsonNumberValue::fromLong(0) });
	realMapPayload->insert({ protocol::EVENT_PAYLOAD_APPLICATION_ID, openkit::json::JsonStringValue::fromString(APP_ID.getStringData()) });
	realMapPayload->insert({ protocol::EVENT_PAYLOAD_INSTANCE_ID, openkit::json::JsonStringValue::fromString(core::util::StringUtil::toInvariantString(DEVICE_ID)) });
	realMapPayload->insert({ protocol::EVENT_PAYLOAD_SESSION_ID, openkit::json::JsonStringValue::fromString(core::util::StringUtil::toInvariantString(SESSION_ID)) });
	realMapPayload->insert({ core::objects::APP_VERSION, openkit::json::JsonStringValue::fromString(APP_VERSION.getStringData()) });
	realMapPayload->insert({ core::objects::OS_NAME, openkit::json::JsonStringValue::fromString(OS_NAME.getStringData()) });
	realMapPayload->insert({ core::objects::DEVICE_MANUFACTURER, openkit::json::JsonStringValue::fromString(DEVICE_MANUFACTURER.getStringData()) });
	realMapPayload->insert({ core::objects::DEVICE_MODEL_IDENTIFIER, openkit::json::JsonStringValue::fromString(MODEL_ID.getStringData()) });
	realMapPayload->insert({ protocol::EVENT_SCHEMA_VERSION, openkit::json::JsonStringValue::fromString("1.0") });
	realMapPayload->insert({ core::objects::EVENT_PROVIDER, openkit::json::JsonStringValue::fromString(APP_ID.getStringData()) });

	realMapPayload->insert({ "event.name", openkit::json::JsonStringValue::fromString("event name") });
	realMapPayload->insert({ "event.kind", openkit::json::JsonStringValue::fromString(core::objects::EVENT_KIND_RUM) });

	auto str = openkit::json::JsonObjectValue::fromMap(realMapPayload)->toString();

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::EVENT)	// event type
		<< "&pl=" << core::util::URLEncoding::urlencode(str, { '_' }).getStringData()	// payload
		;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when error was reported
		IsEventMapEqual(s.str())
	)).Times(1);

	auto target = createBeacon()->build();

	// then
	target->sendEvent(eventName, mapWithName);
}

TEST_F(BeaconTest, sendEventWithEventKindInPayload)
{
	// given
	Utf8String_t eventName("event name");
	auto mapWithName = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	mapWithName->insert({ "event.kind", openkit::json::JsonStringValue::fromString("trying to override") });

	auto realMapPayload = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();

	realMapPayload->insert({ core::objects::TIMESTAMP, openkit::json::JsonNumberValue::fromLong(0) });
	realMapPayload->insert({ protocol::EVENT_PAYLOAD_APPLICATION_ID, openkit::json::JsonStringValue::fromString(APP_ID.getStringData()) });
	realMapPayload->insert({ protocol::EVENT_PAYLOAD_INSTANCE_ID, openkit::json::JsonStringValue::fromString(core::util::StringUtil::toInvariantString(DEVICE_ID)) });
	realMapPayload->insert({ protocol::EVENT_PAYLOAD_SESSION_ID, openkit::json::JsonStringValue::fromString(core::util::StringUtil::toInvariantString(SESSION_ID)) });
	realMapPayload->insert({ core::objects::APP_VERSION, openkit::json::JsonStringValue::fromString(APP_VERSION.getStringData()) });
	realMapPayload->insert({ core::objects::OS_NAME, openkit::json::JsonStringValue::fromString(OS_NAME.getStringData()) });
	realMapPayload->insert({ core::objects::DEVICE_MANUFACTURER, openkit::json::JsonStringValue::fromString(DEVICE_MANUFACTURER.getStringData()) });
	realMapPayload->insert({ core::objects::DEVICE_MODEL_IDENTIFIER, openkit::json::JsonStringValue::fromString(MODEL_ID.getStringData()) });
	realMapPayload->insert({ protocol::EVENT_SCHEMA_VERSION, openkit::json::JsonStringValue::fromString("1.0") });
	realMapPayload->insert({ core::objects::EVENT_PROVIDER, openkit::json::JsonStringValue::fromString(APP_ID.getStringData()) });

	realMapPayload->insert({ "event.name", openkit::json::JsonStringValue::fromString("event name") });
	realMapPayload->insert({ "event.kind", openkit::json::JsonStringValue::fromString("trying to override") });

	auto jsonValues = std::make_shared<openkit::json::JsonArrayValue::JsonValueList>();
	jsonValues->push_back(openkit::json::JsonStringValue::fromString("event.kind"));
	
	auto str = openkit::json::JsonObjectValue::fromMap(realMapPayload)->toString();

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::EVENT)	// event type
		<< "&pl=" << core::util::URLEncoding::urlencode(str, { '_' }).getStringData()	// payload
		;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when error was reported
		IsEventMapEqual(s.str())
	)).Times(1);

	auto target = createBeacon()->build();

	// then
	target->sendEvent(eventName, mapWithName);
}

TEST_F(BeaconTest, sendValidEventTryingToOverrideDtValuesWhichAreNotAllowed)
{
	// given
	Utf8String_t eventName("event name");
	auto mapWithName = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();

	mapWithName->insert({ protocol::EVENT_PAYLOAD_APPLICATION_ID, openkit::json::JsonStringValue::fromString("trying to override") });
	mapWithName->insert({ protocol::EVENT_PAYLOAD_INSTANCE_ID, openkit::json::JsonStringValue::fromString("trying to override") });
	mapWithName->insert({ protocol::EVENT_PAYLOAD_SESSION_ID, openkit::json::JsonStringValue::fromString("trying to override") });
	mapWithName->insert({ protocol::EVENT_SCHEMA_VERSION, openkit::json::JsonStringValue::fromString("trying to override") });

	auto realMapPayload = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();

	realMapPayload->insert({ core::objects::TIMESTAMP, openkit::json::JsonNumberValue::fromLong(0) });
	realMapPayload->insert({ protocol::EVENT_PAYLOAD_APPLICATION_ID, openkit::json::JsonStringValue::fromString(APP_ID.getStringData()) });
	realMapPayload->insert({ protocol::EVENT_PAYLOAD_INSTANCE_ID, openkit::json::JsonStringValue::fromString(core::util::StringUtil::toInvariantString(DEVICE_ID)) });
	realMapPayload->insert({ protocol::EVENT_PAYLOAD_SESSION_ID, openkit::json::JsonStringValue::fromString(core::util::StringUtil::toInvariantString(SESSION_ID)) });
	realMapPayload->insert({ protocol::EVENT_SCHEMA_VERSION, openkit::json::JsonStringValue::fromString("1.0") });
	realMapPayload->insert({ core::objects::APP_VERSION, openkit::json::JsonStringValue::fromString(APP_VERSION.getStringData()) });
	realMapPayload->insert({ core::objects::OS_NAME, openkit::json::JsonStringValue::fromString(OS_NAME.getStringData()) });
	realMapPayload->insert({ core::objects::DEVICE_MANUFACTURER, openkit::json::JsonStringValue::fromString(DEVICE_MANUFACTURER.getStringData()) });
	realMapPayload->insert({ core::objects::DEVICE_MODEL_IDENTIFIER, openkit::json::JsonStringValue::fromString(MODEL_ID.getStringData()) });
	realMapPayload->insert({ core::objects::EVENT_PROVIDER, openkit::json::JsonStringValue::fromString(APP_ID.getStringData()) });

	realMapPayload->insert({ "event.name", openkit::json::JsonStringValue::fromString("event name") });
	realMapPayload->insert({ "event.kind", openkit::json::JsonStringValue::fromString(core::objects::EVENT_KIND_RUM) });

	auto str = openkit::json::JsonObjectValue::fromMap(realMapPayload)->toString();

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::EVENT)	// event type
		<< "&pl=" << core::util::URLEncoding::urlencode(str, { '_' }).getStringData()	// payload
		;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when error was reported
		IsEventMapEqual(s.str())
	)).Times(1);

	auto target = createBeacon()->build();

	// then
	target->sendEvent(eventName, mapWithName);
}

TEST_F(BeaconTest, sendEventWithEmptyPayload)
{
	// given
	Utf8String_t eventName("event name");
	auto emptyMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();

	auto realMapPayload = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	
	realMapPayload->insert({ core::objects::TIMESTAMP, openkit::json::JsonNumberValue::fromLong(0) });
	realMapPayload->insert({ protocol::EVENT_PAYLOAD_APPLICATION_ID, openkit::json::JsonStringValue::fromString(APP_ID.getStringData()) });
	realMapPayload->insert({ protocol::EVENT_PAYLOAD_INSTANCE_ID, openkit::json::JsonStringValue::fromString(core::util::StringUtil::toInvariantString(DEVICE_ID)) });
	realMapPayload->insert({ protocol::EVENT_PAYLOAD_SESSION_ID, openkit::json::JsonStringValue::fromString(core::util::StringUtil::toInvariantString(SESSION_ID)) });
	realMapPayload->insert({ core::objects::APP_VERSION, openkit::json::JsonStringValue::fromString(APP_VERSION.getStringData()) });
	realMapPayload->insert({ core::objects::OS_NAME, openkit::json::JsonStringValue::fromString(OS_NAME.getStringData()) });
	realMapPayload->insert({ core::objects::DEVICE_MANUFACTURER, openkit::json::JsonStringValue::fromString(DEVICE_MANUFACTURER.getStringData()) });
	realMapPayload->insert({ core::objects::DEVICE_MODEL_IDENTIFIER, openkit::json::JsonStringValue::fromString(MODEL_ID.getStringData()) });
	realMapPayload->insert({ protocol::EVENT_SCHEMA_VERSION, openkit::json::JsonStringValue::fromString("1.0") });
	realMapPayload->insert({ core::objects::EVENT_PROVIDER, openkit::json::JsonStringValue::fromString(APP_ID.getStringData()) });

	realMapPayload->insert({ "event.name", openkit::json::JsonStringValue::fromString("event name") });
	realMapPayload->insert({ "event.kind", openkit::json::JsonStringValue::fromString(core::objects::EVENT_KIND_RUM) });

	auto str = openkit::json::JsonObjectValue::fromMap(realMapPayload)->toString();

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::EVENT)	// event type
		<< "&pl=" << core::util::URLEncoding::urlencode(str, { '_' }).getStringData()	// payload
		;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when error was reported
		IsEventMapEqual(s.str())
	)).Times(1);

	auto target = createBeacon()->build();

	// then
	target->sendEvent(eventName, emptyMap);
}

TEST_F(BeaconTest, sendEventWithNullPtrPayload)
{
	// given
	Utf8String_t eventName("event name");

	auto realMapPayload = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	
	realMapPayload->insert({ core::objects::TIMESTAMP, openkit::json::JsonNumberValue::fromLong(0) });
	realMapPayload->insert({ protocol::EVENT_PAYLOAD_APPLICATION_ID, openkit::json::JsonStringValue::fromString(APP_ID.getStringData()) });
	realMapPayload->insert({ protocol::EVENT_PAYLOAD_INSTANCE_ID, openkit::json::JsonStringValue::fromString(core::util::StringUtil::toInvariantString(DEVICE_ID)) });
	realMapPayload->insert({ protocol::EVENT_PAYLOAD_SESSION_ID, openkit::json::JsonStringValue::fromString(core::util::StringUtil::toInvariantString(SESSION_ID)) });
	realMapPayload->insert({ core::objects::APP_VERSION, openkit::json::JsonStringValue::fromString(APP_VERSION.getStringData()) });
	realMapPayload->insert({ core::objects::OS_NAME, openkit::json::JsonStringValue::fromString(OS_NAME.getStringData()) });
	realMapPayload->insert({ core::objects::DEVICE_MANUFACTURER, openkit::json::JsonStringValue::fromString(DEVICE_MANUFACTURER.getStringData()) });
	realMapPayload->insert({ core::objects::DEVICE_MODEL_IDENTIFIER, openkit::json::JsonStringValue::fromString(MODEL_ID.getStringData()) });
	realMapPayload->insert({ protocol::EVENT_SCHEMA_VERSION, openkit::json::JsonStringValue::fromString("1.0") });
	realMapPayload->insert({ core::objects::EVENT_PROVIDER, openkit::json::JsonStringValue::fromString(APP_ID.getStringData()) });

	realMapPayload->insert({ "event.name", openkit::json::JsonStringValue::fromString("event name") });
	realMapPayload->insert({ "event.kind", openkit::json::JsonStringValue::fromString(core::objects::EVENT_KIND_RUM) });

	auto str = openkit::json::JsonObjectValue::fromMap(realMapPayload)->toString();

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::EVENT)	// event type
		<< "&pl=" << core::util::URLEncoding::urlencode(str, { '_' }).getStringData()	// payload
		;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when error was reported
		IsEventMapEqual(s.str())
	)).Times(1);

	auto target = createBeacon()->build();

	// then
	target->sendEvent(eventName, nullptr);
}

TEST_F(BeaconTest, sendValidEventTryingToOverrideTimestamp)
{
	// given
	Utf8String_t eventName("event name");
	auto map = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	map->insert({ core::objects::TIMESTAMP, openkit::json::JsonStringValue::fromString("Test") });

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when error was reported
		ContainsString("timestamp%22%3A%22Test%22")
	)).Times(1);

	auto target = createBeacon()->build();

	// then
	target->sendEvent(eventName, map);
}

TEST_F(BeaconTest, sendValidEventTryingToOverrideAppVersion)
{
	// given
	Utf8String_t eventName("event name");
	auto map = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	map->insert({ core::objects::APP_VERSION, openkit::json::JsonStringValue::fromString("Test") });

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when error was reported
		ContainsString("app.version%22%3A%22Test%22")
	)).Times(1);

	auto target = createBeacon()->build();

	// then
	target->sendEvent(eventName, map);
}

TEST_F(BeaconTest, sendValidEventTryingToOverrideOsName)
{
	// given
	Utf8String_t eventName("event name");
	auto map = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	map->insert({ core::objects::OS_NAME, openkit::json::JsonStringValue::fromString("Test") });

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when error was reported
		ContainsString("os.name%22%3A%22Test%22")
	)).Times(1);

	auto target = createBeacon()->build();

	// then
	target->sendEvent(eventName, map);
}

TEST_F(BeaconTest, sendValidEventTryingToOverrideDeviceManufacturer)
{
	// given
	Utf8String_t eventName("event name");
	auto map = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	map->insert({ core::objects::DEVICE_MANUFACTURER, openkit::json::JsonStringValue::fromString("Test") });

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when error was reported
		ContainsString("device.manufacturer%22%3A%22Test%22")
	)).Times(1);

	auto target = createBeacon()->build();

	// then
	target->sendEvent(eventName, map);
}

TEST_F(BeaconTest, sendValidEventTryingToOverrideDeviceModelIdentifier)
{
	// given
	Utf8String_t eventName("event name");
	auto map = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	map->insert({ core::objects::DEVICE_MODEL_IDENTIFIER, openkit::json::JsonStringValue::fromString("Test") });

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when error was reported
		ContainsString("device.model.identifier%22%3A%22Test%22")
	)).Times(1);

	auto target = createBeacon()->build();

	// then
	target->sendEvent(eventName, map);
}

TEST_F(BeaconTest, sendValidEventTryingToOverrideEventProvider)
{
	// given
	Utf8String_t eventName("event name");
	auto map = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	map->insert({ core::objects::EVENT_PROVIDER, openkit::json::JsonStringValue::fromString("Test") });

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when error was reported
		ContainsString("event.provider%22%3A%22Test%22")
	)).Times(1);

	auto target = createBeacon()->build();

	// then
	target->sendEvent(eventName, map);
}


TEST_F(BeaconTest, sendEventWithEmptyEventNameThrowsException)
{
	// given
	auto emptyMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	auto target = createBeacon()->build();

	// then
	EXPECT_THROW(
		target->sendEvent(Utf8String_t(), emptyMap),
		std::invalid_argument
	);
}

TEST_F(BeaconTest, sendEventNotReportedIfDataSendingDisallowed)
{
	// with
	ON_CALL(*mockServerConfiguration, isSendingDataAllowed())
		.WillByDefault(testing::Return(false));

	// given
	Utf8String_t eventName("event name");
	auto emptyMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	auto target = createBeacon()->build();

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	// when, expect no interaction with beacon cache
	target->sendEvent(eventName, emptyMap);
}

TEST_F(BeaconTest, sendEventNotReportedIfSendingEventDataDisallowed)
{
	// with
	ON_CALL(*mockPrivacyConfiguration, isEventReportingAllowed())
		.WillByDefault(testing::Return(false));

	Utf8String_t eventName("event name");
	auto emptyMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	auto target = createBeacon()->build();

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	// when, expect no interaction with beacon cache
	target->sendEvent(eventName, emptyMap);
}

TEST_F(BeaconTest, sendEventNotReportIfDisallowedByTrafficControl)
{
	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	Utf8String_t eventName("event name");
	auto emptyMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	const auto trafficControlPercentage = 50;

	auto mockRandomGenerator = MockIPRNGenerator::createNice();
	ON_CALL(*mockRandomGenerator, nextPercentageValue())
		.WillByDefault(testing::Return(trafficControlPercentage));

	ON_CALL(*mockServerConfiguration, getTrafficControlPercentage())
		.WillByDefault(testing::Return(trafficControlPercentage));

	auto target = createBeacon()->with(mockRandomGenerator).build();

	// when
	target->sendEvent(eventName, emptyMap);
}

TEST_F(BeaconTest, sendEventPayloadIsTooBig)
{
	// given
	Utf8String_t eventName("event name");
	auto bigMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();

	for (int i = 0; i < 1000; i++)
	{
		bigMap->insert({ "KeyName" + std::to_string(i), openkit::json::JsonStringValue::fromString("Test " + std::to_string(i)) });
	}

	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);
	
	auto target = createBeacon()->build();

	// then
	EXPECT_THROW(
		target->sendEvent(eventName, bigMap),
		std::invalid_argument
	);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// sendBizEvent
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(BeaconTest, sendBizEventWithPayload)
{
	// given
	Utf8String_t eventType("event type");
	
	auto mapWithName = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	mapWithName->insert({ "custom", openkit::json::JsonStringValue::fromString("CustomValue") });

	auto realMapPayload = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();

	realMapPayload->insert({ "custom", openkit::json::JsonStringValue::fromString("CustomValue") });

	realMapPayload->insert({ core::objects::TIMESTAMP, openkit::json::JsonNumberValue::fromLong(0) });
	realMapPayload->insert({ protocol::EVENT_PAYLOAD_APPLICATION_ID, openkit::json::JsonStringValue::fromString(APP_ID.getStringData()) });
	realMapPayload->insert({ protocol::EVENT_PAYLOAD_INSTANCE_ID, openkit::json::JsonStringValue::fromString(core::util::StringUtil::toInvariantString(DEVICE_ID)) });
	realMapPayload->insert({ protocol::EVENT_PAYLOAD_SESSION_ID, openkit::json::JsonStringValue::fromString(core::util::StringUtil::toInvariantString(SESSION_ID)) });
	realMapPayload->insert({ core::objects::APP_VERSION, openkit::json::JsonStringValue::fromString(APP_VERSION.getStringData()) });
	realMapPayload->insert({ core::objects::OS_NAME, openkit::json::JsonStringValue::fromString(OS_NAME.getStringData()) });
	realMapPayload->insert({ core::objects::DEVICE_MANUFACTURER, openkit::json::JsonStringValue::fromString(DEVICE_MANUFACTURER.getStringData()) });
	realMapPayload->insert({ core::objects::DEVICE_MODEL_IDENTIFIER, openkit::json::JsonStringValue::fromString(MODEL_ID.getStringData()) });
	realMapPayload->insert({ protocol::EVENT_SCHEMA_VERSION, openkit::json::JsonStringValue::fromString("1.0") });
	realMapPayload->insert({ core::objects::EVENT_PROVIDER, openkit::json::JsonStringValue::fromString(APP_ID.getStringData()) });

	realMapPayload->insert({ "event.name", openkit::json::JsonStringValue::fromString("event type") });
	realMapPayload->insert({ "event.type", openkit::json::JsonStringValue::fromString("event type") });
	realMapPayload->insert({ "dt.rum.custom_attributes_size", openkit::json::JsonNumberValue::fromLong(50) });
	realMapPayload->insert({ "event.kind", openkit::json::JsonStringValue::fromString(core::objects::EVENT_KIND_BIZ) });

	auto str = openkit::json::JsonObjectValue::fromMap(realMapPayload)->toString();

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::EVENT)	// event type
		<< "&pl=" << core::util::URLEncoding::urlencode(str, { '_' }).getStringData()	// payload
		;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when error was reported
		IsEventMapEqual(s.str())
	)).Times(1);

	auto target = createBeacon()->build();

	// then
	target->sendBizEvent(eventType, mapWithName);
}

TEST_F(BeaconTest, sendBizEventWithNameInPayload)
{
	// given
	Utf8String_t eventType("event type");
	auto mapWithName = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	mapWithName->insert({ "event.name", openkit::json::JsonStringValue::fromString("trying to override") });

	auto realMapPayload = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();

	realMapPayload->insert({ core::objects::TIMESTAMP, openkit::json::JsonNumberValue::fromLong(0) });
	realMapPayload->insert({ protocol::EVENT_PAYLOAD_APPLICATION_ID, openkit::json::JsonStringValue::fromString(APP_ID.getStringData()) });
	realMapPayload->insert({ protocol::EVENT_PAYLOAD_INSTANCE_ID, openkit::json::JsonStringValue::fromString(core::util::StringUtil::toInvariantString(DEVICE_ID)) });
	realMapPayload->insert({ protocol::EVENT_PAYLOAD_SESSION_ID, openkit::json::JsonStringValue::fromString(core::util::StringUtil::toInvariantString(SESSION_ID)) });
	realMapPayload->insert({ core::objects::APP_VERSION, openkit::json::JsonStringValue::fromString(APP_VERSION.getStringData()) });
	realMapPayload->insert({ core::objects::OS_NAME, openkit::json::JsonStringValue::fromString(OS_NAME.getStringData()) });
	realMapPayload->insert({ core::objects::DEVICE_MANUFACTURER, openkit::json::JsonStringValue::fromString(DEVICE_MANUFACTURER.getStringData()) });
	realMapPayload->insert({ core::objects::DEVICE_MODEL_IDENTIFIER, openkit::json::JsonStringValue::fromString(MODEL_ID.getStringData()) });
	realMapPayload->insert({ protocol::EVENT_SCHEMA_VERSION, openkit::json::JsonStringValue::fromString("1.0") });
	realMapPayload->insert({ core::objects::EVENT_PROVIDER, openkit::json::JsonStringValue::fromString(APP_ID.getStringData()) });

	realMapPayload->insert({ "event.name", openkit::json::JsonStringValue::fromString("trying to override") });
	realMapPayload->insert({ "event.type", openkit::json::JsonStringValue::fromString("event type") });
	realMapPayload->insert({ "dt.rum.custom_attributes_size", openkit::json::JsonNumberValue::fromLong(61) });
	realMapPayload->insert({ "event.kind", openkit::json::JsonStringValue::fromString(core::objects::EVENT_KIND_BIZ) });

	auto str = openkit::json::JsonObjectValue::fromMap(realMapPayload)->toString();

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::EVENT)	// event type
		<< "&pl=" << core::util::URLEncoding::urlencode(str, { '_' }).getStringData()	// payload
		;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when error was reported
		IsEventMapEqual(s.str())
	)).Times(1);

	auto target = createBeacon()->build();

	// then
	target->sendBizEvent(eventType, mapWithName);
}

TEST_F(BeaconTest, sendValidBizEventTryingToOverrideDtValuesWhichAreNotAllowed)
{
	// given
	Utf8String_t eventType("event type");
	auto mapWithName = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();

	mapWithName->insert({ protocol::EVENT_PAYLOAD_APPLICATION_ID, openkit::json::JsonStringValue::fromString("trying to override") });
	mapWithName->insert({ protocol::EVENT_PAYLOAD_INSTANCE_ID, openkit::json::JsonStringValue::fromString("trying to override") });
	mapWithName->insert({ protocol::EVENT_PAYLOAD_SESSION_ID, openkit::json::JsonStringValue::fromString("trying to override") });
	mapWithName->insert({ "event.kind", openkit::json::JsonStringValue::fromString("trying to override") });
	mapWithName->insert({ protocol::EVENT_SCHEMA_VERSION, openkit::json::JsonStringValue::fromString("trying to override") });
	mapWithName->insert({ "dt.rum.custom_attributes_size", openkit::json::JsonStringValue::fromString("trying to override") });

	auto realMapPayload = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();

	realMapPayload->insert({ core::objects::TIMESTAMP, openkit::json::JsonNumberValue::fromLong(0) });
	realMapPayload->insert({ protocol::EVENT_PAYLOAD_APPLICATION_ID, openkit::json::JsonStringValue::fromString(APP_ID.getStringData()) });
	realMapPayload->insert({ protocol::EVENT_PAYLOAD_INSTANCE_ID, openkit::json::JsonStringValue::fromString(core::util::StringUtil::toInvariantString(DEVICE_ID)) });
	realMapPayload->insert({ protocol::EVENT_PAYLOAD_SESSION_ID, openkit::json::JsonStringValue::fromString(core::util::StringUtil::toInvariantString(SESSION_ID)) });
	realMapPayload->insert({ core::objects::APP_VERSION, openkit::json::JsonStringValue::fromString(APP_VERSION.getStringData()) });
	realMapPayload->insert({ core::objects::OS_NAME, openkit::json::JsonStringValue::fromString(OS_NAME.getStringData()) });
	realMapPayload->insert({ core::objects::DEVICE_MANUFACTURER, openkit::json::JsonStringValue::fromString(DEVICE_MANUFACTURER.getStringData()) });
	realMapPayload->insert({ core::objects::DEVICE_MODEL_IDENTIFIER, openkit::json::JsonStringValue::fromString(MODEL_ID.getStringData()) });
	realMapPayload->insert({ protocol::EVENT_SCHEMA_VERSION, openkit::json::JsonStringValue::fromString("1.0") });
	realMapPayload->insert({ core::objects::EVENT_PROVIDER, openkit::json::JsonStringValue::fromString(APP_ID.getStringData()) });

	realMapPayload->insert({ "event.name", openkit::json::JsonStringValue::fromString("event type") });
	realMapPayload->insert({ "event.type", openkit::json::JsonStringValue::fromString("event type") });
	realMapPayload->insert({ "dt.rum.custom_attributes_size", openkit::json::JsonNumberValue::fromLong(280) });
	realMapPayload->insert({ "event.kind", openkit::json::JsonStringValue::fromString(core::objects::EVENT_KIND_BIZ) });

	auto str = openkit::json::JsonObjectValue::fromMap(realMapPayload)->toString();

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::EVENT)	// event type
		<< "&pl=" << core::util::URLEncoding::urlencode(str, { '_' }).getStringData()	// payload
		;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when error was reported
		IsEventMapEqual(s.str())
	)).Times(1);

	auto target = createBeacon()->build();

	// then
	target->sendBizEvent(eventType, mapWithName);
}

TEST_F(BeaconTest, sendBizEventWithEmptyPayload)
{
	// given
	Utf8String_t eventType("event type");
	auto emptyMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();

	auto realMapPayload = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();

	realMapPayload->insert({ core::objects::TIMESTAMP, openkit::json::JsonNumberValue::fromLong(0) });
	realMapPayload->insert({ protocol::EVENT_PAYLOAD_APPLICATION_ID, openkit::json::JsonStringValue::fromString(APP_ID.getStringData()) });
	realMapPayload->insert({ protocol::EVENT_PAYLOAD_INSTANCE_ID, openkit::json::JsonStringValue::fromString(core::util::StringUtil::toInvariantString(DEVICE_ID)) });
	realMapPayload->insert({ protocol::EVENT_PAYLOAD_SESSION_ID, openkit::json::JsonStringValue::fromString(core::util::StringUtil::toInvariantString(SESSION_ID)) });
	realMapPayload->insert({ core::objects::APP_VERSION, openkit::json::JsonStringValue::fromString(APP_VERSION.getStringData()) });
	realMapPayload->insert({ core::objects::OS_NAME, openkit::json::JsonStringValue::fromString(OS_NAME.getStringData()) });
	realMapPayload->insert({ core::objects::DEVICE_MANUFACTURER, openkit::json::JsonStringValue::fromString(DEVICE_MANUFACTURER.getStringData()) });
	realMapPayload->insert({ core::objects::DEVICE_MODEL_IDENTIFIER, openkit::json::JsonStringValue::fromString(MODEL_ID.getStringData()) });
	realMapPayload->insert({ protocol::EVENT_SCHEMA_VERSION, openkit::json::JsonStringValue::fromString("1.0") });
	realMapPayload->insert({ core::objects::EVENT_PROVIDER, openkit::json::JsonStringValue::fromString(APP_ID.getStringData()) });

	realMapPayload->insert({ "event.name", openkit::json::JsonStringValue::fromString("event type") });
	realMapPayload->insert({ "event.type", openkit::json::JsonStringValue::fromString("event type") });
	realMapPayload->insert({ "dt.rum.custom_attributes_size", openkit::json::JsonNumberValue::fromLong(27) });
	realMapPayload->insert({ "event.kind", openkit::json::JsonStringValue::fromString(core::objects::EVENT_KIND_BIZ) });

	auto str = openkit::json::JsonObjectValue::fromMap(realMapPayload)->toString();

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::EVENT)	// event type
		<< "&pl=" << core::util::URLEncoding::urlencode(str, { '_' }).getStringData()	// payload
		;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when error was reported
		IsEventMapEqual(s.str())
	)).Times(1);

	auto target = createBeacon()->build();

	// then
	target->sendBizEvent(eventType, emptyMap);
}

TEST_F(BeaconTest, sendBizEventWithNullPtrPayload)
{
	// given
	Utf8String_t eventType("event type");

	auto realMapPayload = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();

	realMapPayload->insert({ core::objects::TIMESTAMP, openkit::json::JsonNumberValue::fromLong(0) });
	realMapPayload->insert({ protocol::EVENT_PAYLOAD_APPLICATION_ID, openkit::json::JsonStringValue::fromString(APP_ID.getStringData()) });
	realMapPayload->insert({ protocol::EVENT_PAYLOAD_INSTANCE_ID, openkit::json::JsonStringValue::fromString(core::util::StringUtil::toInvariantString(DEVICE_ID)) });
	realMapPayload->insert({ protocol::EVENT_PAYLOAD_SESSION_ID, openkit::json::JsonStringValue::fromString(core::util::StringUtil::toInvariantString(SESSION_ID)) });
	realMapPayload->insert({ core::objects::APP_VERSION, openkit::json::JsonStringValue::fromString(APP_VERSION.getStringData()) });
	realMapPayload->insert({ core::objects::OS_NAME, openkit::json::JsonStringValue::fromString(OS_NAME.getStringData()) });
	realMapPayload->insert({ core::objects::DEVICE_MANUFACTURER, openkit::json::JsonStringValue::fromString(DEVICE_MANUFACTURER.getStringData()) });
	realMapPayload->insert({ core::objects::DEVICE_MODEL_IDENTIFIER, openkit::json::JsonStringValue::fromString(MODEL_ID.getStringData()) });
	realMapPayload->insert({ protocol::EVENT_SCHEMA_VERSION, openkit::json::JsonStringValue::fromString("1.0") });
	realMapPayload->insert({ core::objects::EVENT_PROVIDER, openkit::json::JsonStringValue::fromString(APP_ID.getStringData()) });

	realMapPayload->insert({ "event.name", openkit::json::JsonStringValue::fromString("event type") });
	realMapPayload->insert({ "event.type", openkit::json::JsonStringValue::fromString("event type") });
	realMapPayload->insert({ "dt.rum.custom_attributes_size", openkit::json::JsonNumberValue::fromLong(27) });
	realMapPayload->insert({ "event.kind", openkit::json::JsonStringValue::fromString(core::objects::EVENT_KIND_BIZ) });

	auto str = openkit::json::JsonObjectValue::fromMap(realMapPayload)->toString();

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::EVENT)	// event type
		<< "&pl=" << core::util::URLEncoding::urlencode(str, { '_' }).getStringData()	// payload
		;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when error was reported
		IsEventMapEqual(s.str())
	)).Times(1);

	auto target = createBeacon()->build();

	// then
	target->sendBizEvent(eventType, nullptr);
}

TEST_F(BeaconTest, sendValidBizEventTryingToOverrideTimestamp)
{
	// given
	Utf8String_t eventType("event type");
	auto map = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	map->insert({ core::objects::TIMESTAMP, openkit::json::JsonStringValue::fromString("Test") });

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when error was reported
		ContainsString("timestamp%22%3A%22Test%22")
	)).Times(1);

	auto target = createBeacon()->build();

	// then
	target->sendBizEvent(eventType, map);
}

TEST_F(BeaconTest, sendValidBizEventTryingToOverrideAppVersion)
{
	// given
	Utf8String_t eventType("event type");
	auto map = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	map->insert({ core::objects::APP_VERSION, openkit::json::JsonStringValue::fromString("Test") });

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when error was reported
		ContainsString("app.version%22%3A%22Test%22")
	)).Times(1);

	auto target = createBeacon()->build();

	// then
	target->sendBizEvent(eventType, map);
}

TEST_F(BeaconTest, sendValidBizEventTryingToOverrideOsName)
{
	// given
	Utf8String_t eventType("event type");
	auto map = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	map->insert({ core::objects::OS_NAME, openkit::json::JsonStringValue::fromString("Test") });

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when error was reported
		ContainsString("os.name%22%3A%22Test%22")
	)).Times(1);

	auto target = createBeacon()->build();

	// then
	target->sendBizEvent(eventType, map);
}

TEST_F(BeaconTest, sendValidBizEventTryingToOverrideDeviceManufacturer)
{
	// given
	Utf8String_t eventType("event type");
	auto map = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	map->insert({ core::objects::DEVICE_MANUFACTURER, openkit::json::JsonStringValue::fromString("Test") });

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when error was reported
		ContainsString("device.manufacturer%22%3A%22Test%22")
	)).Times(1);

	auto target = createBeacon()->build();

	// then
	target->sendBizEvent(eventType, map);
}

TEST_F(BeaconTest, sendValidBizEventTryingToOverrideDeviceModelIdentifier)
{
	// given
	Utf8String_t eventType("event type");
	auto map = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	map->insert({ core::objects::DEVICE_MODEL_IDENTIFIER, openkit::json::JsonStringValue::fromString("Test") });

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when error was reported
		ContainsString("device.model.identifier%22%3A%22Test%22")
	)).Times(1);

	auto target = createBeacon()->build();

	// then
	target->sendBizEvent(eventType, map);
}

TEST_F(BeaconTest, sendValidBizEventTryingToOverrideEventProvider)
{
	// given
	Utf8String_t eventType("event type");
	auto map = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	map->insert({ core::objects::EVENT_PROVIDER, openkit::json::JsonStringValue::fromString("Test") });

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when error was reported
		ContainsString("event.provider%22%3A%22Test%22")
	)).Times(1);

	auto target = createBeacon()->build();

	// then
	target->sendBizEvent(eventType, map);
}

TEST_F(BeaconTest, sendBizEventWithEmptyEventTypeThrowsException)
{
	// given
	auto emptyMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	auto target = createBeacon()->build();

	// then
	EXPECT_THROW(
		target->sendBizEvent(Utf8String_t(), emptyMap),
		std::invalid_argument
	);
}

TEST_F(BeaconTest, sendBizEventNotReportedIfDataSendingDisallowed)
{
	// with
	ON_CALL(*mockServerConfiguration, isSendingDataAllowed())
		.WillByDefault(testing::Return(false));

	// given
	Utf8String_t eventType("event type");
	auto emptyMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	auto target = createBeacon()->build();

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	// when, expect no interaction with beacon cache
	target->sendBizEvent(eventType, emptyMap);
}

TEST_F(BeaconTest, sendBizEventNotReportedIfSendingEventDataDisallowed)
{
	// with
	ON_CALL(*mockPrivacyConfiguration, isEventReportingAllowed())
		.WillByDefault(testing::Return(false));

	Utf8String_t eventType("event type");
	auto emptyMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	auto target = createBeacon()->build();

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	// when, expect no interaction with beacon cache
	target->sendBizEvent(eventType, emptyMap);
}

TEST_F(BeaconTest, sendBizEventNotReportIfDisallowedByTrafficControl)
{
	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	Utf8String_t eventType("event type");
	auto emptyMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();
	const auto trafficControlPercentage = 50;

	auto mockRandomGenerator = MockIPRNGenerator::createNice();
	ON_CALL(*mockRandomGenerator, nextPercentageValue())
		.WillByDefault(testing::Return(trafficControlPercentage));

	ON_CALL(*mockServerConfiguration, getTrafficControlPercentage())
		.WillByDefault(testing::Return(trafficControlPercentage));

	auto target = createBeacon()->with(mockRandomGenerator).build();

	// when
	target->sendBizEvent(eventType, emptyMap);
}

TEST_F(BeaconTest, sendBizEventPayloadIsTooBig)
{
	// given
	Utf8String_t eventType("event type");
	auto bigMap = std::make_shared<openkit::json::JsonObjectValue::JsonObjectMap>();

	for (int i = 0; i < 1000; i++)
	{
		bigMap->insert({ "KeyName" + std::to_string(i), openkit::json::JsonStringValue::fromString("Test " + std::to_string(i)) });
	}

	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	auto target = createBeacon()->build();

	// then
	EXPECT_THROW(
		target->sendBizEvent(eventType, bigMap),
		std::invalid_argument
	);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// reportError tests (with cause)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(BeaconTest, reportErrorWithCause)
{
	// with
	Utf8String_t errorName("SomeError");
	Utf8String_t causeName("CausedBy");
	Utf8String_t causeDescription("SomeReason");
	Utf8String_t causeStackTrace("HereComesTheTrace");

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::FAILURE_EXCEPTION)	// event type
		<< "&it=" << THREAD_ID										// thread ID
		<< "&na=" << errorName.getStringData()						// name of error event
		<< "&pa=" << ACTION_ID										// parent action
		<< "&s0=1"													// sequence number of reported error
		<< "&t0=0"													// event time since session start
		<< "&ev=" << causeName.getStringData()						// reported error value
		<< "&rs=" << causeDescription.getStringData()					// reported error reason
		<< "&st=" << causeStackTrace.getStringData()				// reported error stack trace
		<< "&tt=c"													// error technology type
		;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when error was reported
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->reportError(ACTION_ID, errorName, causeName, causeDescription, causeStackTrace);
}

TEST_F(BeaconTest, reportingErrorWithCauseWithEmptyErrorNameThrowsException)
{
	// given
	Utf8String_t causeName("CausedBy");
	Utf8String_t causeDescription("SomeReason");
	Utf8String_t causeStackTrace("HereComesTheTrace");

	auto target = createBeacon()->build();

	// then
	EXPECT_THROW(
		target->reportError(ACTION_ID, Utf8String_t(), causeName, causeDescription, causeStackTrace),
		std::invalid_argument
	);
}

TEST_F(BeaconTest, reportErrorIsTruncatingReasonIfTooLong)
{
	// with
	Utf8String_t errorName("SomeError");
	Utf8String_t causeName;
	Utf8String_t causeStackTrace("HereComesTheTrace");

	Utf8String_t causeDescription(std::string(1001, 'a'));
	Utf8String_t causeDescriptionTruncated(std::string(1000, 'a'));

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::FAILURE_EXCEPTION)	// event type
		<< "&it=" << THREAD_ID										// thread ID
		<< "&na=" << errorName.getStringData()						// name of error event
		<< "&pa=" << ACTION_ID										// parent action
		<< "&s0=1"													// sequence number of reported error
		<< "&t0=0"													// event time since session start
		<< "&rs=" << causeDescriptionTruncated.getStringData()		// reported error reason
		<< "&st=" << causeStackTrace.getStringData()				// reported error stack trace
		<< "&tt=c"													// error technology type
		;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when error was reported
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->reportError(ACTION_ID, errorName, causeName, causeDescription, causeStackTrace);
}

TEST_F(BeaconTest, reportErrorIsTruncatingStacktraceIfTooLong)
{
	// with
	Utf8String_t errorName("SomeError");
	Utf8String_t causeName;
	Utf8String_t causeDescription("SomeReason");

	Utf8String_t causeStackTrace(std::string(128001, 'a'));
	Utf8String_t causeStackTraceTruncated(std::string(128000, 'a'));

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::FAILURE_EXCEPTION)	// event type
		<< "&it=" << THREAD_ID										// thread ID
		<< "&na=" << errorName.getStringData()						// name of error event
		<< "&pa=" << ACTION_ID										// parent action
		<< "&s0=1"													// sequence number of reported error
		<< "&t0=0"													// event time since session start
		<< "&rs=" << causeDescription.getStringData()				// reported error reason
		<< "&st=" << causeStackTraceTruncated.getStringData()		// reported error stack trace
		<< "&tt=c"													// error technology type
		;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when error was reported
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->reportError(ACTION_ID, errorName, causeName, causeDescription, causeStackTrace);
}

TEST_F(BeaconTest, reportErrorIsTruncatingStacktraceUntilLastBreakIfTooLong)
{
	// with
	Utf8String_t errorName("SomeError");
	Utf8String_t causeName;
	Utf8String_t causeDescription("SomeReason");

	Utf8String_t causeStackTraceTruncated(std::string(127900, 'a'));
	auto causeStackTraceStr = std::string(127900, 'a');
	causeStackTraceStr.append("\n");
	causeStackTraceStr.append(std::string(1000, 'a'));

	Utf8String_t causeStackTrace(causeStackTraceStr);

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::FAILURE_EXCEPTION)	// event type
		<< "&it=" << THREAD_ID										// thread ID
		<< "&na=" << errorName.getStringData()						// name of error event
		<< "&pa=" << ACTION_ID										// parent action
		<< "&s0=1"													// sequence number of reported error
		<< "&t0=0"													// event time since session start
		<< "&rs=" << causeDescription.getStringData()				// reported error reason
		<< "&st=" << causeStackTraceTruncated.getStringData()		// reported error stack trace
		<< "&tt=c"													// error technology type
		;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when error was reported
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->reportError(ACTION_ID, errorName, causeName, causeDescription, causeStackTrace);
}

TEST_F(BeaconTest, reportErrorWithEmptyCauseNameWorks)
{
	// with
	Utf8String_t errorName("SomeError");
	Utf8String_t causeName;
	Utf8String_t causeDescription("SomeReason");
	Utf8String_t causeStackTrace("HereComesTheTrace");

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::FAILURE_EXCEPTION)	// event type
		<< "&it=" << THREAD_ID										// thread ID
		<< "&na=" << errorName.getStringData()						// name of error event
		<< "&pa=" << ACTION_ID										// parent action
		<< "&s0=1"													// sequence number of reported error
		<< "&t0=0"													// event time since session start
		<< "&rs=" << causeDescription.getStringData()				// reported error reason
		<< "&st=" << causeStackTrace.getStringData()				// reported error stack trace
		<< "&tt=c"													// error technology type
		;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when error was reported
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->reportError(ACTION_ID, errorName, causeName, causeDescription, causeStackTrace);
}

TEST_F(BeaconTest, reportErrorWithEmptyCauseDescriptionWorks)
{
	// with
	Utf8String_t errorName("SomeError");
	Utf8String_t causeName("CausedBy");
	Utf8String_t causeDescription;
	Utf8String_t causeStackTrace("HereComesTheTrace");

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::FAILURE_EXCEPTION)	// event type
		<< "&it=" << THREAD_ID										// thread ID
		<< "&na=" << errorName.getStringData()						// name of error event
		<< "&pa=" << ACTION_ID										// parent action
		<< "&s0=1"													// sequence number of reported error
		<< "&t0=0"													// event time since session start
		<< "&ev=" << causeName.getStringData()						// reported error value
		<< "&st=" << causeStackTrace.getStringData()				// reported error stack trace
		<< "&tt=c"													// error technology type
		;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when error was reported
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->reportError(ACTION_ID, errorName, causeName, causeDescription, causeStackTrace);
}

TEST_F(BeaconTest, reportErrorWithEmptyCauseStackTraceWorks)
{
	// with
	Utf8String_t errorName("SomeError");
	Utf8String_t causeName("CausedBy");
	Utf8String_t causeDescription("SomeReason");
	Utf8String_t causeStackTrace;

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::FAILURE_EXCEPTION)	// event type
		<< "&it=" << THREAD_ID										// thread ID
		<< "&na=" << errorName.getStringData()						// name of error event
		<< "&pa=" << ACTION_ID										// parent action
		<< "&s0=1"													// sequence number of reported error
		<< "&t0=0"													// event time since session start
		<< "&ev=" << causeName.getStringData()						// reported error value
		<< "&rs=" << causeDescription.getStringData()				// reported error reason
		<< "&tt=c"													// error technology type
		;
	EXPECT_CALL(*mockBeaconCache, addEventData(
		BeaconKey_t(SESSION_ID, SESSION_SEQUENCE),	// beacon key
		0,											// timestamp when error was reported
		testing::Eq(s.str())
	)).Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->reportError(ACTION_ID, errorName, causeName, causeDescription, causeStackTrace);
}

TEST_F(BeaconTest, errorWithCauseNotReportedIfDataSendingIsDisallowed)
{
	// with
	ON_CALL(*mockServerConfiguration, isSendingDataAllowed)
		.WillByDefault(testing::Return(false));

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	auto target = createBeacon()->build();

	// when
	target->reportError(ACTION_ID, "Error name", "cause", "description", "stack trace");
}

TEST_F(BeaconTest, errorWithCauseNotReportedIfErrorSendingIsDisallowed)
{
	// with
	ON_CALL(*mockServerConfiguration, isSendingErrorsAllowed)
		.WillByDefault(testing::Return(false));

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	auto target = createBeacon()->build();

	// when
	target->reportError(ACTION_ID, "Error name", "cause", "description", "stack trace");
}

TEST_F(BeaconTest, errorWithCauseNotReportedIfErrorReportingIsDisallowed)
{
	// with
	ON_CALL(*mockPrivacyConfiguration, isErrorReportingAllowed)
		.WillByDefault(testing::Return(false));

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	auto target = createBeacon()->build();

	// when
	target->reportError(ACTION_ID, "Error name", "cause", "description", "stack trace");
}

TEST_F(BeaconTest, errorWithCauseNotReportedIfDisallowedByTrafficControl)
{
	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	const auto trafficControlPercentage = 50;

	auto mockRandomGenerator = MockIPRNGenerator::createNice();
	ON_CALL(*mockRandomGenerator, nextPercentageValue())
		.WillByDefault(testing::Return(trafficControlPercentage));

	ON_CALL(*mockServerConfiguration, getTrafficControlPercentage())
		.WillByDefault(testing::Return(trafficControlPercentage));

	auto target = createBeacon()->with(mockRandomGenerator).build();

	// when
	target->reportError(1, "error", "causeName", "causeDescription", "stackTrace");
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
		<< "&it=" << THREAD_ID						// thread ID
		<< "&na=" << errorName.getStringData()		// reported crash name
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

TEST_F(BeaconTest, reportingCrashWithEmptyErrorNameThrowsException)
{
	// given
	auto target = createBeacon()->build();

	// when, then
	EXPECT_THROW(
		target->reportCrash(Utf8String_t(), "reason", "stack trace"),
		std::invalid_argument
	);
}

TEST_F(BeaconTest, reportCrashIsTruncatingReasonIfTooLong)
{
	// with
	Utf8String_t errorName("someEvent");
	Utf8String_t stacktrace("someStacktrace");

	Utf8String_t reason(std::string(1001, 'a'));
	Utf8String_t reasonTruncated(std::string(1000, 'a'));

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::FAILURE_CRASH)	// event type
		<< "&it=" << THREAD_ID						// thread ID
		<< "&na=" << errorName.getStringData()		// reported crash name
		<< "&pa=0"									// parent action
		<< "&s0=1"									// sequence number of reported crash
		<< "&t0=0"									// event time since session start
		<< "&rs=" << reasonTruncated.getStringData()// reported reason
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

TEST_F(BeaconTest, reportCrashIsTruncatingStacktraceIfTooLong)
{
	// with
	Utf8String_t errorName("someEvent");
	Utf8String_t reason("someReason");
	Utf8String_t stacktrace(std::string(128001, 'a'));
	Utf8String_t stacktraceTruncated(std::string(128000, 'a'));

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::FAILURE_CRASH)	// event type
		<< "&it=" << THREAD_ID						// thread ID
		<< "&na=" << errorName.getStringData()		// reported crash name
		<< "&pa=0"									// parent action
		<< "&s0=1"									// sequence number of reported crash
		<< "&t0=0"									// event time since session start
		<< "&rs=" << reason.getStringData()			// reported reason
		<< "&st=" << stacktraceTruncated.getStringData()// reported stacktrace
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

TEST_F(BeaconTest, reportCrashIsTruncatingStacktraceUntilLastBreakIfTooLong)
{
	// with
	Utf8String_t errorName("someEvent");
	Utf8String_t reason("someReason");
	Utf8String_t stacktraceTruncated(std::string(127900, 'a'));
	auto causeStackTraceStr = std::string(127900, 'a');
	causeStackTraceStr.append("\n");
	causeStackTraceStr.append(std::string(1000, 'a'));

	Utf8String_t stacktrace(causeStackTraceStr);

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::FAILURE_CRASH)	// event type
		<< "&it=" << THREAD_ID						// thread ID
		<< "&na=" << errorName.getStringData()		// reported crash name
		<< "&pa=0"									// parent action
		<< "&s0=1"									// sequence number of reported crash
		<< "&t0=0"									// event time since session start
		<< "&rs=" << reason.getStringData()			// reported reason
		<< "&st=" << stacktraceTruncated.getStringData()// reported stacktrace
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

TEST_F(BeaconTest, reportCrashWithEmptyReasonWorks)
{
	// with
	Utf8String_t errorName("someEvent");
	Utf8String_t reason;
	Utf8String_t stacktrace("someStackTrace");

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::FAILURE_CRASH)	// event type
		<< "&it=" << THREAD_ID						// thread ID
		<< "&na=" << errorName.getStringData()		// reported crash name
		<< "&pa=0"									// parent action
		<< "&s0=1"									// sequence number of reported crash
		<< "&t0=0"									// event time since session start
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

TEST_F(BeaconTest, reportCrashWithEmptyStackTraceWorks)
{
	// with
	Utf8String_t errorName("someEvent");
	Utf8String_t reason("someReason");
	Utf8String_t stacktrace;

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::FAILURE_CRASH)	// event type
		<< "&it=" << THREAD_ID						// thread ID
		<< "&na=" << errorName.getStringData()		// reported crash name
		<< "&pa=0"									// parent action
		<< "&s0=1"									// sequence number of reported crash
		<< "&t0=0"									// event time since session start
		<< "&rs=" << reason.getStringData()			// reported reason
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

TEST_F(BeaconTest, crashNotReportedIfDataSendingDisallowed)
{
	// with
	ON_CALL(*mockServerConfiguration, isSendingDataAllowed())
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

TEST_F(BeaconTest, reportCrashDoesNotReportIfDisallowedByTrafficControl)
{
	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	const auto trafficControlPercentage = 50;

	auto mockRandomGenerator = MockIPRNGenerator::createNice();
	ON_CALL(*mockRandomGenerator, nextPercentageValue())
		.WillByDefault(testing::Return(trafficControlPercentage));

	ON_CALL(*mockServerConfiguration, getTrafficControlPercentage())
		.WillByDefault(testing::Return(trafficControlPercentage));

	auto target = createBeacon()->with(mockRandomGenerator).build();

	// when
	target->reportCrash("OutOfMemory exception", "insufficient memory", "stacktrace:123");
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
		.WillByDefault(testing::ReturnRef(TRACER_URL));
	ON_CALL(*tracer, getBytesSent())
		.WillByDefault(testing::Return(numBytesSent));
	ON_CALL(*tracer, getBytesReceived())
		.WillByDefault(testing::Return(numBytesReceived));
	ON_CALL(*tracer, getResponseCode())
		.WillByDefault(testing::Return(responseCode));

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::WEBREQUEST)	// event type
		<< "&it=" << THREAD_ID						// thread ID
		<< "&na=" << UrlEncoding_t::urlencode(TRACER_URL, { '_' }).getStringData() // tracer url
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

TEST_F(BeaconTest, addWebRequestWithNullWebRequestTracerThrowsException)
{
	// given
	auto target = createBeacon()->build();

	// when, then
	EXPECT_THROW(
		target->addWebRequest(ACTION_ID, nullptr),
		std::invalid_argument
	);
}

TEST_F(BeaconTest, addWebRequestWithEmptyUrlThrowsException)
{
	// given
	Utf8String_t url;
	auto webRequestTracer = MockIWebRequestTracerInternals::createNice();
	ON_CALL(*webRequestTracer, getURL)
		.WillByDefault(testing::ReturnRef(url));

	auto target = createBeacon()->build();

	// when, then
	EXPECT_THROW(
		target->addWebRequest(ACTION_ID, webRequestTracer),
		std::invalid_argument
	);
}

TEST_F(BeaconTest, canAddSentBytesEqualToZeroToWebRequest)
{
	// given
	int32_t numBytesReceived = 14;
	int32_t responseCode = 15;

	auto target = createBeacon()->build();

	auto tracer = MockIWebRequestTracerInternals::createNice();
	ON_CALL(*tracer, getURL())
		.WillByDefault(testing::ReturnRef(TRACER_URL));
	ON_CALL(*tracer, getBytesSent())
		.WillByDefault(testing::Return(0));
	ON_CALL(*tracer, getBytesReceived())
		.WillByDefault(testing::Return(numBytesReceived));
	ON_CALL(*tracer, getResponseCode())
		.WillByDefault(testing::Return(responseCode));

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::WEBREQUEST)	// event type
		<< "&it=" << THREAD_ID						// thread ID
		<< "&na=" << UrlEncoding_t::urlencode(TRACER_URL, { '_' }).getStringData() // tracer url
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
		.WillByDefault(testing::ReturnRef(TRACER_URL));
	ON_CALL(*tracer, getBytesSent())
		.WillByDefault(testing::Return(-1));
	ON_CALL(*tracer, getBytesReceived())
		.WillByDefault(testing::Return(numBytesReceived));
	ON_CALL(*tracer, getResponseCode())
		.WillByDefault(testing::Return(responseCode));

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::WEBREQUEST)	// event type
		<< "&it=" << THREAD_ID						// thread ID
		<< "&na=" << UrlEncoding_t::urlencode(TRACER_URL, { '_' }).getStringData() // tracer url
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
		.WillByDefault(testing::ReturnRef(TRACER_URL));
	ON_CALL(*tracer, getBytesSent())
		.WillByDefault(testing::Return(numBytesSent));
	ON_CALL(*tracer, getBytesReceived())
		.WillByDefault(testing::Return(0));
	ON_CALL(*tracer, getResponseCode())
		.WillByDefault(testing::Return(responseCode));

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::WEBREQUEST)	// event type
		<< "&it=" << THREAD_ID						// thread ID
		<< "&na=" << UrlEncoding_t::urlencode(TRACER_URL, { '_' }).getStringData() // tracer url
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
		.WillByDefault(testing::ReturnRef(TRACER_URL));
	ON_CALL(*tracer, getBytesSent())
		.WillByDefault(testing::Return(numBytesSent));
	ON_CALL(*tracer, getBytesReceived())
		.WillByDefault(testing::Return(-1));
	ON_CALL(*tracer, getResponseCode())
		.WillByDefault(testing::Return(responseCode));

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::WEBREQUEST)	// event type
		<< "&it=" << THREAD_ID						// thread ID
		<< "&na=" << UrlEncoding_t::urlencode(TRACER_URL, { '_' }).getStringData() // tracer url
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
		.WillByDefault(testing::ReturnRef(TRACER_URL));
	ON_CALL(*tracer, getBytesSent())
		.WillByDefault(testing::Return(numBytesSent));
	ON_CALL(*tracer, getBytesReceived())
		.WillByDefault(testing::Return(numBytesReceived));
	ON_CALL(*tracer, getResponseCode())
		.WillByDefault(testing::Return(0));

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::WEBREQUEST)	// event type
		<< "&it=" << THREAD_ID						// thread ID
		<< "&na=" << UrlEncoding_t::urlencode(TRACER_URL, { '_' }).getStringData() // tracer url
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
		.WillByDefault(testing::ReturnRef(TRACER_URL));
	ON_CALL(*tracer, getBytesSent())
		.WillByDefault(testing::Return(numBytesSent));
	ON_CALL(*tracer, getBytesReceived())
		.WillByDefault(testing::Return(numBytesReceived));
	ON_CALL(*tracer, getResponseCode())
		.WillByDefault(testing::Return(-1));

	// expect
	std::stringstream s;
	s << "et=" << static_cast<int32_t>(EventType_t::WEBREQUEST)	// event type
		<< "&it=" << THREAD_ID						// thread ID
		<< "&na=" << UrlEncoding_t::urlencode(TRACER_URL, { '_' }).getStringData() // tracer url
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

TEST_F(BeaconTest, webRequestNotReportedIfDataSendingDisallowed)
{
	// with
	ON_CALL(*mockServerConfiguration, isSendingDataAllowed())
		.WillByDefault(testing::Return(false));

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	auto target = createBeacon()
		->build();
	auto tracer = MockIWebRequestTracerInternals::createNice();
	ON_CALL(*tracer, getURL())
		.WillByDefault(testing::ReturnRef(TRACER_URL));

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
	ON_CALL(*tracer, getURL())
		.WillByDefault(testing::ReturnRef(TRACER_URL));

	// when, expect no interaction with beacon cache
	target->addWebRequest(ACTION_ID, tracer);
}

TEST_F(BeaconTest, webRequestNotReportedIfDisallowedByTrafficControl)
{
	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	const auto trafficControlPercentage = 50;

	auto mockRandomGenerator = MockIPRNGenerator::createNice();
	ON_CALL(*mockRandomGenerator, nextPercentageValue())
		.WillByDefault(testing::Return(trafficControlPercentage));

	ON_CALL(*mockServerConfiguration, getTrafficControlPercentage())
		.WillByDefault(testing::Return(trafficControlPercentage));

	auto target = createBeacon()->with(mockRandomGenerator).build();
	auto tracer = MockIWebRequestTracerInternals::createNice();
	ON_CALL(*tracer, getURL())
		.WillByDefault(testing::ReturnRef(TRACER_URL));

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
		<< "&it=" << THREAD_ID						// thread ID
		<< "&na=" << userID.getStringData()			// reported user ID
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

TEST_F(BeaconTest, cannotIdentifyUserIfDataSendingDisallowed)
{
	// with
	ON_CALL(*mockServerConfiguration, isSendingDataAllowed())
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

	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	auto target = createBeacon()->build();

	// when, expect no interaction with beacon cache
	target->identifyUser("jane@doe.com");
}

TEST_F(BeaconTest, cannotIdentifyUserIfDisallowedByTrafficControl)
{
	// expect
	EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
		.Times(0);

	// given
	const auto trafficControlPercentage = 50;

	auto mockRandomGenerator = MockIPRNGenerator::createNice();
	ON_CALL(*mockRandomGenerator, nextPercentageValue())
		.WillByDefault(testing::Return(trafficControlPercentage));

	ON_CALL(*mockServerConfiguration, getTrafficControlPercentage())
		.WillByDefault(testing::Return(trafficControlPercentage));

	auto target = createBeacon()->with(mockRandomGenerator).build();

	// when, expect no interaction with beacon cache
	target->identifyUser("jane@doe.com");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// send tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(BeaconTest, canHandleNoDataInBeaconSend)
{
	// expect
	EXPECT_CALL(*mockBeaconCache, hasDataForSending(BeaconKey_t(SESSION_ID, SESSION_SEQUENCE)))
		.Times(1)
		.WillOnce(testing::Return(true));
	EXPECT_CALL(*mockBeaconCache, prepareDataForSending(BeaconKey_t(SESSION_ID, SESSION_SEQUENCE)))
		.Times(1);
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

	auto statusResponse = MockIStatusResponse ::createNice();
	ON_CALL(*statusResponse, isErroneousResponse())
		.WillByDefault(testing::Return(false));

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
	ASSERT_THAT(obtained, testing::Eq(statusResponse));
}

TEST_F(BeaconTest, sendCanHandleMultipleChunks)
{
	// with
	Utf8String_t firstChunk("some beacon string");
	Utf8String_t secondChunk("some more beacon string");

	// expect
	auto beaconCache = MockIBeaconCache::createNice();
	EXPECT_CALL(*beaconCache, prepareDataForSending(testing::_))
		.Times(1);
	EXPECT_CALL(*beaconCache, hasDataForSending(testing::_))
		.Times(3)
		.WillOnce(testing::Return(true))
		.WillOnce(testing::Return(true))
		.WillRepeatedly(testing::Return(false));
	EXPECT_CALL(*beaconCache, getNextBeaconChunk(testing::_, testing::_, testing::_, testing::_))
		.Times(2)
		.WillOnce(testing::Return(firstChunk))
		.WillOnce(testing::Return(secondChunk));

	// given
	auto firstResponse = MockIStatusResponse::createNice();
	ON_CALL(*firstResponse, isErroneousResponse())
		.WillByDefault(testing::Return(false));
	auto secondResponse = MockIStatusResponse::createNice();
	ON_CALL(*secondResponse, isErroneousResponse())
		.WillByDefault(testing::Return(false));

	auto httpClient = MockIHTTPClient::createNice();
	EXPECT_CALL(*httpClient, sendBeaconRequest(testing::_, testing::_, testing::_))
		.Times(2)
		.WillOnce(testing::Return(firstResponse))
		.WillOnce(testing::Return(secondResponse));

	auto httpClientProvider = MockIHTTPClientProvider::createNice();
	ON_CALL(*httpClientProvider, createClient(testing::_))
		.WillByDefault(testing::Return(httpClient));

	auto target = createBeacon()->with(beaconCache).build();

	// when
	auto response = target->send(httpClientProvider, *mockAdditionalQueryParameters);

	// then
	ASSERT_THAT(response, testing::NotNull());
	ASSERT_THAT(response, testing::Eq(secondResponse));
}

TEST_F(BeaconTest, sendDataAndFakeErrorResponse)
{
	// with
	Utf8String_t ipAddress("127.0.0.1");
	auto beaconCache = std::make_shared<BeaconCache_t>(mockLogger);

	auto statusResponse = MockIStatusResponse::createNice();
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
	ASSERT_THAT(obtained, testing::Eq(statusResponse));
}

TEST_F(BeaconTest, beaconDataPrefixVS2)
{
	// expect
	EXPECT_CALL(*mockBeaconCache, prepareDataForSending(testing::_))
		.Times(1);
	EXPECT_CALL(*mockBeaconCache, hasDataForSending(testing::_))
		.Times(1)
		.WillOnce(testing::Return(true));

	// given
	const int32_t sessionSequence = 1213;
	const int32_t visitStoreVersion = 2;
	const auto ipAddress = "192.168.0.1";
	const core::UTF8String appVersion{ "1111" };
	const core::UTF8String os { "system" };
	const core::UTF8String manufacturer { "manufacturer" };
	const core::UTF8String modelId{ "model" };

	ON_CALL(*mockOpenKitConfiguration, getApplicationVersion())
		.WillByDefault(testing::ReturnRef(appVersion));
	ON_CALL(*mockOpenKitConfiguration, getOperatingSystem())
		.WillByDefault(testing::ReturnRef(os));
	ON_CALL(*mockOpenKitConfiguration, getManufacturer())
		.WillByDefault(testing::ReturnRef(manufacturer));
	ON_CALL(*mockOpenKitConfiguration, getModelId())
		.WillByDefault(testing::ReturnRef(modelId));
	ON_CALL(*mockServerConfiguration, getVisitStoreVersion())
		.WillByDefault(testing::Return(visitStoreVersion));

	// expect
	std::stringstream expectedPrefix;
	expectedPrefix << "vv=" << protocol::PROTOCOL_VERSION
		<< "&va=" << protocol::OPENKIT_VERSION
		<< "&ap=" << APP_ID.getStringData()
		<< "&vn=" << appVersion.getStringData()
		<< "&pt=" << protocol::PLATFORM_TYPE_OPENKIT
		<< "&tt=" << protocol::AGENT_TECHNOLOGY_TYPE
		<< "&vi=" << DEVICE_ID
		<< "&sn=" << SESSION_ID
		<< "&ip=" << ipAddress
		<< "&os=system"
		<< "&mf=manufacturer"
		<< "&md=model"
		<< "&dl=2"
		<< "&cl=2"
		<< "&vs=" << visitStoreVersion
		<< "&ss=" << sessionSequence
		<< "&tx=0"
		<< "&tv=0"
		<< "&mp=" << MULTIPLICITY;

	const core::UTF8String expected{ expectedPrefix.str() };
	EXPECT_CALL(*mockBeaconCache, getNextBeaconChunk(testing::_, expected, testing::_, testing::_))
		.Times(1)
		.WillRepeatedly(testing::ReturnNull());
	
	// when
	auto target = createBeacon()->withIpAddress(ipAddress)
		.withSessionSequenceNumber(sessionSequence)
		.build();
	
	auto httpClientProvider = MockIHTTPClientProvider::createNice();
	target->send(httpClientProvider, *mockAdditionalQueryParameters);
	
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
	ON_CALL(*action, getName())
		.WillByDefault(testing::ReturnRef(ACTION_NAME));

	target->addAction(action);
	target->reportValue(ACTION_ID, "Int32Value", 42);
	target->reportValue(ACTION_ID, "Int64Value", int64_t(42));
	target->reportValue(ACTION_ID, "DoubleValue", 3.1415);
	target->reportValue(ACTION_ID, "StringValue", "HelloWorld");
	target->reportEvent(ACTION_ID, "SomeEvent");
	target->reportError(ACTION_ID, "SomeError", -123);
	target->reportError(ACTION_ID, "OtherError", "causeName", "causeReason", "causeStackTrace");
	target->reportCrash("SomeCrash", "SomeReason", "SomeStacktrace");
	target->endSession();

	// when
	target->clearData();

	// then
	ASSERT_THAT(target->isEmpty(), testing::Eq(true));
}

TEST_F(BeaconTest, noActionIsAddedIfDataSendingDisallowed)
{
    // expect
    EXPECT_CALL(*mockBeaconCache, addEventData(testing::_, testing::_, testing::_))
            .Times(0);
    EXPECT_CALL(*mockBeaconCache, addActionData(testing::_, testing::_, testing::_))
            .Times(0);

    // given
    ON_CALL(*mockServerConfiguration, isSendingDataAllowed())
        .WillByDefault(testing::Return(false));
	auto action = MockIActionCommon::createNice();
	ON_CALL(*action, getName())
		.WillByDefault(testing::ReturnRef(ACTION_NAME));
    ON_CALL(*action, getID())
        .WillByDefault(testing::Return(ACTION_ID));
    auto target = createBeacon()->with(mockBeaconCache).build();

    // when
    target->addAction(action);
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

	auto mockRandomStrict = MockIPRNGenerator::createNice();

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

TEST_F(BeaconTest, initializeServerConfigurationDelegatesToBeacon)
{
	// with
	auto serverConfig = MockIServerConfiguration::createStrict();

	// expect
	EXPECT_CALL(*mockBeaconConfiguration, initializeServerConfiguration(testing::Eq(serverConfig)))
		.Times(1);

	// given
	auto target = createBeacon()->build();

	// when
	target->initializeServerConfiguration(serverConfig);
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
	// expect
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

TEST_F(BeaconTest, isActionReportingAllowedByPrivacySettingsDelegatesToPrivacyConfig)
{
	// expect
	EXPECT_CALL(*mockPrivacyConfiguration, isActionReportingAllowed())
		.Times(2)
		.WillOnce(testing::Return(true))
		.WillOnce(testing::Return(false));

	// given
	auto target = createBeacon()->build();

	// when
	auto obtained = target->isActionReportingAllowedByPrivacySettings();

	// then
	ASSERT_THAT(obtained, testing::Eq(true));

	// and when
	obtained = target->isActionReportingAllowedByPrivacySettings();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(BeaconTest, isDataCapturingEnabledReturnsFalseIfDataSendingIsDisallowed)
{
    // given
    ON_CALL(*mockServerConfiguration, isSendingDataAllowed())
        .WillByDefault(testing::Return(false));

	auto target = createBeacon()->build();

	// when
	auto obtained = target->isDataCapturingEnabled();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(BeaconTest, isDataCapturingEnabledReturnsTrueIfDataSendingIsAllowedAndTcValueGreaterThanTcPercentageFromServerConfig)
{
    // given
	const auto trafficControlPercentage = 50;

    ON_CALL(*mockServerConfiguration, isSendingDataAllowed())
        .WillByDefault(testing::Return(true));
	ON_CALL(*mockServerConfiguration, getTrafficControlPercentage())
		.WillByDefault(testing::Return(trafficControlPercentage + 1));

	auto mockRandomGenerator = MockIPRNGenerator::createNice();
	ON_CALL(*mockRandomGenerator, nextPercentageValue())
		.WillByDefault(testing::Return(trafficControlPercentage));

    auto target = createBeacon()->with(mockRandomGenerator).build();

    // when
    auto obtained = target->isDataCapturingEnabled();

    // then
    ASSERT_THAT(obtained, testing::Eq(true));
}

TEST_F(BeaconTest, isDataCapturingEnabledReturnsFalseIfTcValueEqualToTcPercentageFromServerConfig)
{
	// given
	const auto trafficControlPercentage = 50;

	ON_CALL(*mockServerConfiguration, isSendingDataAllowed())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mockServerConfiguration, getTrafficControlPercentage())
		.WillByDefault(testing::Return(trafficControlPercentage));

	auto mockRandomGenerator = MockIPRNGenerator::createNice();
	ON_CALL(*mockRandomGenerator, nextPercentageValue())
		.WillByDefault(testing::Return(trafficControlPercentage));

	auto target = createBeacon()->with(mockRandomGenerator).build();

	// when
	auto obtained = target->isDataCapturingEnabled();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(BeaconTest, IsDataCapturingEnabledReturnsFalseIfTcValueGreaterThanTcPercentageFromServerConfig)
{
	// given
	const auto trafficControlPercentage = 50;

	ON_CALL(*mockServerConfiguration, isSendingDataAllowed())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mockServerConfiguration, getTrafficControlPercentage())
		.WillByDefault(testing::Return(trafficControlPercentage - 1));

	auto mockRandomGenerator = MockIPRNGenerator::createNice();
	ON_CALL(*mockRandomGenerator, nextPercentageValue())
		.WillByDefault(testing::Return(trafficControlPercentage));

	auto target = createBeacon()->with(mockRandomGenerator).build();

	// when
	auto obtained = target->isDataCapturingEnabled();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(BeaconTest, isErrorCapturingEnabledReturnsFalseIfSendingErrorsIsDisallowed)
{
	// given
	ON_CALL(*mockServerConfiguration, isSendingErrorsAllowed())
		.WillByDefault(testing::Return(false));

	auto target = createBeacon()->build();

	// when
	auto obtained = target->isErrorCapturingEnabled();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(BeaconTest, isErrorCapturingEnabledReturnsFalseIfTcValueEqualToTcPercentageFromServerConfig)
{
	// given
	const auto trafficControlPercentage = 50;

	ON_CALL(*mockServerConfiguration, isSendingErrorsAllowed())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mockServerConfiguration, getTrafficControlPercentage())
		.WillByDefault(testing::Return(trafficControlPercentage));

	auto mockRandomGenerator = MockIPRNGenerator::createNice();
	ON_CALL(*mockRandomGenerator, nextPercentageValue())
		.WillByDefault(testing::Return(trafficControlPercentage));

	auto target = createBeacon()->with(mockRandomGenerator).build();

	// when
	auto obtained = target->isErrorCapturingEnabled();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(BeaconTest, isErrorCapturingEnabledReturnsFalseIfTcValueGreaterThanTcPercentageFromServerConfig)
{
	// given
	const auto trafficControlPercentage = 50;

	ON_CALL(*mockServerConfiguration, isSendingErrorsAllowed())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mockServerConfiguration, getTrafficControlPercentage())
		.WillByDefault(testing::Return(trafficControlPercentage - 1));

	auto mockRandomGenerator = MockIPRNGenerator::createNice();
	ON_CALL(*mockRandomGenerator, nextPercentageValue())
		.WillByDefault(testing::Return(trafficControlPercentage));

	auto target = createBeacon()->with(mockRandomGenerator).build();

	// when
	auto obtained = target->isErrorCapturingEnabled();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(BeaconTest, isErrorCapturingEnabledReturnsTrueIfErrorSendingIsAllowedAndTcValueGreaterThanTcPercentageFromServerConfig)
{
	// given
	const auto trafficControlPercentage = 50;

	ON_CALL(*mockServerConfiguration, isSendingErrorsAllowed())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mockServerConfiguration, getTrafficControlPercentage())
		.WillByDefault(testing::Return(trafficControlPercentage + 1));

	auto mockRandomGenerator = MockIPRNGenerator::createNice();
	ON_CALL(*mockRandomGenerator, nextPercentageValue())
		.WillByDefault(testing::Return(trafficControlPercentage));

	auto target = createBeacon()->with(mockRandomGenerator).build();

	// when
	auto obtained = target->isErrorCapturingEnabled();

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
}

TEST_F(BeaconTest, isCrashCapturingEnabledReturnsFalseIfSendingErrorsIsDisallowed)
{
	// given
	ON_CALL(*mockServerConfiguration, isSendingCrashesAllowed())
		.WillByDefault(testing::Return(false));

	auto target = createBeacon()->build();

	// when
	auto obtained = target->isCrashCapturingEnabled();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(BeaconTest, isCrashCapturingEnabledReturnsFalseIfTcValueEqualToTcPercentageFromServerConfig)
{
	// given
	const auto trafficControlPercentage = 50;

	ON_CALL(*mockServerConfiguration, isSendingCrashesAllowed())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mockServerConfiguration, getTrafficControlPercentage())
		.WillByDefault(testing::Return(trafficControlPercentage));

	auto mockRandomGenerator = MockIPRNGenerator::createNice();
	ON_CALL(*mockRandomGenerator, nextPercentageValue())
		.WillByDefault(testing::Return(trafficControlPercentage));

	auto target = createBeacon()->with(mockRandomGenerator).build();

	// when
	auto obtained = target->isCrashCapturingEnabled();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(BeaconTest, isCrashCapturingEnabledReturnsFalseIfTcValueGreaterThanTcPercentageFromServerConfig)
{
	// given
	const auto trafficControlPercentage = 50;

	ON_CALL(*mockServerConfiguration, isSendingCrashesAllowed())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mockServerConfiguration, getTrafficControlPercentage())
		.WillByDefault(testing::Return(trafficControlPercentage - 1));

	auto mockRandomGenerator = MockIPRNGenerator::createNice();
	ON_CALL(*mockRandomGenerator, nextPercentageValue())
		.WillByDefault(testing::Return(trafficControlPercentage));

	auto target = createBeacon()->with(mockRandomGenerator).build();

	// when
	auto obtained = target->isCrashCapturingEnabled();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(BeaconTest, isCrashCapturingEnabledReturnsTrueIfCrashSendingIsAllowedAndTcValueGreaterThanTcPercentageFromServerConfig)
{
	// given
	const auto trafficControlPercentage = 50;

	ON_CALL(*mockServerConfiguration, isSendingCrashesAllowed())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mockServerConfiguration, getTrafficControlPercentage())
		.WillByDefault(testing::Return(trafficControlPercentage + 1));

	auto mockRandomGenerator = MockIPRNGenerator::createNice();
	ON_CALL(*mockRandomGenerator, nextPercentageValue())
		.WillByDefault(testing::Return(trafficControlPercentage));

	auto target = createBeacon()->with(mockRandomGenerator).build();

	// when
	auto obtained = target->isCrashCapturingEnabled();

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

TEST_F(BeaconTest, sendConstructsCorrectBeaconPrefixVisitStore1)
{
	// expect
	EXPECT_CALL(*mockBeaconCache, prepareDataForSending(testing::_))
		.Times(1);
	EXPECT_CALL(*mockBeaconCache, hasDataForSending(testing::_))
		.Times(1)
		.WillOnce(testing::Return(true));

	// given
	const int32_t visitStoreVersion = 1;
	auto httpClientProvider = MockIHTTPClientProvider::createNice();
	ON_CALL(*mockServerConfiguration, getVisitStoreVersion())
		.WillByDefault(testing::Return(visitStoreVersion));

	auto target = createBeacon()->build();

	std::stringstream expectedPrefix;
	expectedPrefix << "vv=" << protocol::PROTOCOL_VERSION
		<< "&va=" << protocol::OPENKIT_VERSION
		<< "&ap=" << APP_ID.getStringData()
		<< "&vn=1.0"
		<< "&pt=" << protocol::PLATFORM_TYPE_OPENKIT
		<< "&tt=" << protocol::AGENT_TECHNOLOGY_TYPE
		<< "&vi=" << DEVICE_ID
		<< "&sn=" << SESSION_ID
		<< "&ip=127.0.0.1"
		<< "&os=" << OS_NAME.getStringData()
		<< "&mf=" << DEVICE_MANUFACTURER.getStringData()
		<< "&md=" << MODEL_ID.getStringData()
		<< "&dl=" << static_cast<int32_t>(core::configuration::DEFAULT_DATA_COLLECTION_LEVEL)
		<< "&cl=" << static_cast<int32_t>(core::configuration::DEFAULT_CRASH_REPORTING_LEVEL)
		<< "&vs=1"
		<< "&tx=0"
		<< "&tv=0"
		<< "&mp=" << MULTIPLICITY;

	// then
	const core::UTF8String expected{ expectedPrefix.str() };
	EXPECT_CALL(*mockBeaconCache, getNextBeaconChunk(testing::_, expected, testing::_, testing::_))
		.Times(1);

	// when
	auto response = target->send(httpClientProvider, *mockAdditionalQueryParameters);
	ASSERT_THAT(response, testing::IsNull());
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
