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

#include "HTTPClientConfiguration.h"

using namespace configuration;

HTTPClientConfiguration::HTTPClientConfiguration(core::UTF8String URL, uint32_t serverID, core::UTF8String applicationID)
	: mBaseURL(URL)
	, mServerID(serverID)
	, mApplicationID(applicationID)
{
}

core::UTF8String HTTPClientConfiguration::getBaseURL()
{
	return mBaseURL;
}

uint32_t HTTPClientConfiguration::getServerID()
{
	return mServerID;
}

core::UTF8String HTTPClientConfiguration::getApplicationID()
{
	return mApplicationID;
}