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

#include "protocol/http/HttpHeaderUtil.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>


using HeaderNameHasher_t = protocol::HeaderNameHasher;
using HeaderNameEqualityComparer_t = protocol::HeaderNameEqualityComparer;

class HttpHeaderUtilTest : public testing::Test
{
};

TEST_F(HttpHeaderUtilTest, HeaderNameHasherGivesSameHashForSameInputs)
{
	// given
	HeaderNameHasher_t target;

	// then
	ASSERT_THAT(target(""), testing::Eq(target("")));

	// then
	ASSERT_THAT(target("X-Foo"), testing::Eq(target("X-Foo")));
}

TEST_F(HttpHeaderUtilTest, HeaderNameHasherGivesSameHashForSameInputsIgnoringCase)
{
	// given
	HeaderNameHasher_t target;

	// then
	ASSERT_THAT(target("X-Foo"), testing::Eq(target("x-foo")));

	// then
	ASSERT_THAT(target("X-Foo"), testing::Eq(target("x-fOO")));
}

TEST_F(HttpHeaderUtilTest, HeaderNameHasherGivesDifferentHashForDifferentInputs)
{
	// given
	HeaderNameHasher_t target;

	// then
	ASSERT_THAT(target("X-Foo"), testing::Ne(target("X-Bar")));

	// then
	ASSERT_THAT(target("X-Foo"), testing::Ne(target("x-bar")));
}

TEST_F(HttpHeaderUtilTest, HeaderNameEqualityComparerGivesTrueForSameInputs)
{
	// given
	HeaderNameEqualityComparer_t target;

	// then
	ASSERT_THAT(target("", ""), testing::Eq(true));

	// then
	ASSERT_THAT(target("X-Foo", "X-Foo"), testing::Eq(true));
}

TEST_F(HttpHeaderUtilTest, HeaderNameEqualityComparerGivesTrueForSameInputsIgnoringCase)
{
	// given
	HeaderNameEqualityComparer_t target;

	// then
	ASSERT_THAT(target("X-Foo", "x-foo"), testing::Eq(true));

	// then
	ASSERT_THAT(target("X-Foo", "x-fOO"), testing::Eq(true));
}

TEST_F(HttpHeaderUtilTest, HeaderNameEqualityComparerGivesFalseForDifferentInputs)
{
	// given
	HeaderNameEqualityComparer_t target;

	// then
	ASSERT_THAT(target("X-Foo", "X-Bar"), testing::Eq(false));

	// then
	ASSERT_THAT(target("X-Foo", "x-bar"), testing::Eq(false));
}
