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

#ifndef _TEST_CORE_COMMUNICATION_MOCK_MOCKBEACONSENDINGCONTEXT_H
#define _TEST_CORE_COMMUNICATION_MOCK_MOCKBEACONSENDINGCONTEXT_H

#include "core/SessionWrapper.h"
#include "core/communication/IBeaconSendingContext.h"
#include "core/objects/SessionInternals.h"
#include "protocol/IHTTPClient.h"
#include "protocol/IStatusResponse.h"
#include "providers/IHTTPClientProvider.h"

#include "gmock/gmock.h"

#include <memory>
#include <vector>

namespace test
{
class MockIBeaconSendingContext
	: public core::communication::IBeaconSendingContext
{
	public:
		MockIBeaconSendingContext()
		{
			ON_CALL(*this, getCurrentState())
				.WillByDefault(testing::Return(nullptr));
			ON_CALL(*this, getNextState())
				.WillByDefault(testing::Return(nullptr));

			ON_CALL(*this, getHTTPClientProvider())
				.WillByDefault(testing::Return(nullptr));
			ON_CALL(*this, getHTTPClient())
				.WillByDefault(testing::Return(nullptr));

			ON_CALL(*this, getAllNewSessions())
				.WillByDefault(testing::Return(std::vector<std::shared_ptr<core::SessionWrapper>>()));
			ON_CALL(*this, getAllOpenAndConfiguredSessions())
				.WillByDefault(testing::Return(std::vector<std::shared_ptr<core::SessionWrapper>>()));
			ON_CALL(*this, getAllFinishedAndConfiguredSessions())
				.WillByDefault(testing::Return(std::vector<std::shared_ptr<core::SessionWrapper>>()));

			ON_CALL(*this, getCurrentStateType())
				.WillByDefault(testing::Return(core::communication::IBeaconSendingState::StateType::BEACON_SENDING_COUNT));
		}

		~MockIBeaconSendingContext() override = default;

		static std::shared_ptr<testing::NiceMock<MockIBeaconSendingContext>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockIBeaconSendingContext>>();
		}

		static std::shared_ptr<testing::StrictMock<MockIBeaconSendingContext>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockIBeaconSendingContext>>();
		}

		MOCK_METHOD0(executeCurrentState, void());

		MOCK_METHOD0(requestShutdown, void());

		MOCK_CONST_METHOD0(isShutdownRequested, bool());

		MOCK_METHOD0(waitForInit, bool());

		MOCK_METHOD1(waitForInit,
			bool(
				int64_t
			)
		);

		MOCK_METHOD1(setInitCompleted, void(bool));

		MOCK_CONST_METHOD0(isInitialized, bool());

		MOCK_CONST_METHOD0(isInTerminalState, bool());

		MOCK_CONST_METHOD0(isCaptureOn, bool());

		MOCK_CONST_METHOD0(getCurrentState, std::shared_ptr<core::communication::IBeaconSendingState>());

		MOCK_METHOD1(setNextState,
			void(
				std::shared_ptr<core::communication::IBeaconSendingState>
			)
		);

		MOCK_METHOD0(getNextState, std::shared_ptr<core::communication::IBeaconSendingState>());

		MOCK_METHOD0(getHTTPClientProvider, std::shared_ptr<providers::IHTTPClientProvider>());

		MOCK_METHOD0(getHTTPClient, std::shared_ptr<protocol::IHTTPClient>());

		MOCK_CONST_METHOD0(getCurrentTimestamp, int64_t());

		MOCK_METHOD0(sleep, void());

		MOCK_METHOD1(sleep,
			void(
				int64_t
			)
		);

		MOCK_CONST_METHOD0(getLastOpenSessionBeaconSendTime, int64_t());

		MOCK_METHOD1(setLastOpenSessionBeaconSendTime,
			void(
				int64_t
			)
		);

		MOCK_CONST_METHOD0(getLastStatusCheckTime, int64_t());

		MOCK_METHOD1(setLastStatusCheckTime,
			void(
				int64_t
			)
		);

		MOCK_CONST_METHOD0(getSendInterval, int64_t());

		MOCK_METHOD0(disableCapture, void());

		MOCK_METHOD1(handleStatusResponse,
			void(
				std::shared_ptr<protocol::IStatusResponse>
			)
		);

		MOCK_METHOD0(clearAllSessionData, void());

		MOCK_METHOD0(getAllNewSessions, std::vector<std::shared_ptr<core::SessionWrapper>>());

		MOCK_METHOD0(getAllOpenAndConfiguredSessions, std::vector<std::shared_ptr<core::SessionWrapper>>());

		MOCK_METHOD0(getAllFinishedAndConfiguredSessions, std::vector<std::shared_ptr<core::SessionWrapper>>());

		MOCK_METHOD1(startSession,
			void(
				std::shared_ptr<core::objects::SessionInternals>
			)
		);

		MOCK_METHOD1(finishSession,
			void(
				std::shared_ptr<core::objects::SessionInternals>
			)
		);

		MOCK_CONST_METHOD0(getCurrentStateType, core::communication::IBeaconSendingState::StateType());

		MOCK_METHOD1(removeSession,
			bool(
				std::shared_ptr<core::SessionWrapper>
			)
		);
	};
}
#endif
