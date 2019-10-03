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

#ifndef _TEST_CORE_COMMUNICATION_MOCKABSTRACTBEACONSENDINGSTATE_H
#define _TEST_CORE_COMMUNICATION_MOCKABSTRACTBEACONSENDINGSTATE_H

#include "Types.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

namespace test
{
class MockAbstractBeaconSendingState : public types::AbstractBeaconSendingState_t
	{
	public:
		MockAbstractBeaconSendingState()
			: AbstractBeaconSendingState(types::AbstractBeaconSendingState_t::StateType::BEACON_SENDING_COUNT)//no valid state
		{

		}
		virtual ~MockAbstractBeaconSendingState() {}
		virtual types::AbstractBeaconSendingState_sp getShutdownState() override
		{
			return types::AbstractBeaconSendingState_sp(nonCopyableReturnProxy());
		}

		virtual const char* getStateName() const override
		{
			return "MockState";
		}


		MOCK_METHOD0(nonCopyableReturnProxy, types::AbstractBeaconSendingState_t*());
		MOCK_CONST_METHOD0(isTerminalState, bool());
		MOCK_METHOD1(doExecute, void(types::BeaconSendingContext_t&));
		MOCK_METHOD1(execute, void(types::BeaconSendingContext_t&));

		void RealExecute(types::BeaconSendingContext_t& context)
		{
			return types::AbstractBeaconSendingState_t::execute(context);
		}
	};
}
#endif