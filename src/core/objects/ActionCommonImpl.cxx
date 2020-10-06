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

#include "ActionCommonImpl.h"
#include "protocol/IBeacon.h"
#include "core/objects/LeafAction.h"
#include "core/objects/WebRequestTracer.h"

#include <sstream>
#include <cinttypes>

using namespace core::objects;

ActionCommonImpl::ActionCommonImpl
(
	std::shared_ptr<openkit::ILogger> logger,
	std::shared_ptr<protocol::IBeacon> beacon,
	std::shared_ptr<IOpenKitComposite> parent,
	const core::UTF8String& name,
	const std::string& actionClassName
)
	: mLogger(logger)
	, mBeacon(beacon)
	, mParent(parent)
	, mName (name)
	, mStartTime(mBeacon->getCurrentTimestamp())
	, mEndTime(-1)
	, mStartSequenceNumber(mBeacon->createSequenceNumber())
	, mEndSequenceNumber(-1)
	, mActionID(mBeacon->createID())
	, mParentID(parent->getActionId())
	, mActionClassName(actionClassName)
	, mIsActionLeft()
	, mMutex()
{
}

std::shared_ptr<openkit::IAction> ActionCommonImpl::enterAction
(
	std::shared_ptr<openkit::IRootAction> rootAction,
	const char* actionName
)
{
	core::UTF8String actionNameString(actionName);
	if (actionNameString.empty())
	{
		mLogger->warning("%s enterAction: actionName must not be null or empty", toString().c_str());
		return std::make_shared<NullAction>(rootAction);
	}

	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("%s enterAction(%s)", toString().c_str(), actionNameString.getStringData().c_str());
	}

	// synchronized scope
	{
		std::lock_guard<Mutex_t> lock(mMutex);

		if (!isActionLeft())
		{
			auto leafActionImpl = std::make_shared<ActionCommonImpl>(
				mLogger,
				mBeacon,
				shared_from_this(),
				actionNameString,
				"LeafAction"
			);
			storeChildInList(leafActionImpl);

			auto childAction = std::make_shared<LeafAction>(leafActionImpl, rootAction);
			return childAction;
		}
	}

	return std::make_shared<NullAction>(rootAction);
}

void ActionCommonImpl::reportEvent(const char* eventName)
{
	UTF8String eventNameString(eventName);
	if (eventNameString.empty())
	{
		mLogger->warning("%s reportEvent: eventName must not be null or empty", toString().c_str());
		return;
	}
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("%s reportEvent(%s)", toString().c_str(), eventNameString.getStringData().c_str());
	}

	// synchronized scope
	{
		std::lock_guard<Mutex_t> lock(mMutex);

		if (!isActionLeft())
		{
			mBeacon->reportEvent(mActionID, eventNameString);
		}
	}
}

void ActionCommonImpl::reportValue(const char* valueName, int32_t value)
{
	UTF8String valueNameString(valueName);
	if (valueNameString.empty())
	{
		mLogger->warning("%s reportValue (int32_t): valueName must not be null or empty", toString().c_str());
		return;
	}
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("%s reportValue (int32_t) (%s, %d)",
			toString().c_str(),
			valueNameString.getStringData().c_str(),
			value
		);
	}

	// synchronized scope
	{
		std::lock_guard<Mutex_t> lock(mMutex);

		if (!isActionLeft())
		{
			mBeacon->reportValue(mActionID, valueNameString, value);
		}
	}
}

void ActionCommonImpl::reportValue(const char* valueName, int64_t value)
{
	UTF8String valueNameString(valueName);
	if (valueNameString.empty())
	{
		mLogger->warning("%s reportValue (int64_t): valueName must not be null or empty", toString().c_str());
		return;
	}
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("%s reportValue (int64_t) (%s, %" PRId64 ")",
			toString().c_str(),
			valueNameString.getStringData().c_str(),
			value
		);
	}

	// synchronized scope
	{
		std::lock_guard<Mutex_t> lock(mMutex);

		if (!isActionLeft())
		{
			mBeacon->reportValue(mActionID, valueNameString, value);
		}
	}
}

void ActionCommonImpl::reportValue(const char* valueName, double value)
{
	UTF8String valueNameString(valueName);
	if (valueNameString.empty())
	{
		mLogger->warning("%s reportValue (double): valueName must not be null or empty", toString().c_str());
		return;
	}
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("%s reportValue (double) (%s, %f)",
			toString().c_str(),
			valueNameString.getStringData().c_str(),
			value
		);
	}

	// synchronized scope
	{
		std::lock_guard<Mutex_t> lock(mMutex);

		if (!isActionLeft())
		{
			mBeacon->reportValue(mActionID, valueNameString, value);
		}
	}
}

void ActionCommonImpl::reportValue(const char* valueName, const char* value)
{
	UTF8String valueNameString(valueName);
	if (valueNameString.empty())
	{
		mLogger->warning("%s reportValue (string): valueName must not be null or empty", toString().c_str());
		return;
	}
	if (mLogger->isDebugEnabled())
	{
		UTF8String valueString(value);
		mLogger->debug("%s reportValue (string) (%s, %s)",
			toString().c_str(),
			valueNameString.getStringData().c_str(),
			(value != nullptr ? valueString.getStringData().c_str() : "null")
		);
	}

	// synchronized scope
	{
		std::lock_guard<Mutex_t> lock(mMutex);

		if (!isActionLeft())
		{
			mBeacon->reportValue(mActionID, valueNameString, value);
		}
	}
}


void ActionCommonImpl::reportError(const char* errorName, int32_t errorCode, const char* reason)
{
	UTF8String errorNameString(errorName);
	UTF8String reasonString(reason);
	if (errorNameString.empty())
	{
		mLogger->warning("%s reportError: errorName must not be null or empty", toString().c_str());
		return;
	}
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("%s reportError(%s, %d, %s)",
			toString().c_str(),
			errorNameString.getStringData().c_str(), errorCode,
			(reason != nullptr ? reasonString.getStringData().c_str() : "null")
		);
	}

	// synchronized scope
	{
		std::lock_guard<Mutex_t> lock(mMutex);

		if (!isActionLeft())
		{
			mBeacon->reportError(mActionID, errorNameString, errorCode, reasonString);
		}
	}
}

std::shared_ptr<openkit::IWebRequestTracer> ActionCommonImpl::traceWebRequest(const char* url)
{
	core::UTF8String urlString(url);
	if (urlString.empty())
	{
		mLogger->warning("%s traceWebRequest (string): url must not be null or empty", toString().c_str());
		return NullWebRequestTracer::instance();
	}
	if (!WebRequestTracer::isValidURLScheme(urlString))
	{
		mLogger->warning("%s traceWebRequest (string): url \"%s\" does not have a valid scheme",
			toString().c_str(),
			urlString.getStringData().c_str()
		);
		return NullWebRequestTracer::instance();
	}
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("%s traceWebRequest(%s)", toString().c_str(), urlString.getStringData().c_str());
	}

	// synchronized scope
	{
		std::lock_guard<Mutex_t> lock(mMutex);

		if (!isActionLeft())
		{
			auto tracer = std::make_shared<core::objects::WebRequestTracer>(mLogger, shared_from_this(), mBeacon, urlString);
			storeChildInList(tracer);

			return tracer;
		}
	}

	return NullWebRequestTracer::instance();
}

bool ActionCommonImpl::leaveAction()
{
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("%s leaveAction(%s)", toString().c_str(), mName.getStringData().c_str());
	}

	// synchronized scope
	{
		std::lock_guard<Mutex_t> lock(mMutex);

		if (isActionLeft())
		{
			return false;
		}

		mIsActionLeft = true;
	}

	// close all child objects
	// Note: at this point it's save to do any further operations outside a mutual exclusive scope
	// after the action was left, no further child objects must be added
	auto childObjects = getCopyOfChildObjects();
	for (auto childObject : childObjects)
	{
		childObject->close();
	}

	mEndTime = mBeacon->getCurrentTimestamp();
	mEndSequenceNumber = mBeacon->createSequenceNumber();

	auto thisAction = shared_from_this();

	// add action to Beacon
	mBeacon->addAction(thisAction);

	// detach from parent
	mParent->onChildClosed(thisAction);

	return true;
}

void ActionCommonImpl::close()
{
	leaveAction();
}

bool ActionCommonImpl::isActionLeft() const
{
	return mIsActionLeft;
}

int32_t ActionCommonImpl::getID() const
{
	return mActionID;
}

int32_t ActionCommonImpl::getActionId() const
{
	return getID();
}

int32_t ActionCommonImpl::getParentID() const
{
	return mParentID;
}

const core::UTF8String& ActionCommonImpl::getName() const
{
	return mName;
}

int64_t ActionCommonImpl::getStartTime() const
{
	return mStartTime;
}

int64_t ActionCommonImpl::getEndTime() const
{
	return mEndTime;
}

int32_t ActionCommonImpl::getStartSequenceNumber() const
{
	return mStartSequenceNumber;
}

int32_t ActionCommonImpl::getEndSequenceNumber() const
{
	return mEndSequenceNumber;
}

void ActionCommonImpl::onChildClosed(std::shared_ptr<core::objects::IOpenKitObject> childObject)
{
	// synchronized scope
	{
		std::lock_guard<Mutex_t> lock(mMutex);

		removeChildFromList(childObject);
	}
}

const std::string ActionCommonImpl::toString() const
{
	std::stringstream ss;

	ss << mActionClassName
		<< " [sn=" << mBeacon->getSessionNumber()
		<< ", id=" << mActionID
		<< ", name=" << mName.getStringData()
		<< ", pa=" << mParentID
		<< "]";

	return ss.str();
}