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

#include "communication/BeaconSendingTimeSyncState.h"
#include "communication/AbstractBeaconSendingState.h"
#include "protocol/ProtocolConstants.h"

#include "MockBeaconSendingContext.h"
#include "../protocol/MockHTTPClient.h"
#include "../communication/CustomMatchers.h"

class BeaconSendingTimeSyncTest : public testing::Test
{
public:

	BeaconSendingTimeSyncTest()
		: target(nullptr)
		, mockHTTPClient(nullptr)
	{
	}

	void SetUp()
	{
		target = std::shared_ptr<communication::AbstractBeaconSendingState>(new communication::BeaconSendingTimeSyncState(true));
		targetNotInitial = std::shared_ptr<communication::AbstractBeaconSendingState>(new communication::BeaconSendingTimeSyncState(false));
		std::shared_ptr<configuration::HTTPClientConfiguration> httpClientConfiguration = std::make_shared<configuration::HTTPClientConfiguration>(core::UTF8String(""),0, core::UTF8String(""));
		mockHTTPClient = std::shared_ptr<testing::NiceMock<test::MockHTTPClient>>(new testing::NiceMock<test::MockHTTPClient>(httpClientConfiguration));

		core::UTF8String response1 = core::UTF8String(protocol::RESPONSE_KEY_REQUEST_RECEIVE_TIME);
		response1.concatenate("=6&");
		response1.concatenate(protocol::RESPONSE_KEY_RESPONSE_SEND_TIME);
		response1.concatenate("=7");
		reponsesForASuccessfullTimeSync.push_back(new protocol::TimeSyncResponse(response1, 200));
		reponsesForASuccessfullTimeSyncWithRetries.push_back(nullptr);
		reponsesForASuccessfullTimeSyncWithRetries.push_back(new protocol::TimeSyncResponse(response1, 200));

		core::UTF8String response2 = core::UTF8String(protocol::RESPONSE_KEY_REQUEST_RECEIVE_TIME);
		response2.concatenate("=20&");
		response2.concatenate(protocol::RESPONSE_KEY_RESPONSE_SEND_TIME);
		response2.concatenate("=22");
		reponsesForASuccessfullTimeSync.push_back(new protocol::TimeSyncResponse(response2, 200));
		reponsesForASuccessfullTimeSyncWithRetries.push_back(nullptr);
		reponsesForASuccessfullTimeSyncWithRetries.push_back(nullptr);
		reponsesForASuccessfullTimeSyncWithRetries.push_back(new protocol::TimeSyncResponse(response2, 200));

		core::UTF8String response3 = core::UTF8String(protocol::RESPONSE_KEY_REQUEST_RECEIVE_TIME);
		response3.concatenate("=40&");
		response3.concatenate(protocol::RESPONSE_KEY_RESPONSE_SEND_TIME);
		response3.concatenate("=41");
		reponsesForASuccessfullTimeSync.push_back(new protocol::TimeSyncResponse(response3, 200));
		reponsesForASuccessfullTimeSyncWithRetries.push_back(nullptr);
		reponsesForASuccessfullTimeSyncWithRetries.push_back(nullptr);
		reponsesForASuccessfullTimeSyncWithRetries.push_back(nullptr);
		reponsesForASuccessfullTimeSyncWithRetries.push_back(new protocol::TimeSyncResponse(response3, 200));

		core::UTF8String response4 = core::UTF8String(protocol::RESPONSE_KEY_REQUEST_RECEIVE_TIME);
		response4.concatenate("=48&");
		response4.concatenate(protocol::RESPONSE_KEY_RESPONSE_SEND_TIME);
		response4.concatenate("=50");
		reponsesForASuccessfullTimeSync.push_back(new protocol::TimeSyncResponse(response4, 200));
		reponsesForASuccessfullTimeSyncWithRetries.push_back(nullptr);
		reponsesForASuccessfullTimeSyncWithRetries.push_back(nullptr);
		reponsesForASuccessfullTimeSyncWithRetries.push_back(nullptr);
		reponsesForASuccessfullTimeSyncWithRetries.push_back(nullptr);
		reponsesForASuccessfullTimeSyncWithRetries.push_back(new protocol::TimeSyncResponse(response4, 200));

		core::UTF8String response5 = core::UTF8String(protocol::RESPONSE_KEY_REQUEST_RECEIVE_TIME);
		response5.concatenate("=60&");
		response5.concatenate(protocol::RESPONSE_KEY_RESPONSE_SEND_TIME);
		response5.concatenate("=61");
		reponsesForASuccessfullTimeSync.push_back(new protocol::TimeSyncResponse(response5, 200));
		reponsesForASuccessfullTimeSyncWithRetries.push_back(new protocol::TimeSyncResponse(response5, 200));

		timestampsForASuccessfullTimeSync = { 2L, 8L, 10L, 23L, 32L, 42L, 44L, 52L, 54L, 62L, 66L };
		timestampsForASuccessfullTimeSyncWithRetries = { 2L, 8L, 2L, 8L,
														10L, 23L, 10L, 23L, 10L, 23L,
														32L, 42L, 32L, 42L, 32L, 42L, 32L, 42L,
														44L, 52L, 44L, 52L, 44L, 52L, 44L, 52L, 44L, 52L,
														54L, 62L, 66L };
	}

	void TearDown()
	{
		for (auto response : reponsesForASuccessfullTimeSync)
		{
			delete response;
		}

		for (auto response : reponsesForASuccessfullTimeSyncWithRetries)
		{
			delete response;
		}
	}

	std::shared_ptr<communication::AbstractBeaconSendingState> target;//time sync state with initialTimeSync set to true
	std::shared_ptr<communication::AbstractBeaconSendingState> targetNotInitial;//time sync state with initialTimeSync set to false
	std::shared_ptr<testing::NiceMock<test::MockHTTPClient>> mockHTTPClient;
	std::vector<protocol::TimeSyncResponse*> reponsesForASuccessfullTimeSync;
	std::vector<protocol::TimeSyncResponse*> reponsesForASuccessfullTimeSyncWithRetries;
	std::vector<uint64_t> timestampsForASuccessfullTimeSync;
	std::vector<uint64_t> timestampsForASuccessfullTimeSyncWithRetries;

};

TEST_F(BeaconSendingTimeSyncTest, timeSyncStateIsNotATerminalState)
{
	ASSERT_FALSE(target->isTerminalState());
}

TEST_F(BeaconSendingTimeSyncTest, getShutdownStateGivesATerminalStateInstanceForInitialTimeSync)
{
	// when
	std::shared_ptr<AbstractBeaconSendingState> obtained = target->getShutdownState();

	//then
	ASSERT_TRUE(obtained != nullptr);
	ASSERT_THAT(obtained, IsABeaconSendingTerminalState());
}

TEST_F(BeaconSendingTimeSyncTest, getShutdownStateGivesAFlushSessionsStateInstanceForNotInitialTimeSync)
{
	// when
	std::shared_ptr<AbstractBeaconSendingState> obtained = targetNotInitial->getShutdownState();

	//then
	ASSERT_TRUE(obtained != nullptr);
	ASSERT_THAT(obtained, IsABeaconSendingFlushSessionsState());
}

TEST_F(BeaconSendingTimeSyncTest, isTimeSyncRequiredReturnsFalseImmediatelyIfTimeSyncIsNotSupported)
{
	testing::NiceMock<test::MockBeaconSendingContext> mockContext;//NiceMock: ensure that required calls are there but do not object about other calls

    // given
	ON_CALL(mockContext, isTimeSyncSupported())
		.WillByDefault(testing::Return(false));
	ON_CALL(mockContext, getLastTimeSyncTime())
		.WillByDefault(testing::Return(-1));

	// when/then
	BeaconSendingTimeSyncState* underTest = static_cast<BeaconSendingTimeSyncState*>(target.get());
	ASSERT_FALSE(underTest->isTimeSyncRequired(mockContext));
}

TEST_F(BeaconSendingTimeSyncTest, timeSyncIsRequiredWhenLastTimeSyncTimeIsNegative)
{
	//given
	testing::NiceMock<test::MockBeaconSendingContext> mockContext;//NiceMock: ensure that required calls are there but do not object about other calls
	ON_CALL(mockContext, getLastTimeSyncTime())
		.WillByDefault(testing::Return(-1));
	ON_CALL(mockContext, isTimeSyncSupported())
		.WillByDefault(testing::Return(true));
	// when/then
	BeaconSendingTimeSyncState* underTest = static_cast<BeaconSendingTimeSyncState*>(target.get());
	ASSERT_TRUE(underTest->isTimeSyncRequired(mockContext));
}

TEST_F(BeaconSendingTimeSyncTest, isTimeSyncRequiredBoundaries)
{
	//given
	BeaconSendingTimeSyncState* underTest = static_cast<BeaconSendingTimeSyncState*>(target.get());

	testing::NiceMock<test::MockBeaconSendingContext> mockContext;//NiceMock: ensure that required calls are there but do not object about other calls
	ON_CALL(mockContext, isTimeSyncSupported())
		.WillByDefault(testing::Return(true));
	ON_CALL(mockContext, getLastTimeSyncTime())
		.WillByDefault(testing::Return(0));

	// when the last sync time is TIME_SYNC_INTERVAL_IN_MILLIS - 1 milliseconds ago
	ON_CALL(mockContext, getCurrentTimestamp())
		.WillByDefault(testing::Return(BeaconSendingTimeSyncState::TIME_SYNC_INTERVAL_IN_MILLIS.count() -1));

	 // then
	 ASSERT_FALSE(underTest->isTimeSyncRequired(mockContext));

	 // when the last sync time is TIME_SYNC_INTERVAL_IN_MILLIS milliseconds ago
	 ON_CALL(mockContext, getCurrentTimestamp())
		 .WillByDefault(testing::Return(BeaconSendingTimeSyncState::TIME_SYNC_INTERVAL_IN_MILLIS.count()));

	 // then
	 ASSERT_FALSE(underTest->isTimeSyncRequired(mockContext));

	 // when the last sync time is TIME_SYNC_INTERVAL_IN_MILLIS + 1 milliseconds ago
	 ON_CALL(mockContext, getCurrentTimestamp())
		 .WillByDefault(testing::Return(BeaconSendingTimeSyncState::TIME_SYNC_INTERVAL_IN_MILLIS.count() + 1));

	 // then
	 ASSERT_TRUE(underTest->isTimeSyncRequired(mockContext));
}

TEST_F(BeaconSendingTimeSyncTest, timeSyncNotRequiredAndCaptureOnTruePerformsStateTransitionToCaptureOnState)
{
	// given
	testing::NiceMock<test::MockBeaconSendingContext> mockContext;//NiceMock: ensure that required calls are there but do not object about other calls
	ON_CALL(mockContext, isTimeSyncSupported())
		.WillByDefault(testing::Return(false));
	ON_CALL(mockContext, isCaptureOn())
		.WillByDefault(testing::Return(true));

	// then
	EXPECT_CALL(mockContext, setNextState(IsABeaconSendingCaptureOnState()))
		.Times(::testing::Exactly(1));

	// when
	target->execute(mockContext);
}

TEST_F(BeaconSendingTimeSyncTest, timeSyncRequestsAreInterruptedAfterUnsuccessfulRetries)
{
	// given
	testing::NiceMock<test::MockBeaconSendingContext> mockContext;//NiceMock: ensure that required calls are there but do not object about other calls
	ON_CALL(mockContext, getLastTimeSyncTime())
		.WillByDefault(testing::Return(-1));
	ON_CALL(mockContext, isTimeSyncSupported())
		.WillByDefault(testing::Return(true));
	ON_CALL(mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mockHTTPClient));
	ON_CALL(*mockHTTPClient, sendTimeSyncRequestRawPtrProxy())
		.WillByDefault(testing::Return(nullptr));

	
	// then
	EXPECT_CALL(*mockHTTPClient, sendTimeSyncRequestRawPtrProxy())
		.Times(testing::Exactly(communication::BeaconSendingTimeSyncState::REQUIRED_TIME_SYNC_REQUESTS + 1));

	// when
	target->execute(mockContext);

}

TEST_F(BeaconSendingTimeSyncTest, sleepTimeDoublesBetweenConsecutiveTimeSyncRequests)
{
	// given
	testing::NiceMock<test::MockBeaconSendingContext> mockContext;//NiceMock: ensure that required calls are there but do not object about other calls
	ON_CALL(mockContext, getLastTimeSyncTime())
		.WillByDefault(testing::Return(-1));
	ON_CALL(mockContext, isTimeSyncSupported())
		.WillByDefault(testing::Return(true));
	ON_CALL(mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mockHTTPClient));
	ON_CALL(*mockHTTPClient, sendTimeSyncRequestRawPtrProxy())
		.WillByDefault(testing::Return(nullptr));

	// then
	testing::InSequence s;

	uint64_t initialSleep = communication::BeaconSendingTimeSyncState::INITIAL_RETRY_SLEEP_TIME_MILLISECONDS.count();

	// then
	// from first round
	EXPECT_CALL(mockContext, sleep(initialSleep))
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, sleep(initialSleep * 2))
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, sleep(initialSleep * 4))
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, sleep(initialSleep * 8))
		.Times(::testing::Exactly(1));
	EXPECT_CALL(mockContext, sleep(initialSleep * 16))
		.Times(::testing::Exactly(1));

	// when
	target->execute(mockContext);
}

TEST_F(BeaconSendingTimeSyncTest, sleepTimeIsResetToInitialValueAfterASuccessfulTimeSyncResponse)
{
	testing::NiceMock<test::MockBeaconSendingContext> mockContext;//NiceMock: ensure that required calls are there but do not object about other calls
	ON_CALL(mockContext, getLastTimeSyncTime())
		.WillByDefault(testing::Return(-1));
	ON_CALL(mockContext, isTimeSyncSupported())
		.WillByDefault(testing::Return(true));
	ON_CALL(mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mockHTTPClient));
	ON_CALL(*mockHTTPClient, sendTimeSyncRequestRawPtrProxy())
		.WillByDefault(testing::Return(nullptr));
	// given
	std::vector<protocol::TimeSyncResponse*>& responses = reponsesForASuccessfullTimeSyncWithRetries;
	ON_CALL(*mockHTTPClient, sendTimeSyncRequestRawPtrProxy())
		.WillByDefault(testing::Invoke([&responses]() -> protocol::TimeSyncResponse* {
		if (responses.size() > 0)
		{
			auto response = *responses.begin();
			responses.erase(responses.begin());
			return response;
		}
		else
		{
			return nullptr;
		}
	}));

	std::vector<uint64_t>& timestamps = timestampsForASuccessfullTimeSyncWithRetries;
	//given
	ON_CALL(mockContext, getCurrentTimestamp())
		.WillByDefault(testing::Invoke(
			[&timestamps]() -> uint64_t {
		if (timestamps.size() > 0)
		{
			auto time = *timestamps.begin();
			timestamps.erase(timestamps.begin());
			return time;
		}
		else
		{
			return 0;
		}
	}));

	testing::Sequence s;
	uint64_t initialRetrySleepTime = communication::BeaconSendingTimeSyncState::INITIAL_RETRY_SLEEP_TIME_MILLISECONDS.count();

	// then verify init was done
	EXPECT_CALL(mockContext, initializeTimeSync(2L, true))
		.Times(testing::Exactly(1));

	// and verify method calls
	EXPECT_CALL(mockContext, sleep(initialRetrySleepTime))
		.Times(testing::Exactly(1))
		.InSequence(s);
	// second time sync request -> 2 retries
	EXPECT_CALL(mockContext, sleep(initialRetrySleepTime))
		.Times(testing::Exactly(1))
		.InSequence(s);
	EXPECT_CALL(mockContext, sleep(initialRetrySleepTime * 2))
		.Times(testing::Exactly(1))
		.InSequence(s);
	// third time sync request -> 3 retries
	EXPECT_CALL(mockContext, sleep(initialRetrySleepTime))
		.Times(testing::Exactly(1))
		.InSequence(s);
	EXPECT_CALL(mockContext, sleep(initialRetrySleepTime * 2))
		.Times(testing::Exactly(1))
		.InSequence(s);
	EXPECT_CALL(mockContext, sleep(initialRetrySleepTime * 4))
		.Times(testing::Exactly(1))
		.InSequence(s);
	// fourth time sync request -> 4 retries
	EXPECT_CALL(mockContext, sleep(initialRetrySleepTime))
		.Times(testing::Exactly(1))
		.InSequence(s);
	EXPECT_CALL(mockContext, sleep(initialRetrySleepTime * 2))
		.Times(testing::Exactly(1))
		.InSequence(s);
	EXPECT_CALL(mockContext, sleep(initialRetrySleepTime * 4))
		.Times(testing::Exactly(1))
		.InSequence(s);
	EXPECT_CALL(mockContext, sleep(initialRetrySleepTime * 8))
		.Times(testing::Exactly(1))
		.InSequence(s);
	// fifth time sync request -> 0 retries
	EXPECT_CALL(mockContext, setLastTimeSyncTime(66L))
		.Times(testing::Exactly(1));
	EXPECT_CALL(mockContext, setInitCompleted(testing::_))
		.Times(testing::Exactly(1));


	// when
	target->execute(mockContext);
}


TEST_F(BeaconSendingTimeSyncTest, successfulTimeSyncInitializesTimeProvider)
{
	// given
	testing::NiceMock<test::MockBeaconSendingContext> mockContext;//NiceMock: ensure that required calls are there but do not object about other calls
	ON_CALL(mockContext, getLastTimeSyncTime())
		.WillByDefault(testing::Return(-1));
	ON_CALL(mockContext, isTimeSyncSupported())
		.WillByDefault(testing::Return(true));
	ON_CALL(mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mockHTTPClient));

	std::vector<protocol::TimeSyncResponse*>& responses = reponsesForASuccessfullTimeSync;
	ON_CALL(*mockHTTPClient, sendTimeSyncRequestRawPtrProxy())
		.WillByDefault(testing::Invoke([&responses]() -> protocol::TimeSyncResponse* {
			if (responses.size() > 0)
			{
				auto response = *responses.begin();
				responses.erase(responses.begin());
				return response;
			}
			else
			{
				return nullptr;
			}
		}));

	std::vector<uint64_t>& timestamps = timestampsForASuccessfullTimeSync;
	//given
	ON_CALL(mockContext, getCurrentTimestamp())
		.WillByDefault(testing::Invoke(
			[&timestamps]() -> uint64_t {
			if (timestamps.size() > 0)
			{
				auto time = *timestamps.begin();
				timestamps.erase(timestamps.begin());
				return time;
			}
			else
			{
				return 0;
			}
	}));


	// verify init was done
	EXPECT_CALL(mockContext, initializeTimeSync(2L, true))
		.Times(testing::Exactly(1));

	// verify number of method calls
	uint32_t numberOfTimeSyncRequests = communication::BeaconSendingTimeSyncState::REQUIRED_TIME_SYNC_REQUESTS;
	EXPECT_CALL(mockContext, getHTTPClient())
		.Times(testing::Exactly(numberOfTimeSyncRequests));
	EXPECT_CALL(*mockHTTPClient, sendTimeSyncRequestRawPtrProxy())
		.Times(testing::Exactly(numberOfTimeSyncRequests));
	EXPECT_CALL(mockContext, getCurrentTimestamp())
		.Times(testing::Exactly(numberOfTimeSyncRequests * 2 + 1));

	EXPECT_CALL(mockContext, setLastTimeSyncTime(66L))
		.Times(testing::Exactly(1));
	//if initCompleted is called on mockContext this results in a failure because this is not an initial time sync and the mock
	// is a StrictMock
	EXPECT_CALL(mockContext, setInitCompleted(testing::_))
		.Times(testing::Exactly(0));

	// when
	targetNotInitial->execute(mockContext);
}

TEST_F(BeaconSendingTimeSyncTest, successfulTimeSyncSetSuccessfulInitCompletionInContextWhenItIsInitialTimeSync)
{
	// given
	testing::NiceMock<test::MockBeaconSendingContext> mockContext;//NiceMock: ensure that required calls are there but do not object about other calls
	ON_CALL(mockContext, getLastTimeSyncTime())
		.WillByDefault(testing::Return(-1));
	ON_CALL(mockContext, isTimeSyncSupported())
		.WillByDefault(testing::Return(true));
	ON_CALL(mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mockHTTPClient));
	ON_CALL(*mockHTTPClient, sendTimeSyncRequestRawPtrProxy())
		.WillByDefault(testing::Return(nullptr));

	std::vector<protocol::TimeSyncResponse*>& responses = reponsesForASuccessfullTimeSync;
	ON_CALL(*mockHTTPClient, sendTimeSyncRequestRawPtrProxy())
		.WillByDefault(testing::Invoke([&responses]() -> protocol::TimeSyncResponse* {
		if (responses.size() > 0)
		{
			auto response = *responses.begin();
			responses.erase(responses.begin());
			return response;
		}
		else
		{
			return nullptr;
		}
	}));

	std::vector<uint64_t>& timestamps = timestampsForASuccessfullTimeSync;
	//given
	ON_CALL(mockContext, getCurrentTimestamp())
		.WillByDefault(testing::Invoke(
			[&timestamps]() -> uint64_t {
		if (timestamps.size() > 0)
		{
			auto time = *timestamps.begin();
			timestamps.erase(timestamps.begin());
			return time;
		}
		else
		{
			return 0;
		}
	}));


	// verify init was done
	EXPECT_CALL(mockContext, initializeTimeSync(2L, true))
		.Times(testing::Exactly(1));

	// verify number of method calls
	uint32_t numberOfTimeSyncRequests = communication::BeaconSendingTimeSyncState::REQUIRED_TIME_SYNC_REQUESTS;
	EXPECT_CALL(mockContext, getHTTPClient())
		.Times(testing::Exactly(numberOfTimeSyncRequests));
	EXPECT_CALL(*mockHTTPClient, sendTimeSyncRequestRawPtrProxy())
		.Times(testing::Exactly(numberOfTimeSyncRequests));
	EXPECT_CALL(mockContext, getCurrentTimestamp())
		.Times(testing::Exactly(numberOfTimeSyncRequests * 2 + 1));
	EXPECT_CALL(mockContext, setLastTimeSyncTime(66L))
		.Times(testing::Exactly(1));
	EXPECT_CALL(mockContext, setInitCompleted(true))
		.Times(testing::Exactly(1));

	// when
	target->execute(mockContext);
}

TEST_F(BeaconSendingTimeSyncTest, timeSyncSupportIsDisabledIfBothTimeStampsInTimeSyncResponseAreNegative)
{
	//given
	testing::NiceMock<test::MockBeaconSendingContext> mockContext;//NiceMock: ensure that required calls are there but do not object about other calls
	core::UTF8String disableString = core::UTF8String(protocol::RESPONSE_KEY_REQUEST_RECEIVE_TIME);
	disableString.concatenate("=-1&");
	disableString.concatenate(protocol::RESPONSE_KEY_RESPONSE_SEND_TIME);
	disableString.concatenate("=-2");
	protocol::TimeSyncResponse* disable = new protocol::TimeSyncResponse(disableString, 200);

	ON_CALL(mockContext, getLastTimeSyncTime())
		.WillByDefault(testing::Return(-1));
	ON_CALL(mockContext, isTimeSyncSupported())
		.WillByDefault(testing::Return(true));
	ON_CALL(mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mockHTTPClient));
	ON_CALL(*mockHTTPClient, sendTimeSyncRequestRawPtrProxy())
		.WillByDefault(testing::Return(disable));


	// then verify that time sync was disabled
	EXPECT_CALL(mockContext, disableTimeSyncSupport())
		.Times(testing::Exactly(1));

	// when
	target->execute(mockContext);
}

TEST_F(BeaconSendingTimeSyncTest, timeSyncSupportIsDisabledIfFirstTimeStampInTimeSyncResponseIsNegative)
{
	//given
	testing::NiceMock<test::MockBeaconSendingContext> mockContext;//NiceMock: ensure that required calls are there but do not object about other calls
	core::UTF8String disableString = core::UTF8String(protocol::RESPONSE_KEY_REQUEST_RECEIVE_TIME);
	disableString.concatenate("=-1&");
	disableString.concatenate(protocol::RESPONSE_KEY_RESPONSE_SEND_TIME);
	disableString.concatenate("=7");
	protocol::TimeSyncResponse* disable = new protocol::TimeSyncResponse(disableString, 200);

	ON_CALL(mockContext, getLastTimeSyncTime())
		.WillByDefault(testing::Return(-1));
	ON_CALL(mockContext, isTimeSyncSupported())
		.WillByDefault(testing::Return(true));
	ON_CALL(mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mockHTTPClient));
	ON_CALL(*mockHTTPClient, sendTimeSyncRequestRawPtrProxy())
		.WillByDefault(testing::Return(disable));


	// then verify that time sync was disabled
	EXPECT_CALL(mockContext, disableTimeSyncSupport())
		.Times(testing::Exactly(1));

	// when
	target->execute(mockContext);
}

TEST_F(BeaconSendingTimeSyncTest, timeSyncSupportIsDisabledIfSecondTimeStampInTimeSyncResponseIsNegative)
{
	//given
	testing::NiceMock<test::MockBeaconSendingContext> mockContext;//NiceMock: ensure that required calls are there but do not object about other calls
	core::UTF8String disableString = core::UTF8String(protocol::RESPONSE_KEY_REQUEST_RECEIVE_TIME);
	disableString.concatenate("=1&");
	disableString.concatenate(protocol::RESPONSE_KEY_RESPONSE_SEND_TIME);
	disableString.concatenate("=-1");
	protocol::TimeSyncResponse* disable = new protocol::TimeSyncResponse(disableString, 200);

	ON_CALL(mockContext, getLastTimeSyncTime())
		.WillByDefault(testing::Return(-1));
	ON_CALL(mockContext, isTimeSyncSupported())
		.WillByDefault(testing::Return(true));
	ON_CALL(mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mockHTTPClient));
	ON_CALL(*mockHTTPClient, sendTimeSyncRequestRawPtrProxy())
		.WillByDefault(testing::Return(disable));


	// then verify that time sync was disabled
	EXPECT_CALL(mockContext, disableTimeSyncSupport())
		.Times(testing::Exactly(1));

	// when
	target->execute(mockContext);
}

TEST_F(BeaconSendingTimeSyncTest, timeProviderInitializeIsCalledIfItIsAnInitialTimeSyncEvenWhenResponseIsErroneous)
{
	// given
	testing::NiceMock<test::MockBeaconSendingContext> mockContext;//NiceMock: ensure that required calls are there but do not object about other calls
	ON_CALL(mockContext, getLastTimeSyncTime())
		.WillByDefault(testing::Return(-1));
	ON_CALL(mockContext, isTimeSyncSupported())
		.WillByDefault(testing::Return(true));
	ON_CALL(mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mockHTTPClient));
	ON_CALL(*mockHTTPClient, sendTimeSyncRequestRawPtrProxy())
		.WillByDefault(testing::Return(nullptr));


	// then
	EXPECT_CALL(mockContext, initializeTimeSync(0L, true))
		.Times(testing::Exactly(1));

	// when
	target->execute(mockContext);
}

TEST_F(BeaconSendingTimeSyncTest, stateTransitionToCaptureOffIsPerformedIfTimeSyncIsSupportedButFailed)
{
	// given
	testing::NiceMock<test::MockBeaconSendingContext> mockContext;//NiceMock: ensure that required calls are there but do not object about other calls
	ON_CALL(mockContext, getLastTimeSyncTime())
		.WillByDefault(testing::Return(-1));
	ON_CALL(mockContext, isTimeSyncSupported())
		.WillByDefault(testing::Return(true));
	ON_CALL(mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mockHTTPClient));
	ON_CALL(*mockHTTPClient, sendTimeSyncRequestRawPtrProxy())
		.WillByDefault(testing::Return(nullptr));

	// then
	EXPECT_CALL(mockContext, setNextState(IsABeaconSendingCaptureOffState()))
		.Times(testing::Exactly(1));

	// when
	target->execute(mockContext);
}

TEST_F(BeaconSendingTimeSyncTest, stateTransitionIsPerformedToAppropriateStateIfTimeSyncIsSupportedAndCapturingIsEnabled)
{
	// given
	testing::NiceMock<test::MockBeaconSendingContext> mockContext;//NiceMock: ensure that required calls are there but do not object about other calls
	ON_CALL(mockContext, getLastTimeSyncTime())
		.WillByDefault(testing::Return(-1));
	ON_CALL(mockContext, isTimeSyncSupported())
		.WillByDefault(testing::Return(true));
	ON_CALL(mockContext, isShutdownRequested())
		.WillByDefault(testing::Return(false));
	ON_CALL(mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mockHTTPClient));
	ON_CALL(*mockHTTPClient, sendTimeSyncRequestRawPtrProxy())
		.WillByDefault(testing::Return(nullptr));
	ON_CALL(mockContext, isCaptureOn())
		.WillByDefault(testing::Return(true));

	std::vector<protocol::TimeSyncResponse*>& responses = reponsesForASuccessfullTimeSync;
	ON_CALL(*mockHTTPClient, sendTimeSyncRequestRawPtrProxy())
		.WillByDefault(testing::Invoke([&responses]() -> protocol::TimeSyncResponse* {
		if (responses.size() > 0)
		{
			auto response = *responses.begin();
			responses.erase(responses.begin());
			return response;
		}
		else
		{
			return nullptr;
		}
	}));

	std::vector<uint64_t>& timestamps = timestampsForASuccessfullTimeSync;
	//given
	ON_CALL(mockContext, getCurrentTimestamp())
		.WillByDefault(testing::Invoke(
			[&timestamps]() -> uint64_t {
		if (timestamps.size() > 0)
		{
			auto time = *timestamps.begin();
			timestamps.erase(timestamps.begin());
			return time;
		}
		else
		{
			return 0;
		}
	}));


	// verify init was done
	EXPECT_CALL(mockContext, setNextState(IsABeaconSendingCaptureOnState()));

	// when
	target->execute(mockContext);
}

TEST_F(BeaconSendingTimeSyncTest, stateTransitionIsPerformedToAppropriateStateIfTimeSyncIsSupportedAndCapturingIsDisabled)
{
	// given
	testing::NiceMock<test::MockBeaconSendingContext> mockContext;//NiceMock: ensure that required calls are there but do not object about other calls
	ON_CALL(mockContext, getLastTimeSyncTime())
		.WillByDefault(testing::Return(-1));
	ON_CALL(mockContext, isTimeSyncSupported())
		.WillByDefault(testing::Return(true));
	ON_CALL(mockContext, isShutdownRequested())
		.WillByDefault(testing::Return(false));
	ON_CALL(mockContext, getHTTPClient())
		.WillByDefault(testing::Return(mockHTTPClient));
	ON_CALL(*mockHTTPClient, sendTimeSyncRequestRawPtrProxy())
		.WillByDefault(testing::Return(nullptr));
	ON_CALL(mockContext, isCaptureOn())
		.WillByDefault(testing::Return(false));

	std::vector<protocol::TimeSyncResponse*>& responses = reponsesForASuccessfullTimeSync;
	ON_CALL(*mockHTTPClient, sendTimeSyncRequestRawPtrProxy())
		.WillByDefault(testing::Invoke([&responses]() -> protocol::TimeSyncResponse* {
		if (responses.size() > 0)
		{
			auto response = *responses.begin();
			responses.erase(responses.begin());
			return response;
		}
		else
		{
			return nullptr;
		}
	}));

	std::vector<uint64_t>& timestamps = timestampsForASuccessfullTimeSync;
	//given
	ON_CALL(mockContext, getCurrentTimestamp())
		.WillByDefault(testing::Invoke(
			[&timestamps]() -> uint64_t {
		if (timestamps.size() > 0)
		{
			auto time = *timestamps.begin();
			timestamps.erase(timestamps.begin());
			return time;
		}
		else
		{
			return 0;
		}
	}));


	// verify init was done
	EXPECT_CALL(mockContext, setNextState(IsABeaconSendingCaptureOffState()));

	// when
	target->execute(mockContext);
}
