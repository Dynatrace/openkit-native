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

#include "OpenKit.h"
#include "core/objects/NullSession.h"
#include "core/objects/SessionCreator.h"
#include "core/objects/SessionProxy.h"

#include <inttypes.h> // for PRId64 macro

using namespace core::objects;

// initialize global instance count with 0.
int32_t OpenKit::gInstanceCount = 0;
std::mutex OpenKit::gInitLock;

OpenKit::OpenKit(const core::objects::IOpenKitInitializer& initializer)
	: mLogger(initializer.getLogger())
	, mPrivacyConfiguration(initializer.getPrivacyConfiguration())
	, mOpenKitConfiguration(initializer.getOpenKitConfiguration())
	, mTimingProvider(initializer.getTimingProvider())
	, mThreadIDProvider(initializer.getThreadIdProvider())
	, mSessionIDProvider(initializer.getSessionIdProvider())
	, mBeaconCache(initializer.getBeaconCache())
	, mBeaconSender(initializer.getBeaconSender())
	, mBeaconCacheEvictor(initializer.getBeaconCacheEvictor())
	, mSessionWatchdog(initializer.getSessionWatchdog())
	, mMutex()
	, mIsShutdown(0)
{
	globalInit();
	logOpenKitInstanceCreation(mLogger, mOpenKitConfiguration);
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
		logger->debug("OpenKit() - applicationName=%s, applicationID=%s, deviceID=%" PRId64  ", origDeviceID=%s, endpointURL=%s",
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
	mSessionWatchdog->initialize();
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

		if (mIsShutdown)
		{
			return NullSession::instance();
		}

		auto sessionCreator = std::make_shared<core::objects::SessionCreator>(*this, clientIPAddress);
		auto sessionProxy = core::objects::SessionProxy::createSessionProxy(
			mLogger,
			shared_from_this(),
			sessionCreator,
			mBeaconSender,
			mSessionWatchdog
		);

		storeChildInList(sessionProxy);

		return sessionProxy;
	}
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
	mSessionWatchdog->shutdown();
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