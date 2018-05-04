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

#include "communication/BeaconSendingTimeSyncState.h"

#include <math.h>
#include <algorithm>

#include "communication/BeaconSendingContext.h"
#include "communication/BeaconSendingTerminalState.h"
#include "communication/BeaconSendingCaptureOffState.h"
#include "communication/BeaconSendingCaptureOnState.h"
#include "communication/BeaconSendingFlushSessionsState.h"
 

using namespace communication;

std::chrono::milliseconds BeaconSendingTimeSyncState::TIME_SYNC_INTERVAL_IN_MILLIS = std::chrono::minutes(1);
uint32_t BeaconSendingTimeSyncState::REQUIRED_TIME_SYNC_REQUESTS = 5;
std::chrono::milliseconds BeaconSendingTimeSyncState::INITIAL_RETRY_SLEEP_TIME_MILLISECONDS = std::chrono::seconds(1);
constexpr uint32_t TIME_SYNC_RETRY_COUNT = 5;

BeaconSendingTimeSyncState::BeaconSendingTimeSyncState()
	: BeaconSendingTimeSyncState(true)
{
}

BeaconSendingTimeSyncState::BeaconSendingTimeSyncState(bool initialSync)
	: AbstractBeaconSendingState(AbstractBeaconSendingState::StateType::BEACON_SENDING_TIME_SYNC_STATE)
	, mInitialTimeSync(initialSync)
{
}

void BeaconSendingTimeSyncState::doExecute(BeaconSendingContext& context)
{
	if (!isTimeSyncRequired(context))
	{
		setNextState(context);
		return;
	}

	// execute time sync requests - note during initial sync it might be possible
	// that the time sync capability is disabled.
	std::vector<int64_t> timeSyncOffsets = executeTimeSyncRequests(context);

	handleTimeSyncResponses(context, timeSyncOffsets);

	// mark init being completed if it's the initial time sync
	if (mInitialTimeSync)
	{
		context.setInitCompleted(true);
	}
}

std::shared_ptr<AbstractBeaconSendingState> BeaconSendingTimeSyncState::getShutdownState()
{
	if (mInitialTimeSync)
	{
		return std::shared_ptr<AbstractBeaconSendingState>(new BeaconSendingTerminalState());
	}
	else
	{
		return std::shared_ptr<AbstractBeaconSendingState>(new BeaconSendingFlushSessionsState());
	}
}

const char* BeaconSendingTimeSyncState::getStateName() const
{
	return "TimeSync";
}

bool BeaconSendingTimeSyncState::isTimeSyncRequired(BeaconSendingContext& context)
{

	if (!context.isTimeSyncSupported())
	{
		return false; // time sync not supported by server, therefore not required
	}

	return ((context.getLastTimeSyncTime() < 0)
		|| (context.getCurrentTimestamp() - context.getLastTimeSyncTime() > static_cast<int64_t>(TIME_SYNC_INTERVAL_IN_MILLIS.count())));
}

void BeaconSendingTimeSyncState::setNextState(BeaconSendingContext& context)
{
	// advance to next state
	if (context.isCaptureOn())
	{
		context.setNextState(std::shared_ptr<AbstractBeaconSendingState>(new BeaconSendingCaptureOnState()));
	}
	else
	{
		context.setNextState(std::shared_ptr<AbstractBeaconSendingState>(new BeaconSendingCaptureOffState()));
	}
}

void BeaconSendingTimeSyncState::handleTimeSyncResponses(BeaconSendingContext& context, std::vector<int64_t>& timeSyncOffsets)
{
	// time sync requests were *not* successful
	// either because of networking issues
	// -OR-
	// the server does not support time sync at all (e.g. AppMon).
	//
	// -> handle this case
	if (timeSyncOffsets.size() < REQUIRED_TIME_SYNC_REQUESTS) 
	{
		handleErroneousTimeSyncRequest(context);
		return;
	}

	// initialize time provider with cluster time offset
	context.initializeTimeSync(computeClusterTimeOffset(timeSyncOffsets), true);

	// also update the time when last time sync was performed to now
	context.setLastTimeSyncTime(context.getCurrentTimestamp());

	// set the next state
	setNextState(context);
}

int64_t BeaconSendingTimeSyncState::computeClusterTimeOffset(std::vector<int64_t>& timeSyncOffsets) 
{
	// time sync requests were successful -> calculate cluster time offset
	std::sort(timeSyncOffsets.begin(), timeSyncOffsets.end());

	// take median value from sorted offset list
	auto median = timeSyncOffsets.at(REQUIRED_TIME_SYNC_REQUESTS / 2);

	// calculate variance from median
	int64_t medianVariance = 0;
	for (uint32_t i = 0; i < REQUIRED_TIME_SYNC_REQUESTS; i++)
	{
		auto diff = timeSyncOffsets[i] - median;
		medianVariance += diff * diff;
	}
	medianVariance = medianVariance / REQUIRED_TIME_SYNC_REQUESTS;

	// calculate cluster time offset as arithmetic mean of all offsets that are in range of 1x standard deviation
	int64_t sum = 0;
	int64_t count = 0;
	for (uint32_t i = 0; i < REQUIRED_TIME_SYNC_REQUESTS; i++)
	{
		auto diff = timeSyncOffsets[i] - median;
		if (diff * diff <= medianVariance)
		{
			sum += timeSyncOffsets[i];
			count++;
		}
	}

	return static_cast<int64_t>(std::round(sum / static_cast<double>(count)));
}

void BeaconSendingTimeSyncState::handleErroneousTimeSyncRequest(BeaconSendingContext& context)
{
	// if this is the initial sync try, we have to initialize the time provider
	// in every other case we keep the previous setting
	if (mInitialTimeSync)
	{
		context.initializeTimeSync(0, context.isTimeSyncSupported());
	}

	if (context.isTimeSyncSupported())
	{
		// server supports time sync
		context.setNextState(std::shared_ptr<AbstractBeaconSendingState>(new BeaconSendingCaptureOffState()));
	}
	else
	{
		// otherwise set the next state based on the configuration
		setNextState(context);
	}
}

std::vector<int64_t> BeaconSendingTimeSyncState::executeTimeSyncRequests(BeaconSendingContext& context)
{
	std::vector<int64_t> timeSyncOffsets;

	uint32_t retry = 0;
	int64_t sleepTimeInMillis = INITIAL_RETRY_SLEEP_TIME_MILLISECONDS.count();

	// no check for shutdown here, time sync has to be completed
	while (timeSyncOffsets.size() < REQUIRED_TIME_SYNC_REQUESTS && !context.isShutdownRequested())
	{
		// doExecute time-sync request and take timestamps
		auto requestSendTime = context.getCurrentTimestamp();
		std::unique_ptr<protocol::TimeSyncResponse> timeSyncResponse = context.getHTTPClient()->sendTimeSyncRequest();
		int64_t responseReceiveTime = context.getCurrentTimestamp();

		if (timeSyncResponse != nullptr)
		{
			int64_t requestReceiveTime = timeSyncResponse->getRequestReceiveTime();
			int64_t responseSendTime = timeSyncResponse->getResponseSendTime();

			// check both timestamps for being > 0
			if ((requestReceiveTime > 0) && (responseSendTime > 0))
			{
				// if yes -> continue time-sync
				auto offset = ((requestReceiveTime - requestSendTime) + (responseSendTime - responseReceiveTime)) / 2;
				timeSyncOffsets.push_back(offset);
				retry = 0; // on successful response reset the retry count & initial sleep time
				sleepTimeInMillis = INITIAL_RETRY_SLEEP_TIME_MILLISECONDS.count();
			}
			else
			{
				// if no -> stop time sync, it's not supported
				context.disableTimeSyncSupport();
				break;
			}
		}
		else if (retry >= TIME_SYNC_RETRY_COUNT)
		{
			// retry limits exceeded
			break;
		}
		else
		{
			context.sleep(sleepTimeInMillis);
			sleepTimeInMillis *= 2;
			retry++;
		}
	}

	return timeSyncOffsets;
}