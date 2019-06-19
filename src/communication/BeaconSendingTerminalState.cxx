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

#include "communication/BeaconSendingTerminalState.h"
#include "communication/BeaconSendingContext.h"

using namespace communication;

BeaconSendingTerminalState::BeaconSendingTerminalState()
	: AbstractBeaconSendingState(AbstractBeaconSendingState::StateType::BEACON_SENDING_TERMINAL_STATE)
{

}

BeaconSendingTerminalState::~BeaconSendingTerminalState()
{

}

void BeaconSendingTerminalState::doExecute(BeaconSendingContext& context)
{
	context.requestShutdown();
}

std::shared_ptr<AbstractBeaconSendingState> BeaconSendingTerminalState::getShutdownState()
{
	return std::shared_ptr<AbstractBeaconSendingState>(new BeaconSendingTerminalState());
}

const char* BeaconSendingTerminalState::getStateName() const
{
	return "Terminal";
}
