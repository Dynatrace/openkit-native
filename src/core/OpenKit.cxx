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

#include "core/OpenKit.h"
#include "protocol/Beacon.h"
#include "providers/DefaultHTTPClientProvider.h"
#include "providers/DefaultTimingProvider.h"
#include "providers/DefaultThreadIDProvider.h"
#include "caching/BeaconCache.h"

#include <inttypes.h> // for PRId64 macro

using namespace core;

OpenKit::OpenKit(std::shared_ptr<openkit::ILogger> logger, std::shared_ptr<configuration::Configuration> configuration)
	: OpenKit(logger, configuration,
		std::make_shared<providers::DefaultHTTPClientProvider>(),
		std::make_shared<providers::DefaultTimingProvider>(),
		std::make_shared<providers::DefaultThreadIDProvider>()
	)
{

}

OpenKit::OpenKit(std::shared_ptr<openkit::ILogger> logger, std::shared_ptr<configuration::Configuration> configuration,
	std::shared_ptr<providers::IHTTPClientProvider> httpClientProvider,
	std::shared_ptr<providers::ITimingProvider> timingProvider,
	std::shared_ptr<providers::IThreadIDProvider> threadIDProvider)
	: mLogger(logger)
	, mConfiguration(configuration)
	, mTimingProvider(timingProvider)
	, mThreadIDProvider(threadIDProvider)
	, mBeaconCache(std::make_shared<caching::BeaconCache>(logger))
	, mBeaconSender(std::make_shared<core::BeaconSender>(logger, configuration, httpClientProvider, timingProvider))
	, mBeaconCacheEvictor(std::make_shared<caching::BeaconCacheEvictor>(logger, mBeaconCache, configuration->getBeaconCacheConfiguration(), timingProvider))
	, mIsShutdown(0)
	, NULL_SESSION(std::make_shared<core::NullSession>())
{
	if (logger->isInfoEnabled())
	{
		// TODO: Use proper version information (incl. the build number)
		logger->info("%s OpenKit %s instantiated", configuration->getOpenKitType(), openkit::DEFAULT_APPLICATION_VERSION);
	}
	if (logger->isDebugEnabled())
	{
		logger->debug("applicationName=%s, applicationID=%s, deviceID=%" PRId64 ", endpointURL=%s",
			configuration->getApplicationName() != nullptr ? configuration->getApplicationName().getStringData().c_str() : "",
			configuration->getApplicationID() != nullptr ? configuration->getApplicationID().getStringData().c_str() : "",
			configuration->getDeviceID(),
			configuration->getEndpointURL() != nullptr ? configuration->getEndpointURL().getStringData().c_str() : "");
	}
}

void OpenKit::initialize()
{
	mBeaconCacheEvictor->start();
	mBeaconSender->initialize();
}

bool OpenKit::waitForInitCompletion()
{
	return mBeaconSender->waitForInit();
}

bool OpenKit::waitForInitCompletion(int64_t timeoutMillis)
{
	return mBeaconSender->waitForInit(timeoutMillis);
}

bool OpenKit::isInitialized() const
{
	return mBeaconSender->isInitialized();
}

std::shared_ptr<openkit::ISession> OpenKit::createSession(const char* clientIPAddress)
{
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("OpenKit createSession(%s)", clientIPAddress != nullptr ? clientIPAddress : "null");
	}
	if (mIsShutdown == 1 || clientIPAddress == nullptr || strlen(clientIPAddress) == 0)
	{
		return NULL_SESSION;
	}

	std::shared_ptr<protocol::Beacon> beacon = std::make_shared<protocol::Beacon>(mLogger, mBeaconCache, mConfiguration, clientIPAddress, mThreadIDProvider, mTimingProvider);
	auto newSession = std::make_shared<core::Session>(mLogger, mBeaconSender, beacon);
	newSession->startSession();
	return newSession;
}

void OpenKit::shutdown()
{
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("OpenKit shutdown requested");
	}
	mIsShutdown = 1;
	mBeaconCacheEvictor->stop();
	mBeaconSender->shutdown();
}