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

#include "configuration/Configuration.h"
#include "configuration/HTTPClientConfiguration.h"


using namespace configuration;

Configuration::Configuration(const HTTPClientConfiguration& httpClientConfiguration)
	: mHTTPClientConfiguration(httpClientConfiguration)
	, mIsCapture(false)
{
}

const HTTPClientConfiguration& Configuration::getHTTPClientConfiguration() const
{
	return mHTTPClientConfiguration;
}

void Configuration::updateSettings(std::unique_ptr<protocol::StatusResponse> statusResponse)
{
	if (statusResponse == nullptr)
	{
		return;
	}

	if (statusResponse->getResponseCode() != 200)
	{
		return;
	}

	//TODO johannes.baeuerle update settings based on response
	//throw std::runtime_error("not implemented");
}

bool Configuration::isCapture() const
{
	return mIsCapture;
}
