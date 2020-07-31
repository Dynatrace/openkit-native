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

#include "util/json/lexer/JsonLexer.h"
#include "util/json/lexer/JsonTokenType.h"
#include "../reader/MockResettableReader.h"
#include "core/util/StringUtil.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <exception>


using namespace util::json::lexer;

MATCHER_P2(isLiteralTokenOf, tokenType, value, "")
{
	(void)result_listener; // result_listener argument is unused, keep compiler quiet

	if (arg == nullptr)
	{
		return false;
	}
	if (arg->getTokenType() != tokenType)
	{
		return false;
	}

	return arg->getValue() == value;
}

MATCHER_P(isStructuralTokenOf, tokenType, "")
{
	(void)result_listener; // result_listener argument is unused, keep compiler quiet

	if (arg == nullptr)
	{
		return false;
	}
	if (arg->getTokenType() != tokenType)
	{
		return false;
	}

	return arg->getValue() == "";
}


class JsonLexerTest : public testing::Test
{
protected:

	static const std::string getNestedExceptionMessage(std::exception& exception)
	{
		try
		{
			std::rethrow_if_nested(exception);
		}
		catch (const std::exception& e)
		{
			return e.what();
		}

		throw std::logic_error("not a nested exception");
	}

	static void assertNextTokenThrowsLexerException(JsonLexer& lexer, const std::string& message)
	{
		try
		{
			lexer.nextToken();
			FAIL() << "Expected JsonLexerException to be thrown";
		}
		catch (JsonLexerException& e)
		{
			ASSERT_THAT(e.getMessage(), testing::Eq(message));
		}
	}
};

TEST_F(JsonLexerTest, lexingEmptyStringReturnsNullAsNextToken)
{
	// given
	auto target = JsonLexer("");

	// when
	auto obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, testing::IsNull());
}

TEST_F(JsonLexerTest ,lexingStringWithWhitespacesOnlyReturnsNullAsNextToken)
{
	// given
	auto target = JsonLexer(" \r\n\t");

	// when
	auto obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, testing::IsNull());
}

TEST_F(JsonLexerTest, lexingStructuralCharacterBeginArrayGivesExpectedToken)
{
	// given
	auto target = JsonLexer("[");

	// when
	auto obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isStructuralTokenOf(JsonTokenType::LEFT_SQUARE_BRACKET));
}

TEST_F(JsonLexerTest, lexingStructuralCharacterEndArrayGivesExpectedToken)
{
	// given
	auto target = JsonLexer("]");

	// when
	auto obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isStructuralTokenOf(JsonTokenType::RIGHT_SQUARE_BRACKET));
}

TEST_F(JsonLexerTest, lexingArrayTokenWithoutWhitespaceWorks)
{
	// given
	auto target = JsonLexer("[]");

	// when
	auto obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isStructuralTokenOf(JsonTokenType::LEFT_SQUARE_BRACKET));

	// and when
	obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isStructuralTokenOf(JsonTokenType::RIGHT_SQUARE_BRACKET));

	// and when
	obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, testing::IsNull());
}

TEST_F(JsonLexerTest, lexingArrayTokensWorksWithWhitespaces)
{
	// given
	auto target = JsonLexer(" \t[ \r\n]\t");

	// when
	auto obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isStructuralTokenOf(JsonTokenType::LEFT_SQUARE_BRACKET));

	// and when
	obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isStructuralTokenOf(JsonTokenType::RIGHT_SQUARE_BRACKET));

	// and when
	obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, testing::IsNull());
}

TEST_F(JsonLexerTest, lexingStructuralCharacterBeginObjectGivesExpectedToken)
{
	// given
	auto target = JsonLexer("{");

	// when
	auto obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isStructuralTokenOf(JsonTokenType::LEFT_BRACE));
}

TEST_F(JsonLexerTest, lexingStructuralCharacterEndObjectGivesExpectedToken)
{
	// given
	auto target = JsonLexer("}");

	// when
	auto obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isStructuralTokenOf(JsonTokenType::RIGHT_BRACE));
}

TEST_F(JsonLexerTest, lexingObjectTokensWithoutWhitespacesWorks)
{
	// given
	auto target = JsonLexer("{}");

	// when
	auto obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isStructuralTokenOf(JsonTokenType::LEFT_BRACE));

	// and when
	obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isStructuralTokenOf(JsonTokenType::RIGHT_BRACE));

	// and when
	obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, testing::IsNull());
}

TEST_F(JsonLexerTest, lexingObjectTokensWorksWithWhitespaces)
{
	// given
	auto target = JsonLexer(" \t{ \r\n}\t");

	// when
	auto obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isStructuralTokenOf(JsonTokenType::LEFT_BRACE));

	// and when
	obtained = target.nextToken();

	// when
	ASSERT_THAT(obtained, isStructuralTokenOf(JsonTokenType::RIGHT_BRACE));

	// and when
	obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, testing::IsNull());
}

TEST_F(JsonLexerTest, lexingNameSeparatorTokenGivesAppropriateToken)
{
	// given
	auto target = JsonLexer(":");

	// when
	auto obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isStructuralTokenOf(JsonTokenType::COLON));
}

TEST_F(JsonLexerTest, lexingValueSeparatorTokenGivesAppropriateToken)
{
	// given
	auto target = JsonLexer(",");

	// when
	auto obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isStructuralTokenOf(JsonTokenType::COMMA));
}

TEST_F(JsonLexerTest, lexingBooleanTrueLiteralGivesAppropriateToken)
{
	// given
	auto target = JsonLexer("true");

	// when
	auto obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isLiteralTokenOf(JsonTokenType::LITERAL_BOOLEAN, "true"));
}

TEST_F(JsonLexerTest, lexingBooleanTrueLiteralWithLeadingAndTrailingWhitespaces)
{
	// given
	auto target = JsonLexer("\t true \t");

	// when
	auto obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isLiteralTokenOf(JsonTokenType::LITERAL_BOOLEAN, "true"));

	// and when
	obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, testing::IsNull());
}

TEST_F(JsonLexerTest, lexingBooleanTrueLiteralWithWrongCasingThrowsAnError)
{
	// given
	auto target = JsonLexer("trUe");

	// when, then
	assertNextTokenThrowsLexerException(target, "Unexpected literal \"trUe\"");
}

TEST_F(JsonLexerTest, lexingBooleanFalseLiteralGivesAppropriateToken)
{
	// given
	auto target = JsonLexer("false");

	// when
	auto obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isLiteralTokenOf(JsonTokenType::LITERAL_BOOLEAN, "false"));
}

TEST_F(JsonLexerTest, lexingBooleanFalseLiteralWithLeadingAndTrailingWhitespaces)
{
	// given
	auto target = JsonLexer("\t false \t");

	// when
	auto obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isLiteralTokenOf(JsonTokenType::LITERAL_BOOLEAN, "false"));

	// and when
	obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, testing::IsNull());
}

TEST_F(JsonLexerTest, lexingNullLiteralGivesAppropriateToken)
{
	// given
	auto target = JsonLexer("null");

	// when
	auto obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isLiteralTokenOf(JsonTokenType::LITERAL_NULL, "null"));
}

TEST_F(JsonLexerTest, lexingNullLiteralWithLeadingAndTrailingWhitespacesGivesAppropriateToken)
{
	// given
	auto target = JsonLexer("\t\tnull\t\t");

	// when
	auto obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isLiteralTokenOf(JsonTokenType::LITERAL_NULL, "null"));

	// and when
	obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, testing::IsNull());
}

TEST_F(JsonLexerTest, lexingNullLiteralWithWrongCasingThrowsAnError)
{
	// given
	auto target = JsonLexer("nUll");

	// when, then
	assertNextTokenThrowsLexerException(target, "Unexpected literal \"nUll\"");
}

TEST_F(JsonLexerTest, lexingIntegerNumberGivesAppropriateToken)
{
	// given
	auto target = JsonLexer("42");

	// when
	auto obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isLiteralTokenOf(JsonTokenType::VALUE_NUMBER, "42"));
}

TEST_F(JsonLexerTest, lexingNegativeIntegerNumberGivesAppropriateToken)
{
	// given
	auto target = JsonLexer("-42");

	// when
	auto obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isLiteralTokenOf(JsonTokenType::VALUE_NUMBER, "-42"));
}

TEST_F(JsonLexerTest, lexingMinusSignWithoutSubsequentDigitsThrowsException)
{
	// given
	auto target = JsonLexer("-");

	// when, then
	assertNextTokenThrowsLexerException(target,  "Invalid number literal \"-\"");
}

TEST_F(JsonLexerTest, lexingIntegerNumberWithLeadingPlusThrowsException)
{
	// given
	auto target = JsonLexer("+42");

	// when
	assertNextTokenThrowsLexerException(target,  "Unexpected literal \"+42\"");
}

TEST_F(JsonLexerTest, lexingNumberWithLeadingYeroThrowsException)
{
	// given
	auto target = JsonLexer("01234");

	// when, then
	assertNextTokenThrowsLexerException(target, "Invalid number literal \"01234\"");
}

TEST_F(JsonLexerTest, lexingNumberWithFractionPartGivesAppropriateToken)
{
	// given
	auto target = JsonLexer("123.45");

	// when
	auto obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isLiteralTokenOf(JsonTokenType::VALUE_NUMBER, "123.45"));
}

TEST_F(JsonLexerTest, lexingNegativeNumberWithFractionPartGivesAppropriateToken)
{
	// given
	auto target = JsonLexer("-123.45");

	// when
	auto obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isLiteralTokenOf(JsonTokenType::VALUE_NUMBER, "-123.45"));
}

TEST_F(JsonLexerTest, lexingNumberWithDecimalSeparatorAndNoSubsequentDigitsThrowsException)
{
	// given
	auto target = JsonLexer("1234.");

	// when, then
	assertNextTokenThrowsLexerException(target, "Invalid number literal \"1234.\"");
}

TEST_F(JsonLexerTest, lexingNumberWithOnlyZerosInDecimalPartGivesAppropriateToken)
{
	// given
	auto target = JsonLexer("123.00");

	// when
	auto obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isLiteralTokenOf(JsonTokenType::VALUE_NUMBER, "123.00"));
}

TEST_F(JsonLexerTest, lexingNumberWithExponentGivesAppropriateToken)
{
	// given
	auto target = JsonLexer("1e3");

	// when
	auto obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isLiteralTokenOf(JsonTokenType::VALUE_NUMBER, "1e3"));
}

TEST_F(JsonLexerTest, lexingNumberWithUpperCaeExponentGivesAppropriateToken)
{
	// given
	auto target = JsonLexer("1E2");

	// when
	auto obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isLiteralTokenOf(JsonTokenType::VALUE_NUMBER, "1E2"));
}

TEST_F(JsonLexerTest, lexingNumberWithExplicitPositiveExponentGivesAppropriateToken)
{
	// given
	auto target = JsonLexer("1e+5");

	// when
	auto obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isLiteralTokenOf(JsonTokenType::VALUE_NUMBER, "1e+5"));
}

TEST_F(JsonLexerTest, lexingNumberWithExplicitPositiveUpperCaseExponentGivesAppropriateToken)
{
	// given
	auto target = JsonLexer("1E+5");

	// when
	auto obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isLiteralTokenOf(JsonTokenType::VALUE_NUMBER, "1E+5"));
}

TEST_F(JsonLexerTest, lexingNumberWithNegativeExponentGivesAppropriateToken)
{
	// given
	auto target = JsonLexer("1e-2");

	// when
	auto obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isLiteralTokenOf(JsonTokenType::VALUE_NUMBER, "1e-2"));
}

TEST_F(JsonLexerTest, lexingNumberWithNegativeUpperCaeExponentGivesAppropriateToken)
{
	// given
	auto target = JsonLexer("1E-2");

	// when
	auto obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isLiteralTokenOf(JsonTokenType::VALUE_NUMBER, "1E-2"));
}

TEST_F(JsonLexerTest, lexingNumberWithExponentAndNoDigitsThrowsException)
{
	// given
	auto target = JsonLexer("1e");

	// when, then
	assertNextTokenThrowsLexerException(target, "Invalid number literal \"1e\"");
}

TEST_F(JsonLexerTest, lexingNumberWithUpperCaseExponentAndNoDigitsThrowsException)
{
	// given
	auto target = JsonLexer("2E");

	// when, then
	assertNextTokenThrowsLexerException(target, "Invalid number literal \"2E\"");
}

TEST_F(JsonLexerTest, lexingNumberWithExponentFollowedByPlusAndNoDigitsThrowsException)
{
	// given
	auto target = JsonLexer("1e+");

	// when, then
	assertNextTokenThrowsLexerException(target, "Invalid number literal \"1e+\"");
}

TEST_F(JsonLexerTest, lexingNumberWithUpperCaseExponentFollowedByPlusAndNoDigitsThrowsException)
{
	// given
	auto target = JsonLexer("2E+");

	// when, then
	assertNextTokenThrowsLexerException(target, "Invalid number literal \"2E+\"");
}

TEST_F(JsonLexerTest, lexingNumberWithExponentFollowedByMinusAndNoDigitsThrowsException)
{
	// given
	auto target = JsonLexer("1e-");

	// when, then
	assertNextTokenThrowsLexerException(target, "Invalid number literal \"1e-\"");
}

TEST_F(JsonLexerTest, lexingNumberWithUpperCaseExponentFollowedByMinusAndNoDigitsThrowsException)
{
	// given
	auto target = JsonLexer("2E-");

	// when, then
	assertNextTokenThrowsLexerException(target, "Invalid number literal \"2E-\"");
}

TEST_F(JsonLexerTest, lexingNumberWithFractionAndExponentGivesAppropriateToken)
{
	// given
	auto target = JsonLexer("1.234e-2");

	// when
	auto obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isLiteralTokenOf(JsonTokenType::VALUE_NUMBER, "1.234e-2"));
}

TEST_F(JsonLexerTest, lexingNumberWithFractionAndUpperCaseExponentGivesAppropriateToken)
{
	// given
	auto target = JsonLexer("1.25E-3");

	// when
	auto obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isLiteralTokenOf(JsonTokenType::VALUE_NUMBER, "1.25E-3"));
}

TEST_F(JsonLexerTest, lexingNumberWithDecimalSeparatorImmediatelyFollowedByExponentThrowsException)
{
	// given
	auto target = JsonLexer("1.e-2");

	// when, then
	assertNextTokenThrowsLexerException(target, "Invalid number literal \"1.e-2\"");
}

TEST_F(JsonLexerTest, lexingNumberWithDecimalSeparatorImmediatelyFollowedByUpperCaseExponentThrowsException)
{
	// given
	auto target = JsonLexer("1.E-5");

	// when, then
	assertNextTokenThrowsLexerException(target, "Invalid number literal \"1.E-5\"");
}

TEST_F(JsonLexerTest, lexingStringGivesAppropriateToken)
{
	// given
	auto target = JsonLexer("\"foobar\"");

	// when
	auto obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isLiteralTokenOf(JsonTokenType::VALUE_STRING, "foobar"));
}

TEST_F(JsonLexerTest, lexingEmptyStringGivesAppropriateToken)
{
	// given
	auto target = JsonLexer("\"\"");

	// when
	auto obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isLiteralTokenOf(JsonTokenType::VALUE_STRING, ""));
}

TEST_F(JsonLexerTest, lexingUnterminatedStringThrowsException)
{
	// given
	auto target = JsonLexer("\"foo");

	// when, then
	assertNextTokenThrowsLexerException(target, "Unterminated string literal \"foo\"");
}

TEST_F(JsonLexerTest, lexingStringWithEscapedCharactersWorks)
{
	// given
	auto target = JsonLexer(R"("\u0000\u0001\u0010\n\"\\\/\b\f\n\r\t")");

	// when
	auto obtained = target.nextToken();

	// then
	std::stringstream stream;
	// gcc < version 6 treats \u0000 as \u0001 (see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53690) so use the hex representation
	stream << '\x0000' << "\u0001\u0010\n\"\\/\b\f\n\r\t";
	ASSERT_THAT(obtained, isLiteralTokenOf(JsonTokenType::VALUE_STRING, stream.str()));
}

TEST_F(JsonLexerTest, lexingStringWithInvalidEscapeSequenceThrowsException)
{
	// given
	auto target = JsonLexer(R"("Hello \a World!')");

	// when, then
	assertNextTokenThrowsLexerException(target, R"(Invalid escape sequence "\a")");
}

TEST_F(JsonLexerTest, lexingUnterminatedStringAfterEscapeSequenceThrowsAnException)
{
	// given
	auto target = JsonLexer("\"foo \\");

	// when, then
	assertNextTokenThrowsLexerException(target, "Unterminated string literal \"foo \"");
}

TEST_F(JsonLexerTest, lexingStringWithEscapeAsciiCharactersGivesAppropriateToken)
{
	// given
	auto target = JsonLexer(R"("\u0048\u0065\u006c\u006c\u006f\u0020\u0057\u006f\u0072\u006c\u0064\u0021")");

	// when
	auto obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isLiteralTokenOf(JsonTokenType::VALUE_STRING, "Hello World!"));
}

TEST_F(JsonLexerTest, lexingStringWithEscapedUpperCaseAsciiCharactersGivesAppropriateToken)
{
	// given
	auto target = JsonLexer(R"("\u0048\u0065\u006C\u006C\u006F\u0020\u0057\u006F\u0072\u006C\u0064\u0021")");

	// when
	auto obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isLiteralTokenOf(JsonTokenType::VALUE_STRING, "Hello World!"));
}

TEST_F(JsonLexerTest, lexingStringWithCharactersThatMustBeEscapedButAreNotThrowsAnException)
{
	// given
	auto target = JsonLexer("\"\n\"");

	// when, then
	assertNextTokenThrowsLexerException(target, R"(Invalid control character "\u000A")");
}

TEST_F(JsonLexerTest, lexingStringWithSurrogatePairGivesAppropriateToken)
{
	// given
	auto target = JsonLexer(R"("\u0048\u0065\u006C\u006C\u006F\u0020\uD834\uDD1E\u0021")");

	// when
	auto obtained = target.nextToken();

	// then
	std::basic_stringstream<char16_t> stream;
	stream << u"Hello " << static_cast<char16_t>(0xD834) << static_cast<char16_t>(0xDD1E) << u"!";
	std::u16string expectedUtf16String  = stream.str();

	auto expectedString = core::util::StringUtil::convertUtf16StringToUtf8String(expectedUtf16String);

	ASSERT_THAT(obtained, isLiteralTokenOf(JsonTokenType::VALUE_STRING, expectedString));
}

TEST_F(JsonLexerTest, lexingStringWithHighSurrogateOnlyThrowsAnException)
{
	// given
	auto target = JsonLexer(R"("\u0048\u0065\u006C\u006C\u006F\u0020\uD834\u0021")");

	// when, then
	assertNextTokenThrowsLexerException(target, R"(Invalid UTF-16 surrogate pair "\uD834")");
}

TEST_F(JsonLexerTest, lexingStringWithLowSurrogateOnlyThrowsAnException)
{
	// given
	auto target = JsonLexer(R"("\u0048\u0065\u006C\u006C\u006F\u0020\uDD1E\u0021")");

	// when, then
	assertNextTokenThrowsLexerException(target, R"(Invalid UTF-16 surrogate pair "\uDD1E")");
}

TEST_F(JsonLexerTest, lexingStringWithNonHexCharacterInUnicodeEscapeSequenceThrowsAnException)
{
	// given
	auto target = JsonLexer(R"("\u0048\u0065\u006C\u006C\u006F\u0020\uDDGE\u0021")");

	// when, then
	assertNextTokenThrowsLexerException(target, R"(Invalid unicode escape sequence "\uDDG")");
}

TEST_F(JsonLexerTest, lexingStringWithTooShortUnicodeEscapeSequenceThrowsAnException)
{
	// given
	auto target = JsonLexer(R"("\u007")");

	// when
	assertNextTokenThrowsLexerException(target, R"(Invalid unicode escape sequence "\u007"")");
}

TEST_F(JsonLexerTest, lexingCompoundTokenStringGivesTokensInAppropriateOrder)
{
	// given
	auto target = JsonLexer(R"({"asdf": [1234.45e-3, "a", null, true, false] } )");

	// when
	auto obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isStructuralTokenOf(JsonTokenType::LEFT_BRACE));

	// and when
	obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isLiteralTokenOf(JsonTokenType::VALUE_STRING, "asdf"));

	// and when
	obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isStructuralTokenOf(JsonTokenType::COLON));

	// and when
	obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isStructuralTokenOf(JsonTokenType::LEFT_SQUARE_BRACKET));

	// and when
	obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isLiteralTokenOf(JsonTokenType::VALUE_NUMBER, "1234.45e-3"));

	// and when
	obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isStructuralTokenOf(JsonTokenType::COMMA));

	// and when
	obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isLiteralTokenOf(JsonTokenType::VALUE_STRING, "a"));

	// and when
	obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isStructuralTokenOf(JsonTokenType::COMMA));

	// and when
	obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isLiteralTokenOf(JsonTokenType::LITERAL_NULL, "null"));

	// and when
	obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isStructuralTokenOf(JsonTokenType::COMMA));

	// and when
	obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isLiteralTokenOf(JsonTokenType::LITERAL_BOOLEAN, "true"));

	// and when
	obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isStructuralTokenOf(JsonTokenType::COMMA));

	// and when
	obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isLiteralTokenOf(JsonTokenType::LITERAL_BOOLEAN, "false"));

	// and when
	obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isStructuralTokenOf(JsonTokenType::RIGHT_SQUARE_BRACKET));

	// and when
	obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, isStructuralTokenOf(JsonTokenType::RIGHT_BRACE));

	// and when
	obtained = target.nextToken();

	// then
	ASSERT_THAT(obtained, testing::IsNull());
}

TEST_F(JsonLexerTest, ioFailuresAreCaughtAndTransformedToLexerExceptions)
{
	// given
	auto ioFailure = std::ios_base::failure("Does not work");
	auto reader = std::make_shared<testing::NiceMock<test::MockResettableReader>>();
	ON_CALL(*reader, read()).WillByDefault(testing::Throw(ioFailure));

	auto target = JsonLexer(reader);

	// when
	try
	{
		target.nextToken();
		FAIL() << "Expected JsonLexerException to be thrown";
	}
	catch (JsonLexerException& e)
	{
		ASSERT_THAT(e.getMessage(), testing::Eq("Exception when reading from input stream"));

		auto nestedExceptionMsg = getNestedExceptionMessage(e);
		ASSERT_THAT(nestedExceptionMsg, testing::Eq(ioFailure.what()));
	}
}

TEST_F(JsonLexerTest, requestingNextTokenAfterIoFailureHasBeenThrownThrowsAnException)
{
	// given
	auto ioFailure = std::ios_base::failure("Does not work");
	auto reader = std::make_shared<testing::NiceMock<test::MockResettableReader>>();
	ON_CALL(*reader, read()).WillByDefault(testing::Throw(ioFailure));

	auto target = JsonLexer(reader);

	// when
	try
	{
		target.nextToken();
		FAIL() << "Expected JsonLexerException to be thrown";
	}
	catch (JsonLexerException& e)
	{
		ASSERT_THAT(e.getMessage(), testing::Eq("Exception when reading from input stream"));

		auto nestedExceptionMsg = getNestedExceptionMessage(e);
		ASSERT_THAT(nestedExceptionMsg, testing::Eq(ioFailure.what()));
	}

	// and when requesting the next token a second time
	assertNextTokenThrowsLexerException(target,  "JSON Lexer is in erroneous state");
}

TEST_F(JsonLexerTest, requestingNextTokenAfterLexerExceptionHasBeenThrownThrowsAnException)
{
	// given
	auto target = JsonLexer("1. 1.234");

	// when requesting token the first time, then
	assertNextTokenThrowsLexerException(target, "Invalid number literal \"1.\"");

	// and when requesting the next token a second time
	assertNextTokenThrowsLexerException(target, "JSON Lexer is in erroneous state");
}