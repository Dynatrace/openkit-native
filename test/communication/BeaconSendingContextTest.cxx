/**
* Copyright 2018 Dynatrace LLC
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

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "configuration/Configuration.h"
#include "communication/BeaconSendingContext.h"
#include "providers/IHTTPClientProvider.h"
#include "providers/DefaultHTTPClientProvider.h"
#include "providers/DefaultSessionIDProvider.h"
#include "protocol/ssl/SSLStrictTrustManager.h"
#include "core/util/DefaultLogger.h"

#include "../protocol/MockHTTPClient.h"
#include "../protocol/MockStatusResponse.h"
#include "../communication/MockAbstractBeaconSendingState.h"
#include "../providers/MockTimingProvider.h"
#include "../providers/MockHTTPClientProvider.h"
#include "../communication/CustomMatchers.h"
#include "../core/MockSession.h"

class BeaconSendingContextTest : public testing::Test
{
public:

	BeaconSendingContextTest()
		: mLogger(nullptr)
		, mConfiguration(nullptr)
		, mMockHttpClientProvider(nullptr)
		, mMockTimingProvider(nullptr)
		, mMockState(nullptr)
	{
	}

	void SetUp()
	{
		mLogger = std::shared_ptr<api::ILogger>(new core::util::DefaultLogger(devNull, true));


		beaconCacheConfiguration = std::make_shared<configuration::BeaconCacheConfiguration>(-1, -1, -1);
		mConfiguration = std::shared_ptr<configuration::Configuration>(new configuration::Configuration(std::shared_ptr<configuration::Device>(new configuration::Device("", "", "")),
			configuration::OpenKitType::DYNATRACE, core::UTF8String(""), core::UTF8String(""), core::UTF8String(""), 1, core::UTF8String(""),
			std::make_shared<providers::DefaultSessionIDProvider>(),
			std::make_shared<protocol::SSLStrictTrustManager>(),
			beaconCacheConfiguration));
		mMockHttpClientProvider = std::shared_ptr<testing::NiceMock<test::MockHTTPClientProvider>>(new testing::NiceMock<test::MockHTTPClientProvider>());
		mMockTimingProvider = std::shared_ptr<testing::NiceMock<test::MockTimingProvider>>(new testing::NiceMock<test::MockTimingProvider>());
		mMockState = std::shared_ptr<testing::StrictMock<test::MockAbstractBeaconSendingState>>(new testing::StrictMock<test::MockAbstractBeaconSendingState>());
	}

	void TearDown()
	{
		mLogger = nullptr;
		mConfiguration = nullptr;
		mMockHttpClientProvider = nullptr;
		mMockTimingProvider = nullptr;
		mMockState = nullptr;
	}

	std::ostringstream devNull;
	std::shared_ptr<api::ILogger> mLogger;
	std::shared_ptr<configuration::BeaconCacheConfiguration> beaconCacheConfiguration;
	std::shared_ptr<configuration::Configuration> mConfiguration;
	std::shared_ptr<testing::NiceMock<test::MockHTTPClientProvider>> mMockHttpClientProvider;
	std::shared_ptr<testing::NiceMock<test::MockTimingProvider>> mMockTimingProvider;
	std::shared_ptr<testing::StrictMock<test::MockAbstractBeaconSendingState>> mMockState;
};

TEST_F(BeaconSendingContextTest, currentStateIsInitializedAccordingly)
{
	// given
	auto target = std::shared_ptr<BeaconSendingContext>(new BeaconSendingContext(mLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration));

	// then
	ASSERT_NE(target->getCurrentState(), nullptr);
	EXPECT_THAT(target->getCurrentState(), IsABeaconSendingInitState());
}

TEST_F(BeaconSendingContextTest, setCurrentStateChangesState)
{
	// given
	auto target = std::shared_ptr<BeaconSendingContext>(new BeaconSendingContext(mLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration));

	// when
	target->setNextState(mMockState);

	// then
	ASSERT_NE(target->getCurrentState(), nullptr);
	ASSERT_EQ(target->getNextState(), mMockState);
}

TEST_F(BeaconSendingContextTest, executeCurrentStateCallsExecuteOnCurrentState)
{
	// given
	auto initMockState = new testing::StrictMock<test::MockAbstractBeaconSendingState>();
	auto target = std::shared_ptr<BeaconSendingContext>(new BeaconSendingContext(mLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration, std::unique_ptr<testing::StrictMock<test::MockAbstractBeaconSendingState>>(initMockState)));

	// then
	EXPECT_CALL(*initMockState, execute(testing::_))
		.Times(testing::Exactly(1));

	// when
	target->executeCurrentState();
}

TEST_F(BeaconSendingContextTest, initCompleteSuccessAndWait)
{
	// given
	auto target = std::shared_ptr<BeaconSendingContext>(new BeaconSendingContext(mLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration));
	target->setInitCompleted(true);

	// when
	bool obtained = target->waitForInit();

	// then
	ASSERT_TRUE(obtained);
}

TEST_F(BeaconSendingContextTest, requestShutdown)
{
	// given
	auto target = std::shared_ptr<BeaconSendingContext>(new BeaconSendingContext(mLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration));

	bool obtained = target->isShutdownRequested();
	ASSERT_FALSE(obtained);

	// when
	target->requestShutdown();

	// then
	obtained = target->isShutdownRequested();
	ASSERT_TRUE(obtained);
}

TEST_F(BeaconSendingContextTest, initCompleteFailureAndWait)
{
	// given
	auto target = std::shared_ptr<BeaconSendingContext>(new BeaconSendingContext(mLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration));
	target->setInitCompleted(false);

	// when
	bool obtained = target->waitForInit();

	// then
	ASSERT_FALSE(obtained);
}

TEST_F(BeaconSendingContextTest, waitForInitCompleteTimeout)
{
	// given
	auto target = std::shared_ptr<BeaconSendingContext>(new BeaconSendingContext(mLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration));
	
	// when init complete was never set and timeout will be reached
	bool obtained = target->waitForInit(1);

	// then
	ASSERT_FALSE(obtained);
}

TEST_F(BeaconSendingContextTest, waitForInitCompleteWhenInitCompletedSuccessfully)
{
	// given
	auto target = std::shared_ptr<BeaconSendingContext>(new BeaconSendingContext(mLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration));
	target->setInitCompleted(true);

	// when init complete was never set and timeout will be reached
	bool obtained = target->waitForInit(1);

	// then
	ASSERT_TRUE(obtained);
}

TEST_F(BeaconSendingContextTest, waitForInitCompleteWhenInitCompletedNotSuccessfully)
{
	// given
	auto target = std::shared_ptr<BeaconSendingContext>(new BeaconSendingContext(mLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration));
	target->setInitCompleted(false);

	// when init complete was never set and timeout will be reached
	bool obtained = target->waitForInit(1);

	// then
	ASSERT_FALSE(obtained);
}

TEST_F(BeaconSendingContextTest, aDefaultConstructedContextIsNotInitialized)
{
	// given
	auto target = std::shared_ptr<BeaconSendingContext>(new BeaconSendingContext(mLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration));

	// that
	ASSERT_FALSE(target->isInitialized());
}

TEST_F(BeaconSendingContextTest, successfullyInitializedContextIsInitialized)
{
	// given
	auto target = std::shared_ptr<BeaconSendingContext>(new BeaconSendingContext(mLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration));

	// when initialized
	target->setInitCompleted(true);

	// then
	ASSERT_TRUE(target->isInitialized());
}

TEST_F(BeaconSendingContextTest, isInTerminalStateChecksCurrentState)
{
	// given
	auto target = std::shared_ptr<BeaconSendingContext>(new BeaconSendingContext(mLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration));
	auto nonTerminalState = std::shared_ptr<testing::StrictMock<test::MockAbstractBeaconSendingState>>(new testing::StrictMock<test::MockAbstractBeaconSendingState>());
	ON_CALL(*nonTerminalState, isTerminalState())
		.WillByDefault(testing::Return(false));
	auto terminalState = std::shared_ptr<testing::StrictMock<test::MockAbstractBeaconSendingState>>(new testing::StrictMock<test::MockAbstractBeaconSendingState>());
	ON_CALL(*terminalState, isTerminalState())
		.WillByDefault(testing::Return(true));

	// when non-terminal state is current state
	target->setNextState(nonTerminalState);

	// verify interactions with mock
	EXPECT_CALL(*nonTerminalState, isTerminalState())
		.Times(testing::Exactly(1));

	// then
	auto nextState = target->getNextState();
	ASSERT_FALSE(nextState->isTerminalState());

	// and when terminal state is current state
	target->setNextState(terminalState);

	// verify interactions with mock
	EXPECT_CALL(*terminalState, isTerminalState())
		.Times(testing::Exactly(1));

	// then
	nextState = target->getNextState();
	ASSERT_TRUE(nextState->isTerminalState());
}

TEST_F(BeaconSendingContextTest, isCaptureOnReturnsValueFromConfiguration)
{
	// given
	auto target = std::shared_ptr<BeaconSendingContext>(new BeaconSendingContext(mLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration));

	// when capturing is enabled
	mConfiguration->enableCapture();
	
	// then
	ASSERT_TRUE(target->isCaptureOn());

	// and when capturing is disabled
	mConfiguration->disableCapture();
	
	// then
	ASSERT_FALSE(target->isCaptureOn());
}

TEST_F(BeaconSendingContextTest, timeSyncIsNotSupportedIfDisabled)
{
	// given
	auto target = std::shared_ptr<BeaconSendingContext>(new BeaconSendingContext(mLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration));

	// when
	target->disableTimeSyncSupport();

	// then
	ASSERT_FALSE(target->isTimeSyncSupported());
}

TEST_F(BeaconSendingContextTest, setAndGetLastOpenSessionBeaconSendTime)
{
	// given
	auto target = std::shared_ptr<BeaconSendingContext>(new BeaconSendingContext(mLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration));

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
	auto target = std::shared_ptr<BeaconSendingContext>(new BeaconSendingContext(mLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration));

	// when
	target->setLastStatusCheckTime(1234L);

	// then
	ASSERT_EQ(target->getLastStatusCheckTime(), 1234L);

	// and when
	target->setLastStatusCheckTime(5678L);

	// then
	ASSERT_EQ(target->getLastStatusCheckTime(), 5678L);
}

TEST_F(BeaconSendingContextTest, getSendIntervalRetrievesItFromConfiguration)
{
	// given
	auto target = std::shared_ptr<BeaconSendingContext>(new BeaconSendingContext(mLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration));
	mConfiguration->setSendInterval(1234L);

	// when
	int64_t obtained = target->getSendInterval();

	// then
	ASSERT_EQ(obtained, 1234L);
}

TEST_F(BeaconSendingContextTest, testGetHTTPClient)
{
	// given
	auto target = std::shared_ptr<BeaconSendingContext>(new BeaconSendingContext(mLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration));
	
	// when
	auto obtained = target->getHTTPClientProvider();

	// then
	ASSERT_EQ(obtained, mMockHttpClientProvider);
}

TEST_F(BeaconSendingContextTest, getHTTPClientProvider)
{
	// given
	auto mockClient = std::shared_ptr<testing::StrictMock<test::MockHTTPClient>>(new testing::StrictMock<test::MockHTTPClient>(mConfiguration->getHTTPClientConfiguration()));
	auto httpClientProvider = std::shared_ptr<testing::StrictMock<test::MockHTTPClientProvider>>(new testing::StrictMock<test::MockHTTPClientProvider>());
	ON_CALL(*mMockHttpClientProvider, createClient(testing::_, testing::_))
		.WillByDefault(testing::Return(mockClient));
	auto target = std::shared_ptr<BeaconSendingContext>(new BeaconSendingContext(mLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration));

	// when
	auto obtained = target->getHTTPClient();

	// then
	ASSERT_NE(obtained, nullptr);
	ASSERT_EQ(obtained, mockClient);

}

TEST_F(BeaconSendingContextTest, getCurrentTimestamp)
{
	// given
	auto timingProvider = std::shared_ptr<testing::StrictMock<test::MockTimingProvider>>(new testing::StrictMock<test::MockTimingProvider>());
	ON_CALL(*timingProvider, provideTimestampInMilliseconds())
		.WillByDefault(testing::Return(1234567890L));
	auto target = std::shared_ptr<BeaconSendingContext>(new BeaconSendingContext(mLogger, mMockHttpClientProvider, timingProvider, mConfiguration));

	// then
	EXPECT_CALL(*timingProvider, provideTimestampInMilliseconds())
		.Times(testing::Exactly(1));

	// when
	auto obtained = target->getCurrentTimestamp();

	// then
	ASSERT_EQ(obtained, 1234567890L);
}

TEST_F(BeaconSendingContextTest, sleepDefaultTime)
{
	// given
	auto timingProvider = std::shared_ptr<testing::StrictMock<test::MockTimingProvider>>(new testing::StrictMock<test::MockTimingProvider>());
	ON_CALL(*timingProvider, provideTimestampInMilliseconds())
		.WillByDefault(testing::Return(1234567890L));
	auto target = std::shared_ptr<BeaconSendingContext>(new BeaconSendingContext(mLogger, mMockHttpClientProvider, timingProvider, mConfiguration));

	// then
	EXPECT_CALL(*timingProvider, sleep(BeaconSendingContext::DEFAULT_SLEEP_TIME_MILLISECONDS.count()))
		.Times(testing::Exactly(1));

	// when
	target->sleep();
}

TEST_F(BeaconSendingContextTest, sleepWithGivenTime)
{
	// given
	auto timingProvider = std::shared_ptr<testing::StrictMock<test::MockTimingProvider>>(new testing::StrictMock<test::MockTimingProvider>());
	ON_CALL(*timingProvider, provideTimestampInMilliseconds())
		.WillByDefault(testing::Return(1234567890L));
	auto target = std::shared_ptr<BeaconSendingContext>(new BeaconSendingContext(mLogger, mMockHttpClientProvider, timingProvider, mConfiguration));

	// then
	EXPECT_CALL(*timingProvider, sleep(1234L))
		.Times(testing::Exactly(1));

	// when
	target->sleep(1234L);
}

TEST_F(BeaconSendingContextTest, defaultLastTimeSyncTimeIsMinusOne)
{
	// given
	auto target = std::shared_ptr<BeaconSendingContext>(new BeaconSendingContext(mLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration));

	// then
	ASSERT_EQ(target->getLastTimeSyncTime(), -1L);
}

TEST_F(BeaconSendingContextTest, getAndSetLastTimeSyncTime)
{
	// given
	auto target = std::shared_ptr<BeaconSendingContext>(new BeaconSendingContext(mLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration));

	// when setting first value
	target->setLastTimeSyncTime(1234L);

	// then
	ASSERT_EQ(target->getLastTimeSyncTime(), 1234L);

	// and when setting other value
	target->setLastTimeSyncTime(4321L);

	// then
	ASSERT_EQ(target->getLastTimeSyncTime(), 4321L);
}

TEST_F(BeaconSendingContextTest, aDefaultConstructedContextDoesNotStoreAnySessions)
{
	// given
	auto target = std::shared_ptr<BeaconSendingContext>(new BeaconSendingContext(mLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration));

	// then
	ASSERT_TRUE(target->getAllOpenSessions().empty());
	ASSERT_TRUE(target->getAllFinishedSessions().empty());
}

TEST_F(BeaconSendingContextTest, startingASessionAddsTheSessionToOpenSessions)
{
	// given
	auto target = std::shared_ptr<BeaconSendingContext>(new BeaconSendingContext(mLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration));
	auto mockSessionOne = std::shared_ptr<testing::NiceMock<test::MockSession>>(new testing::NiceMock<test::MockSession>());
	auto mockSessionTwo = std::shared_ptr<testing::NiceMock<test::MockSession>>(new testing::NiceMock<test::MockSession>());

	// when starting first session
	target->startSession(mockSessionOne);

	// then
	ASSERT_EQ(target->getAllOpenSessions().size(), 1);
	ASSERT_EQ(target->getAllOpenSessions()[0], mockSessionOne);
	ASSERT_TRUE(target->getAllFinishedSessions().empty());

	// when starting second sessions
	target->startSession(mockSessionTwo);

	// then
	ASSERT_EQ(target->getAllOpenSessions().size(), 2);
	ASSERT_EQ(target->getAllOpenSessions()[0], mockSessionOne);
	ASSERT_EQ(target->getAllOpenSessions()[1], mockSessionTwo);
	ASSERT_TRUE(target->getAllFinishedSessions().empty());
}


TEST_F(BeaconSendingContextTest, finishingASessionMovesSessionToFinishedSessions)
{
	// given
	auto target = std::shared_ptr<BeaconSendingContext>(new BeaconSendingContext(mLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration));
	auto mockSessionOne = std::shared_ptr<testing::NiceMock<test::MockSession>>(new testing::NiceMock<test::MockSession>());
	auto mockSessionTwo = std::shared_ptr<testing::NiceMock<test::MockSession>>(new testing::NiceMock<test::MockSession>());

	target->startSession(mockSessionOne);
	target->startSession(mockSessionTwo);

	// when finishing the first session
	target->finishSession(mockSessionOne);

	// then
	ASSERT_EQ(target->getAllOpenSessions().size(), 1);
	ASSERT_EQ(target->getAllOpenSessions()[0], mockSessionTwo);
	ASSERT_EQ(target->getAllFinishedSessions().size(), 1);
	ASSERT_EQ(target->getAllFinishedSessions()[0], mockSessionOne);
	
	// and when finishing the second session
	target->finishSession(mockSessionTwo);

	// then
	ASSERT_TRUE(target->getAllOpenSessions().empty());
	ASSERT_EQ(target->getAllFinishedSessions().size(), 2);
	ASSERT_EQ(target->getAllFinishedSessions()[0], mockSessionOne);
	ASSERT_EQ(target->getAllFinishedSessions()[1], mockSessionTwo);
}

TEST_F(BeaconSendingContextTest, finishingASessionThatHasNotBeenStartedBeforeIsNotAddedToFinishedSessions)
{
	// given
	auto target = std::shared_ptr<BeaconSendingContext>(new BeaconSendingContext(mLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration));
	auto mockSession = std::shared_ptr<testing::NiceMock<test::MockSession>>(new testing::NiceMock<test::MockSession>());
	
	// when the session is not started, but immediately finished
	target->finishSession(mockSession);

	// then
	ASSERT_TRUE(target->getAllOpenSessions().empty());
	ASSERT_TRUE(target->getAllFinishedSessions().empty());
}

TEST_F(BeaconSendingContextTest, getNextFinishedSessionGetsAndRemovesSession)
{
	// given
	auto target = std::shared_ptr<BeaconSendingContext>(new BeaconSendingContext(mLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration));
	auto mockSessionOne = std::shared_ptr<testing::NiceMock<test::MockSession>>(new testing::NiceMock<test::MockSession>());
	auto mockSessionTwo = std::shared_ptr<testing::NiceMock<test::MockSession>>(new testing::NiceMock<test::MockSession>());

	target->startSession(mockSessionOne);
	target->finishSession(mockSessionOne);
	target->startSession(mockSessionTwo);
	target->finishSession(mockSessionTwo);

	// when retrieving the next finished session
	auto obtained = target->getNextFinishedSession();

	// then
	ASSERT_EQ(obtained, mockSessionOne);
	ASSERT_EQ(target->getAllFinishedSessions().size(), 1);
	ASSERT_EQ(target->getAllFinishedSessions()[0], mockSessionTwo);

	// and when retrieving the next finished Session
	obtained = target->getNextFinishedSession();

	// then
	ASSERT_EQ(obtained, mockSessionTwo);
	ASSERT_TRUE(target->getAllFinishedSessions().empty());
}

TEST_F(BeaconSendingContextTest, getNextFinishedSessionReturnsNullIfThereAreNoFinishedSessions)
{
	// given
	auto target = std::shared_ptr<BeaconSendingContext>(new BeaconSendingContext(mLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration));

	// when, then
	ASSERT_EQ(target->getNextFinishedSession(), nullptr);
}

TEST_F(BeaconSendingContextTest, disableCapture)
{
	// given
	auto target = std::shared_ptr<BeaconSendingContext>(new BeaconSendingContext(mLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration));
	auto mockSessionOne = std::shared_ptr<testing::StrictMock<test::MockSession>>(new testing::StrictMock<test::MockSession>());
	auto mockSessionTwo = std::shared_ptr<testing::StrictMock<test::MockSession>>(new testing::StrictMock<test::MockSession>());
	auto mockSessionThree = std::shared_ptr<testing::StrictMock<test::MockSession>>(new testing::StrictMock<test::MockSession>());
	auto mockSessionFour = std::shared_ptr<testing::StrictMock<test::MockSession>>(new testing::StrictMock<test::MockSession>());

	target->startSession(mockSessionOne);
	target->finishSession(mockSessionOne);
	target->startSession(mockSessionTwo);
	target->startSession(mockSessionThree);
	target->startSession(mockSessionFour);
	target->finishSession(mockSessionFour);

	// verify
	EXPECT_CALL(*mockSessionOne, clearCapturedData())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockSessionTwo, clearCapturedData())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockSessionThree, clearCapturedData())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockSessionFour, clearCapturedData())
		.Times(testing::Exactly(1));

	// when
	target->disableCapture();

	// then
	ASSERT_EQ(target->getAllOpenSessions().size(), 2);
	ASSERT_EQ(target->getAllOpenSessions()[0], mockSessionTwo);
	ASSERT_EQ(target->getAllOpenSessions()[1], mockSessionThree);
	ASSERT_TRUE(target->getAllFinishedSessions().empty());
}

TEST_F(BeaconSendingContextTest, handleStatusResponseWhenCapturingIsEnabled)
{
	// given
	mConfiguration->enableCapture();
	auto target = std::shared_ptr<BeaconSendingContext>(new BeaconSendingContext(mLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration));
	auto mockSessionOne = std::shared_ptr<testing::StrictMock<test::MockSession>>(new testing::StrictMock<test::MockSession>());
	auto mockSessionTwo = std::shared_ptr<testing::StrictMock<test::MockSession>>(new testing::StrictMock<test::MockSession>());
	
	target->startSession(mockSessionOne);
	target->finishSession(mockSessionOne);
	target->startSession(mockSessionTwo);

	auto mockStatusResponse = std::unique_ptr<testing::NiceMock<test::MockStatusResponse>>(new testing::NiceMock<test::MockStatusResponse>());
	ON_CALL(*mockStatusResponse, getResponseCode())
		.WillByDefault(testing::Return(200));
	ON_CALL(*mockStatusResponse, isCapture())
		.WillByDefault(testing::Return(true));

	// when
	target->handleStatusResponse(std::move(mockStatusResponse));

	// then
	ASSERT_EQ(target->getAllOpenSessions().size(), 1);
	ASSERT_EQ(target->getAllOpenSessions()[0], mockSessionTwo);
	ASSERT_EQ(target->getAllFinishedSessions().size(), 1);
	ASSERT_EQ(target->getAllFinishedSessions()[0], mockSessionOne);
}

TEST_F(BeaconSendingContextTest, handleStatusResponseWhenCapturingIsDisabled)
{
	// given
	auto target = std::shared_ptr<BeaconSendingContext>(new BeaconSendingContext(mLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration));
	auto mockSessionOne = std::shared_ptr<testing::StrictMock<test::MockSession>>(new testing::StrictMock<test::MockSession>());
	auto mockSessionTwo = std::shared_ptr<testing::StrictMock<test::MockSession>>(new testing::StrictMock<test::MockSession>());
	auto mockSessionThree = std::shared_ptr<testing::StrictMock<test::MockSession>>(new testing::StrictMock<test::MockSession>());
	auto mockSessionFour = std::shared_ptr<testing::StrictMock<test::MockSession>>(new testing::StrictMock<test::MockSession>());

	target->startSession(mockSessionOne);
	target->finishSession(mockSessionOne);
	target->startSession(mockSessionTwo);
	target->startSession(mockSessionThree);
	target->startSession(mockSessionFour);
	target->finishSession(mockSessionFour);

	auto mockStatusResponse = std::unique_ptr<protocol::StatusResponse>();

	mConfiguration->disableCapture();

	// verify
	EXPECT_CALL(*mockSessionOne, clearCapturedData())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockSessionTwo, clearCapturedData())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockSessionThree, clearCapturedData())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockSessionFour, clearCapturedData())
		.Times(testing::Exactly(1));

	// when
	target->handleStatusResponse(std::move(mockStatusResponse));

	// then
	ASSERT_EQ(target->getAllOpenSessions().size(), 2);
	ASSERT_EQ(target->getAllOpenSessions()[0], mockSessionTwo);
	ASSERT_EQ(target->getAllOpenSessions()[1], mockSessionThree);
	ASSERT_TRUE(target->getAllFinishedSessions().empty());
}

TEST_F(BeaconSendingContextTest, isTimeSyncedReturnsTrueIfSyncWasNeverPerformed)
{
	// given
	auto target = std::shared_ptr<BeaconSendingContext>(new BeaconSendingContext(mLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration));

	// when
	ON_CALL(*mMockTimingProvider, isTimeSyncSupported())
		.WillByDefault(testing::Return(true));

	// then
	ASSERT_FALSE(target->isTimeSynced());
}

TEST_F(BeaconSendingContextTest, isTimeSyncedReturnsTrueIfSyncIsNotSupported)
{
	// given
	auto target = std::shared_ptr<BeaconSendingContext>(new BeaconSendingContext(mLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration));

	// when
	target->disableTimeSyncSupport();

	// then
	ASSERT_TRUE(target->isTimeSynced());
}

TEST_F(BeaconSendingContextTest, timingProviderIsCalledOnTimeSyncInit)
{
	// given
	auto target = std::shared_ptr<BeaconSendingContext>(new BeaconSendingContext(mLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration));

	// then
	EXPECT_CALL(*mMockTimingProvider, initialize(1234L, true));

	// when
	target->initializeTimeSync(1234L, true);
}
