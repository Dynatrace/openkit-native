/**
* Copyright 2018 Dynatrace LLC
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

#include "providers/DefaultThreadIDProvider.h"
#include <gtest/gtest.h>

#include <thread>

using namespace providers;

class DefaultThreadIDProviderTest : public testing::Test
{
public:
	DefaultThreadIDProvider provider;
};

TEST_F(DefaultThreadIDProviderTest, currentThreadIDIsReturned)
{
	//when
	int32_t threadID = provider.getThreadID();

	int64_t expectedThreadID = std::hash<std::thread::id>()(std::this_thread::get_id());
	uint32_t lowerBits = (uint32_t)expectedThreadID;//cut the higher 32 bits away
	uint32_t higherBits = (uint32_t)(expectedThreadID >> 32);//shift the higher 32 bits to the right
	int32_t threadIDCalculated = (higherBits ^ lowerBits) & 0x7fffffff;


	ASSERT_EQ(threadID, threadIDCalculated);
}
