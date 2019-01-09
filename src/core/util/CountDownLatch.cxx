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

#include "CountDownLatch.h"

using namespace core::util;

CountDownLatch::CountDownLatch(uint32_t initial)
	: mCount(initial)
	, mMutex()
	, mConditionVariable()
{
}

void CountDownLatch::countDown()
{
	std::unique_lock<std::mutex> lock(mMutex);

	mCount--;
	if (mCount == 0)
	{
		mConditionVariable.notify_all();
	}

}

void CountDownLatch::await()
{
	std::unique_lock<std::mutex> lock(mMutex);

	while (mCount > 0)
	{
		mConditionVariable.wait(lock);
	}
}

void CountDownLatch::await(int64_t milliseconds)
{
	std::unique_lock<std::mutex> lock(mMutex);
	auto start = std::chrono::system_clock::now();

	auto current = std::chrono::system_clock::now();
	while (mCount > 0 && (current - start).count() < milliseconds)
	{
		mConditionVariable.wait_for(lock, std::chrono::milliseconds(milliseconds));
		current = std::chrono::system_clock::now();
	}
}