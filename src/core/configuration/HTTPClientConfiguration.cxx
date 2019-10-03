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

#include "HTTPClientConfiguration.h"

using namespace core::configuration;

HTTPClientConfiguration::HTTPClientConfiguration
(
	const core::UTF8String& url,
	uint32_t serverID,
	const core::UTF8String& applicationID,
	std::shared_ptr<openkit::ISSLTrustManager> sslTrustManager
)
	: mBaseURL(url)
	, mServerID(serverID)
	, mApplicationID(applicationID)
	, mSSLTrustManager(sslTrustManager)
{
}

const core::UTF8String& HTTPClientConfiguration::getBaseURL() const
{
	return mBaseURL;
}

int32_t HTTPClientConfiguration::getServerID() const
{
	return mServerID;
}

const core::UTF8String& HTTPClientConfiguration::getApplicationID() const
{
	return mApplicationID;
}

std::shared_ptr<openkit::ISSLTrustManager> HTTPClientConfiguration::getSSLTrustManager() const
{
	return mSSLTrustManager;
}

