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
#include <iostream>

#include "core/BeaconSender.h"
#include "core/util/CommandLineArguments.h"
#include "providers/DefaultHTTPClientProvider.h"
#include "providers/DefaultSessionIDProvider.h"
#include "providers/DefaultTimingProvider.h"
#include "configuration/HTTPClientConfiguration.h"
#include "configuration/OpenKitType.h"
#include "configuration/Device.h"

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
		std::cerr << "Error: Configuration given on command line is not valid." << std::endl;
		commandLine.printHelp();
		std::cerr << "Exiting." << std::endl;
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
	std::shared_ptr<ISessionIDProvider> sessionIDProvider = std::shared_ptr<ISessionIDProvider>(new DefaultSessionIDProvider());

	std::shared_ptr<configuration::Device> device = std::shared_ptr<configuration::Device>(new configuration::Device(core::UTF8String("ACME OS"), core::UTF8String("Dynatrace"), core::UTF8String("Model E")));

	std::shared_ptr<Configuration> configuration = std::shared_ptr<Configuration>(new Configuration(device, configuration::OpenKitType::CreateDynatraceType(),
																									core::UTF8String("openkit-sample"), applicationID, serverID, beaconURL,
																									httpClientConfig, sessionIDProvider));

	BeaconSender sender(configuration, httpClientProvider, timingProvider);
	
	sender.initialize();

	return 0;
}