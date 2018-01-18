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
/*
TEST_F(StringTest, InitializeStringWithInvalidData)
{
	const char* string = "\xD7\xAA\xD7\x95\xD7\x93\xD7\x94";
}

TEST_F(StringTest, InitializeWithNullString)
{
	//isvalid false
}

TEST_F(StringTest, ValidateFixesExceededLength)
{

}

TEST_F(StringTest, ValidateFixesInvalidCodepoint)
{

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
