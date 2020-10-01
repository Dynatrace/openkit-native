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

#include "Session.h"
#include "RootAction.h"
#include "WebRequestTracer.h"
#include "core/IBeaconSender.h"
#include "protocol/IBeacon.h"

#include <sstream>

using namespace core::objects;

Session::Session(
	std::shared_ptr<openkit::ILogger> logger,
	std::shared_ptr<core::objects::IOpenKitComposite> parent,
	std::shared_ptr<protocol::IBeacon> beacon
)
	: mLogger(logger)
	, mParent(parent)
	, mBeacon(beacon)
	, mNumRemainingNewSessionRequests(MAX_NEW_SESSION_REQUESTS)
	, mIsSessionFinishing(false)
	, mIsSessionFinished(false)
	, mMutex()
	, mSplitByEventsGracePeriodEndTimeInMillis(-1)
{
}

void Session::startSession()
{
	mBeacon->startSession();
}

std::shared_ptr<openkit::IRootAction> Session::enterAction(const char* actionName)
{
	UTF8String actionNameString(actionName);
	if (actionNameString.empty())
	{
		mLogger->warning("%s enterAction: actionName must not be null or empty", toString().c_str());
		return NullRootAction::INSTANCE;
	}
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("%s enterAction(%s)", toString().c_str(), actionName);
	}

	{ // synchronized scope
		std::lock_guard<std::recursive_mutex> lock(mMutex);

		if (!isFinishingOrFinished())
		{
			auto rootActionImpl = std::make_shared<ActionCommonImpl>(
				mLogger,
				mBeacon,
				shared_from_this(),
				actionNameString,
				"RootAction"
			);
			storeChildInList(rootActionImpl);

			auto rootAction = std::make_shared<RootAction>(rootActionImpl);
			return rootAction;
		}
	}

	return NullRootAction::INSTANCE;
}

void Session::identifyUser(const char* userTag)
{
	UTF8String userTagString(userTag);

	if (userTag == nullptr || userTagString.empty())
	{
		mLogger->warning("%s identifyUser: userTag must not be null or empty", toString().c_str());
		return;
	}
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("%s identifyUser(%s)", toString().c_str(), userTag);
	}

	{ // synchronized scope
		std::lock_guard<std::recursive_mutex> lock(mMutex);

		if (!isFinishingOrFinished())
		{
			mBeacon->identifyUser(userTagString);
		}
	}
}

void Session::reportCrash(const char* errorName, const char* reason, const char* stacktrace)
{
	UTF8String errorNameString(errorName);

	if (errorName == nullptr || errorNameString.empty())
	{
		mLogger->warning("%s reportCrash: errorName must not be null or empty", toString().c_str());
		return;
	}

	UTF8String reasonString(reason);
	UTF8String stacktraceString(stacktrace);
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("%s reportCrash(%s, %s, %s)", toString().c_str(), errorNameString.getStringData().c_str(),
				(reason != nullptr ? reasonString.getStringData().c_str() : "null"),
				(stacktrace != nullptr ? stacktraceString.getStringData().c_str() : "null"));
	}

	{ // synchronized scope
		std::lock_guard<std::recursive_mutex> lock(mMutex);

		if (!isFinishingOrFinished())
		{
			mBeacon->reportCrash(errorNameString, reasonString, stacktraceString);
		}
	}
}

std::shared_ptr<openkit::IWebRequestTracer> Session::traceWebRequest(const char* url)
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

	{ // synchronized block
		std::lock_guard<std::recursive_mutex> lock(mMutex);

		if(!isFinishingOrFinished())
		{
			auto tracer =  std::make_shared<core::objects::WebRequestTracer>(
				mLogger,
				shared_from_this(),
				mBeacon,
				urlString
			);
			storeChildInList(tracer);

			return tracer;
		}
	}

	return NullWebRequestTracer::INSTANCE;
}

void Session::end()
{
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("%s end()", toString().c_str());
	}

	{ // synchronized scope
		std::lock_guard<std::recursive_mutex> lock(mMutex);

		if (!markAsIsFinishing())
		{
			return; // end() was already called before
		}
	}

	// leave all Root-Actions for sanity reasons
	auto childObjects = getCopyOfChildObjects();
	for (auto childObject : childObjects)
	{
		childObject->close();
	}

	mBeacon->endSession();

	{ // synchronized scope
		std::lock_guard<std::recursive_mutex> lock(mMutex);

		markAsFinished();
	}

	// last but not least update parent relation
	mParent->onChildClosed(shared_from_this());
}

void Session::close()
{
	end();
}

std::shared_ptr<protocol::IStatusResponse> Session::sendBeacon(std::shared_ptr<providers::IHTTPClientProvider> clientProvider)
{
	return mBeacon->send(clientProvider);
}

bool Session::isEmpty() const
{
	return mBeacon->isEmpty();
}

void Session::clearCapturedData()
{
	mBeacon->clearData();
}

bool Session::tryEnd()
{
	std::lock_guard<std::recursive_mutex> lock(mMutex);

	if (isFinishingOrFinished())
	{
		return true;
	}

	if (getChildCount() == 0)
	{
		end();
		return true;
	}

	return false;
}

int64_t Session::getSplitByEventsGracePeriodEndTimeInMillis()
{
	return mSplitByEventsGracePeriodEndTimeInMillis;
}

void Session::setSplitByEventsGracePeriodEndTimeInMillis(int64_t splitByEventsGracePeriodEndTimeInMillis)
{
	mSplitByEventsGracePeriodEndTimeInMillis = splitByEventsGracePeriodEndTimeInMillis;
}

void Session::updateServerConfiguration(std::shared_ptr<core::configuration::IServerConfiguration> serverConfig)
{
	mBeacon->updateServerConfiguration(serverConfig);
}

void Session::onChildClosed(std::shared_ptr<core::objects::IOpenKitObject> childObject)
{
	{ // synchronized scope
		std::lock_guard<std::recursive_mutex> lock(mMutex);

		removeChildFromList(childObject);
	}
}

bool Session::isDataSendingAllowed()
{
	return isConfigured() && mBeacon->isCaptureEnabled();
}

void Session::enableCapture()
{
	mBeacon->enableCapture();
}

void Session::disableCapture()
{
	mBeacon->disableCapture();
}

bool Session::canSendNewSessionRequest() const
{
	return mNumRemainingNewSessionRequests > 0;
}

void Session::decreaseNumRemainingSessionRequests()
{
	mNumRemainingNewSessionRequests--;
}

std::shared_ptr<protocol::IBeacon> Session::getBeacon()
{
	return mBeacon;
}

const std::string Session::toString() const
{
	std::stringstream ss;
	ss << "Session [sn=" << mBeacon->getSessionNumber() << "]";
	return ss.str();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Session state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Session::isConfigured()
{
	{ // synchronized scope
		std::lock_guard<std::recursive_mutex> lock(mMutex);

		return hasServerConfigurationSet();
	}
}

bool Session::isConfiguredAndFinished()
{
	{ // synchronized scope
		std::lock_guard<std::recursive_mutex> lock(mMutex);

		return hasServerConfigurationSet() && mIsSessionFinished;
	}
}

bool Session::isConfiguredAndOpen()
{
	{ // synchronized scope
		std::lock_guard<std::recursive_mutex> lock(mMutex);

		return hasServerConfigurationSet() && !mIsSessionFinished;
	}
}

bool Session::isFinished()
{
	{ // synchronized scope
		std::lock_guard<std::recursive_mutex> lock(mMutex);

		return mIsSessionFinished;
	}
}

bool Session::markAsIsFinishing()
{
	if (isFinishingOrFinished())
	{
		return false;
	}

	mIsSessionFinishing = true;
	return true;
}

void Session::markAsFinished()
{
	mIsSessionFinished = true;
}

bool Session::hasServerConfigurationSet()
{
	return mBeacon->isServerConfigurationSet();
}

bool Session::isFinishingOrFinished()
{
	return mIsSessionFinishing || mIsSessionFinished;
}
