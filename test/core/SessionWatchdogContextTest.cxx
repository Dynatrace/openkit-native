/**
 * Copyright 2018-2020 Dynatrace LLC
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

#include "core/SessionWatchdogContext.h"
#include "core/objects/SessionInternals.h"

#include "../core/objects/mock/MockSessionInternals.h"
#include "../core/util/mock/MockIInterruptibleThreadSuspender.h"
#include "../providers/mock/MockITimingProvider.h"

#include <array>
#include <memory>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace test;

using MockITimingProvider_sp = std::shared_ptr<MockITimingProvider>;
using MockIInterruptibleThreadSuspender_sp = std::shared_ptr<MockIInterruptibleThreadSuspender>;
using MockSessionInternals_sp = std::shared_ptr<MockSessionInternals>;
using SessionInternals_t = core::objects::SessionInternals;
using ISessionWatchdogContext_t = core::ISessionWatchdogContext;
using SessionWatchdogContext_t = core::SessionWatchdogContext;
using SessionWatchdogContext_sp = std::shared_ptr< SessionWatchdogContext_t>;

class SessionWatchdogContextTest : public testing::Test
{
protected:

	MockITimingProvider_sp mockTimingProvider;
	MockIInterruptibleThreadSuspender_sp mockThreadSuspender;
	MockSessionInternals_sp mockSession;
	int64_t mSplitByEventsGracePeriodEndTimeInMillis = -1;

	void SetUp() override
	{
		mockTimingProvider = MockITimingProvider::createNice();
		mockThreadSuspender = MockIInterruptibleThreadSuspender::createNice();

		mSplitByEventsGracePeriodEndTimeInMillis = -1;
		mockSession = MockSessionInternals::createNice();
		ON_CALL(*mockSession, setSplitByEventsGracePeriodEndTimeInMillis(testing::_))
			.WillByDefault(testing::SaveArg<0>(&mSplitByEventsGracePeriodEndTimeInMillis));
		ON_CALL(*mockSession, getSplitByEventsGracePeriodEndTimeInMillis())
			.WillByDefault(testing::ReturnPointee(&mSplitByEventsGracePeriodEndTimeInMillis));
	}

	SessionWatchdogContext_sp createContext()
	{
		return std::make_shared<SessionWatchdogContext_t>(mockTimingProvider, mockThreadSuspender);
	}
};

TEST_F(SessionWatchdogContextTest, onDefaultShutdownIsFalse)
{
	// given
	auto target = createContext();

	// then
	ASSERT_THAT(target->isShutdownRequested(), testing::Eq(false));
}

TEST_F(SessionWatchdogContextTest, onDefaultSessionsToCloseIsEmpty)
{
	// given
	auto target = createContext();

	// then
	ASSERT_THAT(target->getSessionsToClose().size(), testing::Eq(0));
}

TEST_F(SessionWatchdogContextTest, closeOrEnqueueForClosingDoesNotAddSessionIfItCanBeClosed)
{
	// expect
	EXPECT_CALL(*mockSession, tryEnd())
		.Times(1)
		.WillRepeatedly(testing::Return(true));

	// given
	auto target = createContext();
	auto targetExplicit = std::dynamic_pointer_cast<ISessionWatchdogContext_t>(target);

	// when
	targetExplicit->closeOrEnqueueForClosing(mockSession, 0);

	// then
	ASSERT_THAT(target->getSessionsToClose().size(), testing::Eq(0));
}

TEST_F(SessionWatchdogContextTest, closeOrEnqueueForClosingAddsSessionIfSessionCannotBeClosed)
{
	// expect
	EXPECT_CALL(*mockSession, tryEnd())
		.Times(1)
		.WillRepeatedly(testing::Return(false));

	// given
	auto target = createContext();
	auto targetExplicit = std::dynamic_pointer_cast<ISessionWatchdogContext_t>(target);

	// when
	targetExplicit->closeOrEnqueueForClosing(mockSession, 17);

	// then
	ASSERT_THAT(target->getSessionsToClose().size(), testing::Eq(1));
}

TEST_F(SessionWatchdogContextTest, closeOrEnqueueForClosingSetsSplitByEventsGracePeriodEndTimeIfSessionCannotBeClosed)
{
	// with
	const int64_t timestamp = 10;
	const int64_t gracePeriod = 5;

	// expect
	EXPECT_CALL(*mockSession, setSplitByEventsGracePeriodEndTimeInMillis(timestamp + gracePeriod))
		.Times(1);

	// given
	ON_CALL(*mockTimingProvider, provideTimestampInMilliseconds())
		.WillByDefault(testing::Return(timestamp));
	ON_CALL(*mockSession, tryEnd())
		.WillByDefault(testing::Return(false));

	auto target = createContext();
	auto targetExplicit = std::dynamic_pointer_cast<ISessionWatchdogContext_t>(target);

	// when
	targetExplicit->closeOrEnqueueForClosing(mockSession, gracePeriod);
}

TEST_F(SessionWatchdogContextTest, dequeueFromClosingRemovesSession)
{
	// given
	auto target = createContext();
	auto targetExplicit = std::dynamic_pointer_cast<ISessionWatchdogContext_t>(target);
	targetExplicit->closeOrEnqueueForClosing(mockSession, 0);
	ASSERT_THAT(target->getSessionsToClose().size(), testing::Eq(1));

	// when
	targetExplicit->dequeueFromClosing(mockSession);

	// then
	ASSERT_THAT(target->getSessionsToClose().size(), testing::Eq(0));
}

TEST_F(SessionWatchdogContextTest, executeEndsSessionsWithExpiredGracePeriod)
{
	// expect
	EXPECT_CALL(*mockSession, end())
		.Times(1);

	// given
	std::array<int64_t, 2> timestamps = { 5, 10 };
	int32_t callCount = 0;
	ON_CALL(*mockTimingProvider, provideTimestampInMilliseconds())
		.WillByDefault(testing::Invoke([=, &callCount] {
			return timestamps[callCount++];
		}));
	ON_CALL(*mockSession, tryEnd())
		.WillByDefault(testing::Return(false));

	auto target = createContext();
	auto targetExplicit = std::dynamic_pointer_cast<ISessionWatchdogContext_t>(target);

	targetExplicit->closeOrEnqueueForClosing(mockSession, 4);

	// when
	target->execute();

	// then
	ASSERT_THAT(target->getSessionsToClose().size(), testing::Eq(0));
}

TEST_F(SessionWatchdogContextTest, executeEndsSessionsWithGraceEndTimeSameAsCurrentTime)
{
	// expect
	EXPECT_CALL(*mockSession, end())
		.Times(1);

	// given
	ON_CALL(*mockTimingProvider, provideTimestampInMilliseconds())
		.WillByDefault(testing::Return(5));
	ON_CALL(*mockSession, tryEnd())
		.WillByDefault(testing::Return(false));

	auto target = createContext();
	auto targetExplicit = std::dynamic_pointer_cast<ISessionWatchdogContext_t>(target);

	targetExplicit->closeOrEnqueueForClosing(mockSession, 0);

	// when
	target->execute();

	// then
	ASSERT_THAT(target->getSessionsToClose().size(), testing::Eq(0));
}

TEST_F(SessionWatchdogContextTest, executeDoesNotEndSessionsWhenGracePeriodIsNotExpired)
{
	// with
	const int64_t gracePeriod = 1;
	const int64_t now = 5;

	// expect
	EXPECT_CALL(*mockSession, end())
		.Times(0);
	EXPECT_CALL(*mockSession, setSplitByEventsGracePeriodEndTimeInMillis(now + gracePeriod))
		.Times(1);

	// given
	ON_CALL(*mockTimingProvider, provideTimestampInMilliseconds())
		.WillByDefault(testing::Return(now));
	ON_CALL(*mockSession, tryEnd())
		.WillByDefault(testing::Return(false));

	auto target = createContext();
	auto targetExplicit = std::dynamic_pointer_cast<ISessionWatchdogContext_t>(target);

	targetExplicit->closeOrEnqueueForClosing(mockSession, gracePeriod); // close at now + gracePeriod

	// when
	target->execute();

	// then
	ASSERT_THAT(target->getSessionsToClose().size(), testing::Eq(1));
}

TEST_F(SessionWatchdogContextTest, executeSleepsDefaultTimeIfNoSessionToCloseExists)
{
	// expect
	EXPECT_CALL(*mockThreadSuspender, sleep(SessionWatchdogContext_t::DEFAULT_SLEEP_TIME_MILLISECONDS.count()))
		.Times(1);

	// given
	auto target = createContext();

	// when
	target->execute();
}

TEST_F(SessionWatchdogContextTest, executeSleepsDefaultTimeIfSessionIsExpiredAndNoFurtherNonExpiredSessions)
{
	// expect
	EXPECT_CALL(*mockSession, end())
		.Times(1);
	EXPECT_CALL(*mockThreadSuspender, sleep(SessionWatchdogContext_t::DEFAULT_SLEEP_TIME_MILLISECONDS.count()))
		.Times(1);

	// given
	std::array<int64_t, 2> timestamps = { 5, 10 };
	int32_t callCount = 0;
	ON_CALL(*mockTimingProvider, provideTimestampInMilliseconds())
		.WillByDefault(testing::Invoke([=, &callCount] {
		return timestamps[callCount++];
			}));
	ON_CALL(*mockSession, tryEnd())
		.WillByDefault(testing::Return(false));

	auto target = std::dynamic_pointer_cast<ISessionWatchdogContext_t>(createContext());

	target->closeOrEnqueueForClosing(mockSession, 3);

	// when
	target->execute();
}

TEST_F(SessionWatchdogContextTest, executeSleepsMinimumTimeToNextSessionGraceEndPeriod)
{
	// with
	auto mockSession1 = MockSessionInternals::createNice();
	auto mockSession2 = MockSessionInternals::createNice();

	// expect
	EXPECT_CALL(*mockThreadSuspender, sleep(3))
		.Times(1);
	EXPECT_CALL(*mockSession, end())
		.Times(0);
	EXPECT_CALL(*mockSession1, end())
		.Times(0);
	EXPECT_CALL(*mockSession2, end())
		.Times(0);

	// given
	ON_CALL(*mockSession, getSplitByEventsGracePeriodEndTimeInMillis())
		.WillByDefault(testing::Return(4));
	ON_CALL(*mockSession, tryEnd())
		.WillByDefault(testing::Return(false));

	ON_CALL(*mockSession1, getSplitByEventsGracePeriodEndTimeInMillis())
		.WillByDefault(testing::Return(3));
	ON_CALL(*mockSession1, tryEnd())
		.WillByDefault(testing::Return(false));

	ON_CALL(*mockSession2, getSplitByEventsGracePeriodEndTimeInMillis())
		.WillByDefault(testing::Return(5));
	ON_CALL(*mockSession2, tryEnd())
		.WillByDefault(testing::Return(false));

	ON_CALL(*mockTimingProvider, provideTimestampInMilliseconds())
		.WillByDefault(testing::Return(0));

	// given
	auto target = std::dynamic_pointer_cast<ISessionWatchdogContext_t>(createContext());
	target->closeOrEnqueueForClosing(mockSession, 4);
	target->closeOrEnqueueForClosing(mockSession1, 3);
	target->closeOrEnqueueForClosing(mockSession2, 5);

	// when
	target->execute();
}

TEST_F(SessionWatchdogContextTest, executeDoesNotSleepLongerThanDefaultSleepTime)
{
	// expect
	EXPECT_CALL(*mockThreadSuspender, sleep(SessionWatchdogContext_t::DEFAULT_SLEEP_TIME_MILLISECONDS.count()))
		.Times(1);

	// given
	auto target = std::dynamic_pointer_cast<ISessionWatchdogContext_t>(createContext());
	target->closeOrEnqueueForClosing(mockSession, SessionWatchdogContext_t::DEFAULT_SLEEP_TIME_MILLISECONDS.count() + 10);

	// when
	target->execute();
}

TEST_F(SessionWatchdogContextTest, requestShutdownSetsIsShutdownRequestedToTrue)
{
	// given
	auto target = std::dynamic_pointer_cast<ISessionWatchdogContext_t>(createContext());
	ASSERT_THAT(target->isShutdownRequested(), testing::Eq(false));

	// when
	target->requestShutdown();

	// then
	ASSERT_THAT(target->isShutdownRequested(), testing::Eq(true));
}

TEST_F(SessionWatchdogContextTest, requestShutdownWakesUpThreadSuspender)
{
	// expect
	EXPECT_CALL(*mockThreadSuspender, wakeup())
		.Times(1);

	// given
	auto target = std::dynamic_pointer_cast<ISessionWatchdogContext_t>(createContext());

	// when
	target->requestShutdown();
}
