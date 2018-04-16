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

Session::Session(std::shared_ptr<api::ILogger> logger, std::shared_ptr<BeaconSender> beaconSender, std::shared_ptr<protocol::Beacon> beacon)
	: mLogger(logger)
	, mBeaconSender(beaconSender)
	, mBeacon(beacon)
	, mEndTime(-1)
	, mOpenRootActions()
	, NULL_ROOT_ACTION(std::make_shared<NullRootAction>())
{

}

void Session::startSession()
{
	mBeaconSender->startSession(shared_from_this());
}

std::shared_ptr<api::IRootAction> Session::enterAction(const char* actionName)
{
	UTF8String actionNameString(actionName);
	if (actionNameString.empty())
	{
		mLogger->warning("Session.enterAction: actionName must not be null or empty");
		return NULL_ROOT_ACTION;
	}

	if (isSessionEnded())
	{
		return NULL_ROOT_ACTION;
	}
	std::shared_ptr<api::IRootAction> pointer = std::make_shared<RootAction>(mLogger, mBeacon, actionNameString, shared_from_this());
	mOpenRootActions.put(pointer);
	return pointer;
}

void Session::identifyUser(const char* userTag)
{
	UTF8String userTagString(userTag);

	if (userTag == nullptr || userTagString.empty())
	{
		mLogger->warning("Session.identifyUser: userTag must not be null or empty");
		return;
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
		mLogger->warning("Session.reportCrash: errorName must not be null or empty");
		return;
	}


	if (!isSessionEnded())
	{
		mBeacon->reportCrash(errorNameString, UTF8String(reason), UTF8String(stacktrace));
	}
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

void Session::rootActionEnded(std::shared_ptr<RootAction> rootAction)
{
	mOpenRootActions.remove(rootAction);
}

std::unique_ptr<protocol::StatusResponse> Session::sendBeacon(std::shared_ptr<providers::IHTTPClientProvider> clientProvider)
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