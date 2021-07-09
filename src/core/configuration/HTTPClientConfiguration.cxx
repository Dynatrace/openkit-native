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

#include "HTTPClientConfiguration.h"

using namespace core::configuration;

HTTPClientConfiguration::HTTPClientConfiguration(
	HTTPClientConfiguration::Builder& builder
)
	: mBaseURL(builder.getBaseURL())
	, mServerID(builder.getServerID())
	, mApplicationID(builder.getApplicationID())
	, mSSLTrustManager(builder.getTrustManager())
	, mHttpRequestInterceptor(builder.getHttpRequestInterceptor())
	, mHttpResponseInterceptor(builder.getHttpResponseInterceptor())
{
}

std::shared_ptr<IHTTPClientConfiguration> HTTPClientConfiguration::from(
	std::shared_ptr<core::configuration::IOpenKitConfiguration> openKitConfig
)
{
	if (openKitConfig == nullptr)
	{
		return nullptr;
	}

	return HTTPClientConfiguration::Builder(openKitConfig).build();
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

std::shared_ptr<openkit::IHttpRequestInterceptor> HTTPClientConfiguration::getHttpRequestInterceptor() const
{
	return mHttpRequestInterceptor;
}

std::shared_ptr<openkit::IHttpResponseInterceptor> HTTPClientConfiguration::getHttpResponseInterceptor() const
{
	return mHttpResponseInterceptor;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Builder implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HTTPClientConfiguration::Builder::Builder()
 : mBaseURL("")
 , mServerID(-1)
 , mApplicationID("")
 , mTrustManager(nullptr)
 , mHttpRequestInterceptor(nullptr)
 , mHttpResponseInterceptor(nullptr)
{
}

HTTPClientConfiguration::Builder::Builder(std::shared_ptr<IOpenKitConfiguration> openKitConfig)
	: mBaseURL(openKitConfig->getEndpointUrl())
	, mServerID(openKitConfig->getDefaultServerId())
	, mApplicationID(openKitConfig->getApplicationId())
	, mTrustManager(openKitConfig->getTrustManager())
	, mHttpRequestInterceptor(openKitConfig->getHttpRequestInterceptor())
	, mHttpResponseInterceptor(openKitConfig->getHttpResponseInterceptor())
{
}

HTTPClientConfiguration::Builder::Builder(std::shared_ptr<IHTTPClientConfiguration> httpClientConfig)
	: mBaseURL(httpClientConfig->getBaseURL())
	, mServerID(httpClientConfig->getServerID())
	, mApplicationID(httpClientConfig->getApplicationID())
	, mTrustManager(httpClientConfig->getSSLTrustManager())
	, mHttpRequestInterceptor(httpClientConfig->getHttpRequestInterceptor())
	, mHttpResponseInterceptor(httpClientConfig->getHttpResponseInterceptor())
{
}

HTTPClientConfiguration::Builder& HTTPClientConfiguration::Builder::withBaseURL(
	const core::UTF8String& baseURL
)
{
	mBaseURL = baseURL;
	return *this;
}

const core::UTF8String& HTTPClientConfiguration::Builder::getBaseURL() const
{
	return mBaseURL;
}

HTTPClientConfiguration::Builder& HTTPClientConfiguration::Builder::withServerID(int32_t serverID)
{
	mServerID = serverID;
	return *this;
}

int32_t HTTPClientConfiguration::Builder::getServerID() const
{
	return mServerID;
}

HTTPClientConfiguration::Builder& HTTPClientConfiguration::Builder::withApplicationID(
	const core::UTF8String& applicationID
)
{
	mApplicationID = applicationID;
	return *this;
}

const core::UTF8String& HTTPClientConfiguration::Builder::getApplicationID() const
{
	return mApplicationID;
}

HTTPClientConfiguration::Builder& HTTPClientConfiguration::Builder::withTrustManager(
	std::shared_ptr<openkit::ISSLTrustManager> trustManager
)
{
	mTrustManager = trustManager;
	return *this;
}

std::shared_ptr<openkit::ISSLTrustManager> HTTPClientConfiguration::Builder::getTrustManager() const
{
	return mTrustManager;
}

HTTPClientConfiguration::Builder& HTTPClientConfiguration::Builder::withHttpRequestInterceptor(
	std::shared_ptr<openkit::IHttpRequestInterceptor> httpRequestInterceptor
)
{
	mHttpRequestInterceptor = httpRequestInterceptor;
	return *this;
}

std::shared_ptr<openkit::IHttpRequestInterceptor> HTTPClientConfiguration::Builder::getHttpRequestInterceptor() const
{
	return mHttpRequestInterceptor;
}

HTTPClientConfiguration::Builder& HTTPClientConfiguration::Builder::withHttpResponseInterceptor(
	std::shared_ptr<openkit::IHttpResponseInterceptor> httpResponseInterceptor
)
{
	mHttpResponseInterceptor = httpResponseInterceptor;
	return *this;
}

std::shared_ptr<openkit::IHttpResponseInterceptor>HTTPClientConfiguration::Builder::getHttpResponseInterceptor() const
{
	return mHttpResponseInterceptor;
}

std::shared_ptr<IHTTPClientConfiguration> HTTPClientConfiguration::Builder::build()
{
	return std::make_shared<HTTPClientConfiguration>(*this);
}
