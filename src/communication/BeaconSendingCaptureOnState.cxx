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

#include "BeaconSendingCaptureOnState.h"

#include <chrono>
#include <algorithm>
#include <memory>

#include "communication/BeaconSendingCaptureOffState.h"
#include "communication/BeaconSendingFlushSessionsState.h"
#include "communication/AbstractBeaconSendingState.h"
#include "communication/BeaconSendingContext.h"
#include "communication/BeaconSendingResponseUtil.h"

#include "protocol/StatusResponse.h"

using namespace communication;

BeaconSendingCaptureOnState::BeaconSendingCaptureOnState()
	: AbstractBeaconSendingState(AbstractBeaconSendingState::StateType::BEACON_SENDING_CAPTURE_ON_STATE)
{

}

void BeaconSendingCaptureOnState::doExecute(BeaconSendingContext& context)
{
	context.sleep();
	if (context.isShutdownRequested())
	{
		// shutdown was requested during sleep
		// return and let the base class handle this
		return;
	}

	// sned new session request for all sessions that are new
	auto newSessionsResponse = sendNewSessionRequests(context);
	if (BeaconSendingResponseUtil::isTooManyRequestsResponse(newSessionsResponse))
	{
		// server is currently overloaded, temporarily switch to capture off
		context.setNextState(std::make_shared<BeaconSendingCaptureOffState>(newSessionsResponse->getRetryAfterInMilliseconds()));
		return;
	}
	
	// send all finished sessions
	auto finishedSessionsResponse = sendFinishedSessions(context);
	if (BeaconSendingResponseUtil::isTooManyRequestsResponse(finishedSessionsResponse))
	{
		// server is currently overloaded, temporarily switch to capture off
		context.setNextState(std::make_shared<BeaconSendingCaptureOffState>(finishedSessionsResponse->getRetryAfterInMilliseconds()));
		return;
	}

	// check if we need to send open sessions & do it if necessary
	auto openSessionsResponse = sendOpenSessions(context);
	if (BeaconSendingResponseUtil::isTooManyRequestsResponse(openSessionsResponse))
	{
		// server is currently overloaded, temporarily switch to capture off
		context.setNextState(std::make_shared<BeaconSendingCaptureOffState>(openSessionsResponse->getRetryAfterInMilliseconds()));
		return;
	}

	// collect the last status response
	auto lastStatusResponse = newSessionsResponse;
	if (openSessionsResponse != nullptr) {
		lastStatusResponse = openSessionsResponse;
	}
	else if (finishedSessionsResponse != nullptr) {
		lastStatusResponse = finishedSessionsResponse;
	}

	// handle the last statusResponse received (or null if none was received) from the server
	handleStatusResponse(context, lastStatusResponse);
}

std::shared_ptr<AbstractBeaconSendingState> BeaconSendingCaptureOnState::getShutdownState()
{
	return std::shared_ptr<AbstractBeaconSendingState>(new BeaconSendingFlushSessionsState());
}

const char* BeaconSendingCaptureOnState::getStateName() const
{
	return "CaptureOn";
}

std::shared_ptr<protocol::StatusResponse> BeaconSendingCaptureOnState::sendFinishedSessions(BeaconSendingContext& context)
{
	std::shared_ptr<protocol::StatusResponse> statusResponse = nullptr;
	// check if there's finished Sessions to be sent -> immediately send beacon(s) of finished Sessions
	for (auto session : context.getAllFinishedAndConfiguredSessions())
	{
		if (session->isDataSendingAllowed()) {
			statusResponse = session->sendBeacon(context.getHTTPClientProvider());
			if (!BeaconSendingResponseUtil::isSuccessfulResponse(statusResponse))
			{
				// something went wrong,
				if (BeaconSendingResponseUtil::isTooManyRequestsResponse(statusResponse) || !session->isEmpty())
				{
					break; //  sending did not work, break out for now and retry it later
				}
			}
		}

		// session was sent/is not allowed to be sent - so remove it from beacon cache
		context.removeSession(session);
		session->clearCapturedData();
	}

	return statusResponse;
}

std::shared_ptr<protocol::StatusResponse> BeaconSendingCaptureOnState::sendOpenSessions(BeaconSendingContext& context)
{
	std::shared_ptr<protocol::StatusResponse> statusResponse = nullptr;
	int64_t currentTimestamp = context.getCurrentTimestamp();
	if (currentTimestamp <= context.getLastOpenSessionBeaconSendTime() + context.getSendInterval())
	{
		return nullptr; // send interval to send open sessions has not expired yet
	}

	for (auto session : context.getAllOpenAndConfiguredSessions())
	{
		if (session->isDataSendingAllowed())
		{
			statusResponse = session->sendBeacon(context.getHTTPClientProvider());
			if (BeaconSendingResponseUtil::isTooManyRequestsResponse(statusResponse))
			{
				// server is currently overloaded, return immediately
				break;
			}
		}
		else
		{
			session->clearCapturedData();
		}
	}

	context.setLastOpenSessionBeaconSendTime(currentTimestamp);

	return statusResponse;
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

std::shared_ptr<protocol::StatusResponse> BeaconSendingCaptureOnState::sendNewSessionRequests(BeaconSendingContext& context)
{
	std::shared_ptr<protocol::StatusResponse> statusResponse = nullptr;
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

		statusResponse = context.getHTTPClient()->sendNewSessionRequest();
		if (BeaconSendingResponseUtil::isSuccessfulResponse(statusResponse))
		{
			auto beaconConfiguration = session->getBeaconConfiguration();
			auto newBeaconConfiguration = std::make_shared<configuration::BeaconConfiguration>(statusResponse->getMultiplicity(), beaconConfiguration->getDataCollectionLevel(), beaconConfiguration->getCrashReportingLevel());
			session->updateBeaconConfiguration(newBeaconConfiguration);
		}
		else if (BeaconSendingResponseUtil::isTooManyRequestsResponse(statusResponse))
		{
			// server is currently overloaded, return immediately
			break;
		}
		else
		{
			// any other unsuccessful response
			session->decreaseNumberOfNewSessionRequests();
		}
	}

	return statusResponse;
}