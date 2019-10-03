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
#include "Types.h"
#include "../Types.h"

#include <gtest/gtest.h>
#include <cstdint>
#include <memory>

using namespace test::types;

class URLEncodingTest : public testing::Test
{
};


TEST_F(URLEncodingTest, urlEncodeQueryParameterWithSpacesAndEqualsSign)
{
	Utf8String_t s("q=greater than 5");
	Utf8String_t expectation("q%3Dgreater%20than%205");

	Utf8String_t encoded = UrlEnconding_t::urlencode(s);
	ASSERT_TRUE(encoded.equals(expectation));
}

TEST_F(URLEncodingTest, urlEncodeQueryParameterWithSpacesAndEqualsSignFinallyDecodeAgain)
{
	Utf8String_t s("q=greater than 5");
	Utf8String_t encoded = UrlEnconding_t::urlencode(s);
	Utf8String_t decoded = UrlEnconding_t::urldecode(encoded);

	ASSERT_TRUE(decoded.equals(s));
}

TEST_F(URLEncodingTest, urlEncodeStringNotChangedAllCharactersAllowed)
{
	Utf8String_t s(".All-this~characters_are_Allowed.");

	Utf8String_t encoded = UrlEnconding_t::urlencode(s);
	ASSERT_TRUE(encoded.equals(s));
}

TEST_F(URLEncodingTest, urlEncodeUTF8MultibyteName)
{
	Utf8String_t s("\xD7\xAA\xf0\x9f\x98\x8b");
	Utf8String_t expectation("%D7%AA%F0%9F%98%8B");

	Utf8String_t encoded = UrlEnconding_t::urlencode(s);
	ASSERT_TRUE(encoded.equals(expectation));
}

TEST_F(URLEncodingTest, urlEncodeUTF8MultibyteNameFinallyDecodeAgain)
{
	Utf8String_t s("\xD7\xAA\xf0\x9f\x98\x8b");
	Utf8String_t encoded = UrlEnconding_t::urlencode(s);
	Utf8String_t decoded = UrlEnconding_t::urldecode(encoded);
	ASSERT_TRUE(decoded.equals(s));
}

TEST_F(URLEncodingTest, urlDecodeFailing_PercentFollowedByNonHexCharacterTwoInvalidBytes)
{
	Utf8String_t s("invalid%string");
	Utf8String_t decoded = UrlEnconding_t::urldecode(s);

	Utf8String_t expectation("invalid?string");
	ASSERT_TRUE(decoded.equals(expectation));
}

TEST_F(URLEncodingTest, urlDecodeFailing_PercentFollowedByNonHexCharacterSecondByteInvalid)
{
	Utf8String_t s("invalid%ARstring");
	Utf8String_t decoded = UrlEnconding_t::urldecode(s);

	Utf8String_t expectation("invalid?ARstring");
	ASSERT_TRUE(decoded.equals(expectation));
}

TEST_F(URLEncodingTest, urlDecodeFailing_PercentFollowedByNonHexCharacterFirstByteInvalid)
{
	Utf8String_t s("invalid%XString");
	Utf8String_t decoded = UrlEnconding_t::urldecode(s);

	Utf8String_t expectation("invalid?XString");
	ASSERT_TRUE(decoded.equals(expectation));
}

TEST_F(URLEncodingTest, urlDecodeFailing_OneCharacterMissingAtTheEnd)
{
	Utf8String_t s("invalidstring%E");
	Utf8String_t decoded = UrlEnconding_t::urldecode(s);

	Utf8String_t expectation("invalidstring");
	ASSERT_TRUE(decoded.equals(expectation));
}

TEST_F(URLEncodingTest, urlDecodeFailing_BothCharactersMissingAtTheEnd)
{
	Utf8String_t s("invalidstring%");
	Utf8String_t decoded = UrlEnconding_t::urldecode(s);

	Utf8String_t expectation("invalidstring");
	ASSERT_TRUE(decoded.equals(expectation));
}

TEST_F(URLEncodingTest, urlEncodeDecodePercentSignPreserved)
{
	Utf8String_t s("this text contains a % as valid codepoint");

	Utf8String_t encoded = UrlEnconding_t::urlencode(s);
	Utf8String_t decoded = UrlEnconding_t::urldecode(encoded);

	ASSERT_TRUE(decoded.equals(s));
}

TEST_F(URLEncodingTest, itIsPossibleToMarkAdditionalCharactersAsReserved)
{
	// given
	Utf8String_t input("0123456789-._~");
	std::unordered_set<char> additionalReservedCharacters =  { '0', '_' };

	// when
	auto obtained = UrlEnconding_t::urlencode(input, additionalReservedCharacters);

	// then
	ASSERT_EQ(obtained, "%30123456789-.%5F~");
}