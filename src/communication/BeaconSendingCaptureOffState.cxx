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

#include "communication/BeaconSendingCaptureOffState.h"

#include <chrono>
#include <algorithm>
#include <memory>

#include "communication/BeaconSendingFlushSessionsState.h"
#include "communication/BeaconSendingCaptureOnState.h"
#include "communication/AbstractBeaconSendingState.h"
#include "communication/BeaconSendingContext.h"
#include "communication/BeaconSendingRequestUtil.h"
#include "communication/BeaconSendingResponseUtil.h"
#include "protocol/StatusResponse.h"

using namespace communication;

/// number of retries for the status request
constexpr int32_t STATUS_REQUEST_RETRIES = 5;

const std::chrono::milliseconds BeaconSendingCaptureOffState::INITIAL_RETRY_SLEEP_TIME_MILLISECONDS(std::chrono::seconds(1));

const std::chrono::milliseconds STATUS_CHECK_INTERVAL_MILLISECONDS = std::chrono::hours(2);
const int64_t STATUS_CHECK_INTERVAL = STATUS_CHECK_INTERVAL_MILLISECONDS.count();

BeaconSendingCaptureOffState::BeaconSendingCaptureOffState()
	: BeaconSendingCaptureOffState(int64_t(-1))
{
}

BeaconSendingCaptureOffState::BeaconSendingCaptureOffState(int64_t sleepTimeInMilliseconds)
	: AbstractBeaconSendingState(AbstractBeaconSendingState::StateType::BEACON_SENDING_CAPTURE_OFF_STATE)
	, mSleepTimeInMilliseconds(sleepTimeInMilliseconds)
{
}

void BeaconSendingCaptureOffState::doExecute(BeaconSendingContext& context)
{
	// disable capturing - avoid collecting further data
	context.disableCapture();

	auto currentTime = context.getCurrentTimestamp();

	auto delta = mSleepTimeInMilliseconds > int64_t(0)
		? mSleepTimeInMilliseconds 
		: STATUS_CHECK_INTERVAL - (currentTime - context.getLastStatusCheckTime());
	if (delta > 0 && !context.isShutdownRequested())
	{
		context.sleep(delta);
		if (context.isShutdownRequested())
		{
			// shutdown was requested while sleeping - do not send any status request
			return;
		}
	}
	auto statusResponse = BeaconSendingRequestUtil::sendStatusRequest(context, STATUS_REQUEST_RETRIES, INITIAL_RETRY_SLEEP_TIME_MILLISECONDS.count());
	handleStatusResponse(context, statusResponse);

	// update the last status check time in any case
	context.setLastStatusCheckTime(currentTime);
}

std::shared_ptr<AbstractBeaconSendingState> BeaconSendingCaptureOffState::getShutdownState()
{
	return std::shared_ptr<AbstractBeaconSendingState>(new BeaconSendingFlushSessionsState());
}

const char* BeaconSendingCaptureOffState::getStateName() const
{
	return "CaptureOff";
}

void BeaconSendingCaptureOffState::handleStatusResponse(BeaconSendingContext& context, std::shared_ptr<protocol::StatusResponse> statusResponse)
{
	if (statusResponse != nullptr)
	{
		// handle status response, even if it's erroneous
		// if it's an erroneous response capturing is disabled
		context.handleStatusResponse(statusResponse);
	}
	if (BeaconSendingResponseUtil::isTooManyRequestsResponse(statusResponse))
	{
		// received "too many requests" response
		// in this case stay in capture off state and use the retry-after delay for sleeping
		context.setNextState(std::make_shared<BeaconSendingCaptureOffState>(statusResponse->getRetryAfterInMilliseconds()));
	}
	else if (BeaconSendingResponseUtil::isSuccessfulResponse(statusResponse) && context.isCaptureOn())
	{
		// capturing is re-enabled again, but only if we received a response from the server
		context.setNextState(std::shared_ptr<AbstractBeaconSendingState>(new BeaconSendingCaptureOnState()));
	}
}

int64_t BeaconSendingCaptureOffState::getSleepTimeInMilliseconds() const
{
	return mSleepTimeInMilliseconds;
}
