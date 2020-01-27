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


#include "StatusResponse.h"
#include "ResponseAttributes.h"

#include <stdexcept>
#include <sstream>
#include <inttypes.h>

using namespace protocol;

static constexpr int32_t HTTP_BAD_REQUEST = 400;
static constexpr int32_t HTTP_TOO_MANY_REQUESTS = 429;

static constexpr char RESPONSE_KEY_RETRY_AFTER[] = "retry-after";
static constexpr int64_t DEFAULT_RETRY_AFTER_IN_MILLISECONDS = 10L * 60L * 1000L; // 10 minutes in milliseconds

StatusResponse::StatusResponse
(
	std::shared_ptr<openkit::ILogger> logger,
	std::shared_ptr<IResponseAttributes> responseAttributes,
	int32_t responseCode,
	const ResponseHeaders& responseHeaders
)
	: mLogger(logger)
	, mResponseAttributes(responseAttributes)
	, mResponseCode(responseCode)
	, mResponseHeaders(responseHeaders)
{
}

std::shared_ptr<StatusResponse> StatusResponse::createSuccessResponse(
	std::shared_ptr<openkit::ILogger> logger,
	std::shared_ptr<IResponseAttributes> responseAttributes,
	int32_t responseCode,
	const ResponseHeaders& responseHeaders)
{
	return std::shared_ptr<StatusResponse>(new StatusResponse(logger, responseAttributes, responseCode, responseHeaders));
}

std::shared_ptr<StatusResponse> StatusResponse::createErrorResponse(
	std::shared_ptr<openkit::ILogger> logger,
	int32_t responseCode
)
{
	return createErrorResponse(logger, responseCode, ResponseHeaders());
}

std::shared_ptr<StatusResponse> StatusResponse::createErrorResponse(
	std::shared_ptr<openkit::ILogger> logger,
	int32_t responseCode,
	const ResponseHeaders& responseHeaders
)
{
	auto responseAttributes = ResponseAttributes::withUndefinedDefaults().build();
	return std::shared_ptr<StatusResponse>(new StatusResponse(logger, responseAttributes, responseCode, responseHeaders));
}

bool StatusResponse::isErroneousResponse() const
{
	return getResponseCode() >= HTTP_BAD_REQUEST;
}

bool StatusResponse::isTooManyRequestsResponse() const
{
	return getResponseCode() == HTTP_TOO_MANY_REQUESTS;
}

int32_t StatusResponse::getResponseCode() const
{
	return mResponseCode;
}

const IStatusResponse::ResponseHeaders& StatusResponse::getResponseHeaders() const
{
	return mResponseHeaders;
}

int64_t StatusResponse::getRetryAfterInMilliseconds() const
{
	auto iterator = mResponseHeaders.find(RESPONSE_KEY_RETRY_AFTER);
	if (iterator == mResponseHeaders.end())
	{
		// the Retry-After response header is missing
		mLogger->warning("%s is not available - using default value %" PRId64,
			RESPONSE_KEY_RETRY_AFTER, DEFAULT_RETRY_AFTER_IN_MILLISECONDS);
		return DEFAULT_RETRY_AFTER_IN_MILLISECONDS;
	}

	if (iterator->second.size() != 1)
	{
		// the Retry-After response header has multiple values, but only one is expected
		mLogger->warning("%s has unexpected number of values - using default value %" PRId64,
			RESPONSE_KEY_RETRY_AFTER, DEFAULT_RETRY_AFTER_IN_MILLISECONDS);
		return DEFAULT_RETRY_AFTER_IN_MILLISECONDS;
	}

	auto stringValue = iterator->second.front();
	int32_t delaySeconds;
	try
	{
		delaySeconds = std::stoi(stringValue);
	}
	catch (const std::invalid_argument& e)
	{
		mLogger->error("Failed to parse %s value \"%s\" (std::invalid_argument - reason: %s) - using default value %" PRId64,
			RESPONSE_KEY_RETRY_AFTER, stringValue.c_str(), e.what(), DEFAULT_RETRY_AFTER_IN_MILLISECONDS);
		return DEFAULT_RETRY_AFTER_IN_MILLISECONDS;

	}
	catch (const std::out_of_range& e)
	{
		mLogger->error("Failed to parse %s value \"%s\" (std::out_of_range - reason: %s) - using default value %" PRId64,
			RESPONSE_KEY_RETRY_AFTER, stringValue.c_str(), e.what(), DEFAULT_RETRY_AFTER_IN_MILLISECONDS);
		return DEFAULT_RETRY_AFTER_IN_MILLISECONDS;
	}

	return delaySeconds * 1000L;
}

std::shared_ptr<IResponseAttributes> StatusResponse::getResponseAttributes() const
{
	return mResponseAttributes;
}