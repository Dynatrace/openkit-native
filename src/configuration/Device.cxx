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

#include "Device.h"

using namespace configuration;

Device::Device(const core::UTF8String& operatingSystem, const core::UTF8String& manufacturer, const core::UTF8String& modelID)
	: mOperatingSystem(operatingSystem)
	, mManufacturer(manufacturer)
	, mModelID(modelID)
{
}

const core::UTF8String& Device::getOperatingSystem() const
{
	return mOperatingSystem;
}

const core::UTF8String& Device::getManufacturer() const
{
	return mManufacturer;
}

const core::UTF8String& Device::getModelID() const
{
	return mModelID;
}