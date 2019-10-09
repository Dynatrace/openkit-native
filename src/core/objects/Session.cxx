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

#include "Session.h"
#include "protocol/Beacon.h"
#include "core/BeaconSender.h"
#include "Action.h"
#include "RootAction.h"
#include "WebRequestTracer.h"

#include <sstream>

using namespace core::objects;

Session::Session
(
	std::shared_ptr<openkit::ILogger> logger,
	std::shared_ptr<core::BeaconSender> beaconSender,
	std::shared_ptr<protocol::Beacon> beacon
)
	: mLogger(logger)
	, mBeaconSender(beaconSender)
	, mBeacon(beacon)
	, mEndTime(-1)
	, mOpenRootActions()
{
}

void Session::startSession()
{
	mBeacon->startSession();
	mBeaconSender->startSession(shared_from_this());
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

	if (isSessionEnded())
	{
		return NullRootAction::INSTANCE;
	}
	std::shared_ptr<openkit::IRootAction> pointer = std::make_shared<RootAction>(mLogger, mBeacon, actionNameString, shared_from_this());
	mOpenRootActions.put(pointer);
	return pointer;
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

	if (!isSessionEnded())
	{
		mBeacon->identifyUser(userTagString);
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

	if (!isSessionEnded())
	{
		mBeacon->reportCrash(errorNameString, reasonString, stacktraceString);
	}
}

std::shared_ptr<openkit::IWebRequestTracer> Session::traceWebRequest(const char* url)
{
	core::UTF8String urlString(url);
	if (urlString.empty())
	{
		mLogger->warning("%s traceWebRequest (string): url must not be null or empty", toString().c_str());
		return NullWebRequestTracer::INSTANCE;
	}
	if (!WebRequestTracer::isValidURLScheme(urlString))
	{
		mLogger->warning("%s traceWebRequest (string): url \"%s\" does not have a valid scheme", toString().c_str(), urlString.getStringData().c_str());
		return NullWebRequestTracer::INSTANCE;
	}
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("%s traceWebRequest (string) (%s))", toString().c_str(), urlString.getStringData().c_str());
	}

	if (!isSessionEnded())
	{
		return std::make_shared<core::objects::WebRequestTracer>(mLogger, shared_from_this(), mBeacon, urlString);
	}
	return NullWebRequestTracer::INSTANCE;
}

void Session::end()
{
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("%s end()", toString().c_str());
	}
	int64_t expected = -1L;
	if (atomic_compare_exchange_strong(&mEndTime, &expected, mBeacon->getCurrentTimestamp()) == false)
	{
		return;
	}

	// leave all Root-Actions for sanity reasons
	while (!mOpenRootActions.isEmpty()) {
		auto action = mOpenRootActions.get();
		action->leaveAction();
	}

	mBeacon->endSession(shared_from_this());

	mBeaconSender->finishSession(shared_from_this());
}

void Session::close()
{
	end();
}

bool Session::isSessionEnded() const
{
	return mEndTime != -1L;
}

int64_t Session::getEndTime() const
{
	return mEndTime;
}

void Session::rootActionEnded(std::shared_ptr<RootAction> rootAction)
{
	mOpenRootActions.remove(rootAction);
}

std::shared_ptr<protocol::StatusResponse> Session::sendBeacon(std::shared_ptr<providers::IHTTPClientProvider> clientProvider)
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

void Session::onChildClosed(std::shared_ptr<core::objects::IOpenKitObject> childObject)
{
	removeChildFromList(childObject);
}

const std::string Session::toString() const
{
	std::stringstream ss;
	ss << "Session [sn=" << mBeacon->getSessionNumber() << "]";
	return ss.str();
}

void Session::setBeaconConfiguration(std::shared_ptr<core::configuration::BeaconConfiguration> beaconConfiguration)
{
	mBeacon->setBeaconConfiguration(beaconConfiguration);
}

std::shared_ptr<core::configuration::BeaconConfiguration> Session::getBeaconConfiguration() const
{
	return mBeacon->getBeaconConfiguration();
}