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

#ifndef _TEST_COMMUNICATION_TESTBEACONSENDINGSTATE_H
#define _TEST_COMMUNICATION_TESTBEACONSENDINGSTATE_H

#include "communication/AbstractBeaconSendingState.h"
#include "communication/BeaconSendingTerminalState.h"

using namespace communication;

namespace test {

	class TestBeaconSendingState : public AbstractBeaconSendingState
	{
	public:
		virtual	~TestBeaconSendingState() {}

		virtual std::shared_ptr<AbstractBeaconSendingState> getShutdownState()
		{
			return std::make_shared<BeaconSendingTerminalState>();
		}

	protected:
		virtual void doExecute(BeaconSendingContext&)
		{
			// do nothing
		}
	};
}
#endif