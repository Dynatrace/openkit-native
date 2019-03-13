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
#include "memory.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "caching/BeaconCache.h"

#include "providers/DefaultThreadIDProvider.h"
#include "providers/DefaultTimingProvider.h"
#include "protocol/ssl/SSLStrictTrustManager.h"
#include "providers/DefaultSessionIDProvider.h"
#include "providers/DefaultHTTPClientProvider.h"

#include "OpenKit/IRootAction.h"
#include "configuration/Configuration.h"
#include "core/util/DefaultLogger.h"

#include "../protocol/MockHTTPClient.h"
#include "../protocol/MockBeacon.h"
#include "MockBeaconSender.h"
#include "../providers/MockHTTPClientProvider.h"


using namespace core;

static const char APP_ID[] = "appID";
static const char APP_NAME[] = "appName";
static const char TAG[] = "THE_TAG";

class WebRequestTracerTest : public testing::Test
{
protected:
	void SetUp()
	{
		logger = std::make_shared<core::util::DefaultLogger>(devNull, openkit::LogLevel::LOG_LEVEL_DEBUG);

		threadIDProvider = std::make_shared<providers::DefaultThreadIDProvider>();
		timingProvider = std::make_shared<providers::DefaultTimingProvider>();
		sessionIDProvider = std::make_shared<providers::DefaultSessionIDProvider>();

		std::shared_ptr<configuration::HTTPClientConfiguration> httpClientConfiguration = std::make_shared<configuration::HTTPClientConfiguration>(core::UTF8String(""), 0, core::UTF8String(""));
		mockHTTPClientProvider = std::make_shared<testing::NiceMock<test::MockHTTPClientProvider>>();
		mockHTTPClient = std::shared_ptr<testing::NiceMock<test::MockHTTPClient>>(new testing::NiceMock<test::MockHTTPClient>(httpClientConfiguration));

		trustManager = std::make_shared<protocol::SSLStrictTrustManager>();

		std::shared_ptr<configuration::Device> device = std::shared_ptr<configuration::Device>(new configuration::Device(core::UTF8String(""), core::UTF8String(""), core::UTF8String("")));

		beaconCacheConfiguration = std::make_shared<configuration::BeaconCacheConfiguration>(-1, -1, -1);
		beaconConfiguration = std::make_shared<configuration::BeaconConfiguration>();
		configuration = std::shared_ptr<configuration::Configuration>(new configuration::Configuration(device, configuration::OpenKitType::Type::DYNATRACE,
			core::UTF8String(APP_NAME), "", APP_ID, 0, "",
			sessionIDProvider, trustManager, beaconCacheConfiguration, beaconConfiguration));
		configuration->enableCapture();

		beaconCache = std::make_shared<caching::BeaconCache>(logger);

		beaconSender = std::make_shared<core::BeaconSender>(logger, configuration, mockHTTPClientProvider, timingProvider);
		mockBeaconStrict = std::make_shared<testing::StrictMock<test::MockBeacon>>(logger, beaconCache, configuration, core::UTF8String(""), threadIDProvider, timingProvider);	
		mockBeaconNice = std::make_shared<testing::NiceMock<test::MockBeacon>>(logger, beaconCache, configuration, core::UTF8String(""), threadIDProvider, timingProvider);	

		action = std::make_shared<core::Action>(logger, mockBeaconNice, core::UTF8String("test action"));
	}

	void TearDown()
	{

	}
public:
	std::ostringstream devNull;
	std::shared_ptr<openkit::ILogger> logger;
	std::shared_ptr<providers::IThreadIDProvider> threadIDProvider;
	std::shared_ptr<providers::ITimingProvider> timingProvider;
	std::shared_ptr<providers::ISessionIDProvider> sessionIDProvider;
	
	std::shared_ptr<testing::NiceMock<test::MockHTTPClient>> mockHTTPClient;
	std::shared_ptr<openkit::ISSLTrustManager> trustManager;

	std::shared_ptr<configuration::BeaconConfiguration> beaconConfiguration;
	std::shared_ptr<configuration::BeaconCacheConfiguration> beaconCacheConfiguration;
	std::shared_ptr<configuration::Configuration> configuration;
	std::shared_ptr<caching::BeaconCache> beaconCache;

	std::shared_ptr<core::BeaconSender> beaconSender;
	std::shared_ptr<testing::StrictMock<test::MockBeacon>> mockBeaconStrict;
	std::shared_ptr<testing::NiceMock<test::MockBeacon>> mockBeaconNice;
	std::shared_ptr<testing::NiceMock<test::MockHTTPClientProvider>> mockHTTPClientProvider;

	std::shared_ptr<core::Action> action;
};



TEST_F(WebRequestTracerTest, defaultValues)
{
	//given
	ON_CALL(*mockBeaconNice, createSequenceNumber())
		.WillByDefault(testing::Return(42));
	ON_CALL(*mockBeaconNice, getCurrentTimestamp())
		.WillByDefault(testing::Return(123456789L));

	// test the constructor call
	std::shared_ptr<core::WebRequestTracerBase> testWebRequestTracer = std::make_shared<core::WebRequestTracerBase>(logger, mockBeaconNice, action->getID());

	// verify default values
	auto tracerURL = testWebRequestTracer->getURL();
	ASSERT_TRUE(core::UTF8String(tracerURL).equals(core::UTF8String("<unknown>")));
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
	core::UTF8String theTag(TAG);
	ON_CALL(*mockBeaconNice, createTag(testing::_, testing::_))
		.WillByDefault(testing::Return(theTag));
	std::shared_ptr<core::WebRequestTracerBase> testWebRequestTracer = std::make_shared<core::WebRequestTracerBase>(logger, mockBeaconNice, action->getID());

	//verify
	const char* actualValue = testWebRequestTracer->getTag();
	auto test = theTag.equals(actualValue);
	ASSERT_TRUE(test);
}

TEST_F(WebRequestTracerTest, aNewlyCreatedWebRequestTracerIsNotStopped)
{
	// given
	std::shared_ptr<core::WebRequestTracerBase> testWebRequestTracer = std::make_shared<core::WebRequestTracerBase>(logger, mockBeaconNice, action->getID());

	//verify
	ASSERT_FALSE(testWebRequestTracer->isStopped());
}

TEST_F(WebRequestTracerTest, aWebRequestTracerIsStoppedAfterStopHasBeenCalled)
{
	// given
	std::shared_ptr<core::WebRequestTracerBase> testWebRequestTracer = std::make_shared<core::WebRequestTracerBase>(logger, mockBeaconNice, action->getID());

	// when
	testWebRequestTracer->stop();

	// verify
	ASSERT_TRUE(testWebRequestTracer->isStopped());
}

TEST_F(WebRequestTracerTest, setResponseCodeSetsTheResponseCode)
{
	// given
	std::shared_ptr<core::WebRequestTracerBase> testWebRequestTracer = std::make_shared<core::WebRequestTracerBase>(logger, mockBeaconNice, action->getID());

	// when
	auto obtained = testWebRequestTracer->setResponseCode(418);

	// verify
	ASSERT_EQ(obtained, testWebRequestTracer);
	ASSERT_EQ(testWebRequestTracer->getResponseCode(), 418);
}

TEST_F(WebRequestTracerTest, setResponseCodeDoesNotSetTheResponseCodeIfStopped)
{
	// given
	std::shared_ptr<core::WebRequestTracerBase> testWebRequestTracer = std::make_shared<core::WebRequestTracerBase>(logger, mockBeaconNice, action->getID());

	// when
	testWebRequestTracer->stop();
	auto obtained = testWebRequestTracer->setResponseCode(418);

	// verify
	ASSERT_EQ(obtained, testWebRequestTracer);
	ASSERT_EQ(testWebRequestTracer->getResponseCode(), -1);
}

TEST_F(WebRequestTracerTest, setBytesSentSetsTheNumberOfSentBytes)
{
	// given
	std::shared_ptr<core::WebRequestTracerBase> testWebRequestTracer = std::make_shared<core::WebRequestTracerBase>(logger, mockBeaconNice, action->getID());

	// when
	auto obtained = testWebRequestTracer->setBytesSent(1234);

	// verify
	ASSERT_EQ(obtained, testWebRequestTracer);
	ASSERT_EQ(testWebRequestTracer->getBytesSent(), 1234);
}

TEST_F(WebRequestTracerTest, setBytesSentDoesNotSetAnythingIfStopped)
{
	// given
	std::shared_ptr<core::WebRequestTracerBase> testWebRequestTracer = std::make_shared<core::WebRequestTracerBase>(logger, mockBeaconNice, action->getID());

	// when
	testWebRequestTracer->stop();
	auto obtained = testWebRequestTracer->setBytesSent(1234 );

	// verify
	ASSERT_EQ(obtained, testWebRequestTracer);
	ASSERT_EQ(testWebRequestTracer->getBytesSent(), -1);
}

TEST_F(WebRequestTracerTest, setBytesReceivedSetsTheNumberOfReceivedBytes)
{
	// given
	std::shared_ptr<core::WebRequestTracerBase> testWebRequestTracer = std::make_shared<core::WebRequestTracerBase>(logger, mockBeaconNice, action->getID());

	// when
	auto obtained = testWebRequestTracer->setBytesReceived(1234);

	// verify
	ASSERT_EQ(obtained, testWebRequestTracer);
	ASSERT_EQ(testWebRequestTracer->getBytesReceived(), 1234);
}

TEST_F(WebRequestTracerTest, setBytesReceivedDoesNotSetAnythingIfStopped)
{
	// given
	std::shared_ptr<core::WebRequestTracerBase> testWebRequestTracer = std::make_shared<core::WebRequestTracerBase>(logger, mockBeaconNice, action->getID());

	// when
	testWebRequestTracer->stop();
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
	std::shared_ptr<core::WebRequestTracerBase> testWebRequestTracer = std::make_shared<core::WebRequestTracerBase>(logger, mockBeaconNice, action->getID());

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
	std::shared_ptr<core::WebRequestTracerBase> testWebRequestTracer = std::make_shared<core::WebRequestTracerBase>(logger, mockBeaconNice, action->getID());

	// when
	testWebRequestTracer->stop();
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

	std::shared_ptr<core::WebRequestTracerBase> testWebRequestTracer = std::make_shared<core::WebRequestTracerBase>(logger, mockBeaconNice, action->getID());




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