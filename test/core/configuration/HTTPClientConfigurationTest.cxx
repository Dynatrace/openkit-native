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

#include "mock/MockIHTTPClientConfiguration.h"
#include "mock/MockIOpenKitConfiguration.h"
#include "../../api/mock/MockISslTrustManager.h"
#include "../../api/mock/MockIHttpRequestInterceptor.h"
#include "../../api/mock/MockIHttpResponseInterceptor.h"

#include "core/UTF8String.h"
#include "core/configuration/HTTPClientConfiguration.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace test;

using HTTPClientConfiguration_t = core::configuration::HTTPClientConfiguration;
using Utf8String_t = core::UTF8String;

class HTTPClientConfigurationTest : public testing::Test
{
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// create instance from OpenKitConfiguration
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(HTTPClientConfigurationTest, instanceFromOpenKitConifigTakesOverEndpointUrl)
{
	// with
	const Utf8String_t endpointUrl("https://localhost:9999/1");
	auto openKitConfig = MockIOpenKitConfiguration::createNice();

	// expect
	EXPECT_CALL(*openKitConfig, getEndpointUrl())
		.Times((1))
		.WillOnce(testing::ReturnRef(endpointUrl));

	// given
	auto target = HTTPClientConfiguration_t::Builder(openKitConfig).build();

	// when
	auto obtained = target->getBaseURL();

	// then
	ASSERT_THAT(obtained, testing::Eq(endpointUrl));
}

TEST_F(HTTPClientConfigurationTest, instanceFromOpenKitConifigTakesOverApplicationId)
{
	// with
	const Utf8String_t applicationId("some cryptic appID");
	auto openKitConfig = MockIOpenKitConfiguration::createNice();

	// expect
	EXPECT_CALL(*openKitConfig, getApplicationId())
		.Times((1))
		.WillOnce(testing::ReturnRef(applicationId));

	// given
	auto target = HTTPClientConfiguration_t::Builder(openKitConfig).build();

	// when
	auto obtained = target->getApplicationID();

	// then
	ASSERT_THAT(obtained, testing::Eq(applicationId));
}

TEST_F(HTTPClientConfigurationTest, instanceFromOpenKitConifigTakesOverTrustManager)
{
	// with
	auto trustManager = MockISslTrustManager::createNice();
	auto openKitConfig = MockIOpenKitConfiguration::createNice();

	// expect
	EXPECT_CALL(*openKitConfig, getTrustManager())
		.Times((1))
		.WillOnce(testing::Return(trustManager));

	// given
	auto target = HTTPClientConfiguration_t::Builder(openKitConfig).build();

	// when
	auto obtained = target->getSSLTrustManager();

	// then
	ASSERT_THAT(obtained, testing::Eq(trustManager));
}

TEST_F(HTTPClientConfigurationTest, instanceFromOpenKitConifigTakesOverDefaultServerId)
{
	// with
	const int32_t defaultServerId = 1;
	auto openKitConfig = MockIOpenKitConfiguration::createNice();

	// expect
	EXPECT_CALL(*openKitConfig, getDefaultServerId())
		.Times((1))
		.WillOnce(testing::Return(defaultServerId));

	// given
	auto target = HTTPClientConfiguration_t::Builder(openKitConfig).build();

	// when
	auto obtained = target->getServerID();

	// then
	ASSERT_THAT(obtained, testing::Eq(defaultServerId));
}

TEST_F(HTTPClientConfigurationTest, instanceFromOpenKitConfigTakesOverHttpRequestInterceptor)
{
	// with
	auto httpRequestInterceptor = MockIHttpRequestInterceptor::createNice();
	auto openKitConfig = MockIOpenKitConfiguration::createNice();
	
	// expect
	EXPECT_CALL(*openKitConfig, getHttpRequestInterceptor())
		.Times(1)
		.WillOnce(testing::Return(httpRequestInterceptor));

	// given
	auto target = HTTPClientConfiguration_t::Builder(openKitConfig).build();

	// when
	auto obtained = target->getHttpRequestInterceptor();

	// then
	ASSERT_THAT(obtained, testing::Eq(httpRequestInterceptor));
}

TEST_F(HTTPClientConfigurationTest, instanceFromOpenKitConfigTakesOverHttpResponseInterceptor)
{
	// with
	auto httpResponseInterceptor = MockIHttpResponseInterceptor::createNice();
	auto openKitConfig = MockIOpenKitConfiguration::createNice();

	// expect
	EXPECT_CALL(*openKitConfig, getHttpResponseInterceptor())
		.Times(1)
		.WillOnce(testing::Return(httpResponseInterceptor));

	// given
	auto target = HTTPClientConfiguration_t::Builder(openKitConfig).build();

	// when
	auto obtained = target->getHttpResponseInterceptor();

	// then
	ASSERT_THAT(obtained, testing::Eq(httpResponseInterceptor));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Create builder instance from HTTPClientConfiguration
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(HTTPClientConfigurationTest, builderFromHTTPClientConfigTakesOverBaseUrl)
{
	// with
	Utf8String_t baseUrl("https://localhost:9999/1");
	auto httpConfig = MockIHTTPClientConfiguration::createNice();

	// expect
	EXPECT_CALL(*httpConfig, getBaseURL())
		.Times(1)
		.WillOnce(testing::ReturnRef(baseUrl));

	// given, when
	auto target = HTTPClientConfiguration_t::Builder(httpConfig).build();

	// then
	ASSERT_THAT(target->getBaseURL(), testing::Eq(baseUrl));
}

TEST_F(HTTPClientConfigurationTest, builderFromHTTPClientConfigTakesOverApplicationId)
{
	// with
	Utf8String_t applicationId("some cryptic appId");
	auto httpConfig = MockIHTTPClientConfiguration::createNice();

	// expect
	EXPECT_CALL(*httpConfig, getApplicationID())
		.Times(1)
		.WillOnce(testing::ReturnRef(applicationId));

	// given, when
	auto target = HTTPClientConfiguration_t::Builder(httpConfig).build();

	// then
	ASSERT_THAT(target->getApplicationID(), testing::Eq(applicationId));
}

TEST_F(HTTPClientConfigurationTest, builderFromHTTPClientConfigTakesOverTrustManager)
{
	// with
	auto trustManager = MockISslTrustManager::createNice();
	auto httpConfig = MockIHTTPClientConfiguration::createNice();

	// expect
	EXPECT_CALL(*httpConfig, getSSLTrustManager())
		.Times(1)
		.WillOnce(testing::Return(trustManager));

	// given, when
	auto target = HTTPClientConfiguration_t::Builder(httpConfig).build();

	// then
	ASSERT_THAT(target->getSSLTrustManager(), testing::Eq(trustManager));
}

TEST_F(HTTPClientConfigurationTest, builderFromHTTPClientConfigTakesOverServerId)
{
	// with
	const int32_t serverId = 1;
	auto httpConfig = MockIHTTPClientConfiguration::createNice();

	// expect
	EXPECT_CALL(*httpConfig, getServerID())
		.Times(1)
		.WillOnce(testing::Return(serverId));

	// given, when
	auto target = HTTPClientConfiguration_t::Builder(httpConfig).build();

	// then
	ASSERT_THAT(target->getServerID(), testing::Eq(serverId));
}

TEST_F(HTTPClientConfigurationTest, builderFromHttpClientConfigTakesHttpRequestInterceptor)
{
	// with
	auto httpRequestInterceptor = MockIHttpRequestInterceptor::createNice();
	auto httpConfig = MockIHTTPClientConfiguration::createNice();

	// expect
	EXPECT_CALL(*httpConfig, getHttpRequestInterceptor())
		.Times(1)
		.WillOnce(testing::Return(httpRequestInterceptor));
	
	// given, when
	auto target = HTTPClientConfiguration_t::Builder(httpConfig).build();

	// then
	ASSERT_THAT(target->getHttpRequestInterceptor(), testing::Eq(httpRequestInterceptor));
}

TEST_F(HTTPClientConfigurationTest, builderFromHttpClientConfigTakesHttpResponseInterceptor)
{
	// with
	auto httpResponseInterceptor = MockIHttpResponseInterceptor::createNice();
	auto httpConfig = MockIHTTPClientConfiguration::createNice();

	// expect
	EXPECT_CALL(*httpConfig, getHttpResponseInterceptor())
		.Times(1)
		.WillOnce(testing::Return(httpResponseInterceptor));

	// given, when
	auto target = HTTPClientConfiguration_t::Builder(httpConfig).build();

	// then
	ASSERT_THAT(target->getHttpResponseInterceptor(), testing::Eq(httpResponseInterceptor));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Create instance from not initialized builder
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(HTTPClientConfigurationTest, emptyBuilderCreatesEmptyInstance)
{
	// given
	auto target = HTTPClientConfiguration_t::Builder();

	// when
	auto obtained = target.build();

	// then
	ASSERT_THAT(obtained->getBaseURL(), testing::Eq(""));
	ASSERT_THAT(obtained->getApplicationID(), testing::Eq(""));
	ASSERT_THAT(obtained->getSSLTrustManager(), testing::IsNull());
	ASSERT_THAT(obtained->getServerID(), testing::Eq(-1));
}

TEST_F(HTTPClientConfigurationTest, builderWithBaseUrlPropagatesToInstance)
{
	// given
	const Utf8String_t baseUrl("https://localhost:9999/1");
	auto target = HTTPClientConfiguration_t::Builder().withBaseURL(baseUrl);

	// when
	auto obtained = target.build();

	// then
	ASSERT_THAT(obtained->getBaseURL(), testing::Eq(baseUrl));
}

TEST_F(HTTPClientConfigurationTest, builderWithApplicationIdPropagatesToInstance)
{
	// given
	const Utf8String_t applicationId("some cryptic applicationId");
	auto target = HTTPClientConfiguration_t::Builder().withApplicationID(applicationId);

	// when
	auto obtained = target.build();

	// then
	ASSERT_THAT(obtained->getApplicationID(), testing::Eq(applicationId));
}

TEST_F(HTTPClientConfigurationTest, builderWithTrustManagerPropagatesToInstance)
{
	// given
	auto trustManager = MockISslTrustManager::createNice();
	auto target = HTTPClientConfiguration_t::Builder().withTrustManager(trustManager);

	// when
	auto obtained = target.build();

	// then
	ASSERT_THAT(obtained->getSSLTrustManager(), testing::Eq(trustManager));
}

TEST_F(HTTPClientConfigurationTest, builderWithServerIdPropagatesToInstance)
{
	// given
	const int32_t serverId = 1;
	auto target = HTTPClientConfiguration_t::Builder().withServerID(serverId);

	// when
	auto obtained = target.build();

	// then
	ASSERT_THAT(obtained->getServerID(), testing::Eq(serverId));
}

TEST_F(HTTPClientConfigurationTest, builderWithHttpRequestInterceptorPropagatesToInstance)
{
	// given
	auto httpRequestInterceptor = MockIHttpRequestInterceptor::createNice();
	auto target = HTTPClientConfiguration_t::Builder()
		.withHttpRequestInterceptor(httpRequestInterceptor);

	// when
	auto obtained = target.build();

	// then
	ASSERT_THAT(obtained->getHttpRequestInterceptor(), testing::Eq(httpRequestInterceptor));
}

TEST_F(HTTPClientConfigurationTest, builderWithHttpResponseInterceptorPropagatesToInstance)
{
	// given
	auto httpResponseInterceptor = MockIHttpResponseInterceptor::createNice();
	auto target = HTTPClientConfiguration_t::Builder()
		.withHttpResponseInterceptor(httpResponseInterceptor);

	// when
	auto obtained = target.build();

	// then
	ASSERT_THAT(obtained->getHttpResponseInterceptor(), testing::Eq(httpResponseInterceptor));
}