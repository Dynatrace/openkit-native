/**
 * Copyright 2018-2021 Dynatrace LLC
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

#include "OpenKit/OpenKit-c.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// platform dependent includes for sleep
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

const char APPLICATION_VERSION[] = "1.2.3";

///
/// Parses the command line arguments, thereby reading in the "endpointURL",
/// the "serverID" and the "applicationID".
/// @param[in] argc the command line argument count
/// @param[in] argv the command line arguments
/// @param[out] endpointURL Dynatrace endpoint URL, e.g. "https://bf1234567xyz.com/mbeacon"
/// @param[out] deviceID the numeric ID of the device
/// @param[out] applicationID the ID of the application
///
bool parseCommandLine(int32_t argc, char** argv, char** endpointURL, uint32_t* deviceID, char** applicationID)
{
	*deviceID = 0;
	*endpointURL = NULL;
	*applicationID = NULL;

	int32_t index = 2;
	if (argc > 2 && argv != NULL)
	{
		while (index < argc)
		{
			char* prev = argv[index - 1];
			char* cur = argv[index];
			if (strcmp("-u", prev) == 0)
			{
				*endpointURL = cur;
			}
			else if (strcmp("-a", prev) == 0)
			{
				*applicationID = cur;
			}
			else if (strcmp("-d", prev) == 0)
			{
				*deviceID = atoi(cur);
			}
			else if (strcmp("-s", prev) == 0)
			{
				fprintf(stderr, "parameter -s is deprecated, use -d instead.");
			}
			index++;
		}
	}

	return *deviceID > 0 && *endpointURL != NULL && *applicationID != NULL;
}

#define UNUSED_ARG(x) ((x)=(x))

///
/// Check function if the provided log level is enabled in the logger or not.
/// @param[in] level the log level, either DEBUG or INFO (WARN and ERROR traces are currently always logged)
/// @return @c true for all cases in this sample
///
bool levelEnabledFunction(LOG_LEVEL level)
{
	UNUSED_ARG(level);
	return true;
}

#define MAX_TIME_LEN 24 // e.g. 2063-04-05 21:48:12.123

///
/// Log function to write the trace statements to stdout. The function prepends a human-readable time stamp
/// and the log level.
///
/// @param[in] level the log level, either DEBUG or INFO (WARN and ERROR traces are currently always logged)
/// @param[in] traceStatement the already formatted trace statement string
///
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
	printf("%s [%s] %s\n", buffer, toString(level), traceStatement);
}

///
/// This sample demonstrate the usage of the C binding API of OpenKit C++.
/// It basically calls each function on the C API with demo parameters.
/// @param[in] argc number of arguments
/// @param[in] argv the arguments, provide (at least) -u endpointURL -d deviceID -a applicationID
///
int32_t main(int32_t argc, char** argv)
{
	char* endpointURL = NULL;
	uint32_t deviceID = 0;
	char* applicationID = NULL;

	bool hasValidCommandLineParams = parseCommandLine(argc, argv, &endpointURL, &deviceID, &applicationID);
	if (!hasValidCommandLineParams)
	{
		fprintf(stderr, "The application is is called the following way and requires all arguments.");
		fprintf(stderr, "./openkit-sample -a <application id> -u <endpoint url> -d <device id>");
		exit(-1);
	}

	printf("EndpointURL = %s\n", endpointURL);
	printf("ApplicationID = %s\n", applicationID);
	printf("DeviceID = %u\n", deviceID);

	struct LoggerHandle* loggerHandle = createLogger(&levelEnabledFunction, &logFunction);

	struct OpenKitConfigurationHandle* configurationHandle = createOpenKitConfiguration(endpointURL, applicationID, deviceID);

	useLoggerForConfiguration(configurationHandle, loggerHandle);
	useApplicationVersionForConfiguration(configurationHandle, "v0.1.x");
	useTrustModeForConfiguration(configurationHandle, STRICT_TRUST, NULL);
	useOperatingSystemForConfiguration(configurationHandle, "Test OS");
	useManufacturerForConfiguration(configurationHandle, "Dynatrace");
	useModelIDForConfiguration(configurationHandle, "Some unavailable model");
	useDataCollectionLevelForConfiguration(configurationHandle, DATA_COLLECTION_LEVEL_OFF);
	useCrashReportingLevelForConfiguration(configurationHandle, CRASH_REPORTING_LEVEL_OFF);

	struct OpenKitHandle* openKitHandle = createDynatraceOpenKit(configurationHandle);
	destroyOpenKitConfiguration(configurationHandle);

	waitForInitCompletionWithTimeout(openKitHandle, 20000);

	if (isInitialized(openKitHandle))
	{
		struct SessionHandle* sessionHandle = createSession(openKitHandle, "1.2.3.4");
		identifyUser(sessionHandle, "test user");
		struct RootActionHandle* rootActionHandle = enterRootAction(sessionHandle, "My root action");

		reportEventOnRootAction(rootActionHandle, "An event occurred");
		reportIntValueOnRootAction(rootActionHandle, "IntValue", 1234);
		reportDoubleValueOnRootAction(rootActionHandle, "DblValue", 3.1415);
		reportStringValueOnRootAction(rootActionHandle, "StringValue", "some string");
		reportErrorOnRootAction(rootActionHandle, "Some error name", 0x80050023, "Some more detailed error reason");

		struct ActionHandle* actionHandle = enterAction(rootActionHandle, "My sub action");
		reportEventOnAction(actionHandle, "An event occurred on the subaction");
		reportIntValueOnAction(actionHandle, "IntValue", 4567);
		reportDoubleValueOnAction(actionHandle, "DblValue", 47.11);
		reportStringValueOnAction(actionHandle, "StringValue", "some string");
		reportErrorOnAction(actionHandle, "Some error name", 0x80070001, "Some more detailed error reason");

		struct WebRequestTracerHandle* webRequest = traceWebRequestOnAction(actionHandle, "http://www.stackoverflow.com/");
		startWebRequest(webRequest);
#ifdef _WIN32
		Sleep(144);// 144 ms
#else
		usleep(144 * 1000);
#endif
		setBytesSent(webRequest, 123);
		setBytesReceived(webRequest, 45);
		stopWebRequestWithResponseCode(webRequest, 200);

		leaveAction(actionHandle);
		leaveRootAction(rootActionHandle);

		reportCrash(sessionHandle, "Session crashes", "For no reason", "Stacktrace is missing");

		endSession(sessionHandle);
	}
	shutdownOpenKit(openKitHandle);
	destroyLogger(loggerHandle);

	return 0;
}
