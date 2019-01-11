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
#include "protocol/Response.h"
#include "core/util/DefaultLogger.h"

#include <gtest/gtest.h>

using namespace protocol;

class ResponseTest : public testing::Test
{
public:
	void SetUp()
	{
		logger = std::shared_ptr<openkit::ILogger>(new core::util::DefaultLogger(devNull, true));
	}

	void TearDown()
	{
	}

protected:

	class TestResponse : public Response
	{
	public:

		TestResponse(std::shared_ptr<openkit::ILogger> logger, int32_t responseCode, const Response::ResponseHeaders& responseHeaders) :
			Response(logger, responseCode, responseHeaders)
		{
		}
	};
	std::ostringstream devNull;
	std::shared_ptr<openkit::ILogger> logger;
};

static constexpr char RESPONSE_KEY_RETRY_AFTER[] = "retry-after";
static constexpr int64_t DEFAULT_RETRY_AFTER_IN_MILLISECONDS = 10L * 60L * 1000L;

TEST_F(ResponseTest, isSuccessfulResponseGivesTrueForResponseCodesLessThan400)
{
	// given
	auto target = TestResponse(logger, 399, Response::ResponseHeaders());

	// then
	ASSERT_TRUE(target.isSuccessfulResponse());
}

TEST_F(ResponseTest, isSuccessfulResponseGivesFalseForResponseCodesEqualTo400)
{
	// given
	auto target = TestResponse(logger, 400, Response::ResponseHeaders());

	// then
	ASSERT_FALSE(target.isSuccessfulResponse());
}

TEST_F(ResponseTest, isSuccessfulResponseGivesFalseForResponseCodesGreaterThan400)
{
	// given
	auto target = TestResponse(logger, 401, Response::ResponseHeaders());

	// then
	ASSERT_FALSE(target.isSuccessfulResponse());
}

TEST_F(ResponseTest, isErroneousResponseGivesTrueForErrorCodeEqualTo400)
{
	// given
	auto target = TestResponse(logger, 400, Response::ResponseHeaders());

	// then
	ASSERT_TRUE(target.isErroneousResponse());
}

TEST_F(ResponseTest, isErroneousResponseGivesTrueForErrorCodeGreaterThan400)
{
	// given
	auto target = TestResponse(logger, 401, Response::ResponseHeaders());

	// then
	ASSERT_TRUE(target.isErroneousResponse());
}

TEST_F(ResponseTest, isErroneousResponseGivesFalseForErrorCodeLessThan400)
{
	// given
	auto target = TestResponse(logger, 399, Response::ResponseHeaders());

	// then
	ASSERT_FALSE(target.isErroneousResponse());
}

TEST_F(ResponseTest, isTooManyRequestsResponseGivesTrueIfResponseCodeIsEqualTo429)
{
	// given
	auto target = TestResponse(logger, 429, Response::ResponseHeaders());

	// then
	ASSERT_TRUE(target.isTooManyRequestsResponse());
}

TEST_F(ResponseTest, isTooManyRequestsResponseGivesFalseIfResponseCodeIsNotEqualTo429)
{
	// given
	auto target = TestResponse(logger, 404, Response::ResponseHeaders());

	// then
	ASSERT_FALSE(target.isTooManyRequestsResponse());
}

TEST_F(ResponseTest, responseCodeIsSet)
{
	// given
	auto target = TestResponse(logger, 418, Response::ResponseHeaders());

	// then
	ASSERT_EQ(418, target.getResponseCode());
}

TEST_F(ResponseTest, headersAreSet)
{
	// given
	auto headers = Response::ResponseHeaders
	{
		{ "X-Foo", std::vector<std::string> { "X-BAR" } },
		{ "X-YZ", std::vector<std::string> { } }
	};
	auto target = TestResponse(logger, 418, headers);

	// then
	ASSERT_EQ(headers, target.getResponseHeaders());
}

TEST_F(ResponseTest, getRetryAfterReturnsDefaultValueIfResponseKeyDoesNotExist)
{
	// given
	auto target = TestResponse(logger, 429, Response::ResponseHeaders());

	// when
	auto obtained = target.getRetryAfterInMilliseconds();

	// then
	ASSERT_EQ(DEFAULT_RETRY_AFTER_IN_MILLISECONDS, obtained);
}

TEST_F(ResponseTest, getRetryAfterReturnsDefaultValueIfMultipleValuesWereRetrieved)
{
	// given
	auto responseHeaders = Response::ResponseHeaders
	{
		{ RESPONSE_KEY_RETRY_AFTER, std::vector<std::string>{ "100", "200" } }
	};
	auto target = TestResponse(logger, 429, responseHeaders);

	// when
	auto obtained = target.getRetryAfterInMilliseconds();

	// then
	ASSERT_EQ(DEFAULT_RETRY_AFTER_IN_MILLISECONDS, obtained);
}

TEST_F(ResponseTest, getRetryAfterReturnsDefaultValueIfValueIsNotAnIntegerValue)
{
	// given
	auto responseHeaders = Response::ResponseHeaders
	{
		{ RESPONSE_KEY_RETRY_AFTER, std::vector<std::string>{ "a" } }
	};
	auto target = TestResponse(logger, 429, responseHeaders);

	// when
	auto obtained = target.getRetryAfterInMilliseconds();

	// then
	ASSERT_EQ(DEFAULT_RETRY_AFTER_IN_MILLISECONDS, obtained);
}

TEST_F(ResponseTest, getRetryAfterReturnsDefaultValueIfValueIsOutOfIntegerRange)
{
	// given
	auto responseHeaders = Response::ResponseHeaders
	{
		// use string value 2^31, which is one too high for int32_t
		{ RESPONSE_KEY_RETRY_AFTER, std::vector<std::string>{ "2147483648" } }
	};
	auto target = TestResponse(logger, 429, responseHeaders);

	// when
	auto obtained = target.getRetryAfterInMilliseconds();

	// then
	ASSERT_EQ(DEFAULT_RETRY_AFTER_IN_MILLISECONDS, obtained);
}

TEST_F(ResponseTest, getRetryAfterReturnsParsedValue)
{
	// given
	auto responseHeaders = Response::ResponseHeaders
	{
		{ RESPONSE_KEY_RETRY_AFTER, std::vector<std::string>{ "1234" } }
	};
	auto target = TestResponse(logger, 429, responseHeaders);

	// when
	auto obtained = target.getRetryAfterInMilliseconds();

	// then
	ASSERT_EQ(1234L * 1000L, obtained);
}
