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

#ifndef _TEST_COMMUNICATION_MOCKABSTRACTBEACONSENDINGSTATE_H
#define _TEST_COMMUNICATION_MOCKABSTRACTBEACONSENDINGSTATE_H

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "communication/AbstractBeaconSendingState.h"
#include "communication/BeaconSendingContext.h"

#include "TestBeaconSendingState.h"

using namespace communication;

namespace test
{
	class MockAbstractBeaconSendingState : public AbstractBeaconSendingState
	{
	public:
		MockAbstractBeaconSendingState()
			: AbstractBeaconSendingState(communication::AbstractBeaconSendingState::StateType::BEACON_SENDING_COUNT)//no valid state
		{

		}
		virtual ~MockAbstractBeaconSendingState() {}
		virtual std::shared_ptr<AbstractBeaconSendingState> getShutdownState()
		{
			return std::shared_ptr<AbstractBeaconSendingState>(nonCopyableReturnProxy());
		}

		virtual const char* getStateName() const override
		{
			return "MockState";
		}


		MOCK_METHOD0(nonCopyableReturnProxy, AbstractBeaconSendingState*());
		MOCK_CONST_METHOD0(isTerminalState, bool());
		MOCK_METHOD1(doExecute, void(BeaconSendingContext&));
		MOCK_METHOD1(execute, void(BeaconSendingContext&));

		void RealExecute(BeaconSendingContext& context) { return AbstractBeaconSendingState::execute(context); }

	private:
		//TestBeaconSendingState _real_;
	};
}
#endif