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

#include "OpenKit.h"
#include "protocol/Beacon.h"
#include "providers/DefaultHTTPClientProvider.h"
#include "providers/DefaultTimingProvider.h"
#include "providers/DefaultThreadIDProvider.h"
#include "core/BeaconSender.h"
#include "core/caching/BeaconCache.h"
#include "core/caching/BeaconCacheEvictor.h"

#include <inttypes.h> // for PRId64 macro

using namespace core::objects;

// initialize global instance count with 0.
int32_t OpenKit::gInstanceCount = 0;
std::mutex OpenKit::gInitLock;

OpenKit::OpenKit
(
	std::shared_ptr<openkit::ILogger> logger,
	std::shared_ptr<core::configuration::Configuration> configuration
)
: OpenKit
(
	logger,
	configuration,
	std::make_shared<providers::DefaultHTTPClientProvider>(),
	std::make_shared<providers::DefaultTimingProvider>(),
	std::make_shared<providers::DefaultThreadIDProvider>()
)
{
}

OpenKit::OpenKit
(
	std::shared_ptr<openkit::ILogger> logger,
	std::shared_ptr<core::configuration::Configuration> configuration,
	std::shared_ptr<providers::IHTTPClientProvider> httpClientProvider,
	std::shared_ptr<providers::ITimingProvider> timingProvider,
	std::shared_ptr<providers::IThreadIDProvider> threadIDProvider)
	: mLogger(logger)
	, mClientProvider(httpClientProvider)
	, mConfiguration(configuration)
	, mTimingProvider(timingProvider)
	, mThreadIDProvider(threadIDProvider)
	, mBeaconCache(
		std::make_shared<caching::BeaconCache>(logger)
	)
	, mBeaconSender(
		std::make_shared<core::BeaconSender>(
			logger,
			configuration,
			httpClientProvider,
			timingProvider
		)
	)
	, mBeaconCacheEvictor(
		std::make_shared<caching::BeaconCacheEvictor>(
			logger,
			mBeaconCache,
			configuration->getBeaconCacheConfiguration(),
			timingProvider
		)
	)
	, mIsShutdown(0)
{
	if (logger->isInfoEnabled())
	{
		// TODO: Use proper version information (incl. the build number)
		logger->info("OpenKit() - %s OpenKit %s instantiated", configuration->getOpenKitType(), openkit::DEFAULT_APPLICATION_VERSION);
	}
	if (logger->isDebugEnabled())
	{
		logger->debug("OpenKit() - applicationName=%s, applicationID=%s, deviceID=%ld, origDeviceID=%s, endpointURL=%s",
			configuration->getApplicationName().getStringData().c_str(),
			configuration->getApplicationID().getStringData().c_str(),
			configuration->getDeviceID(),
			configuration->getOrigDeviceID().getStringData().c_str(),
			configuration->getEndpointURL().getStringData().c_str());
	}

	globalInit();
}

OpenKit::~OpenKit()
{
	globalShutdown();
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
		mLogger->debug("OpenKit createSession(%s)", clientIPAddress != nullptr ? core::UTF8String(clientIPAddress).getStringData().c_str() : "null");
	}
	if (mIsShutdown)
	{
		return  NullSession::INSTANCE;
	}

	auto beacon = std::make_shared<protocol::Beacon>(mLogger, mBeaconCache, mConfiguration, clientIPAddress, mThreadIDProvider, mTimingProvider);
	auto newSession = std::make_shared<core::objects::Session>(mLogger, mBeaconSender, beacon);
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

void OpenKit::globalInit()
{
	std::lock_guard<std::mutex> guard(gInitLock);

	gInstanceCount += 1;

	if (gInstanceCount > 1)
	{
		// not the first instance -> no global init needed.
		return;
	}

	mClientProvider->globalInit();
}

void OpenKit::globalShutdown()
{
	std::lock_guard<std::mutex> guard(gInitLock);

	gInstanceCount -= 1;

	if (gInstanceCount > 0)
	{
		// still another instance exists -> no global destroy needed
		return;
	}

	mClientProvider->globalDestroy();
}
