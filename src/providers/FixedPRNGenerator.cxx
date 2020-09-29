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

#include "FixedPRNGenerator.h"

using namespace providers;

FixedPRNGenerator::FixedPRNGenerator(std::shared_ptr<IPRNGenerator> rng)
	: mRandomInt32Number(rng->nextPositiveInt32())
	, mRandomInt64Number(rng->nextPositiveInt64())
{
}

int32_t FixedPRNGenerator::nextPositiveInt32()
{
	return mRandomInt32Number;
}

int64_t FixedPRNGenerator::nextPositiveInt64()
{
	return mRandomInt64Number;
}
