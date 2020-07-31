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

#include "core/util/StringUtil.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"


using StringUtil_t = core::util::StringUtil;

class StringUtilTest : public testing::Test
{
};

// trim tests ----------------------------------------------------------------------------------------------------------

TEST_F(StringUtilTest, leftTrimEmptyString)
{
	// given
	auto string = std::string("");

	// when
	StringUtil_t::leftTrim(string);

	// then
	ASSERT_THAT(string, testing::Eq(std::string("")));
}

TEST_F(StringUtilTest, rightTrimEmptyString)
{
	// given
	auto string = std::string("");

	// when
	StringUtil_t::rightTrim(string);

	// then
	ASSERT_THAT(string, testing::Eq(std::string("")));
}

TEST_F(StringUtilTest, trimEmptyString)
{
	// given
	auto string = std::string("");

	// when
	StringUtil_t::trim(string);

	//then
	ASSERT_THAT(string, testing::Eq(std::string("")));
}

TEST_F(StringUtilTest, leftTrimLeadingWhiteSpaces)
{
	// given
	auto string = std::string("  some string");

	//when
	StringUtil_t::leftTrim(string);

	// then
	ASSERT_THAT(string, testing::Eq(std::string("some string")));
}

TEST_F(StringUtilTest, leftTrimToCopyLeadingWhiteSpaces)
{
	// given
	auto string = std::string("  some string");

	// when
	auto trimmedString = StringUtil_t::leftTrimToCopy(string);

	// then
	ASSERT_THAT(&string, testing::Ne(&trimmedString));
	ASSERT_THAT(trimmedString, testing::Eq(std::string("some string")));
	ASSERT_THAT(string, testing::Eq(std::string("  some string")));
}

TEST_F(StringUtilTest, rightTrimLeadingWhiteSpaces)
{
	// given
	auto string = std::string("  some string");

	// when
	StringUtil_t::rightTrim(string);

	// then
	ASSERT_THAT(string, testing::Eq(std::string("  some string")));
}

TEST_F(StringUtilTest, rightTrimToCopyLadingWhiteSpaces)
{
	// given
	auto string = std::string("  some string");

	// when
	auto trimmedString = StringUtil_t::rightTrimToCopy(string);

	// then
	ASSERT_THAT(&string, testing::Ne(&trimmedString));
	ASSERT_THAT(trimmedString, testing::Eq(std::string("  some string")));
	ASSERT_THAT(string, testing::Eq(std::string("  some string")));
}

TEST_F(StringUtilTest, trimLeadingWhiteSpaces)
{
	// given
	auto string = std::string("  some string");

	// when
	StringUtil_t::trim(string);

	// then
	ASSERT_THAT(string, testing::Eq(std::string("some string")));
}

TEST_F(StringUtilTest, trimToCopyLeadingWhiteSpaces)
{
	// given
	auto string = std::string("  some string");

	// when
	auto trimmedString = StringUtil_t::trimToCopy(string);

	//then
	ASSERT_THAT(&string, testing::Ne(&trimmedString));
	ASSERT_THAT(trimmedString, testing::Eq(std::string("some string")));
	ASSERT_THAT(string, testing::Eq(std::string("  some string")));
}

TEST_F(StringUtilTest, leftTrimTrailingWhiteSpaces)
{
	// given
	auto string = std::string("some string  ");

	// when
	StringUtil_t::leftTrim(string);

	// then
	ASSERT_THAT(string, testing::Eq(std::string("some string  ")));
}

TEST_F(StringUtilTest,leftTrimToCopyTrailingWhiteSpaces)
{
	// given
	auto string = std::string("some string  ");

	// when
	auto trimmedString = StringUtil_t::leftTrimToCopy(string);

	// then
	ASSERT_THAT(&string, testing::Ne(&trimmedString));
	ASSERT_THAT(trimmedString, testing::Eq(std::string("some string  ")));
	ASSERT_THAT(string, testing::Eq(std::string("some string  ")));
}

TEST_F(StringUtilTest, rightTrimTrailingWhiteSpaces)
{
	// given
	auto string = std::string("some string  ");

	// when
	StringUtil_t::rightTrim(string);

	// then
	ASSERT_THAT(string, testing::Eq(std::string("some string")));
}

TEST_F(StringUtilTest, rightTrimToCopyTrailingWhiteSpaces)
{
	// given
	auto string = std::string("some string  ");

	// when
	auto trimmedString = StringUtil_t::rightTrimToCopy(string);

	// then
	ASSERT_THAT(&string, testing::Ne(&trimmedString));
	ASSERT_THAT(trimmedString, testing::Eq(std::string("some string")));
	ASSERT_THAT(string, testing::Eq(std::string("some string  ")));
}

TEST_F(StringUtilTest, trimTrailingWhiteSpaces)
{
	// given
	auto string = std::string("some string  ");

	// when
	StringUtil_t::trim(string);

	//then
	ASSERT_THAT(string, testing::Eq(std::string("some string")));
}

TEST_F(StringUtilTest, trimToCopyTrailingWhiteSpaces)
{
	// given
	auto string = std::string("some string  ");

	// when
	auto trimmedString = StringUtil_t::trimToCopy(string);

	// then
	ASSERT_THAT(&string, testing::Ne(&trimmedString));
	ASSERT_THAT(trimmedString, testing::Eq(std::string("some string")));
	ASSERT_THAT(string, testing::Eq(std::string("some string  ")));
}

TEST_F(StringUtilTest, leftTrimLeadingAndTrailingWhiteSpaces)
{
	// given
	auto string = std::string("  some string  ");

	// when
	StringUtil_t::leftTrim(string);

	// then
	ASSERT_THAT(string, testing::Eq(std::string("some string  ")));
}

TEST_F(StringUtilTest, leftTrimToCopyLeadingAndTrailingWhiteSpaces)
{
	// given
	auto string = std::string("  some string  ");

	// when
	auto trimmedString = StringUtil_t::leftTrimToCopy(string);

	//then
	ASSERT_THAT(&string, testing::Ne(&trimmedString));
	ASSERT_THAT(trimmedString, testing::Eq(std::string("some string  ")));
	ASSERT_THAT(string, testing::Eq(std::string("  some string  ")));
}

TEST_F(StringUtilTest, rightTrimLeadingAndTrailingWhiteSpaces)
{
	// given
	auto string = std::string("  some string  ");

	// when
	StringUtil_t::rightTrim(string);

	// then
	ASSERT_THAT(string, testing::Eq(std::string("  some string")));
}

TEST_F(StringUtilTest,rightTrimToCopyLeadingAndTrailingWhiteSpaces)
{
	// given
	auto string = std::string("  some string  ");

	// when
	auto trimmedString = StringUtil_t::rightTrimToCopy(string);

	// then
	ASSERT_THAT(&string, testing::Ne(&trimmedString));
	ASSERT_THAT(trimmedString, testing::Eq(std::string("  some string")));
	ASSERT_THAT(string, testing::Eq(std::string("  some string  ")));
}


TEST_F(StringUtilTest, trimLeadingAndTrailingWhiteSpaces)
{
	// given
	auto string = std::string("  some string  ");

	// when
	StringUtil_t::trim(string);

	//then
	ASSERT_THAT(string, testing::Eq(std::string("some string")));
}

TEST_F(StringUtilTest, trimToCopyLeadingAndTrailingWhiteSpaces)
{
	// given
	auto string = std::string("  some string  ");

	// when
	auto trimmedString = StringUtil_t::trimToCopy(string);

	// then
	ASSERT_THAT(&string, testing::Ne(&trimmedString));
	ASSERT_THAT(trimmedString, testing::Eq(std::string("some string")));
	ASSERT_THAT(string, testing::Eq(std::string("  some string  ")));
}

// hashing tests -------------------------------------------------------------------------------------------------------

TEST_F(StringUtilTest, toNumericOr64BitHashWithNullString)
{
	// given, when
	auto hash = StringUtil_t::toNumericOr64BitHash(nullptr);

	// then
	ASSERT_THAT(hash, testing::Eq(0));
}

TEST_F(StringUtilTest, toNumericOr64BitHashWithEmptyString)
{
	// given, when
	auto hash = StringUtil_t::toNumericOr64BitHash("");

	// then
	ASSERT_THAT(hash, testing::Eq(0));
}

TEST_F(StringUtilTest, toNumericOr64BitHashWithEmptyWhitespaceString)
{
	// given, when
	auto hash = StringUtil_t::toNumericOr64BitHash("  ");

	// then
	ASSERT_THAT(hash, testing::Eq(0));
}

TEST_F(StringUtilTest, toNumericOr64BitHashWithNumericString)
{
	// given, when
	auto hash = StringUtil_t::toNumericOr64BitHash("42");

	// then
	ASSERT_THAT(hash, testing::Eq(42));
}

TEST_F(StringUtilTest, toNumeric64BitHashWithNonNumericString)
{
	// given, when
	auto hash = StringUtil_t::toNumericOr64BitHash("abcd");

	// then
	ASSERT_THAT(hash, testing::Gt(0));
}

TEST_F(StringUtilTest, toNumericOr64BithashWithAlphanumericString)
{
	// given, when
	auto hash = StringUtil_t::toNumericOr64BitHash("42abcd");

	// then
	ASSERT_THAT(hash, testing::Ne(42));
	ASSERT_THAT(hash, testing::Gt(0));
}

TEST_F(StringUtilTest, to64BitHashWithEmptyString)
{
	// given, when
	auto hash = StringUtil_t::to64BitHash(std::string(""));

	// then
	ASSERT_THAT(hash, testing::Eq(0));
}

TEST_F(StringUtilTest, to64BitHashDifferentStringsDifferentHash)
{
	// given
	auto firstString = std::string("some string");
	auto secondString = std::string("some other string");

	// when
	auto firstHash = StringUtil_t::to64BitHash(firstString);
	auto secondHash = StringUtil_t::to64BitHash(secondString);

	// then
	ASSERT_THAT(firstHash, testing::Ne(secondHash));
}

TEST_F(StringUtilTest, to64BitHashEqualStringsSameHash)
{
	// given
	auto firstString = std::string("str");
	auto secondString = std::string("str");

	// when
	auto firstHash = StringUtil_t::to64BitHash(firstString);
	auto secondHash = StringUtil_t::to64BitHash(secondString);

	// then
	ASSERT_THAT(&firstString, testing::Ne(&secondString));
	ASSERT_THAT(firstString, testing::Eq(secondString));
	ASSERT_THAT(firstHash, testing::Eq(secondHash));
}

TEST_F(StringUtilTest, to64BitHashCaseSensitiveStringsDifferentHash)
{
	// given
	auto lowerCase = std::string("value");
	auto upperCase = std::string("Value");

	// when
	auto lowerCaseHash = StringUtil_t::to64BitHash(lowerCase);
	auto upperCaseHash = StringUtil_t::to64BitHash(upperCase);

	// then
	ASSERT_THAT(lowerCaseHash, testing::Ne(upperCaseHash));
}

TEST_F(StringUtilTest, to64BitHashWithEmptyWhiteSpaceString)
{
	// given, when
	auto hash = StringUtil_t::to64BitHash(std::string("  "));

	// then
	ASSERT_THAT(hash, testing::Eq(0));
}

TEST_F(StringUtilTest, to64BitHashWithEvenString)
{
	// given, when
	auto hash = StringUtil_t::to64BitHash(std::string("even"));

	// then
	ASSERT_THAT(hash, testing::Gt(0));
}

TEST_F(StringUtilTest, to64BitHashWithOddString)
{
	// given, when
	auto hash = StringUtil_t::to64BitHash(std::string("odd"));

	// then
	ASSERT_THAT(hash, testing::Gt(0));
}

// toInvariantString tests ---------------------------------------------------------------------------------------------

TEST_F(StringUtilTest, int32ToInvariantString)
{
	// given
	const int32_t number = -12345;

	// when
	auto obtained = StringUtil_t::toInvariantString(number);

	// then
	ASSERT_THAT(obtained, testing::StrEq("-12345"));
}

TEST_F(StringUtilTest, uint32ToInvariantString)
{
	// given
	const uint32_t number = 98765;

	// when
	auto obtained = StringUtil_t::toInvariantString(number);

	// then
	ASSERT_THAT(obtained, testing::StrEq("98765"));
}

TEST_F(StringUtilTest, int64ToInvariantString)
{
	// given
	const int64_t number = -123456789;

	// when
	auto obtained = StringUtil_t::toInvariantString(number);

	// then
	ASSERT_THAT(obtained, testing::StrEq("-123456789"));
}

TEST_F(StringUtilTest, uint64ToInvariantString)
{
	// given
	const uint64_t number = 123456789012;

	// when
	auto obtained = StringUtil_t::toInvariantString(number);

	// then
	ASSERT_THAT(obtained, testing::StrEq("123456789012"));
}

TEST_F(StringUtilTest, floatToInvariantString)
{
	// given
	const float number = 0.125;

	// when
	auto obtained = StringUtil_t::toInvariantString(number);

	// then
	ASSERT_THAT(obtained, testing::StrEq("0.125"));
}

TEST_F(StringUtilTest, integralFloatToInvariantString)
{
	// given
	const float number = 1234;

	// when
	auto obtained = StringUtil_t::toInvariantString(number);

	// then
	ASSERT_THAT(obtained, testing::StrEq("1234"));
}

TEST_F(StringUtilTest, doubleToInvariantString)
{
	// given
	const double number = -2.875;

	// when
	auto obtained = StringUtil_t::toInvariantString(number);

	// then
	ASSERT_THAT(obtained, testing::StrEq("-2.875"));
}

TEST_F(StringUtilTest, integralDoubleToInvariantString)
{
	// given
	const double number = 271828;

	// when
	auto obtained = StringUtil_t::toInvariantString(number);

	// then
	ASSERT_THAT(obtained, testing::StrEq("271828"));
}