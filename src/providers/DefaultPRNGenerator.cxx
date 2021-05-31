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

#include "DefaultPRNGenerator.h"

#include <limits>

using namespace providers;

DefaultPRNGenerator::DefaultPRNGenerator()
	: mRandomEngine(std::random_device()())
{
}

int64_t DefaultPRNGenerator::nextPositiveInt64()
{
	std::uniform_int_distribution<int64_t> uniform_dist(0);
	return uniform_dist(mRandomEngine);
}

int32_t DefaultPRNGenerator::nextPercentageValue()
{
	// uniform_int_distribution uses closed intervals
	std::uniform_int_distribution<int32_t> uniform_dist(0, 99);
	return uniform_dist(mRandomEngine);
}