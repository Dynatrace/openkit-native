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
#include "core/util/DefaultLogger.h"
#include "providers/DefaultHTTPClientProvider.h"
#include "providers/DefaultSessionIDProvider.h"
#include "providers/DefaultTimingProvider.h"
#include "providers/DefaultThreadIDProvider.h"
#include "configuration/HTTPClientConfiguration.h"
#include "configuration/OpenKitType.h"
#include "configuration/Device.h"
#include "protocol/ssl/SSLStrictTrustManager.h"
#include "protocol/Beacon.h"
#include "core/Session.h"
#include "core/RootAction.h"
#include "caching/BeaconCache.h"

using namespace core;
using namespace communication;
using namespace providers;
using namespace configuration;

constexpr char APPLICATION_VERSION[] = "1.2.3";

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

	auto logger = std::shared_ptr<api::ILogger>(new core::util::DefaultLogger(true));
	std::shared_ptr<protocol::ISSLTrustManager> trustManager = std::make_shared<protocol::SSLStrictTrustManager>();

	std::shared_ptr<IHTTPClientProvider> httpClientProvider = std::shared_ptr<IHTTPClientProvider>(new DefaultHTTPClientProvider());
	std::shared_ptr<ITimingProvider> timingProvider = std::shared_ptr<ITimingProvider>(new DefaultTimingProvider());
	std::shared_ptr<ISessionIDProvider> sessionIDProvider = std::shared_ptr<ISessionIDProvider>(new DefaultSessionIDProvider());
	std::shared_ptr<IThreadIDProvider> threadIDProvider = std::shared_ptr<IThreadIDProvider>(new DefaultThreadIDProvider());

	std::shared_ptr<configuration::Device> device = std::shared_ptr<configuration::Device>(new configuration::Device(core::UTF8String("ACME OS"), core::UTF8String("Dynatrace"), core::UTF8String("Model E")));

	std::shared_ptr<Configuration> configuration = std::shared_ptr<Configuration>(new Configuration(device, configuration::OpenKitType::DYNATRACE,
																									core::UTF8String("openkit-sample"), APPLICATION_VERSION, applicationID, serverID, beaconURL,
																									sessionIDProvider, trustManager ));

	std::shared_ptr<caching::BeaconCache> beaconCache = std::make_shared<caching::BeaconCache>();

	std::shared_ptr<protocol::Beacon> beacon = std::make_shared<protocol::Beacon>(logger, beaconCache, configuration, UTF8String(""), threadIDProvider, timingProvider);
	
	std::shared_ptr<core::BeaconSender> sender = std::make_shared<core::BeaconSender>(logger, configuration, httpClientProvider, timingProvider);
	sender->initialize();
		
	std::shared_ptr<Session> sampleSession(new Session(logger, sender, beacon));
	sampleSession->identifyUser("test user");
	sampleSession->startSession();

	auto rootAction1 = sampleSession->enterAction("root action");
	auto childAction1 = rootAction1->enterAction("child action");

	childAction1->leaveAction();
	rootAction1->leaveAction();

	

	return 0;
}