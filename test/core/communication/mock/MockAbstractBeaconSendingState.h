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

#ifndef _TEST_CORE_COMMUNICATION_MOCK_MOCKABSTRACTBEACONSENDINGSTATE_H
#define _TEST_CORE_COMMUNICATION_MOCK_MOCKABSTRACTBEACONSENDINGSTATE_H

#include "core/communication/AbstractBeaconSendingState.h"
#include "core/communication/IBeaconSendingState.h"

#include "gmock/gmock.h"

namespace test
{
class MockAbstractBeaconSendingState
	: public core::communication::AbstractBeaconSendingState
{
	public:
		MockAbstractBeaconSendingState()
			: AbstractBeaconSendingState(core::communication::IBeaconSendingState::StateType::BEACON_SENDING_COUNT)//no valid state
		{
			ON_CALL(*this, getShutdownState())
				.WillByDefault(testing::Return(nullptr));
		}

		~MockAbstractBeaconSendingState() override = default;

		static std::shared_ptr<testing::NiceMock<MockAbstractBeaconSendingState>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockAbstractBeaconSendingState>>();
		}

		static std::shared_ptr<testing::StrictMock<MockAbstractBeaconSendingState>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockAbstractBeaconSendingState>>();
		}

		const char* getStateName() const override
		{
			return "MockState";
		}

		MOCK_METHOD(std::shared_ptr<core::communication::IBeaconSendingState>, getShutdownState, (), (override));

		MOCK_METHOD(bool, isTerminalState, (), (const, override));

		MOCK_METHOD(void, doExecute, (core::communication::IBeaconSendingContext&), (override));
	};
}
#endif