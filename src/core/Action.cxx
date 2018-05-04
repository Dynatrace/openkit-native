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

#include "Action.h"
#include "protocol/Beacon.h"
#include "RootAction.h"

#include <atomic>
#include <memory>
#include <sstream>

#include "NullWebRequestTracer.h"
#include "WebRequestTracerStringURL.h"

using namespace core;

Action::Action(std::shared_ptr<openkit::ILogger> logger, std::shared_ptr<protocol::Beacon> beacon, const UTF8String& name)
	: Action(logger, beacon, name, nullptr)
{

}

Action::Action(std::shared_ptr<openkit::ILogger> logger, std::shared_ptr<protocol::Beacon> beacon, const UTF8String& name, std::shared_ptr<RootAction> parentAction)
	: mLogger(logger)
	, mParentAction(parentAction)
	, mEndTime(-1)
	, mBeacon(beacon)
	, mID(mBeacon->createID())
	, mName(name)
	, mStartTime(mBeacon->getCurrentTimestamp())
	, mStartSequenceNumber(mBeacon->createSequenceNumber())
	, mEndSequenceNumber(-1)
	, NULL_WEB_REQUEST_TRACER(std::make_shared<NullWebRequestTracer>())
{

}

std::shared_ptr<openkit::IAction> Action::reportEvent(const char* eventName)
{
	UTF8String eventNameString(eventName);
	if (eventNameString.empty())
	{
		mLogger->warning("%s reportEvent: eventName must not be null or empty", toString().c_str());
		return shared_from_this();
	}
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("%s reportEvent(%s)", toString().c_str(), eventName);
	}

	if (!isActionLeft())
	{
		mBeacon->reportEvent(shared_from_this(), eventNameString);
	}
	return shared_from_this();
}

std::shared_ptr<openkit::IAction> Action::reportValue(const char* valueName, int32_t value)
{
	UTF8String valueNameString(valueName);
	if (valueNameString.empty())
	{
		mLogger->warning("%s reportValue (int): valueName must not be null or empty", toString().c_str());
		return shared_from_this();
	}
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("%s reportValue (int) (%s, %d))", toString().c_str(), valueName, value);
	}

	if (!isActionLeft())
	{
		mBeacon->reportValue(shared_from_this(), valueNameString, value);
	}
	return shared_from_this();
}

std::shared_ptr<openkit::IAction> Action::reportValue(const char* valueName, double value)
{
	UTF8String valueNameString(valueName);
	if (valueNameString.empty())
	{
		mLogger->warning("%s reportValue (double): valueName must not be null or empty", toString().c_str());
		return shared_from_this();
	}
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("%s reportValue (double) (%s, %f))", toString().c_str(), valueName, value);
	}

	if (!isActionLeft())
	{
		mBeacon->reportValue(shared_from_this(), valueNameString, value);
	}
	return shared_from_this();
}

std::shared_ptr<openkit::IAction> Action::reportValue(const char* valueName, const char* value)
{
	UTF8String valueNameString(valueName);
	if (valueNameString.empty())
	{
		mLogger->warning("%s reportValue (string): valueName must not be null or empty", toString().c_str());
		return shared_from_this();
	}
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("%s reportValue (string) (%s, %s))", toString().c_str(), valueName, (value != nullptr ? value : "null"));
	}

	if (!isActionLeft())
	{
		mBeacon->reportValue(shared_from_this(), valueNameString, value);
	}
	return shared_from_this();
}

std::shared_ptr<openkit::IAction> Action::reportError(const char* errorName, int32_t errorCode, const char* reason)
{
	UTF8String errorNameString(errorName);
	UTF8String reasonString(reason);
	if (errorNameString.empty())
	{
		mLogger->warning("%s reportError: errorName must not be null or empty", toString().c_str());
		return shared_from_this();
	}
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("%s reportError (%s, %d, %s))", toString().c_str(), errorName, errorCode, (reason != nullptr ? reason : "null"));
	}

	if (!isActionLeft())
	{
		mBeacon->reportError(shared_from_this(), errorNameString, errorCode, reasonString);
	}
	return shared_from_this();
}

std::shared_ptr<openkit::IWebRequestTracer> Action::traceWebRequest(const char* url)
{
	core::UTF8String urlString(url);
	if (urlString.empty())
	{
		mLogger->warning("%s traceWebRequest (string): url must not be null or empty", toString().c_str());
		return NULL_WEB_REQUEST_TRACER;
	}
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("%s traceWebRequest (string) (%s))", toString().c_str(), url);
	}

	if (!isActionLeft())
	{
		return std::make_shared<core::WebRequestTracerStringURL>(mLogger, mBeacon, getID(), urlString);
	}

	return NULL_WEB_REQUEST_TRACER;
}

std::shared_ptr<openkit::IRootAction> Action::leaveAction()
{
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("%s leaveAction(%s))", toString().c_str(), mName.getStringData().c_str());
	}
	int64_t expected = -1L;
	if (atomic_compare_exchange_strong(&mEndTime, &expected, mBeacon->getCurrentTimestamp()) == false)
	{
		return mParentAction;
	}

	return doLeaveAction();
}

std::shared_ptr<openkit::IRootAction> Action::doLeaveAction()
{
	mEndTime = mBeacon->getCurrentTimestamp();
	mEndSequenceNumber = mBeacon->createSequenceNumber();

	// add Action to Beacon
	mBeacon->addAction(shared_from_this());

	if (mParentAction != nullptr)
	{
		//remove Action from the Actions on this level
		mParentAction->childActionEnded(shared_from_this());
	}

	auto returnValue = std::static_pointer_cast<openkit::IRootAction>(mParentAction);
	mParentAction = nullptr;

	return returnValue;

}

int32_t Action::getID() const
{
	return mID;
}

const UTF8String& Action::getName() const
{
	return mName;
}

int32_t Action::getParentID() const
{
	return mParentAction == nullptr ? 0 : mParentAction->getID();
}

int64_t Action::getStartTime() const
{
	return mStartTime;
}

int64_t Action::getEndTime() const
{
	return mEndTime;
}

int32_t Action::getStartSequenceNo() const
{
	return mStartSequenceNumber;
}

int32_t Action::getEndSequenceNo() const
{
	return mEndSequenceNumber;
}

bool Action::isActionLeft() const
{
	return mEndTime != -1;
}

const std::string Action::toString() const
{
	std::stringstream ss;
	ss << "Action [sn=" << mBeacon->getSessionNumber() << ", id=" << mID << ", name=" << mName.getStringData() << ", pa=" << (mParentAction != nullptr ? std::to_string(mParentAction->getID()) : "no parent") << "]";
	return ss.str();
}
