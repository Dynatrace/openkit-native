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

#include "DefaultPRNGenerator.h"


using namespace providers;

DefaultPRNGenerator::DefaultPRNGenerator()
	: mRandomEngine((std::random_device())())
{
}

int32_t DefaultPRNGenerator::nextInt32(int32_t upperBound)
{
	std::uniform_real_distribution<double> uniform_dist(0.0, 1.0);
	return static_cast<int32_t>(uniform_dist(mRandomEngine) * upperBound);
}

int64_t DefaultPRNGenerator::nextInt64(int64_t upperBound)
{
	std::uniform_real_distribution<double> uniform_dist(0.0, 1.0);
	return static_cast<int64_t>(uniform_dist(mRandomEngine) * upperBound);
}