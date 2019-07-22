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

#include "util/json/reader/DefaultResettableReader.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace util::json::reader;

#define ASSERT_THROWS_EXCEPTION(targetCall, exception, message)		\
	try																\
	{																\
		targetCall;													\
		FAIL() << "Expected exception to be throw";					\
	}																\
	catch(exception& e)												\
	{																\
		ASSERT_THAT(e.what(), testing::Eq(std::string(message)));	\
	}

class DefaultResettableReaderTest : public testing::Test
{
};

TEST_F(DefaultResettableReaderTest, readSingleCharacter)
{
	// given
	auto target = DefaultResettableReader("Hello");

	// when
	auto obtained = target.read();

	// then
	ASSERT_THAT(obtained, testing::Eq('H'));
}

TEST_F(DefaultResettableReaderTest, readAllCharacters)
{
	// given
	auto input = std::string("Hello");
	auto target = DefaultResettableReader(input);

	for (size_t i = 0; i < input.length(); i++)
	{
		// when
		auto obtained = target.read();

		// then
		ASSERT_THAT(obtained, testing::Eq(input.at(i)));
	}

	// and when
	auto obtained = target.read();

	// then
	ASSERT_THAT(obtained, testing::Eq(EOF));
}

TEST_F(DefaultResettableReaderTest, readAfterMarkAndResettingSingleCharacter)
{
	// given
	auto input = std::string("abc");
	auto target = DefaultResettableReader(input);

	// when
	target.mark(1);
	auto obtained = target.read();

	// then
	ASSERT_THAT(obtained, testing::Eq('a'));

	// and when
	target.reset();

	for (size_t i = 0; i < input.length(); i++)
	{
		// when
		obtained = target.read();

		// then
		ASSERT_THAT(obtained, testing::Eq(input.at(i)));
	}

	// and when
	obtained = target.read();

	// then
	ASSERT_THAT(obtained, testing::Eq(EOF));
}

TEST_F(DefaultResettableReaderTest, readAfterMarkAndResettingTwoCharactersWithEvenReadOffset)
{
	// given
	auto input = std::string("abc");
	auto target = DefaultResettableReader(input);

	// when
	target.mark(2);
	auto obtained = target.read();

	// then
	ASSERT_THAT(obtained, testing::Eq('a'));

	// and when
	obtained = target.read();

	// then
	ASSERT_THAT(obtained, testing::Eq('b'));

	// and when
	target.reset();

	for (size_t i = 0; i < input.length(); i++)
	{
		// when
		obtained = target.read();

		// then
		ASSERT_THAT(obtained, testing::Eq(input.at(i)));
	}

	// and when
	obtained = target.read();

	// then
	ASSERT_THAT(obtained, testing::Eq(EOF));
}

TEST_F(DefaultResettableReaderTest, readAfterMarkAndResettingTwoCharactersWithOddReadOffset)
{
	// given
	auto input = std::string("abcd");
	auto target = DefaultResettableReader(input);

	// when
	auto obtained = target.read();

	// then
	ASSERT_THAT(obtained, testing::Eq('a'));

	// and when
	target.mark(2);
	obtained = target.read();

	// then
	ASSERT_THAT(obtained, testing::Eq('b'));

	// and when
	obtained = target.read();

	// then
	ASSERT_THAT(obtained, testing::Eq('c'));

	// and when
	target.reset();

	for (size_t i = 1; i < input.length(); i++)
	{
		// when
		obtained = target.read();

		// then
		ASSERT_THAT(obtained, testing::Eq(input.at(i)));
	}

	// and when
	obtained = target.read();

	// then
	ASSERT_THAT(obtained, testing::Eq(EOF));
}

TEST_F(DefaultResettableReaderTest, markWithLowerReadAheadLimitThanPreviousMark)
{
	// given
	auto input = std::string("abcd");
	auto target = DefaultResettableReader(input);

	// when
	target.mark(2);

	// then
	for (size_t i = 0; i < 2; i++)
	{
		auto obtained = target.read();
		ASSERT_THAT(obtained, testing::Eq(input.at(i)));
	}

	// and when
	target.reset();

	for (size_t i = 0; i < 3; i++)
	{
		// and when
		target.mark(1);
		auto obtained = target.read();

		// then
		ASSERT_THAT(obtained, testing::Eq(input.at(i)));

		// and when
		target.reset();
		obtained = target.read();

		// then
		ASSERT_THAT(obtained, testing::Eq(input.at(i)));
	}

	// and when reading to end, then
	for (size_t i = 3; i < input.length(); i++)
	{
		auto obtained = target.read();
		ASSERT_THAT(obtained, testing::Eq(input.at(i)));
	}

	// and finally when
	auto obtained = target.read();

	// then
	ASSERT_THAT(obtained, testing::Eq(EOF));
}

TEST_F(DefaultResettableReaderTest, markWithHigherReadAheadLimitThanPreviousMarkOnEvenPosition)
{
	// given
	auto input = std::string("abcde");
	auto target = DefaultResettableReader(input);

	// when
	target.mark(1);
	auto obtained = target.read();

	// then
	ASSERT_THAT(obtained, testing::Eq('a'));

	// and when reading from reset reader at even position
	target.reset();
	target.mark(2);
	obtained = target.read();

	// then
	ASSERT_THAT(obtained, testing::Eq('a'));

	// and when
	obtained = target.read();

	// then
	ASSERT_THAT(obtained, testing::Eq('b'));

	// and when
	target.reset();

	// then
	for (size_t i = 0; i < input.length(); i++)
	{
		obtained = target.read();
		ASSERT_THAT(obtained, testing::Eq(input.at(i)));
	}

	// and when
	obtained = target.read();

	/// then
	ASSERT_THAT(obtained, testing::Eq(EOF));
}

TEST_F(DefaultResettableReaderTest, markWithHigherReadAheadLimitThanPreviousMarkOnOddPosition)
{
	// given
	auto input = std::string("abcde");
	auto target = DefaultResettableReader(input);

	// when
	target.mark(1);
	auto obtained = target.read();

	// then
	ASSERT_THAT(obtained, testing::Eq('a'));

	// and when
	target.mark(1);
	obtained = target.read();

	// then
	ASSERT_THAT(obtained, testing::Eq('b'));

	// and when reading from reset reader at odd position
	target.reset();
	target.mark(2);
	obtained = target.read();

	// then
	ASSERT_THAT(obtained, testing::Eq('b'));

	// and when
	obtained = target.read();

	// then
	ASSERT_THAT(obtained, testing::Eq('c'));

	// and when
	target.reset();

	// then
	for (size_t i = 1; i < input.length(); i++)
	{
		obtained = target.read();
		ASSERT_THAT(obtained, testing::Eq(input.at(i)));
	}

	// and when
	obtained = target.read();

	// then
	ASSERT_THAT(obtained, testing::Eq(EOF));
}

TEST_F(DefaultResettableReaderTest, markAndReadAfterNotReadingAllPreviouslyMarkedCharacters)
{
	// given
	auto input = std::string("abcdef");
	auto target = DefaultResettableReader(input);

	// when
	target.mark(2);
	auto obtained =target.read();

	// then
	ASSERT_THAT(obtained, testing::Eq('a'));

	// and when
	target.mark(3);

	// then
	for (size_t i = 1; i < 4; i++)
	{
		obtained = target.read();
		ASSERT_THAT(obtained, testing::Eq(input.at(i)));
	}

	// and then
	target.reset();

	// then
	for (size_t i = 1; i < input.length(); i++)
	{
		obtained = target.read();
		ASSERT_THAT(obtained,testing::Eq(input.at(i)));
	}

	// and when
	obtained = target.read();

	// then
	ASSERT_THAT(obtained, testing::Eq(EOF));
}

TEST_F(DefaultResettableReaderTest, resetAfterReadAhaeadLimitExceededThrowsException)
{
	// given
	auto target = DefaultResettableReader("abcd");

	// when
	target.mark(1);

	target.read();
	target.read();

	// when, then
	ASSERT_THROWS_EXCEPTION(target.reset(), std::logic_error, "Cannot reset beyond 1 characters. Tried to reset 2 characters");
}

TEST_F(DefaultResettableReaderTest, resetWhenReadAheadLimitExceededAftermarkAndResetThrowsException)
{
	// given
	auto input = std::string("abcdefgh");
	auto target = DefaultResettableReader(input);

	// when
	target.mark(5);

	// then
	for (size_t i = 0; i < 5; i++)
	{
		auto obtained = target.read();
		ASSERT_THAT(obtained, testing::Eq(input.at(i)));
	}

	// and when
	target.reset();
	target.mark(3);

	// then
	for (size_t i = 0; i < 4; i++)
	{
		auto obtained = target.read();
		ASSERT_THAT(obtained, testing::Eq(input.at(i)));
	}

	// when, then
	ASSERT_THROWS_EXCEPTION(target.reset(), std::logic_error, "Cannot reset beyond 3 characters. Tried to reset 4 characters")
}

TEST_F(DefaultResettableReaderTest, resetOnUnmarkedReaderThrowsAnException)
{
	// given
	auto target = DefaultResettableReader("abcd");

	// when, then
	ASSERT_THROWS_EXCEPTION(target.reset(), std::logic_error, "No position has been marked");
}

TEST_F(DefaultResettableReaderTest, markWithNegativeLookAheadLimitThrowsException)
{
	// given
	auto target = DefaultResettableReader("abcd");

	// when, then
	ASSERT_THROWS_EXCEPTION(target.mark(-1), std::invalid_argument, "lookAheadLimit < 0");
}