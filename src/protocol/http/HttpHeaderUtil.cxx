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

#include "HttpHeaderUtil.h"

#include <locale>
#include <cctype>
#include <cstdint>

using namespace protocol;

// following two values are required by HttpHeaderCollection::HeaderNameHasher
constexpr std::size_t FNV_OFFSET_BASIS = std::size_t(0xcbf29ce484222325);
constexpr std::size_t FNV_PRIME = std::size_t(0x100000001b3);

std::size_t HeaderNameHasher::operator()(const std::string& name) const
{
	// calculate the hash using the FNV-1a algorithm (Fowler-Noll-Vo hash function)
	// the same algorithm is used by MS for hashing strings.
	// The algorithm is duplicated on purpose, as we need case-insensitive hashing
	// and converting the whole input to lower case might require temporary heap memory

	auto hash = FNV_OFFSET_BASIS;
	for (auto c : name)
	{
		hash = hash ^ std::tolower(c, std::locale::classic());
		hash = hash * FNV_PRIME;
	}

	return hash;
}

bool HeaderNameEqualityComparer::operator()(const std::string& lhs, const std::string& rhs) const
{
	return ((lhs.size() == rhs.size())
		&& std::equal(lhs.begin(), lhs.end(), rhs.begin(), [](const char& c1, const char& c2) {
			return (c1 == c2 || std::tolower(c1, std::locale::classic()) == std::tolower(c2, std::locale::classic()));
			}));
}
