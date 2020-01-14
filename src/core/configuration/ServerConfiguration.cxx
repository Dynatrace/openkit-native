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

#include "ServerConfiguration.h"

using namespace core::configuration;

const std::shared_ptr<IServerConfiguration> ServerConfiguration::DEFAULT = ServerConfiguration::Builder().build();

ServerConfiguration::ServerConfiguration(Builder& builder)
	: mIsCaptureEnabled(builder.isCaptureEnabled())
	, mIsCrashReportingEnabled(builder.isCrashReportingEnabled())
	, mIsErrorReportingEnabled(builder.isErrorReportingEnabled())
	, mSendIntervalInMilliseconds(builder.getSendIntervalInMilliseconds())
	, mServerId(builder.getServerId())
	, mBeaconSizeInBytes(builder.getBeaconSizeInBytes())
	, mMultiplicity(builder.getMultiplicity())
	, mMaxSessionDurationInMilliseconds(builder.getMaxSessionDurationInMilliseconds())
	, mMaxEventsPerSession(builder.getMaxEventsPerSession())
	, mSessionTimeoutInMilliseconds(builder.getSessionTimeoutInMilliseconds())
	, mVisitStoreVersion(builder.getVisitStoreVersion())
{
}

std::shared_ptr<core::configuration::IServerConfiguration> ServerConfiguration::from(
	std::shared_ptr<protocol::IStatusResponse> statusResponse
)
{
	if (statusResponse == nullptr)
	{
		return nullptr;
	}

	ServerConfiguration::Builder builder(statusResponse);
	return builder.build();
}

bool ServerConfiguration::isCaptureEnabled() const
{
	return mIsCaptureEnabled;
}

bool ServerConfiguration::isCrashReportingEnabled() const
{
	return mIsCrashReportingEnabled;
}

bool ServerConfiguration::isErrorReportingEnabled() const
{
	return mIsErrorReportingEnabled;
}

int32_t ServerConfiguration::getSendIntervalInMilliseconds() const
{
	return mSendIntervalInMilliseconds;
}

int32_t ServerConfiguration::getServerId() const
{
	return mServerId;
}

int32_t ServerConfiguration::getBeaconSizeInBytes() const
{
	return mBeaconSizeInBytes;
}

int32_t ServerConfiguration::getMultiplicity() const
{
	return mMultiplicity;
}

int32_t ServerConfiguration::getMaxSessionDurationInMilliseconds() const
{
	return mMaxSessionDurationInMilliseconds;
}

int32_t ServerConfiguration::getMaxEventsPerSession() const
{
	return mMaxEventsPerSession;
}

int32_t ServerConfiguration::getSessionTimeoutInMilliseconds() const
{
	return mSessionTimeoutInMilliseconds;
}

int32_t ServerConfiguration::getVisitStoreVersion() const
{
	return mVisitStoreVersion;
}

bool ServerConfiguration::isSendingDataAllowed() const
{
	return isCaptureEnabled() && getMultiplicity() > 0;
}

bool ServerConfiguration::isSendingCrashesAllowed() const
{
	return isCrashReportingEnabled() && isSendingDataAllowed();
}

bool ServerConfiguration::isSendingErrorsAllowed() const
{
	return isErrorReportingEnabled() && isSendingDataAllowed();
}

std::shared_ptr<core::configuration::IServerConfiguration> ServerConfiguration::merge(
		std::shared_ptr<core::configuration::IServerConfiguration> other) const
{
	ServerConfiguration::Builder builder(other);

	// take everything from other instance except multiplicity and server ID.
	builder.withMultiplicity(getMultiplicity());
	builder.withServerId(getServerId());

	return builder.build();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Builder implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ServerConfiguration::Builder::Builder()
	: mCaptureState(ServerConfiguration::DEFAULT_CAPTURE_ENABLED)
	, mCrashReportingState(ServerConfiguration::DEFAULT_CRASH_REPORTING_ENABLED)
	, mErrorReportingState(ServerConfiguration::DEFAULT_ERROR_REPORTING_ENABLED)
	, mSendIntervalInMilliseconds(ServerConfiguration::DEFAULT_SEND_INTERVAL)
	, mServerId(ServerConfiguration::DEFAULT_SERVER_ID)
	, mBeaconSizeInBytes(ServerConfiguration::DEFAULT_BEACON_SIZE)
	, mMultiplicity(ServerConfiguration::DEFAULT_MULTIPLICITY)
	, mMaxSessionDurationInMilliseconds(ServerConfiguration::DEFAULT_MAX_SESSION_DURATION)
	, mMaxEventsPerSession(ServerConfiguration::DEFAULT_MAX_EVENTS_PER_SESSION)
	, mSessionIdleTimeout(ServerConfiguration::DEFAULT_SESSION_TIMEOUT)
	, mVisitStoreVersion(ServerConfiguration::DEFAULT_VISIT_STORE_VERSION)
{
}

ServerConfiguration::Builder::Builder(std::shared_ptr<protocol::IStatusResponse> statusResponse)
	: mCaptureState(statusResponse->isCapture())
	, mCrashReportingState(statusResponse->isCaptureCrashes())
	, mErrorReportingState(statusResponse->isCaptureErrors())
	, mSendIntervalInMilliseconds(statusResponse->getSendInterval())
	, mServerId(statusResponse->getServerID())
	, mBeaconSizeInBytes(statusResponse->getMaxBeaconSize())
	, mMultiplicity(statusResponse->getMultiplicity())
	, mMaxSessionDurationInMilliseconds(ServerConfiguration::DEFAULT_MAX_SESSION_DURATION)
	, mMaxEventsPerSession(ServerConfiguration::DEFAULT_MAX_EVENTS_PER_SESSION)
	, mSessionIdleTimeout(ServerConfiguration::DEFAULT_SESSION_TIMEOUT)
	, mVisitStoreVersion(ServerConfiguration::DEFAULT_VISIT_STORE_VERSION)
{
}

ServerConfiguration::Builder::Builder(std::shared_ptr<core::configuration::IServerConfiguration> serverConfiguration)
	: mCaptureState(serverConfiguration->isCaptureEnabled())
	, mCrashReportingState(serverConfiguration->isCrashReportingEnabled())
	, mErrorReportingState(serverConfiguration->isErrorReportingEnabled())
	, mSendIntervalInMilliseconds(serverConfiguration->getSendIntervalInMilliseconds())
	, mServerId(serverConfiguration->getServerId())
	, mBeaconSizeInBytes(serverConfiguration->getBeaconSizeInBytes())
	, mMultiplicity(serverConfiguration->getMultiplicity())
	, mMaxSessionDurationInMilliseconds(serverConfiguration->getMaxSessionDurationInMilliseconds())
	, mMaxEventsPerSession(serverConfiguration->getMaxEventsPerSession())
	, mSessionIdleTimeout(serverConfiguration->getSessionTimeoutInMilliseconds())
	, mVisitStoreVersion(serverConfiguration->getVisitStoreVersion())
{
}

bool ServerConfiguration::Builder::isCaptureEnabled() const
{
	return mCaptureState;
}

ServerConfiguration::Builder& ServerConfiguration::Builder::withCapture(bool capture)
{
	mCaptureState = capture;
	return *this;
}

bool ServerConfiguration::Builder::isCrashReportingEnabled() const
{
	return mCrashReportingState;
}

ServerConfiguration::Builder& ServerConfiguration::Builder::withCrashReporting(bool crashReportingState)
{
	mCrashReportingState = crashReportingState;
	return *this;
}

bool ServerConfiguration::Builder::isErrorReportingEnabled() const
{
	return mErrorReportingState;
}

ServerConfiguration::Builder& ServerConfiguration::Builder::withErrorReporting(bool errorReportingState)
{
	mErrorReportingState = errorReportingState;
	return *this;
}

int32_t ServerConfiguration::Builder::getSendIntervalInMilliseconds() const
{
	return mSendIntervalInMilliseconds;
}

ServerConfiguration::Builder& ServerConfiguration::Builder::withSendIntervalInMilliseconds(
	int32_t sendIntervalInMilliseconds
)
{
	mSendIntervalInMilliseconds = sendIntervalInMilliseconds;
	return *this;
}

int32_t ServerConfiguration::Builder::getServerId() const
{
	return mServerId;
}

ServerConfiguration::Builder& ServerConfiguration::Builder::withServerId(int32_t serverId)
{
	mServerId = serverId;
	return *this;
}

int32_t ServerConfiguration::Builder::getBeaconSizeInBytes() const
{
	return mBeaconSizeInBytes;
}

ServerConfiguration::Builder& ServerConfiguration::Builder::withBeaconSizeInBytes(int32_t beaconSize)
{
	mBeaconSizeInBytes = beaconSize;
	return *this;
}

int32_t ServerConfiguration::Builder::getMultiplicity() const
{
	return mMultiplicity;
}

ServerConfiguration::Builder& ServerConfiguration::Builder::withMultiplicity(int32_t multiplicity)
{
	mMultiplicity = multiplicity;
	return *this;
}

int32_t ServerConfiguration::Builder::getMaxSessionDurationInMilliseconds() const
{
	return mMaxSessionDurationInMilliseconds;
}

ServerConfiguration::Builder& ServerConfiguration::Builder::withMaxSessionDurationInMilliseconds(
		int32_t maxSessionDurationInMilliseconds)
{
	mMaxSessionDurationInMilliseconds = maxSessionDurationInMilliseconds;
	return *this;
}

int32_t ServerConfiguration::Builder::getMaxEventsPerSession() const
{
	return mMaxEventsPerSession;
}

ServerConfiguration::Builder& ServerConfiguration::Builder::withMaxEventsPerSession(int maxEventsPerSession)
{
	mMaxEventsPerSession = maxEventsPerSession;
	return *this;
}

int32_t ServerConfiguration::Builder::getSessionTimeoutInMilliseconds() const
{
	return mSessionIdleTimeout;
}

ServerConfiguration::Builder& ServerConfiguration::Builder::withSessionTimeoutInMilliseconds(
		int32_t sessionTimeoutInMilliseconds)
{
	mSessionIdleTimeout = sessionTimeoutInMilliseconds;
	return *this;
}

int32_t ServerConfiguration::Builder::getVisitStoreVersion() const
{
	return mVisitStoreVersion;
}

ServerConfiguration::Builder& ServerConfiguration::Builder::withVisitStoreVersion(int32_t visitStoreVersion)
{
	mVisitStoreVersion = visitStoreVersion;
	return *this;
}

std::shared_ptr<core::configuration::IServerConfiguration> ServerConfiguration::Builder::build()
{
	return std::make_shared<ServerConfiguration>(*this);
}