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

#include "api-c/OpenKit-c.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

const char APPLICATION_VERSION[] = "1.2.3";

void parseCommandLine(int32_t argc, char** argv, char** beaconURL, uint32_t* serverID, char** applicationID)
{
	int32_t index = 2;
	if (argc > 2 && argv != NULL)
	{
		while (index < argc)
		{
			char* prev = argv[index - 1];
			char* cur = argv[index];
			if (strcmp("-u", prev) == 0)
			{
				*beaconURL = cur;
			}
			else if (strcmp("-a", prev) == 0)
			{
				*applicationID = cur;
			}
			else if (strcmp("-s", prev) == 0)
			{
				*serverID = atoi(cur);
			}
			index++;
		}
	}
}

#define UNUSED_ARG(x) ((x)=(x))

bool levelEnabledFunction(LOG_LEVEL level) {
	UNUSED_ARG(level);
	return true;
}

#define MAX_TIME_LEN 24 // e.g. 2063-04-05 21:48:12.123

void logFunction(LOG_LEVEL level, const char* traceStatement)
{
	time_t in_time_t;
	char buffer[MAX_TIME_LEN];
	struct tm tmNow;
	time(&in_time_t);
#if defined (_MSC_VER)
	localtime_s(&tmNow, &in_time_t);
#else
	localtime_r(&in_time_t, &tmNow);
#endif
	strftime(buffer, MAX_TIME_LEN, "%Y-%m-%d %H:%M:%S", &tmNow);
	puts(buffer);
	printf("%s [%s] %s", buffer, toString(level), traceStatement);
}

int32_t main(int32_t argc, char** argv)
{
	char* beaconURL = NULL;
	uint32_t serverID = 0;
	char* applicationID = NULL;

	parseCommandLine(argc, argv, &beaconURL, &serverID, &applicationID);

	printf("BeaconURL = %s\n", beaconURL);
	printf("ApplicationID = %s\n", applicationID);
	printf("ServerID = %u\n", serverID);

	struct LoggerHandle* loggerHandle = createLogger(&levelEnabledFunction, &logFunction);
	struct OpenKitHandle* openKitHandle = createDynatraceOpenKit(beaconURL, applicationID, serverID, loggerHandle);
	struct SessionHandle* sessionHandle = createSession(openKitHandle, "1.2.3.4");
	identifyUser(sessionHandle, "test user");
	struct RootActionHandle* rootActionHandle = enterRootAction(sessionHandle, "some root action");
	
	reportEventOnRootAction(rootActionHandle, "An event occurred");
	reportIntValueOnRootAction(rootActionHandle, "IntValue", 1234);
	reportDoubleValueOnRootAction(rootActionHandle, "DblValue", 3.1415);
	reportStringValueOnRootAction(rootActionHandle, "StringValue", "some string");

	leaveRootAction(rootActionHandle);
	endSession(sessionHandle);
	shutdownOpenKit(openKitHandle);

	return 0;
}