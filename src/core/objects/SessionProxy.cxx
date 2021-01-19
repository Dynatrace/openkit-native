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

#include "SessionProxy.h"
#include "NullRootAction.h"
#include "WebRequestTracer.h"
#include "NullWebRequestTracer.h"

#include <algorithm>
#include <sstream>

using namespace core::objects;

SessionProxy::SessionProxy(std::shared_ptr<openkit::ILogger> logger,
	std::shared_ptr<IOpenKitComposite> parent,
	std::shared_ptr<ISessionCreator> sessionCreator,
	std::shared_ptr<providers::ITimingProvider> timingProvider,
	std::shared_ptr<core::IBeaconSender> beaconSender,
	std::shared_ptr<core::ISessionWatchdog> sessionWatchdog)
	: mLockObject()
	, mLogger(logger)
	, mParent(parent)
	, mSessionCreator(sessionCreator)
	, mTimingProvider(timingProvider)
	, mBeaconSender(beaconSender)
	, mSessionWatchdog(sessionWatchdog)
	, mCurrentSession(nullptr)
	, mCurrentSessionIdentifier(-1, -1)
	, mTopLevelActionCount(0)
	, mLastInteractionTime(0)
	, mServerConfiguration(nullptr)
	, mIsFinished(false)
	, mLastUserTag()
{
}

std::shared_ptr<SessionProxy> SessionProxy::createSessionProxy(std::shared_ptr<openkit::ILogger> logger,
	std::shared_ptr<IOpenKitComposite> parent,
	std::shared_ptr<ISessionCreator> sessionCreator,
	std::shared_ptr<providers::ITimingProvider> timingProvider,
	std::shared_ptr<core::IBeaconSender> beaconSender,
	std::shared_ptr<core::ISessionWatchdog> sessionWatchdog)
{
	auto instance = std::shared_ptr<SessionProxy>(new SessionProxy(logger, parent, sessionCreator, timingProvider, beaconSender, sessionWatchdog));
	instance->createInitialSession();

	return instance;
}

std::shared_ptr<openkit::IRootAction> SessionProxy::enterAction(const char* actionName)
{
	core::UTF8String actionNameString(actionName);
	if (actionNameString.empty())
	{
		mLogger->warning("%s enterAction: actionName must not be null or empty", toString().c_str());
		return NullRootAction::instance();
	}
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("%s enterAction(%s)", toString().c_str(), actionNameString.getStringData().c_str());
	}

	{ // synchronized scope
		std::lock_guard<std::recursive_mutex> lock(mLockObject);

		if (mIsFinished)
		{
			return NullRootAction::instance();
		}

		auto session = getOrSplitCurrentSessionByEvents();
		// avoid session splitting by action count, if user opted out of action collection
		if (session->getBeacon()->isActionReportingAllowedByPrivacySettings())
		{
			recordTopLevelActionEvent();
		}
		else
		{
			recordTopLevelEventInteraction();
		}
		
		return session->enterAction(actionName);
	}
}

void SessionProxy::identifyUser(const char* userTag)
{
	core::UTF8String userTagString(userTag);
	if (userTagString.empty())
	{
		mLogger->warning("%s identifyUser: userTag must not be null or empty", toString().c_str());
		return;
	}
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("%s identifyUser(%s)", toString().c_str(), userTag);
	}

	{ // synchronized scope
		std::lock_guard<std::recursive_mutex> lock(mLockObject);

		if (mIsFinished)
		{
			return;
		}

		auto session = getOrSplitCurrentSessionByEvents();
		recordTopLevelEventInteraction();
		session->identifyUser(userTag);
		mLastUserTag = userTag;
	}
}

void SessionProxy::reportCrash(const char* errorName, const char* reason, const char* stacktrace)
{
	core::UTF8String errorNameString(errorName);
	if (errorNameString.empty())
	{
		mLogger->warning("%s reportCrash: errorName must not be null or empty", toString().c_str());
		return;
	}
	if (mLogger->isDebugEnabled())
	{
		core::UTF8String reasonString(reason);
		core::UTF8String stacktraceString(stacktrace);
		mLogger->debug("%s reportCrash(%s, %s, %s)",
			toString().c_str(),
			errorNameString.getStringData().c_str(),
			reasonString.getStringData().c_str(),
			stacktraceString.getStringData().c_str());
	}

	{ // synchronized scope
		std::lock_guard<std::recursive_mutex> lock(mLockObject);

		if (mIsFinished)
		{
			return;
		}

		auto session = getOrSplitCurrentSessionByEvents();
		recordTopLevelEventInteraction();
		session->reportCrash(errorName, reason, stacktrace);

		// create new session after crash report
		splitAndCreateNewInitialSession();
	}
}

std::shared_ptr<openkit::IWebRequestTracer> SessionProxy::traceWebRequest(const char* url)
{
	core::UTF8String urlString(url);
	if (urlString.empty())
	{
		mLogger->warning("%s traceWebRequest: url must not be null or empty", toString().c_str());
		return NullWebRequestTracer::instance();
	}
	if (!WebRequestTracer::isValidURLScheme(urlString))
	{
		mLogger->warning("%s traceWebRequest: url \"%s\" does not have a valid scheme", toString().c_str(), urlString.getStringData().c_str());
		return NullWebRequestTracer::instance();
	}
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("%s traceWebRequest(%s)", toString().c_str(), urlString.getStringData().c_str());
	}

	{ // synchronized scope
		std::lock_guard<std::recursive_mutex> lock(mLockObject);

		if (mIsFinished)
		{
			return NullWebRequestTracer::instance();
		}

		auto session = getOrSplitCurrentSessionByEvents();
		recordTopLevelEventInteraction();
		return session->traceWebRequest(url);
	}
}

void SessionProxy::end()
{
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("%s end()", toString().c_str());
	}

	{ // synchronized scope
		std::lock_guard<std::recursive_mutex> lock(mLockObject);

		if (mIsFinished)
		{
			return;
		}

		mIsFinished = true;
	}

	auto childObjects = getCopyOfChildObjects();
	for (auto&& childObject : childObjects)
	{
		auto childSession = std::dynamic_pointer_cast<core::objects::SessionInternals>(childObject);
		if (childSession != nullptr)
		{
			childSession->end(childSession == mCurrentSession);
		}
		else
		{
			childObject->close();
		}
	}

	auto thisSession = shared_from_this();

	// detach from parent
	mParent->onChildClosed(thisSession);
	mSessionWatchdog->removeFromSplitByTimeout(thisSession);

	// avoid memory leak by resetting curent session
	// TODO stefan.eberl - refactor this by using a weak pointer
	mCurrentSession = nullptr;
}

bool SessionProxy::isFinished()
{
	std::lock_guard<std::recursive_mutex> lock(mLockObject);

	return mIsFinished;
}

void SessionProxy::close()
{
	end();
}

void SessionProxy::onChildClosed(std::shared_ptr<IOpenKitObject> childObject)
{
	std::lock_guard<std::recursive_mutex> lock(mLockObject);
	
	removeChildFromList(childObject);
	
	// if child is a session remove it from watchdog
	auto childSession = std::dynamic_pointer_cast<SessionInternals>(childObject);
	if (childSession != nullptr)
	{
		mSessionWatchdog->dequeueFromClosing(childSession);
	}
}

void SessionProxy::createInitialSession()
{
	mCurrentSession = createSession(mBeaconSender->getLastServerConfiguration(), nullptr);
	updateCurrentSessionIdentifier();
}

std::shared_ptr<SessionInternals> SessionProxy::createSplitSession(std::shared_ptr<core::configuration::IServerConfiguration> updatedServerConfig)
{
	return createSession(nullptr, updatedServerConfig);
}

std::shared_ptr<core::objects::SessionInternals> SessionProxy::getOrSplitCurrentSessionByEvents()
{
	if (isSessionSplitByEventsRequired())
	{
		closeOrEnqueueCurrentSessionForClosing();

		mCurrentSession = createSplitSession(mServerConfiguration);
		
		updateCurrentSessionIdentifier();
		reTagCurrentSession();
	}

	return mCurrentSession;
}

bool SessionProxy::isSessionSplitByEventsRequired() const
{
	if (mServerConfiguration == nullptr || !mServerConfiguration->isSessionSplitByEventsEnabled())
	{
		return false;
	}

	return mServerConfiguration->getMaxEventsPerSession() <= mTopLevelActionCount;
}

std::shared_ptr<SessionInternals> SessionProxy::createSession(
	std::shared_ptr<core::configuration::IServerConfiguration> initialServerConfig,
	std::shared_ptr<core::configuration::IServerConfiguration> updatedServerConfig
)
{
	auto session = mSessionCreator->createSession(shared_from_this());
	auto beacon = session->getBeacon();

	beacon->setServerConfigurationUpdateCallback(
		std::bind(&SessionProxy::onServerConfigurationUpdate, this, std::placeholders::_1));

	storeChildInList(session);

	mLastInteractionTime = beacon->getSessionStartTime();
	mTopLevelActionCount = 0;

	if (initialServerConfig != nullptr)
	{
		session->initializeServerConfiguration(initialServerConfig);
	}
	if (updatedServerConfig != nullptr)
	{
		session->updateServerConfiguration(updatedServerConfig);
	}

	mBeaconSender->addSession(session);

	return session;
}

void SessionProxy::updateCurrentSessionIdentifier()
{
	auto beacon = mCurrentSession->getBeacon();
	mCurrentSessionIdentifier.first = beacon->getSessionNumber();
	mCurrentSessionIdentifier.second = beacon->getSessionSequenceNumber();
}

void SessionProxy::splitAndCreateNewInitialSession()
{
	closeOrEnqueueCurrentSessionForClosing();

	// create a completely new Session
	mSessionCreator->reset();
	mCurrentSession = createSession(mServerConfiguration, nullptr);

	updateCurrentSessionIdentifier();
	reTagCurrentSession();
}

void SessionProxy::closeOrEnqueueCurrentSessionForClosing()
{
	// for grace period use half of the idle timeout
	// or fallback to session interval if not configured
	auto closeGracePeriodInMillis = mServerConfiguration->getSessionTimeoutInMilliseconds() > 0
		? mServerConfiguration->getSessionTimeoutInMilliseconds() / 2
		: mServerConfiguration->getSendIntervalInMilliseconds();

	mSessionWatchdog->closeOrEnqueueForClosing(mCurrentSession, closeGracePeriodInMillis);
}

void SessionProxy::recordTopLevelEventInteraction()
{
	mLastInteractionTime = mTimingProvider->provideTimestampInMilliseconds();
}

void SessionProxy::recordTopLevelActionEvent()
{
	mTopLevelActionCount++;
	recordTopLevelEventInteraction();
}

int32_t SessionProxy::getTopLevelActionCount()
{
	std::lock_guard<std::recursive_mutex> lock(mLockObject);
	return mTopLevelActionCount;
}

int64_t SessionProxy::getLastInteractionTime()
{
	std::lock_guard<std::recursive_mutex> lock(mLockObject);
	return mLastInteractionTime;
}

std::shared_ptr<core::configuration::IServerConfiguration> SessionProxy::getServerConfiguration()
{
	std::lock_guard<std::recursive_mutex> lock(mLockObject);
	return mServerConfiguration;
}

void SessionProxy::onServerConfigurationUpdate(std::shared_ptr<core::configuration::IServerConfiguration> serverConfig)
{
	std::lock_guard<std::recursive_mutex> lock(mLockObject);
	if (mServerConfiguration != nullptr)
	{
		mServerConfiguration = mServerConfiguration->merge(serverConfig);
		return;
	}

	mServerConfiguration = serverConfig;
	if (mServerConfiguration->isSessionSplitBySessionDurationEnabled()
		|| mServerConfiguration->isSessionSplitByIdleTimeoutEnabled())
	{
		mSessionWatchdog->addToSplitByTimeout(shared_from_this());
	}
}

int64_t SessionProxy::splitSessionByTime()
{
	std::lock_guard<std::recursive_mutex> lock(mLockObject);
	if (isFinished())
	{
		return -1;
	}

	auto nextSplitTime = calculateNextSplitTime();
	auto now = mTimingProvider->provideTimestampInMilliseconds();
	if (nextSplitTime < 0 || now < nextSplitTime)
	{
		return nextSplitTime;
	}

	splitAndCreateNewInitialSession();

	return calculateNextSplitTime();
}

int64_t SessionProxy::calculateNextSplitTime()
{
	if (mServerConfiguration == nullptr)
	{
		return -1;
	}

	auto splitByIdleTimeout = mServerConfiguration->isSessionSplitByIdleTimeoutEnabled();
	auto splitBySessionDuration = mServerConfiguration->isSessionSplitBySessionDurationEnabled();

	auto idleTimeout = mLastInteractionTime + mServerConfiguration->getSessionTimeoutInMilliseconds();
	auto sessionMaxTime = mCurrentSession->getBeacon()->getSessionStartTime() +
		mServerConfiguration->getMaxSessionDurationInMilliseconds();

	if (splitByIdleTimeout && splitBySessionDuration)
	{
		return std::min(idleTimeout, sessionMaxTime);
	}
	else if (splitByIdleTimeout)
	{
		return idleTimeout;
	}
	else if (splitBySessionDuration)
	{
		return sessionMaxTime;
	}

	return -1;
}

void SessionProxy::reTagCurrentSession()
{
	if (mLastUserTag.empty())
	{
		return;
	}

	mCurrentSession->identifyUser(mLastUserTag.getStringData().c_str());
}

const std::string SessionProxy::toString() const
{
	std::stringstream ss;
	ss << "SessionProxy [sn=" << mCurrentSessionIdentifier.first << ", seq=" << mCurrentSessionIdentifier.second << "]";
	return ss.str();
}