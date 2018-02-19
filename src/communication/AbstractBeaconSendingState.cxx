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

#include "AbstractBeaconSendingState.h"
#include "communication/BeaconSendingContext.h"

using namespace communication;

AbstractBeaconSendingState::AbstractBeaconSendingState(StateType type)
	: mStateType(type)
{

}

AbstractBeaconSendingState::~AbstractBeaconSendingState()
{

}

void AbstractBeaconSendingState::execute(BeaconSendingContext& context)
{
	doExecute(context);

	if (context.isShutdownRequested()) {
		context.setNextState(getShutdownState());
	}
}

AbstractBeaconSendingState::StateType AbstractBeaconSendingState::getStateType()
{
	return mStateType;
}