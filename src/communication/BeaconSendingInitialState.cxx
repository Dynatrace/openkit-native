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

#include "BeaconSendingInitialState.h"

#include <stdio.h>
#include <stdint.h>

#include "communication/BeaconSendingTerminalState.h"
#include "communication/AbstractBeaconSendingState.h"

using namespace communication;

BeaconSendingInitialState::BeaconSendingInitialState()
{

}

BeaconSendingInitialState::~BeaconSendingInitialState()
{

}

void BeaconSendingInitialState::executeState(BeaconSendingContext& context)
{
	//TODO johannes.baeuerle implement actual initialize state

	if (context.isShutdownRequested())
	{
		context.setNextState(getShutdownState());
	}
}

AbstractBeaconSendingState* BeaconSendingInitialState::getShutdownState()
{
	AbstractBeaconSendingState* state = new BeaconSendingTerminalState();
	return state;
}

bool BeaconSendingInitialState::isAShutdownState()
{
	return false;
}