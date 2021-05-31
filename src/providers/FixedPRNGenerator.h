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

#ifndef _PROVIDERS_FIXEDPRNGENERATOR_H
#define _PROVIDERS_FIXEDPRNGENERATOR_H

#include "IPRNGenerator.h"

#include <memory>

namespace providers
{

	///
	/// Implementation of IPRNGenerator providing fixed random numbers.
	///
	class FixedPRNGenerator : public IPRNGenerator
	{
	public:

		FixedPRNGenerator(std::shared_ptr<IPRNGenerator> rng);

		~FixedPRNGenerator() override = default;

		int64_t nextPositiveInt64() override;

		int32_t nextPercentageValue() override;

	private:
		
		int32_t mRandomPercentage;

		int64_t mRandomInt64Number;
	};
}

#endif
