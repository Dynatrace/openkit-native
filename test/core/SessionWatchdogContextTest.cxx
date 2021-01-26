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

#include "../core/objects/mock/MockSessionInternals.h"
#include "../core/objects/mock/MockISessionProxy.h"
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
using MockISessionProxy_sp = std::shared_ptr<MockISessionProxy>;
using ISessionWatchdogContext_t = core::ISessionWatchdogContext;
using SessionWatchdogContext_t = core::SessionWatchdogContext;
using SessionWatchdogContext_sp = std::shared_ptr< SessionWatchdogContext_t>;

class SessionWatchdogContextTest : public testing::Test
{
protected:

	MockITimingProvider_sp mockTimingProvider;
	MockIInterruptibleThreadSuspender_sp mockThreadSuspender;
	MockSessionInternals_sp mockSession;
	MockISessionProxy_sp mockSessionProxy;
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

		mockSessionProxy = MockISessionProxy::createNice();
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

TEST_F(SessionWatchdogContextTest, onDefaultSessionsToSplitByTimeoutIsEmpty)
{
	// given
	auto target = createContext();

	// then
	ASSERT_THAT(target->getSessionsToSplitByTimeout(), testing::IsEmpty());
}

TEST_F(SessionWatchdogContextTest, closeOrEnqueueForClosingDoesNotAddSessionIfItCanBeClosed)
{
	// expect
	EXPECT_CALL(*mockSession, tryEnd())
		.Times(1)
		.WillRepeatedly(testing::Return(true));

	// given
	auto target = createContext();

	// when
	target->closeOrEnqueueForClosing(mockSession, 0);

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

	// when
	target->closeOrEnqueueForClosing(mockSession, 17);

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

	// when
	target->closeOrEnqueueForClosing(mockSession, gracePeriod);
}

TEST_F(SessionWatchdogContextTest, dequeueFromClosingRemovesSession)
{
	// given
	auto target = createContext();
	target->closeOrEnqueueForClosing(mockSession, 0);
	ASSERT_THAT(target->getSessionsToClose().size(), testing::Eq(1));

	// when
	target->dequeueFromClosing(mockSession);

	// then
	ASSERT_THAT(target->getSessionsToClose().size(), testing::Eq(0));
}

TEST_F(SessionWatchdogContextTest, addToSplitByTimeOutAddsSessionProxyIfNotFinished)
{
	// given
	ON_CALL(*mockSessionProxy, isFinished())
		.WillByDefault(testing::Return(false));

	auto target = createContext();

	// when
	target->addToSplitByTimeout(mockSessionProxy);

	// then
	ASSERT_THAT(target->getSessionsToSplitByTimeout().size(), testing::Eq(1));
}

TEST_F(SessionWatchdogContextTest, addToSplitByTimeOutDoesNotAddSessionProxyIfFinished)
{
	// given
	ON_CALL(*mockSessionProxy, isFinished())
		.WillByDefault(testing::Return(true));

	auto target = createContext();

	// when
	target->addToSplitByTimeout(mockSessionProxy);

	// then
	ASSERT_THAT(target->getSessionsToSplitByTimeout(), testing::IsEmpty());
}

TEST_F(SessionWatchdogContextTest, removeFromSplitByTimeoutRemovesSessionProxy)
{
	// given
	auto target = createContext();
	target->addToSplitByTimeout(mockSessionProxy);

	ASSERT_THAT(target->getSessionsToSplitByTimeout().size(), testing::Eq(1));

	// when
	target->removeFromSplitByTimeout(mockSessionProxy);

	// then
	ASSERT_THAT(target->getSessionsToSplitByTimeout(), testing::IsEmpty());
}

TEST_F(SessionWatchdogContextTest, executeEndsSessionsWithExpiredGracePeriod)
{
	// expect
	EXPECT_CALL(*mockSession, end(false))
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

	target->closeOrEnqueueForClosing(mockSession, 4);

	// when
	target->execute();

	// then
	ASSERT_THAT(target->getSessionsToClose().size(), testing::Eq(0));
}

TEST_F(SessionWatchdogContextTest, executeEndsSessionsWithGraceEndTimeSameAsCurrentTime)
{
	// expect
	EXPECT_CALL(*mockSession, end(false))
		.Times(1);

	// given
	ON_CALL(*mockTimingProvider, provideTimestampInMilliseconds())
		.WillByDefault(testing::Return(5));
	ON_CALL(*mockSession, tryEnd())
		.WillByDefault(testing::Return(false));

	auto target = createContext();

	target->closeOrEnqueueForClosing(mockSession, 0);

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
	EXPECT_CALL(*mockSession, end(false))
		.Times(0);
	EXPECT_CALL(*mockSession, setSplitByEventsGracePeriodEndTimeInMillis(now + gracePeriod))
		.Times(1);

	// given
	ON_CALL(*mockTimingProvider, provideTimestampInMilliseconds())
		.WillByDefault(testing::Return(now));
	ON_CALL(*mockSession, tryEnd())
		.WillByDefault(testing::Return(false));

	auto target = createContext();

	target->closeOrEnqueueForClosing(mockSession, gracePeriod); // close at now + gracePeriod

	// when
	target->execute();

	// then
	ASSERT_THAT(target->getSessionsToClose().size(), testing::Eq(1));
}

TEST_F(SessionWatchdogContextTest, executeSleepsDefaultTimeIfSessionIsExpiredAndNoFurtherNonExpiredSessions)
{
	// expect
	EXPECT_CALL(*mockSession, end(false))
		.Times(1);
	EXPECT_CALL(*mockThreadSuspender, sleep(SessionWatchdogContext_t::getDefaultSleepTime().count()))
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
	EXPECT_CALL(*mockSession, end(testing::_))
		.Times(0);
	EXPECT_CALL(*mockSession1, end(testing::_))
		.Times(0);
	EXPECT_CALL(*mockSession2, end(testing::_))
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

TEST_F(SessionWatchdogContextTest, executeRemovesSessionProxyIfNextSplitTimeIsNegative)
{
	// expect
	EXPECT_CALL(*mockSessionProxy, splitSessionByTime())
		.Times(1)
		.WillOnce(testing::Return(-1));

	// given
	auto target = createContext();
	target->addToSplitByTimeout(mockSessionProxy);

	// when
	target->execute();

	// then
	ASSERT_THAT(target->getSessionsToSplitByTimeout(), testing::IsEmpty());
}

TEST_F(SessionWatchdogContextTest, executeDoesNotRemoveSessionProxyIfNextSplitTimeIsNegative)
{
	// expect
	EXPECT_CALL(*mockSessionProxy, splitSessionByTime())
		.Times(1)
		.WillOnce(testing::Return(10));

	// given
	auto target = createContext();
	target->addToSplitByTimeout(mockSessionProxy);

	// when
	target->execute();

	// then
	ASSERT_THAT(target->getSessionsToSplitByTimeout().size(), testing::Eq(1));
}

TEST_F(SessionWatchdogContextTest, executeSleepsDefaultTimeIfSessionProxySplitTimeIsNegativeAndNoFurtherSessionProxyExists)
{
	// expect
	EXPECT_CALL(*mockSessionProxy, splitSessionByTime())
		.Times(1)
		.WillOnce(testing::Return(-1));
	EXPECT_CALL(*mockThreadSuspender, sleep(SessionWatchdogContext_t::getDefaultSleepTime().count()))
		.Times(1);

	// given
	auto target = createContext();
	target->addToSplitByTimeout(mockSessionProxy);

	// when
	target->execute();
}

TEST_F(SessionWatchdogContextTest, executeSleepsDefaultTimeIfSleepDurationToNextSplitIsNegativeAndNoFurtherSessionProxyExists)
{
	// expect
	EXPECT_CALL(*mockSessionProxy, splitSessionByTime())
		.Times(1)
		.WillOnce(testing::Return(10));
	EXPECT_CALL(*mockThreadSuspender, sleep(SessionWatchdogContext_t::getDefaultSleepTime().count()))
		.Times(1);

	// given
	ON_CALL(*mockTimingProvider, provideTimestampInMilliseconds())
		.WillByDefault(testing::Return(20));

	auto target = createContext();
	target->addToSplitByTimeout(mockSessionProxy);

	// when
	target->execute();
}

TEST_F(SessionWatchdogContextTest, executeSleepsDurationToNextSplitByTimeout)
{
	// with
	const int64_t nextSplitTime = 100;
	const int64_t currentTime = 50;

	// expect
	EXPECT_CALL(*mockSessionProxy, splitSessionByTime())
		.Times(1)
		.WillOnce(testing::Return(nextSplitTime));
	EXPECT_CALL(*mockThreadSuspender, sleep(nextSplitTime - currentTime))
		.Times(1);

	// given
	ON_CALL(*mockTimingProvider, provideTimestampInMilliseconds())
		.WillByDefault(testing::Return(currentTime));

	auto target = createContext();
	target->addToSplitByTimeout(mockSessionProxy);

	// when
	target->execute();
}

TEST_F(SessionWatchdogContextTest, executeDoesNotSleepLongerThanDefaultSleepTimeForDurationToNextSplitByTime)
{
	// with
	auto nextSplitTime = SessionWatchdogContext_t::getDefaultSleepTime().count() + 20;
	const int64_t currentTime = 5;

	// expect
	EXPECT_CALL(*mockSessionProxy, splitSessionByTime())
		.Times(1)
		.WillOnce(testing::Return(nextSplitTime));
	EXPECT_CALL(*mockThreadSuspender, sleep(SessionWatchdogContext_t::getDefaultSleepTime().count()))
		.Times(1);

	// given
	ON_CALL(*mockTimingProvider, provideTimestampInMilliseconds())
		.WillByDefault(testing::Return(currentTime));

	auto target = createContext();
	target->addToSplitByTimeout(mockSessionProxy);

	// when
	target->execute();
}

TEST_F(SessionWatchdogContextTest, executeSleepsMinimumTimeToNextSplitByTime)
{
	// with
	const int64_t nextSplitTimeProxy1 = 120;
	const int64_t nextSplitTimeProxy2 = 100;
	const int64_t currentTime = 50;

	auto mockSessionProxy2 = MockISessionProxy::createNice();

	// expect
	EXPECT_CALL(*mockSessionProxy, splitSessionByTime())
		.Times(1)
		.WillOnce(testing::Return(nextSplitTimeProxy1));
	EXPECT_CALL(*mockSessionProxy2, splitSessionByTime())
		.Times(1)
		.WillOnce(testing::Return(nextSplitTimeProxy2));
	EXPECT_CALL(*mockThreadSuspender, sleep(nextSplitTimeProxy2 - currentTime))
		.Times(1);

	// given
	ON_CALL(*mockTimingProvider, provideTimestampInMilliseconds())
		.WillByDefault(testing::Return(currentTime));

	auto target = createContext();
	target->addToSplitByTimeout(mockSessionProxy);
	target->addToSplitByTimeout(mockSessionProxy2);

	// when
	target->execute();
}

TEST_F(SessionWatchdogContextTest, executeSleepsDefaultTimeIfNoSessionToCloseAndNoSessionProxyToSplitExists)
{
	// expect
	EXPECT_CALL(*mockThreadSuspender, sleep(SessionWatchdogContext_t::getDefaultSleepTime().count()))
		.Times(1);

	// given
	auto target = createContext();

	// when
	target->execute();
}

TEST_F(SessionWatchdogContextTest, executeSleepsMinimumDurationToNextSplitByTime)
{
	// with
	const int64_t gracePeriod = 150;
	const int64_t nextSessionProxySplitTime = 100;
	const int64_t currentTime = 50;

	// expect
	EXPECT_CALL(*mockSessionProxy, splitSessionByTime())
		.Times(1)
		.WillOnce(testing::Return(nextSessionProxySplitTime));
	EXPECT_CALL(*mockSession, end(testing::_))
		.Times(0);
	EXPECT_CALL(*mockThreadSuspender, sleep(nextSessionProxySplitTime - currentTime))
		.Times(1);

	// given
	ON_CALL(*mockTimingProvider, provideTimestampInMilliseconds())
		.WillByDefault(testing::Return(currentTime));

	auto target = createContext();
	target->closeOrEnqueueForClosing(mockSession, gracePeriod);
	target->addToSplitByTimeout(mockSessionProxy);

	// when
	target->execute();
}

TEST_F(SessionWatchdogContextTest, executeSleepsMinimumDurationToNextGracePeriodEnd)
{
	// with
	const int64_t gracePeriod = 100;
	const int64_t nextSessionProxySplitTime = 200;

	// expect
	EXPECT_CALL(*mockSessionProxy, splitSessionByTime())
		.Times(1)
		.WillOnce(testing::Return(nextSessionProxySplitTime));
	EXPECT_CALL(*mockSession, end(testing::_))
		.Times(0);
	EXPECT_CALL(*mockThreadSuspender, sleep(gracePeriod))
		.Times(1);

	auto target = createContext();
	target->closeOrEnqueueForClosing(mockSession, gracePeriod);
	target->addToSplitByTimeout(mockSessionProxy);

	// when
	target->execute();
}

TEST_F(SessionWatchdogContextTest, executeDoesNotSleepLongerThanDefaultSleepTimeForDurationToNextSessionClose)
{
	// expect
	EXPECT_CALL(*mockThreadSuspender, sleep(SessionWatchdogContext_t::getDefaultSleepTime().count()))
		.Times(1);

	// given
	ON_CALL(*mockSession, tryEnd())
		.WillByDefault(testing::Return(false));

	auto target = createContext();
	target->closeOrEnqueueForClosing(mockSession, SessionWatchdogContext_t::getDefaultSleepTime().count() + 10);

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
