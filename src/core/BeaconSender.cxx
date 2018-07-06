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
#include <chrono>

#include "communication/BeaconSendingInitialState.h"
#include "communication/BeaconSendingContext.h"

using namespace core;
using namespace communication;
using namespace providers;

constexpr std::chrono::seconds SHUTDOWN_TIMEOUT = std::chrono::seconds(10);

BeaconSender::BeaconSender(std::shared_ptr<openkit::ILogger> logger,
						   std::shared_ptr<configuration::Configuration> configuration,
						   std::shared_ptr<providers::IHTTPClientProvider> httpClientProvider,
						   std::shared_ptr<providers::ITimingProvider> timingProvider)
	: mLogger(logger)
	, mBeaconSendingContext(std::shared_ptr<BeaconSendingContext>(new BeaconSendingContext(logger, httpClientProvider, timingProvider, configuration)))
	, mSendingThread()
	, mShutdownTrigger(false)
{

}

bool BeaconSender::initialize()
{
	mSendingThread = std::async(std::launch::async, [this] {
		// run the loop as long as OpenKit does not get shutdown or ends itself.
		if (mLogger->isDebugEnabled())
		{
			mLogger->debug("BeaconSender thread started");
		}

		while (mBeaconSendingContext != nullptr && !mBeaconSendingContext->isInTerminalState() && !mShutdownTrigger)
		{
			mBeaconSendingContext->executeCurrentState();
		}

		if (mLogger->isDebugEnabled())
		{
			mLogger->debug("BeaconSender thread stopped");
		}

		std::this_thread::sleep_for(std::chrono::seconds(15));

		return mBeaconSendingContext->isShutdownRequested();
	});
	
	return true;
}

bool BeaconSender::waitForInit() const
{
	return mBeaconSendingContext->waitForInit();
}

bool BeaconSender::waitForInit(int64_t timeoutMillis) const
{
	return mBeaconSendingContext->waitForInit(timeoutMillis);
}

bool BeaconSender::isInitialized() const
{
	return mBeaconSendingContext->isInitialized();
}

void BeaconSender::shutdown()
{
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("BeaconSender thread request shutdown");
	}

	mBeaconSendingContext->requestShutdown();

	auto start = std::chrono::system_clock::now();
	auto current = std::chrono::system_clock::now();
	auto timeOutInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(SHUTDOWN_TIMEOUT).count();
	auto offsetFromStart = (current - start).count();
	while ( offsetFromStart < timeOutInMilliseconds)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		auto threadStatus = mSendingThread.wait_for(std::chrono::milliseconds(0));
		if (threadStatus == std::future_status::ready)
		{
			break;//thread finished
		}
		current = std::chrono::system_clock::now();
		auto offsetFromStart = (current - start).count();
	}

	mShutdownTrigger = true;
}

void BeaconSender::startSession(std::shared_ptr<Session> session)
{
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("BeaconSender startSession");
	}
	mBeaconSendingContext->startSession(session);
}

void BeaconSender::finishSession(std::shared_ptr<Session> session)
{
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("BeaconSender finishSession");
	}
	mBeaconSendingContext->finishSession(session);
}
