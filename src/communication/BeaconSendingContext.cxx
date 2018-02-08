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

#include "BeaconSendingContext.h"

#include "communication/AbstractBeaconSendingState.h"
#include "communication/BeaconSendingInitialState.h"

#include "protocol/HTTPClient.h"

using namespace communication;

BeaconSendingContext::BeaconSendingContext(providers::IHTTPClientProvider& httpClientProvider,
										   providers::ITimingProvider& timingProvider,
										   configuration::Configuration configuration)
	: mCurrentState(std::unique_ptr<AbstractBeaconSendingState>(new BeaconSendingInitialState()))
	, mIsInTerminalState(false)
	, mShutdown(false)
	, mHTTPClientProvider(httpClientProvider)
	, mTimingProvider(timingProvider)
	, mConfiguration(configuration)
{	
}

void BeaconSendingContext::setNextState(std::unique_ptr<AbstractBeaconSendingState> nextState)
{
	if (nextState != nullptr)
	{
		mCurrentState = std::move(nextState);
	}
}

bool BeaconSendingContext::isInTerminalState() const
{
	return mCurrentState->isAShutdownState();
}

void BeaconSendingContext::executeCurrentState()
{
	if (mCurrentState != nullptr)
	{
		mCurrentState->executeState(*this);
	}
}

void BeaconSendingContext::requestShutdown()
{
	mShutdown = true;
}

bool BeaconSendingContext::isShutdownRequested() const
{
	return mShutdown;
}

const configuration::Configuration& BeaconSendingContext::getConfiguration() const
{
	return mConfiguration;
}

std::unique_ptr<protocol::HTTPClient> BeaconSendingContext::getHTTPClient()
{
	return mHTTPClientProvider.createClient(mConfiguration.getHTTPClientConfiguration());
}
