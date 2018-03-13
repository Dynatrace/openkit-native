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
#include "core/util/URLEncoding.h"
#include "memory.h"

#include <cstdint>
#include <gtest/gtest.h>

using namespace core;

class URLEncodingTest : public testing::Test
{
public:
	void SetUp()
	{

	}

	void TearDown()
	{

	}
};


TEST_F(URLEncodingTest, UrlEncodeQueryParameterWithSpacesAndEqualsSign)
{
	UTF8String s("q=greater than 5");
	UTF8String expectation("q%3Dgreater%20than%205");

	UTF8String encoded = core::util::URLEncoding::urlencode(s);
	EXPECT_TRUE(encoded.equals(expectation));
}

TEST_F(URLEncodingTest, UrlEncodeQueryParameterWithSpacesAndEqualsSignFinallyDecodeAgain)
{
	UTF8String s("q=greater than 5");
	UTF8String encoded = core::util::URLEncoding::urlencode(s);
	UTF8String decoded = core::util::URLEncoding::urldecode(encoded);

	EXPECT_TRUE(decoded.equals(s));
}

TEST_F(URLEncodingTest, UrlEncodeStringNotChangedAllCharactersAllowed)
{
	UTF8String s(".All-this~characters_are_Allowed.");

	UTF8String encoded = core::util::URLEncoding::urlencode(s);
	EXPECT_TRUE(encoded.equals(s));
}

TEST_F(URLEncodingTest, UrlEncodeUTF8MultibyteName)
{
	UTF8String s("\xD7\xAA\xf0\x9f\x98\x8b");
	UTF8String expectation("%D7%AA%F0%9F%98%8B");

	UTF8String encoded = core::util::URLEncoding::urlencode(s);
	EXPECT_TRUE(encoded.equals(expectation));
}

TEST_F(URLEncodingTest, UrlEncodeUTF8MultibyteNameFinallyDecodeAgain)
{
	UTF8String s("\xD7\xAA\xf0\x9f\x98\x8b");
	UTF8String encoded = core::util::URLEncoding::urlencode(s);
	UTF8String decoded = core::util::URLEncoding::urldecode(encoded);
	EXPECT_TRUE(decoded.equals(s));
}

TEST_F(URLEncodingTest, UrlDecodeFailing_PercentFollowedByNonHexCharacterTwoInvalidBytes)
{
	UTF8String s("invalid%string");
	UTF8String decoded = core::util::URLEncoding::urldecode(s);

	UTF8String expectation("invalid?string");
	EXPECT_TRUE(decoded.equals(expectation));
}

TEST_F(URLEncodingTest, UrlDecodeFailing_PercentFollowedByNonHexCharacterSecondByteInvalid)
{
	UTF8String s("invalid%ARstring");
	UTF8String decoded = core::util::URLEncoding::urldecode(s);

	UTF8String expectation("invalid?ARstring");
	EXPECT_TRUE(decoded.equals(expectation));
}

TEST_F(URLEncodingTest, UrlDecodeFailing_PercentFollowedByNonHexCharacterFirstByteInvalid)
{
	UTF8String s("invalid%XString");
	UTF8String decoded = core::util::URLEncoding::urldecode(s);

	UTF8String expectation("invalid?XString");
	EXPECT_TRUE(decoded.equals(expectation));
}

TEST_F(URLEncodingTest, UrlDecodeFailing_OneCharacterMissingAtTheEnd)
{
	UTF8String s("invalidstring%E");
	UTF8String decoded = core::util::URLEncoding::urldecode(s);

	UTF8String expectation("invalidstring");
	EXPECT_TRUE(decoded.equals(expectation));
}

TEST_F(URLEncodingTest, UrlDecodeFailing_BothCharactersMissingAtTheEnd)
{
	UTF8String s("invalidstring%");
	UTF8String decoded = core::util::URLEncoding::urldecode(s);

	UTF8String expectation("invalidstring");
	EXPECT_TRUE(decoded.equals(expectation));
}

TEST_F(URLEncodingTest, UrlEncodeDecodePercentSignPreserved)
{
	UTF8String s("this text contains a % as valid codepoint");

	UTF8String encoded = core::util::URLEncoding::urlencode(s);
	UTF8String decoded = core::util::URLEncoding::urldecode(encoded);

	EXPECT_TRUE(decoded.equals(s));

}