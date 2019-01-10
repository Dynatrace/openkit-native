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

#include "OpenKit/DynatraceOpenKitBuilder.h"
#include "providers/DefaultSessionIDProvider.h"
#include "configuration/Configuration.h"

using namespace openkit;

DynatraceOpenKitBuilder::DynatraceOpenKitBuilder(const char* endpointURL, const char* applicationID, int64_t deviceID)
	: AbstractOpenKitBuilder(endpointURL, deviceID)
	, mApplicationID(applicationID)
	, mApplicationName()
{

}

std::shared_ptr<configuration::Configuration> DynatraceOpenKitBuilder::buildConfiguration()
{
	std::shared_ptr<configuration::Device> device = std::make_shared<configuration::Device>(getOperatingSystem(), getManufacturer(), getModelID());

	std::shared_ptr<configuration::BeaconCacheConfiguration> beaconCacheConfiguration = std::make_shared<configuration::BeaconCacheConfiguration>(
			getBeaconCacheMaxRecordAge(),
			getBeaconCacheLowerMemoryBoundary(),
			getBeaconCacheUpperMemoryBoundary()
		);

	std::shared_ptr<configuration::BeaconConfiguration> beaconConfiguration = std::make_shared<configuration::BeaconConfiguration>(
		configuration::BeaconConfiguration::DEFAULT_MULTIPLICITY, // starting with default multiplicity, value changed according to server response
		getDataCollectionLevel(),
		getCrashReportingLevel()
		);

	return std::make_shared<configuration::Configuration>(
			device,	
			configuration::OpenKitType::Type::DYNATRACE,
			mApplicationName,
			getApplicationVersion(),
			mApplicationID,
			getDeviceID(),
			getEndpointURL(),
			std::make_shared<providers::DefaultSessionIDProvider>(),
			getTrustManager(),
			beaconCacheConfiguration,
			beaconConfiguration
		);
}

DynatraceOpenKitBuilder& DynatraceOpenKitBuilder::withApplicationName(const char* applicationName)
{
	if (applicationName != nullptr)
	{
		mApplicationName = std::string(applicationName);
	}
	return *this;
}

