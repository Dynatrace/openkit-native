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

#include "configuration/Configuration.h"
#include "configuration/HTTPClientConfiguration.h"
#include "core/util/URLEncoding.h"


using namespace configuration;

constexpr bool DEFAULT_CAPTURE = true;                            // default: capture on
constexpr int64_t DEFAULT_SEND_INTERVAL = 2 * 60 * 1000;          // default: wait 2m (in ms) to send beacon
constexpr int32_t DEFAULT_MAX_BEACON_SIZE = 30 * 1024;            // default: max 30KB (in B) to send in one beacon
constexpr bool DEFAULT_CAPTURE_ERRORS = true;                     // default: capture errors on
constexpr bool DEFAULT_CAPTURE_CRASHES = true;                    // default: capture crashes on

Configuration::Configuration(std::shared_ptr<configuration::Device> device, OpenKitType openKitType, const core::UTF8String& applicationName, const core::UTF8String& applicationVersion, const core::UTF8String& applicationID, const core::UTF8String& deviceID, const core::UTF8String& endpointURL,
	std::shared_ptr<providers::ISessionIDProvider> sessionIDProvider, std::shared_ptr<openkit::ISSLTrustManager> sslTrustManager,
	std::shared_ptr<configuration::BeaconCacheConfiguration> beaconCacheConfiguration, std::shared_ptr<configuration::BeaconConfiguration> beaconConfiguration)
	: mHTTPClientConfiguration(std::make_shared<configuration::HTTPClientConfiguration>(endpointURL, openKitType.getDefaultServerID(), applicationID, sslTrustManager))
	, mSessionIDProvider(sessionIDProvider)
	, mIsCapture(false)
	, mSendInterval(DEFAULT_SEND_INTERVAL)
	, mMaxBeaconSize(DEFAULT_MAX_BEACON_SIZE)
	, mCaptureErrors(DEFAULT_CAPTURE_ERRORS)
	, mCaptureCrashes(DEFAULT_CAPTURE_CRASHES)
	, mOpenKitType(openKitType)
	, mApplicationName(applicationName)
	, mApplicationID(applicationID)
	, mApplicationIDPercentEncoded(core::util::URLEncoding::urlencode(applicationID, { '_' } ))
	, mApplicationVersion(applicationVersion)
	, mEndpointURL(endpointURL)
	, mDeviceID(deviceID)
	, mDevice(device)
	, mBeaconCacheConfiguration(beaconCacheConfiguration)
	, mBeaconConfiguration(beaconConfiguration)
{
}

std::shared_ptr<HTTPClientConfiguration> Configuration::getHTTPClientConfiguration() const
{
	return mHTTPClientConfiguration;
}

void Configuration::updateSettings(std::shared_ptr<protocol::StatusResponse> statusResponse)
{
	if (statusResponse == nullptr || statusResponse->getResponseCode() != 200)
	{
		disableCapture();
		return;
	}

	mIsCapture = statusResponse->isCapture();

	//if capture is off -> leave other settings on their current values
	if (!statusResponse->isCapture())
	{
		return;
	}

	//use server ID from beacon response or default
	int32_t newServerID = statusResponse->getServerID();
	if (newServerID == -1)
	{
		newServerID = mOpenKitType.getDefaultServerID();
	}

	//check if HTTP configuration changed
	if (mHTTPClientConfiguration->getServerID() != newServerID)
	{
		mHTTPClientConfiguration = std::make_shared<configuration::HTTPClientConfiguration>(mEndpointURL, 
																							newServerID,
																							mApplicationID, 
																							mHTTPClientConfiguration->getSSLTrustManager());
	}

	// use send interval from beacon response or default
	auto newSendInterval = statusResponse->getSendInterval();
	if (newSendInterval == -1)
	{
		newSendInterval = DEFAULT_SEND_INTERVAL;
	}
	if (newSendInterval != mSendInterval)
	{
		mSendInterval = newSendInterval;
	}

	// use max beacon size from beacon response or default
	auto newMaxBeaconSize = statusResponse->getMaxBeaconSize();
	if (newMaxBeaconSize == -1)
	{
		newMaxBeaconSize = DEFAULT_MAX_BEACON_SIZE;
	}
	if (newMaxBeaconSize != mMaxBeaconSize)
	{
		mMaxBeaconSize = newMaxBeaconSize;
	}

	// use capture settings for errors and crashes
	mCaptureErrors = statusResponse->isCaptureErrors();
	mCaptureCrashes = statusResponse->isCaptureCrashes();
}

void Configuration::enableCapture()
{
	mIsCapture = true;
}

void Configuration::disableCapture()
{
	mIsCapture = false;
}

bool Configuration::isCapture() const
{
	return mIsCapture;
}

int32_t Configuration::createSessionNumber()
{
	if (mSessionIDProvider != nullptr)
	{
		return mSessionIDProvider->getNextSessionID();
	}
	else
	{
		return 0;
	}
}

const char* Configuration::getOpenKitType() const
{
	return mOpenKitType.getTypeString();
}

const core::UTF8String& Configuration::getApplicationName() const
{
	return mApplicationName;
}

const core::UTF8String& Configuration::getApplicationID() const
{
	return mApplicationID;
}

const core::UTF8String& Configuration::getApplicationIDPercentEncoded() const
{
	return mApplicationIDPercentEncoded;
}

const core::UTF8String& Configuration::getApplicationVersion() const
{
	return mApplicationVersion;
}

const core::UTF8String& Configuration::getEndpointURL() const
{
	return mEndpointURL;
}

const core::UTF8String& Configuration::getDeviceID() const
{
	return mDeviceID;
}

int64_t Configuration::getSendInterval() const
{
	return mSendInterval;
}

void Configuration::setSendInterval(int64_t sendInterval)
{
	mSendInterval = sendInterval;
}

int32_t Configuration::getMaxBeaconSize() const
{
	return mMaxBeaconSize;
}

bool Configuration::isCaptureErrors() const
{
	return mCaptureErrors;
}

bool Configuration::isCaptureCrashes() const
{
	return mCaptureCrashes;
}

std::shared_ptr<configuration::Device> Configuration::getDevice() const
{
	return mDevice;
}

std::shared_ptr<configuration::BeaconCacheConfiguration> Configuration::getBeaconCacheConfiguration() const
{
	return mBeaconCacheConfiguration;
}

std::shared_ptr<configuration::BeaconConfiguration> Configuration::getBeaconConfiguration() const
{
	return mBeaconConfiguration;
}