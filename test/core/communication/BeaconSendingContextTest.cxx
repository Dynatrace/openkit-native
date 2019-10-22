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

#include "CustomMatchers.h"
#include "../../api/mock/MockILogger.h"
#include "../../api/mock/MockISslTrustManager.h"
#include "../../core/communication/MockAbstractBeaconSendingState.h"
#include "../../core/objects/MockSession.h"
#include "../../protocol/mock/MockIHTTPClient.h"
#include "../../protocol/mock/MockIStatusResponse.h"
#include "../../providers/mock/MockIHTTPClientProvider.h"
#include "../../providers/mock/MockISessionIDProvider.h"
#include "../../providers/mock/MockITimingProvider.h"

#include "core/UTF8String.h"
#include "core/communication/BeaconSendingContext.h"
#include "core/configuration/BeaconConfiguration.h"
#include "core/configuration/BeaconCacheConfiguration.h"
#include "core/configuration/Configuration.h"
#include "core/configuration/Device.h"
#include "core/configuration/OpenKitType.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace test;

using BeaconConfiguration_t = core::configuration::BeaconConfiguration;
using BeaconCacheConfiguration_t = core::configuration::BeaconCacheConfiguration;
using BeaconSendingContext_t = core::communication::BeaconSendingContext;
using Configuration_t = core::configuration::Configuration;
using Configuration_sp = std::shared_ptr<Configuration_t>;
using Device_t = core::configuration::Device;
using MockNiceILogger_sp = std::shared_ptr<testing::NiceMock<MockILogger>>;
using MockNiceIHTTPClientProvider_sp = std::shared_ptr<testing::NiceMock<MockIHTTPClientProvider>>;
using MockNiceITimingProvider_sp = std::shared_ptr<testing::NiceMock<MockITimingProvider>>;
using MockNiceSession_t = testing::NiceMock<MockSession>;
using MockStrictSession_t = testing::StrictMock<MockSession>;
using MockStrictAbstractBeaconSendingState_t = testing::StrictMock<MockAbstractBeaconSendingState>;
using MockStrictAbstractBeaconSendingState_sp = std::shared_ptr<MockStrictAbstractBeaconSendingState_t>;
using OpenKitType_t = core::configuration::OpenKitType;
using Utf8String_t = core::UTF8String;

class BeaconSendingContextTest : public testing::Test
{
protected:

	MockNiceILogger_sp mockLogger;
	Configuration_sp mConfiguration;
	MockNiceIHTTPClientProvider_sp mMockHttpClientProvider;
	MockNiceITimingProvider_sp mMockTimingProvider;
	MockStrictAbstractBeaconSendingState_sp mMockState;


	void SetUp()
	{
		mockLogger = MockILogger::createNice();
		auto beaconCacheConfig = std::make_shared<BeaconCacheConfiguration_t>(-1, -1, -1);
		auto beaconConfig = std::make_shared<BeaconConfiguration_t>();
		mConfiguration = std::make_shared<Configuration_t>
		(
			std::make_shared<Device_t>("", "", ""),
			OpenKitType_t::Type::DYNATRACE,
			Utf8String_t(""),
			Utf8String_t(""),
			Utf8String_t(""),
			1,
			"1",
			Utf8String_t(""),
			MockISessionIDProvider::createNice(),
			MockISslTrustManager::createNice(),
			beaconCacheConfig,
			beaconConfig
		);
		mMockHttpClientProvider = MockIHTTPClientProvider::createNice();
		mMockTimingProvider = MockITimingProvider::createNice();
		mMockState = std::make_shared<MockStrictAbstractBeaconSendingState_t>();
	}

	void TearDown()
	{
		mConfiguration = nullptr;
		mMockHttpClientProvider = nullptr;
		mMockTimingProvider = nullptr;
		mMockState = nullptr;
	}
};

TEST_F(BeaconSendingContextTest, currentStateIsInitializedAccordingly)
{
	// given
	auto target = std::make_shared<BeaconSendingContext_t>(mockLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration);

	// then
	ASSERT_NE(target->getCurrentState(), nullptr);
	EXPECT_THAT(target->getCurrentState(), IsABeaconSendingInitState());
}

TEST_F(BeaconSendingContextTest, setCurrentStateChangesState)
{
	// given
	auto target = std::make_shared<BeaconSendingContext_t>(mockLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration);

	// when
	target->setNextState(mMockState);

	// then
	ASSERT_NE(target->getCurrentState(), nullptr);
	ASSERT_EQ(target->getNextState(), mMockState);
}

TEST_F(BeaconSendingContextTest, executeCurrentStateCallsExecuteOnCurrentState)
{
	// given
	auto initMockState = new testing::StrictMock<MockAbstractBeaconSendingState>();
	auto target = std::make_shared<BeaconSendingContext_t>(mockLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration, std::unique_ptr<testing::StrictMock<test::MockAbstractBeaconSendingState>>(initMockState));

	// then
	EXPECT_CALL(*initMockState, execute(testing::_))
		.Times(testing::Exactly(1));

	// when
	target->executeCurrentState();
}

TEST_F(BeaconSendingContextTest, initCompleteSuccessAndWait)
{
	// given
	auto target = std::make_shared<BeaconSendingContext_t>(mockLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration);
	target->setInitCompleted(true);

	// when
	bool obtained = target->waitForInit();

	// then
	ASSERT_TRUE(obtained);
}

TEST_F(BeaconSendingContextTest, requestShutdown)
{
	// given
	auto target = std::make_shared<BeaconSendingContext_t>(mockLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration);

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
	auto target = std::make_shared<BeaconSendingContext_t>(mockLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration);
	target->setInitCompleted(false);

	// when
	bool obtained = target->waitForInit();

	// then
	ASSERT_FALSE(obtained);
}

TEST_F(BeaconSendingContextTest, waitForInitCompleteTimeout)
{
	// given
	auto target = std::make_shared<BeaconSendingContext_t>(mockLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration);

	// when init complete was never set and timeout will be reached
	bool obtained = target->waitForInit(1);

	// then
	ASSERT_FALSE(obtained);
}

TEST_F(BeaconSendingContextTest, waitForInitCompleteWhenInitCompletedSuccessfully)
{
	// given
	auto target = std::make_shared<BeaconSendingContext_t>(mockLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration);
	target->setInitCompleted(true);

	// when init complete was never set and timeout will be reached
	bool obtained = target->waitForInit(1);

	// then
	ASSERT_TRUE(obtained);
}

TEST_F(BeaconSendingContextTest, waitForInitCompleteWhenInitCompletedNotSuccessfully)
{
	// given
	auto target = std::make_shared<BeaconSendingContext_t>(mockLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration);
	target->setInitCompleted(false);

	// when init complete was never set and timeout will be reached
	bool obtained = target->waitForInit(1);

	// then
	ASSERT_FALSE(obtained);
}

TEST_F(BeaconSendingContextTest, aDefaultConstructedContextIsNotInitialized)
{
	// given
	auto target = std::make_shared<BeaconSendingContext_t>(mockLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration);

	// that
	ASSERT_FALSE(target->isInitialized());
}

TEST_F(BeaconSendingContextTest, successfullyInitializedContextIsInitialized)
{
	// given
	auto target = std::make_shared<BeaconSendingContext_t>(mockLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration);

	// when initialized
	target->setInitCompleted(true);

	// then
	ASSERT_TRUE(target->isInitialized());
}

TEST_F(BeaconSendingContextTest, isInTerminalStateChecksCurrentState)
{
	// given
	auto target = std::make_shared<BeaconSendingContext_t>(mockLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration);
	auto nonTerminalState = std::make_shared<MockStrictAbstractBeaconSendingState_t>();
	ON_CALL(*nonTerminalState, isTerminalState())
		.WillByDefault(testing::Return(false));
	auto terminalState = std::make_shared<MockStrictAbstractBeaconSendingState_t>();
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
	auto target = std::make_shared<BeaconSendingContext_t>(mockLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration);

	// when capturing is enabled
	mConfiguration->enableCapture();

	// then
	ASSERT_TRUE(target->isCaptureOn());

	// and when capturing is disabled
	mConfiguration->disableCapture();

	// then
	ASSERT_FALSE(target->isCaptureOn());
}

TEST_F(BeaconSendingContextTest, setAndGetLastOpenSessionBeaconSendTime)
{
	// given
	auto target = std::make_shared<BeaconSendingContext_t>(mockLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration);

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
	auto target = std::make_shared<BeaconSendingContext_t>(mockLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration);

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
	auto target = std::make_shared<BeaconSendingContext_t>(mockLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration);
	mConfiguration->setSendInterval(1234L);

	// when
	int64_t obtained = target->getSendInterval();

	// then
	ASSERT_EQ(obtained, 1234L);
}

TEST_F(BeaconSendingContextTest, testGetHTTPClient)
{
	// given
	auto target = std::make_shared<BeaconSendingContext_t>(mockLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration);

	// when
	auto obtained = target->getHTTPClientProvider();

	// then
	ASSERT_EQ(obtained, mMockHttpClientProvider);
}

TEST_F(BeaconSendingContextTest, getHTTPClientProvider)
{
	// given
	auto mockClient = MockIHTTPClient::createStrict();
	auto httpClientProvider = MockIHTTPClientProvider::createStrict();
	ON_CALL(*mMockHttpClientProvider, createClient(testing::_, testing::_))
		.WillByDefault(testing::Return(mockClient));
	auto target = std::make_shared<BeaconSendingContext_t>(mockLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration);

	// when
	auto obtained = target->getHTTPClient();

	// then
	ASSERT_NE(obtained, nullptr);
	ASSERT_EQ(obtained, mockClient);

}

TEST_F(BeaconSendingContextTest, getCurrentTimestamp)
{
	// given
	auto timingProvider = MockITimingProvider::createStrict();
	ON_CALL(*timingProvider, provideTimestampInMilliseconds())
		.WillByDefault(testing::Return(1234567890L));
	auto target = std::make_shared<BeaconSendingContext_t>(mockLogger, mMockHttpClientProvider, timingProvider, mConfiguration);

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
	auto timingProvider = MockITimingProvider::createStrict();
	ON_CALL(*timingProvider, provideTimestampInMilliseconds())
		.WillByDefault(testing::Return(1234567890L));
	auto target = std::make_shared<BeaconSendingContext_t>(mockLogger, mMockHttpClientProvider, timingProvider, mConfiguration);

	// then
	EXPECT_CALL(*timingProvider, sleep(BeaconSendingContext_t::DEFAULT_SLEEP_TIME_MILLISECONDS.count()))
		.Times(testing::Exactly(0));

	// when
	auto start = std::chrono::system_clock::now();
	target->sleep();
	auto duration = std::chrono::system_clock::now() - start;

	// then ensure sleep is correct
	ASSERT_GE(duration, BeaconSendingContext_t::DEFAULT_SLEEP_TIME_MILLISECONDS);
}

TEST_F(BeaconSendingContextTest, sleepWithGivenTime)
{
	// given
	auto timingProvider = MockITimingProvider::createStrict();
	ON_CALL(*timingProvider, provideTimestampInMilliseconds())
		.WillByDefault(testing::Return(1234567890L));
	auto target = std::make_shared<BeaconSendingContext_t>(mockLogger, mMockHttpClientProvider, timingProvider, mConfiguration);

	// then
	EXPECT_CALL(*timingProvider, sleep(1234L))
		.Times(testing::Exactly(0));

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
	auto target = std::make_shared<BeaconSendingContext_t>(mockLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration);

	// then
	ASSERT_TRUE(target->getAllNewSessions().empty());
	ASSERT_TRUE(target->getAllOpenAndConfiguredSessions().empty());
	ASSERT_TRUE(target->getAllFinishedAndConfiguredSessions().empty());
}

TEST_F(BeaconSendingContextTest, startingASessionAddsTheSessionToOpenSessions)
{
	// given
	auto target = std::make_shared<BeaconSendingContext_t>(mockLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration);
	auto mockSessionOne = std::make_shared<MockNiceSession_t>(mockLogger);
	auto mockSessionTwo = std::make_shared<MockNiceSession_t>(mockLogger);

	// when starting first session
	target->startSession(mockSessionOne);

	// then
	ASSERT_EQ(target->getAllNewSessions().size(), 1);
	ASSERT_EQ(target->getAllNewSessions()[0]->getWrappedSession(), mockSessionOne);
	ASSERT_TRUE(target->getAllOpenAndConfiguredSessions().empty());
	ASSERT_TRUE(target->getAllFinishedAndConfiguredSessions().empty());

	// when starting second sessions
	target->startSession(mockSessionTwo);

	// then
	ASSERT_EQ(target->getAllNewSessions().size(), 2);
	ASSERT_EQ(target->getAllNewSessions()[0]->getWrappedSession(), mockSessionOne);
	ASSERT_EQ(target->getAllNewSessions()[1]->getWrappedSession(), mockSessionTwo);
	ASSERT_TRUE(target->getAllOpenAndConfiguredSessions ().empty());
	ASSERT_TRUE(target->getAllFinishedAndConfiguredSessions().empty());
}


TEST_F(BeaconSendingContextTest, finishingASessionMovesSessionToFinishedSessions)
{
	// given
	auto target = std::make_shared<BeaconSendingContext_t>(mockLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration);
	auto mockSessionOne = std::make_shared<MockNiceSession_t>(mockLogger);
	auto mockSessionTwo = std::make_shared<MockNiceSession_t>(mockLogger);

	target->startSession(mockSessionOne);
	target->startSession(mockSessionTwo);

	auto sessionWrapper1 = target->findSessionWrapper(mockSessionOne);
	sessionWrapper1->updateBeaconConfiguration(std::make_shared<BeaconConfiguration_t>());

	auto sessionWrapper2 = target->findSessionWrapper(mockSessionTwo);
	sessionWrapper2->updateBeaconConfiguration(std::make_shared<BeaconConfiguration_t>());

	// when finishing the first session
	target->finishSession(mockSessionOne);

	// then
	ASSERT_TRUE(target->getAllNewSessions().empty());
	ASSERT_EQ(target->getAllOpenAndConfiguredSessions().size(), 1);
	ASSERT_EQ(target->getAllOpenAndConfiguredSessions()[0]->getWrappedSession(), mockSessionTwo);
	ASSERT_EQ(target->getAllFinishedAndConfiguredSessions().size(), 1);
	ASSERT_EQ(target->getAllFinishedAndConfiguredSessions()[0]->getWrappedSession(), mockSessionOne);

	// and when finishing the second session
	target->finishSession(mockSessionTwo);

	// then
	ASSERT_TRUE(target->getAllNewSessions().empty());
	ASSERT_TRUE(target->getAllOpenAndConfiguredSessions().empty());
	ASSERT_EQ(target->getAllFinishedAndConfiguredSessions().size(), 2);
	ASSERT_EQ(target->getAllFinishedAndConfiguredSessions()[0]->getWrappedSession(), mockSessionOne);
	ASSERT_EQ(target->getAllFinishedAndConfiguredSessions()[1]->getWrappedSession(), mockSessionTwo);
}

TEST_F(BeaconSendingContextTest, finishingASessionThatHasNotBeenStartedBeforeIsNotAddedToInternalList)
{
	// given
	auto target = std::make_shared<BeaconSendingContext_t>(mockLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration);
	auto mockSession = std::make_shared<MockNiceSession_t>(mockLogger);

	// when the session is not started, but immediately finished
	target->finishSession(mockSession);

	// then
	ASSERT_TRUE(target->getAllNewSessions().empty());
	ASSERT_TRUE(target->getAllOpenAndConfiguredSessions().empty());
	ASSERT_TRUE(target->getAllFinishedAndConfiguredSessions().empty());
}

TEST_F(BeaconSendingContextTest, getNextFinishedSessionReturnsEmptyListIfThereAreNoFinishedSessions)
{
	// given
	auto target = std::make_shared<BeaconSendingContext_t>(mockLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration);

	// when
	auto finishedSessions = target->getAllFinishedAndConfiguredSessions();

	// then
	ASSERT_EQ(finishedSessions.size(), 0);

}

TEST_F(BeaconSendingContextTest, handleStatusResponseWhenCapturingIsEnabled)
{
	// given
	mConfiguration->enableCapture();
	auto target = std::make_shared<BeaconSendingContext_t>(mockLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration);
	auto mockSessionOne = std::make_shared<MockStrictSession_t>(mockLogger);
	auto mockSessionTwo = std::make_shared<MockStrictSession_t>(mockLogger);

	target->startSession(mockSessionOne);
	target->finishSession(mockSessionOne);
	target->startSession(mockSessionTwo);

	auto mockStatusResponse = MockIStatusResponse::createNice();
	ON_CALL(*mockStatusResponse, getResponseCode())
		.WillByDefault(testing::Return(200));
	ON_CALL(*mockStatusResponse, isCapture())
		.WillByDefault(testing::Return(true));

	// when
	target->handleStatusResponse(mockStatusResponse);

	// then
	auto newSessions = target->getAllNewSessions();
	ASSERT_EQ(newSessions.size(), 2);
	ASSERT_EQ(newSessions[0]->getWrappedSession(), mockSessionOne);
	ASSERT_EQ(newSessions[1]->getWrappedSession(), mockSessionTwo);

	auto openedSessions = target->getAllOpenAndConfiguredSessions();
	ASSERT_EQ(openedSessions.size(), 0);

	auto finishedSessions = target->getAllFinishedAndConfiguredSessions();
	ASSERT_EQ(finishedSessions.size(), 0);


}

TEST_F(BeaconSendingContextTest, handleStatusResponseWhenCapturingIsDisabled)
{
	// given
	auto target = std::make_shared<BeaconSendingContext_t>(mockLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration);
	auto mockSessionOne = std::make_shared<MockStrictSession_t>(mockLogger);
	auto mockSessionTwo = std::make_shared<MockStrictSession_t>(mockLogger);
	auto mockSessionThree = std::make_shared<MockStrictSession_t>(mockLogger);
	auto mockSessionFour = std::make_shared<MockStrictSession_t>(mockLogger);

	EXPECT_CALL(*mockSessionOne, setBeaconConfiguration(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockSessionTwo, setBeaconConfiguration(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockSessionThree, setBeaconConfiguration(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockSessionFour, setBeaconConfiguration(testing::_))
		.Times(testing::Exactly(1));

	target->startSession(mockSessionOne);
	target->findSessionWrapper(mockSessionOne)->updateBeaconConfiguration(std::make_shared<BeaconConfiguration_t>());
	target->finishSession(mockSessionOne);
	target->startSession(mockSessionTwo);
	target->findSessionWrapper(mockSessionTwo)->updateBeaconConfiguration(std::make_shared<BeaconConfiguration_t>());
	target->startSession(mockSessionThree);
	target->findSessionWrapper(mockSessionThree)->updateBeaconConfiguration(std::make_shared<BeaconConfiguration_t>());
	target->startSession(mockSessionFour);
	target->findSessionWrapper(mockSessionFour)->updateBeaconConfiguration(std::make_shared<BeaconConfiguration_t>());
	target->finishSession(mockSessionFour);

	auto mockStatusResponse = MockIStatusResponse::createNice();

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

	// then
	auto newSessions = target->getAllNewSessions();
	ASSERT_EQ(newSessions.size(), 0);

	auto openSessions = target->getAllOpenAndConfiguredSessions();
	ASSERT_EQ(openSessions.size(), 2);

	auto finishedSessions = target->getAllFinishedAndConfiguredSessions();
	ASSERT_EQ(finishedSessions.size(), 2);

	// when
	target->handleStatusResponse(mockStatusResponse);

	// then
	newSessions = target->getAllNewSessions();
	ASSERT_EQ(newSessions.size(), 0);

	openSessions = target->getAllOpenAndConfiguredSessions();
	ASSERT_EQ(openSessions.size(), 2);//open sessions stay open

	finishedSessions = target->getAllFinishedAndConfiguredSessions();
	ASSERT_EQ(finishedSessions.size(), 0);//finished sessions are cleared
}

TEST_F(BeaconSendingContextTest, whenStartingASessionTheSessionIsConsideredAsNew)
{
	// given
	auto target = std::make_shared<BeaconSendingContext_t>(mockLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration);

	auto mockSessionOne = std::make_shared<MockNiceSession_t>(mockLogger);
	auto mockSessionTwo = std::make_shared<MockNiceSession_t>(mockLogger);

	// when
	target->startSession(mockSessionOne);

	// then
	ASSERT_EQ(target->getAllNewSessions().size(), 1);
	ASSERT_EQ(target->getAllOpenAndConfiguredSessions().size(), 0);
	ASSERT_EQ(target->getAllFinishedAndConfiguredSessions().size(), 0);

	// when
	target->startSession(mockSessionTwo);

	// then
	ASSERT_EQ(target->getAllNewSessions().size(), 2);
	ASSERT_EQ(target->getAllOpenAndConfiguredSessions().size(), 0);
	ASSERT_EQ(target->getAllFinishedAndConfiguredSessions().size(), 0);
}

TEST_F(BeaconSendingContextTest, finishingANewSessionStillLeavesItNew)
{
	// given
	auto target = std::make_shared<BeaconSendingContext_t>(mockLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration);

	auto mockSessionOne = std::make_shared<MockNiceSession_t>(mockLogger);
	auto mockSessionTwo = std::make_shared<MockNiceSession_t>(mockLogger);

	// when
	target->startSession(mockSessionOne);
	target->finishSession(mockSessionOne);

	// then
	ASSERT_EQ(target->getAllNewSessions().size(), 1);
	ASSERT_EQ(target->getAllOpenAndConfiguredSessions().size(), 0);
	ASSERT_EQ(target->getAllFinishedAndConfiguredSessions().size(), 0);

	// when
	target->startSession(mockSessionTwo);
	target->finishSession(mockSessionTwo);

	// then
	ASSERT_EQ(target->getAllNewSessions().size(), 2);
	ASSERT_EQ(target->getAllOpenAndConfiguredSessions().size(), 0);
	ASSERT_EQ(target->getAllFinishedAndConfiguredSessions().size(), 0);
}

TEST_F(BeaconSendingContextTest, afterASessionHasBeenConfiguredItsOpenAndConfigured)
{
	// given
	auto target = std::make_shared<BeaconSendingContext_t>(mockLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration);

	auto mockSessionOne = std::make_shared<MockNiceSession_t>(mockLogger);
	auto mockSessionTwo = std::make_shared<MockNiceSession_t>(mockLogger);

	// when
	target->startSession(mockSessionOne);
	target->startSession(mockSessionTwo);

	// then
	ASSERT_EQ(target->getAllNewSessions().size(), 2);
	ASSERT_EQ(target->getAllOpenAndConfiguredSessions().size(), 0);
	ASSERT_EQ(target->getAllFinishedAndConfiguredSessions().size(), 0);

	// when
	target->findSessionWrapper(mockSessionOne)->updateBeaconConfiguration(std::make_shared<BeaconConfiguration_t>());

	// then
	ASSERT_EQ(target->getAllNewSessions().size(), 1);
	ASSERT_EQ(target->getAllOpenAndConfiguredSessions().size(), 1);
	ASSERT_EQ(target->getAllFinishedAndConfiguredSessions().size(), 0);

	// when
	target->findSessionWrapper(mockSessionTwo)->updateBeaconConfiguration(std::make_shared<BeaconConfiguration_t>());

	// then
	ASSERT_EQ(target->getAllNewSessions().size(), 0);
	ASSERT_EQ(target->getAllOpenAndConfiguredSessions().size(), 2);
	ASSERT_EQ(target->getAllFinishedAndConfiguredSessions().size(), 0);
}

TEST_F(BeaconSendingContextTest, afterAFinishedSessionHasBeenConfiguredItsFinishedAndConfigured)
{
	// given
	auto target = std::make_shared<BeaconSendingContext_t>(mockLogger, mMockHttpClientProvider, mMockTimingProvider, mConfiguration);

	auto mockSessionOne = std::make_shared<MockNiceSession_t>(mockLogger);
	auto mockSessionTwo = std::make_shared<MockNiceSession_t>(mockLogger);

	// when
	target->startSession(mockSessionOne);
	target->startSession(mockSessionTwo);

	// then
	ASSERT_EQ(target->getAllNewSessions().size(), 2);
	ASSERT_EQ(target->getAllOpenAndConfiguredSessions().size(), 0);
	ASSERT_EQ(target->getAllFinishedAndConfiguredSessions().size(), 0);

	// when
	target->findSessionWrapper(mockSessionOne)->updateBeaconConfiguration(std::make_shared<BeaconConfiguration_t>());
	target->finishSession(mockSessionOne);

	// then
	ASSERT_EQ(target->getAllNewSessions().size(), 1);
	ASSERT_EQ(target->getAllOpenAndConfiguredSessions().size(), 0);
	ASSERT_EQ(target->getAllFinishedAndConfiguredSessions().size(), 1);

	// when
	target->findSessionWrapper(mockSessionTwo)->updateBeaconConfiguration(std::make_shared<BeaconConfiguration_t>());
	target->finishSession(mockSessionTwo);

	// then
	ASSERT_EQ(target->getAllNewSessions().size(), 0);
	ASSERT_EQ(target->getAllOpenAndConfiguredSessions().size(), 0);
	ASSERT_EQ(target->getAllFinishedAndConfiguredSessions().size(), 2);
}
