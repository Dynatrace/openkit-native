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

#include "core/caching/BeaconKey.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using BeaconKey_t = core::caching::BeaconKey;

class BeaconKeyTest : public testing::Test
{
};

TEST_F(BeaconKeyTest, twoInstancesWithSameValuesAreEqual)
{
	// given
	BeaconKey_t keyOne(17, 18);
	BeaconKey_t keyTwo(17, 18);

	// when
	auto obtained = keyOne == keyTwo;

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
}

TEST_F(BeaconKeyTest, twoInstancesWithSameValuesAreNotInequal)
{
	// given
	BeaconKey_t keyOne(17, 18);
	BeaconKey_t keyTwo(17, 18);

	// when
	auto obtained = keyOne != keyTwo;

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(BeaconKeyTest, twoInstancesWithSameValuesHaveSameHash)
{
	// given
	BeaconKey_t keyOne(17, 18);
	BeaconKey_t keyTwo(17, 18);

	// then
	ASSERT_THAT(BeaconKey_t::Hash()(keyOne), testing::Eq(BeaconKey_t::Hash()(keyTwo)));
}

TEST_F(BeaconKeyTest, twoInstancesWithDifferentBeaconIdAreNotEqual)
{
	// given
	BeaconKey_t keyOne(37, 18);
	BeaconKey_t keyTwo(38, 18);

	// when
	auto obtained = keyOne == keyTwo;

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(BeaconKeyTest, twoInstancesWithDifferentBeaconIdAreInequal)
{
	// given
	BeaconKey_t keyOne(37, 18);
	BeaconKey_t keyTwo(38, 18);

	// when
	auto obtained = keyOne != keyTwo;

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
}

TEST_F(BeaconKeyTest, twoInstancesWithDifferentBeaconIdHaveDifferentHash)
{
	// given
	BeaconKey_t keyOne(37, 18);
	BeaconKey_t keyTwo(38, 18);

	// then
	ASSERT_THAT(BeaconKey_t::Hash()(keyOne), testing::Ne(BeaconKey_t::Hash()(keyTwo)));
}

TEST_F(BeaconKeyTest, twoInstancesWithDifferentBeaconSequenceNumberAreNotEqual)
{
	// given
	BeaconKey_t keyOne(17, 37);
	BeaconKey_t keyTwo(17, 73);

	// when
	auto obtained = keyOne == keyTwo;

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(BeaconKeyTest, twoInstancesWithDifferentBeaconSequenceNumberAreInequal)
{
	// given
	BeaconKey_t keyOne(17, 37);
	BeaconKey_t keyTwo(17, 73);

	// when
	auto obtained = keyOne != keyTwo;

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
}

TEST_F(BeaconKeyTest, twoInstancesWithDifferentBeaconSequenceNumberHaveDifferentHash)
{
	// given
	BeaconKey_t keyOne(17, 37);
	BeaconKey_t keyTwo(17, 73);

	// then
	ASSERT_THAT(BeaconKey_t::Hash()(keyOne), testing::Ne(BeaconKey_t::Hash()(keyTwo)));
}
