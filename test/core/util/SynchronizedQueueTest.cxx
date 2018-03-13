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

#include "core/util/SynchronizedQueue.h"

#include <gtest/gtest.h>

using namespace core::util;

class SynchronizedQueueTest : public testing::Test
{
public:
	std::shared_ptr<int32_t> elementOne = std::make_shared<int32_t>(1);
	std::shared_ptr<int32_t> elementTwo = std::make_shared<int32_t>(1);
	SynchronizedQueue<std::shared_ptr<int32_t>> synchronizedQueue;
};

TEST_F(SynchronizedQueueTest, elementCanBeAdded)
{
	//when
	synchronizedQueue.put(elementOne);

	//then
	ASSERT_FALSE(synchronizedQueue.isEmpty());
}

TEST_F(SynchronizedQueueTest, elementCanBeAddedAndRemoved)
{
	//when
	synchronizedQueue.put(elementOne);

	//then
	ASSERT_FALSE(synchronizedQueue.isEmpty());

	//when
	synchronizedQueue.remove(elementOne);

	//then
	ASSERT_TRUE(synchronizedQueue.isEmpty());
}

TEST_F(SynchronizedQueueTest, onlyFirstOccurenceOfAnElementValueIsDeleted)
{
	//when
	synchronizedQueue.put(elementOne);
	synchronizedQueue.put(elementTwo);

	//then
	ASSERT_FALSE(synchronizedQueue.isEmpty());

	//when
	synchronizedQueue.remove(elementOne);

	//then
	ASSERT_FALSE(synchronizedQueue.isEmpty());

	//when
	auto list = synchronizedQueue.toStdVector();

	//then
	ASSERT_GT(list.size(),0u);
	ASSERT_EQ(list[0], elementTwo);
}

TEST_F(SynchronizedQueueTest, getCalledOnEmptyList)
{
	auto element = synchronizedQueue.get();
	ASSERT_EQ(element, nullptr);
}

TEST_F(SynchronizedQueueTest, removeCalledOnEmptyList)
{
	//when
	synchronizedQueue.remove(elementTwo);

	ASSERT_TRUE(synchronizedQueue.isEmpty());
}

TEST_F(SynchronizedQueueTest, checkEmptyList)
{
	ASSERT_TRUE(synchronizedQueue.isEmpty());
}