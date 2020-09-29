/**
 * Copyright 2018-2020 Dynatrace LLC
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

#include "OpenKit/AppMonOpenKitBuilder.h"

using namespace openkit;

const std::string AppMonOpenKitBuilder::OPENKIT_TYPE = "AppMonOpenKit";

AppMonOpenKitBuilder::AppMonOpenKitBuilder(const char* endpointURL, const char* applicationName, int64_t deviceID)
	: AbstractOpenKitBuilder(endpointURL, deviceID)
	, mApplicationName(applicationName)
{
}

AppMonOpenKitBuilder::AppMonOpenKitBuilder(const char* endpointURL, const char* applicationName, const char* deviceID)
	: AbstractOpenKitBuilder(endpointURL, deviceID)
	, mApplicationName(applicationName)
{
}

const std::string& AppMonOpenKitBuilder::getOpenKitType() const
{
	return OPENKIT_TYPE;
}

const std::string& AppMonOpenKitBuilder::getApplicationID() const
{
	return mApplicationName;
}

const std::string& AppMonOpenKitBuilder::getApplicationName() const
{
	return mApplicationName;
}

int32_t AppMonOpenKitBuilder::getDefaultServerID() const
{
	return DEFAULT_SERVER_ID;
}