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
		virtual ~IPRNGenerator() {}

		///
		/// Generate a random number between 0(inclusive) and upperBound(exclusive)
		/// @param[in] upperBound the upper boundary used for random number generation, is exclusive
		/// @return random number within the requested boundaries
		///
		virtual int32_t nextInt32(int32_t upperBound) = 0;

		///
		/// Generate a random number between 0(inclusive) and upperBound(exclusive)
		/// @param[in] upperBound the upper boundary used for random number generation, is exclusive
		/// @return random number within the requested boundaries
		///
		virtual int64_t nextInt64(int64_t upperBound) = 0;
	};
}

#endif