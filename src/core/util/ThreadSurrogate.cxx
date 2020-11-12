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

#include "ThreadSurrogate.h"

#include <chrono>

using namespace core::util;

ThreadSurrogate::ThreadSurrogate()
	: mMutex()
	, mStopConditionVariable()
	, mIsThreadStopped(false)
	, mThread(nullptr)
{
}

bool ThreadSurrogate::start(const ThreadFunction& threadFunction)
{
	std::unique_lock<std::mutex> lock(mMutex);

	if (mThread != nullptr)
	{
		// start was called before
		return false;
	}

	mThread.reset(new std::thread(&ThreadSurrogate::threadWrapperFunction, this, threadFunction));
	mThread->detach(); // daemonize thread

	return true;
}

bool ThreadSurrogate::join(int64_t waitTimeInMillis)
{
	std::unique_lock<std::mutex> lock(mMutex);

	if (mThread == nullptr)
	{
		// start was not called so far
		return true;
	}

	auto waitUntil = std::chrono::steady_clock::now() + std::chrono::milliseconds(waitTimeInMillis);
	while (!mIsThreadStopped && std::chrono::steady_clock::now() < waitUntil)
	{
		mStopConditionVariable.wait_until(lock, waitUntil);
	}

	return mIsThreadStopped;
}

bool ThreadSurrogate::isStarted() const
{
	std::unique_lock<std::mutex> lock(mMutex);
	
	return mThread != nullptr;
}

bool ThreadSurrogate::isAlive() const
{
	std::unique_lock<std::mutex> lock(mMutex);

	return mThread != nullptr && !mIsThreadStopped;
}

void ThreadSurrogate::threadWrapperFunction(const ThreadFunction& threadFunction)
{
	// execute real thread function
	threadFunction();

	{ // thread is about to exit
		std::unique_lock<std::mutex> lock(mMutex);

		mIsThreadStopped = true;
		mStopConditionVariable.notify_all();
	}
}
