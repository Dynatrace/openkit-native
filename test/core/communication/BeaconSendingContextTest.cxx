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
#include "builder/TestBeaconSendingContextBuilder.h"
#include "mock/MockIBeaconSendingState.h"
#include "../configuration/mock/MockIBeaconCacheConfiguration.h"
#include "../configuration/mock/MockIBeaconConfiguration.h"
#include "../objects/mock/MockSessionInternals.h"
#include "../../api/mock/MockILogger.h"
#include "../../api/mock/MockISslTrustManager.h"
#include "../../protocol/mock/MockIHTTPClient.h"
#include "../../protocol/mock/MockIStatusResponse.h"
#include "../../providers/mock/MockIHTTPClientProvider.h"
#include "../../providers/mock/MockISessionIDProvider.h"
#include "../../providers/mock/MockITimingProvider.h"

#include "core/communication/BeaconSendingContext.h"
#include "core/communication/IBeaconSendingState.h"
#include "core/configuration/Configuration.h"
#include "core/configuration/Device.h"
#include "core/configuration/OpenKitType.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace test;

using BeaconSendingContext_t = core::communication::BeaconSendingContext;
using BeaconSendingContextBuilder_sp = std::shared_ptr<TestBeaconSendingContextBuilder>;
using Configuration_t = core::configuration::Configuration;
using Configuration_sp = std::shared_ptr<Configuration_t>;
using Device_t = core::configuration::Device;
using IBeaconSendingState_t = core::communication::IBeaconSendingState;
using IBeaconSendingState_up = std::unique_ptr<IBeaconSendingState_t>;
using MockNiceILogger_sp = std::shared_ptr<testing::NiceMock<MockILogger>>;
using MockNiceIHTTPClientProvider_sp = std::shared_ptr<testing::NiceMock<MockIHTTPClientProvider>>;
using MockNiceITimingProvider_sp = std::shared_ptr<testing::NiceMock<MockITimingProvider>>;
using MockNiceSession_t = testing::NiceMock<MockSessionInternals>;
using MockStrictSession_t = testing::StrictMock<MockSessionInternals>;
using MockStrictIBeaconSendingState_sp = std::shared_ptr<testing::StrictMock<MockIBeaconSendingState>>;
using OpenKitType_t = core::configuration::OpenKitType;

class BeaconSendingContextTest : public testing::Test
{
protected:

	MockNiceILogger_sp mockLogger;
	Configuration_sp mockConfiguration;
	MockNiceIHTTPClientProvider_sp mockHTTPClientProvider;
	MockNiceITimingProvider_sp mockTimingProvider;


	void SetUp()
	{
		mockLogger = MockILogger::createNice();
		mockConfiguration = std::make_shared<Configuration_t>(
			std::make_shared<Device_t>("", "", ""),
			OpenKitType_t::Type::DYNATRACE,
			"",
			"",
			"",
			1,
			"1",
			"",
			MockISessionIDProvider::createNice(),
			MockISslTrustManager::createNice(),
			MockIBeaconCacheConfiguration::createNice(),
			MockIBeaconConfiguration::createNice()
		);
		mockHTTPClientProvider = MockIHTTPClientProvider::createNice();
		mockTimingProvider = MockITimingProvider::createNice();
	}

	BeaconSendingContextBuilder_sp createBeaconSendingContext()
	{
		auto builder = std::make_shared<TestBeaconSendingContextBuilder>(mockConfiguration);
		builder->with(mockLogger)
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

TEST_F(BeaconSendingContextTest, isCaptureOnReturnsValueFromConfiguration)
{
	// given
	auto target = createBeaconSendingContext()->build();

	// when capturing is enabled
	mockConfiguration->enableCapture();

	// then
	ASSERT_TRUE(target->isCaptureOn());

	// and when capturing is disabled
	mockConfiguration->disableCapture();

	// then
	ASSERT_FALSE(target->isCaptureOn());
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

TEST_F(BeaconSendingContextTest, getSendIntervalRetrievesItFromConfiguration)
{
	// given
	auto target = createBeaconSendingContext()->build();
	mockConfiguration->setSendInterval(1234L);

	// when
	int64_t obtained = target->getSendInterval();

	// then
	ASSERT_EQ(obtained, 1234L);
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
	ASSERT_TRUE(target->getAllNewSessions().empty());
	ASSERT_TRUE(target->getAllOpenAndConfiguredSessions().empty());
	ASSERT_TRUE(target->getAllFinishedAndConfiguredSessions().empty());
}

TEST_F(BeaconSendingContextTest, startingASessionAddsTheSessionToOpenSessions)
{
	// given
	auto target = createBeaconSendingContext()->build();
	auto mockSessionOne = MockSessionInternals::createNice();
	auto mockSessionTwo = MockSessionInternals::createNice();

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
	auto target = createBeaconSendingContext()->build();
	auto mockSessionOne = MockSessionInternals::createNice();
	auto mockSessionTwo = MockSessionInternals::createNice();

	target->startSession(mockSessionOne);
	target->startSession(mockSessionTwo);

	auto sessionWrapper1 = target->findSessionWrapper(mockSessionOne);
	sessionWrapper1->updateBeaconConfiguration(MockIBeaconConfiguration::createNice());

	auto sessionWrapper2 = target->findSessionWrapper(mockSessionTwo);
	sessionWrapper2->updateBeaconConfiguration(MockIBeaconConfiguration::createNice());

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
	auto target = createBeaconSendingContext()->build();
	auto mockSession = MockSessionInternals::createNice();

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
	auto target = createBeaconSendingContext()->build();

	// when
	auto finishedSessions = target->getAllFinishedAndConfiguredSessions();

	// then
	ASSERT_EQ(finishedSessions.size(), 0);

}

TEST_F(BeaconSendingContextTest, handleStatusResponseWhenCapturingIsEnabled)
{
	// given
	mockConfiguration->enableCapture();
	auto target = createBeaconSendingContext()->build();
	auto mockSessionOne = MockSessionInternals::createStrict();
	auto mockSessionTwo = MockSessionInternals::createStrict();

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
	auto target = createBeaconSendingContext()->build();
	auto mockSessionOne = MockSessionInternals::createStrict();
	auto mockSessionTwo = MockSessionInternals::createStrict();
	auto mockSessionThree = MockSessionInternals::createStrict();
	auto mockSessionFour = MockSessionInternals::createStrict();

	EXPECT_CALL(*mockSessionOne, setBeaconConfiguration(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockSessionTwo, setBeaconConfiguration(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockSessionThree, setBeaconConfiguration(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mockSessionFour, setBeaconConfiguration(testing::_))
		.Times(testing::Exactly(1));

	target->startSession(mockSessionOne);
	target->findSessionWrapper(mockSessionOne)->updateBeaconConfiguration(MockIBeaconConfiguration::createNice());
	target->finishSession(mockSessionOne);
	target->startSession(mockSessionTwo);
	target->findSessionWrapper(mockSessionTwo)->updateBeaconConfiguration(MockIBeaconConfiguration::createNice());
	target->startSession(mockSessionThree);
	target->findSessionWrapper(mockSessionThree)->updateBeaconConfiguration(MockIBeaconConfiguration::createNice());
	target->startSession(mockSessionFour);
	target->findSessionWrapper(mockSessionFour)->updateBeaconConfiguration(MockIBeaconConfiguration::createNice());
	target->finishSession(mockSessionFour);

	auto mockStatusResponse = MockIStatusResponse::createNice();

	mockConfiguration->disableCapture();

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
	auto target = createBeaconSendingContext()->build();

	auto mockSessionOne = MockSessionInternals::createNice();
	auto mockSessionTwo = MockSessionInternals::createNice();

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
	auto target = createBeaconSendingContext()->build();

	auto mockSessionOne = MockSessionInternals::createNice();
	auto mockSessionTwo = MockSessionInternals::createNice();

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
	auto target = createBeaconSendingContext()->build();

	auto mockSessionOne = MockSessionInternals::createNice();
	auto mockSessionTwo = MockSessionInternals::createNice();

	// when
	target->startSession(mockSessionOne);
	target->startSession(mockSessionTwo);

	// then
	ASSERT_EQ(target->getAllNewSessions().size(), 2);
	ASSERT_EQ(target->getAllOpenAndConfiguredSessions().size(), 0);
	ASSERT_EQ(target->getAllFinishedAndConfiguredSessions().size(), 0);

	// when
	target->findSessionWrapper(mockSessionOne)->updateBeaconConfiguration(MockIBeaconConfiguration::createNice());

	// then
	ASSERT_EQ(target->getAllNewSessions().size(), 1);
	ASSERT_EQ(target->getAllOpenAndConfiguredSessions().size(), 1);
	ASSERT_EQ(target->getAllFinishedAndConfiguredSessions().size(), 0);

	// when
	target->findSessionWrapper(mockSessionTwo)->updateBeaconConfiguration(MockIBeaconConfiguration::createNice());

	// then
	ASSERT_EQ(target->getAllNewSessions().size(), 0);
	ASSERT_EQ(target->getAllOpenAndConfiguredSessions().size(), 2);
	ASSERT_EQ(target->getAllFinishedAndConfiguredSessions().size(), 0);
}

TEST_F(BeaconSendingContextTest, afterAFinishedSessionHasBeenConfiguredItsFinishedAndConfigured)
{
	// given
	auto target = createBeaconSendingContext()->build();

	auto mockSessionOne = MockSessionInternals::createNice();
	auto mockSessionTwo = MockSessionInternals::createNice();

	// when
	target->startSession(mockSessionOne);
	target->startSession(mockSessionTwo);

	// then
	ASSERT_EQ(target->getAllNewSessions().size(), 2);
	ASSERT_EQ(target->getAllOpenAndConfiguredSessions().size(), 0);
	ASSERT_EQ(target->getAllFinishedAndConfiguredSessions().size(), 0);

	// when
	target->findSessionWrapper(mockSessionOne)->updateBeaconConfiguration(MockIBeaconConfiguration::createNice());
	target->finishSession(mockSessionOne);

	// then
	ASSERT_EQ(target->getAllNewSessions().size(), 1);
	ASSERT_EQ(target->getAllOpenAndConfiguredSessions().size(), 0);
	ASSERT_EQ(target->getAllFinishedAndConfiguredSessions().size(), 1);

	// when
	target->findSessionWrapper(mockSessionTwo)->updateBeaconConfiguration(MockIBeaconConfiguration::createNice());
	target->finishSession(mockSessionTwo);

	// then
	ASSERT_EQ(target->getAllNewSessions().size(), 0);
	ASSERT_EQ(target->getAllOpenAndConfiguredSessions().size(), 0);
	ASSERT_EQ(target->getAllFinishedAndConfiguredSessions().size(), 2);
}
