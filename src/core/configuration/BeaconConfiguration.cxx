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

#include "BeaconConfiguration.h"
#include "HTTPClientConfiguration.h"
#include "ServerConfiguration.h"

using namespace core::configuration;

BeaconConfiguration::BeaconConfiguration(
	std::shared_ptr<IOpenKitConfiguration> openKitConfig,
	std::shared_ptr<IPrivacyConfiguration> privacyConfig,
	int32_t serverId
)
	: mOpenKitConfiguration(openKitConfig)
	, mPrivacyConfiguration(privacyConfig)
	, mHTTPClientConfiguration(
		HTTPClientConfiguration::Builder(openKitConfig)
			.withServerID(serverId)
			.build()
	)
	, mServerConfiguration(nullptr)
	, mIsServerConfigurationSet(false)
	, mServerConfigurationUpdateCallback(nullptr)
	, mMutex()
{
}

std::shared_ptr<IBeaconConfiguration> BeaconConfiguration::from(
		std::shared_ptr<core::configuration::IOpenKitConfiguration> openKitConfig,
		std::shared_ptr<core::configuration::IPrivacyConfiguration> privacyConfig, int32_t serverId)
{
	if (openKitConfig == nullptr || privacyConfig == nullptr)
	{
		return nullptr;
	}

	return std::make_shared<BeaconConfiguration>(openKitConfig, privacyConfig, serverId);
}

std::shared_ptr<IOpenKitConfiguration> BeaconConfiguration::getOpenKitConfiguration() const
{
	return mOpenKitConfiguration;
}

std::shared_ptr<IPrivacyConfiguration> BeaconConfiguration::getPrivacyConfiguration() const
{
	return mPrivacyConfiguration;
}

std::shared_ptr<IHTTPClientConfiguration> BeaconConfiguration::getHTTPClientConfiguration() const
{
	return mHTTPClientConfiguration;
}

std::shared_ptr<IServerConfiguration> BeaconConfiguration::getServerConfiguration()
{
	// synchronized scope
	std::lock_guard<std::recursive_mutex> lock(mMutex);
	return getServerConfigurationOrDefault();
}

void BeaconConfiguration::initializeServerConfiguration(std::shared_ptr<IServerConfiguration> initialServerConfiguration)
{
	if ((initialServerConfiguration == nullptr)
		|| (initialServerConfiguration == ServerConfiguration::defaultInstance()))
	{
		// ignore DEFAULT configuration since server configuration update does not take over certain attributes
		// when merging and the configuration already exists.
		return;
	}

	ServerConfigurationUpdateCallback callback;
	{ // synchronized scope
		std::lock_guard<std::recursive_mutex> lock(mMutex);

		if (isServerConfigurationSet())
		{
			return;
		}

		mServerConfiguration = initialServerConfiguration;

		callback = mServerConfigurationUpdateCallback;
	}

	if (callback != nullptr)
	{
		callback(initialServerConfiguration);
	}
}

std::shared_ptr<IServerConfiguration> BeaconConfiguration::getServerConfigurationOrDefault()
{
	auto serverConfig = mServerConfiguration;
	return serverConfig != nullptr ? serverConfig : ServerConfiguration::defaultInstance();
}

void BeaconConfiguration::updateServerConfiguration(
		std::shared_ptr<core::configuration::IServerConfiguration> newServerConfiguration)
{
	if (newServerConfiguration == nullptr)
	{
		return;
	}

	ServerConfigurationUpdateCallback callback;
	{ // synchronized scope
		std::lock_guard<std::recursive_mutex> lock(mMutex);

		if (mServerConfiguration != nullptr)
		{
			newServerConfiguration = mServerConfiguration->merge(newServerConfiguration);
		}

		mServerConfiguration = newServerConfiguration;
		mIsServerConfigurationSet = true;

		callback = mServerConfigurationUpdateCallback;
	}

	// notify has to be called outside of the synchronized block to avoid deadlock situation
	if (callback != nullptr)
	{
		callback(newServerConfiguration);
	}
}

void BeaconConfiguration::enableCapture()
{
	updateCaptureWith(true);
}

void BeaconConfiguration::disableCapture()
{
	updateCaptureWith(false);
}

void BeaconConfiguration::updateCaptureWith(bool captureState)
{
	std::lock_guard<std::recursive_mutex> lock(mMutex);

	auto currentServerConfig = getServerConfigurationOrDefault();
	mServerConfiguration = ServerConfiguration::Builder(currentServerConfig)
		.withCapture(captureState)
		.build();

	mIsServerConfigurationSet = true;
}

bool BeaconConfiguration::isServerConfigurationSet()
{
	//synchronized scope
	std::lock_guard<std::recursive_mutex> lock(mMutex);

	return mIsServerConfigurationSet;
}

void BeaconConfiguration::setServerConfigurationUpdateCallback(ServerConfigurationUpdateCallback serverConfigurationUpdateCallback)
{
	std::lock_guard<std::recursive_mutex> lock(mMutex);

	mServerConfigurationUpdateCallback = serverConfigurationUpdateCallback;
}
