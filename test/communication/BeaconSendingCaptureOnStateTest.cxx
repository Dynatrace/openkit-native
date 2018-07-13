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

#include "communication/BeaconSendingCaptureOnState.h"
#include "communication/AbstractBeaconSendingState.h"

#include "../communication/MockBeaconSendingContext.h"
#include "../communication/CustomMatchers.h"
#include "../protocol/MockStatusResponse.h"
#include "../providers/MockHTTPClientProvider.h"
#include "../core/MockSession.h"

class BeaconSendingCaptureOnStateTest : public testing::Test
{
public:

	BeaconSendingCaptureOnStateTest()
		: mLogger(nullptr)
		, mMockContext(nullptr)
		, mMockSession1Open(nullptr)
		, mMockSession2Open(nullptr)
		, mMockSession3Finished(nullptr)
		, mMockSession4Finished(nullptr)
		, mMockHttpClientProvider(nullptr)
		, mMockHttpClient(nullptr)
		, mHttpClientConfiguration(nullptr)
	{
	}

	void SetUp()
	{
		mLogger = std::shared_ptr<openkit::ILogger>(new core::util::DefaultLogger(devNull, true));
		mMockSession1Open = std::shared_ptr<testing::NiceMock<test::MockSession>>(new testing::NiceMock<test::MockSession>(mLogger));
		mMockSession2Open = std::shared_ptr<testing::NiceMock<test::MockSession>>(new testing::NiceMock<test::MockSession>(mLogger));
		mMockSession3Finished = std::shared_ptr<testing::NiceMock<test::MockSession>>(new testing::NiceMock<test::MockSession>(mLogger));
		mMockSession4Finished = std::shared_ptr<testing::NiceMock<test::MockSession>>(new testing::NiceMock<test::MockSession>(mLogger));
		ON_CALL(*mMockSession1Open, sendBeaconRawPtrProxy(testing::_))
			.WillByDefault(testing::Return(new protocol::StatusResponse("", 200)));
		ON_CALL(*mMockSession2Open, sendBeaconRawPtrProxy(testing::_))
			.WillByDefault(testing::Return(new protocol::StatusResponse("", 404)));

		mMockContext = std::shared_ptr<testing::NiceMock<test::MockBeaconSendingContext>>(new testing::NiceMock<test::MockBeaconSendingContext>(mLogger));
		ON_CALL(*mMockContext, isTimeSyncSupported())
			.WillByDefault(testing::Return(true));
		ON_CALL(*mMockContext, getLastTimeSyncTime())
			.WillByDefault(testing::Return(0L));
		ON_CALL(*mMockContext, getCurrentTimestamp())
			.WillByDefault(testing::Return(42L));
		ON_CALL(*mMockContext, getSendInterval())
			.WillByDefault(testing::Return(0L));

		ON_CALL(*mMockContext, getAllNewSessions())
			.WillByDefault(testing::Invoke(&*mMockContext, &test::MockBeaconSendingContext::RealGetAllNewSessions));
		ON_CALL(*mMockContext, getAllOpenAndConfiguredSessions())
			.WillByDefault(testing::Invoke(&*mMockContext, &test::MockBeaconSendingContext::RealGetAllOpenAndConfiguredSessions));
		ON_CALL(*mMockContext, getAllFinishedAndConfiguredSessions())
			.WillByDefault(testing::Invoke(&*mMockContext, &test::MockBeaconSendingContext::RealGetAllFinishedAndConfiguredSessions));
		ON_CALL(*mMockContext, finishSession(testing::_))
			.WillByDefault(testing::WithArgs<0>(testing::Invoke(&*mMockContext, &test::MockBeaconSendingContext::RealFinishSession)));

		mHttpClientConfiguration = std::make_shared<configuration::HTTPClientConfiguration>("test url",123, "application id");
		mMockHttpClient = std::make_shared<testing::NiceMock<test::MockHTTPClient>>(mHttpClientConfiguration);



		mMockHttpClientProvider = std::shared_ptr<testing::NiceMock<test::MockHTTPClientProvider>>(new testing::NiceMock<test::MockHTTPClientProvider>());
		ON_CALL(*mMockContext, getHTTPClientProvider())
			.WillByDefault(testing::Return(mMockHttpClientProvider));
		ON_CALL(*mMockHttpClientProvider, createClient(testing::_, testing::_))
			.WillByDefault(testing::Return(mMockHttpClient));

		ON_CALL(*mMockContext, getHTTPClient())
			.WillByDefault(testing::Return(mMockHttpClient));

		// testing::Return stores the result of the call as default value 
		// this means that the instance of the StatusReponse* is returned as unique_ptr multiple times
		// when deleting the instance the second time the test would crash
		// using a lambda resolves this issue by creating new instances
		ON_CALL(*mMockHttpClient, sendStatusRequestRawPtrProxy())
			.WillByDefault(testing::Invoke([]() -> protocol::StatusResponse* 
				{
					return new protocol::StatusResponse();
				}));
		ON_CALL(*mMockHttpClient, sendNewSessionRequestRawPtrProxy())
			.WillByDefault(testing::Invoke([]() -> protocol::StatusResponse*
				{
				return new protocol::StatusResponse();
				}));

		ON_CALL(*mMockSession1Open, getBeaconConfiguration())
			.WillByDefault(testing::Return(std::make_shared<configuration::BeaconConfiguration>()));
		ON_CALL(*mMockSession2Open, getBeaconConfiguration())
			.WillByDefault(testing::Return(std::make_shared<configuration::BeaconConfiguration>()));
		ON_CALL(*mMockSession3Finished, getBeaconConfiguration())
			.WillByDefault(testing::Return(std::make_shared<configuration::BeaconConfiguration>()));
		ON_CALL(*mMockSession4Finished, getBeaconConfiguration())
			.WillByDefault(testing::Return(std::make_shared<configuration::BeaconConfiguration>()));

		mMockContext->startSession(mMockSession1Open);
		mMockContext->startSession(mMockSession2Open);
		mMockContext->startSession(mMockSession3Finished);
		mMockContext->finishSession(mMockSession3Finished);
		mMockContext->startSession(mMockSession4Finished);
		mMockContext->finishSession(mMockSession4Finished);
	}

	void TearDown()
	{
		mLogger = nullptr;
		mMockContext = nullptr;
		mMockSession1Open = nullptr;
		mMockSession2Open = nullptr;
		mMockSession3Finished = nullptr;
		mMockSession4Finished = nullptr;
		mMockHttpClientProvider = nullptr;
	}
	
	std::ostringstream devNull;
	std::shared_ptr<openkit::ILogger> mLogger;
	uint32_t mCallCount;
	std::shared_ptr<testing::NiceMock<test::MockBeaconSendingContext>> mMockContext;
	std::shared_ptr<testing::NiceMock<test::MockSession>> mMockSession1Open;
	std::shared_ptr<testing::NiceMock<test::MockSession>> mMockSession2Open;
	std::shared_ptr<testing::NiceMock<test::MockSession>> mMockSession3Finished;
	std::shared_ptr<testing::NiceMock<test::MockSession>> mMockSession4Finished;
	std::shared_ptr<testing::NiceMock<test::MockHTTPClientProvider>> mMockHttpClientProvider;
	std::shared_ptr<testing::NiceMock<test::MockHTTPClient>> mMockHttpClient;
	std::shared_ptr<configuration::HTTPClientConfiguration> mHttpClientConfiguration;
};

TEST_F(BeaconSendingCaptureOnStateTest, aBeaconSendingCaptureOnStateIsNotATerminalState)
{
	// given
	auto target = std::shared_ptr<communication::AbstractBeaconSendingState>(new communication::BeaconSendingCaptureOnState());

	// verify that BeaconSendingCaptureOnState is not a terminal state
	EXPECT_FALSE(target->isTerminalState());
}

TEST_F(BeaconSendingCaptureOnStateTest, aBeaconSendingCaptureOnStateHasTerminalStateBeaconSendingFlushSessions)
{
	// given
	auto target = std::shared_ptr<communication::AbstractBeaconSendingState>(new communication::BeaconSendingCaptureOnState());
	auto terminalState = target->getShutdownState();

	// verify that terminal state is BeaconSendingFlushSessions
	ASSERT_THAT(terminalState, IsABeaconSendingFlushSessionsState());
}

TEST_F(BeaconSendingCaptureOnStateTest, aBeaconSendingCaptureOnStateTransitionsToTimeSyncStateWhenLastSyncTimeIsNegative)
{
	// given
	auto target = std::shared_ptr<communication::AbstractBeaconSendingState>(new communication::BeaconSendingCaptureOnState());

	ON_CALL(*mMockContext, isTimeSyncSupported())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mMockContext, isCaptureOn())
		.WillByDefault(testing::Return(false));
	ON_CALL(*mMockContext, getLastTimeSyncTime())
		.WillByDefault(testing::Return(-1L));

	// then expect lastStatusCheckTime to be updated and that next state is time sync state
	EXPECT_CALL(*mMockContext, setNextState(IsABeaconSendingTimeSyncState()))
		.Times(testing::Exactly(1));

	// when calling execute
	target->execute(*mMockContext);
}

TEST_F(BeaconSendingCaptureOnStateTest, aBeaconSendingCaptureOnStateTransitionsToTimeSyncStateWhenCheckIntervalPassed)
{
	// given
	communication::BeaconSendingCaptureOnState target;
	
	ON_CALL(*mMockContext, isTimeSyncSupported())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mMockContext, isCaptureOn())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mMockContext, getLastTimeSyncTime())
		.WillByDefault(testing::Return(0L));
	ON_CALL(*mMockContext, getCurrentTimestamp())
		.WillByDefault(testing::Return(7500000L));

	EXPECT_CALL(*mMockContext, setNextState(IsABeaconSendingTimeSyncState()))
		.Times(testing::Exactly(1));

	// when calling execute
	target.doExecute(*mMockContext);
}

TEST_F(BeaconSendingCaptureOnStateTest, aBeaconSendingCaptureOnStateTransitionsToFlushStateIfShutdownRequested)
{
	// given
	communication::BeaconSendingCaptureOnState target;

	ON_CALL(*mMockContext, isShutdownRequested())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mMockContext, isCaptureOn())
		.WillByDefault(testing::Return(true));

	EXPECT_CALL(*mMockContext, setNextState(IsABeaconSendingFlushSessionsState()))
		.Times(testing::Exactly(1));

	// when calling execute
	target.execute(*mMockContext);
}

MATCHER_P(IsAmockedSession, mock, "")
{
	if (arg == mock)
	{
		return true;
	}
	*result_listener << "Unexpected mock";

	return false;
}

TEST_F(BeaconSendingCaptureOnStateTest, aBeaconSendingCaptureOnStateContinuesWithNextFinishedSessionIfSendingWasUnsuccessfulButBeaoonIsEmtpy)
{
	// given
	communication::BeaconSendingCaptureOnState target;
	ON_CALL(*mMockSession3Finished, sendBeaconRawPtrProxy(testing::_))
		.WillByDefault(testing::Return(new testing::NiceMock<test::MockStatusResponse>()));
	ON_CALL(*mMockSession4Finished, sendBeaconRawPtrProxy(testing::_))
		.WillByDefault(testing::Return(new testing::NiceMock<test::MockStatusResponse>()));
	ON_CALL(*mMockContext, isCaptureOn())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mMockSession3Finished, isEmpty())
		.WillByDefault(testing::Return(true));

	// then
	EXPECT_CALL(*mMockSession1Open, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession2Open, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession1Open, clearCapturedData())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession2Open, clearCapturedData())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession3Finished, clearCapturedData())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession4Finished, clearCapturedData())
		.Times(testing::Exactly(1));

	EXPECT_CALL(*mMockContext, getAllFinishedAndConfiguredSessions())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockContext, removeSession(testing::_))
		.Times(testing::Exactly(4));
	EXPECT_CALL(*mMockContext, setLastOpenSessionBeaconSendTime(testing::_))
		.Times(testing::Exactly(1));

	// move open sessions to finished session by calling BeaconSendinContext::finishSessions
	mMockContext->finishSession(mMockSession1Open);
	mMockContext->finishSession(mMockSession2Open);

	// when calling execute
	target.doExecute(*mMockContext);
}

TEST_F(BeaconSendingCaptureOnStateTest, aBeaconSendingCaptureOnStateTransitionsToTimeSyncStateIfSessionExpired)
{
	// given
	testing::StrictMock<test::MockBeaconSendingContext> mockContext(mLogger);
	communication::BeaconSendingCaptureOnState target;
	ON_CALL(mockContext, isTimeSyncSupported())
		.WillByDefault(testing::Return(true));
	ON_CALL(mockContext, getLastTimeSyncTime())
		.WillByDefault(testing::Return(0L));
	ON_CALL(mockContext, getCurrentTimestamp())
		.WillByDefault(testing::Return(72000042L));
	
	// then
	EXPECT_CALL(mockContext, isTimeSyncSupported())
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, getCurrentTimestamp())
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, getLastTimeSyncTime())
		.Times(::testing::Exactly(2));
	EXPECT_CALL(mockContext, setNextState(IsABeaconSendingTimeSyncState()))
		.Times(testing::Exactly(1));

	// when calling execute
	target.doExecute(mockContext);
}

TEST_F(BeaconSendingCaptureOnStateTest, aBeaconSendingCaptureOnStateTransitionsToCaptureOffStateWhenCapturingGotDisabled)
{
	// given
	communication::BeaconSendingCaptureOnState target;
	ON_CALL(*mMockContext, isTimeSyncSupported())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mMockContext, isCaptureOn())
		.WillByDefault(testing::Return(false));
	
	// then
	EXPECT_CALL(*mMockContext, isCaptureOn())
		.Times(::testing::Exactly(2));
	EXPECT_CALL(*mMockContext, setNextState(IsABeaconSendingCaptureOffState()))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockContext, setLastOpenSessionBeaconSendTime(testing::_))
		.Times(testing::Exactly(1));

	// when calling execute
	target.execute(*mMockContext);
}

TEST_F(BeaconSendingCaptureOnStateTest, newSessionRequestsAreMadeForAllNewSessions)
{
	// given
	communication::BeaconSendingCaptureOnState target;

	EXPECT_CALL(*mMockHttpClient, sendNewSessionRequestRawPtrProxy())
		.Times(testing::Exactly(2));

	std::vector<protocol::StatusResponse*> responses = { new protocol::StatusResponse("mp=5", 200),
														new protocol::StatusResponse("mp=3", 200)
	};

	auto sessionWrapper1 = std::make_shared<core::SessionWrapper>(mMockSession1Open);
	auto sessionWrapper2 = std::make_shared<core::SessionWrapper>(mMockSession2Open);

	std::vector<std::shared_ptr<core::SessionWrapper>> newSessions = { sessionWrapper1, sessionWrapper2 };

	ON_CALL(*mMockHttpClient, sendNewSessionRequestRawPtrProxy())
		.WillByDefault(testing::Invoke([&responses]() -> protocol::StatusResponse* {
		auto nextResponse = *responses.begin();
		if (nextResponse != nullptr)
		{
			responses.erase(responses.begin());
			return nextResponse;
		}
		return (protocol::StatusResponse*)nullptr;
		}));

	//only return the two new sessions for the new session request
	ON_CALL(*mMockContext, getAllNewSessions())
		.WillByDefault(testing::Return(newSessions));

	/// capture the beacon configuration set by the sendNewSessionRequests(...) method
	std::shared_ptr<configuration::BeaconConfiguration> capturedBeaconConfigurationForSession1;
	std::shared_ptr<configuration::BeaconConfiguration> capturedBeaconConfigurationForSession2;

	ON_CALL(*mMockSession1Open, setBeaconConfiguration(testing::_))
		.WillByDefault(testing::WithArgs<0>(testing::Invoke([&capturedBeaconConfigurationForSession1](std::shared_ptr<configuration::BeaconConfiguration> beaconConfig) {
		capturedBeaconConfigurationForSession1 = beaconConfig;
	})));
	ON_CALL(*mMockSession2Open, setBeaconConfiguration(testing::_))
		.WillByDefault(testing::WithArgs<0>(testing::Invoke([&capturedBeaconConfigurationForSession2](std::shared_ptr<configuration::BeaconConfiguration> beaconConfig) {
		capturedBeaconConfigurationForSession2 = beaconConfig;
	})));

	// when calling execute
	target.execute(*mMockContext);

	ASSERT_EQ(capturedBeaconConfigurationForSession1->getMultiplicity(), 5);
	ASSERT_EQ(capturedBeaconConfigurationForSession1->getDataCollectionLevel(), configuration::BeaconConfiguration::DEFAULT_DATA_COLLECTION_LEVEL);
	ASSERT_EQ(capturedBeaconConfigurationForSession1->getCrashReportingLevel(), configuration::BeaconConfiguration::DEFAULT_CRASH_REPORTING_LEVEL);

	ASSERT_EQ(capturedBeaconConfigurationForSession2->getMultiplicity(), 3);
	ASSERT_EQ(capturedBeaconConfigurationForSession2->getDataCollectionLevel(), configuration::BeaconConfiguration::DEFAULT_DATA_COLLECTION_LEVEL);
	ASSERT_EQ(capturedBeaconConfigurationForSession2->getCrashReportingLevel(), configuration::BeaconConfiguration::DEFAULT_CRASH_REPORTING_LEVEL);
}

// Expectation: Given enough failed new session requests the beacon configuration created by the method
// performing new session requests has a mulitplicity of '0'
TEST_F(BeaconSendingCaptureOnStateTest, multiplicityIsSetToZeroIfNoFurtherNewSessionRequestsAreAllowed)
{
	// given
	communication::BeaconSendingCaptureOnState target;

	std::vector<protocol::StatusResponse*> statusResponses = { new protocol::StatusResponse("mp=5", 200) };

	ON_CALL(*mMockHttpClient, sendNewSessionRequestRawPtrProxy())
		.WillByDefault(testing::Invoke([&statusResponses]() {
			if (statusResponses.size() > 0)
			{
				auto nextResponse = *statusResponses.begin();
				statusResponses.erase(statusResponses.begin());
				return nextResponse;
			}
			else
			{
				return (protocol::StatusResponse*)nullptr;
			}
	}));

	auto sessionWrapper1 = std::make_shared<core::SessionWrapper>(mMockSession1Open);
	auto sessionWrapper2 = std::make_shared<core::SessionWrapper>(mMockSession2Open);
	std::vector<std::shared_ptr<core::SessionWrapper>> newSessions = { sessionWrapper1, sessionWrapper2  };


	// simulate enough failing new session requests
	while (sessionWrapper1->canSendNewSessionRequest())
	{
		sessionWrapper1->decreaseNumberOfNewSessionRequests();
	}

	while (sessionWrapper2->canSendNewSessionRequest())
	{
		sessionWrapper2->decreaseNumberOfNewSessionRequests();
	}

	//only return the two new sessions for the new session request
	ON_CALL(*mMockContext, getAllNewSessions())
		.WillByDefault(testing::Return(newSessions));

	/// capture the beacon configuration set by the sendNewSessionRequests(...) method
	std::shared_ptr<configuration::BeaconConfiguration> capturedBeaconConfigurationForSession1;
	std::shared_ptr<configuration::BeaconConfiguration> capturedBeaconConfigurationForSession2;

	ON_CALL(*mMockSession1Open, setBeaconConfiguration(testing::_))
		.WillByDefault(testing::WithArgs<0>(testing::Invoke([&capturedBeaconConfigurationForSession1](std::shared_ptr<configuration::BeaconConfiguration> beaconConfig) {
			capturedBeaconConfigurationForSession1 = beaconConfig;
			})));
	ON_CALL(*mMockSession2Open, setBeaconConfiguration(testing::_))
		.WillByDefault(testing::WithArgs<0>(testing::Invoke([&capturedBeaconConfigurationForSession2](std::shared_ptr<configuration::BeaconConfiguration> beaconConfig) {
		capturedBeaconConfigurationForSession2 = beaconConfig;
	})));
	
	// expectation: no actual calls to http client regarding new sessions
	EXPECT_CALL(*mMockHttpClient, sendNewSessionRequestRawPtrProxy())
		.Times(testing::Exactly(0));

	// calling getter and setter for beacon configuration once indicates that the execution went into the if clause handling the case when no more new session
	// requests can be made
	EXPECT_CALL(*mMockSession1Open, getBeaconConfiguration())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession1Open, setBeaconConfiguration(testing::_)) 
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession2Open, getBeaconConfiguration())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession2Open, setBeaconConfiguration(testing::_)) 
		.Times(testing::Exactly(1));

	// when calling execute
	target.doExecute(*mMockContext);

	ASSERT_EQ(capturedBeaconConfigurationForSession1->getMultiplicity(), 0);
	ASSERT_EQ(capturedBeaconConfigurationForSession1->getDataCollectionLevel(), configuration::BeaconConfiguration::DEFAULT_DATA_COLLECTION_LEVEL);
	ASSERT_EQ(capturedBeaconConfigurationForSession1->getCrashReportingLevel(), configuration::BeaconConfiguration::DEFAULT_CRASH_REPORTING_LEVEL);

	ASSERT_EQ(capturedBeaconConfigurationForSession2->getMultiplicity(), 0);
	ASSERT_EQ(capturedBeaconConfigurationForSession2->getDataCollectionLevel(), configuration::BeaconConfiguration::DEFAULT_DATA_COLLECTION_LEVEL);
	ASSERT_EQ(capturedBeaconConfigurationForSession2->getCrashReportingLevel(), configuration::BeaconConfiguration::DEFAULT_CRASH_REPORTING_LEVEL);
}

TEST_F(BeaconSendingCaptureOnStateTest, finishedSessionsAreSent)
{
	// given
	communication::BeaconSendingCaptureOnState target;

	auto sessionWrapper1 = std::make_shared<core::SessionWrapper>(mMockSession3Finished);
	sessionWrapper1->updateBeaconConfiguration(std::make_shared<configuration::BeaconConfiguration>());
	auto sessionWrapper2 = std::make_shared<core::SessionWrapper>(mMockSession4Finished);
	sessionWrapper2->updateBeaconConfiguration(std::make_shared<configuration::BeaconConfiguration>());
	std::vector<std::shared_ptr<core::SessionWrapper>> finishedSessions = { sessionWrapper1, sessionWrapper2 };

	//only return the two new sessions for the new session request
	ON_CALL(*mMockContext, getAllFinishedAndConfiguredSessions())
		.WillByDefault(testing::Return(finishedSessions));
	ON_CALL(*mMockContext, getAllNewSessions())
		.WillByDefault(testing::Return(std::vector<std::shared_ptr<core::SessionWrapper>>()));
	ON_CALL(*mMockContext, getAllOpenAndConfiguredSessions())
		.WillByDefault(testing::Return(std::vector<std::shared_ptr<core::SessionWrapper>>()));
	ON_CALL(*mMockContext, isCaptureOn())
		.WillByDefault(testing::Return(true));

	ON_CALL(*mMockSession3Finished, sendBeaconRawPtrProxy(testing::_))
		.WillByDefault(testing::Return(new protocol::StatusResponse("", 200)));
	ON_CALL(*mMockSession4Finished, sendBeaconRawPtrProxy(testing::_))
		.WillByDefault(testing::Return(new protocol::StatusResponse("", 200)));

	EXPECT_CALL(*mMockSession3Finished, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession4Finished, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession3Finished, clearCapturedData())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession4Finished, clearCapturedData())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockContext, removeSession(testing::_))
		.Times(testing::Exactly(2));

	// when calling execute
	target.doExecute(*mMockContext);
}

TEST_F(BeaconSendingCaptureOnStateTest, unsuccessfulFinishedSessionsAreNotRemovedFromCache)
{
	// given
	communication::BeaconSendingCaptureOnState target;

	auto sessionWrapper1 = std::make_shared<core::SessionWrapper>(mMockSession3Finished);
	sessionWrapper1->updateBeaconConfiguration(std::make_shared<configuration::BeaconConfiguration>());
	auto sessionWrapper2 = std::make_shared<core::SessionWrapper>(mMockSession4Finished);
	sessionWrapper2->updateBeaconConfiguration(std::make_shared<configuration::BeaconConfiguration>());
	std::vector<std::shared_ptr<core::SessionWrapper>> finishedSessions = { sessionWrapper1, sessionWrapper2 };

	//only return the two new sessions for the new session request
	ON_CALL(*mMockContext, getAllFinishedAndConfiguredSessions())
		.WillByDefault(testing::Return(finishedSessions));
	ON_CALL(*mMockContext, getAllNewSessions())
		.WillByDefault(testing::Return(std::vector<std::shared_ptr<core::SessionWrapper>>()));
	ON_CALL(*mMockContext, getAllOpenAndConfiguredSessions())
		.WillByDefault(testing::Return(std::vector<std::shared_ptr<core::SessionWrapper>>()));
	ON_CALL(*mMockContext, isCaptureOn())
		.WillByDefault(testing::Return(true));

	ON_CALL(*mMockSession3Finished, sendBeaconRawPtrProxy(testing::_))
		.WillByDefault(testing::Return(nullptr));
	ON_CALL(*mMockSession4Finished, sendBeaconRawPtrProxy(testing::_))
		.WillByDefault(testing::Return(nullptr));

	EXPECT_CALL(*mMockContext, getAllFinishedAndConfiguredSessions())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockContext, removeSession(testing::_))
		.Times(testing::Exactly(0));

	// when calling execute
	target.doExecute(*mMockContext);
}

TEST_F(BeaconSendingCaptureOnStateTest, openSessionsAreSentIfSendIntervalIsExceeded)
{
	// given
	communication::BeaconSendingCaptureOnState target;

	auto sessionWrapper1 = std::make_shared<core::SessionWrapper>(mMockSession1Open);
	sessionWrapper1->updateBeaconConfiguration(std::make_shared<configuration::BeaconConfiguration>(2, openkit::DataCollectionLevel::USER_BEHAVIOR, openkit::CrashReportingLevel::OPT_IN_CRASHES));
	auto sessionWrapper2 = std::make_shared<core::SessionWrapper>(mMockSession2Open);
	sessionWrapper2->updateBeaconConfiguration(std::make_shared<configuration::BeaconConfiguration>(2, openkit::DataCollectionLevel::USER_BEHAVIOR, openkit::CrashReportingLevel::OPT_IN_CRASHES));
	std::vector<std::shared_ptr<core::SessionWrapper>> finishedSessions = { sessionWrapper1, sessionWrapper2 };

	//only return the two new sessions for the new session request
	ON_CALL(*mMockContext, getAllFinishedAndConfiguredSessions())
		.WillByDefault(testing::Return(std::vector<std::shared_ptr<core::SessionWrapper>>()));
	ON_CALL(*mMockContext, getAllNewSessions())
		.WillByDefault(testing::Return(std::vector<std::shared_ptr<core::SessionWrapper>>()));
	ON_CALL(*mMockContext, getAllOpenAndConfiguredSessions())
		.WillByDefault(testing::Return(finishedSessions));
	ON_CALL(*mMockContext, isCaptureOn())
		.WillByDefault(testing::Return(true));

	ON_CALL(*mMockSession3Finished, sendBeaconRawPtrProxy(testing::_))
		.WillByDefault(testing::Return(nullptr));
	ON_CALL(*mMockSession4Finished, sendBeaconRawPtrProxy(testing::_))
		.WillByDefault(testing::Return(nullptr));

	ON_CALL(*mMockContext, getCurrentTimestamp())
		.WillByDefault(testing::Return(100));
	ON_CALL(*mMockContext, getSendInterval())
		.WillByDefault(testing::Return(50));
	ON_CALL(*mMockContext, getLastOpenSessionBeaconSendTime())
		.WillByDefault(testing::Return(45));

	EXPECT_CALL(*mMockSession1Open, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession2Open, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockContext, setLastOpenSessionBeaconSendTime(testing::_))
		.Times(testing::Exactly(1));

	// when calling execute
	target.doExecute(*mMockContext);
}

TEST_F(BeaconSendingCaptureOnStateTest, openSessionsAreNotSentIfSendIntervalIsNotExceeded)
{
	// given
	communication::BeaconSendingCaptureOnState target;

	auto sessionWrapper1 = std::make_shared<core::SessionWrapper>(mMockSession1Open);
	auto sessionWrapper2 = std::make_shared<core::SessionWrapper>(mMockSession2Open);
	std::vector<std::shared_ptr<core::SessionWrapper>> finishedSessions = { sessionWrapper1, sessionWrapper2 };

	//only return the two new sessions for the new session request
	ON_CALL(*mMockContext, getAllFinishedAndConfiguredSessions())
		.WillByDefault(testing::Return(std::vector<std::shared_ptr<core::SessionWrapper>>()));
	ON_CALL(*mMockContext, getAllNewSessions())
		.WillByDefault(testing::Return(std::vector<std::shared_ptr<core::SessionWrapper>>()));
	ON_CALL(*mMockContext, getAllOpenAndConfiguredSessions())
		.WillByDefault(testing::Return(finishedSessions));
	ON_CALL(*mMockContext, isCaptureOn())
		.WillByDefault(testing::Return(true));

	ON_CALL(*mMockSession3Finished, sendBeaconRawPtrProxy(testing::_))
		.WillByDefault(testing::Return(nullptr));
	ON_CALL(*mMockSession4Finished, sendBeaconRawPtrProxy(testing::_))
		.WillByDefault(testing::Return(nullptr));

	ON_CALL(*mMockContext, getCurrentTimestamp())
		.WillByDefault(testing::Return(100));
	ON_CALL(*mMockContext, getSendInterval())
		.WillByDefault(testing::Return(50));
	ON_CALL(*mMockContext, getLastOpenSessionBeaconSendTime())
		.WillByDefault(testing::Return(49));

	EXPECT_CALL(*mMockSession1Open, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mMockSession2Open, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mMockContext, setLastOpenSessionBeaconSendTime(testing::_))
		.Times(testing::Exactly(1));

	// when calling execute
	target.doExecute(*mMockContext);
}

TEST_F(BeaconSendingCaptureOnStateTest, ToStringReturnsCorrectStateName)
{
	// given
	communication::BeaconSendingCaptureOnState target;

	// when
	auto stateName = target.getStateName();

	// then
	ASSERT_STREQ(stateName, "CaptureOn");
}