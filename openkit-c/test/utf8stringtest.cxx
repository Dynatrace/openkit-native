extern "C" {
#include "utf8string.h"
#include "memory.h"
}

#include <stdint.h>
#include <gtest/gtest.h>

class StringTest : public testing::Test
{
public:
	void SetUp()
	{

	}

	void TearDown()
	{

	}
};

TEST_F(StringTest, AStringCanBeInitializedWithAnASCIIString)
{
	const char* test_string = "test123";
	utf8string* s = init_string(test_string);

	EXPECT_EQ(s->data[0], 't');
	EXPECT_EQ(s->data[1], 'e');
	EXPECT_EQ(s->data[2], 's');
	EXPECT_EQ(s->data[3], 't');
	EXPECT_EQ(s->data[4], '1');
	EXPECT_EQ(s->data[5], '2');
	EXPECT_EQ(s->data[6], '3');
	EXPECT_EQ(s->data[7], '\0');

	EXPECT_EQ(s->string_length, 7);
	EXPECT_EQ(s->byte_length, 8);
	destroy_string(s);
}


TEST_F(StringTest, AStringCanBeInitializedWithFour2ByteCharacters)
{
	const char* string = "\xD7\xAA\xD7\x95\xD7\x93\xD7\x94";
	utf8string* s = init_string(string);

	EXPECT_EQ(s->data[0], (char)0xD7); // 1/2
	EXPECT_EQ(s->data[1], (char)0xAA); // 2/2
	EXPECT_EQ(s->data[2], (char)0xD7); // 1/2
	EXPECT_EQ(s->data[3], (char)0x95); // 2/2
	EXPECT_EQ(s->data[4], (char)0xD7); // 1/2
	EXPECT_EQ(s->data[5], (char)0x93); // 2/2
	EXPECT_EQ(s->data[6], (char)0xD7); // 1/2
	EXPECT_EQ(s->data[7], (char)0x94); // 2/2
	EXPECT_EQ(s->data[8], '\0');

	EXPECT_EQ(s->string_length, 4);
	EXPECT_EQ(s->byte_length, 9);
	destroy_string(s);
}

TEST_F(StringTest, AStringCanBeInitializedByReplacingInvalidUTF8FirstByte)
{
	//third character (is ASCII )breaks the 2nd UTF8 character ( 2 byte wide)
	const char* string = "\xD7\xAAy\x95\xD7\x93\xD7\x94";
	utf8string* s = init_string(string);

	EXPECT_EQ(s->data[0], (char)0xD7);
	EXPECT_EQ(s->data[1], (char)0xAA);
	EXPECT_EQ(s->data[2], 'y');
	EXPECT_EQ(s->data[3], '?');
	EXPECT_EQ(s->data[4], (char)0xD7);
	EXPECT_EQ(s->data[5], (char)0x93);
	EXPECT_EQ(s->data[6], (char)0xD7);
	EXPECT_EQ(s->data[7], (char)0x94);

	EXPECT_EQ(s->string_length, 5);
	EXPECT_EQ(s->byte_length, 9);
	destroy_string(s);
}

TEST_F(StringTest, AStringCanBeInitializedByReplacingInvalidUTF8SecondByte)
{
	//third character (is ASCII )breaks the 2nd UTF8 character ( 2 byte wide)
	const char* string = "\xD7\xAA\xD7r\xD7\x93\xD7\x94";
	utf8string* s = init_string(string);

	EXPECT_EQ(s->data[0], (char)0xD7);
	EXPECT_EQ(s->data[1], (char)0xAA);
	EXPECT_EQ(s->data[2], '?');
	EXPECT_EQ(s->data[3], 'r');
	EXPECT_EQ(s->data[4], (char)0xD7);
	EXPECT_EQ(s->data[5], (char)0x93);
	EXPECT_EQ(s->data[6], (char)0xD7);
	EXPECT_EQ(s->data[7], (char)0x94);

	EXPECT_EQ(s->string_length, 5);
	EXPECT_EQ(s->byte_length, 9);
	destroy_string(s);
}


TEST_F(StringTest, AStringCanBeInitializedUsingATwoByteAndAFourByteUTF8)
{
	//third character (is ASCII )breaks the 2nd UTF8 character ( 2 byte wide)
	const char* string = "\xD7\xAA\xf0\x9f\x98\x8b";
	utf8string* s = init_string(string);

	EXPECT_EQ(s->data[0], (char)0xD7);// 1/2
	EXPECT_EQ(s->data[1], (char)0xAA);// 2/2
	EXPECT_EQ(s->data[2], (char)0xF0);// 1/4
	EXPECT_EQ(s->data[3], (char)0x9F);// 2/4
	EXPECT_EQ(s->data[4], (char)0x98);// 3/4
	EXPECT_EQ(s->data[5], (char)0x8B);// 4/4

	EXPECT_EQ(s->string_length, 2);
	EXPECT_EQ(s->byte_length, 7);
	destroy_string(s);
}

TEST_F(StringTest, AStringCanBeInitializedUsingAOneByteUTF8)
{
	const char* string = "\x61\x62\x63\x72\x74\x78";
	utf8string* s = init_string(string);

	EXPECT_EQ(s->data[0], 'a');
	EXPECT_EQ(s->data[1], 'b');
	EXPECT_EQ(s->data[2], 'c');
	EXPECT_EQ(s->data[3], 'r');
	EXPECT_EQ(s->data[4], 't');
	EXPECT_EQ(s->data[5], 'x');

	EXPECT_EQ(s->string_length, 6);
	EXPECT_EQ(s->byte_length, 7);
	destroy_string(s);
}

TEST_F(StringTest, AStringCanInitializedUsingACombinationOfAllByteWidths)
{
	//the following string contains
	//- abc in plain ASCII
	//- a two byte UTF8 characters from the hebrew alphabet
	//- a single byte UTF8 character
	//- a four byte UTF8 character
	//- a single byte UTF8 character
	//- a three byte UTF8 character
	//- xyz in plain ASCII
	const char* string = "abc\xD7\xAA\x78\xF0\x9F\x98\x8B\x64\xEA\xA6\x85xyz";
	utf8string* s = init_string(string);

	EXPECT_EQ(s->data[0], 'a');
	EXPECT_EQ(s->data[1], 'b');
	EXPECT_EQ(s->data[2], 'c');
	EXPECT_EQ(s->data[3], (char)0xD7); // 1/2
	EXPECT_EQ(s->data[4], (char)0xAA); // 2/2
	EXPECT_EQ(s->data[5], (char)0x78); // 1/1
	EXPECT_EQ(s->data[6], (char)0xF0); // 1/4
	EXPECT_EQ(s->data[7], (char)0x9F); // 2/4
	EXPECT_EQ(s->data[8], (char)0x98); // 3/4
	EXPECT_EQ(s->data[9], (char)0x8B); // 4/4
	EXPECT_EQ(s->data[10], (char)0x64);// 1/1
	EXPECT_EQ(s->data[11], (char)0xEA);// 1/3
	EXPECT_EQ(s->data[12], (char)0xA6);// 2/3
	EXPECT_EQ(s->data[13], (char)0x85); // 3/3
	EXPECT_EQ(s->data[14], 'x');
	EXPECT_EQ(s->data[15], 'y');
	EXPECT_EQ(s->data[16], 'z');

	EXPECT_EQ(s->string_length, 11);
	EXPECT_EQ(s->byte_length, 18);
	destroy_string(s);
}

TEST_F(StringTest, AStringCanBeInitializedWithABrokenThreeByteUTF8FollowedByaTwoByteUTF8)
{
	//valid \xEA\xA6\x85 \xD7\xAA
	const char* string = "\xEA\xA6\xD7\xAA";
	utf8string* s = init_string(string);

	EXPECT_EQ(s->data[0], '?');
	EXPECT_EQ(s->data[1], (char)0xD7);
	EXPECT_EQ(s->data[2], (char)0xAA);
	
	EXPECT_EQ(s->string_length, 2);
	EXPECT_EQ(s->byte_length, 4);
	destroy_string(s);
}

TEST_F(StringTest, AStringCanBeInitializedWhenTwoOfThreeMultiByteCharactersAreBroken)
{
	//valid \xea\xa6\x8a \xea\xa6\x8d \xea\xa6\x90
	const char* string = "\xea\xa6\xe6\x8d\xea\xa6\x90";

	utf8string* s = init_string(string);

	EXPECT_EQ(s->data[0], '?');
	EXPECT_EQ(s->data[1], '?');
	EXPECT_EQ(s->data[2], (char)0xEA);
	EXPECT_EQ(s->data[3], (char)0xA6);
	EXPECT_EQ(s->data[4], (char)0x90);

	EXPECT_EQ(s->string_length, 3);
	EXPECT_EQ(s->byte_length, 6);
	destroy_string(s);
}

TEST_F(StringTest, AStringWillNotBeConstructedUsingANullPointer)
{
	utf8string* s = init_string(NULL);
	EXPECT_TRUE(s == NULL);
}


TEST_F(StringTest, AStringCanBeSearchedForASCIICharacters)
{
	const char* string = "abc\xD7\xAA\x78\xF0\x9F\x98\x8B\x64\xEA\xA6\x85xyz";
	utf8string* s = init_string(string);

	char character_c = 'c';
	int32_t pos_of_c = index_of(s, &character_c, 0);
	EXPECT_EQ(pos_of_c, 2);

	char character_y = 'y';
	int32_t pos_of_y = index_of(s, &character_y, 0);
	EXPECT_EQ(pos_of_y, 9);

	destroy_string(s);
}

TEST_F(StringTest, AStringCanBeSearchedForUTF8Characters)
{
	const char* string = "abc\xD7\xAA\x78\xF0\x9F\x98\x8B\x64\xEA\xA6\x85xyz";
	utf8string* s = init_string(string);

	const char utf_character[3] = { 0xEA, 0xA6, 0x85 };

	int32_t pos_of_utf3 = index_of(s, &utf_character[0], 0);
	EXPECT_EQ(pos_of_utf3, 7);

	destroy_string(s);
}

TEST_F(StringTest, AStringDoesNotContainAGivenCharacter)
{
	const char* string = "abcefgh";
	utf8string* s = init_string(string);

	char character_d = 'd';

	int32_t pos_of_d = index_of(s, &character_d, 0);
	EXPECT_EQ(pos_of_d, -1);

	destroy_string(s);
}

TEST_F(StringTest, AStringIndexOfUsingTheOffsetParameter)
{
	const char* string = "abcefgh";
	utf8string* s = init_string(string);

	char character_b = 'b';

	int32_t pos_of_b_offset0 = index_of(s, &character_b, 0);
	EXPECT_EQ(pos_of_b_offset0, 1);

	int32_t pos_of_b_offset1 = index_of(s, &character_b, 1);
	EXPECT_EQ(pos_of_b_offset1, 1);

	int32_t pos_of_b_offset2 = index_of(s, &character_b, 2);
	EXPECT_EQ(pos_of_b_offset2, -1);

	destroy_string(s);
}

TEST_F(StringTest, AStringIsDuplicated_ValidString)
{
	const char* string = "abc\xD7\xAA\x78\xF0\x9F\x98\x8B\x64\xEA\xA6\x85xyz";
	utf8string* s = init_string(string);

	utf8string* duplicate = duplicate_string(s);

	int32_t comparison_result = compare_strings(s, duplicate);
	EXPECT_EQ(comparison_result, 0);

	destroy_string(s);
	destroy_string(duplicate);
}

TEST_F(StringTest, AStringIsDuplicated_NullString)
{
	utf8string* duplicate = duplicate_string(NULL);
	EXPECT_TRUE(duplicate == NULL);
}

TEST_F(StringTest, AStringIsComparedWithAnIdenticalString)
{
	const char* string = "1234567890";
	utf8string* s1 = init_string(string);
	utf8string* s2 = init_string(string);

	int32_t comparison = compare_strings(s1, s2);
	EXPECT_EQ(comparison, 0);

	destroy_string(s1);
	destroy_string(s2);
}

TEST_F(StringTest, AStringIsComparedWithADifferentString)
{
	const char* string1 = "1234567890";
	const char* string2 = "1234567898";
	utf8string* s1 = init_string(string1);
	utf8string* s2 = init_string(string2);

	int32_t comparison = compare_strings(s1, s2);
	EXPECT_NE(comparison, 0);

	destroy_string(s1);
	destroy_string(s2);
}

TEST_F(StringTest, AStringIsComparedWithANullString)
{
	const char* string = "abc\xD7\xAA\x78\xF0\x9F\x98\x8B\x64\xEA\xA6\x85xyz";
	utf8string* s = init_string(string);

	int32_t comparison = compare_strings(s, NULL);
	EXPECT_NE(comparison, 0);
	destroy_string(s);
}

TEST_F(StringTest, SubstringFromValidRange)
{
	const char* string = "1234567890";
	utf8string* s = init_string(string);

	utf8string* substr = substring(s, 3, 7);

	const char* compare = "45678";
	EXPECT_EQ(6, substr->byte_length);
	EXPECT_EQ(5, substr->string_length);
	int32_t comparison_result = compare_to_charpointer(substr, compare);
	EXPECT_EQ(comparison_result, 0);

	destroy_string(substr);
	destroy_string(s);
}

TEST_F(StringTest, SubstringFromValidRange_UTF8Multibyte)
{
	const char* string = "\xD7\xAA\xD7\x95\xD7\x93\xD7\x94"; // 4 2-byte characters
	utf8string* s = init_string(string);

	utf8string* substr = substring(s, 1, 3);

	const char* compare = "\xD7\x95\xD7\x93\xD7\x94";
	EXPECT_EQ(7, substr->byte_length);
	EXPECT_EQ(3, substr->string_length);
	int32_t comparison_result = compare_to_charpointer(substr, compare);
	EXPECT_EQ(comparison_result, 0);

	destroy_string(substr);
	destroy_string(s);
}

TEST_F(StringTest, SubstringFromValidRange_UTF8MultibyteASCIIMix)
{
	const char* string = "\xD7\xAAza\xD7\x95yb\xD7\x93xc\xD7\x94wd"; // 2-byte characters mixed with triplets of ASCII
	utf8string* s = init_string(string);

	utf8string* substr = substring(s, 3, 6);

	const char* compare = "\xD7\x95yb\xD7\x93";
	EXPECT_EQ(7, substr->byte_length);
	EXPECT_EQ(4, substr->string_length);
	int32_t comparison_result = compare_to_charpointer(substr, compare);
	EXPECT_EQ(comparison_result, 0);

	destroy_string(substr);
	destroy_string(s);
}

TEST_F(StringTest, SubstringWithFirstInvalidIndex)
{
	const char* string = "1234567890";
	utf8string* s = init_string(string);

	utf8string* substr = substring(s, -1, 7);

	EXPECT_TRUE(substr == NULL);

	destroy_string(s);
}

TEST_F(StringTest, SubstringWithSecondInvalidIndex)
{
	const char* string = "1234567890";
	utf8string* s = init_string(string);

	utf8string* substr = substring(s, 1, 12);

	EXPECT_TRUE(substr == NULL);

	destroy_string(s);
}

TEST_F(StringTest, SubstringWithSecondIndexSmallerThanFirstIndex)
{
	const char* string = "1234567890";
	utf8string* s = init_string(string);

	utf8string* substr = substring(s, 6, 3);

	EXPECT_TRUE(substr == NULL);

	destroy_string(s);
}

TEST_F(StringTest, SubstringCalledOnEmptyString)
{
	utf8string* substr = substring(NULL, 1, 3);

	EXPECT_TRUE(substr == NULL);
}
/*
TEST_F(StringTest, ConcatenateWithOtherString)
{

}

TEST_F(StringTest, ConcatenateWithCharPointer)
{

}

TEST_F(StringTest, ConcatenateWithEmptyString)
{

}

TEST_F(StringTest, ConcatenateWithInvalidCharPointer)
{

}




TEST_F(StringTest, TokenizeReturnsValidSubstringSameCharacter)
{

}

TEST_F(StringTest, TokenizeReturnsValidSubstringChangingCharacters)
{

}
*/
