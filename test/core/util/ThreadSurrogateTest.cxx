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

#include "core/util/ThreadSurrogate.h"

#include <atomic>
#include <cstdint>
#include <future>
#include <chrono>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ThreadSurrogate_t = core::util::ThreadSurrogate;


class ThreadSurrogateTest : public testing::Test
{
public:

	void threadFunction()
	{
		mThreadCallCount.fetch_add(1);
		mPromise.set_value(UpdatedValue);
	}

protected:

	const int64_t TearDownJoinTimeout = 1000; // up to 1 sec
	const int32_t UpdatedValue = 10;

	void SetUp() override
	{
		mThreadCallCount = 0;
		mFuture = mPromise.get_future();
		mTarget.reset(new ThreadSurrogate_t());
	}

	void TearDown() override
	{
		mTarget->join(TearDownJoinTimeout);
		mTarget.reset(nullptr);
	}

	std::promise<int32_t> mPromise;
	std::future<int32_t> mFuture;
	std::atomic<int32_t> mThreadCallCount;
	std::unique_ptr<ThreadSurrogate_t> mTarget;
};

TEST_F(ThreadSurrogateTest, startExecutesGivenThreadFunction)
{
	// when
	mTarget->start(std::bind(&ThreadSurrogateTest::threadFunction, this));
	auto obtained = mFuture.wait_for(std::chrono::milliseconds(100));

	// then
	ASSERT_THAT(obtained, testing::Eq(std::future_status::ready));
	ASSERT_THAT(mFuture.get(), testing::Eq(UpdatedValue));
}

TEST_F(ThreadSurrogateTest, startReturnsTrueOnSuccess)
{
	// when
	auto obtained = mTarget->start(std::bind(&ThreadSurrogateTest::threadFunction, this));
	mFuture.wait_for(std::chrono::milliseconds(100));

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
}

TEST_F(ThreadSurrogateTest, startCanOnlyBeUsedOnce)
{
	// given
	mTarget->start(std::bind(&ThreadSurrogateTest::threadFunction, this)); // start immediately

	// when started the second time
	auto obtained = mTarget->start(std::bind(&ThreadSurrogateTest::threadFunction, this));
	mFuture.wait_for(std::chrono::milliseconds(100));

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
	ASSERT_THAT(mThreadCallCount, testing::Eq(1));
}

TEST_F(ThreadSurrogateTest, joinOnANotStartedThreadDoesNothing)
{
	// when
	auto obtained = mTarget->join(10);

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
	ASSERT_THAT(mThreadCallCount, testing::Eq(0));
}

TEST_F(ThreadSurrogateTest, joinWaitsForTheThreadToFinish)
{
	// given
	mTarget->start(std::bind(&ThreadSurrogateTest::threadFunction, this));

	// when
	auto obtained = mTarget->join(100);

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
	ASSERT_THAT(mThreadCallCount, testing::Eq(1));
}

TEST_F(ThreadSurrogateTest, joinCanBeCalledOnFinishedThread)
{
	// given
	mTarget->start(std::bind(&ThreadSurrogateTest::threadFunction, this));

	// when joining first time
	auto obtained = mTarget->join(100);

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
	ASSERT_THAT(mThreadCallCount, testing::Eq(1));

	// and when joining second time
	obtained = mTarget->join(100);

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
	ASSERT_THAT(mThreadCallCount, testing::Eq(1));
}

TEST_F(ThreadSurrogateTest, isStartedReturnsFalseIfThreadWasNotStarted)
{
	// when
	auto obtained = mTarget->isStarted();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(ThreadSurrogateTest, isStartedReturnsTrueIfThreadIsRunning)
{
	// given
	std::atomic<bool> shutdown(false);
	mTarget->start([&shutdown]
		{
			while (!shutdown)
			{
				std::this_thread::yield();
			}
		});

	// when
	auto obtained = mTarget->isStarted();
	shutdown = true;
	mTarget->join(TearDownJoinTimeout);

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
}

TEST_F(ThreadSurrogateTest, isStartedReturnsTrueIfThreadIsAlreadyTerminated)
{
	// given
	mTarget->start(std::bind(&ThreadSurrogateTest::threadFunction, this));

	// when
	mTarget->join(TearDownJoinTimeout);
	auto obtained = mTarget->isStarted();

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
}

TEST_F(ThreadSurrogateTest, isAliveReturnsFalseIfThreadWasNotStarted)
{
	// when
	auto obtained = mTarget->isAlive();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(ThreadSurrogateTest, isAliveReturnsTrueIfThreadIsRunning)
{
	// given
	std::atomic<bool> shutdown(false);
	mTarget->start([&shutdown]
		{
			while (!shutdown)
			{
				std::this_thread::yield();
			}
		});

	// when
	auto obtained = mTarget->isAlive();
	shutdown = true;
	mTarget->join(TearDownJoinTimeout);

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
}

TEST_F(ThreadSurrogateTest, isAliveReturnsFalseIfThreadIsAlreadyTerminated)
{
	// given
	mTarget->start(std::bind(&ThreadSurrogateTest::threadFunction, this));

	// when
	mTarget->join(TearDownJoinTimeout);
	auto obtained = mTarget->isAlive();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}