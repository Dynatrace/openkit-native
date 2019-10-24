/**
* Copyright 2018-2019 Dynatrace LLC
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

#include "BeaconSendingInitialState.h"
#include "IBeaconSendingState.h"

#include "protocol/HTTPClient.h"
#include "core/configuration/Configuration.h"
#include "core/configuration/HTTPClientConfiguration.h"

using namespace core::communication;

const std::chrono::milliseconds BeaconSendingContext::DEFAULT_SLEEP_TIME_MILLISECONDS(std::chrono::seconds(1));

BeaconSendingContext::BeaconSendingContext
(
	std::shared_ptr<openkit::ILogger> logger,
	std::shared_ptr<providers::IHTTPClientProvider> httpClientProvider,
	std::shared_ptr<providers::ITimingProvider> timingProvider,
	std::shared_ptr<core::configuration::Configuration> configuration,
	std::unique_ptr<IBeaconSendingState> initialState
)
	: mLogger(logger)
	, mCurrentState(std::move(initialState))
	, mNextState(nullptr)
	, mShutdown(false)
	, mShutdownMutex()
	, mSleepConditionVariable()
	, mInitSucceeded(false)
	, mConfiguration(configuration)
	, mHTTPClientProvider(httpClientProvider)
	, mTimingProvider(timingProvider)
	, mLastStatusCheckTime(0)
	, mLastOpenSessionBeaconSendTime(0)
	, mInitCountdownLatch(1)
	, mSessions()
{
}

BeaconSendingContext::BeaconSendingContext
(
	std::shared_ptr<openkit::ILogger> logger,
	std::shared_ptr<providers::IHTTPClientProvider> httpClientProvider,
	std::shared_ptr<providers::ITimingProvider> timingProvider,
	std::shared_ptr<core::configuration::Configuration> configuration
)
: BeaconSendingContext(
	logger,
	httpClientProvider,
	timingProvider,
	configuration,
	std::unique_ptr<IBeaconSendingState>(new BeaconSendingInitialState())
)
{
}

void BeaconSendingContext::setNextState(std::shared_ptr<IBeaconSendingState> nextState)
{
	mNextState = nextState;
}

bool BeaconSendingContext::isInTerminalState() const
{
	return mCurrentState->isTerminalState();
}

void BeaconSendingContext::executeCurrentState()
{
	mNextState = nullptr;
	mCurrentState->execute(*this);

	if (mNextState != nullptr && mNextState != mCurrentState)// mCcurrentState->execute(...) can trigger state changes
	{
		if (mLogger->isInfoEnabled())
		{
			mLogger->info("BeaconSendingContext executeCurrentState() - State change from '%s' to '%s'", mCurrentState->getStateName(), mNextState->getStateName());
		}
		mCurrentState = mNextState;
	}
}

void BeaconSendingContext::requestShutdown()
{
	std::unique_lock<std::mutex> lock(mShutdownMutex);
	mShutdown = true;
	mSleepConditionVariable.notify_all(); // wake up all sleeping threads
}

bool BeaconSendingContext::isShutdownRequested() const
{
	std::unique_lock<std::mutex> lock(mShutdownMutex);
	return mShutdown;
}

std::shared_ptr<providers::IHTTPClientProvider> BeaconSendingContext::getHTTPClientProvider()
{
	return mHTTPClientProvider;
}

std::shared_ptr<protocol::IHTTPClient> BeaconSendingContext::getHTTPClient()
{
	auto httpClientConfig = mConfiguration->getHTTPClientConfiguration();
	return mHTTPClientProvider->createClient(mLogger, httpClientConfig);
}

int64_t BeaconSendingContext::getSendInterval() const
{
	return mConfiguration->getSendInterval();
}

void BeaconSendingContext::handleStatusResponse(std::shared_ptr<protocol::IStatusResponse> response)
{
	mConfiguration->updateSettings(response);

	if (!isCaptureOn())
	{
		// capturing was turned off
		clearAllSessionData();
	}
}

void BeaconSendingContext::clearAllSessionData()
{
	// clear captured data from finished sessions
	for (auto session : mSessions.toStdVector())
	{
		session->clearCapturedData();
		if (session->isSessionFinished())
		{
			mSessions.remove(session);
		}
	}
}

bool BeaconSendingContext::isCaptureOn() const
{
	return mConfiguration->isCapture();
}

std::shared_ptr<IBeaconSendingState> BeaconSendingContext::getCurrentState() const
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

bool BeaconSendingContext::waitForInit(int64_t timeoutMillis)
{
	mInitCountdownLatch.await(timeoutMillis);
	return mInitSucceeded;
}

void BeaconSendingContext::sleep()
{
	sleep(DEFAULT_SLEEP_TIME_MILLISECONDS.count());
}

void BeaconSendingContext::sleep(int64_t ms)
{
	std::unique_lock<std::mutex> lock(mShutdownMutex);
	mSleepConditionVariable.wait_for(lock, std::chrono::milliseconds(ms), [&] { return mShutdown; });
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

IBeaconSendingState::StateType BeaconSendingContext::getCurrentStateType() const
{
	return mCurrentState->getStateType();
}

void BeaconSendingContext::startSession(std::shared_ptr<core::objects::SessionInternals> session)
{
	auto sessionWrapper = std::make_shared<core::SessionWrapper>(session);
	mSessions.put(sessionWrapper);
}

void BeaconSendingContext::finishSession(std::shared_ptr<core::objects::SessionInternals> session)
{
	std::shared_ptr<core::SessionWrapper> sessionWrapper = findSessionWrapper(session);
	if (sessionWrapper != nullptr)
	{
		sessionWrapper->finishSession();
	}
}

std::vector<std::shared_ptr<core::SessionWrapper>> BeaconSendingContext::getAllNewSessions()
{
	std::vector<std::shared_ptr<core::SessionWrapper>> newSessions;

	for (auto wrapper : mSessions.toStdVector())
	{
		if (!wrapper->isBeaconConfigurationSet())
		{
			newSessions.push_back(wrapper);
		}
	}

	return newSessions;
}

std::vector<std::shared_ptr<core::SessionWrapper>> BeaconSendingContext::getAllOpenAndConfiguredSessions()
{
	std::vector<std::shared_ptr<core::SessionWrapper>> openSessions;

	for (auto wrapper : mSessions.toStdVector())
	{
		if (wrapper->isBeaconConfigurationSet() && !wrapper->isSessionFinished())
		{
			openSessions.push_back(wrapper);
		}
	}

	return openSessions;
}

std::vector<std::shared_ptr<core::SessionWrapper>> BeaconSendingContext::getAllFinishedAndConfiguredSessions()
{
	std::vector<std::shared_ptr<core::SessionWrapper>> finishedSessions;
	for (auto wrapper : mSessions.toStdVector())
	{
		if (wrapper->isBeaconConfigurationSet() && wrapper->isSessionFinished())
		{
			finishedSessions.push_back(wrapper);
		}
	}

	return finishedSessions;
}

std::shared_ptr<IBeaconSendingState> BeaconSendingContext::getNextState()
{
	return mNextState;
}

std::shared_ptr<core::SessionWrapper> BeaconSendingContext::findSessionWrapper(std::shared_ptr<core::objects::SessionInternals> session)
{
	for (auto sessionWrapper : mSessions.toStdVector())
	{
		if ( sessionWrapper->getWrappedSession() == session )
		{
			return sessionWrapper;
		}
	}
	return nullptr;
}

bool BeaconSendingContext::removeSession(std::shared_ptr<core::SessionWrapper> sessionWrapper)
{
	return mSessions.remove(sessionWrapper);
}