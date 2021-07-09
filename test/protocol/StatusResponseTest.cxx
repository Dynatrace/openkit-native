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

#include "core/UTF8String.h"
#include "protocol/IStatusResponse.h"
#include "protocol/StatusResponse.h"
#include "protocol/ResponseAttributes.h"
#include "protocol/http/HttpHeaderCollection.h"

#include "../api/mock/MockILogger.h"

#include "gtest/gtest.h"

#include <cstdint>
#include <cctype>

using namespace test;

using IStatusResponse_t = protocol::IStatusResponse;
using MockNiceILogger_sp = std::shared_ptr<testing::NiceMock<MockILogger>>;
using StatusResponse_t = protocol::StatusResponse;
using ResponseAttributes_t = protocol::ResponseAttributes;
using IResponseAttributes_sp = std::shared_ptr<protocol::IResponseAttributes>;
using Utf8String_t = core::UTF8String;
using HttpHeaderCollection_t = protocol::HttpHeaderCollection;

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
	ASSERT_THAT(target->isErroneousResponse(), testing::Eq(true));
}

TEST_F(StatusResponseTest, isErroneousResponseGivesTrueForErrorCodeGreaterThan400)
{
	// given
	auto target = StatusResponse_t::createErrorResponse(logger, 401);

	// then
	ASSERT_THAT(target->isErroneousResponse(), testing::Eq(true));
}

TEST_F(StatusResponseTest, isErroneousResponseGivesFalseForErrorCodeLessThan400)
{
	// given
	auto target = StatusResponse_t::createSuccessResponse(logger, attributes, 399, HttpHeaderCollection_t());

	// then
	ASSERT_THAT(target->isErroneousResponse(), testing::Eq(false));
}

TEST_F(StatusResponseTest, isTooManyRequestsResponseGivesTrueIfResponseCodeIsEqualTo429)
{
	// given
	auto target = StatusResponse_t::createErrorResponse(logger, 429);

	// then
	ASSERT_THAT(target->isTooManyRequestsResponse(), testing::Eq(true));
}

TEST_F(StatusResponseTest, isTooManyRequestsResponseGivesFalseIfResponseCodeIsNotEqualTo429)
{
	// given
	auto target = StatusResponse_t::createErrorResponse(logger, 404);

	// then
	ASSERT_THAT(target->isTooManyRequestsResponse(), testing::Eq(false));
}

TEST_F(StatusResponseTest, isErroneousResponseGivesTrueIfStatusResponseAttributeIndicatesError)
{
	// given
	attributes = ResponseAttributes_t::withUndefinedDefaults().withStatus(StatusResponse_t::RESPONSE_STATUS_ERROR).build();

	auto target = StatusResponse_t::createSuccessResponse(logger, attributes, StatusResponse_t::HTTP_OK, HttpHeaderCollection_t());

	// when, then
	ASSERT_THAT(target->isErroneousResponse(), testing::Eq(true));
}

TEST_F(StatusResponseTest, isErroneousResponseGivesFalseIfStatusResponseAttributeDoesNotIndicateError)
{
	// given
	std::string data = StatusResponse_t::RESPONSE_STATUS_ERROR.getStringData();
	std::transform(data.begin(), data.end(), data.begin(),
		[](unsigned char c) { return static_cast<unsigned char>(std::tolower(c)); });
	core::UTF8String status{ data };
	attributes = ResponseAttributes_t::withUndefinedDefaults().withStatus(status).build();

	auto target = StatusResponse_t::createSuccessResponse(logger, attributes, StatusResponse_t::HTTP_OK, HttpHeaderCollection_t());

	// when, then
	ASSERT_THAT(target->isErroneousResponse(), testing::Eq(false));
}

TEST_F(StatusResponseTest, isErroneousResponseGivesFalseIfStatusResponseAttributeIsNotSet)
{
	// given
	attributes = ResponseAttributes_t::withUndefinedDefaults().build();

	auto target = StatusResponse_t::createSuccessResponse(logger, attributes, StatusResponse_t::HTTP_OK, HttpHeaderCollection_t());

	// when, then
	ASSERT_THAT(target->isErroneousResponse(), testing::Eq(false));
}

TEST_F(StatusResponseTest, getRetryAfterReturnsDefaultValueIfResponseKeyDoesNotExist)
{
	// given
	auto target = StatusResponse_t::createErrorResponse(logger, 429);

	// when
	auto obtained = target->getRetryAfterInMilliseconds();

	// then
	ASSERT_THAT(obtained, testing::Eq(DEFAULT_RETRY_AFTER_IN_MILLISECONDS));
}

TEST_F(StatusResponseTest, getRetryAfterReturnsDefaultValueIfMultipleValuesWereRetrieved)
{
	// given
	auto responseHeaders = HttpHeaderCollection_t
	{
		{ RESPONSE_KEY_RETRY_AFTER, { "100", "200" } }
	};
	auto target = StatusResponse_t::createErrorResponse(logger, 429, responseHeaders);

	// when
	auto obtained = target->getRetryAfterInMilliseconds();

	// then
	ASSERT_THAT(obtained, testing::Eq(DEFAULT_RETRY_AFTER_IN_MILLISECONDS));
}

TEST_F(StatusResponseTest, getRetryAfterReturnsDefaultValueIfValueIsNotAnIntegerValue)
{
	// given
	auto responseHeaders = HttpHeaderCollection_t
	{
		{ RESPONSE_KEY_RETRY_AFTER, { "a" } }
	};
	auto target = StatusResponse_t::createErrorResponse(logger, 429, responseHeaders);

	// when
	auto obtained = target->getRetryAfterInMilliseconds();

	// then
	ASSERT_THAT(obtained, testing::Eq(DEFAULT_RETRY_AFTER_IN_MILLISECONDS));
}

TEST_F(StatusResponseTest, getRetryAfterReturnsDefaultValueIfValueIsOutOfIntegerRange)
{
	// given
	auto responseHeaders = HttpHeaderCollection_t
	{
		// use string value 2^31, which is one too high for int32_t
		{ RESPONSE_KEY_RETRY_AFTER, { "2147483648" } }
	};
	auto target = StatusResponse_t::createErrorResponse(logger, 429, responseHeaders);

	// when
	auto obtained = target->getRetryAfterInMilliseconds();

	// then
	ASSERT_THAT(obtained, testing::Eq(DEFAULT_RETRY_AFTER_IN_MILLISECONDS));
}

TEST_F(StatusResponseTest, getRetryAfterReturnsParsedValue)
{
	// given
	auto responseHeaders = HttpHeaderCollection_t
	{
		{ RESPONSE_KEY_RETRY_AFTER, { "1234" } }
	};
	auto target = StatusResponse_t::createErrorResponse(logger, 429, responseHeaders);

	// when
	auto obtained = target->getRetryAfterInMilliseconds();

	// then
	ASSERT_THAT(obtained, testing::Eq(1234L * 1000L));
}
