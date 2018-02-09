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

#include <stdint.h>

#include "core/BeaconSender.h"
#include "providers/DefaultHTTPClientProvider.h"
#include "providers/DefaultTimingProvider.h"
#include "configuration/HTTPClientConfiguration.h"

using namespace core;
using namespace communication;
using namespace providers;
using namespace configuration;

void parseCommandLine(uint32_t argc, char** argv, UTF8String& beaconURL, uint32_t& serverID, UTF8String& applicationID)
{

}

int32_t main(int32_t argc, char** argv)
{
	UTF8String beaconURL = UTF8String("");
	uint32_t serverID = 0;
	UTF8String applicationID = UTF8String();

	parseCommandLine(argc, argv, beaconURL, serverID, applicationID);
	HTTPClientConfiguration httpClientConfig(beaconURL, serverID, applicationID);

	std::shared_ptr<DefaultHTTPClientProvider> httpClientProvider = std::shared_ptr<DefaultHTTPClientProvider>(new DefaultHTTPClientProvider());
	std::shared_ptr<DefaultTimingProvider> timingProvider = std::shared_ptr<DefaultTimingProvider>(new DefaultTimingProvider());
	std::shared_ptr<Configuration> configuration = std::shared_ptr<Configuration>(new Configuration(httpClientConfig));

	BeaconSender sender(configuration, httpClientProvider, timingProvider);
	
	sender.initialize();

	std::this_thread::sleep_for(std::chrono::seconds(2));

	sender.shutdown();
	
	return 0;
}