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

#include "protocol/HTTPResponseParser.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace protocol;


class HTTPResponseParserTest : public testing::Test
{
};


TEST_F(HTTPResponseParserTest, defaultConstructedGivesEmptyResponseBody)
{
	// given
	auto target = HTTPResponseParser();

	// then
	ASSERT_TRUE(target.getResponseBody().empty());
}

TEST_F(HTTPResponseParserTest, responseBodyDataSetsResponseData)
{
	// given
	auto target = HTTPResponseParser();
	auto body = std::string("foo");

	// when
	auto obtained = target.responseBodyData(body.data(), sizeof(std::string::value_type), body.length());

	// then
	ASSERT_EQ(3, obtained);
	ASSERT_EQ(std::string("foo"), target.getResponseBody());
}

TEST_F(HTTPResponseParserTest, responseBodyDataConcatenatesDataIfCalledMultipleTimes)
{
	// given
	auto target = HTTPResponseParser();
	auto bodyOne = std::string("foo");
	auto bodyTwo = std::string("bar");

	// when
	auto obtainedOne = target.responseBodyData(bodyOne.data(), sizeof(std::string::value_type), bodyOne.length());
	auto obtainedTwo = target.responseBodyData(bodyTwo.data(), sizeof(std::string::value_type), bodyTwo.length());

	// then
	ASSERT_EQ(3, obtainedOne);
	ASSERT_EQ(3, obtainedTwo);
	ASSERT_EQ(std::string("foobar"), target.getResponseBody());
}

TEST_F(HTTPResponseParserTest, defaultConstructedGivesEmptyResponseHeaders)
{
	// given
	auto target = HTTPResponseParser();

	// then
	ASSERT_TRUE(target.getResponseHeaders().empty());
}

TEST_F(HTTPResponseParserTest, emptyDataDoesNotCrashParser)
{
	// given
	auto target = HTTPResponseParser();

	// when adding the HTTP status line
	auto obtained = target.responseHeaderData("", sizeof(std::string::value_type), std::size_t(0));

	// then
	ASSERT_EQ(0, obtained);
	ASSERT_TRUE(target.getResponseHeaders().empty());
}

TEST_F(HTTPResponseParserTest, headerLineWithCrLfDoesNotCrashParser)
{
	// given
	auto target = HTTPResponseParser();

	// when adding the HTTP status line
	auto obtained = target.responseHeaderData("\r\n", sizeof(std::string::value_type), std::size_t(2));

	// then
	ASSERT_EQ(2, obtained);
	ASSERT_TRUE(target.getResponseHeaders().empty());
}

TEST_F(HTTPResponseParserTest, aParseableResponseHeaderMustContainAColon)
{
	// given
	auto target = HTTPResponseParser();
	auto statusLine = std::string("HTTP/1.1 200 OK\r\n");

	// when adding the HTTP status line
	auto obtained = target.responseHeaderData(statusLine.data(), sizeof(std::string::value_type), statusLine.length());

	// then
	ASSERT_EQ(statusLine.length(), obtained);
	ASSERT_TRUE(target.getResponseHeaders().empty());
}

TEST_F(HTTPResponseParserTest, parsingColonSeparatedLineWorks)
{
	// given
	auto target = HTTPResponseParser();
	auto headerString = std::string("content-length:42");

	// when adding the line
	auto obtained = target.responseHeaderData(headerString.data(), sizeof(std::string::value_type), headerString.length());

	// then
	ASSERT_EQ(headerString.length(), obtained);

	auto obtainedHeaders = target.getResponseHeaders();
	ASSERT_FALSE(obtainedHeaders.empty());
	ASSERT_NE(obtainedHeaders.end(), obtainedHeaders.find("content-length"));
	ASSERT_EQ(std::vector<std::string>{"42"}, obtainedHeaders.find("content-length")->second);
}

TEST_F(HTTPResponseParserTest, keyIsTransformedToLowerCase)
{
	// given
	auto target = HTTPResponseParser();
	auto headerString = std::string("Content-Length:42");

	// when adding the line
	auto obtained = target.responseHeaderData(headerString.data(), sizeof(std::string::value_type), headerString.length());

	// then
	ASSERT_EQ(headerString.length(), obtained);

	auto obtainedHeaders = target.getResponseHeaders();
	ASSERT_NE(obtainedHeaders.end(), obtainedHeaders.find("content-length"));
	ASSERT_EQ(std::vector<std::string>{"42"}, obtainedHeaders.find("content-length")->second);
}

TEST_F(HTTPResponseParserTest, optionalWhitespacheFromValuesAreStripped)
{
	// given
	auto target = HTTPResponseParser();
	auto headerString = std::string("Content-Length: \t 42\t\t ");

	// when adding the line
	auto obtained = target.responseHeaderData(headerString.data(), sizeof(std::string::value_type), headerString.length());

	// then
	ASSERT_EQ(headerString.length(), obtained);

	auto obtainedHeaders = target.getResponseHeaders();
	ASSERT_NE(obtainedHeaders.end(), obtainedHeaders.find("content-length"));
	ASSERT_EQ(std::vector<std::string>{"42"}, obtainedHeaders.find("content-length")->second);
}

TEST_F(HTTPResponseParserTest, trailingCRLFIsStripped)
{
	// given
	auto target = HTTPResponseParser();
	auto headerString = std::string("Content-Length: 42\r\n");

	// when adding the line
	auto obtained = target.responseHeaderData(headerString.data(), sizeof(std::string::value_type), headerString.length());

	// then
	ASSERT_EQ(headerString.length(), obtained);

	auto obtainedHeaders = target.getResponseHeaders();
	ASSERT_NE(obtainedHeaders.end(), obtainedHeaders.find("content-length"));
	ASSERT_EQ(std::vector<std::string>{"42"}, obtainedHeaders.find("content-length")->second);
}

TEST_F(HTTPResponseParserTest, multipleLinesWithSimilarKeyMergesValues)
{
	// given
	auto target = HTTPResponseParser();
	auto headerStringOne = std::string("Set-Cookie:Test=test_value\r\n");
	auto headerStringTwo = std::string("set-cookie: foo=bar\r\n");

	// when adding the lines
	auto obtainedOne = target.responseHeaderData(headerStringOne.data(), sizeof(std::string::value_type), headerStringOne.length());
	auto obtainedTwo = target.responseHeaderData(headerStringTwo.data(), sizeof(std::string::value_type), headerStringTwo.length());

	// then
	ASSERT_EQ(headerStringOne.length(), obtainedOne);
	ASSERT_EQ(headerStringTwo.length(), obtainedTwo);

	auto obtainedHeaders = target.getResponseHeaders();
	ASSERT_NE(obtainedHeaders.end(), obtainedHeaders.find("set-cookie"));
	ASSERT_THAT(obtainedHeaders.find("set-cookie")->second, testing::ElementsAre(std::string("Test=test_value"), std::string("foo=bar")));
}
