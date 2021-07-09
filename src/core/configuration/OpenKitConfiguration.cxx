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

#include "OpenKitConfiguration.h"
#include "core/util/URLEncoding.h"

using namespace core::configuration;

OpenKitConfiguration::OpenKitConfiguration(const openkit::IOpenKitBuilder& builder)
	: mEndpointUrl(builder.getEndpointURL())
	, mDeviceId(builder.getDeviceID())
	, mOrigDeviceId(builder.getOrigDeviceID())
	, mOpenKitType(builder.getOpenKitType())
	, mApplicationId(builder.getApplicationID())
	, mPercentEncodedApplicationId(core::util::URLEncoding::urlencode(builder.getApplicationID(), {'_'}))
	, mApplicationName(builder.getApplicationName())
	, mApplicationVersion(builder.getApplicationVersion())
	, mOperatingSystem(builder.getOperatingSystem())
	, mManufacturer(builder.getManufacturer())
	, mModelId(builder.getModelID())
	, mDefaultServerId(builder.getDefaultServerID())
	, mTrustManager(builder.getTrustManager())
	, mHttpRequestInterceptor(builder.getHttpRequestInterceptor())
	, mHttpResponseInterceptor(builder.getHttpResponseInterceptor())
{
}

std::shared_ptr<core::configuration::IOpenKitConfiguration> OpenKitConfiguration::from(
	const openkit::IOpenKitBuilder& builder
)
{
	return std::make_shared<OpenKitConfiguration>(builder);
}

const core::UTF8String& OpenKitConfiguration::getEndpointUrl() const
{
	return mEndpointUrl;
}

int64_t OpenKitConfiguration::getDeviceId() const
{
	return mDeviceId;
}

const core::UTF8String& OpenKitConfiguration::getOrigDeviceId() const
{
	return mOrigDeviceId;
}

const std::string& OpenKitConfiguration::getOpenKitType() const
{
	return mOpenKitType;
}

const core::UTF8String& OpenKitConfiguration::getApplicationId() const
{
	return mApplicationId;
}

const core::UTF8String& OpenKitConfiguration::getApplicationIdPercentEncoded() const
{
	return mPercentEncodedApplicationId;
}

const core::UTF8String& OpenKitConfiguration::getApplicationName() const
{
	return mApplicationName;
}

const core::UTF8String& OpenKitConfiguration::getApplicationVersion() const
{
	return mApplicationVersion;
}

const core::UTF8String& OpenKitConfiguration::getOperatingSystem() const
{
	return mOperatingSystem;
}

const core::UTF8String& OpenKitConfiguration::getManufacturer() const
{
	return mManufacturer;
}

const core::UTF8String& OpenKitConfiguration::getModelId() const
{
	return mModelId;
}

int32_t OpenKitConfiguration::getDefaultServerId() const
{
	return mDefaultServerId;
}

std::shared_ptr<openkit::ISSLTrustManager> OpenKitConfiguration::getTrustManager() const
{
	return mTrustManager;
}


std::shared_ptr<openkit::IHttpRequestInterceptor> OpenKitConfiguration::getHttpRequestInterceptor() const
{
	return mHttpRequestInterceptor;
}

std::shared_ptr<openkit::IHttpResponseInterceptor> OpenKitConfiguration::getHttpResponseInterceptor() const
{
	return mHttpResponseInterceptor;
}
