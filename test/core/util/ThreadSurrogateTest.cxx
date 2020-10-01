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
		mPromise.set_value_at_thread_exit(UpdatedValue);
	}

protected:

	const int32_t UpdatedValue = 10;

	void SetUp() override
	{
		mThreadCallCount.store(0);
		mFuture = mPromise.get_future();
	}

	std::promise<int32_t> mPromise;
	std::future<int32_t> mFuture;
	std::atomic<int32_t> mThreadCallCount;
};

TEST_F(ThreadSurrogateTest, startExecutesGivenThreadFunction)
{
	// given
	ThreadSurrogate_t target;

	// when
	target.start(std::bind(&ThreadSurrogateTest::threadFunction, this));
	auto obtained = mFuture.wait_for(std::chrono::milliseconds(100));

	// then
	ASSERT_THAT(obtained, testing::Eq(std::future_status::ready));
	ASSERT_THAT(mFuture.get(), testing::Eq(UpdatedValue));
}

TEST_F(ThreadSurrogateTest, startReturnsTrueOnSuccess)
{
	// given
	ThreadSurrogate_t target;

	// when
	auto obtained = target.start(std::bind(&ThreadSurrogateTest::threadFunction, this));
	mFuture.wait_for(std::chrono::milliseconds(100));

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
}

TEST_F(ThreadSurrogateTest, startCanOnlyBeUsedOnce)
{
	// given
	ThreadSurrogate_t target;
	target.start(std::bind(&ThreadSurrogateTest::threadFunction, this)); // start immediately

	// when started the second time
	auto obtained = target.start(std::bind(&ThreadSurrogateTest::threadFunction, this));
	mFuture.wait_for(std::chrono::milliseconds(100));

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
	ASSERT_THAT(mThreadCallCount, testing::Eq(1));
}

TEST_F(ThreadSurrogateTest, joinOnANotStartedThreadDoesNothing)
{
	// given
	ThreadSurrogate_t target;

	// when
	auto obtained = target.join(10);

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
	ASSERT_THAT(mThreadCallCount, testing::Eq(0));
}

TEST_F(ThreadSurrogateTest, joinWaitsForTheThreadToFinish)
{
	// given
	ThreadSurrogate_t target;
	target.start(std::bind(&ThreadSurrogateTest::threadFunction, this));

	// when
	auto obtained = target.join(100);

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
	ASSERT_THAT(mThreadCallCount, testing::Eq(1));
}

TEST_F(ThreadSurrogateTest, joinCanBeCalledOnFinishedThread)
{
	// given
	ThreadSurrogate_t target;
	target.start(std::bind(&ThreadSurrogateTest::threadFunction, this));

	// when joining first time
	auto obtained = target.join(100);

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
	ASSERT_THAT(mThreadCallCount, testing::Eq(1));

	// and when joining second time
	obtained = target.join(100);

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
	ASSERT_THAT(mThreadCallCount, testing::Eq(1));
}

TEST_F(ThreadSurrogateTest, isStartedReturnsFalseIfThreadWasNotStarted)
{
	// given
	ThreadSurrogate_t target;

	// when
	auto obtained = target.isStarted();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(ThreadSurrogateTest, isStartedReturnsTrueIfThreadIsRunning)
{
	// given
	std::atomic<bool> shutdown(false);
	ThreadSurrogate_t target;
	target.start([&shutdown]
		{
			while (!shutdown)
			{
				std::this_thread::yield();
			}
		});

	// when
	auto obtained = target.isStarted();
	shutdown = true;
	target.join(100);

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
}

TEST_F(ThreadSurrogateTest, isStartedReturnsTrueIfThreadIsAlreadyTerminated)
{
	// given
	ThreadSurrogate_t target;
	target.start(std::bind(&ThreadSurrogateTest::threadFunction, this));

	// when
	target.join(100);
	auto obtained = target.isStarted();

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
}

TEST_F(ThreadSurrogateTest, isAliveReturnsFalseIfThreadWasNotStarted)
{
	// given
	ThreadSurrogate_t target;

	// when
	auto obtained = target.isAlive();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(ThreadSurrogateTest, isAliveReturnsTrueIfThreadIsRunning)
{
	// given
	std::atomic<bool> shutdown(false);
	ThreadSurrogate_t target;
	target.start([&shutdown]
		{
			while (!shutdown)
			{
				std::this_thread::yield();
			}
		});

	// when
	auto obtained = target.isAlive();
	shutdown = true;
	target.join(100);

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
}

TEST_F(ThreadSurrogateTest, isAliveReturnsFalseIfThreadIsAlreadyTerminated)
{
	// given
	ThreadSurrogate_t target;
	target.start(std::bind(&ThreadSurrogateTest::threadFunction, this));

	// when
	target.join(100);
	auto obtained = target.isAlive();

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}