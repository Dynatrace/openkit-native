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

	sendNewSessionRequests(context);

	statusResponse = nullptr;
	
	// send all finished sessions (this method may set this.statusResponse)
	sendFinishedSessions(context);

	// check if we need to send open sessions & do it if necessary (this method may set this.statusResponse)
	sendOpenSessions(context);

	// handle the last statusResponse received (or null if none was received) from the server
	handleStatusResponse(context, std::move(statusResponse));
}

std::shared_ptr<AbstractBeaconSendingState> BeaconSendingCaptureOnState::getShutdownState()
{
	return std::shared_ptr<AbstractBeaconSendingState>(new BeaconSendingFlushSessionsState());
}

const char* BeaconSendingCaptureOnState::getStateName() const
{
	return "CaptureOn";
}

void BeaconSendingCaptureOnState::sendFinishedSessions(BeaconSendingContext& context)
{
	// check if there's finished Sessions to be sent -> immediately send beacon(s) of finished Sessions
	for(auto session : context.getAllFinishedAndConfiguredSessions())
	{
		if (session->isDataSendingAllowed()) {
			statusResponse = std::move(session->sendBeacon(context.getHTTPClientProvider()));
			if (statusResponse == nullptr)
			{
				// something went wrong,
				if (!session->isEmpty())
				{
					break; //  sending did not work, break out for now and retry it later
				}
			}
		}

		// session was sent/is not allowed to be sent - so remove it from beacon cache
		context.removeSession(session);
		session->clearCapturedData();
}
}

void BeaconSendingCaptureOnState::sendOpenSessions(BeaconSendingContext& context)
{
	int64_t currentTimestamp = context.getCurrentTimestamp();
	if (currentTimestamp <= context.getLastOpenSessionBeaconSendTime() + context.getSendInterval())
	{
		return; // send interval to send open sessions has not expired yet
	}

	for (auto session : context.getAllOpenAndConfiguredSessions())
	{
		if (session->isDataSendingAllowed())
		{
			statusResponse = std::move(session->sendBeacon(context.getHTTPClientProvider()));
		}
		else
		{
			session->clearCapturedData();
		}
	}

	context.setLastOpenSessionBeaconSendTime(currentTimestamp);
}

void BeaconSendingCaptureOnState::handleStatusResponse(BeaconSendingContext& context, std::shared_ptr<protocol::StatusResponse> statusResponse)
{
	if (statusResponse == nullptr)
	{
		return; // nothing to handle
	}

	context.handleStatusResponse(statusResponse);
	if (!context.isCaptureOn()) {
		// capturing is turned off -> make state transition
		context.setNextState(std::shared_ptr<AbstractBeaconSendingState>(new BeaconSendingCaptureOffState()));
	}
}

void BeaconSendingCaptureOnState::sendNewSessionRequests(BeaconSendingContext& context)
{
	for (auto session : context.getAllNewSessions() )
	{
		if (!session->canSendNewSessionRequest())
		{
			// already exceeded the maximum number of session requests, disable any further data collecting
			auto beaconConfiguration = session->getBeaconConfiguration();
			auto newBeaconConfiguration = std::make_shared<configuration::BeaconConfiguration>(0, beaconConfiguration->getDataCollectionLevel(), beaconConfiguration->getCrashReportingLevel());
			session->updateBeaconConfiguration(newBeaconConfiguration);
			continue;
		}

		auto response = context.getHTTPClient()->sendNewSessionRequest();
		if (response != nullptr)
		{
			auto beaconConfiguration = session->getBeaconConfiguration();
			auto newBeaconConfiguration = std::make_shared<configuration::BeaconConfiguration>(response->getMultiplicity(), beaconConfiguration->getDataCollectionLevel(), beaconConfiguration->getCrashReportingLevel());
			session->updateBeaconConfiguration(newBeaconConfiguration);
		}
		else
		{
			// did not retrieve any response from server, maybe the cluster is down?
			session->decreaseNumberOfNewSessionRequests();
		}
	}
}