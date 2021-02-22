/**
 * Copyright 2018-2021 Dynatrace LLC
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

#include "core/communication/IBeaconSendingContext.h"
#include "core/objects/SessionInternals.h"
#include "protocol/IHTTPClient.h"
#include "protocol/IStatusResponse.h"
#include "providers/IHTTPClientProvider.h"
#include "protocol/ResponseAttributes.h"

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

			ON_CALL(*this, getAllNotConfiguredSessions())
				.WillByDefault(testing::Return(std::vector<std::shared_ptr<core::objects::SessionInternals>>()));
			ON_CALL(*this, getAllOpenAndConfiguredSessions())
				.WillByDefault(testing::Return(std::vector<std::shared_ptr<core::objects::SessionInternals>>()));
			ON_CALL(*this, getAllFinishedAndConfiguredSessions())
				.WillByDefault(testing::Return(std::vector<std::shared_ptr<core::objects::SessionInternals>>()));

			ON_CALL(*this, getCurrentStateType())
				.WillByDefault(testing::Return(core::communication::IBeaconSendingState::StateType::BEACON_SENDING_COUNT));

			ON_CALL(*this, getLastResponseAttributes())
				.WillByDefault(testing::Return(protocol::ResponseAttributes::withUndefinedDefaults().build()));
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

		MOCK_METHOD(void, executeCurrentState, (), (override));

		MOCK_METHOD(void, requestShutdown, (), (override));

		MOCK_METHOD(bool, isShutdownRequested, (), (const, override));

		MOCK_METHOD(bool, waitForInit, (), (override));

		MOCK_METHOD(bool, waitForInit, (int64_t), (override));

		MOCK_METHOD(void, setInitCompleted, (bool), (override));

		MOCK_METHOD(bool, isInitialized, (), (const, override));

		MOCK_METHOD(bool, isInTerminalState, (), (const, override));

		MOCK_METHOD(bool, isCaptureOn, (), (const, override));

		MOCK_METHOD(std::shared_ptr<core::communication::IBeaconSendingState>, getCurrentState, (), (const, override));

		MOCK_METHOD(
			void,
			setNextState,
			(
				std::shared_ptr<core::communication::IBeaconSendingState>
			),
			(override)
		);

		MOCK_METHOD(std::shared_ptr<core::communication::IBeaconSendingState>, getNextState, (), (override));

		MOCK_METHOD(std::shared_ptr<providers::IHTTPClientProvider>, getHTTPClientProvider, (), (override));

		MOCK_METHOD(std::shared_ptr<protocol::IHTTPClient>, getHTTPClient, (), (override));

		MOCK_METHOD(int64_t, getCurrentTimestamp, (), (const, override));

		MOCK_METHOD(void, sleep, (), (override));

		MOCK_METHOD(void, sleep, (int64_t), (override));

		MOCK_METHOD(int64_t, getLastOpenSessionBeaconSendTime, (), (const, override));

		MOCK_METHOD(void, setLastOpenSessionBeaconSendTime, (int64_t), (override));

		MOCK_METHOD(int64_t, getLastStatusCheckTime, (), (const, override));

		MOCK_METHOD(void, setLastStatusCheckTime, (int64_t), (override));

		MOCK_METHOD(int64_t, getSendInterval, (), (const, override));

		MOCK_METHOD(void, disableCaptureAndClear, (), (override));

		MOCK_METHOD(void, handleStatusResponse, (std::shared_ptr<protocol::IStatusResponse>), (override));

		MOCK_METHOD(
			std::shared_ptr<protocol::IResponseAttributes>,
			updateFrom,
			(
				std::shared_ptr<protocol::IStatusResponse>
			),
			(override)
		);

		MOCK_METHOD(std::shared_ptr<protocol::IResponseAttributes>, getLastResponseAttributes, (), (const, override));

		MOCK_METHOD(std::shared_ptr<core::configuration::IServerConfiguration>, getLastServerConfiguration, (), (const, override));

		MOCK_METHOD(std::vector<std::shared_ptr<core::objects::SessionInternals>>, getAllNotConfiguredSessions, (), (override));

		MOCK_METHOD(std::vector<std::shared_ptr<core::objects::SessionInternals>>, getAllOpenAndConfiguredSessions, (), (override));

		MOCK_METHOD(std::vector<std::shared_ptr<core::objects::SessionInternals>>, getAllFinishedAndConfiguredSessions, (), (override));

		MOCK_METHOD(size_t, getSessionCount, (), (override));

		MOCK_METHOD(int32_t, getCurrentServerID, (), (const, override));

		MOCK_METHOD(void, addSession, (std::shared_ptr<core::objects::SessionInternals>), (override));

		MOCK_METHOD(bool, removeSession, (std::shared_ptr<core::objects::SessionInternals>), (override));

		MOCK_METHOD(core::communication::IBeaconSendingState::StateType, getCurrentStateType, (), (const, override));

		MOCK_METHOD(int64_t, getConfigurationTimestamp, (), (const, override));
	};
}
#endif
