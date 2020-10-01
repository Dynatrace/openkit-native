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

#include "InterruptibleThreadSuspender.h"

#include <chrono>

using namespace core::util;


InterruptibleThreadSuspender::InterruptibleThreadSuspender()
	: mSignaled(false)
	, mMutex()
	, mConditionVariable()
{
}

void InterruptibleThreadSuspender::sleep(int64_t millis)
{
	auto waitUntil = std::chrono::steady_clock::now() + std::chrono::milliseconds(millis);

	std::unique_lock<std::mutex> lock(mMutex);

	while (!mSignaled && std::chrono::steady_clock::now() < waitUntil)
	{
		mConditionVariable.wait_until(lock, waitUntil);
	}
}

void InterruptibleThreadSuspender::wakeup()
{
	std::unique_lock<std::mutex> lock(mMutex);

	mSignaled = true;
	mConditionVariable.notify_all();
}