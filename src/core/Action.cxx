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

#include <atomic>
#include <memory>

using namespace core;

Action::Action(std::shared_ptr<protocol::Beacon> beacon, const UTF8String& name, util::SynchronizedQueue<std::shared_ptr<Action>>& sameLevelActions)
	: Action(beacon, name, nullptr, sameLevelActions)
{

}

Action::Action(std::shared_ptr<protocol::Beacon> beacon, const UTF8String& name, std::shared_ptr<Action> parentAction, util::SynchronizedQueue<std::shared_ptr<Action>>& sameLevelActions)
	: mParentAction(parentAction)
	, mEndTime(-1)
	, mBeacon(beacon)
	, mID(mBeacon->createID())
	, mName(name)
	, mStartTime(mBeacon->getCurrentTimestamp())
	, mStartSequenceNumber(mBeacon->createSequenceNumber())
	, mEndSequenceNumber(-1)
	, mSameLevelActions(sameLevelActions)
{
	mSameLevelActions.put(shared_from_this());
}

std::shared_ptr<api::IAction> Action::leaveAction()
{
	int64_t expected = -1L;
	if (atomic_compare_exchange_strong(&mEndTime, &expected, mBeacon->getCurrentTimestamp()) == false)
	{
		return mParentAction;
	}

	return doLeaveAction();
}

std::shared_ptr<api::IAction> Action::doLeaveAction()
{
	mEndTime = mBeacon->getCurrentTimestamp();
	mEndSequenceNumber = mBeacon->createSequenceNumber();

	// add Action to Beacon
	mBeacon->addAction(shared_from_this());

	//remove Action from the Actions on this level
	mSameLevelActions.remove(shared_from_this());

	return mParentAction;
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