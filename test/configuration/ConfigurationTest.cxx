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

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "configuration/Configuration.h"
#include "providers/DefaultSessionIDProvider.h"
#include "protocol/ssl/SSLStrictTrustManager.h"

#include "../protocol/MockStatusResponse.h"


using namespace configuration;

class ConfigurationTest : public testing::Test
{
public:
	void SetUp()
	{
		sslTrustManager = std::shared_ptr<openkit::ISSLTrustManager>(new protocol::SSLStrictTrustManager());
		sessionIDProvider = std::shared_ptr<providers::ISessionIDProvider>(new providers::DefaultSessionIDProvider());
		device = std::shared_ptr<Device>(new Device("", "", ""));
		beaconCacheConfiguration = std::make_shared<configuration::BeaconCacheConfiguration>(-1, -1, -1);
		beaconConfiguration = std::make_shared<configuration::BeaconConfiguration>();
	}

	std::unique_ptr<configuration::Configuration> getDefaultConfiguration()
	{
		return std::unique_ptr<Configuration>(new Configuration(device, openKitType, "", "", "", 0, "", sessionIDProvider, sslTrustManager, beaconCacheConfiguration, beaconConfiguration));
	}

	std::unique_ptr<configuration::Configuration> getConfiguration(const core::UTF8String& beaconURL)
	{
		return std::unique_ptr<Configuration>(new Configuration(device, openKitType, "", "", "", 0, beaconURL, sessionIDProvider, sslTrustManager, beaconCacheConfiguration, beaconConfiguration));
	}
private:
	std::shared_ptr<Device> device = nullptr;
	OpenKitType openKitType = OpenKitType::Type::DYNATRACE;
	std::shared_ptr<providers::ISessionIDProvider> sessionIDProvider = nullptr;
	std::shared_ptr<openkit::ISSLTrustManager> sslTrustManager = nullptr;
	std::shared_ptr<configuration::BeaconConfiguration> beaconConfiguration = nullptr;
	std::shared_ptr<configuration::BeaconCacheConfiguration> beaconCacheConfiguration = nullptr;

	std::shared_ptr<Configuration> testConfiguration = nullptr;
};

TEST_F(ConfigurationTest, enableAndDisableCapturing)
{
	//given
	auto target = getDefaultConfiguration();
	ASSERT_TRUE(target != nullptr);

	//when
	target->enableCapture();
	//then
	ASSERT_TRUE(target->isCapture());

	//when
	target->disableCapture();
	//then
	ASSERT_FALSE(target->isCapture());
}

TEST_F(ConfigurationTest, capturingIsDisabledIfStatusResponseIsNull)
{
	//given
	auto target = getDefaultConfiguration();
	ASSERT_TRUE(target != nullptr);

	//when status response to handle is nullptr
	target->enableCapture();
	target->updateSettings(nullptr);

	//then
	ASSERT_FALSE(target->isCapture());
}

TEST_F(ConfigurationTest, capturingIsEnabledFromStatusResponse)
{
	//given
	auto mockResponse = std::make_shared<testing::NiceMock<test::MockStatusResponse>>();

	//when
	auto target = getDefaultConfiguration();
	ASSERT_TRUE(target != nullptr);
	target->disableCapture();
	ON_CALL(*mockResponse, isCapture())
		.WillByDefault(testing::Return(true));
	ON_CALL(*mockResponse, getResponseCode())
		.WillByDefault(testing::Return(200));

	target->updateSettings(std::move(mockResponse) );

	//then
	ASSERT_TRUE(target->isCapture());
}

TEST_F(ConfigurationTest, capturingIsDisabledFromStatusResponse)
{
	//given
	auto mockResponse = std::make_shared<testing::NiceMock<test::MockStatusResponse>>();

	//when
	auto target = getDefaultConfiguration();
	ASSERT_TRUE(target != nullptr);
	target->disableCapture();
	ON_CALL(*mockResponse, isCapture())
		.WillByDefault(testing::Return(false));
	ON_CALL(*mockResponse, getResponseCode())
		.WillByDefault(testing::Return(200));

	target->updateSettings(std::move(mockResponse));

	//then
	ASSERT_FALSE(target->isCapture());
}

TEST_F(ConfigurationTest, tenantURLisSetCorrectly)
{
	core::UTF8String host("localhost:9999");

	core::UTF8String tenantURL = core::UTF8String("http://");
	tenantURL.concatenate(host);
	tenantURL.concatenate("/mbeacon");

	auto target = getConfiguration(tenantURL);

	ASSERT_TRUE(tenantURL.equals(target->getHTTPClientConfiguration()->getBaseURL()));
}

TEST_F(ConfigurationTest, defaultDataCollectionLevelIsDefaultValueFromBeaconConfiguration)
{
	auto target = getDefaultConfiguration();
	ASSERT_EQ(target->getBeaconConfiguration()->getDataCollectionLevel(), configuration::BeaconConfiguration::DEFAULT_DATA_COLLECTION_LEVEL);
}

TEST_F(ConfigurationTest, defaultCrashReportingLevelIsDefaultValueFromBeaconConfiguration)
{
	auto target = getDefaultConfiguration();
	ASSERT_EQ(target->getBeaconConfiguration()->getCrashReportingLevel(), configuration::BeaconConfiguration::DEFAULT_CRASH_REPORTING_LEVEL);
}

TEST_F(ConfigurationTest, getApplicationID)
{
	// given
	auto target = getDefaultConfiguration();

	// then
	ASSERT_EQ(target->getApplicationID(), "/App_ID%");
}

TEST_F(ConfigurationTest, getApplicationIDPercentEncodedDoesProperEncoding)
{
	// given
	auto target = getDefaultConfiguration();

	// then
	ASSERT_EQ(target->getApplicationIDPercentEncoded(), "%2FApp%5FID%25");
}