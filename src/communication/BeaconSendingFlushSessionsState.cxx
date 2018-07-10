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

#include "BeaconSendingFlushSessionsState.h"

#include <chrono>
#include <algorithm>
#include <memory>

#include "communication/BeaconSendingFlushSessionsState.h"
#include "communication/AbstractBeaconSendingState.h"
#include "communication/BeaconSendingContext.h"
#include "communication/BeaconSendingTerminalState.h"

using namespace communication;

BeaconSendingFlushSessionsState::BeaconSendingFlushSessionsState()
	: AbstractBeaconSendingState(AbstractBeaconSendingState::StateType::BEACON_SENDING_FLUSH_SESSIONS_STATE)
{

}

void BeaconSendingFlushSessionsState::doExecute(BeaconSendingContext& context)
{
	// first get all sessions that do not have any multiplicity set -> and move them to open sessions
	auto newSessions = context.getAllNewSessions();
	for (auto it = newSessions.begin() ; it != newSessions.end(); it++)
	{
		auto beaconConfiguration = (*it)->getWrappedSession()->getBeaconConfiguration();
		auto updatedBeaconConfiguration = std::make_shared<configuration::BeaconConfiguration>(1, beaconConfiguration->getDataCollectionLevel(), beaconConfiguration->getCrashReportingLevel());
		(*it)->updateBeaconConfiguration(updatedBeaconConfiguration);

	}

	// end open sessions -> will be flushed afterwards
	auto openSessions = context.getAllOpenAndConfiguredSessions();
	for (auto it = openSessions.begin(); it != openSessions.end(); it++)
	{
		(*it)->end();
	}

	// flush already finished (and previously ended) sessions
	auto finishedSession = context.getAllFinishedAndConfiguredSessions();
	for (auto it = finishedSession.begin(); it != finishedSession.end(); it++)
	{
		std::shared_ptr<core::SessionWrapper> sessionWrapper = (*it);
		if (sessionWrapper->isDataSendingAllowed())
		{
			sessionWrapper->sendBeacon(context.getHTTPClientProvider());
		}
		sessionWrapper->clearCapturedData();
		context.removeSession(sessionWrapper);
	}

	// make last state transition to terminal state
	context.setNextState(std::shared_ptr<AbstractBeaconSendingState>(new BeaconSendingTerminalState()));
}

std::shared_ptr<AbstractBeaconSendingState> BeaconSendingFlushSessionsState::getShutdownState()
{
	return std::shared_ptr<AbstractBeaconSendingState>(new BeaconSendingTerminalState());
}

const char* BeaconSendingFlushSessionsState::getStateName() const
{
	return "FlushSessions";
}

