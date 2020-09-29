/**
 * Copyright 2018-2020 Dynatrace LLC
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
	: mRandomEngine((std::random_device())())
{
}

int32_t DefaultPRNGenerator::nextPositiveInt32()
{
	std::uniform_real_distribution<double> uniform_dist(0.0, 1.0);
	return static_cast<int32_t>(uniform_dist(mRandomEngine) * std::numeric_limits<int32_t>::max());
}

int64_t DefaultPRNGenerator::nextPositiveInt64()
{
	std::uniform_real_distribution<double> uniform_dist(0.0, 1.0);
	return static_cast<int64_t>(uniform_dist(mRandomEngine) * std::numeric_limits<int64_t>::max());
}