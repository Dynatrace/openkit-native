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

#include "BeaconSendingCaptureOnState.h"

#include <chrono>
#include <algorithm>
#include <memory>

#include "communication/BeaconSendingCaptureOffState.h"
#include "communication/BeaconSendingFlushSessionsState.h"
#include "communication/BeaconSendingTimeSyncState.h"
#include "communication/AbstractBeaconSendingState.h"
#include "communication/BeaconSendingContext.h"

#include "protocol/StatusResponse.h"

using namespace communication;

BeaconSendingCaptureOnState::BeaconSendingCaptureOnState()
	: AbstractBeaconSendingState(AbstractBeaconSendingState::StateType::BEACON_SENDING_CAPTURE_ON_STATE)
	, statusResponse(nullptr)
{

}

void BeaconSendingCaptureOnState::doExecute(BeaconSendingContext& context)
{
	// check if time sync is required (from time to time a re-sync must be performed)
	if (BeaconSendingTimeSyncState::isTimeSyncRequired(context))
	{
		// time re-sync required -> transition
		context.setNextState(std::shared_ptr<AbstractBeaconSendingState>(new BeaconSendingTimeSyncState()));
		return;
	}

	context.sleep();

	statusResponse = nullptr;

	// send all finished sessions (this method may set this.statusResponse)
	sendFinishedSessions(context);

	// handle the last statusResponse received (or null if none was received) from the server
	handleStatusResponse(context, std::move(statusResponse));
}

std::shared_ptr<AbstractBeaconSendingState> BeaconSendingCaptureOnState::getShutdownState()
{
	return std::shared_ptr<AbstractBeaconSendingState>(new BeaconSendingFlushSessionsState());
}

void BeaconSendingCaptureOnState::sendFinishedSessions(BeaconSendingContext& context)
{
	auto finishedSession = context.getNextFinishedSession();
	while (finishedSession != nullptr)
	{
		//statusResponse = finishedSession->sendBeacon(context.getHTTPClientProvider()); // TODO: Enable once sendBeacon is implemented
		if (statusResponse == nullptr)
		{
			// something went wrong,
			//if (!finishedSession.isEmpty()) // TODO: Enable once sendBeacon is implemented
			{
				// well there is more data to send, and we could not do it (now)
				// just push it back
				context.pushBackFinishedSession(finishedSession);
				break; //  sending did not work, break out for now and retry it later
			}
		}

		// session was sent - so remove it from beacon cache
		//finishedSession.clearCapturedData(); // TODO: Enable once sendBeacon is implemented
		finishedSession = context.getNextFinishedSession();
	}
}

void BeaconSendingCaptureOnState::sendOpenSessions(BeaconSendingContext& context)
{
	int64_t currentTimestamp = context.getCurrentTimestamp();
	if (currentTimestamp <= context.getLastOpenSessionBeaconSendTime() + context.getSendInterval())
	{
		return; // send interval to send open sessions has not expired yet
	}

	auto openSessions = context.getAllOpenSessions();
	for (size_t i = 0; i < openSessions.size(); i++)
	{
		//statusResponse = session.sendBeacon(context.getHTTPClientProvider()); // TODO: Enable once sendBeacon is implemented
	}

	context.setLastOpenSessionBeaconSendTime(currentTimestamp);
}

void BeaconSendingCaptureOnState::handleStatusResponse(BeaconSendingContext& context, std::unique_ptr<protocol::StatusResponse> statusResponse)
{
	if (statusResponse == nullptr)
	{
		return; // nothing to handle
	}

	context.handleStatusResponse(std::move(statusResponse));
	if (!context.isCaptureOn()) {
		// capturing is turned off -> make state transition
		context.setNextState(std::shared_ptr<AbstractBeaconSendingState>(new BeaconSendingCaptureOffState()));
	}
}