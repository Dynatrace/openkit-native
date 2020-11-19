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

#include "BeaconKey.h"

#include <type_traits>

using namespace core::caching;

///
/// hashCombine method was borrowed from boost.org boost::hash_detail::hash_combine_impl
///
static void hashCombine(size_t& seed, size_t value)
{
	seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

BeaconKey::BeaconKey(int32_t beaconId, int32_t beaconSequenceNumber)
	: mBeaconId(beaconId)
	, mBeaconSequenceNumber(beaconSequenceNumber)
{
}

int32_t BeaconKey::getBeaconId() const
{
	return mBeaconId;
}

int32_t BeaconKey::getBeaconSequenceNumber() const
{
	return mBeaconSequenceNumber;
}

bool BeaconKey::operator == (const BeaconKey& rhs) const
{
	return getBeaconId() == rhs.getBeaconId()
		&& getBeaconSequenceNumber() == rhs.getBeaconSequenceNumber();
}

bool BeaconKey::operator != (const BeaconKey& rhs) const
{
	return !operator==(rhs);
}

size_t BeaconKey::Hash::operator()(const BeaconKey& beaconKey) const noexcept
{
	size_t hash = 0;
	hashCombine(hash, beaconKey.getBeaconId());
	hashCombine(hash, beaconKey.getBeaconSequenceNumber());

	return hash;
}
