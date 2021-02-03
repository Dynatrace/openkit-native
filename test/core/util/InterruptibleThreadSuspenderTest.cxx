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

#include "core/util/InterruptibleThreadSuspender.h"

#include <chrono>
#include <thread>
#include <atomic>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using InterruptibleThreadSuspender_t = core::util::InterruptibleThreadSuspender;

class InterruptibleThreadSuspenderTest : public testing::Test
{
};


TEST_F(InterruptibleThreadSuspenderTest, sleepSuspendsCurrentThreadForSpecifiedAmountOfTime)
{
	// given
	const int64_t sleepTimeMillis = 5;
	InterruptibleThreadSuspender_t target;

	// when
	auto before = std::chrono::steady_clock::now();
	target.sleep(sleepTimeMillis);
	auto actualSleepTime = std::chrono::steady_clock::now() - before;

	// then
	ASSERT_THAT(actualSleepTime, testing::Ge(std::chrono::milliseconds(sleepTimeMillis)));
}

TEST_F(InterruptibleThreadSuspenderTest, sleepingMultipleTimesWorks)
{
	// given
	const int64_t sleepTimeMillis = 1;
	const int32_t numSleeps = 5;
	InterruptibleThreadSuspender_t target;

	for (auto i = 0; i < numSleeps; i++)
	{
		// when
		auto before = std::chrono::steady_clock::now();
		target.sleep(sleepTimeMillis);
		auto actualSleepTime = std::chrono::steady_clock::now() - before;

		// then
		ASSERT_THAT(actualSleepTime, testing::Ge(std::chrono::milliseconds(sleepTimeMillis)));
	}
}	

TEST_F(InterruptibleThreadSuspenderTest, wakeUpInterruptsSleep)
{
	// given
	const int64_t sleepTimeInMillis = 5000;
	InterruptibleThreadSuspender_t target;
	std::atomic<int64_t> actualSleepTime(0);

	// when
	std::thread task([=, &target, &actualSleepTime] {
		auto start = std::chrono::steady_clock::now();
		target.sleep(sleepTimeInMillis);
		auto sleepTime = std::chrono::steady_clock::now() - start;
		actualSleepTime.store(std::chrono::duration_cast<std::chrono::milliseconds>(sleepTime).count());
	});
	std::this_thread::sleep_for(std::chrono::milliseconds(10)); // give task a bit time
	target.wakeup();
	task.join();

	// then
	ASSERT_THAT(actualSleepTime, testing::Lt(sleepTimeInMillis));
}

TEST_F(InterruptibleThreadSuspenderTest, sleepReturnsImmediatelyAfterWakeUpWasCalledOnce)
{
	// given
	const int64_t sleepTimeMillis = 50;
	InterruptibleThreadSuspender_t target;

	// when
	target.wakeup();
	auto start = std::chrono::steady_clock::now();
	target.sleep(sleepTimeMillis);
	auto sleptTimeMillis = std::chrono::steady_clock::now() - start;

	// then
	ASSERT_THAT(sleptTimeMillis, testing::Lt(std::chrono::milliseconds(sleepTimeMillis)));
}
