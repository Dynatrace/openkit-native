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

/// this basic sample creates a Session, a RootAction and an Action
/// to report some values and a web request
/// it shows the intended usage of the OpenKit API

#include <stdint.h>
#include <iostream>
#include <thread>

#include "OpenKit.h" // TODO stefan.eberl - include from <OpenKit/OpenKit.h>
#include "CommandLineArguments.h"

constexpr char APPLICATION_VERSION[] = "1.2.3";

void parseCommandLine(uint32_t argc, char** argv, std::string& beaconURL, uint32_t& serverID, std::string& applicationID)
{
	sample::CommandLineArguments commandLine;
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
	std::string beaconURL;
	uint32_t serverID = 0;
	std::string applicationID;

	parseCommandLine(argc, argv, beaconURL, serverID, applicationID);

	openkit::DynatraceOpenKitBuilder builder(beaconURL.c_str(), applicationID.c_str(), serverID);
	builder.withApplicationName("openkit-sample-c++")
		.withApplicationVersion(APPLICATION_VERSION)
		.withManufacturer("Dynatrace")
		.withOperatingSystem("ACME OS")
		.withModelID("Model E")
		.withLogLevel(openkit::LogLevel::LOG_LEVEL_DEBUG);

	auto openKit = builder.build();
	openKit->waitForInitCompletion(20000);

	if (openKit->isInitialized())
	{
		std::shared_ptr<openkit::ISession> sampleSession = openKit->createSession("172.16.23.30");
		sampleSession->identifyUser("test user");

		auto rootAction1 = sampleSession->enterAction("root action");
		auto childAction1 = rootAction1->enterAction("child action");

		rootAction1->reportValue("the answer", 42);

		childAction1->reportValue("some string", "1337.3.1415");

		auto webRequest = childAction1->traceWebRequest("http://www.stackoverflow.com/");
		webRequest->start();

		std::this_thread::sleep_for(std::chrono::milliseconds(144));

		webRequest->setResponseCode(200);
		webRequest->setBytesSent(123);
		webRequest->setBytesReceived(45);
		webRequest->stop();

		childAction1->leaveAction();
		rootAction1->leaveAction();

		sampleSession->end();
	}

	openKit->shutdown();

	return 0;
}