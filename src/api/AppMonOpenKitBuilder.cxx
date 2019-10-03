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

#include "OpenKit/AppMonOpenKitBuilder.h"
#include "providers/DefaultSessionIDProvider.h"
#include "core/configuration/Configuration.h"

using namespace openkit;

AppMonOpenKitBuilder::AppMonOpenKitBuilder(const char* endpointURL, const char* applicationName, int64_t deviceID)
	: AbstractOpenKitBuilder(endpointURL, deviceID)
	, mApplicationName(applicationName)
{
}

AppMonOpenKitBuilder::AppMonOpenKitBuilder(const char* endpointURL, const char* applicationName, const char* deviceID)
	: AbstractOpenKitBuilder(endpointURL, deviceID)
	, mApplicationName(applicationName)
{
}

std::shared_ptr<core::configuration::Configuration> AppMonOpenKitBuilder::buildConfiguration()
{
	auto device = std::make_shared<core::configuration::Device>(getOperatingSystem(), getManufacturer(), getModelID());

	auto beaconCacheConfiguration = std::make_shared<core::configuration::BeaconCacheConfiguration>(
		getBeaconCacheMaxRecordAge(),
		getBeaconCacheLowerMemoryBoundary(),
		getBeaconCacheUpperMemoryBoundary()
		);

	auto beaconConfiguration = std::make_shared<core::configuration::BeaconConfiguration>(
		core::configuration::BeaconConfiguration::DEFAULT_MULTIPLICITY, // starting with default multiplicity, value changed according to server response
		getDataCollectionLevel(),
		getCrashReportingLevel()
		);

	return std::make_shared<core::configuration::Configuration>(
		device,
		core::configuration::OpenKitType::Type::APPMON,
		mApplicationName,
		getApplicationVersion(),
		mApplicationName,
		getDeviceID(),
		getOrigDeviceID(),
		getEndpointURL(),
		std::make_shared<providers::DefaultSessionIDProvider>(),
		getTrustManager(),
		beaconCacheConfiguration,
		beaconConfiguration
		);
}
