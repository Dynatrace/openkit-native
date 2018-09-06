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
#include "DefaultPRNGenerator.h"

#include <random>

using namespace providers;

DefaultSessionIDProvider::DefaultSessionIDProvider()
	: mLastSessionNumber(0)
	, mNextIDMutex()
{
	providers::DefaultPRNGenerator randomGenerator;
	mLastSessionNumber = randomGenerator.nextInt32(std::numeric_limits<int32_t>::max());
	
}

DefaultSessionIDProvider::DefaultSessionIDProvider(int32_t initialOffset)
	: mLastSessionNumber(initialOffset)
	, mNextIDMutex()
{
}

int32_t DefaultSessionIDProvider::getNextSessionID()
{
	std::lock_guard<std::mutex> lock(mNextIDMutex);
	if (mLastSessionNumber == std::numeric_limits<int32_t>::max())
	{
		mLastSessionNumber = 0;
	}
	return ++mLastSessionNumber;
}
