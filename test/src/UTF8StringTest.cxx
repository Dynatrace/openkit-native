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
#include "core/UTF8String.h"
#include "memory.h"

#include <stdint.h>
#include <gtest/gtest.h>

using namespace core;

class UTF8StringTest : public testing::Test
{
public:
	void SetUp()
	{

	}

	void TearDown()
	{

	}
};

TEST_F(UTF8StringTest, AStringCanBeInitializedWithAnASCIIString)
{
	UTF8String s("test123");

	const std::string stringData = s.getStringData();

	EXPECT_EQ(stringData[0], 't');
	EXPECT_EQ(stringData[1], 'e');
	EXPECT_EQ(stringData[2], 's');
	EXPECT_EQ(stringData[3], 't');
	EXPECT_EQ(stringData[4], '1');
	EXPECT_EQ(stringData[5], '2');
	EXPECT_EQ(stringData[6], '3');

	EXPECT_EQ(s.getStringLength(), 7);
	EXPECT_EQ(stringData.size(), 7);
}


TEST_F(UTF8StringTest, AStringCanBeInitializedWithFour2ByteCharacters)
{
	UTF8String s("\xD7\xAA\xD7\x95\xD7\x93\xD7\x94");

	const std::string stringData = s.getStringData();

	EXPECT_EQ(stringData[0], (char)0xD7); // 1/2
	EXPECT_EQ(stringData[1], (char)0xAA); // 2/2
	EXPECT_EQ(stringData[2], (char)0xD7); // 1/2
	EXPECT_EQ(stringData[3], (char)0x95); // 2/2
	EXPECT_EQ(stringData[4], (char)0xD7); // 1/2
	EXPECT_EQ(stringData[5], (char)0x93); // 2/2
	EXPECT_EQ(stringData[6], (char)0xD7); // 1/2
	EXPECT_EQ(stringData[7], (char)0x94); // 2/2

	EXPECT_EQ(s.getStringLength(), 4);
	EXPECT_EQ(stringData.size(), 8);
}

TEST_F(UTF8StringTest, AStringCanBeInitializedByReplacingInvalidUTF8FirstByte)
{
	//third character (is ASCII )breaks the 2nd UTF8 character ( 2 byte wide)
	UTF8String s("\xD7\xAAy\x95\xD7\x93\xD7\x94");

	const std::string stringData = s.getStringData();
	EXPECT_EQ(stringData[0], (char)0xD7);
	EXPECT_EQ(stringData[1], (char)0xAA);
	EXPECT_EQ(stringData[2], 'y');
	EXPECT_EQ(stringData[3], (char)0xEF);
	EXPECT_EQ(stringData[4], (char)0xBF);
	EXPECT_EQ(stringData[5], (char)0xBD);
	EXPECT_EQ(stringData[6], (char)0xD7);
	EXPECT_EQ(stringData[7], (char)0x93);
	EXPECT_EQ(stringData[8], (char)0xD7);
	EXPECT_EQ(stringData[9], (char)0x94);

	EXPECT_EQ(s.getStringLength(), 5);
	EXPECT_EQ(stringData.size(), 10);
}

TEST_F(UTF8StringTest, AStringCanBeInitializedByReplacingInvalidUTF8SecondByte)
{
	//third character (is ASCII )breaks the 2nd UTF8 character ( 2 byte wide)
	UTF8String s("\xD7\xAA\x95r\xD7\x93\xD7\x94");

	const std::string stringData = s.getStringData();

	EXPECT_EQ(stringData[0], (char)0xD7);
	EXPECT_EQ(stringData[1], (char)0xAA);
	EXPECT_EQ(stringData[2], (char)0xEF);
	EXPECT_EQ(stringData[3], (char)0xBF);
	EXPECT_EQ(stringData[4], (char)0xBD);
	EXPECT_EQ(stringData[5], 'r');
	EXPECT_EQ(stringData[6], (char)0xD7);
	EXPECT_EQ(stringData[7], (char)0x93);
	EXPECT_EQ(stringData[8], (char)0xD7);
	EXPECT_EQ(stringData[9], (char)0x94);

	EXPECT_EQ(s.getStringLength(), 5);
	EXPECT_EQ(stringData.size(), 10);
}


TEST_F(UTF8StringTest, AStringCanBeInitializedUsingATwoByteAndAFourByteUTF8)
{
	//third character (is ASCII )breaks the 2nd UTF8 character ( 2 byte wide)
	UTF8String s("\xD7\xAA\xf0\x9f\x98\x8b");

	const std::string stringData = s.getStringData();

	EXPECT_EQ(stringData[0], (char)0xD7);// 1/2
	EXPECT_EQ(stringData[1], (char)0xAA);// 2/2
	EXPECT_EQ(stringData[2], (char)0xF0);// 1/4
	EXPECT_EQ(stringData[3], (char)0x9F);// 2/4
	EXPECT_EQ(stringData[4], (char)0x98);// 3/4
	EXPECT_EQ(stringData[5], (char)0x8B);// 4/4

	EXPECT_EQ(s.getStringLength(), 2);
	EXPECT_EQ(stringData.size(), 6);
}

TEST_F(UTF8StringTest, AStringCanBeInitializedUsingAOneByteUTF8)
{
	UTF8String s("\x61\x62\x63\x72\x74\x78");

	const std::string stringData = s.getStringData();

	EXPECT_EQ(stringData[0], 'a');
	EXPECT_EQ(stringData[1], 'b');
	EXPECT_EQ(stringData[2], 'c');
	EXPECT_EQ(stringData[3], 'r');
	EXPECT_EQ(stringData[4], 't');
	EXPECT_EQ(stringData[5], 'x');

	EXPECT_EQ(s.getStringLength(), 6);
	EXPECT_EQ(stringData.size(), 6);
}

TEST_F(UTF8StringTest, AStringCanInitializedUsingACombinationOfAllByteWidths)
{
	//the following string contains
	//- abc in plain ASCII
	//- a two byte UTF8 characters from the hebrew alphabet
	//- a single byte UTF8 character
	//- a four byte UTF8 character
	//- a single byte UTF8 character
	//- a three byte UTF8 character
	//- xyz in plain ASCII
	UTF8String s("abc\xD7\xAA\x78\xF0\x9F\x98\x8B\x64\xEA\xA6\x85xyz");

	const std::string stringData = s.getStringData();

	EXPECT_EQ(stringData[0], 'a');
	EXPECT_EQ(stringData[1], 'b');
	EXPECT_EQ(stringData[2], 'c');
	EXPECT_EQ(stringData[3], (char)0xD7); // 1/2
	EXPECT_EQ(stringData[4], (char)0xAA); // 2/2
	EXPECT_EQ(stringData[5], (char)0x78); // 1/1
	EXPECT_EQ(stringData[6], (char)0xF0); // 1/4
	EXPECT_EQ(stringData[7], (char)0x9F); // 2/4
	EXPECT_EQ(stringData[8], (char)0x98); // 3/4
	EXPECT_EQ(stringData[9], (char)0x8B); // 4/4
	EXPECT_EQ(stringData[10], (char)0x64);// 1/1
	EXPECT_EQ(stringData[11], (char)0xEA);// 1/3
	EXPECT_EQ(stringData[12], (char)0xA6);// 2/3
	EXPECT_EQ(stringData[13], (char)0x85); // 3/3
	EXPECT_EQ(stringData[14], 'x');
	EXPECT_EQ(stringData[15], 'y');
	EXPECT_EQ(stringData[16], 'z');

	EXPECT_EQ(s.getStringLength(), 11);
	EXPECT_EQ(stringData.size(), 17);
}

TEST_F(UTF8StringTest, AStringCanBeInitializedWithABrokenThreeByteUTF8FollowedByaTwoByteUTF8)
{
	//valid \xEA\xA6\x85 \xD7\xAA
	UTF8String s("\xEA\xA6\xD7\xAA");

	const std::string stringData = s.getStringData();

	EXPECT_EQ(stringData[0], (char)0xEF);
	EXPECT_EQ(stringData[1], (char)0xBF);
	EXPECT_EQ(stringData[2], (char)0xBD);
	EXPECT_EQ(stringData[3], (char)0xD7);
	EXPECT_EQ(stringData[4], (char)0xAA);
	
	EXPECT_EQ(s.getStringLength(), 2);
	EXPECT_EQ(stringData.size(), 5);
}

TEST_F(UTF8StringTest, AStringCanBeInitializedWhenTwoOfThreeMultiByteCharactersAreBroken)
{
	//valid \xea\xa6\x8a \xea\xa6\x8d \xea\xa6\x90
	UTF8String s("\xea\xa6\xe6\x8d\xea\xa6\x90");

	const std::string stringData = s.getStringData();

	EXPECT_EQ(stringData[0], (char)0xEF);
	EXPECT_EQ(stringData[1], (char)0xBF);
	EXPECT_EQ(stringData[2], (char)0xBD);
	EXPECT_EQ(stringData[3], (char)0xEF);
	EXPECT_EQ(stringData[4], (char)0xBF);
	EXPECT_EQ(stringData[5], (char)0xBD);
	EXPECT_EQ(stringData[6], (char)0xEA);
	EXPECT_EQ(stringData[7], (char)0xA6);
	EXPECT_EQ(stringData[8], (char)0x90);

	EXPECT_EQ(s.getStringLength(), 3);
	EXPECT_EQ(stringData.size(), 9);
}

TEST_F(UTF8StringTest, AStringWillNotBeConstructedUsingANullPointer)
{
	UTF8String s("");
	const std::string stringData = s.getStringData();
	EXPECT_TRUE(stringData.size() == 0);
	EXPECT_EQ(s.getStringLength(), 0);

	UTF8String s2(s);
	const std::string stringData2 = s2.getStringData();
	EXPECT_TRUE(stringData2.size() == 0);
	EXPECT_EQ(s2.getStringLength(), 0);
}

TEST_F(UTF8StringTest, AStringCanBeSearchedForASCIICharacters)
{
	UTF8String s("abc\xD7\xAA\x78\xF0\x9F\x98\x8B\x64\xEA\xA6\x85xyz");

	int32_t pos_of_c = s.getIndexOf("c", 0);
	EXPECT_EQ(pos_of_c, 2);

	int32_t pos_of_y = s.getIndexOf("y", 0);
	EXPECT_EQ(pos_of_y, 9);
}

TEST_F(UTF8StringTest, AStringCanBeSearchedForUTF8Characters)
{
	UTF8String s("abc\xD7\xAA\x78\xF0\x9F\x98\x8B\x64\xEA\xA6\x85xyz");

	const char* utf_character = "\xEA\xA6\x85";

	int32_t pos_of_utf3 = s.getIndexOf(utf_character, 0);
	EXPECT_EQ(pos_of_utf3, 7);
}

TEST_F(UTF8StringTest, AStringDoesNotContainAGivenCharacter)
{
	UTF8String s("abcefgh");

	char character_d = 'd';

	int32_t pos_of_d = s.getIndexOf(&character_d, 0);
	EXPECT_EQ(pos_of_d, -1);
}

TEST_F(UTF8StringTest, AStringIndexOfUsingTheOffsetParameter)
{
	UTF8String s("abcefgh");

	char character_b = 'b';

	int32_t pos_of_b_offset0 = s.getIndexOf(&character_b, 0);
	EXPECT_EQ(pos_of_b_offset0, 1);

	int32_t pos_of_b_offset1 = s.getIndexOf(&character_b, 1);
	EXPECT_EQ(pos_of_b_offset1, 1);

	int32_t pos_of_b_offset2 = s.getIndexOf(&character_b, 2);
	EXPECT_EQ(pos_of_b_offset2, -1);
}

TEST_F(UTF8StringTest, AStringIsDuplicated_ValidString)
{
	UTF8String s("abc\xD7\xAA\x78\xF0\x9F\x98\x8B\x64\xEA\xA6\x85xyz");

	UTF8String duplicate(s);

	bool comparison_result = s.compare(duplicate);
	EXPECT_TRUE(comparison_result);
}


TEST_F(UTF8StringTest, AStringIsComparedWithAnIdenticalString)
{
	UTF8String s1("1234567890");
	UTF8String s2("1234567890");
	
	bool comparison = s1.compare(s2);
	EXPECT_TRUE(comparison);
}

TEST_F(UTF8StringTest, AStringIsComparedWithADifferentString)
{
	UTF8String s1("1234567890");
	UTF8String s2("1234567898");

	bool comparison = s1.compare(s2);
	EXPECT_FALSE(comparison);
}

TEST_F(UTF8StringTest, AStringIsComparedWithANullString)
{
	UTF8String s("abc\xD7\xAA\x78\xF0\x9F\x98\x8B\x64\xEA\xA6\x85xyz");
	
	bool comparison = s.compare(NULL);
	EXPECT_FALSE(comparison);
}

TEST_F(UTF8StringTest, SubstringFromValidRange)
{
	UTF8String s("1234567890");

	UTF8String substr = s.substring(3, 7);

	const char* expect = "45678";
	EXPECT_EQ(6, substr.getStringData().size());
	EXPECT_EQ(5, substr.getStringLength());
	int32_t comparison_result = substr.compare(expect);
	EXPECT_EQ(comparison_result, 0);

}

TEST_F(UTF8StringTest, SubstringFromValidRange_UTF8Multibyte)
{
	UTF8String s("\xD7\xAA\xD7\x95\xD7\x93\xD7\x94"); // 4 2-byte characters

	UTF8String substr = s.substring(1, 3);

	const char* expect = "\xD7\x95\xD7\x93\xD7\x94";
	EXPECT_EQ(7, substr.getStringData().size());
	EXPECT_EQ(3, substr.getStringLength());
	int32_t comparison_result = substr.compare(expect);
	EXPECT_EQ(comparison_result, 0);
}

TEST_F(UTF8StringTest, SubstringFromValidRange_UTF8MultibyteASCIIMix)
{
	UTF8String s("\xD7\xAAza\xD7\x95yb\xD7\x93xc\xD7\x94wd"); // 2-byte characters mixed with triplets of ASCII

	UTF8String substr = s.substring(3, 6);

	const char* expect = "\xD7\x95yb\xD7\x93";
	EXPECT_EQ(7, substr.getStringData().size());
	EXPECT_EQ(4, substr.getStringLength());
	int32_t comparison_result = substr.compare(expect);
	EXPECT_EQ(comparison_result, 0);
}

TEST_F(UTF8StringTest, SubstringWithFirstInvalidIndex)
{
	UTF8String s("1234567890");

	UTF8String substr = s.substring(-1, 7);

	EXPECT_TRUE(substr.getStringLength() == 0);
}

TEST_F(UTF8StringTest, SubstringWithSecondInvalidIndex)
{
	UTF8String s("1234567890");

	UTF8String substr = s.substring(1, 12);

	EXPECT_TRUE(substr.getStringLength() == 0);
}

TEST_F(UTF8StringTest, SubstringWithSecondIndexSmallerThanFirstIndex)
{
	UTF8String s("1234567890");

	UTF8String substr = s.substring(6, 3);

	EXPECT_TRUE(substr.getStringLength() ==0);
}

TEST_F(UTF8StringTest, ConcatenateASCIIWithUTFString)
{
	UTF8String s1("abc");
	UTF8String s2("\xD7\x95yb\xD7\x93");

	s1.concatenate(s2);

	const std::string stringData = s1.getStringData();

	EXPECT_EQ(stringData[0], 'a');
	EXPECT_EQ(stringData[1], 'b');
	EXPECT_EQ(stringData[2], 'c');
	EXPECT_EQ(stringData[3], (char)0xD7);
	EXPECT_EQ(stringData[4], (char)0x95);
	EXPECT_EQ(stringData[5], 'y');
	EXPECT_EQ(stringData[6], 'b');
	EXPECT_EQ(stringData[7], (char)0xD7);
	EXPECT_EQ(stringData[8], (char)0x93);

	EXPECT_EQ(s1.getStringLength(), 7);
	EXPECT_EQ(stringData.size(), 9);
}

TEST_F(UTF8StringTest, ConcatenateUTFWithASCIIString)
{
	UTF8String s1("\xD7\xAA\xD7\x95");
	UTF8String s2("test");

	s1.concatenate(s2);

	const std::string stringData = s1.getStringData();

	EXPECT_EQ(stringData[0], (char)0xD7);
	EXPECT_EQ(stringData[1], (char)0xAA);
	EXPECT_EQ(stringData[2], (char)0xD7);
	EXPECT_EQ(stringData[3], (char)0x95);
	EXPECT_EQ(stringData[4], 't');
	EXPECT_EQ(stringData[5], 'e');
	EXPECT_EQ(stringData[6], 's');
	EXPECT_EQ(stringData[7], 't');

	EXPECT_EQ(s1.getStringLength(), 6);
	EXPECT_EQ(stringData.size(), 8);
}

TEST_F(UTF8StringTest, ConcatenateWithCharPointer)
{
	UTF8String s("part 1 -");
	s.concatenate("part 2");

	const std::string stringData = s.getStringData();

	EXPECT_EQ(stringData[0], 'p');
	EXPECT_EQ(stringData[1], 'a');
	EXPECT_EQ(stringData[2], 'r');
	EXPECT_EQ(stringData[3], 't');
	EXPECT_EQ(stringData[4], ' ');
	EXPECT_EQ(stringData[5], '1');
	EXPECT_EQ(stringData[6], ' ');
	EXPECT_EQ(stringData[7], '-');
	EXPECT_EQ(stringData[8], 'p');
	EXPECT_EQ(stringData[9], 'a');
	EXPECT_EQ(stringData[10], 'r');
	EXPECT_EQ(stringData[11], 't');
	EXPECT_EQ(stringData[12], ' ');
	EXPECT_EQ(stringData[13], '2');

	EXPECT_EQ(s.getStringLength(), 14);
	EXPECT_EQ(stringData.size(), 14);
}


TEST_F(UTF8StringTest, ConcatenateWithEmptyString)
{
	UTF8String s("test123");
	s.concatenate("");

	const std::string stringData = s.getStringData();

	EXPECT_EQ(stringData[0], 't');
	EXPECT_EQ(stringData[1], 'e');
	EXPECT_EQ(stringData[2], 's');
	EXPECT_EQ(stringData[3], 't');
	EXPECT_EQ(stringData[4], '1');
	EXPECT_EQ(stringData[5], '2');
	EXPECT_EQ(stringData[6], '3');

	EXPECT_EQ(s.getStringLength(), 7);
	EXPECT_EQ(stringData.size(), 7);
}
