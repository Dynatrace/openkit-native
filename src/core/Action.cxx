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

using namespace core;

Action::Action(std::shared_ptr<protocol::Beacon> beacon, const UTF8String& name)
	: Action(beacon, name, nullptr)
{

}

Action::Action(std::shared_ptr<protocol::Beacon> beacon, const UTF8String& name, std::shared_ptr<RootAction> parentAction)
	: mParentAction(parentAction)
	, mEndTime(-1)
	, mBeacon(beacon)
	, mID(mBeacon->createID())
	, mName(name)
	, mStartTime(mBeacon->getCurrentTimestamp())
	, mStartSequenceNumber(mBeacon->createSequenceNumber())
	, mEndSequenceNumber(-1)
{

}

std::shared_ptr<api::IAction> Action::reportEvent(const char* eventName)
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

std::shared_ptr<api::IAction> Action::reportValue(const char* valueName, int32_t value)
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

std::shared_ptr<api::IAction> Action::reportValue(const char* valueName, double value)
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

std::shared_ptr<api::IAction> Action::reportValue(const char* valueName, const char* value)
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

std::shared_ptr<api::IAction> Action::reportError(const char* errorName, int32_t errorCode, const char* reason)
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

std::shared_ptr<api::IRootAction> Action::leaveAction()
{
	int64_t expected = -1L;
	if (atomic_compare_exchange_strong(&mEndTime, &expected, mBeacon->getCurrentTimestamp()) == false)
	{
		return mParentAction;
	}

	return doLeaveAction();
}

std::shared_ptr<api::IRootAction> Action::doLeaveAction()
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

	auto returnValue = std::static_pointer_cast<api::IRootAction>(mParentAction);
	mParentAction = nullptr;

	return returnValue;

}

bool Action::isNullObject() const
{
	return false;
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