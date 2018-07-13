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

#include "SessionWrapper.h"
#include "configuration/BeaconConfiguration.h"

using namespace core;

constexpr uint32_t MAX_NEW_SESSION_REQUESTS = 4;

SessionWrapper::SessionWrapper(std::shared_ptr<Session> session)
	: mWrappedSession(session)
	, mIsBeaconConfigurationSet(false)
	, mSessionFinished(false)
	, mNumNewSessionRequestsLeft(MAX_NEW_SESSION_REQUESTS)
{

}

void SessionWrapper::updateBeaconConfiguration(std::shared_ptr<configuration::BeaconConfiguration> beaconConfiguration)
{
	mWrappedSession->setBeaconConfiguration(beaconConfiguration);
	mIsBeaconConfigurationSet = true;
}

std::shared_ptr<configuration::BeaconConfiguration> SessionWrapper::getBeaconConfiguration() const
{
	return mWrappedSession->getBeaconConfiguration();
}

bool SessionWrapper::isBeaconConfigurationSet() const
{
	return mIsBeaconConfigurationSet;
}

void SessionWrapper::finishSession()
{
	mSessionFinished = true;
}

bool SessionWrapper::isSessionFinished() const
{
	return mSessionFinished;
}

void SessionWrapper::decreaseNumberOfNewSessionRequests()
{
	mNumNewSessionRequestsLeft--;
}

bool SessionWrapper::canSendNewSessionRequest() const
{
	return mNumNewSessionRequestsLeft > 0;
}

bool SessionWrapper::isEmpty() const
{
	return mWrappedSession->isEmpty();
}

void SessionWrapper::end()
{
	mWrappedSession->end();
}

std::shared_ptr<Session> SessionWrapper::getWrappedSession() const
{
	return mWrappedSession;
}

bool SessionWrapper::isDataSendingAllowed() const
{
	return isBeaconConfigurationSet() && mWrappedSession->getBeaconConfiguration()->getMultiplicity() > 0;
}

void SessionWrapper::clearCapturedData()
{
	mWrappedSession->clearCapturedData();
}

std::unique_ptr<protocol::StatusResponse> SessionWrapper::sendBeacon(std::shared_ptr<providers::IHTTPClientProvider> httpClientProvider)
{
	return mWrappedSession->sendBeacon(httpClientProvider);
}
