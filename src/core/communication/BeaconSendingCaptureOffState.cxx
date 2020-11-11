/**
 * Copyright 2018-2020 Dynatrace LLC
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
#include "BeaconSendingCaptureOffState.h"
#include "BeaconSendingCaptureOnState.h"
#include "BeaconSendingContext.h"
#include "BeaconSendingFlushSessionsState.h"
#include "BeaconSendingRequestUtil.h"
#include "BeaconSendingResponseUtil.h"
#include "protocol/IStatusResponse.h"

#include <chrono>
#include <algorithm>

using namespace core::communication;

/// number of retries for the status request
static constexpr int32_t STATUS_REQUEST_RETRIES = 5;

static const std::chrono::milliseconds STATUS_CHECK_INTERVAL_MILLISECONDS = std::chrono::hours(2);
static const std::chrono::milliseconds INITIAL_RETRY_SLEEP_TIME_MILLISECONDS = std::chrono::seconds(1);

BeaconSendingCaptureOffState::BeaconSendingCaptureOffState()
	: BeaconSendingCaptureOffState(int64_t(-1))
{
}

BeaconSendingCaptureOffState::BeaconSendingCaptureOffState(int64_t sleepTimeInMilliseconds)
	: AbstractBeaconSendingState(IBeaconSendingState::StateType::BEACON_SENDING_CAPTURE_OFF_STATE)
	, mSleepTimeInMilliseconds(sleepTimeInMilliseconds)
{
}

void BeaconSendingCaptureOffState::doExecute(IBeaconSendingContext& context)
{
	// disable capturing - avoid collecting further data
	context.disableCaptureAndClear();

	auto currentTime = context.getCurrentTimestamp();

	auto delta = mSleepTimeInMilliseconds > int64_t(0)
		? mSleepTimeInMilliseconds
		: STATUS_CHECK_INTERVAL_MILLISECONDS.count() - (currentTime - context.getLastStatusCheckTime());
	if (delta > 0 && !context.isShutdownRequested())
	{
		context.sleep(delta);
		if (context.isShutdownRequested())
		{
			// shutdown was requested while sleeping - do not send any status request
			return;
		}
	}
	auto statusResponse = BeaconSendingRequestUtil::sendStatusRequest(
		context,
		STATUS_REQUEST_RETRIES,
		INITIAL_RETRY_SLEEP_TIME_MILLISECONDS.count()
	);
	handleStatusResponse(context, statusResponse);

	// update the last status check time in any case
	context.setLastStatusCheckTime(currentTime);
}

std::shared_ptr<IBeaconSendingState> BeaconSendingCaptureOffState::getShutdownState()
{
	return std::make_shared<BeaconSendingFlushSessionsState>();
}

const char* BeaconSendingCaptureOffState::getStateName() const
{
	return "CaptureOff";
}

void BeaconSendingCaptureOffState::handleStatusResponse(
	IBeaconSendingContext& context,
	std::shared_ptr<protocol::IStatusResponse> statusResponse
)
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
		auto captureOnState = std::make_shared<BeaconSendingCaptureOnState>();
		context.setNextState(captureOnState);
	}
}

int64_t BeaconSendingCaptureOffState::getSleepTimeInMilliseconds() const
{
	return mSleepTimeInMilliseconds;
}
