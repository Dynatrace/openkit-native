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

#include "protocol/http/HttpRequest.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using HttpRequest_t = protocol::HttpRequest;

class HttpRequestTest : public testing::Test
{
};

TEST_F(HttpRequestTest, defaultConstructedHttpRequestHasNoHeaders)
{
	// given
	auto target = HttpRequest_t("", "");

	// then
	ASSERT_THAT(target.getHttpHeaders().begin(), testing::Eq(target.getHttpHeaders().end()));
}

TEST_F(HttpRequestTest, getUriGivesRequestUriPassedInConstructor)
{
	// given
	const std::string requestUri = "https://foo.bar.com/mbeacon/1";

	auto target = HttpRequest_t(requestUri, "GET");

	// when
	auto obtained = target.getUri();

	// then
	ASSERT_THAT(obtained, testing::Eq(requestUri));
}

TEST_F(HttpRequestTest, getMethodGivesRequestMethodPassedInConstructor)
{
	// given
	const std::string requestMethod = "GET";

	auto target = HttpRequest_t("http://localhost", requestMethod);

	// when
	auto obtained = target.getMethod();

	// then
	ASSERT_THAT(obtained, testing::Eq(requestMethod));
}

TEST_F(HttpRequestTest, setHeaderStoresHeaderAndValueInHttpHeaderCollection)
{
	// given
	std::string headerName = "X-Foo";
	std::string headerValue = "bar";

	auto target = HttpRequest_t("", "");

	// when
	target.setHeader(headerName, headerValue);

	// then
	ASSERT_THAT(target.getHttpHeaders().contains(headerName), testing::Eq(true));
	ASSERT_THAT(target.getHttpHeaders().getHeader(headerName), testing::ContainerEq(std::list<std::string>{headerValue}));
}

TEST_F(HttpRequestTest, setHeaderOverwritesHeaderAndValueInHttpHeaderCollection)
{
	// given
	std::string headerName = "X-Foo";
	std::string headerValue = "bar";

	auto target = HttpRequest_t("", "");
	target.setHeader(headerName, "some value");

	// when
	target.setHeader(headerName, headerValue);

	// then
	ASSERT_THAT(target.getHttpHeaders().contains(headerName), testing::Eq(true));
	ASSERT_THAT(target.getHttpHeaders().getHeader(headerName), testing::ContainerEq(std::list<std::string>{headerValue}));
}

TEST_F(HttpRequestTest, setHeaderDoesNotSetRestrictedHeaders)
{
	// given
	std::list<std::string> restrictedHttpRequestHeaders = {
		"access-control-request-headers",
		"access-control-request-method",
		"connection",
		"content-length",
		"content-transfer-encoding",
		"host",
		"keep-alive",
		"origin",
		"trailer",
		"transfer-encoding",
		"upgrade",
		"via"
	};

	auto target = HttpRequest_t("", "");

	for (auto headerName : restrictedHttpRequestHeaders)
	{
		// when
		target.setHeader(headerName, "some value");

		// then
		ASSERT_THAT(target.getHttpHeaders().begin(), testing::Eq(target.getHttpHeaders().end()));
	}
}

TEST_F(HttpRequestTest, containsHeaderGivesTrueIfHeaderIsContained)
{
	// given
	std::string headerName = "X-Foo";

	auto target = HttpRequest_t("", "");
	target.setHeader(headerName, "some value");

	// when
	auto obtained = target.containsHeader(headerName);

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
}

TEST_F(HttpRequestTest, containsHeaderGivesFalseIfHeaderIsNotContained)
{
	// given
	auto target = HttpRequest_t("", "");

	// when
	auto obtained = target.containsHeader("X-Foo");

	// then
	ASSERT_THAT(obtained, testing::Eq(false));
}

TEST_F(HttpRequestTest, getHeaderGivesThePreviouslySetHeader)
{
	// given
	std::string headerName = "X-Foo";
	std::string headerValue = "bar";

	auto target = HttpRequest_t("", "");
	target.setHeader(headerName, headerValue);

	// when
	auto obtained = target.getHeader(headerName);

	// then
	ASSERT_THAT(obtained, testing::Eq(headerValue));
}

TEST_F(HttpRequestTest, getHeaderGivesEmptyStringIfHeaderIsNotContained)
{
	// given
	auto target = HttpRequest_t("", "");

	// when
	auto obtained = target.getHeader("X-Foo");

	// then
	ASSERT_THAT(obtained, testing::IsEmpty());
}

TEST_F(HttpRequestTest, getHeaderNamesGivesEmptyListIfNoHeaderWasSet)
{
	// given
	HttpRequest_t target("", "");

	// when
	auto obtained = target.getHeaderNames();

	// then
	ASSERT_THAT(obtained, testing::IsEmpty());
}

TEST_F(HttpRequestTest, getHeaderNamesGivesPreviouslySetHeaderNames)
{
	// given
	std::string headerNameOne = "X-Foo";
	std::string headerValueOne = "bar";
	std::string headerNameTwo = "X-Bar";
	std::string headerValueTwo = "foobar";

	HttpRequest_t target("", "");

	// when
	target.setHeader(headerNameOne, headerValueOne);
	auto obtained = target.getHeaderNames();

	// then
	ASSERT_THAT(obtained, testing::ContainerEq(std::list<std::string>{headerNameOne}));

	// and when
	target.setHeader(headerNameTwo, headerValueTwo);
	obtained = target.getHeaderNames();

	// then
	ASSERT_THAT(obtained, testing::UnorderedElementsAre(headerNameOne, headerNameTwo));
}
