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

#include <core/configuration/ServerConfiguration.h>
#include "CustomMatchers.h"
#include "builder/TestBeaconSendingContextBuilder.h"
#include "mock/MockIBeaconSendingState.h"
#include "../configuration/mock/MockIBeaconCacheConfiguration.h"
#include "../configuration/mock/MockIBeaconConfiguration.h"
#include "../configuration/mock/MockIHTTPClientConfiguration.h"
#include "../configuration/mock/MockIPrivacyConfiguration.h"
#include "../objects/mock/MockSessionInternals.h"
#include "../../api/mock/MockILogger.h"
#include "../../api/mock/MockISslTrustManager.h"
#include "../../protocol/mock/MockIHTTPClient.h"
#include "../../protocol/mock/MockIStatusResponse.h"
#include "../../providers/mock/MockIHTTPClientProvider.h"
#include "../../providers/mock/MockISessionIDProvider.h"
#include "../../providers/mock/MockITimingProvider.h"

#include "core/UTF8String.h"
#include "core/communication/BeaconSendingContext.h"
#include "core/communication/IBeaconSendingState.h"
#include "core/configuration/IHTTPClientConfiguration.h"
#include "core/configuration/ServerConfiguration.h"
#include "protocol/StatusResponse.h"
#include "protocol/ResponseAttributes.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace test;

using BeaconSendingContext_t = core::communication::BeaconSendingContext;
using BeaconSendingContextBuilder_sp = std::shared_ptr<TestBeaconSendingContextBuilder>;
using IBeaconSendingState_t = core::communication::IBeaconSendingState;
using IBeaconSendingState_up = std::unique_ptr<IBeaconSendingState_t>;
using IHTTPClientConfiguration_sp = std::shared_ptr<core::configuration::IHTTPClientConfiguration>;
using MockIHTTPClientConfiguration_sp = std::shared_ptr<MockIHTTPClientConfiguration>;
using MockNiceILogger_sp = std::shared_ptr<testing::NiceMock<MockILogger>>;
using MockNiceIHTTPClientProvider_sp = std::shared_ptr<testing::NiceMock<MockIHTTPClientProvider>>;
using MockNiceITimingProvider_sp = std::shared_ptr<testing::NiceMock<MockITimingProvider>>;
using MockNiceSession_t = testing::NiceMock<MockSessionInternals>;
using MockStrictSession_t = testing::StrictMock<MockSessionInternals>;
using MockStrictIBeaconSendingState_sp = std::shared_ptr<testing::StrictMock<MockIBeaconSendingState>>;
using ServerConfiguration_t = core::configuration::ServerConfiguration;
using Utf8String_t = core::UTF8String;

class BeaconSendingContextTest : public testing::Test
{
protected:

	MockNiceILogger_sp mockLogger;
	MockNiceIHTTPClientProvider_sp mockHTTPClientProvider;
	MockNiceITimingProvider_sp mockTimingProvider;
	MockIHTTPClientConfiguration_sp mockHttpClientConfig;


	void SetUp() override
	{
		mockLogger = MockILogger::createNice();

		auto httpClient = MockIHTTPClient::createNice();
		ON_CALL(*httpClient, sendBeaconRequest(testing::_, testing::_))
			.WillByDefault(testing::Return(MockIStatusResponse::createNice()));

		mockHTTPClientProvider = MockIHTTPClientProvider::createNice();
		ON_CALL(*mockHTTPClientProvider, createClient(testing::_, testing::_))
			.WillByDefault(testing::Return(httpClient));

		mockTimingProvider = MockITimingProvider::createNice();
		mockHttpClientConfig = MockIHTTPClientConfiguration::createNice();
	}

	BeaconSendingContextBuilder_sp createBeaconSendingContext()
	{
		auto builder = std::make_shared<TestBeaconSendingContextBuilder>();
		builder->with(mockLogger)
			.with(mockHttpClientConfig)
			.with(mockHTTPClientProvider)
			.with(mockTimingProvider)
		;

		return builder;
	}
};

TEST_F(BeaconSendingContextTest, currentStateIsInitializedAccordingly)
{
	// given
	auto target = createBeaconSendingContext()->build();

	// when
	auto obtained = target->getCurrentState();

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained, IsABeaconSendingInitState());
}

TEST_F(BeaconSendingContextTest, setCurrentStateChangesState)
{
	// given
	auto mockState = MockIBeaconSendingState::createStrict();
	auto target = createBeaconSendingContext()->build();

	// when
	target->setNextState(mockState);

	// then
	ASSERT_THAT(target->getCurrentState(), testing::NotNull());
	ASSERT_THAT(target->getNextState(), testing::Eq(mockState));
}

TEST_F(BeaconSendingContextTest, executeCurrentStateCallsExecuteOnCurrentState)
{
	// with
	auto mockState = new MockIBeaconSendingState();

	// expect
	EXPECT_CALL(*mockState, execute(testing::_))
		.Times(testing::Exactly(1));

	// given
	auto target = createBeaconSendingContext()
		->with(std::unique_ptr<IBeaconSendingState_t>(mockState))
		.build();

	// when
	target->executeCurrentState();
}

TEST_F(BeaconSendingContextTest, initCompleteSuccessAndWait)
{
	// given
	auto target = createBeaconSendingContext()->build();
	target->setInitCompleted(true);

	// when
	auto obtained = target->waitForInit();

	// then
	ASSERT_TRUE(obtained);
}

TEST_F(BeaconSendingContextTest, requestShutdown)
{
	// given
	auto target = createBeaconSendingContext()->build();
	ASSERT_THAT(target->isShutdownRequested(), testing::Eq(false));

	// when
	target->requestShutdown();

	// then
	ASSERT_THAT(target->isShutdownRequested(), testing::Eq(true));
}

TEST_F(BeaconSendingContextTest, initCompleteFailureAndWait)
{
	// given
	auto target = createBeaconSendingContext()->build();
	target->setInitCompleted(false);

	// when
	bool obtained = target->waitForInit();

	// then
	ASSERT_FALSE(obtained);
}

TEST_F(BeaconSendingContextTest, waitForInitCompleteTimeout)
{
	// given
	auto target = createBeaconSendingContext()->build();

	// when init complete was never set and timeout will be reached
	bool obtained = target->waitForInit(1);

	// then
	ASSERT_FALSE(obtained);
}

TEST_F(BeaconSendingContextTest, waitForInitCompleteWhenInitCompletedSuccessfully)
{
	// given
	auto target = createBeaconSendingContext()->build();
	target->setInitCompleted(true);

	// when init complete was never set and timeout will be reached
	bool obtained = target->waitForInit(1);

	// then
	ASSERT_TRUE(obtained);
}

TEST_F(BeaconSendingContextTest, waitForInitCompleteWhenInitCompletedNotSuccessfully)
{
	// given
	auto target =createBeaconSendingContext()->build();
	target->setInitCompleted(false);

	// when init complete was never set and timeout will be reached
	bool obtained = target->waitForInit(1);

	// then
	ASSERT_FALSE(obtained);
}

TEST_F(BeaconSendingContextTest, aDefaultConstructedContextIsNotInitialized)
{
	// given
	auto target = createBeaconSendingContext()->build();

	// that
	ASSERT_FALSE(target->isInitialized());
}

TEST_F(BeaconSendingContextTest, successfullyInitializedContextIsInitialized)
{
	// given
	auto target = createBeaconSendingContext()->build();

	// when initialized
	target->setInitCompleted(true);

	// then
	ASSERT_TRUE(target->isInitialized());
}

TEST_F(BeaconSendingContextTest, notSuccessfullyInitializedContextIsNotInitialzed)
{
	// given
	auto target = createBeaconSendingContext()->build();
	target->setInitCompleted(false);

	// when, then
	ASSERT_THAT(target->isInitialized(), testing::Eq(false));
}

TEST_F(BeaconSendingContextTest, isInTerminalStateChecksCurrentState)
{
	// with
	auto terminalState = MockIBeaconSendingState::createStrict();
	ON_CALL(*terminalState, isTerminalState())
		.WillByDefault(testing::Return(true));

	// given
	auto target = createBeaconSendingContext()->build();

	ASSERT_THAT(target->isInTerminalState(), testing::Eq(false));
	ASSERT_THAT(target->getCurrentState(), IsABeaconSendingInitState());

	// when
	target->setNextState(terminalState);

	// then
	ASSERT_THAT(target->isInTerminalState(), testing::Eq(false));
	ASSERT_THAT(target->getCurrentState(), IsABeaconSendingInitState());
}

TEST_F(BeaconSendingContextTest, isCaptureOnIsTakenFromDefaultServerConfig)
{
	// given, when
	auto target = createBeaconSendingContext()->build();

	// then
	ASSERT_THAT(target->isCaptureOn(), testing::Eq(ServerConfiguration_t::DEFAULT->isCaptureEnabled()));
}


TEST_F(BeaconSendingContextTest, setAndGetLastOpenSessionBeaconSendTime)
{
	// given
	auto target = createBeaconSendingContext()->build();

	// when
	target->setLastOpenSessionBeaconSendTime(1234L);

	// then
	ASSERT_EQ(target->getLastOpenSessionBeaconSendTime(), 1234L);

	// and when
	target->setLastOpenSessionBeaconSendTime(5678L);

	// then
	ASSERT_EQ(target->getLastOpenSessionBeaconSendTime(), 5678L);
}

TEST_F(BeaconSendingContextTest, setAndGetLastStatusCheckTime)
{
	// given
	auto target = createBeaconSendingContext()->build();

	// when
	target->setLastStatusCheckTime(1234L);

	// then
	ASSERT_EQ(target->getLastStatusCheckTime(), 1234L);

	// and when
	target->setLastStatusCheckTime(5678L);

	// then
	ASSERT_EQ(target->getLastStatusCheckTime(), 5678L);
}

TEST_F(BeaconSendingContextTest, getSendIntervalIsTakeFromDefaultSessionConfig)
{
	// given
	auto target = createBeaconSendingContext()->build();

	// when
	auto obtained = target->getSendInterval();

	// then
	ASSERT_THAT(obtained, testing::Eq(ServerConfiguration_t::DEFAULT->getSendIntervalInMilliseconds()));
}

TEST_F(BeaconSendingContextTest, getHTTPClientProvider)
{
	// given
	auto target = createBeaconSendingContext()->build();

	// when
	auto obtained = target->getHTTPClientProvider();

	// then
	ASSERT_EQ(obtained, mockHTTPClientProvider);
}

TEST_F(BeaconSendingContextTest, getHTTPClient)
{
	// given
	auto mockClient = MockIHTTPClient::createStrict();
	auto httpClientProvider = MockIHTTPClientProvider::createStrict();
	EXPECT_CALL(*httpClientProvider, createClient(testing::_, testing::_))
		.Times(testing::Exactly(1))
		.WillOnce(testing::Return(mockClient));

	auto target = createBeaconSendingContext()
		->with(httpClientProvider)
		.build();

	// when
	auto obtained = target->getHTTPClient();

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained, testing::Eq(mockClient));
}

TEST_F(BeaconSendingContextTest, getCurrentTimestamp)
{
	// with
	int64_t timestamp = 1234567890;
	auto timingProvider = MockITimingProvider::createStrict();

	// expect
	EXPECT_CALL(*timingProvider, provideTimestampInMilliseconds())
		.Times(testing::Exactly(1))
		.WillOnce(testing::Return(timestamp));

	// given
	auto target = createBeaconSendingContext()
		->with(timingProvider)
		.build();

	// when
	auto obtained = target->getCurrentTimestamp();

	// then
	ASSERT_THAT(obtained, testing::Eq(timestamp));
}

TEST_F(BeaconSendingContextTest, sleepDefaultTime)
{
	// with
	auto timingProvider = MockITimingProvider::createStrict();

	// given
	auto target = createBeaconSendingContext()
		->with(timingProvider)
		.build();

	// when
	auto start = std::chrono::system_clock::now();
	target->sleep();
	auto duration = std::chrono::system_clock::now() - start;

	// then ensure sleep is correct
	ASSERT_GE(duration, BeaconSendingContext_t::DEFAULT_SLEEP_TIME_MILLISECONDS);
}

TEST_F(BeaconSendingContextTest, sleepWithGivenTime)
{
	// with
	auto timingProvider = MockITimingProvider::createStrict();
	ON_CALL(*timingProvider, provideTimestampInMilliseconds())
		.WillByDefault(testing::Return(1234567890L));

	// given
	auto target = createBeaconSendingContext()
		->with(timingProvider)
		.build();

	// when
	auto start = std::chrono::system_clock::now();
	target->sleep(100L);
	auto duration = std::chrono::system_clock::now() - start;

	// then ensure sleep is correct
	ASSERT_GE(duration, std::chrono::milliseconds(100L));
}

TEST_F(BeaconSendingContextTest, aDefaultConstructedContextDoesNotStoreAnySessions)
{
	// given
	auto target = createBeaconSendingContext()->build();

	// then
	ASSERT_TRUE(target->getAllNotConfiguredSessions().empty());
	ASSERT_TRUE(target->getAllOpenAndConfiguredSessions().empty());
	ASSERT_TRUE(target->getAllFinishedAndConfiguredSessions().empty());
}

TEST_F(BeaconSendingContextTest, addSession)
{
	// given
	auto target = createBeaconSendingContext()->build();

	auto mockSessionOne = MockSessionInternals::createStrict();
	auto mockSessionTwo = MockSessionInternals::createStrict();

	// when
	target->addSession(mockSessionOne);

	// then
	ASSERT_THAT(target->getSessionCount(), testing::Eq(1));

	// and when
	target->addSession(mockSessionTwo);

	// then
	ASSERT_THAT(target->getSessionCount(), testing::Eq(2));
}

TEST_F(BeaconSendingContextTest, removeSession)
{
	// given
	auto target = createBeaconSendingContext()->build();

	auto mockSessionOne = MockSessionInternals::createStrict();
	auto mockSessionTwo = MockSessionInternals::createStrict();

	target->addSession(mockSessionOne);
	target->addSession(mockSessionTwo);
	ASSERT_THAT(target->getSessionCount(), testing::Eq(2));

	// when
	target->removeSession(mockSessionOne);

	// then
	ASSERT_THAT(target->getSessionCount(), testing::Eq(1));

	// and when
	target->removeSession(mockSessionTwo);

	// then
	ASSERT_THAT(target->getSessionCount(), testing::Eq(0));
}

TEST_F(BeaconSendingContextTest, disableCaptureAndClearModifiesCaptureFlag)
{
	// given
	auto target = createBeaconSendingContext()->build();
	ASSERT_THAT(target->isCaptureOn(), testing::Eq(true));

	// when
	target->disableCaptureAndClear();

	// then
	ASSERT_THAT(target->isCaptureOn(), testing::Eq(false));
}

TEST_F(BeaconSendingContextTest, disableCaptureAndClearClearsCapturedSessionData)
{
	// with
	auto mockSession = MockSessionInternals::createNice();

	// expect
	EXPECT_CALL(*mockSession, clearCapturedData()).Times(1);

	// given
	auto target = createBeaconSendingContext()->build();
	target->addSession(mockSession);

	// when
	target->disableCaptureAndClear();

	// then
	ASSERT_THAT(target->getSessionCount(), testing::Eq(1));
}

TEST_F(BeaconSendingContextTest, disableCaptureAndClearRemovesFinishedSession)
{
	// given
	auto mockSession = MockSessionInternals::createNice();

	// expect
	EXPECT_CALL(*mockSession, isFinished()).Times(1)
		.WillOnce(testing::Return(true));
	EXPECT_CALL(*mockSession, clearCapturedData()).Times(1);

	auto target = createBeaconSendingContext()->build();
	target->addSession(mockSession);

	// when
	target->disableCaptureAndClear();

	// then
	ASSERT_THAT(target->getSessionCount(), testing::Eq(0));
}

TEST_F(BeaconSendingContextTest, handleStatusResponseDisablesCaptureIfResponseIsNull)
{
	// given
	auto target = createBeaconSendingContext()->build();
	ASSERT_THAT(target->isCaptureOn(), testing::Eq(true));

	// when
	target->handleStatusResponse(nullptr);

	// then
	ASSERT_THAT(target->isCaptureOn(), testing::Eq(false));
}

TEST_F(BeaconSendingContextTest, handleStatusResponseClearsSessionDataIfResponseIsNull)
{
	// with
	auto mockSession = MockSessionInternals::createNice();

	// expect
	EXPECT_CALL(*mockSession, clearCapturedData()).Times(1);

	// given
	auto target = createBeaconSendingContext()->build();
	target->addSession(mockSession);

	// when
	target->handleStatusResponse(nullptr);

	// then
	ASSERT_THAT(target->getSessionCount(), testing::Eq(1));
}

TEST_F(BeaconSendingContextTest, handleStatusResponseRemovesFinishedSessionIfResponseIsNull)
{
	// with
	auto mockSession = MockSessionInternals::createNice();

	// expect
	EXPECT_CALL(*mockSession, isFinished())
		.Times(1)
		.WillOnce(testing::Return(true));
	EXPECT_CALL(*mockSession, clearCapturedData())
		.Times(1);

	// given
	auto target = createBeaconSendingContext()->build();
	target->addSession(mockSession);

	// when
	target->handleStatusResponse(nullptr);

	// then
	ASSERT_THAT(target->getSessionCount(), testing::Eq(0));
}

TEST_F(BeaconSendingContextTest, handleStatusResponseDisablesCaptureIfResponseCodeIsNotOk)
{
	// given
	auto response = MockIStatusResponse::createNice();
	ON_CALL(*response, getResponseCode()).WillByDefault(testing::Return(404));

	auto target = createBeaconSendingContext()->build();
	ASSERT_THAT(target->isCaptureOn(), testing::Eq(true));

	// when
	target->handleStatusResponse(response);

	// then
	ASSERT_THAT(target->isCaptureOn(), testing::Eq(false));
}

TEST_F(BeaconSendingContextTest, handleStatusResponseClearsSessionDataIfResponseCodeIsNotOk)
{
	// with
	auto response = MockIStatusResponse::createNice();
	ON_CALL(*response, getResponseCode()).WillByDefault(testing::Return(404));

	auto mockSession = MockSessionInternals::createNice();

	// expect
	EXPECT_CALL(*mockSession, clearCapturedData())
		.Times(1);

	auto target = createBeaconSendingContext()->build();
	target->addSession(mockSession);

	// when
	target->handleStatusResponse(response);

	// then
	ASSERT_THAT(target->getSessionCount(), testing::Eq(1));
}

TEST_F(BeaconSendingContextTest, handleStatusResponseRemovesFinishedSessionsIfResponseCodeIsNotOk)
{
	// with
	auto response = MockIStatusResponse::createNice();
	ON_CALL(*response, getResponseCode()).WillByDefault(testing::Return(404));

	auto mockSession = MockSessionInternals::createNice();

	// expect
	EXPECT_CALL(*mockSession, isFinished())
		.Times(1)
		.WillOnce(testing::Return(true));
	EXPECT_CALL(*mockSession, clearCapturedData())
		.Times(1);

	// given
	auto target = createBeaconSendingContext()->build();
	target->addSession(mockSession);

	// when
	target->handleStatusResponse(response);

	// then
	ASSERT_THAT(target->getSessionCount(), testing::Eq(0));
}

TEST_F(BeaconSendingContextTest, handleStatusResponseClearsSessionDataIfResponseIsCaptureOff)
{
	// with
	auto responseAttributes = protocol::ResponseAttributes::withUndefinedDefaults().withCapture(false).build();
	auto response = protocol::StatusResponse::createSuccessResponse(
		mockLogger,
		responseAttributes,
		200,
		protocol::IStatusResponse::ResponseHeaders()
	);

	auto mockSession = MockSessionInternals::createNice();

	// expect
	EXPECT_CALL(*mockSession, clearCapturedData()).Times(1);

	// given
	auto target = createBeaconSendingContext()->build();
	target->addSession(mockSession);

	// when
	target->handleStatusResponse(response);

	// then
	ASSERT_THAT(target->getSessionCount(), testing::Eq(1));
}

TEST_F(BeaconSendingContextTest, handleStatusResponseRemovesFinishedSessionsIfResponseIsCaptureOff)
{
	// with
	auto responseAttributes = protocol::ResponseAttributes::withUndefinedDefaults().withCapture(false).build();
	auto response = protocol::StatusResponse::createSuccessResponse(
		mockLogger,
		responseAttributes,
		200,
		protocol::IStatusResponse::ResponseHeaders()
	);

	auto mockSession = MockSessionInternals::createNice();

	// expect
	EXPECT_CALL(*mockSession, isFinished())
		.Times(1)
		.WillOnce(testing::Return(true));
	EXPECT_CALL(*mockSession, clearCapturedData())
		.Times(1);

	// given
	auto target = createBeaconSendingContext()->build();
	target->addSession(mockSession);

	// when
	target->handleStatusResponse(response);

	// then
	ASSERT_THAT(target->getSessionCount(), testing::Eq(0));
}

TEST_F(BeaconSendingContextTest, handleStatusResponseUpdatesSendInterval)
{
	// given
	const int32_t sendInterval = 999;

	auto responseAttributes = protocol::ResponseAttributes::withUndefinedDefaults()
		.withCapture(true)
		.withSendIntervalInMilliseconds(sendInterval)
		.build();
	auto response = protocol::StatusResponse::createSuccessResponse(
		mockLogger,
		responseAttributes,
		200,
		protocol::IStatusResponse::ResponseHeaders()
	);

	auto mockSession = MockSessionInternals::createStrict();

	auto target = createBeaconSendingContext()->build();
	target->addSession(mockSession);
	ASSERT_THAT(target->getSendInterval(), testing::Ne(sendInterval));

	// when
	target->handleStatusResponse(response);
	auto obtained = target->getSendInterval();

	// then
	ASSERT_THAT(obtained, testing::Eq(sendInterval));
}

TEST_F(BeaconSendingContextTest, handleStatusResponseUpdatesCaptureStateToFalse)
{
	// with
	auto responseAttributes = protocol::ResponseAttributes::withUndefinedDefaults()
		.withCapture(false)
		.build();
	auto response = protocol::StatusResponse::createSuccessResponse(
		mockLogger,
		responseAttributes,
		200,
		protocol::IStatusResponse::ResponseHeaders()
	);

	auto mockSession = MockSessionInternals::createNice();

	// expect
	EXPECT_CALL(*mockSession, clearCapturedData()).Times(1);

	// given
	auto target = createBeaconSendingContext()->build();
	target->addSession(mockSession);
	ASSERT_THAT(target->isCaptureOn(), testing::Eq(true));

	// when
	target->handleStatusResponse(response);

	// then
	ASSERT_THAT(target->isCaptureOn(), testing::Eq(false));
}

TEST_F(BeaconSendingContextTest, handleStatusResponseUpdatesCaptureStateToTrue)
{
	// given
	auto responseAttributes = protocol::ResponseAttributes::withUndefinedDefaults()
		.withCapture(true)
		.build();
	auto response = protocol::StatusResponse::createSuccessResponse(
		mockLogger,
		responseAttributes,
		200,
		protocol::IStatusResponse::ResponseHeaders()
	);

	auto mockSession = MockSessionInternals::createStrict();

	auto target = createBeaconSendingContext()->build();
	target->disableCaptureAndClear();
	target->addSession(mockSession);
	ASSERT_THAT(target->isCaptureOn(), testing::Eq(false));

	// when
	target->handleStatusResponse(response);

	// then
	ASSERT_THAT(target->isCaptureOn(), testing::Eq(true));
}

TEST_F(BeaconSendingContextTest, handleStatusResponseUpdatesHttpClientConfig)
{
	// with
	Utf8String_t baseUrl("https://localhost:9999/1");
	Utf8String_t applicationId("some cryptic appId");
	const int32_t initialServerId = 42;
	auto trustManager = MockISslTrustManager::createNice();

	const int32_t serverId = 73;
	auto responseAttributes = protocol::ResponseAttributes::withUndefinedDefaults()
		.withCapture(true)
		.withServerId(serverId)
		.build();
	auto response = protocol::StatusResponse::createSuccessResponse(
		mockLogger,
		responseAttributes,
		200,
		protocol::IStatusResponse::ResponseHeaders()
	);

	// expect
	EXPECT_CALL(*mockHttpClientConfig, getBaseURL())
		.Times(1)
		.WillRepeatedly(testing::ReturnRef(baseUrl));
	EXPECT_CALL(*mockHttpClientConfig, getApplicationID())
		.Times(1)
		.WillRepeatedly(testing::ReturnRef(applicationId));
	EXPECT_CALL(*mockHttpClientConfig, getServerID())
		.Times(2)
		.WillRepeatedly(testing::Return(initialServerId));
	EXPECT_CALL(*mockHttpClientConfig, getSSLTrustManager())
		.Times(1)
		.WillRepeatedly(testing::Return(trustManager));

	IHTTPClientConfiguration_sp httpConfigCapture = nullptr;
	EXPECT_CALL(*mockHTTPClientProvider, createClient(testing::_, testing::_))
		.Times(1)
		.WillOnce(
			testing::DoAll(
				testing::SaveArg<1>(&httpConfigCapture),
				testing::Return(MockIHTTPClient::createNice())
			)
		);

	// given
	auto target = createBeaconSendingContext()->build();

	// when
	target->handleStatusResponse(response);
	target->getHTTPClient();

	// then
	ASSERT_THAT(httpConfigCapture, testing::NotNull());
	ASSERT_THAT(httpConfigCapture, testing::Ne(mockHttpClientConfig));
	ASSERT_THAT(httpConfigCapture->getServerID(), testing::Eq(serverId));
	ASSERT_THAT(httpConfigCapture->getBaseURL(), testing::Eq(baseUrl));
	ASSERT_THAT(httpConfigCapture->getApplicationID(), testing::Eq(applicationId));
	ASSERT_THAT(httpConfigCapture->getSSLTrustManager(), testing::Eq(trustManager));
}

TEST_F(BeaconSendingContextTest, onDefaultGetAllNotConfiguredSessionsIsEmpty)
{
	// given
	auto target = createBeaconSendingContext()->build();

	// when
	auto obtained = target->getAllNotConfiguredSessions();

	// then
	ASSERT_THAT(obtained.size(), testing::Eq(0));
}

TEST_F(BeaconSendingContextTest, getAllNotConfiguredSessionsReturnsOnlyNotConfiguredSessions)
{
	// given
	auto relevantSession = MockSessionInternals::createNice();
	ON_CALL(*relevantSession, isConfigured()).WillByDefault(testing::Return(false));

	auto ignoredSession = MockSessionInternals::createNice();
	ON_CALL(*ignoredSession, isConfigured()).WillByDefault(testing::Return(true));

	auto target = createBeaconSendingContext()->build();
	target->addSession(relevantSession);
	target->addSession(ignoredSession);

	ASSERT_THAT(target->getSessionCount(), testing::Eq(2));

	// when
	auto obtained = target->getAllNotConfiguredSessions();

	// then
	ASSERT_THAT(obtained.size(), testing::Eq(1));
	ASSERT_THAT(*obtained.begin(), testing::Eq(relevantSession));
}

TEST_F(BeaconSendingContextTest, onDefaultGetAllOpenAndConfiguredSessionsIsEmpty)
{
	// given
	auto target = createBeaconSendingContext()->build();

	// when
	auto obtained = target->getAllOpenAndConfiguredSessions();

	// then
	ASSERT_THAT(obtained.size(), testing::Eq(0));
}

TEST_F(BeaconSendingContextTest, getAllOpenAndConfiguredSessionsReturnsOnlyConfiguredNotFinsihedSessions)
{
	// given
	auto relevantSession = MockSessionInternals::createNice();
	ON_CALL(*relevantSession, isConfiguredAndOpen()).WillByDefault(testing::Return(true));

	auto ignoredSession = MockSessionInternals::createNice();
	ON_CALL(*ignoredSession, isConfiguredAndOpen()).WillByDefault(testing::Return(false));

	auto target = createBeaconSendingContext()->build();
	target->addSession(relevantSession);
	target->addSession(ignoredSession);

	ASSERT_THAT(target->getSessionCount(), testing::Eq(2));

	// when
	auto obtained = target->getAllOpenAndConfiguredSessions();

	// then
	ASSERT_THAT(obtained.size(), testing::Eq(1));
	ASSERT_THAT(*obtained.begin(), testing::Eq(relevantSession));
}

TEST_F(BeaconSendingContextTest, onDefaultGetAllFinishedAndConfiguredSessionsIsEmpty)
{
	// given
	auto target = createBeaconSendingContext()->build();

	// when
	auto obtained = target->getAllFinishedAndConfiguredSessions();

	// then
	ASSERT_THAT(obtained.size(), testing::Eq(0));
}

TEST_F(BeaconSendingContextTest, getAllFinishedAndConfiguredSessionsReturnsOnlyConfiguredAndFinsihedSessions)
{
	// given
	auto relevantSession = MockSessionInternals::createNice();
	ON_CALL(*relevantSession, isConfiguredAndFinished()).WillByDefault(testing::Return(true));

	auto ignoredSession = MockSessionInternals::createNice();
	ON_CALL(*ignoredSession, isConfiguredAndFinished()).WillByDefault(testing::Return(false));

	auto target = createBeaconSendingContext()->build();
	target->addSession(relevantSession);
	target->addSession(ignoredSession);

	ASSERT_THAT(target->getSessionCount(), testing::Eq(2));

	// when
	auto obtained = target->getAllFinishedAndConfiguredSessions();

	// then
	ASSERT_THAT(obtained.size(), testing::Eq(1));
	ASSERT_THAT(*obtained.begin(), testing::Eq(relevantSession));
}

TEST_F(BeaconSendingContextTest, getCurrentServerIdReturnsServerIdOfHttpClientConfig)
{
	// given
	const int32_t serverId = 37;
	ON_CALL(*mockHttpClientConfig, getServerID()).WillByDefault(testing::Return(serverId));

	auto target = createBeaconSendingContext()->build();

	// when
	auto obtained = target->getCurrentServerID();

	// then
	ASSERT_THAT(obtained, testing::Eq(serverId));
}

