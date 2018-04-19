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

#include "RootAction.h"

#include <memory>

#include "NullWebRequestTracer.h"
#include "WebRequestTracerStringURL.h"

using namespace core;

RootAction::RootAction(std::shared_ptr<openkit::ILogger> logger, std::shared_ptr<protocol::Beacon> beacon, const UTF8String& name, std::shared_ptr<Session> session)
	: mLogger(logger)
	, mBeacon(beacon)
	, mOpenChildActions()
	, mSession(session)
	, mID(mBeacon->createID())
	, mName(name)
	, mStartTime(mBeacon->getCurrentTimestamp())
	, mStartSequenceNumber(mBeacon->createSequenceNumber())
	, mEndSequenceNumber(-1)
	, mEndTime(-1)
	, NULL_ACTION(std::make_shared<NullAction>())
	, NULL_WEB_REQUEST_TRACER(std::make_shared<NullWebRequestTracer>())
{

}

std::shared_ptr<openkit::IAction> RootAction::enterAction(const char* actionName)
{
	UTF8String actionNameString(actionName);
	if (actionNameString.empty())
	{
		mLogger->warning("RootAction.enterAction: actionName must not be null or empty");
		return NULL_ACTION;
	}

	if (!isActionLeft())
	{
		auto childAction = std::make_shared<Action>(mLogger, mBeacon, UTF8String(actionName), shared_from_this());
		mOpenChildActions.put(std::static_pointer_cast<openkit::IAction>(childAction));
		return childAction;
	}
	return NULL_ACTION;
}

std::shared_ptr<openkit::IRootAction> RootAction::reportEvent(const char* eventName)
{
	UTF8String eventNameString(eventName);
	if (eventNameString.empty())
	{
		return shared_from_this();
	}

	if (!isActionLeft())
	{
		mBeacon->reportEvent(shared_from_this(), eventNameString);
	}
	return shared_from_this();
}

std::shared_ptr<openkit::IRootAction> RootAction::reportValue(const char* valueName, int32_t value)
{
	UTF8String valueNameString(valueName);
	if (valueNameString.empty())
	{
		return shared_from_this();
	}

	if (!isActionLeft())
	{
		mBeacon->reportValue(shared_from_this(), valueNameString, value);
	}
	return shared_from_this();
}

std::shared_ptr<openkit::IRootAction> RootAction::reportValue(const char* valueName, double value)
{
	UTF8String valueNameString(valueName);
	if (valueNameString.empty())
	{
		return shared_from_this();
	}

	if (!isActionLeft())
	{
		mBeacon->reportValue(shared_from_this(), valueNameString, value);
	}
	return shared_from_this();
}

std::shared_ptr<openkit::IRootAction> RootAction::reportValue(const char* valueName, const char* value)
{
	UTF8String valueNameString(valueName);
	if (valueNameString.empty())
	{
		return shared_from_this();
	}

	if (!isActionLeft())
	{
		mBeacon->reportValue(shared_from_this(), valueNameString, value);
	}
	return shared_from_this();
}

std::shared_ptr<openkit::IRootAction> RootAction::reportError(const char* errorName, int32_t errorCode, const char* reason)
{
	UTF8String errorNameString(errorName);
	UTF8String reasonString(reason);
	if (errorNameString.empty())
	{
		return shared_from_this();
	}

	if (!isActionLeft())
	{
		mBeacon->reportError(shared_from_this(), errorNameString, errorCode, reasonString);
	}
	return shared_from_this();
}

std::shared_ptr<openkit::IWebRequestTracer> RootAction::traceWebRequest(const char* url)
{
	core::UTF8String urlString(url);
	if (urlString.empty())
	{
		mLogger->warning("Action.traceWebRequest (URLConnection): connection must not be null");
		return NULL_WEB_REQUEST_TRACER;
	}
	if (!isActionLeft())
	{
		return std::make_shared<core::WebRequestTracerStringURL>(mBeacon, getID(), urlString);
	}

	return NULL_WEB_REQUEST_TRACER;
}

void RootAction::doLeaveAction()
{
	// add Action to Beacon
	mBeacon->addAction(shared_from_this());

	while (!mOpenChildActions.isEmpty())
	{
		auto action = mOpenChildActions.get();
		action->leaveAction();
	}

	// leave event of the root action must be later than the leaveAction calls of the childs
	mEndTime = mBeacon->getCurrentTimestamp();
	mEndSequenceNumber = mBeacon->createSequenceNumber();

	mSession->rootActionEnded(std::static_pointer_cast<RootAction>(shared_from_this()));
	mSession = nullptr;
}

void RootAction::leaveAction()
{
	int64_t expected = -1L;
	if (atomic_compare_exchange_strong(&mEndTime, &expected, mBeacon->getCurrentTimestamp()) == false)
	{
		return;
	}

	doLeaveAction();
}

void RootAction::childActionEnded(std::shared_ptr<Action> childAction)
{
	mOpenChildActions.remove(childAction);
}

int32_t RootAction::getID() const
{
	return mID;
}

const UTF8String& RootAction::getName() const
{
	return mName;
}

int64_t RootAction::getStartTime() const
{
	return mStartTime;
}

int64_t RootAction::getEndTime() const
{
	return mEndTime;
}

int32_t RootAction::getStartSequenceNo() const
{
	return mStartSequenceNumber;
}

int32_t RootAction::getEndSequenceNo() const
{
	return mEndSequenceNumber;
}

bool RootAction::isActionLeft() const
{
	return mEndTime != -1;
}

bool RootAction::hasOpenChildActions() const
{
	return !mOpenChildActions.isEmpty();
}