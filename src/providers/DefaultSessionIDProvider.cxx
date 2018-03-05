/**
* Copyright 2018 Dynatrace LLC
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

#include "DefaultSessionIDProvider.h"

#include <random>

using namespace providers;

DefaultSessionIDProvider::DefaultSessionIDProvider()
	: mLastSessionNumber(0)
	, mNextIDMutex()
{
	// Seed with a real random value, if available

	std::default_random_engine e1((std::random_device())());
	std::uniform_int_distribution<int32_t> uniform_dist(0, INT32_MAX);
	mLastSessionNumber = uniform_dist(e1);
	
}

DefaultSessionIDProvider::DefaultSessionIDProvider(int32_t initialOffset)
	: mLastSessionNumber(initialOffset)
	, mNextIDMutex()
{
}

int32_t DefaultSessionIDProvider::getNextSessionID()
{
	std::lock_guard<std::mutex> lock(mNextIDMutex);
	if (mLastSessionNumber == INT32_MAX)
	{
		mLastSessionNumber = 0;
	}
	return ++mLastSessionNumber;
}
