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

#ifndef _PROVIDERS_IPRNGENERATOR_H
#define _PROVIDERS_IPRNGENERATOR_H

#include <cstdint>

namespace providers
{
	///
	/// Interface providing pseudo random numbers
	///
	class IPRNGenerator
	{
	public:

		///
		/// Destructor
		///
		virtual ~IPRNGenerator() = default;

		///
		/// Provide a positive random 32-bit number between 0(inclusive) and std::numeric_limits<int32_t>::max() (exclusive).
		///
		virtual int32_t nextPositiveInt32() = 0;

		///
		/// Provide a positive random 64-bit number between 0(inclusive) and std::numeric_limits<int64_t>::max() (exclusive).
		///
		virtual int64_t nextPositiveInt64() = 0;
	};
}

#endif