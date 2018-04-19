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

#include "AppMonOpenKitBuilder.h"
#include "providers/DefaultSessionIDProvider.h"

using namespace api;

AppMonOpenKitBuilder::AppMonOpenKitBuilder(const char* endpointURL, const char* applicationName, uint64_t deviceID)
	: AbstractOpenKitBuilder(endpointURL, deviceID)
	, mApplicationName(applicationName)
{

}


std::shared_ptr<configuration::Configuration> AppMonOpenKitBuilder::buildConfiguration()
{
	std::shared_ptr<configuration::Device> device = std::make_shared<configuration::Device>(getOperatingSystem(), getManufacturer(), getModelID());

	std::shared_ptr<configuration::BeaconCacheConfiguration> beaconCacheConfiguration = std::make_shared<configuration::BeaconCacheConfiguration>(
		getBeaconCacheMaxRecordAge(),
		getBeaconCacheLowerMemoryBoundary(),
		getBeaconCacheUpperMemoryBoundary()
		);

	return std::make_shared<configuration::Configuration>(
		device,
		configuration::OpenKitType::APPMON,
		mApplicationName,
		getApplicationVersion(),
		mApplicationName,
		getDeviceID(),
		getEndpointURL(),
		std::make_shared<providers::DefaultSessionIDProvider>(),
		getTrustManager(),
		beaconCacheConfiguration
		);
}
