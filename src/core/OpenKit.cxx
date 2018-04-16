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

#include "OpenKit.h"
#include "providers/DefaultHTTPClientProvider.h"
#include "providers/DefaultTimingProvider.h"
#include "providers/DefaultThreadIDProvider.h"
#include "caching/BeaconCache.h"

using namespace core;

OpenKit::OpenKit(std::shared_ptr<api::ILogger> logger, std::shared_ptr<configuration::Configuration> configuration)
	: OpenKit(logger, configuration,
		std::make_shared<providers::DefaultHTTPClientProvider>(),
		std::make_shared<providers::DefaultTimingProvider>(),
		std::make_shared<providers::DefaultThreadIDProvider>()
	)
{

}

OpenKit::OpenKit(std::shared_ptr<api::ILogger> logger, std::shared_ptr<configuration::Configuration> configuration,
	std::shared_ptr<providers::IHTTPClientProvider> httpClientProvider,
	std::shared_ptr<providers::ITimingProvider> timingProvider,
	std::shared_ptr<providers::IThreadIDProvider> threadIDProvider)
	: mLogger(logger)
	, mConfiguration(configuration)
	, mTimingProvider(timingProvider)
	, mThreadIDProvider(threadIDProvider)
	, mBeaconCache(nullptr)
	, mBeaconSender(nullptr)
	, mBeaconCacheEvictor(nullptr)
{

}

void OpenKit::initialize()
{

}

bool OpenKit::waitForInitCompletion()
{
	return false;
}

bool OpenKit::waitForInitCompletion(int64_t timeoutMillis)
{
	return false;
}

bool OpenKit::isInitialized() const
{
	return true;
}

std::shared_ptr<api::ISession> OpenKit::createSession(const char* clientIPAddress)
{
	return nullptr;
}

void OpenKit::shutdown()
{

}