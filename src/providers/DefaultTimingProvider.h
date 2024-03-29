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

#ifndef _PROVIDERS_DEFAULTTIMINGPROVIDER_H
#define _PROVIDERS_DEFAULTTIMINGPROVIDER_H

#include "ITimingProvider.h"

#include <chrono>

namespace providers
{

	///
	/// Default implementation for timing provider
	///
	class DefaultTimingProvider : public ITimingProvider
	{
	public:

		DefaultTimingProvider();

		~DefaultTimingProvider() override = default;

		///
		/// Provide the current timestamp in milliseconds.
		/// @returns the current timestamp
		///
		int64_t provideTimestampInMilliseconds() override;

		///
		/// Provide the current timestamp in nanoseconds.
		/// @returns the current timestamp
		///
		int64_t provideTimestampInNanoseconds() override;

	private:

		static std::chrono::nanoseconds calculateReferenceTimestamp();

		const std::chrono::nanoseconds mReferenceTimestamp;
	};
}

#endif