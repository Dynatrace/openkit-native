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

#include "DefaultTimingProvider.h"

#include <chrono>

using namespace providers;

DefaultTimingProvider::DefaultTimingProvider()
	: mReferenceTimestamp(calculateReferenceTimestamp())
{
}

int64_t DefaultTimingProvider::provideTimestampInMilliseconds()
{
	auto steadyClockSinceEpoch = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch());
	auto durationSinceEpoch = mReferenceTimestamp + steadyClockSinceEpoch;

	return std::chrono::duration_cast<std::chrono::milliseconds>(durationSinceEpoch).count();
}

int64_t DefaultTimingProvider::provideTimestampInNanoseconds()
{
	auto steadyClockSinceEpoch = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch());
	auto durationSinceEpoch = mReferenceTimestamp + steadyClockSinceEpoch;

	return std::chrono::duration_cast<std::chrono::nanoseconds>(durationSinceEpoch).count();
}

std::chrono::nanoseconds DefaultTimingProvider::calculateReferenceTimestamp()
{
	auto wallClockSinceEpoch = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
	auto steadyClockSinceEpoch = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch());

	return wallClockSinceEpoch - steadyClockSinceEpoch;
}