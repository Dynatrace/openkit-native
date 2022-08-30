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

#include "builder/TestOpenKitBuilder.h"
#include "mock/MockIOpenKitObject.h"
#include "../mock/MockISessionWatchdog.h"
#include "../mock/MockIBeaconSender.h"
#include "../caching/mock/MockIBeaconCache.h"
#include "../caching/mock/MockIBeaconCacheEvictor.h"
#include "../configuration/mock/MockIOpenKitConfiguration.h"
#include "../configuration/mock/MockIPrivacyConfiguration.h"
#include "../../api/mock/MockILogger.h"
#include "../../providers/mock/MockITimingProvider.h"
#include "../../providers/mock/MockIThreadIDProvider.h"
#include "../../providers/mock/MockISessionIDProvider.h"
#include "../../DefaultValues.h"

#include "core/UTF8String.h"
#include "core/objects/IOpenKitObject.h"
#include "core/objects/NullSession.h"
#include "core/objects/OpenKit.h"
#include "core/objects/SessionProxy.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <memory>

using namespace test;

using IOpenKitObject_t = core::objects::IOpenKitObject;
using NullSession_t = core::objects::NullSession;
using MockIBeaconCache_sp = std::shared_ptr<MockIBeaconCache>;
using MockIBeaconCacheEvictor_sp = std::shared_ptr<MockIBeaconCacheEvictor>;
using MockIBeaconSender_sp = std::shared_ptr<MockIBeaconSender>;
using MockISessionWatchdog_sp = std::shared_ptr<MockISessionWatchdog>;
using MockILogger_sp = std::shared_ptr<MockILogger>;
using MockIOpenKitConfiguration_sp = std::shared_ptr<MockIOpenKitConfiguration>;
using MockIPrivacyConfiguration_sp = std::shared_ptr<MockIPrivacyConfiguration>;
using MockITimingProvider_sp = std::shared_ptr<MockITimingProvider>;
using MockIThreadIDProvider_sp = std::shared_ptr<MockIThreadIDProvider>;
using MockISessionIDProvider_sp = std::shared_ptr<MockISessionIDProvider>;
using OpenKit_sp = std::shared_ptr<core::objects::OpenKit>;
using SessionProxy_t = core::objects::SessionProxy;
using Utf8String_t = core::UTF8String;

const Utf8String_t APP_ID("appID");
const int64_t DEVICE_ID = 1234;

class OpenKitTest : public testing::Test
{
protected:

	MockILogger_sp mockLogger;
	MockIPrivacyConfiguration_sp mockPrivacyConfig;
	MockIOpenKitConfiguration_sp mockOpenKitConfig;
	MockITimingProvider_sp mockTimingProvider;
	MockIThreadIDProvider_sp mockThreadIdProvider;
	MockISessionIDProvider_sp mockSessionIdProvider;
	MockIBeaconCache_sp mockBeaconCache;
	MockIBeaconSender_sp mockBeaconSender;
	MockIBeaconCacheEvictor_sp mockBeaconCacheEvictor;
	MockISessionWatchdog_sp mockSessionWatchdog;

	void SetUp() override
	{
		mockLogger = MockILogger::createNice();
		mockPrivacyConfig = MockIPrivacyConfiguration::createNice();

		mockOpenKitConfig = MockIOpenKitConfiguration::createNice();
		ON_CALL(*mockOpenKitConfig, getApplicationId())
			.WillByDefault(testing::ReturnRef(APP_ID));
		ON_CALL(*mockOpenKitConfig, getDeviceId())
			.WillByDefault(testing::Return(DEVICE_ID));
		ON_CALL(*mockOpenKitConfig, getOperatingSystem())
			.WillByDefault(testing::ReturnRef(DefaultValues::UTF8_EMPTY_STRING));
		ON_CALL(*mockOpenKitConfig, getManufacturer())
			.WillByDefault(testing::ReturnRef(DefaultValues::UTF8_EMPTY_STRING));
		ON_CALL(*mockOpenKitConfig, getModelId())
			.WillByDefault(testing::ReturnRef(DefaultValues::UTF8_EMPTY_STRING));

		mockTimingProvider = MockITimingProvider::createNice();
		mockThreadIdProvider = MockIThreadIDProvider::createNice();
		mockSessionIdProvider = MockISessionIDProvider::createNice();
		mockBeaconCache = MockIBeaconCache::createNice();
		mockBeaconSender = MockIBeaconSender::createNice();
		mockBeaconCacheEvictor = MockIBeaconCacheEvictor::createNice();
		mockSessionWatchdog = MockISessionWatchdog::createNice();
	}

	std::shared_ptr<TestOpenKitBuilder> createOpenKit()
	{
		auto builder = std::make_shared<TestOpenKitBuilder>();

		builder->with(mockLogger)
			.with(mockPrivacyConfig)
			.with(mockOpenKitConfig)
			.with(mockTimingProvider)
			.with(mockThreadIdProvider)
			.with(mockSessionIdProvider)
			.with(mockBeaconCache)
			.with(mockBeaconSender)
			.with(mockBeaconCacheEvictor)
			.with(mockSessionWatchdog)
		;

		return builder;
	}
};

TEST_F(OpenKitTest, initializeStartsTheBeaconCacheEvictor)
{
	// with
	auto beaconCacheEvictor = MockIBeaconCacheEvictor::createStrict();

	// expect
	EXPECT_CALL(*beaconCacheEvictor, start()).Times(1);

	// given
	auto target = createOpenKit()
		->with(beaconCacheEvictor)
		.build();

	// when
	target->initialize();
}

TEST_F(OpenKitTest, initializeInitializesBeaconSender)
{
	// with
	auto beaconSender = MockIBeaconSender::createStrict();

	// expect
	EXPECT_CALL(*beaconSender, initialize()).Times(1);

	// given
	auto target = createOpenKit()
		->with(beaconSender)
		.build();

	// when
	target->initialize();
}

TEST_F(OpenKitTest, initializeInitializesSessionWatchdog)
{
	// with
	auto sessionWatchdog = MockISessionWatchdog::createStrict();

	// expect
	EXPECT_CALL(*sessionWatchdog, initialize()).Times(1);

	// given
	auto target = createOpenKit()
		->with(sessionWatchdog)
		.build();

	// when
	target->initialize();
}


TEST_F(OpenKitTest, waitForInitCompletionForwardsCallToBeaconSender)
{
	// with
	auto beaconSender = MockIBeaconSender::createStrict();

	// expect
	EXPECT_CALL(*beaconSender, waitForInit())
		.Times(2)
		.WillOnce(testing::Return(false))
		.WillRepeatedly(testing::Return(true));

	// given
	auto target = createOpenKit()
		->with(beaconSender)
		.build();

	// when
	auto obtained = target->waitForInitCompletion();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));

	// and when
	obtained = target->waitForInitCompletion();

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
}

TEST_F(OpenKitTest, waitForInitCompletionWithTimoutFowardsTheCallToBeaconSender)
{
	// with
	auto beaconSender = MockIBeaconSender::createStrict();

	// expect
	EXPECT_CALL(*beaconSender, waitForInit(testing::Eq(100)))
		.Times(1)
		.WillRepeatedly(testing::Return(false));
	EXPECT_CALL(*beaconSender, waitForInit(testing::Eq(200)))
		.Times(1)
		.WillRepeatedly(testing::Return(true));

	// given
	auto target = createOpenKit()
		->with(beaconSender)
		.build();

	// when
	auto obtained = target->waitForInitCompletion(100);

	// then
	ASSERT_THAT(obtained, testing::Eq(false));

	// and when
	obtained = target->waitForInitCompletion(200);

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
}

TEST_F(OpenKitTest, isInitializedForwardsCallToBeaconSender)
{
	// with
	auto beaconSender = MockIBeaconSender::createStrict();

	// expect
	EXPECT_CALL(*beaconSender, isInitialized())
		.Times(2)
		.WillOnce(testing::Return(false))
		.WillRepeatedly(testing::Return(true));

	// given
	auto target = createOpenKit()
		->with(beaconSender)
		.build();

	// when
	auto obtained = target->isInitialized();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));

	// and when
	obtained = target->isInitialized();

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
}

TEST_F(OpenKitTest, shutdownStopsTheBeaconCacheEvictor)
{
	// with
	auto beaconCacheEvictor = MockIBeaconCacheEvictor::createStrict();

	// expect
	EXPECT_CALL(*beaconCacheEvictor, stop()).Times(1);

	// given
	auto target = createOpenKit()
		->with(beaconCacheEvictor)
		.build();

	// when
	target->shutdown();
}

TEST_F(OpenKitTest, shutdownShutsDownBeaconSender)
{
	// with
	auto beaconSender = MockIBeaconSender::createStrict();

	// expect
	EXPECT_CALL(*beaconSender, shutdown()).Times(1);

	// given
	auto target = createOpenKit()
		->with(beaconSender)
		.build();

	// when
	target->shutdown();
}

TEST_F(OpenKitTest, shutdownShutsDownSessionWatchdog)
{
	// with
	auto sessionWatchdog = MockISessionWatchdog::createStrict();

	// expect
	EXPECT_CALL(*sessionWatchdog, shutdown()).Times(1);

	// given
	auto target = createOpenKit()
		->with(sessionWatchdog)
		.build();

	// when
	target->shutdown();
}

TEST_F(OpenKitTest, shutdownClosesAllChildObjects)
{
	// with
	auto childObjectOne = MockIOpenKitObject::createStrict();
	auto childObjectTwo = MockIOpenKitObject::createStrict();

	// expect
	EXPECT_CALL(*childObjectOne, close()).Times(1);
	EXPECT_CALL(*childObjectTwo, close()).Times(1);

	// given
	auto target = createOpenKit()->build();
	target->storeChildInList(childObjectOne);
	target->storeChildInList(childObjectTwo);

	// when
	target->shutdown();
}

TEST_F(OpenKitTest, closeCallsShutdown)
{
	// with
	auto childObjectOne = MockIOpenKitObject::createStrict();
	auto childObjectTwo = MockIOpenKitObject::createStrict();

	// expect
	EXPECT_CALL(*childObjectOne, close()).Times(1);
	EXPECT_CALL(*childObjectTwo, close()).Times(1);

	// given
	auto target = createOpenKit()->build();
	target->storeChildInList(childObjectOne);
	target->storeChildInList(childObjectTwo);

	// when
	target->close();
}

TEST_F(OpenKitTest, callingShutdownASecondTimeReturnsImmediately)
{
	// with
	auto beaconSender = MockIBeaconSender::createStrict();
	auto beaconCacheEvictor = MockIBeaconCacheEvictor::createStrict();

	auto childObjectOne = MockIOpenKitObject::createStrict();
	auto childObjectTwo = MockIOpenKitObject::createStrict();

	// expect
	EXPECT_CALL(*beaconCacheEvictor, stop()).Times(1);
	EXPECT_CALL(*beaconSender, shutdown()).Times(1);
	EXPECT_CALL(*childObjectOne, close()).Times(1);
	EXPECT_CALL(*childObjectTwo, close()).Times(1);

	// given
	auto target = createOpenKit()
		->with(beaconSender)
		.with(beaconCacheEvictor)
		.build();
	target->storeChildInList(childObjectOne);
	target->storeChildInList(childObjectTwo);

	// when
	target->shutdown();
	target->shutdown();
}

TEST_F(OpenKitTest, createSessionReturnsSessionProxyObject)
{
	// given
	auto target = createOpenKit()->build();

	// when
	auto obtained = target->createSession("127.0.0.1");

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(std::dynamic_pointer_cast<SessionProxy_t>(obtained), testing::NotNull());

	// break dependency cycle: session as child in OpenKit
	obtained->end();
}

TEST_F(OpenKitTest, createSessionWithoutIpReturnsSessionProxyObject)
{
	// given
	auto target = createOpenKit()->build();

	// when
	auto obtained = target->createSession();

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(std::dynamic_pointer_cast<SessionProxy_t>(obtained), testing::NotNull());

	// break dependency cycle: session as child in OpenKit
	obtained->end();
}

TEST_F(OpenKitTest, createSessionAddsNewlyCreatedSessionToListOfChildren)
{
	// given
	auto target = createOpenKit()->build();

	// when
	auto obtainedOne = target->createSession("127.0.0.1");

	// then
	ASSERT_THAT(obtainedOne, testing::NotNull());
	auto objectOne = std::dynamic_pointer_cast<IOpenKitObject_t>(obtainedOne);
	ASSERT_THAT(objectOne, testing::NotNull());

	auto childObjects = target->getCopyOfChildObjects();
	ASSERT_THAT(childObjects.size(), testing::Eq(size_t(1)));
	ASSERT_THAT(objectOne, testing::Eq(*childObjects.begin()));

	// and when
	auto obtainedTwo = target->createSession("192.168.0.1");

	// then
	ASSERT_THAT(obtainedTwo, testing::NotNull());
	auto objectTwo = std::dynamic_pointer_cast<IOpenKitObject_t>(obtainedTwo);
	ASSERT_THAT(objectTwo, testing::NotNull());

	childObjects = target->getCopyOfChildObjects();
	ASSERT_THAT(childObjects.size(), testing::Eq(size_t(2)));
	ASSERT_THAT(objectOne, testing::Eq(*childObjects.begin()));
	ASSERT_THAT(objectTwo, testing::Eq(*(++childObjects.begin())));

	// break dependency cycle: sessions as children in OpenKit
	obtainedOne->end();
	obtainedTwo->end();
}

TEST_F(OpenKitTest, createSesionWithoutIpAddsNewlyCreatedSessionToListOfChildren)
{
	// given
	auto target = createOpenKit()->build();

	// when
	auto obtainedOne = target->createSession();

	// then
	ASSERT_THAT(obtainedOne, testing::NotNull());
	auto objectOne = std::dynamic_pointer_cast<IOpenKitObject_t>(obtainedOne);
	ASSERT_THAT(objectOne, testing::NotNull());

	auto childObjects = target->getCopyOfChildObjects();
	ASSERT_THAT(childObjects.size(), testing::Eq(size_t(1)));
	ASSERT_THAT(objectOne, testing::Eq(*childObjects.begin()));

	// and when
	auto obtainedTwo = target->createSession();

	// then
	ASSERT_THAT(obtainedTwo, testing::NotNull());
	auto objectTwo = std::dynamic_pointer_cast<IOpenKitObject_t>(obtainedTwo);
	ASSERT_THAT(objectTwo, testing::NotNull());

	childObjects = target->getCopyOfChildObjects();
	ASSERT_THAT(childObjects.size(), testing::Eq(size_t(2)));
	ASSERT_THAT(objectOne, testing::Eq(*childObjects.begin()));
	ASSERT_THAT(objectTwo, testing::Eq(*(++childObjects.begin())));

	// break dependency cycle: sessions as children in OpenKit
	obtainedOne->end();
	obtainedTwo->end();
}

TEST_F(OpenKitTest, createSessionAfterShutdownHasBeenCalledReturnsNullSession)
{
	// given
	auto target = createOpenKit()->build();
	target->shutdown();

	// when
	auto obtained = target->createSession("10.0.0.1");

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	auto nullSession = std::dynamic_pointer_cast<NullSession_t>(obtained);
	ASSERT_THAT(nullSession, testing::NotNull());
	ASSERT_THAT(nullSession, testing::Eq(NullSession_t::instance()));
}

TEST_F(OpenKitTest, createSessionWithoutIpAfterShutdownHasBeenCalledReturnsNullSession)
{
	// given
	auto target = createOpenKit()->build();
	target->shutdown();

	// when
	auto obtained = target->createSession();

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	auto nullSession = std::dynamic_pointer_cast<NullSession_t>(obtained);
	ASSERT_THAT(nullSession, testing::NotNull());
	ASSERT_THAT(nullSession, testing::Eq(NullSession_t::instance()));
}

TEST_F(OpenKitTest, onChildClosedRemovesArgumentFromListOfChildren)
{
	// given
	auto target = createOpenKit()->build();

	auto childObjectOne = MockIOpenKitObject::createStrict();
	auto childObjectTwo = MockIOpenKitObject::createStrict();
	target->storeChildInList(childObjectOne);
	target->storeChildInList(childObjectTwo);

	// when
	target->onChildClosed(childObjectOne);

	// then
	auto childObjects = target->getCopyOfChildObjects();
	ASSERT_THAT(childObjects.size(), testing::Eq(size_t(1)));
	ASSERT_THAT(*childObjects.begin(), testing::Eq(childObjectTwo));

	// and when
	target->onChildClosed(childObjectTwo);

	// then
	auto numChildObjects = target->getChildCount();
	ASSERT_THAT(numChildObjects, testing::Eq(size_t(0)));
}