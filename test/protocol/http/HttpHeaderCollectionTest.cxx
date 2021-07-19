/**
 * Copyright 2018-2021 Dynatrace LLC
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

#include "protocol/http/HttpHeaderCollection.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using HttpHeaderCollection_t = protocol::HttpHeaderCollection;

class HttpHeaderCollectionTest : public testing::Test
{
};

TEST_F(HttpHeaderCollectionTest, defaultConstructedHttpHeaderCollectionIsEmpty)
{
	// given
	HttpHeaderCollection_t target;

	// then
	ASSERT_THAT(target.empty(), testing::Eq(true));
	ASSERT_THAT(target.begin(), testing::Eq(target.end()));
}

TEST_F(HttpHeaderCollectionTest, setHeaderInsertsNewHeader)
{
	// given
	const std::string name = "X-Foo";
	const std::string value = "bar";
	HttpHeaderCollection_t target;

	// when
	target.setHeader(name, value);

	// then
	auto iter = target.begin();
	ASSERT_THAT(iter, testing::Ne(target.end()));
	ASSERT_THAT(iter->first, testing::Eq(name));
	ASSERT_THAT(iter->second, testing::ContainerEq(std::list<std::string>{value}));
	iter++;
	ASSERT_THAT(iter, testing::Eq(target.end()));
}

TEST_F(HttpHeaderCollectionTest, setHeaderReplacesPreviousOne)
{
	// given
	const std::string name = "X-Foo";
	const std::string value = "bar";

	HttpHeaderCollection_t target;
	target.setHeader(name, "baz");

	// when
	target.setHeader(name, value);

	// then
	auto iter = target.begin();
	ASSERT_THAT(iter, testing::Ne(target.end()));
	ASSERT_THAT(iter->first, testing::Eq(name));
	ASSERT_THAT(iter->second, testing::ContainerEq(std::list<std::string>{value}));
	iter++;
	ASSERT_THAT(iter, testing::Eq(target.end()));
}

TEST_F(HttpHeaderCollectionTest, setHeaderReplacesPreviousOneWithDifferentCase)
{
	// given
	const std::string name = "X-Foo";
	const std::string value = "bar";

	HttpHeaderCollection_t target;
	target.setHeader("x-foo", "baz");

	// when
	target.setHeader(name, value);

	// then
	auto iter = target.begin();
	ASSERT_THAT(iter, testing::Ne(target.end()));
	ASSERT_THAT(iter->first, testing::Eq(name));
	ASSERT_THAT(iter->second, testing::ContainerEq(std::list<std::string>{value}));
	iter++;
	ASSERT_THAT(iter, testing::Eq(target.end()));
}

TEST_F(HttpHeaderCollectionTest, setHeaderWithMultipleDifferentHeaders)
{
	// given
	const std::string nameOne = "X-Foo";
	const std::string valueOne = "foo";
	const std::string nameTwo = "X-Bar";
	const std::string valueTwo = "bar";

	HttpHeaderCollection_t target;

	// when
	target.setHeader(nameOne, valueOne);
	target.setHeader(nameTwo, valueTwo);

	// then
	std::list<std::string> headerNames;
	std::list<std::list<std::string>> headerValues;
	for (auto iter = target.begin(); iter != target.end(); iter++)
	{
		headerNames.push_back(iter->first);
		headerValues.push_back(iter->second);
	}

	ASSERT_THAT(headerNames, testing::UnorderedElementsAre(nameOne, nameTwo));
	ASSERT_THAT(headerValues, testing::UnorderedElementsAre(
		testing::ContainerEq(std::list<std::string>{valueOne}),
		testing::ContainerEq(std::list<std::string>{valueTwo})
	));
}

TEST_F(HttpHeaderCollectionTest, appendHeaderInsertsHeaderIfNotPresent)
{
	// given
	const std::string name = "X-Foo";
	const std::string value = "bar";
	HttpHeaderCollection_t target;

	// when
	target.appendHeader(name, value);

	// then
	auto iter = target.begin();
	ASSERT_THAT(iter, testing::Ne(target.end()));
	ASSERT_THAT(iter->first, testing::Eq(name));
	ASSERT_THAT(iter->second, testing::ContainerEq(std::list<std::string>{value}));
	iter++;
	ASSERT_THAT(iter, testing::Eq(target.end()));
}

TEST_F(HttpHeaderCollectionTest, appendHeaderAppendsToExistingHeader)
{
	// given
	const std::string name = "X-Foo";
	const std::string valueOne = "bar";
	const std::string valueTwo = "foobar";

	HttpHeaderCollection_t target;
	target.appendHeader(name, valueOne);

	// when
	target.appendHeader(name, valueTwo);

	// then
	auto iter = target.begin();
	ASSERT_THAT(iter, testing::Ne(target.end()));
	ASSERT_THAT(iter->first, testing::Eq(name));
	ASSERT_THAT(iter->second, testing::ContainerEq(std::list<std::string>{valueOne, valueTwo}));
	iter++;
	ASSERT_THAT(iter, testing::Eq(target.end()));
}

TEST_F(HttpHeaderCollectionTest, appendHeaderWorksCaseInsensitive)
{
	// given
	const std::string nameOne = "X-Foo";
	const std::string valueOne = "bar";
	const std::string nameTwo = "x-fOO";
	const std::string valueTwo = "foobar";

	HttpHeaderCollection_t target;
	target.appendHeader(nameOne, valueOne);

	// when
	target.appendHeader(nameTwo, valueTwo);

	// then
	auto iter = target.begin();
	ASSERT_THAT(iter, testing::Ne(target.end()));
	ASSERT_THAT(iter->first, testing::Eq(nameOne));
	ASSERT_THAT(iter->second, testing::ContainerEq(std::list<std::string>{valueOne, valueTwo}));
	iter++;
	ASSERT_THAT(iter, testing::Eq(target.end()));
}

TEST_F(HttpHeaderCollectionTest, appendHeaderWorksWithMultipleHeadersAndValues)
{
	// given
	const std::string nameOne = "X-Foo";
	const std::string valueOne = "bar";
	const std::string nameTwo = "x-fOO";
	const std::string valueTwo = "foobar";
	const std::string nameThree = "X-Bar";
	const std::string valueThree = "bar";

	HttpHeaderCollection_t target;

	// when
	target.appendHeader(nameOne, valueOne);
	target.appendHeader(nameThree, valueThree);
	target.appendHeader(nameTwo, valueTwo);

	// then
	std::list<std::string> headerNames;
	std::list<std::list<std::string>> headerValues;
	for (auto iter = target.begin(); iter != target.end(); iter++)
	{
		headerNames.push_back(iter->first);
		headerValues.push_back(iter->second);
	}

	ASSERT_THAT(headerNames, testing::UnorderedElementsAre(nameOne, nameThree));
	ASSERT_THAT(headerValues, testing::UnorderedElementsAre(
		testing::ContainerEq(std::list<std::string>{valueOne, valueTwo}),
		testing::ContainerEq(std::list<std::string>{valueThree})
	));
}

TEST_F(HttpHeaderCollectionTest, containsGivesTrueIfHeaderIsContained)
{
	// given
	const std::string name = "X-Foo";
	const std::string value = "bar";
	HttpHeaderCollection_t target;
	target.setHeader(name, value);

	// when
	auto obtained = target.contains(name);

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
}

TEST_F(HttpHeaderCollectionTest, containsGivesTrueIfHeaderIsContainedIgnoringCase)
{
	// given
	const std::string name = "X-Foo";
	const std::string value = "bar";
	HttpHeaderCollection_t target;
	target.setHeader(name, value);

	// when
	auto obtained = target.contains("x-foo");

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
}

TEST_F(HttpHeaderCollectionTest, containsGivesFalseIfHeaderIsNotContained)
{
	// given
	const std::string name = "X-Foo";
	const std::string value = "bar";
	HttpHeaderCollection_t target;
	target.setHeader(name, value);

	// when
	auto obtained = target.contains("X-Bar");

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(HttpHeaderCollectionTest, getHeaderGivesPreviouslySetHeaderValues)
{
	// given
	const std::string name = "X-Foo";
	const std::string value = "bar";
	HttpHeaderCollection_t target;
	target.setHeader(name, value);

	// when
	auto obtained = target.getHeader(name);

	// then
	ASSERT_THAT(obtained, testing::ContainerEq(std::list<std::string>{value}));
}

TEST_F(HttpHeaderCollectionTest, getHeaderWorksCaseInsensitive)
{
	// given
	const std::string name = "X-Foo";
	const std::string value = "bar";
	HttpHeaderCollection_t target;
	target.setHeader(name, value);

	// when
	auto obtained = target.getHeader("x-fOO");

	// then
	ASSERT_THAT(obtained, testing::ContainerEq(std::list<std::string>{value}));
}

TEST_F(HttpHeaderCollectionTest, getHeaderWorksWithMultipleHeaderValues)
{
	// given
	const std::string name = "X-Foo";
	const std::string valueOne = "foo";
	const std::string valueTwo = "bar";
	HttpHeaderCollection_t target;
	target.setHeader(name, valueOne);
	target.appendHeader(name, valueTwo);

	// when
	auto obtained = target.getHeader(name);

	// then
	ASSERT_THAT(obtained, testing::ContainerEq(std::list<std::string>{valueOne, valueTwo}));
}

TEST_F(HttpHeaderCollectionTest, getHeaderThrowsExceptionIfHeaderIsNotContained)
{
	// given
	const std::string name = "X-Foo";
	const std::string value = "bar";
	HttpHeaderCollection_t target;
	target.setHeader(name, value);

	// when, then
	ASSERT_THROW(target.getHeader("X-Bar"), std::invalid_argument);
}

TEST_F(HttpHeaderCollectionTest, initializationWithInitializerListWorks)
{
	// given
	const std::string nameOne = "X-Foo";
	const std::string valueOne = "bar";
	const std::string nameTwo = "X-Bar";
	const std::string valueTwo = "foobar";
	const std::string valueThree = "bar";

	// when
	HttpHeaderCollection_t target
	{
		{nameOne, {valueOne}},
		{nameTwo, {valueTwo, valueThree}}
	};

	// then
	std::list<std::string> headerNames;
	std::list<std::list<std::string>> headerValues;
	for (auto iter = target.begin(); iter != target.end(); iter++)
	{
		headerNames.push_back(iter->first);
		headerValues.push_back(iter->second);
	}

	ASSERT_THAT(headerNames, testing::UnorderedElementsAre(nameOne, nameTwo));
	ASSERT_THAT(headerValues, testing::UnorderedElementsAre(
		testing::ContainerEq(std::list<std::string>{valueOne}),
		testing::ContainerEq(std::list<std::string>{valueTwo, valueThree})
	));
}

TEST_F(HttpHeaderCollectionTest, clearRemovesPreviouslySetHeaders)
{
	// given
	const std::string nameOne = "X-Foo";
	const std::string valueOne = "bar";
	const std::string nameTwo = "X-Bar";
	const std::string valueTwo = "foobar";
	const std::string valueThree = "bar";

	HttpHeaderCollection_t target
	{
		{nameOne, {valueOne}},
		{nameTwo, {valueTwo, valueThree}}
	};

	ASSERT_THAT(target.empty(), testing::Eq(false));

	// when
	target.clear();

	// then
	ASSERT_THAT(target.empty(), testing::Eq(true));
}

TEST_F(HttpHeaderCollectionTest, defaultConstructedGivesEmptyHeaderNames)
{
	// given
	HttpHeaderCollection_t target;

	// when
	auto obtained = target.getHeaderNames();

	// then
	ASSERT_THAT(obtained, testing::IsEmpty());
}

TEST_F(HttpHeaderCollectionTest, getHeaderNamesWithSingleHeaderName)
{
	// given
	const std::string name = "X-Foo";
	const std::string valueOne = "bar";
	const std::string valueTwo = "foobar";

	HttpHeaderCollection_t target
	{
		{name, {valueOne}}
	};

	// when
	auto obtained = target.getHeaderNames();

	// then
	ASSERT_THAT(obtained, testing::ContainerEq(std::list<std::string>{name}));

	// and when appending another value
	target.appendHeader(name, valueTwo);
	obtained = target.getHeaderNames();

	// then
	ASSERT_THAT(obtained, testing::ContainerEq(std::list<std::string>{name}));
}


TEST_F(HttpHeaderCollectionTest, getHeaderNamesWithMultipleHeaderName)
{
	// given
	const std::string nameOne = "X-Foo";
	const std::string valueOne = "bar";
	const std::string nameTwo = "X-Bar";
	const std::string valueTwo = "foobar";
	const std::string valueThree = "bar";

	HttpHeaderCollection_t target
	{
		{nameOne, {valueOne}},
		{nameTwo, {valueTwo, valueThree}}
	};

	// when
	auto obtained = target.getHeaderNames();

	// then
	ASSERT_THAT(obtained, testing::UnorderedElementsAre(nameOne, nameTwo));
}