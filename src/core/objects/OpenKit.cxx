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
#include "protocol/HTTPClient.h"
#include "providers/DefaultHTTPClientProvider.h"
#include "providers/DefaultSessionIDProvider.h"
#include "providers/DefaultTimingProvider.h"
#include "providers/DefaultThreadIDProvider.h"
#include "core/BeaconSender.h"
#include "core/caching/BeaconCache.h"
#include "core/caching/BeaconCacheEvictor.h"
#include "core/configuration/BeaconCacheConfiguration.h"
#include "core/configuration/BeaconConfiguration.h"
#include "core/configuration/HTTPClientConfiguration.h"
#include "core/configuration/PrivacyConfiguration.h"
#include "core/configuration/OpenKitConfiguration.h"
#include "core/objects/NullSession.h"

#include <inttypes.h> // for PRId64 macro

using namespace core::objects;

// initialize global instance count with 0.
int32_t OpenKit::gInstanceCount = 0;
std::mutex OpenKit::gInitLock;

OpenKit::OpenKit(
	openkit::IOpenKitBuilder& builder
)
	: mLogger(builder.getLogger())
	, mPrivacyConfiguration(core::configuration::PrivacyConfiguration::from(builder))
	, mOpenKitConfiguration(core::configuration::OpenKitConfiguration::from(builder))
	, mTimingProvider(std::make_shared<providers::DefaultTimingProvider>())
	, mThreadIDProvider(std::make_shared<providers::DefaultThreadIDProvider>())
	, mSessionIDProvider(std::make_shared<providers::DefaultSessionIDProvider>())
	, mBeaconCache(std::make_shared<core::caching::BeaconCache>(mLogger))
	, mBeaconSender(
		std::make_shared<core::BeaconSender>(
			mLogger,
			core::configuration::HTTPClientConfiguration::from(mOpenKitConfiguration),
			std::make_shared<providers::DefaultHTTPClientProvider>(),
			mTimingProvider
		)
	)
	, mBeaconCacheEvictor(
		std::make_shared<core::caching::BeaconCacheEvictor>(
			mLogger,
			mBeaconCache,
			core::configuration::BeaconCacheConfiguration::from(builder),
			mTimingProvider
		)
	)
	, mMutex()
	, mIsShutdown(0)
{
	globalInit();
	logOpenKitInstanceCreation(mLogger, mOpenKitConfiguration);
}

OpenKit::OpenKit(
	std::shared_ptr<openkit::ILogger> logger,
	std::shared_ptr<core::configuration::IPrivacyConfiguration> privacyConfiguration,
	std::shared_ptr<core::configuration::IOpenKitConfiguration> openKitConfiguration,
	std::shared_ptr<providers::ITimingProvider> timingProvider,
	std::shared_ptr<providers::IThreadIDProvider> threadIDProvider,
	std::shared_ptr<providers::ISessionIDProvider> sessionIDProvider,
	std::shared_ptr<core::caching::IBeaconCache> beaconCache,
	std::shared_ptr<core::IBeaconSender> beaconSender,
	std::shared_ptr<core::caching::IBeaconCacheEvictor> beaconCacheEvictor
)
	: mLogger(logger)
	, mPrivacyConfiguration(privacyConfiguration)
	, mOpenKitConfiguration(openKitConfiguration)
	, mTimingProvider(timingProvider)
	, mThreadIDProvider(threadIDProvider)
	, mSessionIDProvider(sessionIDProvider)
	, mBeaconCache(beaconCache)
	, mBeaconSender(beaconSender)
	, mBeaconCacheEvictor(beaconCacheEvictor)
	, mMutex()
	, mIsShutdown(0)
{
	globalInit();
	logOpenKitInstanceCreation(logger, openKitConfiguration);
}


OpenKit::~OpenKit()
{
	globalShutdown();
}

void OpenKit::logOpenKitInstanceCreation(
	std::shared_ptr<openkit::ILogger> logger,
	std::shared_ptr<core::configuration::IOpenKitConfiguration> openKitConfiguration
)
{
	if (logger->isInfoEnabled())
	{
		// TODO: Use proper version information (incl. the build number)
		logger->info("OpenKit() - %s OpenKit %s instantiated",
			openKitConfiguration->getOpenKitType().c_str(),
			openkit::DEFAULT_APPLICATION_VERSION
		);
	}
	if (logger->isDebugEnabled())
	{
		logger->debug("OpenKit() - applicationName=%s, applicationID=%s, deviceID=%ld, origDeviceID=%s, endpointURL=%s",
			openKitConfiguration->getApplicationName().getStringData().c_str(),
			openKitConfiguration->getApplicationId().getStringData().c_str(),
			openKitConfiguration->getDeviceId(),
			openKitConfiguration->getOrigDeviceId().getStringData().c_str(),
			openKitConfiguration->getEndpointUrl().getStringData().c_str());
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
		mLogger->debug("OpenKit createSession(%s)", clientIPAddress != nullptr ? core::UTF8String(clientIPAddress).getStringData().c_str() : "null");
	}

	{ // synchronized scope
		std::lock_guard<std::mutex> lock(mMutex);

		if (!mIsShutdown)
		{
			auto serverId = mBeaconSender->getCurrentServerID();
			auto beaconConfiguration = core::configuration::BeaconConfiguration::from(
					mOpenKitConfiguration,
					mPrivacyConfiguration,
					serverId
			);

			auto beacon = std::make_shared<protocol::Beacon>(
					mLogger,
					mBeaconCache,
					beaconConfiguration,
					clientIPAddress,
					mSessionIDProvider,
					mThreadIDProvider,
					mTimingProvider
			);
			auto newSession = std::make_shared<core::objects::Session>(
				mLogger,
				shared_from_this(),
				beacon
			);
			newSession->startSession();
			mBeaconSender->addSession(newSession);

			storeChildInList(newSession);

			return newSession;
		}
	}

	return NullSession::INSTANCE;
}

std::shared_ptr<openkit::ISession> OpenKit::createSession()
{
	return createSession(nullptr);
}

void OpenKit::shutdown()
{
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("OpenKit shutdown requested");
	}

	{ // synchronized scope
		std::lock_guard<std::mutex> lock(mMutex);

		if(mIsShutdown)
		{
			return;
		}

		mIsShutdown = true;
	}

	// close all child objects
	auto childObjects = getCopyOfChildObjects();
	for (auto childObject : childObjects)
	{
		childObject->close();
	}

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

	protocol::HTTPClient::globalInit();
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

	protocol::HTTPClient::globalDestroy();
}

void OpenKit::onChildClosed(std::shared_ptr<core::objects::IOpenKitObject> childObject)
{
	 // synchronized scope
	std::lock_guard<std::mutex> lock(mMutex);

	removeChildFromList(childObject);
}

void OpenKit::close()
{
	shutdown();
}

std::shared_ptr<openkit::ILogger> OpenKit::getLogger()
{
	return mLogger;
}

std::shared_ptr<core::configuration::IOpenKitConfiguration> OpenKit::getOpenKitConfiguration()
{
	return mOpenKitConfiguration;
}

std::shared_ptr<core::configuration::IPrivacyConfiguration> OpenKit::getPrivacyConfiguration()
{
	return mPrivacyConfiguration;
}

std::shared_ptr<core::caching::IBeaconCache> OpenKit::getBeaconCache()
{
	return mBeaconCache;
}

std::shared_ptr<providers::ISessionIDProvider> OpenKit::getSessionIdProvider()
{
	return mSessionIDProvider;
}

std::shared_ptr<providers::IThreadIDProvider> OpenKit::getThreadIdProvider()
{
	return mThreadIDProvider;
}

std::shared_ptr<providers::ITimingProvider> OpenKit::getTimingProvider()
{
	return mTimingProvider;
}

int32_t OpenKit::getCurrentServerId()
{
	return mBeaconSender->getCurrentServerID();
}