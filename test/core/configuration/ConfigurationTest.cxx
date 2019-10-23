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

#include "Types.h"
#include "../Types.h"
#include "../configuration/Types.h"
#include "../../api/Types.h"
#include "../../providers/Types.h"
#include "../../protocol/Types.h"
#include "../../protocol/mock/MockIStatusResponse.h"

#include "core/configuration/ConfigurationDefaults.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace test;
using namespace test::types;

class ConfigurationTest : public testing::Test
{
protected:
	void SetUp()
	{
		sslTrustManager = std::make_shared<SslStrictTrustManager_t>();
		sessionIDProvider = std::make_shared<DefaultSessionIdProvider_t>();
		device = std::make_shared<Device_t>("", "", "");
		beaconCacheConfiguration = std::make_shared<BeaconCacheConfiguration_t>(-1, -1, -1);
		beaconConfiguration = std::make_shared<BeaconConfiguration_t>();
	}

	std::shared_ptr<Configuration_t> getDefaultConfiguration()
	{
		return std::make_shared<Configuration_t>
		(
				device,
				openKitType,
				"",
				"",
				"/App_ID%",
				0,
				"0",
				"",
				sessionIDProvider,
				sslTrustManager,
				beaconCacheConfiguration,
				beaconConfiguration
		);
	}

	std::shared_ptr<Configuration_t> getConfiguration(const Utf8String_t& beaconURL)
	{
		return std::make_shared<Configuration_t>
		(
				device,
				openKitType,
				"",
				"",
				"/App_ID%",
				0,
				"0",
				beaconURL,
				sessionIDProvider,
				sslTrustManager,
				beaconCacheConfiguration,
				beaconConfiguration
		);
	}
private:
	Device_sp device = nullptr;
	OpenKitType_t openKitType = OpenKitType_t::Type::DYNATRACE;
	ISessionIdProvider_sp sessionIDProvider = nullptr;
	ISslTrustManager_sp sslTrustManager = nullptr;
	BeaconConfiguration_sp beaconConfiguration = nullptr;
	BeaconCacheConfiguration_sp beaconCacheConfiguration = nullptr;

	Configuration_sp testConfiguration = nullptr;
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
	auto mockResponse = MockIStatusResponse::createNice();

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
	auto mockResponse = MockIStatusResponse::createNice();

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
	Utf8String_t host("localhost:9999");

	Utf8String_t tenantURL = Utf8String_t("http://");
	tenantURL.concatenate(host);
	tenantURL.concatenate("/mbeacon");

	auto target = getConfiguration(tenantURL);

	ASSERT_TRUE(tenantURL.equals(target->getHTTPClientConfiguration()->getBaseURL()));
}

TEST_F(ConfigurationTest, defaultDataCollectionLevelIsDefaultValueFromBeaconConfiguration)
{
	auto target = getDefaultConfiguration();
	ASSERT_EQ(target->getBeaconConfiguration()->getDataCollectionLevel(), core::configuration::DEFAULT_DATA_COLLECTION_LEVEL);
}

TEST_F(ConfigurationTest, defaultCrashReportingLevelIsDefaultValueFromBeaconConfiguration)
{
	auto target = getDefaultConfiguration();
	ASSERT_EQ(target->getBeaconConfiguration()->getCrashReportingLevel(), core::configuration::DEFAULT_CRASH_REPORTING_LEVEL);
}

TEST_F(ConfigurationTest, getApplicationID)
{
	// given
	auto target = getDefaultConfiguration();

	// then
	ASSERT_EQ(target->getApplicationID(), "/App_ID%");
}

TEST_F(ConfigurationTest, getApplicationIDPercentEncodedDoesPropperEncoding)
{
	// given
	auto target = getDefaultConfiguration();

	// then
	ASSERT_EQ(target->getApplicationIDPercentEncoded(), "%2FApp%5FID%25");
}
