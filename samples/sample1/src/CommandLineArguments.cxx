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

#include "CommandLineArguments.h"

using namespace sample;

CommandLineArguments::CommandLineArguments()
	: mServerID(0)
	, mApplicationID("")
	, mBeaconURL("")
{
}

void CommandLineArguments::parse(uint32_t argc, char** argv)
{
	uint32_t index = 2;//start at index 2, 0 is the binary name, if only 1 further argument exists this already is an error
	if (argc > 2 && argv != nullptr)
	{
		while (index < argc)
		{
			std::string previous = std::string(argv[index - 1]);
			std::string current = std::string(argv[index]);

			if (previous == "-a")
			{
				mApplicationID = current;
			}

			if (previous == "-u")
			{
				mBeaconURL = current;
			}

			if (previous == "-s")
			{
				try
				{
					mServerID = std::stoi(current);
				}
				catch (...)
				{
					mServerID = -1;
				}
			}
			index++;
		}
	}
}

const std::string& CommandLineArguments::getApplicationID() const
{
	return mApplicationID;
}

const std::string& CommandLineArguments::getBeaconURL() const
{
	return mBeaconURL;
}

int32_t CommandLineArguments::getServerID() const
{
	return mServerID;
}

bool CommandLineArguments::isValidConfiguration() const
{
	return (mServerID > 0) && !mBeaconURL.empty() && !mApplicationID.empty();
}

void CommandLineArguments::printHelp()
{
	std::cerr << "The application is is called the following way and requires all arguments." << std::endl;
	std::cerr << "./openkit-sample -a <application id> -u <beacon url> -s <server id>" << std::endl;
}