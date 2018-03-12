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

using namespace core;

RootAction::RootAction(std::shared_ptr<protocol::Beacon> beacon, const UTF8String& name, util::SynchronizedQueue<std::shared_ptr<Action>>& parentActions)
	: Action(beacon, name, parentActions)
	, mBeacon(beacon)
	, mOpenChildActions()
{

}

std::shared_ptr<api::IAction> RootAction::enterAction()
{
	if (!isActionLeft())
	{
		return std::shared_ptr<Action>(new Action(mBeacon, getName(), shared_from_this(), mOpenChildActions));
	}
	return nullptr;//TODO johannes.baeuerle: NullAction
}

std::shared_ptr<api::IAction> RootAction::doLeaveAction()
{
	while (!mOpenChildActions.isEmpty())
	{
		auto action = mOpenChildActions.get();
		action->leaveAction();
	}

	return Action::leaveAction();
}

std::shared_ptr<api::IAction> RootAction::leaveAction()
{
	int64_t expected = -1L;
	if (atomic_compare_exchange_strong(&mEndTime, &expected, mBeacon->getCurrentTimestamp()) == false)
	{
		return mParentAction;
	}

	return doLeaveAction();
}
