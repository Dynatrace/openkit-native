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

#include "protocol/http/HttpResponse.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using HttpHeaderCollection_t = protocol::HttpHeaderCollection;
using HttpResponse_t = protocol::HttpResponse;

class HttpResponseTest : public testing::Test
{
};

TEST_F(HttpResponseTest, getRequestUriGivesValuePassedInConstructor)
{
	// given
	const std::string requestUri = "http://some.domain.com/foo/bar";
	HttpResponse_t target(requestUri, "", -1, "", HttpHeaderCollection_t(), "");

	// when
	auto obtained = target.getRequestUri();

	// then
	ASSERT_THAT(obtained, testing::Eq(requestUri));
}

TEST_F(HttpResponseTest, getRequestMethodGivesValuePassedInConstructor)
{
	// given
	const std::string requestMethod = "POST";
	HttpResponse_t target("", requestMethod, -1, "", HttpHeaderCollection_t(), "");

	// when
	auto obtained = target.getRequestMethod();

	// then
	ASSERT_THAT(obtained, testing::Eq(requestMethod));
}

TEST_F(HttpResponseTest, getStatusCodeGivesValuePassedInConstructor)
{
	// given
	const int32_t statusCode = 418;
	HttpResponse_t target("", "", statusCode, "", HttpHeaderCollection_t(), "");

	// when
	auto obtained = target.getStatusCode();

	// then
	ASSERT_THAT(obtained, testing::Eq(statusCode));
}

TEST_F(HttpResponseTest, getReasonPhraseGivesValuePassedInConstructor)
{
	// given
	const std::string reasonPhrase = "I'm a teapot";
	HttpResponse_t target("", "", -1, reasonPhrase, HttpHeaderCollection_t(), "");

	// when
	auto obtained = target.getReasonPhrase();

	// then
	ASSERT_THAT(obtained, testing::Eq(reasonPhrase));
}

TEST_F(HttpResponseTest, containsHeaderReturnsTruefIfHeaderIsContainedInCollection)
{
	// given
	const std::string headerName = "X-Foo";

	HttpHeaderCollection_t responseHeaders;
	responseHeaders.setHeader(headerName, "some value");

	HttpResponse_t target("", "", -1, "", responseHeaders, "");

	// when
	auto obtained = target.containsHeader(headerName);

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
}

TEST_F(HttpResponseTest, containsHeaderReturnsFalseIfHeaderIsNotContainedInCollection)
{
	// given
	HttpResponse_t target("", "", -1, "", HttpHeaderCollection_t(), "");

	// when
	auto obtained = target.containsHeader("X-Foo");

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(HttpResponseTest, getHeaderReturnsHeaderValue)
{
	// given
	HttpHeaderCollection_t responseHeaders;
	
	const std::string headerName = "X-Foo";
	const std::string headerValue = "foo";
	responseHeaders.setHeader(headerName, headerValue);

	HttpResponse_t target("", "", -1, "", responseHeaders, "");

	// when
	auto obtained = target.getHeader(headerName);

	// then
	ASSERT_THAT(obtained, testing::ContainerEq(std::list<std::string>{headerValue}));
}

TEST_F(HttpResponseTest, getHeaderReturnsMultipleHeaderValues)
{
	// given
	HttpHeaderCollection_t responseHeaders;

	const std::string headerName = "X-Foo";
	const std::string headerValueOne = "foo";
	const std::string headerValueTwo = "bar";
	responseHeaders.setHeader(headerName, headerValueOne);
	responseHeaders.appendHeader(headerName, headerValueTwo);

	HttpResponse_t target("", "", -1, "", responseHeaders, "");

	// when
	auto obtained = target.getHeader(headerName);

	// then
	ASSERT_THAT(obtained, testing::ContainerEq(std::list<std::string>{headerValueOne, headerValueTwo}));
}

TEST_F(HttpResponseTest, getHeaderReturnsEmptyListIfHeaderIsNotContained)
{
	// given
	HttpResponse_t target("", "", -1, "", HttpHeaderCollection_t(), "");

	// when
	auto obtained = target.getHeader("X-Foo");

	// then
	ASSERT_THAT(obtained, testing::IsEmpty());
}

TEST_F(HttpResponseTest, getResponseBodyGivesValuePassedInConstructor)
{
	// given
	std::string responseBody = "Lorem ipsum dolor sit amet, consectetur adipiscing elit.";
	HttpResponse_t target("", "", -1, "", HttpHeaderCollection_t(), responseBody);

	// when
	auto obtained = target.getResponseBody();

	// then
	ASSERT_THAT(obtained, testing::Eq(responseBody));
}
