extern "C" {
#include "utf8string.h"
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

	ASSERT_EQ(s->data[0], 't');
	ASSERT_EQ(s->data[1], 'e');
	ASSERT_EQ(s->data[2], 's');
	ASSERT_EQ(s->data[3], 't');
	ASSERT_EQ(s->data[4], '1');
	ASSERT_EQ(s->data[5], '2');
	ASSERT_EQ(s->data[6], '3');
	ASSERT_EQ(s->data[7], '\0');

	ASSERT_EQ(s->string_length, 7);
	ASSERT_EQ(s->byte_length, 8);
	destroy_string(s);
}


TEST_F(StringTest, AStringCanBeInitializedWithFour2ByteCharacters)
{
	const char* string = "\xD7\xAA\xD7\x95\xD7\x93\xD7\x94";
	utf8string* s = init_string(string);

	ASSERT_EQ(s->data[0], (char)0xD7);
	ASSERT_EQ(s->data[1], (char)0xAA);
	ASSERT_EQ(s->data[2], (char)0xD7);
	ASSERT_EQ(s->data[3], (char)0x95);
	ASSERT_EQ(s->data[4], (char)0xD7);
	ASSERT_EQ(s->data[5], (char)0x93);
	ASSERT_EQ(s->data[6], (char)0xD7);
	ASSERT_EQ(s->data[7], (char)0x94);
	ASSERT_EQ(s->data[8], '\0');

	ASSERT_EQ(s->string_length, 4);
	ASSERT_EQ(s->byte_length, 9);
	destroy_string(s);
}

TEST_F(StringTest, AStringCanBeInitializedByReplacingInvalidUTF8FirstByte)
{
	//third character (is ASCII )breaks the 2nd UTF8 character ( 2 byte wide)
	const char* string = "\xD7\xAAy\x95\xD7\x93\xD7\x94";
	utf8string* s = init_string(string);

	ASSERT_EQ(s->data[0], (char)0xD7);
	ASSERT_EQ(s->data[1], (char)0xAA);
	ASSERT_EQ(s->data[2], 'y');
	ASSERT_EQ(s->data[3], '?');
	ASSERT_EQ(s->data[4], (char)0xD7);
	ASSERT_EQ(s->data[5], (char)0x93);
	ASSERT_EQ(s->data[6], (char)0xD7);
	ASSERT_EQ(s->data[7], (char)0x94);

	ASSERT_EQ(s->string_length, 5);
	ASSERT_EQ(s->byte_length, 9);
	destroy_string(s);
}

TEST_F(StringTest, AStringCanBeInitializedByReplacingInvalidUTF8SecondByte)
{
	//third character (is ASCII )breaks the 2nd UTF8 character ( 2 byte wide)
	const char* string = "\xD7\xAA\xD7r\xD7\x93\xD7\x94";
	utf8string* s = init_string(string);

	ASSERT_EQ(s->data[0], (char)0xD7);
	ASSERT_EQ(s->data[1], (char)0xAA);
	ASSERT_EQ(s->data[2], '?');
	ASSERT_EQ(s->data[3], 'r');
	ASSERT_EQ(s->data[4], (char)0xD7);
	ASSERT_EQ(s->data[5], (char)0x93);
	ASSERT_EQ(s->data[6], (char)0xD7);
	ASSERT_EQ(s->data[7], (char)0x94);

	ASSERT_EQ(s->string_length, 5);
	ASSERT_EQ(s->byte_length, 9);
	destroy_string(s);
}


TEST_F(StringTest, AStringCanBeInitializedUsingATwoByteAndAFourByteUTF8)
{
	//third character (is ASCII )breaks the 2nd UTF8 character ( 2 byte wide)
	const char* string = "\xD7\xAA\xf0\x9f\x98\x8b";
	utf8string* s = init_string(string);

	ASSERT_EQ(s->data[0], (char)0xD7);// 1/2
	ASSERT_EQ(s->data[1], (char)0xAA);// 2/2
	ASSERT_EQ(s->data[2], (char)0xF0);// 1/4
	ASSERT_EQ(s->data[3], (char)0x9F);// 2/4
	ASSERT_EQ(s->data[4], (char)0x98);// 3/4
	ASSERT_EQ(s->data[5], (char)0x8B);// 4/4

	ASSERT_EQ(s->string_length, 2);
	ASSERT_EQ(s->byte_length, 7);
	destroy_string(s);
}

TEST_F(StringTest, AStringCanBeInitializedUsingAOneByteUTF8)
{
	const char* string = "\x61\x62\x63\x72\x74\x78";
	utf8string* s = init_string(string);

	ASSERT_EQ(s->data[0], 'a');
	ASSERT_EQ(s->data[1], 'b');
	ASSERT_EQ(s->data[2], 'c');
	ASSERT_EQ(s->data[3], 'r');
	ASSERT_EQ(s->data[4], 't');
	ASSERT_EQ(s->data[5], 'x');

	ASSERT_EQ(s->string_length, 6);
	ASSERT_EQ(s->byte_length, 7);
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

	ASSERT_EQ(s->data[0], 'a');
	ASSERT_EQ(s->data[1], 'b');
	ASSERT_EQ(s->data[2], 'c');
	ASSERT_EQ(s->data[3], (char)0xD7); // 1/2
	ASSERT_EQ(s->data[4], (char)0xAA); // 2/2
	ASSERT_EQ(s->data[5], (char)0x78); // 1/1
	ASSERT_EQ(s->data[6], (char)0xF0); // 1/4
	ASSERT_EQ(s->data[7], (char)0x9F); // 2/4
	ASSERT_EQ(s->data[8], (char)0x98); // 3/4
	ASSERT_EQ(s->data[9], (char)0x8B); // 4/4
	ASSERT_EQ(s->data[10], (char)0x64);// 1/1
	ASSERT_EQ(s->data[11], (char)0xEA);// 1/3
	ASSERT_EQ(s->data[12], (char)0xA6);// 2/3
	ASSERT_EQ(s->data[13], (char)0x85); // 3/3
	ASSERT_EQ(s->data[14], 'x');
	ASSERT_EQ(s->data[15], 'y');
	ASSERT_EQ(s->data[16], 'z');

	ASSERT_EQ(s->string_length, 11);
	ASSERT_EQ(s->byte_length, 18);
	destroy_string(s);
}

TEST_F(StringTest, AStringCanBeInitializedWithABrokenThreeByteUTF8FollowedByaTwoByteUTF8)
{
	//valid \xEA\xA6\x85 \xD7\xAA
	const char* string = "\xEA\xA6\xD7\xAA";
	utf8string* s = init_string(string);

	ASSERT_EQ(s->data[0], '?');
	ASSERT_EQ(s->data[1], (char)0xD7);
	ASSERT_EQ(s->data[2], (char)0xAA);
	
	ASSERT_EQ(s->string_length, 2);
	ASSERT_EQ(s->byte_length, 4);
	destroy_string(s);
}

TEST_F(StringTest, AStringCanBeInitializedWhenTwoOfThreeMultiByteCharactersAreBroken)
{
	//valid \xea\xa6\x8a \xea\xa6\x8d \xea\xa6\x90
	const char* string = "\xea\xa6\xe6\x8d\xea\xa6\x90";

	utf8string* s = init_string(string);

	ASSERT_EQ(s->data[0], '?');
	ASSERT_EQ(s->data[1], '?');
	ASSERT_EQ(s->data[2], (char)0xEA);
	ASSERT_EQ(s->data[3], (char)0xA6);
	ASSERT_EQ(s->data[4], (char)0x90);

	ASSERT_EQ(s->string_length, 3);
	ASSERT_EQ(s->byte_length, 6);
	destroy_string(s);
}
/*
TEST_F(StringTest, InitializeWithNullString)
{
	//isvalid false
}


TEST_F(StringTest, GetStringLengthASCII)
{

}

TEST_F(StringTest, GetStringLengthUTF8)
{

}

TEST_F(StringTest, StringsAreCutAt250CharacterBorder)
{

}

TEST_F(StringTest, CompareEqual)
{

}

TEST_F(StringTest, CompareDifferent)
{

}

TEST_F(StringTest, DuplicateValidString)
{

}

TEST_F(StringTest, DuplicateNullString)
{

}

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

TEST_F(StringTest, IndexOfExistingCharacter)
{

}

TEST_F(StringTest, IndexOfNonExisitingCharacter)
{

}

TEST_F(StringTest, SubstringFromValidRange)
{

}

TEST_F(StringTest, SubstringWithFirstInvalidIndex)
{

}

TEST_F(StringTest, SubstringWithSecondInvalidIndex)
{

}

TEST_F(StringTest, SubstringCalledOnEmptyString)
{

}

TEST_F(StringTest, TokenizeReturnsValidSubstringSameCharacter)
{

}

TEST_F(StringTest, TokenizeReturnsValidSubstringChangingCharacters)
{

}
*/
