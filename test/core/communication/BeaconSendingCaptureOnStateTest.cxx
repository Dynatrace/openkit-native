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
#include "MockBeaconSendingContext.h"
#include "../objects/MockSession.h"
#include "../../api/mock/MockILogger.h"
#include "../../protocol/mock/MockIHTTPClient.h"
#include "../../providers/mock/MockIHTTPClientProvider.h"

#include "core/objects/Session.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace test;
using namespace test::types;

using MockNiceIHTTPClient_sp = std::shared_ptr<testing::NiceMock<MockIHTTPClient>>;
using MockNiceIHTTPClientProvider_sp = std::shared_ptr<testing::NiceMock<MockIHTTPClientProvider>>;
using MockNiceILogger_sp = std::shared_ptr<testing::NiceMock<MockILogger>>;
using MockNiceSession_t = testing::NiceMock<MockSession>;
using MockNiceSession_sp = std::shared_ptr<MockNiceSession_t>;
using MockNiceBeaconSendingContext_t = testing::NiceMock<MockBeaconSendingContext>;
using MockNiceBeaconSendingContext_sp = std::shared_ptr<MockNiceBeaconSendingContext_t>;

class BeaconSendingCaptureOnStateTest : public testing::Test
{
protected:

	MockNiceILogger_sp mLogger;
	MockNiceBeaconSendingContext_sp mMockContext;
	MockNiceSession_sp mMockSession1Open;
	MockNiceSession_sp mMockSession2Open;
	MockNiceSession_sp mMockSession3Finished;
	MockNiceSession_sp mMockSession4Finished;
	MockNiceIHTTPClientProvider_sp mMockHttpClientProvider;
	MockNiceIHTTPClient_sp mMockHttpClient;

	void SetUp()
	{
		mLogger = MockILogger::createNice();
		mMockSession1Open = std::make_shared<MockNiceSession_t>(mLogger);
		mMockSession2Open = std::make_shared<MockNiceSession_t>(mLogger);
		mMockSession3Finished = std::make_shared<MockNiceSession_t>(mLogger);
		mMockSession4Finished = std::make_shared<MockNiceSession_t>(mLogger);
		ON_CALL(*mMockSession1Open, sendBeaconRawPtrProxy(testing::_))
			.WillByDefault(testing::Invoke([&](IHttpClientProvider_sp) -> StatusResponse_sp
			{
				return std::make_shared<StatusResponse_t>(mLogger, "", 200, IStatusResponse_t::ResponseHeaders());
			}));
		ON_CALL(*mMockSession2Open, sendBeaconRawPtrProxy(testing::_))
			.WillByDefault(testing::Invoke([&](IHttpClientProvider_sp) -> StatusResponse_sp
			{
				return std::make_shared<StatusResponse_t>(mLogger, "", 404, IStatusResponse_t::ResponseHeaders());
			}));

		mMockContext = std::make_shared<MockNiceBeaconSendingContext_t>(mLogger);
		ON_CALL(*mMockContext, isShutdownRequested())
			.WillByDefault(testing::Return(false));
		ON_CALL(*mMockContext, getCurrentTimestamp())
			.WillByDefault(testing::Return(42L));
		ON_CALL(*mMockContext, getSendInterval())
			.WillByDefault(testing::Return(0L));

		ON_CALL(*mMockContext, getAllNewSessions())
			.WillByDefault(testing::Invoke(&*mMockContext, &MockBeaconSendingContext::RealGetAllNewSessions));
		ON_CALL(*mMockContext, getAllOpenAndConfiguredSessions())
			.WillByDefault(testing::Invoke(&*mMockContext, &MockBeaconSendingContext::RealGetAllOpenAndConfiguredSessions));
		ON_CALL(*mMockContext, getAllFinishedAndConfiguredSessions())
			.WillByDefault(testing::Invoke(&*mMockContext, &MockBeaconSendingContext::RealGetAllFinishedAndConfiguredSessions));
		ON_CALL(*mMockContext, finishSession(testing::_))
			.WillByDefault(testing::WithArgs<0>(testing::Invoke(&*mMockContext, &MockBeaconSendingContext::RealFinishSession)));

		mMockHttpClient = MockIHTTPClient::createNice();

		mMockHttpClientProvider = MockIHTTPClientProvider::createNice();
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
		ON_CALL(*mMockHttpClient, sendStatusRequest())
			.WillByDefault(testing::Invoke([this]() -> StatusResponse_sp
			{
				return std::make_shared<StatusResponse_t>(mLogger, "", 200, IStatusResponse_t::ResponseHeaders());
			}));
		ON_CALL(*mMockHttpClient, sendNewSessionRequest())
			.WillByDefault(testing::Invoke([this]() -> StatusResponse_sp
			{
				return std::make_shared<StatusResponse_t>(mLogger, "", 200, IStatusResponse_t::ResponseHeaders());
			}));

		ON_CALL(*mMockSession1Open, getBeaconConfiguration())
			.WillByDefault(testing::Return(std::make_shared<BeaconConfiguration_t>()));
		ON_CALL(*mMockSession2Open, getBeaconConfiguration())
			.WillByDefault(testing::Return(std::make_shared<BeaconConfiguration_t>()));
		ON_CALL(*mMockSession3Finished, getBeaconConfiguration())
			.WillByDefault(testing::Return(std::make_shared<BeaconConfiguration_t>()));
		ON_CALL(*mMockSession4Finished, getBeaconConfiguration())
			.WillByDefault(testing::Return(std::make_shared<BeaconConfiguration_t>()));

		mMockContext->startSession(mMockSession1Open);
		mMockContext->startSession(mMockSession2Open);
		mMockContext->startSession(mMockSession3Finished);
		mMockContext->finishSession(mMockSession3Finished);
		mMockContext->startSession(mMockSession4Finished);
		mMockContext->finishSession(mMockSession4Finished);
	}
};

TEST_F(BeaconSendingCaptureOnStateTest, aBeaconSendingCaptureOnStateIsNotATerminalState)
{
	// given
	auto target = BeaconSendingCaptureOnState_t();

	// verify that BeaconSendingCaptureOnState is not a terminal state
	EXPECT_FALSE(target.isTerminalState());
}

TEST_F(BeaconSendingCaptureOnStateTest, aBeaconSendingCaptureOnStateHasTerminalStateBeaconSendingFlushSessions)
{
	// given
	auto target = BeaconSendingCaptureOnState_t();

	// when
	auto terminalState = target.getShutdownState();

	// verify that terminal state is BeaconSendingFlushSessions
	ASSERT_THAT(terminalState, IsABeaconSendingFlushSessionsState());
}

TEST_F(BeaconSendingCaptureOnStateTest, aBeaconSendingCaptureOnStateTransitionsToFlushStateIfShutdownRequested)
{
	// given
	auto target = BeaconSendingCaptureOnState_t();

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

TEST_F(BeaconSendingCaptureOnStateTest, aBeaconSendingCaptureOnStateContinuesWithNextFinishedSessionIfSendingWasUnsuccessfulButBeaconIsEmtpy)
{
	// given
	auto target = BeaconSendingCaptureOnState_t();

	ON_CALL(*mMockSession3Finished, sendBeaconRawPtrProxy(testing::_))
		.WillByDefault(testing::Invoke([&](IHttpClientProvider_sp) -> StatusResponse_sp {
			return std::make_shared<StatusResponse_t>(mLogger, "", 400, IStatusResponse_t::ResponseHeaders());
		}));
	ON_CALL(*mMockSession4Finished, sendBeaconRawPtrProxy(testing::_))
		.WillByDefault(testing::Invoke([&](IHttpClientProvider_sp) -> StatusResponse_sp {
			return std::make_shared<StatusResponse_t>(mLogger, "", 200, IStatusResponse_t::ResponseHeaders());
		}));
	ON_CALL(*mMockSession1Open, sendBeaconRawPtrProxy(testing::_))
		.WillByDefault(testing::Invoke([&](IHttpClientProvider_sp) -> StatusResponse_sp
		{
			return std::make_shared<StatusResponse_t>(mLogger, "", 200, IStatusResponse_t::ResponseHeaders());
		}));
	ON_CALL(*mMockSession2Open, sendBeaconRawPtrProxy(testing::_))
		.WillByDefault(testing::Invoke([&](IHttpClientProvider_sp) -> StatusResponse_sp
		{
			return std::make_shared<StatusResponse_t>(mLogger, "", 200, IStatusResponse_t::ResponseHeaders());
		}));
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
	target.execute(*mMockContext);
}

TEST_F(BeaconSendingCaptureOnStateTest, aBeaconSendingCaptureOnStateTransitionsToCaptureOffStateWhenCapturingGotDisabled)
{
	// given
	auto target = BeaconSendingCaptureOnState_t();

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
	auto target = BeaconSendingCaptureOnState_t();

	EXPECT_CALL(*mMockHttpClient, sendNewSessionRequest())
		.Times(testing::Exactly(2));

	std::vector<StatusResponse_sp> responses =
	{
		std::make_shared<StatusResponse_t>(mLogger, "mp=5", 200, IStatusResponse_t::ResponseHeaders()),
		std::make_shared<StatusResponse_t>(mLogger, "mp=3", 200, IStatusResponse_t::ResponseHeaders())
	};

	auto sessionWrapper1 = std::make_shared<SessionWrapper_t>(mMockSession1Open);
	auto sessionWrapper2 = std::make_shared<SessionWrapper_t>(mMockSession2Open);

	std::vector<SessionWrapper_sp> newSessions = { sessionWrapper1, sessionWrapper2 };

	ON_CALL(*mMockHttpClient, sendNewSessionRequest())
		.WillByDefault(testing::Invoke([&]() -> StatusResponse_sp
		{
			auto nextResponse = *responses.begin();
			if (nextResponse != nullptr)
			{
				responses.erase(responses.begin());
				return nextResponse;
			}
			return std::make_shared<StatusResponse_t>(mLogger, "", 400, IStatusResponse_t::ResponseHeaders());
		}));

	//only return the two new sessions for the new session request
	ON_CALL(*mMockContext, getAllNewSessions())
		.WillByDefault(testing::Return(newSessions));

	/// capture the beacon configuration set by the sendNewSessionRequests(...) method
	BeaconConfiguration_sp capturedBeaconConfigurationForSession1;
	BeaconConfiguration_sp capturedBeaconConfigurationForSession2;

	ON_CALL(*mMockSession1Open, setBeaconConfiguration(testing::_))
		.WillByDefault(testing::WithArgs<0>(testing::Invoke([&capturedBeaconConfigurationForSession1](BeaconConfiguration_sp beaconConfig) {
		capturedBeaconConfigurationForSession1 = beaconConfig;
	})));
	ON_CALL(*mMockSession2Open, setBeaconConfiguration(testing::_))
		.WillByDefault(testing::WithArgs<0>(testing::Invoke([&capturedBeaconConfigurationForSession2](BeaconConfiguration_sp beaconConfig) {
		capturedBeaconConfigurationForSession2 = beaconConfig;
	})));

	// when calling execute
	target.execute(*mMockContext);

	ASSERT_EQ(capturedBeaconConfigurationForSession1->getMultiplicity(), 5);
	ASSERT_EQ(capturedBeaconConfigurationForSession1->getDataCollectionLevel(), BeaconConfiguration_t::DEFAULT_DATA_COLLECTION_LEVEL);
	ASSERT_EQ(capturedBeaconConfigurationForSession1->getCrashReportingLevel(), BeaconConfiguration_t::DEFAULT_CRASH_REPORTING_LEVEL);

	ASSERT_EQ(capturedBeaconConfigurationForSession2->getMultiplicity(), 3);
	ASSERT_EQ(capturedBeaconConfigurationForSession2->getDataCollectionLevel(), BeaconConfiguration_t::DEFAULT_DATA_COLLECTION_LEVEL);
	ASSERT_EQ(capturedBeaconConfigurationForSession2->getCrashReportingLevel(), BeaconConfiguration_t::DEFAULT_CRASH_REPORTING_LEVEL);
}

// Expectation: Given enough failed new session requests the beacon configuration created by the method
// performing new session requests has a mulitplicity of '0'
TEST_F(BeaconSendingCaptureOnStateTest, multiplicityIsSetToZeroIfNoFurtherNewSessionRequestsAreAllowed)
{
	// given
	auto target = BeaconSendingCaptureOnState_t();

	std::vector<StatusResponse_sp> statusResponses =
	{
		std::make_shared<StatusResponse_t>(mLogger, "mp=5", 200, IStatusResponse_t::ResponseHeaders())
	};

	ON_CALL(*mMockHttpClient, sendNewSessionRequest())
		.WillByDefault(testing::Invoke([&]()
		{
			if (statusResponses.size() > 0)
			{
				auto nextResponse = *statusResponses.begin();
				statusResponses.erase(statusResponses.begin());
				return nextResponse;
			}
			else
			{
				return std::make_shared<StatusResponse_t>(mLogger, "", 400, IStatusResponse_t::ResponseHeaders());
			}
		}));

	auto sessionWrapper1 = std::make_shared<SessionWrapper_t>(mMockSession1Open);
	auto sessionWrapper2 = std::make_shared<SessionWrapper_t>(mMockSession2Open);
	std::vector<SessionWrapper_sp> newSessions = { sessionWrapper1, sessionWrapper2  };


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
	BeaconConfiguration_sp capturedBeaconConfigurationForSession1;
	BeaconConfiguration_sp capturedBeaconConfigurationForSession2;

	ON_CALL(*mMockSession1Open, setBeaconConfiguration(testing::_))
		.WillByDefault(testing::WithArgs<0>(testing::Invoke([&capturedBeaconConfigurationForSession1](BeaconConfiguration_sp beaconConfig) {
			capturedBeaconConfigurationForSession1 = beaconConfig;
			})));
	ON_CALL(*mMockSession2Open, setBeaconConfiguration(testing::_))
		.WillByDefault(testing::WithArgs<0>(testing::Invoke([&capturedBeaconConfigurationForSession2](BeaconConfiguration_sp beaconConfig) {
		capturedBeaconConfigurationForSession2 = beaconConfig;
	})));

	// expectation: no actual calls to http client regarding new sessions
	EXPECT_CALL(*mMockHttpClient, sendNewSessionRequest())
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
	target.execute(*mMockContext);

	ASSERT_EQ(capturedBeaconConfigurationForSession1->getMultiplicity(), 0);
	ASSERT_EQ(capturedBeaconConfigurationForSession1->getDataCollectionLevel(), BeaconConfiguration_t::DEFAULT_DATA_COLLECTION_LEVEL);
	ASSERT_EQ(capturedBeaconConfigurationForSession1->getCrashReportingLevel(), BeaconConfiguration_t::DEFAULT_CRASH_REPORTING_LEVEL);

	ASSERT_EQ(capturedBeaconConfigurationForSession2->getMultiplicity(), 0);
	ASSERT_EQ(capturedBeaconConfigurationForSession2->getDataCollectionLevel(), BeaconConfiguration_t::DEFAULT_DATA_COLLECTION_LEVEL);
	ASSERT_EQ(capturedBeaconConfigurationForSession2->getCrashReportingLevel(), BeaconConfiguration_t::DEFAULT_CRASH_REPORTING_LEVEL);
}

TEST_F(BeaconSendingCaptureOnStateTest, newSessionRequestsAreAbortedWhenTooManyRequestsResponseIsReceived)
{
	// given
	auto target = BeaconSendingCaptureOnState_t();

	auto responseHeaders = IStatusResponse_t::ResponseHeaders
	{
		{ "retry-after", { "456" } }
	};
	std::vector<StatusResponse_sp> responses =
	{
		std::make_shared<StatusResponse_t>(mLogger, "", 429, responseHeaders),
		std::make_shared<StatusResponse_t>(mLogger, "mp=1", 200, IStatusResponse_t::ResponseHeaders())
	};

	auto sessionWrapper1 = std::make_shared<SessionWrapper_t>(mMockSession1Open);
	auto sessionWrapper2 = std::make_shared<SessionWrapper_t>(mMockSession2Open);

	std::vector<SessionWrapper_sp> newSessions = { sessionWrapper1, sessionWrapper2 };

	ON_CALL(*mMockHttpClient, sendNewSessionRequest())
		.WillByDefault(testing::Invoke([&]() -> StatusResponse_sp
		{
			auto nextResponse = *responses.begin();
			if (nextResponse != nullptr)
			{
				responses.erase(responses.begin());
				return nextResponse;
			}
			return std::make_shared<StatusResponse_t>(mLogger, "", 200, IStatusResponse_t::ResponseHeaders());
		}));

	//only return the two new sessions for the new session request
	ON_CALL(*mMockContext, getAllNewSessions())
		.WillByDefault(testing::Return(newSessions));

	/// capture the beacon configuration set by the sendNewSessionRequests(...) method
	BeaconConfiguration_sp capturedBeaconConfigurationForSession1;
	BeaconConfiguration_sp capturedBeaconConfigurationForSession2;

	ON_CALL(*mMockSession1Open, setBeaconConfiguration(testing::_))
		.WillByDefault(testing::WithArgs<0>(testing::Invoke([&](BeaconConfiguration_sp beaconConfig) {
		capturedBeaconConfigurationForSession1 = beaconConfig;
	})));
	ON_CALL(*mMockSession2Open, setBeaconConfiguration(testing::_))
		.WillByDefault(testing::WithArgs<0>(testing::Invoke([&](BeaconConfiguration_sp beaconConfig) {
		capturedBeaconConfigurationForSession2 = beaconConfig;
	})));

	AbstractBeaconSendingState_sp savedNextState = nullptr;
	EXPECT_CALL(*mMockContext, setNextState(IsABeaconSendingCaptureOffState()))
		.Times(testing::Exactly(1))
		.WillOnce(testing::SaveArg<0>(&savedNextState));
	EXPECT_CALL(*mMockHttpClient, sendNewSessionRequest())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession3Finished, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mMockSession4Finished, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(0));

	// when calling execute
	target.execute(*mMockContext);

	// verify captured state
	ASSERT_NE(nullptr, savedNextState);
	ASSERT_EQ(int64_t(456 * 1000), std::static_pointer_cast<BeaconSendingCaptureOffState_t>(savedNextState)->getSleepTimeInMilliseconds());
}

TEST_F(BeaconSendingCaptureOnStateTest, finishedSessionsAreSent)
{
	// given
	auto target = BeaconSendingCaptureOnState_t();

	auto sessionWrapper1 = std::make_shared<SessionWrapper_t>(mMockSession3Finished);
	sessionWrapper1->updateBeaconConfiguration(std::make_shared<BeaconConfiguration_t>());
	auto sessionWrapper2 = std::make_shared<SessionWrapper_t>(mMockSession4Finished);
	sessionWrapper2->updateBeaconConfiguration(std::make_shared<BeaconConfiguration_t>());
	std::vector<SessionWrapper_sp> finishedSessions = { sessionWrapper1, sessionWrapper2 };

	//only return the two new sessions for the new session request
	ON_CALL(*mMockContext, getAllFinishedAndConfiguredSessions())
		.WillByDefault(testing::Return(finishedSessions));
	ON_CALL(*mMockContext, getAllNewSessions())
		.WillByDefault(testing::Return(std::vector<SessionWrapper_sp>()));
	ON_CALL(*mMockContext, getAllOpenAndConfiguredSessions())
		.WillByDefault(testing::Return(std::vector<SessionWrapper_sp>()));
	ON_CALL(*mMockContext, isCaptureOn())
		.WillByDefault(testing::Return(true));

	ON_CALL(*mMockSession3Finished, sendBeaconRawPtrProxy(testing::_))
		.WillByDefault(testing::Invoke([&](IHttpClientProvider_sp) -> StatusResponse_sp
		{
			return std::make_shared<StatusResponse_t>(mLogger, "", 200, IStatusResponse_t::ResponseHeaders());
		})
	);
	ON_CALL(*mMockSession4Finished, sendBeaconRawPtrProxy(testing::_))
		.WillByDefault(testing::Invoke([&](IHttpClientProvider_sp) -> StatusResponse_sp
		{
			return std::make_shared<StatusResponse_t>(mLogger, "", 200, IStatusResponse_t::ResponseHeaders());
		})
	);

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
	target.execute(*mMockContext);
}

TEST_F(BeaconSendingCaptureOnStateTest, unsuccessfulFinishedSessionsAreNotRemovedFromCache)
{
	// given
	auto target = BeaconSendingCaptureOnState_t();

	auto sessionWrapper1 = std::make_shared<SessionWrapper_t>(mMockSession3Finished);
	sessionWrapper1->updateBeaconConfiguration(std::make_shared<BeaconConfiguration_t>());
	auto sessionWrapper2 = std::make_shared<SessionWrapper_t>(mMockSession4Finished);
	sessionWrapper2->updateBeaconConfiguration(std::make_shared<BeaconConfiguration_t>());
	std::vector<SessionWrapper_sp> finishedSessions = { sessionWrapper1, sessionWrapper2 };

	//only return the two new sessions for the new session request
	ON_CALL(*mMockContext, getAllFinishedAndConfiguredSessions())
		.WillByDefault(testing::Return(finishedSessions));
	ON_CALL(*mMockContext, getAllNewSessions())
		.WillByDefault(testing::Return(std::vector<SessionWrapper_sp>()));
	ON_CALL(*mMockContext, getAllOpenAndConfiguredSessions())
		.WillByDefault(testing::Return(std::vector<SessionWrapper_sp>()));
	ON_CALL(*mMockContext, isCaptureOn())
		.WillByDefault(testing::Return(true));

	ON_CALL(*mMockSession3Finished, sendBeaconRawPtrProxy(testing::_))
		.WillByDefault(testing::Invoke([&](IHttpClientProvider_sp) -> StatusResponse_sp
		{
			return std::make_shared<StatusResponse_t>(mLogger, Utf8String_t(), 400, IStatusResponse_t::ResponseHeaders());
		})
	);
	ON_CALL(*mMockSession4Finished, sendBeaconRawPtrProxy(testing::_))
		.WillByDefault(testing::Invoke([&](IHttpClientProvider_sp) -> StatusResponse_sp
		{
			return std::make_shared<StatusResponse_t>(mLogger, Utf8String_t(), 400, IStatusResponse_t::ResponseHeaders());
		})
	);

	EXPECT_CALL(*mMockContext, getAllFinishedAndConfiguredSessions())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockContext, removeSession(testing::_))
		.Times(testing::Exactly(0));

	// when calling execute
	target.execute(*mMockContext);
}

TEST_F(BeaconSendingCaptureOnStateTest, sendingFinishedSessionsIsAbortedImmediatelyWhenTooManyRequestsResponseIsReceived)
{
	// given
	auto target = BeaconSendingCaptureOnState_t();

	auto sessionWrapper1 = std::make_shared<SessionWrapper_t>(mMockSession3Finished);
	sessionWrapper1->updateBeaconConfiguration(std::make_shared<BeaconConfiguration_t>());
	auto sessionWrapper2 = std::make_shared<SessionWrapper_t>(mMockSession4Finished);
	sessionWrapper2->updateBeaconConfiguration(std::make_shared<BeaconConfiguration_t>());
	std::vector<SessionWrapper_sp> finishedSessions = { sessionWrapper1, sessionWrapper2 };

	//only return the two new sessions for the new session request
	ON_CALL(*mMockContext, getAllFinishedAndConfiguredSessions())
		.WillByDefault(testing::Return(finishedSessions));
	ON_CALL(*mMockContext, getAllNewSessions())
		.WillByDefault(testing::Return(std::vector<SessionWrapper_sp>()));
	ON_CALL(*mMockContext, getAllOpenAndConfiguredSessions())
		.WillByDefault(testing::Return(std::vector<SessionWrapper_sp>()));
	ON_CALL(*mMockContext, isCaptureOn())
		.WillByDefault(testing::Return(true));

	ON_CALL(*mMockSession3Finished, sendBeaconRawPtrProxy(testing::_))
		.WillByDefault(testing::Invoke([&](IHttpClientProvider_sp) -> StatusResponse_sp {
			auto responseHeaders = IStatusResponse_t::ResponseHeaders
			{
				{ "retry-after", {"678"} }
			};
			return std::make_shared<StatusResponse_t>(mLogger, "", 429, responseHeaders);
		}));
	ON_CALL(*mMockSession4Finished, sendBeaconRawPtrProxy(testing::_))
		.WillByDefault(testing::Invoke([&](IHttpClientProvider_sp) -> StatusResponse_sp {
			return std::make_shared<StatusResponse_t>(mLogger, "", 200, IStatusResponse_t::ResponseHeaders());
		}));

	EXPECT_CALL(*mMockSession3Finished, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession4Finished, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mMockSession1Open, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mMockSession2Open, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mMockSession3Finished, clearCapturedData())
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mMockSession4Finished, clearCapturedData())
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mMockContext, removeSession(testing::_))
		.Times(testing::Exactly(0));
	std::shared_ptr<AbstractBeaconSendingState_t> savedNextState = nullptr;
	EXPECT_CALL(*mMockContext, setNextState(IsABeaconSendingCaptureOffState()))
		.Times(testing::Exactly(1))
		.WillOnce(testing::SaveArg<0>(&savedNextState));

	// when calling execute
	target.execute(*mMockContext);

	// verify captured state
	ASSERT_NE(nullptr, savedNextState);
	ASSERT_EQ(int64_t(678 * 1000), std::static_pointer_cast<BeaconSendingCaptureOffState_t>(savedNextState)->getSleepTimeInMilliseconds());
}

TEST_F(BeaconSendingCaptureOnStateTest, openSessionsAreSentIfSendIntervalIsExceeded)
{
	// given
	auto target = BeaconSendingCaptureOnState_t();

	auto sessionWrapper1 = std::make_shared<SessionWrapper_t>(mMockSession1Open);
	sessionWrapper1->updateBeaconConfiguration(std::make_shared<BeaconConfiguration_t>(2, DataCollectionLevel_t::USER_BEHAVIOR, openkit::CrashReportingLevel::OPT_IN_CRASHES));
	auto sessionWrapper2 = std::make_shared<SessionWrapper_t>(mMockSession2Open);
	sessionWrapper2->updateBeaconConfiguration(std::make_shared<BeaconConfiguration_t>(2, DataCollectionLevel_t::USER_BEHAVIOR, openkit::CrashReportingLevel::OPT_IN_CRASHES));
	std::vector<SessionWrapper_sp> openSessions = { sessionWrapper1, sessionWrapper2 };

	//only return the two new sessions for the new session request
	ON_CALL(*mMockContext, getAllFinishedAndConfiguredSessions())
		.WillByDefault(testing::Return(std::vector<SessionWrapper_sp>()));
	ON_CALL(*mMockContext, getAllNewSessions())
		.WillByDefault(testing::Return(std::vector<SessionWrapper_sp>()));
	ON_CALL(*mMockContext, getAllOpenAndConfiguredSessions())
		.WillByDefault(testing::Return(openSessions));
	ON_CALL(*mMockContext, isCaptureOn())
		.WillByDefault(testing::Return(true));

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
	target.execute(*mMockContext);
}

TEST_F(BeaconSendingCaptureOnStateTest, openSessionsAreNotSentIfSendIntervalIsNotExceeded)
{
	// given
	auto target = BeaconSendingCaptureOnState_t();

	auto sessionWrapper1 = std::make_shared<SessionWrapper_t>(mMockSession1Open);
	auto sessionWrapper2 = std::make_shared<SessionWrapper_t>(mMockSession2Open);
	std::vector<SessionWrapper_sp> openSessions = { sessionWrapper1, sessionWrapper2 };

	//only return the two new sessions for the new session request
	ON_CALL(*mMockContext, getAllFinishedAndConfiguredSessions())
		.WillByDefault(testing::Return(std::vector<SessionWrapper_sp>()));
	ON_CALL(*mMockContext, getAllNewSessions())
		.WillByDefault(testing::Return(std::vector<SessionWrapper_sp>()));
	ON_CALL(*mMockContext, getAllOpenAndConfiguredSessions())
		.WillByDefault(testing::Return(openSessions));
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

	// when calling execute
	target.execute(*mMockContext);
}

TEST_F(BeaconSendingCaptureOnStateTest, sendingOpenSessionsIsAbortedImmediatelyWhenTooManyRequestsResponseIsReceived)
{
	// given
	auto target = BeaconSendingCaptureOnState_t();

	auto sessionWrapper1 = std::make_shared<SessionWrapper_t>(mMockSession1Open);
	sessionWrapper1->updateBeaconConfiguration(std::make_shared<BeaconConfiguration_t>(2, DataCollectionLevel_t::USER_BEHAVIOR, openkit::CrashReportingLevel::OPT_IN_CRASHES));
	auto sessionWrapper2 = std::make_shared<SessionWrapper_t>(mMockSession2Open);
	sessionWrapper2->updateBeaconConfiguration(std::make_shared<BeaconConfiguration_t>(2, DataCollectionLevel_t::USER_BEHAVIOR, openkit::CrashReportingLevel::OPT_IN_CRASHES));
	std::vector<SessionWrapper_sp> openSessions = { sessionWrapper1, sessionWrapper2 };

	//only return the two new sessions for the new session request
	ON_CALL(*mMockContext, getAllFinishedAndConfiguredSessions())
		.WillByDefault(testing::Return(std::vector<SessionWrapper_sp>()));
	ON_CALL(*mMockContext, getAllNewSessions())
		.WillByDefault(testing::Return(std::vector<SessionWrapper_sp>()));
	ON_CALL(*mMockContext, getAllOpenAndConfiguredSessions())
		.WillByDefault(testing::Return(openSessions));
	ON_CALL(*mMockContext, isCaptureOn())
		.WillByDefault(testing::Return(true));

	ON_CALL(*mMockSession1Open, sendBeaconRawPtrProxy(testing::_))
		.WillByDefault(testing::Invoke([&](IHttpClientProvider_sp) -> StatusResponse_sp
	{
		auto responseHeaders = IStatusResponse_t::ResponseHeaders
		{
			{ "retry-after", {"678"} }
		};
		return std::make_shared<StatusResponse_t>(mLogger, "", 429, responseHeaders);
	}));
	ON_CALL(*mMockSession2Open, sendBeaconRawPtrProxy(testing::_))
		.WillByDefault(testing::Invoke([&](IHttpClientProvider_sp) -> StatusResponse_sp
	{
		return std::make_shared<StatusResponse_t>(mLogger, core::UTF8String(), 200, IStatusResponse_t::ResponseHeaders());
	}));

	ON_CALL(*mMockContext, getCurrentTimestamp())
		.WillByDefault(testing::Return(100));
	ON_CALL(*mMockContext, getSendInterval())
		.WillByDefault(testing::Return(50));
	ON_CALL(*mMockContext, getLastOpenSessionBeaconSendTime())
		.WillByDefault(testing::Return(45));

	EXPECT_CALL(*mMockSession1Open, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession2Open, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(0));

	std::shared_ptr<AbstractBeaconSendingState_t> savedNextState = nullptr;
	EXPECT_CALL(*mMockContext, setNextState(IsABeaconSendingCaptureOffState()))
		.Times(testing::Exactly(1))
		.WillOnce(testing::SaveArg<0>(&savedNextState));

	// when calling execute
	target.execute(*mMockContext);

	// verify captured state
	ASSERT_NE(nullptr, savedNextState);
	ASSERT_EQ(int64_t(678 * 1000), std::static_pointer_cast<BeaconSendingCaptureOffState_t>(savedNextState)->getSleepTimeInMilliseconds());
}

TEST_F(BeaconSendingCaptureOnStateTest, nothingIsSentIfStateIsInterruptedDuringSleep)
{
	// given
	auto target = BeaconSendingCaptureOnState_t();

	auto sessionWrapper1 = std::make_shared<SessionWrapper_t>(mMockSession1Open);
	std::vector<SessionWrapper_sp> newSessions = { sessionWrapper1 };
	auto sessionWrapper2 = std::make_shared<SessionWrapper_t>(mMockSession2Open);
	sessionWrapper2->updateBeaconConfiguration(std::make_shared<BeaconConfiguration_t>(2, DataCollectionLevel_t::USER_BEHAVIOR, openkit::CrashReportingLevel::OPT_IN_CRASHES));
	std::vector<SessionWrapper_sp> openSessions = { sessionWrapper2 };
	auto sessionWrapper3 = std::make_shared<SessionWrapper_t>(mMockSession3Finished);
	sessionWrapper3->updateBeaconConfiguration(std::make_shared<BeaconConfiguration_t>());
	std::vector<SessionWrapper_sp> finishedSessions = { sessionWrapper3 };

	//only return the two new sessions for the new session request
	ON_CALL(*mMockContext, getAllNewSessions())
		.WillByDefault(testing::Return(newSessions));
	ON_CALL(*mMockContext, getAllOpenAndConfiguredSessions())
		.WillByDefault(testing::Return(openSessions));
	ON_CALL(*mMockContext, getAllFinishedAndConfiguredSessions())
		.WillByDefault(testing::Return(finishedSessions));
	ON_CALL(*mMockContext, isCaptureOn())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mMockContext, getCurrentTimestamp())
		.WillByDefault(testing::Return(100));
	ON_CALL(*mMockContext, getSendInterval())
		.WillByDefault(testing::Return(50));
	ON_CALL(*mMockContext, getLastOpenSessionBeaconSendTime())
		.WillByDefault(testing::Return(45));
	ON_CALL(*mMockContext, isShutdownRequested())
		.WillByDefault(testing::Return(true));

	ON_CALL(*mMockSession1Open, sendBeaconRawPtrProxy(testing::_))
		.WillByDefault(testing::Invoke([&](IHttpClientProvider_sp) -> StatusResponse_sp
	{
		return std::make_shared<StatusResponse_t>(mLogger, core::UTF8String(), 200, IStatusResponse_t::ResponseHeaders());
	}));
	ON_CALL(*mMockSession2Open, sendBeaconRawPtrProxy(testing::_))
		.WillByDefault(testing::Invoke([&](IHttpClientProvider_sp) -> StatusResponse_sp
	{
		return std::make_shared<StatusResponse_t>(mLogger, core::UTF8String(), 200, IStatusResponse_t::ResponseHeaders());
	}));
	ON_CALL(*mMockSession3Finished, sendBeaconRawPtrProxy(testing::_))
		.WillByDefault(testing::Invoke([&](IHttpClientProvider_sp) -> StatusResponse_sp
	{
		return std::make_shared<StatusResponse_t>(mLogger, core::UTF8String(), 200, IStatusResponse_t::ResponseHeaders());
	}));

	EXPECT_CALL(*mMockHttpClient, sendNewSessionRequest())
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mMockSession1Open, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mMockSession2Open, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mMockSession3Finished, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(0));

	EXPECT_CALL(*mMockContext, setNextState(IsABeaconSendingFlushSessionsState()))
		.Times(testing::Exactly(1));

	// when calling execute
	target.execute(*mMockContext);
}

TEST_F(BeaconSendingCaptureOnStateTest, getStateNameReturnsCorrectStateName)
{
	// given
	auto target = BeaconSendingCaptureOnState_t();

	// when
	auto stateName = target.getStateName();

	// then
	ASSERT_STREQ(stateName, "CaptureOn");
}