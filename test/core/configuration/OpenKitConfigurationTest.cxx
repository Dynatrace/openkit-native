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

#include "../../api/mock/MockIOpenKitBuilder.h"
#include "../../api/mock/MockISslTrustManager.h"

#include "core/configuration/OpenKitConfiguration.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace test;

using MockIOpenKitBuilder_sp = std::shared_ptr<MockIOpenKitBuilder>;
using OpenKitConfiguration_t = core::configuration::OpenKitConfiguration;

class OpenKitConfigurationTest : public testing::Test
{
protected:

	const std::string ENDPOINT_URL = "https://localhost:9999/1";
	const int64_t DEVICE_ID = 37;
	const std::string OPENKIT_TYPE = "Dynatrace NextGen";
	const std::string APPLICATION_ID = "Application-ID";
	const std::string APPLICATION_NAME = "Application Name";
	const std::string APPLICATION_VERSION = "1.2.3.4-b4321";
	const std::string OPERATING_SYSTEM = "Linux #253-Microsoft Mon Dec 31 17:49:00 PST 2018 x86_64 GNU/Linux";
	const std::string MANUFACTURER = "Dynatrace";
	const std::string MODEL_ID = "Latest Model";
	const int32_t DEFAULT_SERVER_ID = 777;

	MockIOpenKitBuilder_sp mockOpenKitBuilder;

	void SetUp() override
	{
		mockOpenKitBuilder = MockIOpenKitBuilder::createNice();
		ON_CALL(*mockOpenKitBuilder, getEndpointURL())
			.WillByDefault(testing::ReturnRef(ENDPOINT_URL));
		ON_CALL(*mockOpenKitBuilder, getDeviceID())
			.WillByDefault(testing::Return(DEVICE_ID));
		ON_CALL(*mockOpenKitBuilder, getOpenKitType())
			.WillByDefault(testing::ReturnRef(OPENKIT_TYPE));
		ON_CALL(*mockOpenKitBuilder, getApplicationID())
			.WillByDefault(testing::ReturnRef(APPLICATION_ID));
		ON_CALL(*mockOpenKitBuilder, getApplicationName())
			.WillByDefault(testing::ReturnRef(APPLICATION_NAME));
		ON_CALL(*mockOpenKitBuilder, getApplicationVersion())
			.WillByDefault(testing::ReturnRef(APPLICATION_VERSION));
		ON_CALL(*mockOpenKitBuilder, getOperatingSystem())
			.WillByDefault(testing::ReturnRef(OPERATING_SYSTEM));
		ON_CALL(*mockOpenKitBuilder, getManufacturer())
			.WillByDefault(testing::ReturnRef(MANUFACTURER));
		ON_CALL(*mockOpenKitBuilder, getModelID())
			.WillByDefault(testing::ReturnRef(MODEL_ID));
		ON_CALL(*mockOpenKitBuilder, getDefaultServerID())
			.WillByDefault(testing::Return(DEFAULT_SERVER_ID));
	}
};

TEST_F(OpenKitConfigurationTest, creatingOpenKitFromNonNullBuilderGivesNonNullConfiguration)
{
	// when
	auto obtained = OpenKitConfiguration_t::from(*mockOpenKitBuilder);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
}

TEST_F(OpenKitConfigurationTest, creatingAnOpenKitconfigurationFromBuilderCopiesEndpointUrl)
{
	// expect
	EXPECT_CALL(*mockOpenKitBuilder, getEndpointURL()).Times(1);

	// when
	auto obtained = OpenKitConfiguration_t::from(*mockOpenKitBuilder);

	// then
	ASSERT_THAT(obtained->getEndpointUrl(), testing::Eq(ENDPOINT_URL));
}

TEST_F(OpenKitConfigurationTest, creatingAnOpenKitConfigurationFromBuilderCopiesDeviceId)
{
	// expect
	EXPECT_CALL(*mockOpenKitBuilder, getDeviceID()).Times(1);

	// when
	auto obtained = OpenKitConfiguration_t::from(*mockOpenKitBuilder);

	// then
	ASSERT_THAT(obtained->getDeviceId(), testing::Eq(DEVICE_ID));
}

TEST_F(OpenKitConfigurationTest, creatingAnOpenKitConfigurationFromBuilderCopiesOpenKitType)
{
	// expect
	EXPECT_CALL(*mockOpenKitBuilder, getOpenKitType()).Times(1);

	// when
	auto obtained = OpenKitConfiguration_t::from(*mockOpenKitBuilder);

	// then
	ASSERT_THAT(obtained->getOpenKitType(), testing::Eq(OPENKIT_TYPE));
}

TEST_F(OpenKitConfigurationTest, creatingAnOpenKitConfigurationFromBuilderCopiesApplicationId)
{
	// expect
	EXPECT_CALL(*mockOpenKitBuilder, getApplicationID()).Times(2); // 1 to pass the value + 1 for percent encoding

	// when
	auto obtained = OpenKitConfiguration_t::from(*mockOpenKitBuilder);

	// then
	ASSERT_THAT(obtained->getApplicationId(), testing::Eq(APPLICATION_ID));
}

TEST_F(OpenKitConfigurationTest, creatingAnOpenKitConfigurationFromBuilderPercentEncodesApplicationId)
{
	// with
	std::string appId("/App_ID%");
	ON_CALL(*mockOpenKitBuilder, getApplicationID()).WillByDefault(testing::ReturnRef(appId));

	// given
	auto obtained = OpenKitConfiguration_t::from(*mockOpenKitBuilder);

	// then
	ASSERT_THAT(obtained->getApplicationIdPercentEncoded(), testing::Eq("%2FApp%5FID%25"));
}

TEST_F(OpenKitConfigurationTest, creatingAnOpenKitConfigurationFromBuilderCopiesApplicationName)
{
	// expect
	EXPECT_CALL(*mockOpenKitBuilder, getApplicationName()).Times(1);

	// when
	auto obtained = OpenKitConfiguration_t::from(*mockOpenKitBuilder);

	// then
	ASSERT_THAT(obtained->getApplicationName(), testing::Eq(APPLICATION_NAME));
}

TEST_F(OpenKitConfigurationTest, creatingAnOpenKitConfigurationFromBuilderCopiesApplicationVersion)
{
	// expect
	EXPECT_CALL(*mockOpenKitBuilder, getApplicationVersion()).Times(1);

	// when
	auto obtained = OpenKitConfiguration_t::from(*mockOpenKitBuilder);

	// then
	ASSERT_THAT(obtained->getApplicationVersion(), testing::Eq(APPLICATION_VERSION));
}

TEST_F(OpenKitConfigurationTest, creatingAnOpenKitConfigurationFromBuilderCopiesOperatingSystem)
{
	// expect
	EXPECT_CALL(*mockOpenKitBuilder, getOperatingSystem()).Times(1);

	// when
	auto obtained = OpenKitConfiguration_t::from(*mockOpenKitBuilder);

	// then
	ASSERT_THAT(obtained->getOperatingSystem(), testing::Eq(OPERATING_SYSTEM));
}

TEST_F(OpenKitConfigurationTest, creatingAnOpenKitConfigurationFromBuilderCopiesManufacturer)
{
	// expect
	EXPECT_CALL(*mockOpenKitBuilder, getManufacturer()).Times(1);

	// when
	auto obtained = OpenKitConfiguration_t::from(*mockOpenKitBuilder);

	// then
	ASSERT_THAT(obtained->getManufacturer(), testing::Eq(MANUFACTURER));
}

TEST_F(OpenKitConfigurationTest, creatingAnOpenKitConfigurationFromBuilderCopiesModelId)
{
	// expect
	EXPECT_CALL(*mockOpenKitBuilder, getModelID()).Times(1);

	// when
	auto obtained = OpenKitConfiguration_t::from(*mockOpenKitBuilder);

	// then
	ASSERT_THAT(obtained->getModelId(), testing::Eq(MODEL_ID));
}

TEST_F(OpenKitConfigurationTest, creatingAnOpenKitConfigurationFromBuilderCopiesDefaultServerId)
{
	// expect
	EXPECT_CALL(*mockOpenKitBuilder, getDefaultServerID()).Times(1);

	// when
	auto obtained = OpenKitConfiguration_t::from(*mockOpenKitBuilder);

	// then
	ASSERT_THAT(obtained->getDefaultServerId(), testing::Eq(DEFAULT_SERVER_ID));
}

TEST_F(OpenKitConfigurationTest, creatingAnOpenKitConfigurationFromBuilderCopiesTrustManager)
{
	// with
	auto trustManager = MockISslTrustManager::createStrict();

	// expect
	EXPECT_CALL(*mockOpenKitBuilder, getTrustManager())
		.Times(1)
		.WillOnce(testing::Return(trustManager));

	// when
	auto obtained = OpenKitConfiguration_t::from(*mockOpenKitBuilder);

	// then
	ASSERT_THAT(obtained->getTrustManager(), testing::Eq(trustManager));
}