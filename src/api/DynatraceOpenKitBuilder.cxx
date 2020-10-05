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

#include "OpenKit/DynatraceOpenKitBuilder.h"

using namespace openkit;

DynatraceOpenKitBuilder::DynatraceOpenKitBuilder(const char* endpointURL, const char* applicationID, int64_t deviceID)
	: AbstractOpenKitBuilder(endpointURL, deviceID)
	, mApplicationID(applicationID)
	, mApplicationName()
	, mOpenKitType(OPENKIT_TYPE)
{
}

DynatraceOpenKitBuilder::DynatraceOpenKitBuilder(const char* endpointURL, const char* applicationID, const char* deviceID)
	: AbstractOpenKitBuilder(endpointURL, deviceID)
	, mApplicationID(applicationID)
	, mApplicationName()
	, mOpenKitType(OPENKIT_TYPE)
{
}

DynatraceOpenKitBuilder& DynatraceOpenKitBuilder::withApplicationName(const char* applicationName)
{
	if (applicationName != nullptr)
	{
		mApplicationName = std::string(applicationName);
	}
	return *this;
}

const std::string& DynatraceOpenKitBuilder::getOpenKitType() const
{
	return mOpenKitType;
}

const std::string& DynatraceOpenKitBuilder::getApplicationID() const
{
	return mApplicationID;
}

const std::string& DynatraceOpenKitBuilder::getApplicationName() const
{
	return mApplicationName;
}

int32_t DynatraceOpenKitBuilder::getDefaultServerID() const
{
	return DEFAULT_SERVER_ID;
}
