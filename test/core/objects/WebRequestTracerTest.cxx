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

#include "Types.h"
#include "MockTypes.h"
#include "../Types.h"
#include "../caching/Types.h"
#include "../configuration/Types.h"
#include "../util/Types.h"
#include "../../api/Types.h"
#include "../../protocol/Types.h"
#include "../../protocol/MockTypes.h"
#include "../../providers/Types.h"
#include "../../providers/MockTypes.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>

using namespace test::types;

static const char APP_ID[] = "appID";
static const char APP_NAME[] = "appName";
static const char TAG[] = "THE_TAG";

class WebRequestTracerTest : public testing::Test
{
protected:
	void SetUp()
	{
		logger = std::make_shared<DefaultLogger_t>(devNull, LogLevel_t::LOG_LEVEL_DEBUG);

		threadIDProvider = std::make_shared<DefaultThreadIdProvider_t>();
		timingProvider = std::make_shared<DefaultTimingProvider_t>();
		sessionIDProvider = std::make_shared<DefaultSessionIdProvider_t>();

		auto httpClientConfiguration = std::make_shared<HttpClientConfiguration_t>(Utf8String_t(""), 0, Utf8String_t(""));
		mockHTTPClientProvider = std::make_shared<testing::NiceMock<test::MockHTTPClientProvider>>();
		mockHTTPClient = std::make_shared<MockNiceHttpClient_t>(httpClientConfiguration);

		trustManager = std::make_shared<SslStrictTrustManager_t>();

		auto device = std::make_shared<Device_t>(Utf8String_t(""), Utf8String_t(""), Utf8String_t(""));

		beaconCacheConfiguration = std::make_shared<BeaconCacheConfiguration_t>(-1, -1, -1);
		beaconConfiguration = std::make_shared<BeaconConfiguration_t>();
		configuration = std::make_shared<Configuration_t>
		(
			device,
			OpenKitType_t::Type::DYNATRACE,
			Utf8String_t(APP_NAME),
			"",
			APP_ID,
			0,
			"0",
			"",
			sessionIDProvider,
			trustManager,
			beaconCacheConfiguration,
			beaconConfiguration
		);
		configuration->enableCapture();

		beaconCache = std::make_shared<BeaconCache_t>(logger);

		beaconSender = std::make_shared<BeaconSender_t>(logger, configuration, mockHTTPClientProvider, timingProvider);
		mockBeaconStrict = std::make_shared<MockStrictBeacon_t>(logger, beaconCache, configuration, nullptr, threadIDProvider, timingProvider);
		mockBeaconNice = std::make_shared<MockNiceBeacon_t>(logger, beaconCache, configuration, nullptr, threadIDProvider, timingProvider);

		action = std::make_shared<Action_t>(logger, mockBeaconNice, Utf8String_t("test action"));

		mockStrictParent = std::make_shared<MockStrictOpenKitComposite_t>();
	}

	void TearDown()
	{

	}
public:
	std::ostringstream devNull;
	ILogger_sp logger;
	IThreadIdProvider_sp threadIDProvider;
	ITimingProvider_sp timingProvider;
	ISessionIdProvider_sp sessionIDProvider;

	MockNiceHttpClient_sp mockHTTPClient;
	ISslTrustManager_sp trustManager;

	BeaconConfiguration_sp beaconConfiguration;
	BeaconCacheConfiguration_sp beaconCacheConfiguration;
	Configuration_sp configuration;
	BeaconCache_sp beaconCache;

	BeaconSender_sp beaconSender;
	MockStrictBeacon_sp mockBeaconStrict;
	MockNiceBeacon_sp mockBeaconNice;
	MockNiceHttpClientProvider_sp mockHTTPClientProvider;

	Action_sp action;
	MockStrictOpenKitComposite_sp mockStrictParent;

};



TEST_F(WebRequestTracerTest, defaultValues)
{
	//given
	ON_CALL(*mockBeaconNice, createSequenceNumber())
		.WillByDefault(testing::Return(42));
	ON_CALL(*mockBeaconNice, getCurrentTimestamp())
		.WillByDefault(testing::Return(123456789L));

	// test the constructor call
	auto testWebRequestTracer = std::make_shared<WebRequestTracer_t>(logger, action,mockBeaconNice, "");

	// verify default values
	auto tracerURL = testWebRequestTracer->getURL();
	ASSERT_TRUE(Utf8String_t(tracerURL).equals(Utf8String_t("<unknown>")));
	ASSERT_EQ(testWebRequestTracer->getResponseCode(), -1);
	ASSERT_EQ(testWebRequestTracer->getStartTime(), 123456789L);
	ASSERT_EQ(testWebRequestTracer->getEndTime(), -1);
	ASSERT_EQ(testWebRequestTracer->getStartSequenceNo(), 42);
	ASSERT_EQ(testWebRequestTracer->getEndSequenceNo(), -1);
	ASSERT_EQ(testWebRequestTracer->getBytesSent(), -1);
	ASSERT_EQ(testWebRequestTracer->getBytesReceived(), -1);
}

TEST_F(WebRequestTracerTest, getTag)
{
	// given
	Utf8String_t theTag(TAG);
	ON_CALL(*mockBeaconNice, createTag(testing::_, testing::_))
		.WillByDefault(testing::Return(theTag));
	auto testWebRequestTracer  = std::make_shared<WebRequestTracer_t>(logger, action, mockBeaconNice, "");

	//verify
	const char* actualValue = testWebRequestTracer->getTag();
	auto test = theTag.equals(actualValue);
	ASSERT_TRUE(test);
}

TEST_F(WebRequestTracerTest, aNewlyCreatedWebRequestTracerIsNotStopped)
{
	// given
	auto testWebRequestTracer  = std::make_shared<WebRequestTracer_t>(logger, action, mockBeaconNice, "");

	//verify
	ASSERT_FALSE(testWebRequestTracer->isStopped());
}

TEST_F(WebRequestTracerTest, aWebRequestTracerIsStoppedAfterStopHasBeenCalled)
{
	// given
	auto testWebRequestTracer  = std::make_shared<WebRequestTracer_t>(logger, action, mockBeaconNice, "");

	// when
	testWebRequestTracer->stop();

	// verify
	ASSERT_TRUE(testWebRequestTracer->isStopped());
}

TEST_F(WebRequestTracerTest, aWebRequestTracerIsStoppedAfterStopWithResponseCodeHasBeenCalled)
{
	// given
	auto testWebRequestTracer  = std::make_shared<WebRequestTracer_t>(logger, action, mockBeaconNice, "");

	// when
	testWebRequestTracer->stop(200);

	// verify
	ASSERT_TRUE(testWebRequestTracer->isStopped());
}

TEST_F(WebRequestTracerTest, setResponseCodeSetsTheResponseCode)
{
	// given
	auto testWebRequestTracer  = std::make_shared<WebRequestTracer_t>(logger, action, mockBeaconNice, "");

	// when
	auto obtained = testWebRequestTracer->setResponseCode(418);

	// verify
	ASSERT_EQ(obtained, testWebRequestTracer);
	ASSERT_EQ(testWebRequestTracer->getResponseCode(), 418);
}

TEST_F(WebRequestTracerTest, stopWithResponseCodeSetsTheResponseCode)
{
	// given
	auto testWebRequestTracer  = std::make_shared<WebRequestTracer_t>(logger, action, mockBeaconNice, "");

	// when
	testWebRequestTracer->stop(418);

	// verify
	ASSERT_EQ(testWebRequestTracer->getResponseCode(), 418);
}

TEST_F(WebRequestTracerTest, setResponseCodeDoesNotSetTheResponseCodeIfStopped)
{
	// given
	auto testWebRequestTracer  = std::make_shared<WebRequestTracer_t>(logger, action, mockBeaconNice, "");

	// when
	testWebRequestTracer->stop();
	auto obtained = testWebRequestTracer->setResponseCode(418);

	// verify
	ASSERT_EQ(obtained, testWebRequestTracer);
	ASSERT_EQ(testWebRequestTracer->getResponseCode(), -1);
}

TEST_F(WebRequestTracerTest, stopWithResponseCodeDoesNotSetTheResponseCodeIfStopped)
{
	// given
	auto testWebRequestTracer  = std::make_shared<WebRequestTracer_t>(logger, action, mockBeaconNice, "");
	testWebRequestTracer->stop(200);

	// when
	testWebRequestTracer->stop(404);

	// verify
	ASSERT_EQ(testWebRequestTracer->getResponseCode(), 200);
}

TEST_F(WebRequestTracerTest, setBytesSentSetsTheNumberOfSentBytes)
{
	// given
	auto testWebRequestTracer  = std::make_shared<WebRequestTracer_t>(logger, action, mockBeaconNice, "");

	// when
	auto obtained = testWebRequestTracer->setBytesSent(1234);

	// verify
	ASSERT_EQ(obtained, testWebRequestTracer);
	ASSERT_EQ(testWebRequestTracer->getBytesSent(), 1234);
}

TEST_F(WebRequestTracerTest, setBytesSentDoesNotSetAnythingIfStopped)
{
	// given
	auto testWebRequestTracer  = std::make_shared<WebRequestTracer_t>(logger, action, mockBeaconNice, "");

	// when
	testWebRequestTracer->stop();
	auto obtained = testWebRequestTracer->setBytesSent(1234 );

	// verify
	ASSERT_EQ(obtained, testWebRequestTracer);
	ASSERT_EQ(testWebRequestTracer->getBytesSent(), -1);
}

TEST_F(WebRequestTracerTest, setBytesSentDoesNotSetAnythingIfStoppedWithResponseCode)
{
	// given
	auto testWebRequestTracer  = std::make_shared<WebRequestTracer_t>(logger, action, mockBeaconNice, "");

	// when
	testWebRequestTracer->stop(200);
	auto obtained = testWebRequestTracer->setBytesSent(1234 );

	// verify
	ASSERT_EQ(obtained, testWebRequestTracer);
	ASSERT_EQ(testWebRequestTracer->getBytesSent(), -1);
}

TEST_F(WebRequestTracerTest, setBytesReceivedSetsTheNumberOfReceivedBytes)
{
	// given
	auto testWebRequestTracer  = std::make_shared<WebRequestTracer_t>(logger, action, mockBeaconNice, "");

	// when
	auto obtained = testWebRequestTracer->setBytesReceived(1234);

	// verify
	ASSERT_EQ(obtained, testWebRequestTracer);
	ASSERT_EQ(testWebRequestTracer->getBytesReceived(), 1234);
}

TEST_F(WebRequestTracerTest, setBytesReceivedDoesNotSetAnythingIfStopped)
{
	// given
	auto testWebRequestTracer  = std::make_shared<WebRequestTracer_t>(logger, action, mockBeaconNice, "");

	// when
	testWebRequestTracer->stop();
	auto obtained = testWebRequestTracer->setBytesReceived(1234);

	// verify
	ASSERT_EQ(obtained, testWebRequestTracer);
	ASSERT_EQ(testWebRequestTracer->getBytesReceived(), -1);
}

TEST_F(WebRequestTracerTest, setBytesReceivedDoesNotSetAnythingIfStoppedWithResponseCode)
{
	// given
	auto testWebRequestTracer  = std::make_shared<WebRequestTracer_t>(logger, action, mockBeaconNice, "");

	// when
	testWebRequestTracer->stop(200);
	auto obtained = testWebRequestTracer->setBytesReceived(1234);

	// verify
	ASSERT_EQ(obtained, testWebRequestTracer);
	ASSERT_EQ(testWebRequestTracer->getBytesReceived(), -1);
}

TEST_F(WebRequestTracerTest, startSetsTheStartTime)
{
	// given
	ON_CALL(*mockBeaconNice, getCurrentTimestamp())
		.WillByDefault(testing::Return(123456789L));
	auto testWebRequestTracer  = std::make_shared<WebRequestTracer_t>(logger, action, mockBeaconNice, "");

	// when
	auto obtained = testWebRequestTracer->start();

	// verify
	ASSERT_EQ(obtained, testWebRequestTracer);
	ASSERT_EQ(testWebRequestTracer->getStartTime(), 123456789L);
}

TEST_F(WebRequestTracerTest, startDoesNothingIfAlreadyStopped)
{
	// given
	ON_CALL(*mockBeaconNice, getCurrentTimestamp())
		.WillByDefault(testing::Return(123456789L));
	auto testWebRequestTracer  = std::make_shared<WebRequestTracer_t>(logger, action, mockBeaconNice, "");

	// when
	testWebRequestTracer->stop();
	auto obtained = testWebRequestTracer->start();

	// verify
	ASSERT_EQ(obtained, testWebRequestTracer);
	ASSERT_EQ(testWebRequestTracer->getStartTime(), 123456789L);
}

TEST_F(WebRequestTracerTest, startDoesNothingIfAlreadyStoppedWithResponseCode)
{
	// given
	ON_CALL(*mockBeaconNice, getCurrentTimestamp())
		.WillByDefault(testing::Return(123456789L));
	auto testWebRequestTracer  = std::make_shared<WebRequestTracer_t>(logger, action, mockBeaconNice, "");

	// when
	testWebRequestTracer->stop(200);
	auto obtained = testWebRequestTracer->start();

	// verify
	ASSERT_EQ(obtained, testWebRequestTracer);
	ASSERT_EQ(testWebRequestTracer->getStartTime(), 123456789L);
}

TEST_F(WebRequestTracerTest, stopCanOnlyBeExecutedOnce)
{
	// given
	ON_CALL(*mockBeaconNice, createSequenceNumber())
		.WillByDefault(testing::Return(42));

	// when
	EXPECT_CALL(*mockBeaconNice, createSequenceNumber())
		.Times(testing::Exactly(2));
	EXPECT_CALL(*mockBeaconNice, addWebRequest(testing::_, testing::_))
		.Times(testing::Exactly(1));

	auto testWebRequestTracer  = std::make_shared<WebRequestTracer_t>(logger, action, mockBeaconNice, "");

	testWebRequestTracer->stop();
	ASSERT_EQ(testWebRequestTracer->getEndSequenceNo(), 42);

	// when called another time
	testing::Mock::VerifyAndClearExpectations(&mockBeaconNice);

	EXPECT_CALL(*mockBeaconNice, createSequenceNumber())
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mockBeaconNice, addWebRequest(testing::_, testing::_))
		.Times(testing::Exactly(0));

	testWebRequestTracer->stop();
}

TEST_F(WebRequestTracerTest, stopWithResponseCodeCanOnlyBeExecutedOnce)
{
	// given
	ON_CALL(*mockBeaconNice, createSequenceNumber())
		.WillByDefault(testing::Return(42));

	// when
	EXPECT_CALL(*mockBeaconNice, createSequenceNumber())
		.Times(testing::Exactly(2));
	EXPECT_CALL(*mockBeaconNice, addWebRequest(testing::_, testing::_))
		.Times(testing::Exactly(1));

	auto testWebRequestTracer  = std::make_shared<WebRequestTracer_t>(logger, action, mockBeaconNice, "");

	testWebRequestTracer->stop(200);
	ASSERT_EQ(testWebRequestTracer->getEndSequenceNo(), 42);

	// when called another time
	testing::Mock::VerifyAndClearExpectations(&mockBeaconNice);

	EXPECT_CALL(*mockBeaconNice, createSequenceNumber())
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mockBeaconNice, addWebRequest(testing::_, testing::_))
		.Times(testing::Exactly(0));

	testWebRequestTracer->stop(404);
}

TEST_F(WebRequestTracerTest, aNewlyCreatedWebRequestTracerDoesNotAttacoTOTheParent)
{
	// expect
	EXPECT_CALL(*mockStrictParent, getActionId())
		.Times(testing::Exactly(2));

	// given, when
	auto target = std::make_shared<WebRequestTracer_t>(logger, mockStrictParent, mockBeaconNice, "");

	// then
	auto obtained = target->getParent();
	ASSERT_THAT(mockStrictParent, testing::Eq(obtained));

}