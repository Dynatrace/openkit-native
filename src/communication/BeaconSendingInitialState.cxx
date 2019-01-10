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

#include "BeaconSendingInitialState.h"

#include <chrono>
#include <algorithm>
#include <memory>

#include "communication/BeaconSendingTerminalState.h"
#include "communication/BeaconSendingTimeSyncState.h"
#include "communication/AbstractBeaconSendingState.h"
#include "communication/BeaconSendingRequestUtil.h"
#include "communication/BeaconSendingResponseUtil.h"
#include "communication/BeaconSendingContext.h"

#include "protocol/StatusResponse.h"
using namespace communication;

constexpr uint32_t MAX_INITIAL_STATUS_REQUEST_RETRIES = 5;

const std::vector<std::chrono::milliseconds> BeaconSendingInitialState::REINIT_DELAY_MILLISECONDS =
{
	std::chrono::minutes(1),
	std::chrono::minutes(5),
	std::chrono::minutes(15),
	std::chrono::hours(1),
	std::chrono::hours(2)
};

const std::chrono::milliseconds BeaconSendingInitialState::INITIAL_RETRY_SLEEP_TIME_MILLISECONDS(std::chrono::seconds(1));

BeaconSendingInitialState::BeaconSendingInitialState()
	: AbstractBeaconSendingState(AbstractBeaconSendingState::StateType::BEACON_SENDING_INIT_STATE)
	, mReinitializeDelayIndex(0)

{

}

void BeaconSendingInitialState::doExecute(BeaconSendingContext& context)
{
	/// execute the status request until we get a response
	auto statusResponse = executeStatusRequest(context);
	if (context.isShutdownRequested()) 
	{
		// shutdown was requested -> abort init with failure
		// transition to shutdown state is handled by base class
		context.setInitCompleted(false);
	}
	else if (BeaconSendingResponseUtil::isSuccessfulResponse(statusResponse)) 
	{
		// success -> continue with time sync
		context.handleStatusResponse(statusResponse);
		context.setNextState(std::shared_ptr<AbstractBeaconSendingState>(new BeaconSendingTimeSyncState(true)));
	}
}

std::shared_ptr<AbstractBeaconSendingState> BeaconSendingInitialState::getShutdownState()
{
	return std::shared_ptr<AbstractBeaconSendingState>(new BeaconSendingTerminalState());
}

const char* BeaconSendingInitialState::getStateName() const
{
	return "Initial";
}

std::shared_ptr<protocol::StatusResponse> BeaconSendingInitialState::executeStatusRequest(BeaconSendingContext& context)
{
	std::shared_ptr<protocol::StatusResponse> statusResponse = nullptr;
	while (!context.isShutdownRequested())
	{
		auto currentTimestamp = context.getCurrentTimestamp();
		context.setLastOpenSessionBeaconSendTime(currentTimestamp);
		context.setLastStatusCheckTime(currentTimestamp);

		statusResponse = BeaconSendingRequestUtil::sendStatusRequest(context, MAX_INITIAL_STATUS_REQUEST_RETRIES, INITIAL_RETRY_SLEEP_TIME_MILLISECONDS.count());
		if (BeaconSendingResponseUtil::isSuccessfulResponse(statusResponse))
		{
			// successful status response was received
			break;
		}

		int64_t sleepTime = REINIT_DELAY_MILLISECONDS[mReinitializeDelayIndex].count();
		if (BeaconSendingResponseUtil::isTooManyRequestsResponse(statusResponse))
		{
			// in case of too many requests the server might send us a retry-after
			sleepTime = statusResponse->getRetryAfterInMilliseconds();

			// also temporarily disable capturing to avoid further server overloading
			context.disableCapture();
		}

		// status request needs to be sent again after some delay
		context.sleep(sleepTime);

		mReinitializeDelayIndex = std::min(mReinitializeDelayIndex + 1, uint32_t(REINIT_DELAY_MILLISECONDS.size() - 1)); // ensure no out of bounds
	}

	return statusResponse;
}
