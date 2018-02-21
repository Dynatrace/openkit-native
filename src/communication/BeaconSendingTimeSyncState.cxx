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

#include "BeaconSendingTimeSyncState.h"

#include "BeaconSendingContext.h"
#include "BeaconSendingTerminalState.h"

using namespace communication;

BeaconSendingTimeSyncState::BeaconSendingTimeSyncState()
	: BeaconSendingTimeSyncState(true)
{
}

BeaconSendingTimeSyncState::BeaconSendingTimeSyncState(bool initialSync)
	: AbstractBeaconSendingState(AbstractBeaconSendingState::StateType::BEACON_SENDING_TIMESYNC_STATE)
	, mInitialTimeSync(initialSync)
{
}

void BeaconSendingTimeSyncState::doExecute(BeaconSendingContext& context)
{

}

std::shared_ptr<AbstractBeaconSendingState> BeaconSendingTimeSyncState::getShutdownState()
{
	if (mInitialTimeSync)
	{
		return std::shared_ptr<AbstractBeaconSendingState>(new BeaconSendingTerminalState());
	}
	else
	{
		return std::shared_ptr<AbstractBeaconSendingState>(new BeaconSendingTerminalState());//TODO johannes.baeuerle - once available make transition to flush sessions state
		//return std::shared_ptr<AbstractBeaconSendingState>(new BeaconSendingFlushSessionsState());
	}
}

bool BeaconSendingTimeSyncState::isAShutdownState()
{
	return false;
}