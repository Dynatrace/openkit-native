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

using namespace core;

RootAction::RootAction(std::shared_ptr<protocol::Beacon> beacon, const UTF8String& name, std::shared_ptr<Session> session)
	: mBeacon(beacon)
	, mOpenChildActions()
	, mSession(session)
	, mID(mBeacon->createID())
	, mName(name)
	, mStartTime(mBeacon->getCurrentTimestamp())
	, mStartSequenceNumber(mBeacon->createSequenceNumber())
	, mEndSequenceNumber(-1)
	, mEndTime(-1)
{

}

std::shared_ptr<api::IAction> RootAction::enterAction(const char* actionName)
{
	if (!isActionLeft())
	{
		auto childAction = std::make_shared<Action>(mBeacon, UTF8String(actionName), shared_from_this());
		mOpenChildActions.put(std::static_pointer_cast<api::IAction>(childAction));
		return childAction;
	}
	return nullptr;//TODO johannes.baeuerle: NullAction
}

void RootAction::doLeaveAction()
{
	mEndTime = mBeacon->getCurrentTimestamp();
	mEndSequenceNumber = mBeacon->createSequenceNumber();

	// add Action to Beacon
	mBeacon->addAction(shared_from_this());

	while (!mOpenChildActions.isEmpty())
	{
		auto action = mOpenChildActions.get();
		action->leaveAction();
	}

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