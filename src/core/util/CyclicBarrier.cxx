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

#include "CyclicBarrier.h"

#include <mutex>
#include <condition_variable>

using namespace core::util;

CyclicBarrier::CyclicBarrier(uint32_t numThreads)
	: mNumThreads(numThreads)
	, mWaits(0)
	, mMutex()
	, mConditionVariable()
{
}

void CyclicBarrier::await()
{
	std::unique_lock<std::mutex> lock(mMutex);

	if (++mWaits < mNumThreads)
	{
		mConditionVariable.wait(lock);
	}
	else
	{
		mConditionVariable.notify_all();
		mWaits = 0;
		lock.unlock();
	}
}
