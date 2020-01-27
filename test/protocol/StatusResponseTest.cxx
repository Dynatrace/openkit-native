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

#include "core/UTF8String.h"
#include "protocol/IStatusResponse.h"
#include "protocol/StatusResponse.h"
#include "protocol/ResponseAttributes.h"

#include "../api/mock/MockILogger.h"

#include "gtest/gtest.h"

#include <cstdint>

using namespace test;

using IStatusResponse_t = protocol::IStatusResponse;
using MockNiceILogger_sp = std::shared_ptr<testing::NiceMock<MockILogger>>;
using StatusResponse_t = protocol::StatusResponse;
using ResponseAttributes_t = protocol::ResponseAttributes;
using IResponseAttributes_sp = std::shared_ptr<protocol::IResponseAttributes>;
using Utf8String_t = core::UTF8String;

static constexpr char RESPONSE_KEY_RETRY_AFTER[] = "retry-after";
static constexpr int64_t DEFAULT_RETRY_AFTER_IN_MILLISECONDS = 10L * 60L * 1000L;

class StatusResponseTest : public testing::Test
{
protected:
	MockNiceILogger_sp logger;
	IResponseAttributes_sp attributes;

	void SetUp() override
	{
		logger = MockILogger::createNice();
		attributes = ResponseAttributes_t::withJsonDefaults().build();
	}
};

TEST_F(StatusResponseTest, isErroneousResponseGivesTrueForErrorCodeEqualTo400)
{
	// given
	auto target = StatusResponse_t::createErrorResponse(logger, 400);

	// then
	ASSERT_TRUE(target->isErroneousResponse());
}

TEST_F(StatusResponseTest, isErroneousResponseGivesTrueForErrorCodeGreaterThan400)
{
	// given
	auto target = StatusResponse_t::createErrorResponse(logger, 401);

	// then
	ASSERT_TRUE(target->isErroneousResponse());
}

TEST_F(StatusResponseTest, isErroneousResponseGivesFalseForErrorCodeLessThan400)
{
	// given
	auto target = StatusResponse_t::createSuccessResponse(logger, attributes, 399, IStatusResponse_t::ResponseHeaders());

	// then
	ASSERT_FALSE(target->isErroneousResponse());
}

TEST_F(StatusResponseTest, isTooManyRequestsResponseGivesTrueIfResponseCodeIsEqualTo429)
{
	// given
	auto target = StatusResponse_t::createErrorResponse(logger, 429);

	// then
	ASSERT_TRUE(target->isTooManyRequestsResponse());
}

TEST_F(StatusResponseTest, isTooManyRequestsResponseGivesFalseIfResponseCodeIsNotEqualTo429)
{
	// given
	auto target = StatusResponse_t::createErrorResponse(logger, 404);

	// then
	ASSERT_FALSE(target->isTooManyRequestsResponse());
}

TEST_F(StatusResponseTest, responseCodeIsSet)
{
	// given
	auto target = StatusResponse_t::createErrorResponse(logger, 418);

	// then
	ASSERT_EQ(418, target->getResponseCode());
}

TEST_F(StatusResponseTest, headersAreSetForErrorResponse)
{
	// given
	auto headers = IStatusResponse_t::ResponseHeaders
	{
		{ "X-Foo", std::vector<std::string> { "X-BAR" } },
		{ "X-YZ", std::vector<std::string> { } }
	};
	auto target = StatusResponse_t::createErrorResponse(logger, 418, headers);

	// then
	ASSERT_EQ(headers, target->getResponseHeaders());
}

TEST_F(StatusResponseTest, headersAreSetForSuccessResponse)
{
	// given
	auto headers = IStatusResponse_t::ResponseHeaders
	{
		{ "X-Foo", std::vector<std::string> { "X-BAR" } },
		{ "X-YZ", std::vector<std::string> { } }
	};
	auto responseAttributes = ResponseAttributes_t::withUndefinedDefaults().build();
	auto target = StatusResponse_t::createSuccessResponse(logger, responseAttributes, 200, headers);

	// then
	ASSERT_EQ(headers, target->getResponseHeaders());
}

TEST_F(StatusResponseTest, getRetryAfterReturnsDefaultValueIfResponseKeyDoesNotExist)
{
	// given
	auto target = StatusResponse_t::createErrorResponse(logger, 429);

	// when
	auto obtained = target->getRetryAfterInMilliseconds();

	// then
	ASSERT_EQ(DEFAULT_RETRY_AFTER_IN_MILLISECONDS, obtained);
}

TEST_F(StatusResponseTest, getRetryAfterReturnsDefaultValueIfMultipleValuesWereRetrieved)
{
	// given
	auto responseHeaders = IStatusResponse_t::ResponseHeaders
	{
		{ RESPONSE_KEY_RETRY_AFTER, std::vector<std::string>{ "100", "200" } }
	};
	auto target = StatusResponse_t::createErrorResponse(logger, 429, responseHeaders);

	// when
	auto obtained = target->getRetryAfterInMilliseconds();

	// then
	ASSERT_EQ(DEFAULT_RETRY_AFTER_IN_MILLISECONDS, obtained);
}

TEST_F(StatusResponseTest, getRetryAfterReturnsDefaultValueIfValueIsNotAnIntegerValue)
{
	// given
	auto responseHeaders = IStatusResponse_t::ResponseHeaders
	{
		{ RESPONSE_KEY_RETRY_AFTER, std::vector<std::string>{ "a" } }
	};
	auto target = StatusResponse_t::createErrorResponse(logger, 429, responseHeaders);

	// when
	auto obtained = target->getRetryAfterInMilliseconds();

	// then
	ASSERT_EQ(DEFAULT_RETRY_AFTER_IN_MILLISECONDS, obtained);
}

TEST_F(StatusResponseTest, getRetryAfterReturnsDefaultValueIfValueIsOutOfIntegerRange)
{
	// given
	auto responseHeaders = IStatusResponse_t::ResponseHeaders
	{
		// use string value 2^31, which is one too high for int32_t
		{ RESPONSE_KEY_RETRY_AFTER, std::vector<std::string>{ "2147483648" } }
	};
	auto target = StatusResponse_t::createErrorResponse(logger, 429, responseHeaders);

	// when
	auto obtained = target->getRetryAfterInMilliseconds();

	// then
	ASSERT_EQ(DEFAULT_RETRY_AFTER_IN_MILLISECONDS, obtained);
}

TEST_F(StatusResponseTest, getRetryAfterReturnsParsedValue)
{
	// given
	auto responseHeaders = IStatusResponse_t::ResponseHeaders
	{
		{ RESPONSE_KEY_RETRY_AFTER, std::vector<std::string>{ "1234" } }
	};
	auto target = StatusResponse_t::createErrorResponse(logger, 429, responseHeaders);

	// when
	auto obtained = target->getRetryAfterInMilliseconds();

	// then
	ASSERT_EQ(1234L * 1000L, obtained);
}
