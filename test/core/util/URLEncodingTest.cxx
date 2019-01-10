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
#include "core/util/URLEncoding.h"
#include "memory.h"

#include <cstdint>
#include <gtest/gtest.h>

using namespace core;

class URLEncodingTest : public testing::Test
{
public:

};


TEST_F(URLEncodingTest, urlEncodeQueryParameterWithSpacesAndEqualsSign)
{
	UTF8String s("q=greater than 5");
	UTF8String expectation("q%3Dgreater%20than%205");

	UTF8String encoded = core::util::URLEncoding::urlencode(s);
	ASSERT_TRUE(encoded.equals(expectation));
}

TEST_F(URLEncodingTest, urlEncodeQueryParameterWithSpacesAndEqualsSignFinallyDecodeAgain)
{
	UTF8String s("q=greater than 5");
	UTF8String encoded = core::util::URLEncoding::urlencode(s);
	UTF8String decoded = core::util::URLEncoding::urldecode(encoded);

	ASSERT_TRUE(decoded.equals(s));
}

TEST_F(URLEncodingTest, urlEncodeStringNotChangedAllCharactersAllowed)
{
	UTF8String s(".All-this~characters_are_Allowed.");

	UTF8String encoded = core::util::URLEncoding::urlencode(s);
	ASSERT_TRUE(encoded.equals(s));
}

TEST_F(URLEncodingTest, urlEncodeUTF8MultibyteName)
{
	UTF8String s("\xD7\xAA\xf0\x9f\x98\x8b");
	UTF8String expectation("%D7%AA%F0%9F%98%8B");

	UTF8String encoded = core::util::URLEncoding::urlencode(s);
	ASSERT_TRUE(encoded.equals(expectation));
}

TEST_F(URLEncodingTest, urlEncodeUTF8MultibyteNameFinallyDecodeAgain)
{
	UTF8String s("\xD7\xAA\xf0\x9f\x98\x8b");
	UTF8String encoded = core::util::URLEncoding::urlencode(s);
	UTF8String decoded = core::util::URLEncoding::urldecode(encoded);
	ASSERT_TRUE(decoded.equals(s));
}

TEST_F(URLEncodingTest, urlDecodeFailing_PercentFollowedByNonHexCharacterTwoInvalidBytes)
{
	UTF8String s("invalid%string");
	UTF8String decoded = core::util::URLEncoding::urldecode(s);

	UTF8String expectation("invalid?string");
	ASSERT_TRUE(decoded.equals(expectation));
}

TEST_F(URLEncodingTest, urlDecodeFailing_PercentFollowedByNonHexCharacterSecondByteInvalid)
{
	UTF8String s("invalid%ARstring");
	UTF8String decoded = core::util::URLEncoding::urldecode(s);

	UTF8String expectation("invalid?ARstring");
	ASSERT_TRUE(decoded.equals(expectation));
}

TEST_F(URLEncodingTest, urlDecodeFailing_PercentFollowedByNonHexCharacterFirstByteInvalid)
{
	UTF8String s("invalid%XString");
	UTF8String decoded = core::util::URLEncoding::urldecode(s);

	UTF8String expectation("invalid?XString");
	ASSERT_TRUE(decoded.equals(expectation));
}

TEST_F(URLEncodingTest, urlDecodeFailing_OneCharacterMissingAtTheEnd)
{
	UTF8String s("invalidstring%E");
	UTF8String decoded = core::util::URLEncoding::urldecode(s);

	UTF8String expectation("invalidstring");
	ASSERT_TRUE(decoded.equals(expectation));
}

TEST_F(URLEncodingTest, urlDecodeFailing_BothCharactersMissingAtTheEnd)
{
	UTF8String s("invalidstring%");
	UTF8String decoded = core::util::URLEncoding::urldecode(s);

	UTF8String expectation("invalidstring");
	ASSERT_TRUE(decoded.equals(expectation));
}

TEST_F(URLEncodingTest, urlEncodeDecodePercentSignPreserved)
{
	UTF8String s("this text contains a % as valid codepoint");

	UTF8String encoded = core::util::URLEncoding::urlencode(s);
	UTF8String decoded = core::util::URLEncoding::urldecode(encoded);

	ASSERT_TRUE(decoded.equals(s));
}

TEST_F(URLEncodingTest, itIsPossibleToMarkAdditionalCharactersAsReserved)
{
	// given
	UTF8String input("0123456789-._~");
	std::unordered_set<char> additionalReservedCharacters =  { '0', '_' };

	// when
	auto obtained = core::util::URLEncoding::urlencode(input, additionalReservedCharacters);

	// then
	ASSERT_EQ(obtained, "%30123456789-.%5F~");
}