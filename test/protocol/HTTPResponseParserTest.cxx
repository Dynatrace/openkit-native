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

#include "protocol/HTTPResponseParser.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using HttpResponseParser_t = protocol::HTTPResponseParser;

class HTTPResponseParserTest : public testing::Test
{
};

TEST_F(HTTPResponseParserTest, defaultConstructedGivesEmptyResponseBody)
{
	// given
	auto target = HttpResponseParser_t();

	// then
	ASSERT_THAT(target.getResponseBody(), testing::IsEmpty());
}

TEST_F(HTTPResponseParserTest, responseBodyDataSetsResponseData)
{
	// given
	auto target = HttpResponseParser_t();
	auto body = std::string("foo");

	// when
	auto obtained = target.responseBodyData(body.data(), sizeof(std::string::value_type), body.length());

	// then
	ASSERT_THAT(obtained, testing::Eq(3));
	ASSERT_THAT(target.getResponseBody(), testing::Eq(body));
}

TEST_F(HTTPResponseParserTest, responseBodyDataConcatenatesDataIfCalledMultipleTimes)
{
	// given
	auto target = HttpResponseParser_t();
	auto bodyOne = std::string("foo");
	auto bodyTwo = std::string("bar");

	// when
	auto obtainedOne = target.responseBodyData(bodyOne.data(), sizeof(std::string::value_type), bodyOne.length());
	auto obtainedTwo = target.responseBodyData(bodyTwo.data(), sizeof(std::string::value_type), bodyTwo.length());

	// then
	ASSERT_THAT(obtainedOne, testing::Eq(3));
	ASSERT_THAT(obtainedTwo, testing::Eq(3));
	ASSERT_THAT(target.getResponseBody(), testing::Eq(bodyOne + bodyTwo));
}

TEST_F(HTTPResponseParserTest, defaultConstructedGivesResponseStatusEqualMinusOne)
{
	// given
	auto target = HttpResponseParser_t();

	// then
	ASSERT_THAT(target.getResponseStatus(), testing::Eq(-1));
}

TEST_F(HTTPResponseParserTest, httpStatusLineWithReasonPhraseCanBeParsed)
{
	// given
	const std::string httpStatusLine = "HTTP/1.1 418 I'm a teapot";

	auto target = HttpResponseParser_t();

	// when
	auto obtained = target.responseHeaderData(httpStatusLine.data(), sizeof(std::string::value_type), httpStatusLine.length());

	// then
	ASSERT_THAT(obtained, testing::Eq(httpStatusLine.length()));
	ASSERT_THAT(target.getResponseStatus(), testing::Eq(418));
	ASSERT_THAT(target.getReasonPhrase(), testing::Eq("I'm a teapot"));
}

TEST_F(HTTPResponseParserTest, httpStatusLineWithReasonPhraseAndCrLfCanBeParsed)
{
	// given
	const std::string httpStatusLine = "HTTP/1.1 418 I'm a teapot\r\n";

	auto target = HttpResponseParser_t();

	// when
	auto obtained = target.responseHeaderData(httpStatusLine.data(), sizeof(std::string::value_type), httpStatusLine.length());

	// then
	ASSERT_THAT(obtained, testing::Eq(httpStatusLine.length()));
	ASSERT_THAT(target.getResponseStatus(), testing::Eq(418));
	ASSERT_THAT(target.getReasonPhrase(), testing::Eq("I'm a teapot"));
}

TEST_F(HTTPResponseParserTest, httpStatusLineWithoutReasonPhraseCanBeParsed)
{
	// given
	const std::string httpStatusLine = "HTTP/1.1 200";

	auto target = HttpResponseParser_t();

	// when
	auto obtained = target.responseHeaderData(httpStatusLine.data(), sizeof(std::string::value_type), httpStatusLine.length());

	// then
	ASSERT_THAT(obtained, testing::Eq(httpStatusLine.length()));
	ASSERT_THAT(target.getResponseStatus(), testing::Eq(200));
	ASSERT_THAT(target.getReasonPhrase(), testing::IsEmpty());
}

TEST_F(HTTPResponseParserTest, httpStatusLineWithCrLfButWithoutReasonPhraseCanBeParsed)
{
	// given
	const std::string httpStatusLine = "HTTP/1.1 302\r\n";

	auto target = HttpResponseParser_t();

	// when
	auto obtained = target.responseHeaderData(httpStatusLine.data(), sizeof(std::string::value_type), httpStatusLine.length());

	// then
	ASSERT_THAT(obtained, testing::Eq(httpStatusLine.length()));
	ASSERT_THAT(target.getResponseStatus(), testing::Eq(302));
	ASSERT_THAT(target.getReasonPhrase(), testing::IsEmpty());
}

TEST_F(HTTPResponseParserTest, httpStatusLineWithLessThanThreeDigitsStatusCodeCannotBeParsed)
{
	// given
	const std::string httpStatusLine = "HTTP/1.1 30\r\n";

	auto target = HttpResponseParser_t();

	// when
	auto obtained = target.responseHeaderData(httpStatusLine.data(), sizeof(std::string::value_type), httpStatusLine.length());

	// then
	ASSERT_THAT(obtained, testing::Eq(httpStatusLine.length()));
	ASSERT_THAT(target.getResponseStatus(), testing::Eq(-1));
	ASSERT_THAT(target.getReasonPhrase(), testing::IsEmpty());
}

TEST_F(HTTPResponseParserTest, httpStatusLineWithMoreThanThreeDigitsStatusCodeCannotBeParsed)
{
	// given
	const std::string httpStatusLine = "HTTP/1.1 3045\r\n";

	auto target = HttpResponseParser_t();

	// when
	auto obtained = target.responseHeaderData(httpStatusLine.data(), sizeof(std::string::value_type), httpStatusLine.length());

	// then
	ASSERT_THAT(obtained, testing::Eq(httpStatusLine.length()));
	ASSERT_THAT(target.getResponseStatus(), testing::Eq(-1));
	ASSERT_THAT(target.getReasonPhrase(), testing::IsEmpty());
}

TEST_F(HTTPResponseParserTest, headerLineWithCrLffDoesNotCrashParser)
{
	// given
	auto target = HttpResponseParser_t();

	// when adding the HTTP status line
	auto obtained = target.responseHeaderData("\r\n", sizeof(std::string::value_type), std::size_t(2));

	// then
	ASSERT_THAT(obtained, testing::Eq(2));
	ASSERT_THAT(target.getResponseHeaders().empty(), testing::Eq(true));
}

TEST_F(HTTPResponseParserTest, parsingColonSeparatedLineWorks)
{
	// given
	auto target = HttpResponseParser_t();
	auto headerString = std::string("content-length:42");

	// when adding the line
	auto obtained = target.responseHeaderData(headerString.data(), sizeof(std::string::value_type), headerString.length());

	// then
	ASSERT_THAT(obtained, testing::Eq(headerString.length()));

	auto obtainedHeaders = target.getResponseHeaders();
	ASSERT_THAT(obtainedHeaders.empty(), testing::Eq(false));
	ASSERT_THAT(obtainedHeaders.contains("content-length"), testing::Eq(true));
	ASSERT_THAT(obtainedHeaders.getHeader("content-length"), testing::ContainerEq(std::list<std::string>{"42"}));
}

TEST_F(HTTPResponseParserTest, optionalWhitespacheFromValuesAreStripped)
{
	// given
	auto target = HttpResponseParser_t();
	auto headerString = std::string("Content-Length: \t 42\t\t ");

	// when adding the line
	auto obtained = target.responseHeaderData(headerString.data(), sizeof(std::string::value_type), headerString.length());

	// then
	ASSERT_THAT(obtained, testing::Eq(headerString.length()));

	auto obtainedHeaders = target.getResponseHeaders();
	ASSERT_THAT(obtainedHeaders.contains("Content-Length"), testing::Eq(true));
	ASSERT_THAT(obtainedHeaders.getHeader("Content-Length"), testing::ContainerEq(std::list<std::string>{"42"}));
}

TEST_F(HTTPResponseParserTest, trailingCRLFIsStripped)
{
	// given
	auto target = HttpResponseParser_t();
	auto headerString = std::string("Content-Length: 42\r\n");

	// when adding the line
	auto obtained = target.responseHeaderData(headerString.data(), sizeof(std::string::value_type), headerString.length());

	// then
	ASSERT_THAT(obtained, testing::Eq(headerString.length()));

	auto obtainedHeaders = target.getResponseHeaders();
	ASSERT_THAT(obtainedHeaders.contains("Content-Length"), testing::Eq(true));
	ASSERT_THAT(obtainedHeaders.getHeader("Content-Length"), testing::ContainerEq(std::list<std::string>{"42"}));
}

TEST_F(HTTPResponseParserTest, multipleHeaderValuesWithSameNameAreMergedCorrectly)
{
	// given
	auto target = HttpResponseParser_t();
	auto headerStringOne = std::string("Set-Cookie:Test=test_value\r\n");
	auto headerStringTwo = std::string("set-cookie: foo=bar\r\n");

	// when adding the lines
	auto obtainedOne = target.responseHeaderData(headerStringOne.data(), sizeof(std::string::value_type), headerStringOne.length());
	auto obtainedTwo = target.responseHeaderData(headerStringTwo.data(), sizeof(std::string::value_type), headerStringTwo.length());

	// then
	ASSERT_THAT(obtainedOne, testing::Eq(headerStringOne.length()));
	ASSERT_THAT(obtainedTwo, testing::Eq(headerStringTwo.length()));

	auto obtainedHeaders = target.getResponseHeaders();
	ASSERT_THAT(obtainedHeaders.contains("set-cookie"), testing::Eq(true));
	ASSERT_THAT(obtainedHeaders.getHeader("set-cookie"), testing::ContainerEq(std::list<std::string>{"Test=test_value", "foo=bar"}));
}

TEST_F(HTTPResponseParserTest, parsingStatusLineClearsPreviouslyParsedValues)
{
	// given
	auto target = HttpResponseParser_t();

	const std::vector<std::string> firstResponseHeaderLines = 
	{
		std::string("HTTP/1.1 301 Moved Permanently\r\n"),
		std::string("X-Foo: bar\r\n"),
		std::string("Set-Cookie: __Secure-ID=123; Secure; Domain=example.com\r\n"),
		std::string("\r\n")
	};
	
	for (const auto& headerLine : firstResponseHeaderLines)
	{
		target.responseHeaderData(headerLine.data(), sizeof(std::string::value_type), headerLine.length());
	}

	const std::string firstResponseBodyData = "foobar";
	target.responseBodyData(firstResponseBodyData.data(), sizeof(std::string::value_type), firstResponseBodyData.length());

	// verify
	ASSERT_THAT(target.getResponseStatus(), testing::Eq(301));
	ASSERT_THAT(target.getReasonPhrase(), testing::Eq("Moved Permanently"));
	ASSERT_THAT(target.getResponseHeaders().empty(), testing::Eq(false));
	ASSERT_THAT(target.getResponseHeaders().contains("X-Foo"), testing::Eq(true));
	ASSERT_THAT(target.getResponseHeaders().getHeader("X-Foo"), testing::ContainerEq(std::list<std::string>{ "bar" }));
	ASSERT_THAT(target.getResponseHeaders().contains("Set-Cookie"), testing::Eq(true));
	ASSERT_THAT(target.getResponseHeaders().getHeader("Set-Cookie"), 
		testing::ContainerEq(std::list<std::string>{ "__Secure-ID=123; Secure; Domain=example.com" }));
	ASSERT_THAT(target.getResponseBody(), testing::Eq("foobar"));

	// when
	const std::string secondStatusLine = "HTTP/1.1 200 OK\r\n";
	target.responseHeaderData(secondStatusLine.data(), sizeof(std::string::value_type), secondStatusLine.length());

	// then
	ASSERT_THAT(target.getResponseStatus(), testing::Eq(200));
	ASSERT_THAT(target.getReasonPhrase(), testing::Eq("OK"));
	ASSERT_THAT(target.getResponseHeaders().empty(), testing::Eq(true));
	ASSERT_THAT(target.getResponseBody(), testing::IsEmpty());
}
