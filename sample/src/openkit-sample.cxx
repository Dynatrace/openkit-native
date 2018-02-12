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
#include "core/util/CommandLineArguments.h"
#include "providers/DefaultHTTPClientProvider.h"
#include "providers/DefaultTimingProvider.h"
#include "configuration/HTTPClientConfiguration.h"

using namespace core;
using namespace communication;
using namespace providers;
using namespace configuration;

void parseCommandLine(uint32_t argc, char** argv, UTF8String& beaconURL, uint32_t& serverID, UTF8String& applicationID)
{
	core::util::CommandLineArguments commandLine;
	commandLine.parse(argc, argv);

	if (commandLine.isValidConfiguration())
	{
		beaconURL = commandLine.getBeaconURL();
		serverID = commandLine.getServerID();
		applicationID = commandLine.getApplicationID();
	}
	else
	{
		printf("Error: Configuration given on command line is not valid.\n");
		commandLine.printHelp();
		printf("Exitting.\n");
		exit(-1);
	}

}

int32_t main(int32_t argc, char** argv)
{
	UTF8String beaconURL;
	uint32_t serverID = 0;
	UTF8String applicationID;

	parseCommandLine(argc, argv, beaconURL, serverID, applicationID);

	std::shared_ptr<HTTPClientConfiguration> httpClientConfig = std::shared_ptr<HTTPClientConfiguration>(new HTTPClientConfiguration(beaconURL, serverID, applicationID));

	std::shared_ptr<IHTTPClientProvider> httpClientProvider = std::shared_ptr<IHTTPClientProvider>(new DefaultHTTPClientProvider());
	std::shared_ptr<ITimingProvider> timingProvider = std::shared_ptr<ITimingProvider>(new DefaultTimingProvider());
	std::shared_ptr<Configuration> configuration = std::shared_ptr<Configuration>(new Configuration(httpClientConfig));

	BeaconSender sender(configuration, httpClientProvider, timingProvider);
	
	sender.initialize();

	std::this_thread::sleep_for(std::chrono::seconds(2));

	sender.shutdown();
	
	return 0;
}