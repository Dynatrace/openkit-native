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

#include "core/SessionWatchdog.h"

#include "mock/MockISessionWatchdogContext.h"
#include "../api/mock/MockILogger.h"
#include "../core/objects/mock/MockSessionInternals.h"
#include "../core/objects/mock/MockISessionProxy.h"

#include <chrono>
#include <future>
#include <memory>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace test;

using SessionWatchdog_t = core::SessionWatchdog;
using SessionWatchdog_sp = std::shared_ptr<SessionWatchdog_t>;
using MockILogger_sp = std::shared_ptr<MockILogger>;
using MockISessionWatchdogContext_sp = std::shared_ptr<MockISessionWatchdogContext>;
using MockSessionInternals_sp = std::shared_ptr<MockSessionInternals>;

class SessionWatchdogTest : public testing::Test
{
protected:

	MockILogger_sp mockLogger;
	MockISessionWatchdogContext_sp mockContext;
	MockSessionInternals_sp mockSession;

	std::promise<void> mPromise;
	std::future<void> mFuture;

	void SetUp() override
	{
		mockLogger = MockILogger::createNice();
		ON_CALL(*mockLogger, isDebugEnabled())
			.WillByDefault(testing::Return(true));

		mockContext = MockISessionWatchdogContext::createNice();
		mockSession = MockSessionInternals::createNice();

		mFuture = mPromise.get_future();
	}

	SessionWatchdog_sp createWatchdog()
	{
		return std::make_shared<SessionWatchdog_t>(mockLogger, mockContext);
	}
};

TEST_F(SessionWatchdogTest, initializeLogsWatchdogThreadStart)
{
	// expect
	EXPECT_CALL(*mockLogger, mockDebug(testing::_))
		.Times(2); // shutdown calls
	EXPECT_CALL(*mockLogger, mockDebug("SessionWatchdog initialize() - thread started"))
		.Times(1);
	EXPECT_CALL(*mockContext, isShutdownRequested())
		.Times(1)
		.WillOnce(testing::Invoke([this] {
				this->mPromise.set_value();
				return true;
			})
		);
	EXPECT_CALL(*mockContext, execute())
		.Times(0);

	// given
	auto target = createWatchdog();

	// when
	target->initialize();

	auto status = mFuture.wait_for(std::chrono::seconds(1)); // wait for thread
	ASSERT_THAT(status, testing::Eq(std::future_status::ready));

	// cleanup
	target->shutdown();
}

TEST_F(SessionWatchdogTest, contextIsExecutedUntilShutdownIsRequested)
{
	// expect
	EXPECT_CALL(*mockContext, isShutdownRequested())
		.Times(3)
		.WillOnce(testing::Return(false))
		.WillOnce(testing::Return(false))
		.WillOnce(testing::Invoke([this] {
				this->mPromise.set_value();
				return true;
			})
		);
	EXPECT_CALL(*mockContext, execute())
		.Times(2);

	// given
	auto target = createWatchdog();

	// when
	target->initialize();

	auto status = mFuture.wait_for(std::chrono::seconds(1)); // wait for thread
	ASSERT_THAT(status, testing::Eq(std::future_status::ready));

	// cleanup
	target->shutdown();
}

TEST_F(SessionWatchdogTest, shutdownLogsWatchdogThreadStop)
{
	// expect
	EXPECT_CALL(*mockLogger, mockDebug(testing::_))
		.Times(2); // shutdown calls
	EXPECT_CALL(*mockLogger, mockDebug("SessionWatchdog shutdown() - thread stopped"))
		.Times(1);

	// given
	ON_CALL(*mockContext, isShutdownRequested())
		.WillByDefault(testing::Invoke([this] {
				this->mPromise.set_value();
				return true;
			})
		);

	auto target = createWatchdog();
	target->initialize();

	auto status = mFuture.wait_for(std::chrono::seconds(1)); // wait for thread
	ASSERT_THAT(status, testing::Eq(std::future_status::ready));

	// when
	target->shutdown();
}

TEST_F(SessionWatchdogTest, shutdownLogsInvocation)
{
	// expect
	EXPECT_CALL(*mockLogger, mockDebug(testing::_))
		.Times(2); // shutdown calls
	EXPECT_CALL(*mockLogger, mockDebug("SessionWatchdog shutdown() - thread request shutdown"))
		.Times(1);

	// given
	ON_CALL(*mockContext, isShutdownRequested())
		.WillByDefault(testing::Invoke([this] {
				this->mPromise.set_value();
				return true;
			})
		);

	auto target = createWatchdog();
	target->initialize();

	auto status = mFuture.wait_for(std::chrono::seconds(1)); // wait for thread
	ASSERT_THAT(status, testing::Eq(std::future_status::ready));

	// when
	target->shutdown();
}

TEST_F(SessionWatchdogTest, closeOrEnqueueForClosingDelegatesToSessionWatchdogContext)
{
	// with
	const int64_t gracePeriod = 1;

	// expect
	EXPECT_CALL(*mockContext, closeOrEnqueueForClosing(testing::Eq(mockSession), gracePeriod))
		.Times(1);

	// given
	auto target = createWatchdog();

	// when
	target->closeOrEnqueueForClosing(mockSession, gracePeriod);
}

TEST_F(SessionWatchdogTest, dequeueFromClosingDelegatesToSessionWatchdogContext)
{
	// expect
	EXPECT_CALL(*mockContext, dequeueFromClosing(testing::Eq(mockSession)))
		.Times(1);

	// given
	auto target = createWatchdog();

	// when
	target->dequeueFromClosing(mockSession);
}

TEST_F(SessionWatchdogTest, addToSplitByTimeoutDelegatesToSessionWatchdogContext)
{
	// with
	auto mockSessionProxy = MockISessionProxy::createNice();

	// expect
	EXPECT_CALL(*mockContext, addToSplitByTimeout(testing::Eq(mockSessionProxy)))
		.Times(1);

	// given
	auto target = createWatchdog();

	// when
	target->addToSplitByTimeout(mockSessionProxy);
}

TEST_F(SessionWatchdogTest, removeFromSplitByTimeoutDelegatesToSessionWatchdogContext)
{
	// with
	auto mockSessionProxy = MockISessionProxy::createNice();

	// expect
	EXPECT_CALL(*mockContext, removeFromSplitByTimeout(testing::Eq(mockSessionProxy)))
		.Times(1);

	// given
	auto target = createWatchdog();

	// when
	target->removeFromSplitByTimeout(mockSessionProxy);
}