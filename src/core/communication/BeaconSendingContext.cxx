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

#include "BeaconSendingContext.h"

#include "BeaconSendingInitialState.h"
#include "BeaconSendingResponseUtil.h"
#include "IBeaconSendingState.h"
#include "protocol/HTTPClient.h"
#include "protocol/ResponseAttributes.h"
#include "core/configuration/ServerConfiguration.h"
#include "core/configuration/HTTPClientConfiguration.h"

using namespace core::communication;

static const std::chrono::milliseconds DEFAULT_SLEEP_TIME_MILLISECONDS = std::chrono::seconds(1);

BeaconSendingContext::BeaconSendingContext(
	std::shared_ptr<openkit::ILogger> logger,
	std::shared_ptr<core::configuration::IHTTPClientConfiguration> httpClientConfig,
	std::shared_ptr<providers::IHTTPClientProvider> httpClientProvider,
	std::shared_ptr<providers::ITimingProvider> timingProvider,
	std::shared_ptr<core::util::IInterruptibleThreadSuspender> threadSuspender,
	std::unique_ptr<IBeaconSendingState> initialState
)
	: mLogger(logger)
	, mCurrentState(std::move(initialState))
	, mNextState(nullptr)
	, mShutdown(false)
	, mInitSucceeded(false)
	, mServerConfiguration(core::configuration::ServerConfiguration::defaultInstance())
	, mHTTPClientConfiguration(httpClientConfig)
	, mHTTPClientProvider(httpClientProvider)
	, mTimingProvider(timingProvider)
	, mLastStatusCheckTime(0)
	, mLastOpenSessionBeaconSendTime(0)
	, mLastResponseAttributes(protocol::ResponseAttributes::withUndefinedDefaults().build())
	, mInitCountdownLatch(1)
	, mThreadSuspender(threadSuspender)
	, mSessions()
{
}

BeaconSendingContext::BeaconSendingContext
(
	std::shared_ptr<openkit::ILogger> logger,
	std::shared_ptr<core::configuration::IHTTPClientConfiguration> httpClientConfig,
	std::shared_ptr<providers::IHTTPClientProvider> httpClientProvider,
	std::shared_ptr<providers::ITimingProvider> timingProvider,
	std::shared_ptr<core::util::IInterruptibleThreadSuspender> threadSuspender
)
: BeaconSendingContext(
	logger,
	httpClientConfig,
	httpClientProvider,
	timingProvider,
	threadSuspender,
	std::unique_ptr<IBeaconSendingState>(new BeaconSendingInitialState())
)
{
}

const std::chrono::milliseconds& BeaconSendingContext::getDefaultSleepTime()
{
	return DEFAULT_SLEEP_TIME_MILLISECONDS;
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
	mShutdown = true;
	mThreadSuspender->wakeup();
}

bool BeaconSendingContext::isShutdownRequested() const
{
	return mShutdown;
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

void BeaconSendingContext::setInitCompleted(bool success)
{
	mInitSucceeded = success;
	mInitCountdownLatch.countDown();
}

bool BeaconSendingContext::isInitialized() const
{
	return mInitSucceeded;
}

bool BeaconSendingContext::isInTerminalState() const
{
	return mCurrentState->isTerminalState();
}

bool BeaconSendingContext::isCaptureOn() const
{
	return mServerConfiguration->isCaptureEnabled();
}

std::shared_ptr<IBeaconSendingState> BeaconSendingContext::getCurrentState() const
{
	return mCurrentState;
}

void BeaconSendingContext::setNextState(std::shared_ptr<IBeaconSendingState> nextState)
{
	mNextState = nextState;
}

std::shared_ptr<IBeaconSendingState> BeaconSendingContext::getNextState()
{
	return mNextState;
}

std::shared_ptr<providers::IHTTPClientProvider> BeaconSendingContext::getHTTPClientProvider()
{
	return mHTTPClientProvider;
}

std::shared_ptr<protocol::IHTTPClient> BeaconSendingContext::getHTTPClient()
{
	return mHTTPClientProvider->createClient(mLogger, mHTTPClientConfiguration);
}

int64_t BeaconSendingContext::getCurrentTimestamp() const
{
	return mTimingProvider->provideTimestampInMilliseconds();
}

void BeaconSendingContext::sleep()
{
	sleep(DEFAULT_SLEEP_TIME_MILLISECONDS.count());
}

void BeaconSendingContext::sleep(int64_t ms)
{
	mThreadSuspender->sleep(ms);
}

int64_t BeaconSendingContext::getLastOpenSessionBeaconSendTime() const
{
	return mLastOpenSessionBeaconSendTime;
}

void BeaconSendingContext::setLastOpenSessionBeaconSendTime(int64_t timestamp)
{
	mLastOpenSessionBeaconSendTime = timestamp;
}

int64_t BeaconSendingContext::getLastStatusCheckTime() const
{
	return mLastStatusCheckTime;
}

void BeaconSendingContext::setLastStatusCheckTime(int64_t lastStatusCheckTime)
{
	mLastStatusCheckTime = lastStatusCheckTime;
}

int64_t BeaconSendingContext::getSendInterval() const
{
	return mLastResponseAttributes->getSendIntervalInMilliseconds();
}

void BeaconSendingContext::disableCaptureAndClear()
{
	// first disable in configuration, so no further data will get collected
	disableCapture();
	clearAllSessionData();
}

void BeaconSendingContext::disableCapture()
{
	mServerConfiguration = core::configuration::ServerConfiguration::Builder(mServerConfiguration)
			.withCapture(false)
			.build();
}

void BeaconSendingContext::handleStatusResponse(std::shared_ptr<protocol::IStatusResponse> response)
{
	if (response == nullptr || response->getResponseCode() != 200)
	{
		disableCaptureAndClear();
		return;
	}

	auto updatedAttributes = updateLastResponseAttributesFrom(response);
	mServerConfiguration = core::configuration::ServerConfiguration::Builder(updatedAttributes).build();
	if (!isCaptureOn())
	{
		// capturing was turned off
		clearAllSessionData();
	}

	auto serverId = mServerConfiguration->getServerId();
	if (serverId != mHTTPClientConfiguration->getServerID())
	{
		mHTTPClientConfiguration = core::configuration::HTTPClientConfiguration::Builder(mHTTPClientConfiguration)
			.withServerID(serverId)
			.build();
	}
}

std::shared_ptr<protocol::IResponseAttributes> BeaconSendingContext::updateLastResponseAttributesFrom(std::shared_ptr<protocol::IStatusResponse> statusResponse)
{
	if (BeaconSendingResponseUtil::isSuccessfulResponse(statusResponse))
	{
		mLastResponseAttributes = mLastResponseAttributes->merge(statusResponse->getResponseAttributes());
	}

	return mLastResponseAttributes;
}

std::shared_ptr<protocol::IResponseAttributes> BeaconSendingContext::getLastResponseAttributes() const
{
	return mLastResponseAttributes;
}

void BeaconSendingContext::clearAllSessionData()
{
	// clear captured data from finished sessions
	for (auto session : mSessions.toStdVector())
	{
		session->clearCapturedData();
		if (session->isFinished())
		{
			mSessions.remove(session);
		}
	}
}

std::vector<std::shared_ptr<core::objects::SessionInternals>> BeaconSendingContext::getAllNotConfiguredSessions()
{
	std::vector<std::shared_ptr<core::objects::SessionInternals>> notConfiguredSessions;

	for (auto session : mSessions.toStdVector())
	{
		if (!session->isConfigured())
		{
			notConfiguredSessions.push_back(session);
		}
	}

	return notConfiguredSessions;
}

std::vector<std::shared_ptr<core::objects::SessionInternals>> BeaconSendingContext::getAllOpenAndConfiguredSessions()
{
	std::vector<std::shared_ptr<core::objects::SessionInternals>> openSessions;

	for (auto session : mSessions.toStdVector())
	{
		if (session->isConfiguredAndOpen())
		{
			openSessions.push_back(session);
		}
	}

	return openSessions;
}

std::vector<std::shared_ptr<core::objects::SessionInternals>> BeaconSendingContext::getAllFinishedAndConfiguredSessions()
{
	std::vector<std::shared_ptr<core::objects::SessionInternals>> finishedSessions;
	for (auto session : mSessions.toStdVector())
	{
		if (session->isConfiguredAndFinished())
		{
			finishedSessions.push_back(session);
		}
	}

	return finishedSessions;
}

size_t BeaconSendingContext::getSessionCount()
{
	return mSessions.size();
}

int32_t BeaconSendingContext::getCurrentServerID() const
{
	return mHTTPClientConfiguration->getServerID();
}

void BeaconSendingContext::addSession(std::shared_ptr<core::objects::SessionInternals> session)
{
	mSessions.put(session);
}

bool BeaconSendingContext::removeSession(std::shared_ptr<core::objects::SessionInternals> sessionWrapper)
{
	return mSessions.remove(sessionWrapper);
}

IBeaconSendingState::StateType BeaconSendingContext::getCurrentStateType() const
{
	return mCurrentState->getStateType();
}

int64_t BeaconSendingContext::getConfigurationTimestamp() const
{
	return mLastResponseAttributes->getTimestampInMilliseconds();
}