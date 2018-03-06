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

#include "BeaconSender.h"

#include <memory>

#include "communication/BeaconSendingInitialState.h"
#include "communication/BeaconSendingContext.h"

using namespace core;
using namespace communication;
using namespace providers;

BeaconSender::BeaconSender(std::shared_ptr<configuration::Configuration> configuration,
						   std::shared_ptr<providers::IHTTPClientProvider> httpClientProvider,
						   std::shared_ptr<providers::ITimingProvider> timingProvider)
	: mBeaconSendingContext(std::shared_ptr<BeaconSendingContext>(new BeaconSendingContext(httpClientProvider, timingProvider, configuration)))
	, mSendingThread(nullptr)
{

}

void beaconSendingLoop(std::shared_ptr<BeaconSendingContext> context)
{
	while ( context != nullptr && !context->isInTerminalState())
	{
		context->executeCurrentState();
	}
}

bool BeaconSender::initialize()
{
	mSendingThread = std::unique_ptr<std::thread>(new std::thread(&beaconSendingLoop, mBeaconSendingContext));
	mSendingThread->join();
	return true;
}

bool BeaconSender::waitForInit() const
{
	return mBeaconSendingContext->waitForInit();
}

void BeaconSender::shutdown()
{
	mBeaconSendingContext->requestShutdown();
	mSendingThread->join();
}