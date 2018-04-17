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

#include "AppMonOpenKitBuilder.h"

using namespace api;

AppMonOpenKitBuilder::AppMonOpenKitBuilder(const char* endpointURL, const char* applicationID, uint64_t deviceID)
	: AbstractOpenKitBuilder(endpointURL, deviceID)
{
	throw std::runtime_error("function not implemented yet");
}

std::shared_ptr<configuration::Configuration> AppMonOpenKitBuilder::buildConfiguration()
{
	throw std::runtime_error("function not implemented yet");
}