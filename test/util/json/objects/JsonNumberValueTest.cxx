/**
 * Copyright 2018-2020 Dynatrace LLC
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

#include "util/json/objects/JsonNumberValue.h"

#include <cstdint>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace util::json::objects;

class JsonNumberValueTest : public testing::Test
{
protected:
	double pi = 3.14159265358979323846;
	double euler = 2.71828182845904523536;
};

TEST_F(JsonNumberValueTest, isNumberType)
{
	ASSERT_THAT(JsonNumberValue::fromLong(0)->getValueType(), testing::Eq(JsonValueType::NUMBER_VALUE));
}

TEST_F(JsonNumberValueTest, constructingJsonNumberValueWithLongValueSetsIsIntegerFlagToTrue)
{
	// given
	auto target = JsonNumberValue::fromLong(42);

	// then
	ASSERT_THAT(target->isInteger(), testing::Eq(true));
}

TEST_F(JsonNumberValueTest, constructingJsonNumberValueWithDoubleValueSetsIsIntegerFlagToFalse)
{
	// given
	auto target = JsonNumberValue::fromDouble(42.0);

	// then
	ASSERT_THAT(target->isInteger(), testing::Eq(false));
}

TEST_F(JsonNumberValueTest, isInt32ValueReturnsTrueIfValueIsAnIntegerAndInRangeOfInt32)
{
	// when min int32_t value is used, then
	ASSERT_THAT(JsonNumberValue::fromLong(INT32_MIN)->isInt32Value(), testing::Eq(true));

	// when max int32_t value is used, then
	ASSERT_THAT(JsonNumberValue::fromLong(INT32_MAX)->isInt32Value(), testing::Eq(true));
}

TEST_F(JsonNumberValueTest, isInt32ValueReturnsFalseIfIntegerIsOutsideRangeOfInt32)
{
	// when one less than int32_t min value is used, then
	ASSERT_THAT(JsonNumberValue::fromLong(INT32_MIN - int64_t(1L))->isInt32Value(), testing::Eq(false));

	// when one more than int32_t max value is used, then
	ASSERT_THAT(JsonNumberValue::fromLong(INT32_MAX + int64_t(1L))->isInt32Value(), testing::Eq(false));
}

TEST_F(JsonNumberValueTest,isInt32ValueReturnsFalseIfValueIsAFloatingPointValue)
{
	// when it actually fits into an integer, then
	ASSERT_THAT(JsonNumberValue::fromDouble(42)->isInt32Value(), testing::Eq(false));

	// when it is really a floating point value, then
	ASSERT_THAT(JsonNumberValue::fromDouble(3.14159)->isInt32Value(), testing::Eq(false));
}

TEST_F(JsonNumberValueTest, getInt32ValueReturns32BitIntValue)
{
	// when constructed from a long that fits into the int 32 range
	ASSERT_THAT(JsonNumberValue::fromLong(INT32_MIN)->getInt32Value(), testing::Eq(INT32_MIN));
}

TEST_F(JsonNumberValueTest, getInt32ValueReturnsCastedValueFromLong)
{
	// when constructed from a long that does not fit into 32-bit, then
	ASSERT_THAT(JsonNumberValue::fromLong(0x11111111deadbabeL)->getInt32Value(), testing::Eq(static_cast<int32_t>(0xdeadbabe)));
}

TEST_F(JsonNumberValueTest, getInt32ValueReturnsCastedValueFromDouble)
{
	// when constructed from a double with fractional part, then
	ASSERT_THAT(JsonNumberValue::fromDouble(pi)->getInt32Value(), testing::Eq(3));

	// and when constructed from another double with fractional part, then
	ASSERT_THAT(JsonNumberValue::fromDouble(euler)->getInt32Value(), testing::Eq(2));
}

TEST_F(JsonNumberValueTest, getLongValueReturnsValueWhenConstructedFromLong)
{
	// when, then
	ASSERT_THAT(JsonNumberValue::fromLong(0xFEEDFACECAFEBEEFL)->getLongValue(), testing::Eq(-77129852519530769L));
}

TEST_F(JsonNumberValueTest, getLongValueReturnsTruncatedValueWhenConstructedFromDouble)
{
	// when constructed from a double with fractional part, then
	ASSERT_THAT(JsonNumberValue::fromDouble(pi)->getLongValue(), testing::Eq(3));

	// and when constructed from another double with fractional part, then
	ASSERT_THAT(JsonNumberValue::fromDouble(euler)->getLongValue(), testing::Eq(2));
}

TEST_F(JsonNumberValueTest, getFloatValueReturns32BitFloatingPointValue)
{
	// when
	// then (expected value from wikipedia)
	ASSERT_THAT(JsonNumberValue::fromDouble(pi)->getFloatValue(), testing::Eq(3.14159274101F));
}

TEST_F(JsonNumberValueTest, getFloatValueReturns64BitFloatingPointValue)
{
	// when, then
	ASSERT_THAT(JsonNumberValue::fromDouble(pi)->getDoubleValue(), testing::Eq(pi));

	// and when, then
	ASSERT_THAT(JsonNumberValue::fromDouble(euler)->getDoubleValue(), testing::Eq(euler));
}

TEST_F(JsonNumberValueTest, fromNumberLiteralReturnsNullIfLiteralIsInvalid)
{
	// given
	auto emptyString = std::string("");
	auto textString = std::string("foobar");
	auto alphaNumericString = std::string("1234foo");

	// when constructed with empty string, then
	ASSERT_THAT(JsonNumberValue::fromNumberLiteral(emptyString), testing::IsNull());

	// and when constructed with arbitrary string, then
	ASSERT_THAT(JsonNumberValue::fromNumberLiteral(textString), testing::IsNull());

	// and when  constructed with alpha-numeric string, then
	ASSERT_THAT(JsonNumberValue::fromNumberLiteral(alphaNumericString), testing::IsNull());
}

TEST_F(JsonNumberValueTest, fromNumberLiteralReturnsIntegerRepresentationForIntegerNumbers)
{
	// given
	auto integerString = std::string("1234567890");

	// when constructed from positive integer literal
	auto obtained = JsonNumberValue::fromNumberLiteral(integerString);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->isInteger(), testing::Eq(true));
	ASSERT_THAT(obtained->getLongValue(), testing::Eq(1234567890));
}

TEST_F(JsonNumberValueTest, fromNumberLiteralReturnsNullForUnparsableLong)
{
	// given
	auto overLongIntegerString = std::string("92233720368547758089");

	// when
	auto obtained = JsonNumberValue::fromNumberLiteral(overLongIntegerString);

	// then
	ASSERT_THAT(obtained, testing::IsNull());
}

TEST_F(JsonNumberValueTest, fromNumberLiteralReturnsDoubleIfLiteralContainsFractionPart)
{
	// given
	auto floatingPointString = std::string("1.25");

	// when constructed from a positive floating point literal
	auto obtained = JsonNumberValue::fromNumberLiteral(floatingPointString);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->isInteger(), testing::Eq(false));
	ASSERT_THAT(obtained->getDoubleValue(), testing::Eq(1.25));
}

TEST_F(JsonNumberValueTest, fromNumberLiteralReturnsDoubleIfLiteralContainsExponentPart)
{
	// given
	auto floatingPointString = std::string("15E-1");

	// when constructed from positive floating point literal
	auto obtained = JsonNumberValue::fromNumberLiteral(floatingPointString);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->isInteger(), testing::Eq(false));
	ASSERT_THAT(obtained->getDoubleValue(), testing::Eq(1.5));
}

TEST_F(JsonNumberValueTest, fromNumberLiteralReturnsDoubleIfLiteralContainsFractionAndExponentPart)
{
	// given
	auto floatingPointString = std::string("0.0625e+2");

	// when constructed from positive floating point literal
	auto obtained = JsonNumberValue::fromNumberLiteral(floatingPointString);

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained->isInteger(), testing::Eq(false));
	ASSERT_THAT(obtained->getDoubleValue(), testing::Eq(6.25));
}