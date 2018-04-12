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

#include "communication/BeaconSendingCaptureOffState.h"

#include <chrono>
#include <algorithm>
#include <memory>

#include "communication/BeaconSendingFlushSessionsState.h"
#include "communication/BeaconSendingCaptureOnState.h"
#include "communication/BeaconSendingTimeSyncState.h"
#include "communication/AbstractBeaconSendingState.h"
#include "communication/BeaconSendingContext.h"
#include "communication/BeaconSendingRequestUtil.h"
#include "protocol/StatusResponse.h"

using namespace communication;

/// number of retries for the status request
constexpr int32_t STATUS_REQUEST_RETRIES = 5;

const std::chrono::milliseconds BeaconSendingCaptureOffState::INITIAL_RETRY_SLEEP_TIME_MILLISECONDS(std::chrono::seconds(1));

const std::chrono::milliseconds STATUS_CHECK_INTERVAL_MILLISECONDS = std::chrono::hours(2);
const int64_t STATUS_CHECK_INTERVAL = STATUS_CHECK_INTERVAL_MILLISECONDS.count();

BeaconSendingCaptureOffState::BeaconSendingCaptureOffState()
	: AbstractBeaconSendingState(AbstractBeaconSendingState::StateType::BEACON_SENDING_CAPTURE_OFF_STATE)
{

}

void BeaconSendingCaptureOffState::doExecute(BeaconSendingContext& context)
{
	// disable capturing - avoid collecting further data
	context.disableCapture();

	auto currentTime = context.getCurrentTimestamp();

	auto delta = STATUS_CHECK_INTERVAL - (currentTime - context.getLastStatusCheckTime());
	if (delta > 0 && !context.isShutdownRequested()) {
		context.sleep(delta);
	}
	auto statusResponse = BeaconSendingRequestUtil::sendStatusRequest(context, STATUS_REQUEST_RETRIES, INITIAL_RETRY_SLEEP_TIME_MILLISECONDS.count());
	handleStatusResponse(context, std::move(statusResponse));

	// update the last status check time in any case
	context.setLastStatusCheckTime(currentTime);
}

std::shared_ptr<AbstractBeaconSendingState> BeaconSendingCaptureOffState::getShutdownState()
{
	return std::shared_ptr<AbstractBeaconSendingState>(new BeaconSendingFlushSessionsState());
}

const char* BeaconSendingCaptureOffState::getStateName() const
{
	return "CaptureOff state";
}

void BeaconSendingCaptureOffState::handleStatusResponse(BeaconSendingContext& context, std::unique_ptr<protocol::StatusResponse> statusResponse)
{
	bool statusReponseIsNull = statusResponse == nullptr;
	if (!statusReponseIsNull) {
		context.handleStatusResponse(std::move(statusResponse));
	}
	// if initial time sync failed before
	if (context.isTimeSyncSupported() && !context.isTimeSynced())
	{
		// then retry initial time sync
		context.setNextState(std::shared_ptr<AbstractBeaconSendingState>(new BeaconSendingTimeSyncState(true)));
	}
	else if (!statusReponseIsNull && context.isCaptureOn())
	{
		// capturing is re-enabled again, but only if we received a response from the server
		context.setNextState(std::shared_ptr<AbstractBeaconSendingState>(new BeaconSendingCaptureOnState()));
	}
}