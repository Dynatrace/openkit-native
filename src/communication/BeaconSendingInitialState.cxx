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

#include <chrono>
#include <algorithm>
#include <vector>

#include "communication/BeaconSendingTerminalState.h"
#include "communication/AbstractBeaconSendingState.h"
#include "communication/BeaconSendingRequestUtil.h"

#include "protocol/StatusResponse.h"
using namespace communication;

constexpr uint32_t MAX_INITIAL_STATUS_REQUEST_RETRIES = 5;
constexpr std::chrono::milliseconds INITIAL_RETRY_SLEEP_TIME_MILLISECONDS = std::chrono::seconds(1);

static const std::vector<std::chrono::milliseconds> REINIT_DELAY_MILLISECONDS = 
{
	std::chrono::minutes(1),
	std::chrono::minutes(5),
	std::chrono::minutes(15),
	std::chrono::hours(1),
	std::chrono::hours(2)
};

BeaconSendingInitialState::BeaconSendingInitialState()
	: mReinitializeDelayIndex(0)
{

}

void BeaconSendingInitialState::doExecute(BeaconSendingContext& context)
{
	std::unique_ptr<protocol::StatusResponse> statusResponse = nullptr;
	while (true) {
		auto currentTimestamp = context.getCurrentTimestamp();
		context.setLastOpenSessionBeaconSendTime(currentTimestamp);
		context.setLastStatusCheckTime(currentTimestamp);

		statusResponse = BeaconSendingRequestUtil::sendStatusRequest(context, MAX_INITIAL_STATUS_REQUEST_RETRIES, INITIAL_RETRY_SLEEP_TIME_MILLISECONDS.count());
		if (context.isShutdownRequested() || statusResponse != nullptr)
		{
			// shutdown was requested or a status response was received
			break;
		}

		// status request needs to be sent again after some delay
		context.sleep(REINIT_DELAY_MILLISECONDS[mReinitializeDelayIndex].count());

		mReinitializeDelayIndex = std::min(mReinitializeDelayIndex + 1, uint32_t(REINIT_DELAY_MILLISECONDS.size() - 1)); // ensure no out of bounds
	}

	if (context.isShutdownRequested()) 
	{
		// shutdown was requested -> abort init with failure
		// transition to shutdown state is handled by base class
		context.setInitCompleted(false);
	}
	else if (statusResponse != nullptr) 
	{
		// success -> continue with time sync
		context.handleStatusResponse(std::move(statusResponse));
		//context.setNextState(new BeaconSendingTimeSyncState(true));//not yet implemented //TODO johannes.baeuerle
	}
}

std::unique_ptr<AbstractBeaconSendingState> BeaconSendingInitialState::getShutdownState()
{
	return std::unique_ptr<AbstractBeaconSendingState>(new BeaconSendingTerminalState());
}

bool BeaconSendingInitialState::isAShutdownState()
{
	return false;
}