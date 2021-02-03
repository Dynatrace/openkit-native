/**
 * Copyright 2018-2021 Dynatrace LLC
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

#include "OpenKitInitializer.h"

#include "core/BeaconSender.h"
#include "core/SessionWatchdog.h"
#include "core/SessionWatchdogContext.h"
#include "core/caching/BeaconCache.h"
#include "core/caching/BeaconCacheEvictor.h"
#include "core/configuration/BeaconCacheConfiguration.h"
#include "core/configuration/HTTPClientConfiguration.h"
#include "core/configuration/OpenKitConfiguration.h"
#include "core/configuration/PrivacyConfiguration.h"
#include "core/util/InterruptibleThreadSuspender.h"
#include "providers/DefaultHTTPClientProvider.h"
#include "providers/DefaultSessionIDProvider.h"
#include "providers/DefaultThreadIDProvider.h"
#include "providers/DefaultTimingProvider.h"

using namespace core::objects;

OpenKitInitializer::OpenKitInitializer(const openkit::IOpenKitBuilder& builder)
	: mLogger(builder.getLogger())
	, mPrivacyConfiguration(core::configuration::PrivacyConfiguration::from(builder))
	, mOpenKitConfiguration(core::configuration::OpenKitConfiguration::from(builder))
	, mTimingProvider(std::make_shared<providers::DefaultTimingProvider>())
	, mThreadIdProvider(std::make_shared<providers::DefaultThreadIDProvider>())
	, mSessionIdProvider(std::make_shared<providers::DefaultSessionIDProvider>())
	, mBeaconCache(nullptr)
	, mBeaconCacheEvictor(nullptr)
	, mBeaconSender(nullptr)
	, mSessionWatchdog(nullptr)
{
	mBeaconCache = std::make_shared<core::caching::BeaconCache>(mLogger);
	mBeaconCacheEvictor = std::make_shared<core::caching::BeaconCacheEvictor>(
		mLogger,
		mBeaconCache,
		core::configuration::BeaconCacheConfiguration::from(builder),
		mTimingProvider
	);
	auto httpClientConfig = core::configuration::HTTPClientConfiguration::from(mOpenKitConfiguration);
	// shared thread suspender between BeaconSender and HttpClient. HttpClient will be woken up when
	// BeaconSender shuts down
	auto beaconSenderThreadSuspender = std::make_shared<core::util::InterruptibleThreadSuspender>();
	mBeaconSender = std::make_shared<core::BeaconSender>(
		mLogger,
		httpClientConfig,
		std::make_shared<providers::DefaultHTTPClientProvider>(mLogger, beaconSenderThreadSuspender),
		mTimingProvider,
		beaconSenderThreadSuspender
	);
	mSessionWatchdog = std::make_shared<core::SessionWatchdog>(
		mLogger,
		std::make_shared<core::SessionWatchdogContext>(
			mTimingProvider,
			std::make_shared<core::util::InterruptibleThreadSuspender>()
		)
	);
}

std::shared_ptr<openkit::ILogger> OpenKitInitializer::getLogger() const
{
	return mLogger;
}

std::shared_ptr<core::configuration::IPrivacyConfiguration> OpenKitInitializer::getPrivacyConfiguration() const
{
	return mPrivacyConfiguration;
}

std::shared_ptr<core::configuration::IOpenKitConfiguration> OpenKitInitializer::getOpenKitConfiguration() const
{
	return mOpenKitConfiguration;
}

std::shared_ptr<providers::ITimingProvider> OpenKitInitializer::getTimingProvider() const
{
	return mTimingProvider;
}

std::shared_ptr<providers::IThreadIDProvider> OpenKitInitializer::getThreadIdProvider() const
{
	return mThreadIdProvider;
}

std::shared_ptr<providers::ISessionIDProvider> OpenKitInitializer::getSessionIdProvider() const
{
	return mSessionIdProvider;
}

std::shared_ptr<core::caching::IBeaconCache> OpenKitInitializer::getBeaconCache() const
{
	return mBeaconCache;
}

std::shared_ptr<core::caching::IBeaconCacheEvictor> OpenKitInitializer::getBeaconCacheEvictor() const
{
	return mBeaconCacheEvictor;
}

std::shared_ptr<core::IBeaconSender> OpenKitInitializer::getBeaconSender() const
{
	return mBeaconSender;
}

std::shared_ptr<core::ISessionWatchdog> OpenKitInitializer::getSessionWatchdog() const
{
	return mSessionWatchdog;
}
