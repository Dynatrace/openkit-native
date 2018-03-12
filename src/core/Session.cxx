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

#include "Session.h"
#include "protocol/Beacon.h"
#include "BeaconSender.h"
#include "Action.h"
#include "RootAction.h"

using namespace core;

Session::Session(std::shared_ptr<BeaconSender> beaconSender, std::shared_ptr<protocol::Beacon> beacon)
	: mBeaconSender(beaconSender)
	, mBeacon(beacon)
	, mEndTime(-1)
	, mOpenRootActions()
{
	mBeaconSender->startSession(shared_from_this());
}

std::shared_ptr<api::IRootAction> Session::enterAction(const UTF8String& actionName)
{
	if (isSessionEnded())
	{
		return nullptr;//TODO johannes.baeuerle add NullRootAction
	}
	auto pointer = new RootAction(mBeacon, actionName, mOpenRootActions);
	return std::shared_ptr<api::IRootAction>(pointer);
}

void Session::end()
{
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

bool Session::isSessionEnded() const
{
	return mEndTime != -1L;
}

int64_t Session::getEndTime() const
{
	return mEndTime;
}