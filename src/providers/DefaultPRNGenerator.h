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

#ifndef _PROVIDERS_DEFAULTPRNGENERATOR_H
#define _PROVIDERS_DEFAULTPRNGENERATOR_H

#include "IPRNGenerator.h"

#include <random>

namespace providers
{
	
	///
	/// Default implementation for random number generator based on STL's random::device
	///
	class DefaultPRNGenerator : public IPRNGenerator
	{
	public:

		///
		/// Default constructor
		///
		DefaultPRNGenerator();

		virtual int32_t nextInt32(int32_t upperBound) override;

		virtual int64_t nextInt64(int64_t upperBound) override;

	private:
		/// random engine generating the random numbers
		std::default_random_engine mRandomEngine;

	};
}

#endif