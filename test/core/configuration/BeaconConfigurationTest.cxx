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

#include "mock/MockIOpenKitConfiguration.h"
#include "mock/MockIPrivacyConfiguration.h"
#include "mock/MockIServerConfiguration.h"
#include "../../api/mock/MockISslTrustManager.h"

#include "core/UTF8String.h"
#include "core/configuration/BeaconConfiguration.h"
#include "core/configuration/ServerConfiguration.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <memory>
#include <string>

using namespace test;

using BeaconConfiguration_t = core::configuration::BeaconConfiguration;
using IServerConfiguration_sp = std::shared_ptr<core::configuration::IServerConfiguration>;
using BeaconConfiguration_sp = std::shared_ptr<BeaconConfiguration_t>;
using MockIOpenKitConfiguration_sp = std::shared_ptr<MockIOpenKitConfiguration>;
using MockIPrivacyConfiguration_sp = std::shared_ptr<MockIPrivacyConfiguration>;
using MockIServerConfiguration_sp = std::shared_ptr<MockIServerConfiguration>;
using ServerConfiguration_t = core::configuration::ServerConfiguration;
using Utf8String_t = core::UTF8String;

constexpr int32_t SERVER_ID = 1;

class BeaconConfigurationTest : public testing::Test
{
protected:

	MockIOpenKitConfiguration_sp mockOpenKitConfig;
	MockIPrivacyConfiguration_sp mockPrivacyConfig;

	void SetUp() override
	{
		mockOpenKitConfig = MockIOpenKitConfiguration::createNice();
		mockPrivacyConfig = MockIPrivacyConfiguration::createNice();
	}

	MockIServerConfiguration_sp mockServerConfig(bool enableCapture)
	{
		auto serverConfig = MockIServerConfiguration::createNice();
		ON_CALL(*serverConfig, isCaptureEnabled()).WillByDefault(testing::Return(enableCapture));
		ON_CALL(*serverConfig, isCrashReportingEnabled()).WillByDefault(testing::Return(true));
		ON_CALL(*serverConfig, isErrorReportingEnabled()).WillByDefault(testing::Return(true));
		ON_CALL(*serverConfig, getServerId()).WillByDefault(testing::Return(73));
		ON_CALL(*serverConfig, getBeaconSizeInBytes()).WillByDefault(testing::Return(1024));
		ON_CALL(*serverConfig, getMultiplicity()).WillByDefault(testing::Return(37));

		return serverConfig;
	}

	BeaconConfiguration_sp createBeaconConfig()
	{
		return std::make_shared<BeaconConfiguration_t>(mockOpenKitConfig, mockPrivacyConfig, SERVER_ID);
	}
};

TEST_F(BeaconConfigurationTest, fromWithNullOpenKitConfigurationGivesNull)
{
	// when, then
	ASSERT_THAT(BeaconConfiguration_t::from(nullptr, mockPrivacyConfig, SERVER_ID), testing::IsNull());
}

TEST_F(BeaconConfigurationTest, fromWithNullPrivacyConfigurationGivesNull)
{
	// when, then
	ASSERT_THAT(BeaconConfiguration_t::from(mockOpenKitConfig, nullptr, SERVER_ID), testing::IsNull());
}

TEST_F(BeaconConfigurationTest, fromWithNunNullArgumentsGivesNonNullBeaconConfiguration)
{
	// when, then
	ASSERT_THAT(createBeaconConfig(), testing::NotNull());
}

TEST_F(BeaconConfigurationTest, getOpenKitConfigurationReturnsPassedObject)
{
	// given
	auto target = createBeaconConfig();

	// when
	auto obtained = target->getOpenKitConfiguration();

	// then
	ASSERT_THAT(obtained, testing::Eq(mockOpenKitConfig));
}

TEST_F(BeaconConfigurationTest, getPrivacyConfigurationReturnsPassedObject)
{
	// given
	auto target = createBeaconConfig();

	// when
	auto obtained = target->getPrivacyConfiguration();

	// then
	ASSERT_THAT(obtained, testing::Eq(mockPrivacyConfig));
}

TEST_F(BeaconConfigurationTest, newInstanceReturnsDefaultServerConfiguration)
{
	// given
	auto target = createBeaconConfig();

	// when
	auto obtained = target->getServerConfiguration();

	// then
	ASSERT_THAT(obtained, testing::Eq(ServerConfiguration_t::defaultInstance()));
}

TEST_F(BeaconConfigurationTest, newInstanceReturnsIsServerConfigurationSetFalse)
{
	// given
	auto target = createBeaconConfig();

	// when
	auto obtained = target->isServerConfigurationSet();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(BeaconConfigurationTest, newInstanceReturnsHttpClientConfigWithGivenServerId)
{
	// given
	const Utf8String_t endpointUrl("https://localhost:9999/1");
	const Utf8String_t applicationId("some cryptic appId");
	auto trustManager = MockISslTrustManager::createStrict();
	const int32_t serverId = 73;
	ON_CALL(*mockOpenKitConfig, getEndpointUrl()).WillByDefault(testing::ReturnRef(endpointUrl));
	ON_CALL(*mockOpenKitConfig, getApplicationId()).WillByDefault(testing::ReturnRef(applicationId));
	ON_CALL(*mockOpenKitConfig, getTrustManager()).WillByDefault(testing::Return(trustManager));
	ON_CALL(*mockOpenKitConfig, getDefaultServerId()).WillByDefault(testing::Return(serverId));

	auto target = BeaconConfiguration_t::from(mockOpenKitConfig, mockPrivacyConfig, serverId);

	// when
	auto obtained = target->getHTTPClientConfiguration();

	// then
	ASSERT_THAT(obtained->getBaseURL(), testing::Eq(endpointUrl));
	ASSERT_THAT(obtained->getApplicationID(), testing::Eq(applicationId));
	ASSERT_THAT(obtained->getSSLTrustManager(), testing::Eq(trustManager));
	ASSERT_THAT(obtained->getServerID(), testing::Eq(serverId));
}

TEST_F(BeaconConfigurationTest, initializeServerConfigurationDoesNotSetIsServerConfigurationSet)
{
	// given
	auto serverConfig = MockIServerConfiguration::createStrict(); // expect no calls
	auto target = createBeaconConfig();

	// when
	target->initializeServerConfiguration(serverConfig);

	// then
	ASSERT_THAT(target->isServerConfigurationSet(), testing::Eq(false));
}

TEST_F(BeaconConfigurationTest, initializeServerConfigurationSetsServerConfiguration)
{
	// given
	auto serverConfig = MockIServerConfiguration::createStrict(); // expect no calls
	auto target = createBeaconConfig();

	// when
	target->initializeServerConfiguration(serverConfig);

	// then
	ASSERT_THAT(target->getServerConfiguration(), testing::Eq(serverConfig));
}

TEST_F(BeaconConfigurationTest, initializeServerConfigurationWithNullConfigurationDoesNothing)
{
	// given
	auto updateServerConfigurationCallbackCalled = false;
	auto updateServerConfigurationCallback = [&updateServerConfigurationCallbackCalled](IServerConfiguration_sp /* updatedServerConfiguration */) {
		updateServerConfigurationCallbackCalled = true;
	};

	auto target = createBeaconConfig();
	target->setServerConfigurationUpdateCallback(updateServerConfigurationCallback);

	// when
	target->initializeServerConfiguration(nullptr);

	// then
	ASSERT_THAT(updateServerConfigurationCallbackCalled, testing::Eq(false));
	ASSERT_THAT(target->getServerConfiguration(), testing::Eq(ServerConfiguration_t::defaultInstance()));
}

TEST_F(BeaconConfigurationTest, initializeServerConfigurationWithDefaultConfigurationDoesNothing)
{
	// given
	auto updateServerConfigurationCallbackCalled = false;
	auto updateServerConfigurationCallback = [&updateServerConfigurationCallbackCalled](IServerConfiguration_sp /* updatedServerConfiguration */) {
		updateServerConfigurationCallbackCalled = true;
	};

	auto target = createBeaconConfig();
	target->setServerConfigurationUpdateCallback(updateServerConfigurationCallback);

	// when
	target->initializeServerConfiguration(ServerConfiguration_t::defaultInstance());

	// then
	ASSERT_THAT(updateServerConfigurationCallbackCalled, testing::Eq(false));
}

TEST_F(BeaconConfigurationTest, initializeServerConfigurationDoesNotSetServerConfigurationIfAlreadySet)
{
	// given
	auto updatedServerConfig = MockIServerConfiguration::createNice();
	auto initialServerConfig = MockIServerConfiguration::createNice();

	auto target = createBeaconConfig();
	target->updateServerConfiguration(updatedServerConfig);
	ASSERT_THAT(target->isServerConfigurationSet(), testing::Eq(true));

	// when
	target->initializeServerConfiguration(initialServerConfig);

	// then
	ASSERT_THAT(target->getServerConfiguration(), testing::Eq(updatedServerConfig));
}

TEST_F(BeaconConfigurationTest, initializeServerConfigurationDoesInvokeCallbackIfCallbackIsSet)
{
	// given
	auto serverConfig = MockIServerConfiguration::createNice();
	auto updateServerConfigurationCallbackCalled = false;
	auto updateServerConfigurationCallback = [&updateServerConfigurationCallbackCalled](IServerConfiguration_sp /* updatedServerConfiguration */) {
		updateServerConfigurationCallbackCalled = true;
	};

	auto target = createBeaconConfig();
	target->setServerConfigurationUpdateCallback(updateServerConfigurationCallback);

	// when
	target->initializeServerConfiguration(serverConfig);

	// then
	ASSERT_THAT(updateServerConfigurationCallbackCalled, testing::Eq(true));
}

TEST_F(BeaconConfigurationTest, updateServerConfigurationSetsIsServerConfigurationSet)
{
	// given
	auto serverConfig = MockIServerConfiguration::createStrict();
	auto target = createBeaconConfig();

	// when
	target->updateServerConfiguration(serverConfig);

	// then
	ASSERT_THAT(target->isServerConfigurationSet(), testing::Eq(true));
}

TEST_F(BeaconConfigurationTest, updateServerConfigurationTakesOverServerConfigurationIfNotSet)
{
	// given
	auto serverConfig = MockIServerConfiguration::createStrict();
	auto target = createBeaconConfig();

	// when
	target->updateServerConfiguration(serverConfig);

	// then
	ASSERT_THAT(target->getServerConfiguration(), testing::Eq(serverConfig));
}

TEST_F(BeaconConfigurationTest, updateServerConfigurationMergesServerConfigIfAlreadySet)
{
	// with
	auto serverConfig1 = MockIServerConfiguration::createStrict();
	auto serverConfig2 = MockIServerConfiguration::createStrict();

	// expect
	EXPECT_CALL(*serverConfig1, merge(testing::Eq(serverConfig2)))
		.Times(1)
		.WillOnce(testing::Return(serverConfig2));

	// given
	auto target = createBeaconConfig();
	target->updateServerConfiguration(serverConfig1);

	// when
	target->updateServerConfiguration(serverConfig2);

	// then
	ASSERT_THAT(target->getServerConfiguration(), testing::Eq(serverConfig2));
}

TEST_F(BeaconConfigurationTest, updateServerConfigurationDoesNotUpdateHttpClientConfig)
{
	// given
	const int32_t serverId = 73;
	auto serverConfig = MockIServerConfiguration::createStrict();
	ON_CALL(*serverConfig, getServerId()).WillByDefault(testing::Return(serverId));

	auto target = createBeaconConfig();
	auto httpConfig = target->getHTTPClientConfiguration();

	// when
	target->updateServerConfiguration(serverConfig);
	auto obtained = target->getHTTPClientConfiguration();

	// then
	ASSERT_THAT(obtained->getServerID(), testing::Eq(SERVER_ID));
	ASSERT_THAT(obtained, testing::Eq(httpConfig));
}

TEST_F(BeaconConfigurationTest, updateServerConfigurationDoesNotUpdateHttpClientConfigurationIfServerIdEquals)
{
	// given
	const int32_t serverId = 73;
	auto serverConfig = MockIServerConfiguration::createStrict();
	ON_CALL(*serverConfig, getServerId()).WillByDefault(testing::Return(serverId));
	ON_CALL(*mockOpenKitConfig, getDefaultServerId()).WillByDefault(testing::Return(serverId));

	auto target = createBeaconConfig();
	auto httpConfig = target->getHTTPClientConfiguration();

	// when
	target->updateServerConfiguration(serverConfig);
	auto obtained = target->getHTTPClientConfiguration();

	// then
	ASSERT_THAT(obtained, testing::Eq(httpConfig));
}

TEST_F(BeaconConfigurationTest, updateServerConfigurationWithNullDoesNothing)
{
	// given
	auto target = createBeaconConfig();

	// when
	target->updateServerConfiguration(nullptr);

	// then
	ASSERT_THAT(target->isServerConfigurationSet(), testing::Eq(false));
	ASSERT_THAT(target->getServerConfiguration(), testing::Eq(ServerConfiguration_t::defaultInstance()));
}

TEST_F(BeaconConfigurationTest, updateServerConfigurationDoesInvokeCallbackIfCallbackIsSet)
{
	// given
	auto serverConfig = MockIServerConfiguration::createNice();
	IServerConfiguration_sp capturedServerConfiguration = nullptr;
	auto updateServerConfigurationCallback = [&capturedServerConfiguration](IServerConfiguration_sp updatedServerConfiguration) {
		capturedServerConfiguration = updatedServerConfiguration;
	};

	auto target = createBeaconConfig();
	target->setServerConfigurationUpdateCallback(updateServerConfigurationCallback);

	// when
	target->updateServerConfiguration(serverConfig);

	// then
	ASSERT_THAT(capturedServerConfiguration, testing::Eq(serverConfig));
}

TEST_F(BeaconConfigurationTest, updateServerConfigurationDoesNotInvokeCallbackIfNoCallbackIsSet)
{
	// given
	auto serverConfig = MockIServerConfiguration::createNice();
	IServerConfiguration_sp capturedServerConfiguration = nullptr;
	auto updateServerConfigurationCallback = [&capturedServerConfiguration](IServerConfiguration_sp updatedServerConfiguration) {
		capturedServerConfiguration = updatedServerConfiguration;
	};

	auto target = createBeaconConfig();
	target->setServerConfigurationUpdateCallback(updateServerConfigurationCallback);
	target->setServerConfigurationUpdateCallback(nullptr);

	// when
	target->updateServerConfiguration(serverConfig);

	// then
	ASSERT_THAT(capturedServerConfiguration, testing::Eq(nullptr));
}

TEST_F(BeaconConfigurationTest, enableCaptureSetsIsConfigurationSet)
{
	// given
	auto target = createBeaconConfig();

	// when
	target->enableCapture();

	// then
	ASSERT_THAT(target->isServerConfigurationSet(), testing::Eq(true));
}

TEST_F(BeaconConfigurationTest, enableCaptureUpdatesServerConfigIfCaptureIsDisabled)
{
	// given
	auto initialServerConfig = MockIServerConfiguration::createNice();
	ON_CALL(*initialServerConfig, isCaptureEnabled()).WillByDefault(testing::Return(false));

	auto target = createBeaconConfig();
	target->updateServerConfiguration(initialServerConfig);

	// when
	target->enableCapture();
	auto obtained = target->getServerConfiguration();

	// then
	ASSERT_THAT(obtained, testing::Ne(initialServerConfig));
	ASSERT_THAT(obtained, testing::Ne(ServerConfiguration_t::defaultInstance()));
	ASSERT_THAT(obtained->isCaptureEnabled(), testing::Eq(true));
}

TEST_F(BeaconConfigurationTest, enableCaptureDoesOnlyModifyCaptureFlag)
{
	// given
	auto initialServerConfig = mockServerConfig(false);

	auto target = createBeaconConfig();
	target->updateServerConfiguration(initialServerConfig);

	// when
	target->enableCapture();
	auto obtained = target->getServerConfiguration();

	// then
	ASSERT_THAT(obtained, testing::Ne(initialServerConfig));
	ASSERT_THAT(obtained->isCaptureEnabled(), testing::Ne(initialServerConfig->isCaptureEnabled()));
	ASSERT_THAT(obtained->isCrashReportingEnabled(), testing::Eq(initialServerConfig->isCrashReportingEnabled()));
	ASSERT_THAT(obtained->isErrorReportingEnabled(), testing::Eq(initialServerConfig->isErrorReportingEnabled()));
	ASSERT_THAT(obtained->getServerId(), testing::Eq(initialServerConfig->getServerId()));
	ASSERT_THAT(obtained->getBeaconSizeInBytes(), testing::Eq(initialServerConfig->getBeaconSizeInBytes()));
	ASSERT_THAT(obtained->getMultiplicity(), testing::Eq(initialServerConfig->getMultiplicity()));
}

TEST_F(BeaconConfigurationTest, disableCaptureSetsIsServerConfigurationSet)
{
	// given
	auto target = createBeaconConfig();

	// when
	target->disableCapture();

	// then
	ASSERT_THAT(target->isServerConfigurationSet(), testing::Eq(true));
}

TEST_F(BeaconConfigurationTest, disableCaptureUpdatesServerConfigIfCaptureGetsDisabled)
{
	// given
	auto initialServerConfig = MockIServerConfiguration::createNice();
	ON_CALL(*initialServerConfig, isCaptureEnabled()).WillByDefault(testing::Return(true));

	auto target = createBeaconConfig();
	target->updateServerConfiguration(initialServerConfig);

	// when
	target->disableCapture();
	auto obtained = target->getServerConfiguration();

	// then
	ASSERT_THAT(obtained, testing::Ne(initialServerConfig));
	ASSERT_THAT(obtained, testing::Ne(ServerConfiguration_t::defaultInstance()));
	ASSERT_THAT(obtained->isCaptureEnabled(), testing::Eq(false));
}

TEST_F(BeaconConfigurationTest, disableCaptureDoesOnlyModifyCaptureFlag)
{
	// given
	auto initialServerConfig = mockServerConfig(true);

	auto target = createBeaconConfig();
	target->updateServerConfiguration(initialServerConfig);

	// when
	target->disableCapture();
	auto obtained = target->getServerConfiguration();

	// then
	ASSERT_THAT(obtained, testing::Ne(initialServerConfig));
	ASSERT_THAT(obtained->isCaptureEnabled(), testing::Ne(initialServerConfig->isCaptureEnabled()));
	ASSERT_THAT(obtained->isCrashReportingEnabled(), testing::Eq(initialServerConfig->isCrashReportingEnabled()));
	ASSERT_THAT(obtained->isErrorReportingEnabled(), testing::Eq(initialServerConfig->isCrashReportingEnabled()));
	ASSERT_THAT(obtained->getServerId(), testing::Eq(initialServerConfig->getServerId()));
	ASSERT_THAT(obtained->getBeaconSizeInBytes(), testing::Eq(initialServerConfig->getBeaconSizeInBytes()));
	ASSERT_THAT(obtained->getMultiplicity(), testing::Eq(initialServerConfig->getMultiplicity()));
}
