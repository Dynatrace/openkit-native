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

#include "BeaconSender.h"

#include "communication/BeaconSendingInitialState.h"
#include "communication/BeaconSendingContext.h"

#include <memory>
#include <chrono>


using namespace core;
using namespace core::communication;
using namespace providers;

constexpr int64_t SHUTDOWN_TIMEOUT_MILLISECONDS = 10 * 1000;

BeaconSender::BeaconSender
(
	std::shared_ptr<openkit::ILogger> logger,
	std::shared_ptr<core::configuration::IHTTPClientConfiguration> httpClientConfiguration,
	std::shared_ptr<providers::IHTTPClientProvider> httpClientProvider,
	std::shared_ptr<providers::ITimingProvider> timingProvider,
	std::shared_ptr<core::util::IInterruptibleThreadSuspender> threadSuspender
)
	: mLogger(logger)
	, mBeaconSendingContext(
		std::make_shared<BeaconSendingContext>(
			logger,
			httpClientConfiguration,
			httpClientProvider,
			timingProvider,
			threadSuspender
		)
	)
	, mSendingThread(new core::util::ThreadSurrogate())
	, mTimingProvider(timingProvider)
{
}

bool BeaconSender::initialize()
{
	if (mSendingThread->isStarted())
	{
		return false;
	}

	mSendingThread->start([this] {
		// run the loop as long as OpenKit does not get shutdown or ends itself.
		if (mLogger->isDebugEnabled())
		{
			mLogger->debug("BeaconSender thread started");
		}

		while (mBeaconSendingContext != nullptr && !mBeaconSendingContext->isInTerminalState())
		{
			mBeaconSendingContext->executeCurrentState();
		}

		if (mLogger->isDebugEnabled())
		{
			mLogger->debug("BeaconSender thread stopped");
		}
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
	if (mSendingThread->isAlive())
	{
		mSendingThread->join(SHUTDOWN_TIMEOUT_MILLISECONDS);
	}

	// if the thread is still running here it will either finish later or killed when the main process is ended
}

int32_t BeaconSender::getCurrentServerID() const
{
	return mBeaconSendingContext->getCurrentServerID();
}

void BeaconSender::addSession(std::shared_ptr<core::objects::SessionInternals> session)
{
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("BeaconSender addSession");
	}
	mBeaconSendingContext->addSession(session);
}
