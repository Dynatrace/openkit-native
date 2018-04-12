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

#include "BeaconSendingContext.h"

#include "communication/AbstractBeaconSendingState.h"
#include "communication/BeaconSendingInitialState.h"

#include "protocol/HTTPClient.h"
#include "configuration/Configuration.h"
#include "configuration/HTTPClientConfiguration.h"

using namespace communication;

const std::chrono::milliseconds BeaconSendingContext::DEFAULT_SLEEP_TIME_MILLISECONDS(std::chrono::seconds(1));

BeaconSendingContext::BeaconSendingContext(std::shared_ptr<api::ILogger> logger,
										   std::shared_ptr<providers::IHTTPClientProvider> httpClientProvider,
										   std::shared_ptr<providers::ITimingProvider> timingProvider,
										   std::shared_ptr<configuration::Configuration> configuration)
	: mLogger(logger)
	, mCurrentState(std::unique_ptr<AbstractBeaconSendingState>(new BeaconSendingInitialState()))
	, mIsInTerminalState(false)
	, mShutdown(false)
	, mInitSucceeded(false)
	, mConfiguration(configuration)
	, mHTTPClientProvider(httpClientProvider)
	, mTimingProvider(timingProvider)
	, mLastStatusCheckTime(0)
	, mLastOpenSessionBeaconSendTime(0)
	, mInitCountdownLatch(1)
	, mIsTimeSyncSupported(true)
	, mLastTimeSyncTime(-1)
	, mOpenSessions()
	, mFinishedSessions()
{	
}

void BeaconSendingContext::setNextState(std::shared_ptr<AbstractBeaconSendingState> nextState)
{
	mCurrentState = nextState;
}

bool BeaconSendingContext::isInTerminalState() const
{
	return mCurrentState->isTerminalState();
}

void BeaconSendingContext::executeCurrentState()
{
	mCurrentState->execute(*this);
}

void BeaconSendingContext::requestShutdown()
{
	mShutdown = true;
}

bool BeaconSendingContext::isShutdownRequested() const
{
	return mShutdown;
}

const std::shared_ptr<configuration::Configuration> BeaconSendingContext::getConfiguration() const
{
	return mConfiguration;
}

std::shared_ptr<providers::IHTTPClientProvider> BeaconSendingContext::getHTTPClientProvider()
{
	return mHTTPClientProvider;
}

std::shared_ptr<protocol::IHTTPClient> BeaconSendingContext::getHTTPClient()
{
	std::shared_ptr<configuration::HTTPClientConfiguration> httpClientConfig = mConfiguration->getHTTPClientConfiguration();
	return mHTTPClientProvider->createClient(mLogger, httpClientConfig);
}

int64_t BeaconSendingContext::getSendInterval() const
{
	return mConfiguration->getSendInterval();
}

void BeaconSendingContext::handleStatusResponse(std::unique_ptr<protocol::StatusResponse> response)
{
	mConfiguration->updateSettings(std::move(response));

	if (!isCaptureOn())
	{
		// capturing was turned off
		clearAllSessionData();
	}
}

void BeaconSendingContext::clearAllSessionData()
{
	// clear captured data from finished sessions
	auto finishedSessionsVec = mFinishedSessions.toStdVector();
	for (auto it = finishedSessionsVec.begin(); it != finishedSessionsVec.end(); it++)
	{
		(*it)->clearCapturedData();
	}
	mFinishedSessions.clear();

	// clear captured data from open sessions
	auto openSessionsVec = mOpenSessions.toStdVector();
	for (auto it = openSessionsVec.begin(); it != openSessionsVec.end(); it++)
	{
		(*it)->clearCapturedData();
	}
}

bool BeaconSendingContext::isCaptureOn() const
{
	return mConfiguration->isCapture();
}

std::shared_ptr<AbstractBeaconSendingState> BeaconSendingContext::getCurrentState() const
{
	return mCurrentState;
}

void BeaconSendingContext::setInitCompleted(bool success)
{
	mInitSucceeded = success;
	mInitCountdownLatch.countDown();
}

bool BeaconSendingContext::isInitialized() const
{
	return mInitSucceeded;
}

bool BeaconSendingContext::waitForInit()
{
	mInitCountdownLatch.await();
	return mInitSucceeded;
}

void BeaconSendingContext::sleep()
{
	mTimingProvider->sleep(DEFAULT_SLEEP_TIME_MILLISECONDS.count());
}

void BeaconSendingContext::sleep(int64_t ms)
{
	mTimingProvider->sleep(ms);
}

int64_t BeaconSendingContext::getLastStatusCheckTime() const
{
	return mLastStatusCheckTime;
}

void BeaconSendingContext::setLastStatusCheckTime(int64_t lastStatusCheckTime)
{
	mLastStatusCheckTime = lastStatusCheckTime;
}

void BeaconSendingContext::disableCapture()
{
	// first disable in configuration, so no further data will get collected
	mConfiguration->disableCapture();
	clearAllSessionData();
}

int64_t BeaconSendingContext::getCurrentTimestamp() const
{
	return mTimingProvider->provideTimestampInMilliseconds();
}

int64_t BeaconSendingContext::getLastOpenSessionBeaconSendTime() const
{
	return mLastOpenSessionBeaconSendTime;
}

void BeaconSendingContext::setLastOpenSessionBeaconSendTime(int64_t timestamp)
{
	mLastOpenSessionBeaconSendTime = timestamp;
}

AbstractBeaconSendingState::StateType BeaconSendingContext::getCurrentStateType() const
{
	return mCurrentState->getStateType();
}

bool BeaconSendingContext::isTimeSyncSupported() const
{
	return mIsTimeSyncSupported;
}


void BeaconSendingContext::disableTimeSyncSupport()
{
	mIsTimeSyncSupported = false;
}

bool BeaconSendingContext::isTimeSynced() const
{
	return !mIsTimeSyncSupported || getLastTimeSyncTime() >= 0;
}

int64_t BeaconSendingContext::getLastTimeSyncTime() const
{
	return mLastTimeSyncTime;
}

void BeaconSendingContext::setLastTimeSyncTime(int64_t lastTimeSyncTime)
{
	mLastTimeSyncTime = lastTimeSyncTime;
}

void BeaconSendingContext::initializeTimeSync(int64_t clusterTimeOffset, bool isTimeSyncSupported)
{
	mTimingProvider->initialize(clusterTimeOffset, isTimeSyncSupported);
}

void BeaconSendingContext::startSession(std::shared_ptr<core::Session> session)
{
	mOpenSessions.put(session);
}

void BeaconSendingContext::pushBackFinishedSession(std::shared_ptr<core::Session> session)
{
	mFinishedSessions.put(session);
}

void BeaconSendingContext::finishSession(std::shared_ptr<core::Session> session)
{
	if (mOpenSessions.remove(session))
	{
		mFinishedSessions.put(session);
	}
}

std::shared_ptr<core::Session> BeaconSendingContext::getNextFinishedSession()
{
	return mFinishedSessions.get();
}

std::vector<std::shared_ptr<core::Session>> BeaconSendingContext::getAllOpenSessions()
{
	auto result = mOpenSessions.toStdVector();
	return result;
}

std::vector<std::shared_ptr<core::Session>> BeaconSendingContext::getAllFinishedSessions()
{
	auto result = mFinishedSessions.toStdVector();
	return result;
}