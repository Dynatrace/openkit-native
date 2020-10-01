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

using namespace core::objects;

SessionProxy::SessionProxy(std::shared_ptr<openkit::ILogger> logger,
	std::shared_ptr<IOpenKitComposite> parent,
	std::shared_ptr<ISessionCreator> sessionCreator,
	std::shared_ptr<core::IBeaconSender> beaconSender,
	std::shared_ptr<core::ISessionWatchdog> sessionWatchdog)
	: mLockObject()
	, mLogger(logger)
	, mParent(parent)
	, mSessionCreator(sessionCreator)
	, mBeaconSender(beaconSender)
	, mSessionWatchdog(sessionWatchdog)
	, mCurrentSession(nullptr)
	, mTopLevelActionCount(0)
	, mLastInteractionTime(0)
	, mServerConfiguration(nullptr)
	, mIsFinished(false)
{
	
}

std::shared_ptr<SessionProxy> SessionProxy::createSessionProxy(std::shared_ptr<openkit::ILogger> logger,
	std::shared_ptr<IOpenKitComposite> parent,
	std::shared_ptr<ISessionCreator> sessionCreator,
	std::shared_ptr<core::IBeaconSender> beaconSender,
	std::shared_ptr<core::ISessionWatchdog> sessionWatchdog)
{
	auto instance = std::shared_ptr<SessionProxy>(new SessionProxy(logger, parent, sessionCreator, beaconSender, sessionWatchdog));
	instance->createInitialSession();

	return instance;
}

std::shared_ptr<openkit::IRootAction> SessionProxy::enterAction(const char* actionName)
{
	core::UTF8String actionNameString(actionName);
	if (actionNameString.empty())
	{
		mLogger->warning("%s enterAction: actionName must not be null or empty", toString().c_str());
		return NullRootAction::INSTANCE;
	}
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("%s enterAction(%s)", toString().c_str(), actionNameString.getStringData().c_str());
	}

	{ // synchronized scope
		std::lock_guard<std::mutex> lock(mLockObject);

		if (mIsFinished)
		{
			return NullRootAction::INSTANCE;
		}

		auto session = getOrSplitCurrentSession();
		recordTopLevelActionEvent();
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
		std::lock_guard<std::mutex> lock(mLockObject);

		if (mIsFinished)
		{
			return;
		}

		auto session = getOrSplitCurrentSession();
		recordTopLevelEventInteraction();
		return session->identifyUser(userTag);
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
		std::lock_guard<std::mutex> lock(mLockObject);

		if (mIsFinished)
		{
			return;
		}

		auto session = getOrSplitCurrentSession();
		recordTopLevelEventInteraction();
		return session->reportCrash(errorName, reason, stacktrace);
	}
}

std::shared_ptr<openkit::IWebRequestTracer> SessionProxy::traceWebRequest(const char* url)
{
	core::UTF8String urlString(url);
	if (urlString.empty())
	{
		mLogger->warning("%s traceWebRequest: url must not be null or empty", toString().c_str());
		return NullWebRequestTracer::INSTANCE;
	}
	if (!WebRequestTracer::isValidURLScheme(urlString))
	{
		mLogger->warning("%s traceWebRequest: url \"%s\" does not have a valid scheme", toString().c_str(), urlString.getStringData().c_str());
		return NullWebRequestTracer::INSTANCE;
	}
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("%s traceWebRequest(%s)", toString().c_str(), urlString.getStringData().c_str());
	}

	{ // synchronized scope
		std::lock_guard<std::mutex> lock(mLockObject);

		if (mIsFinished)
		{
			return NullWebRequestTracer::INSTANCE;
		}

		auto session = getOrSplitCurrentSession();
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
		std::lock_guard<std::mutex> lock(mLockObject);

		if (mIsFinished)
		{
			return;
		}

		mIsFinished = true;
	}

	auto childObjects = getCopyOfChildObjects();
	for (auto&& childObject : childObjects)
	{
		childObject->close();
	}

	auto thisSession = shared_from_this();

	// detach from parent
	mParent->onChildClosed(thisSession);
}

bool SessionProxy::isFinished()
{
	std::lock_guard<std::mutex> lock(mLockObject);

	return mIsFinished;
}

void SessionProxy::close()
{
	end();
}

void SessionProxy::onChildClosed(std::shared_ptr<IOpenKitObject> childObject)
{
	std::lock_guard<std::mutex> lock(mLockObject);
	
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
	mCurrentSession = createSession(nullptr);
}

std::shared_ptr<openkit::ISession> SessionProxy::getOrSplitCurrentSession()
{
	if (isSessionSplitRequired())
	{
		auto newSession = createSession(mServerConfiguration);
		mTopLevelActionCount = 0;

		// try to close old session or wait half the max session duration and then close it forcefully
		auto closeGracePeriodMillis = mServerConfiguration->getMaxSessionDurationInMilliseconds() / 2;
		mSessionWatchdog->closeOrEnqueueForClosing(mCurrentSession, closeGracePeriodMillis);

		mCurrentSession = newSession;
	}

	return mCurrentSession;
}

bool SessionProxy::isSessionSplitRequired() const
{
	if (mServerConfiguration == nullptr || !mServerConfiguration->isSessionSplitByEventsEnabled())
	{
		return false;
	}

	return mServerConfiguration->getMaxEventsPerSession() <= mTopLevelActionCount;
}

std::shared_ptr<SessionInternals> SessionProxy::createSession(std::shared_ptr<core::configuration::IServerConfiguration> sessionServerConfig)
{
	auto session = mSessionCreator->createSession(shared_from_this());
	session->getBeacon()->setServerConfigurationUpdateCallback(
		std::bind(&SessionProxy::onServerConfigurationUpdate, this, std::placeholders::_1));

	storeChildInList(session);

	if (sessionServerConfig != nullptr)
	{
		session->updateServerConfiguration(sessionServerConfig);
	}

	mBeaconSender->addSession(session);

	return session;
}

void SessionProxy::recordTopLevelEventInteraction()
{
	mLastInteractionTime = mCurrentSession->getBeacon()->getCurrentTimestamp();
}

void SessionProxy::recordTopLevelActionEvent()
{
	mTopLevelActionCount++;
	recordTopLevelEventInteraction();
}

int32_t SessionProxy::getTopLevelActionCount()
{
	std::lock_guard<std::mutex> lock(mLockObject);
	return mTopLevelActionCount;
}

int64_t SessionProxy::getLastInteractionTime()
{
	std::lock_guard<std::mutex> lock(mLockObject);
	return mLastInteractionTime;
}

void SessionProxy::onServerConfigurationUpdate(std::shared_ptr<core::configuration::IServerConfiguration> serverConfig)
{
	std::lock_guard<std::mutex> lock(mLockObject);
	if (mServerConfiguration == nullptr)
	{
		mServerConfiguration = serverConfig;
	}
}

const std::string SessionProxy::toString() const
{
	return "SessionProxy";
}