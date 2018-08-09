/**
* Copyright 2018 Dynatrace LLC
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

#include "Response.h"

#include <inttypes.h>

using namespace protocol;

static constexpr int32_t HTTP_BAD_REQUEST = 400;
static constexpr char RESPONSE_KEY_RETRY_AFTER[] = "retry-after";
static constexpr int64_t DEFAULT_RETRY_AFTER_IN_MILLISECONDS = 10L * 60L * 1000L; // 10 minutes in milliseconds

Response::Response(std::shared_ptr<openkit::ILogger> logger, int32_t responseCode, const ResponseHeaders& responseHeaders)
	: mLogger(logger)
	, mResponseCode(responseCode)
	, mResponseHeaders(responseHeaders)
{
}

bool Response::isErroneousResponse() const
{
	return getResponseCode() >= HTTP_BAD_REQUEST;
}

int32_t Response::getResponseCode() const
{
	return mResponseCode;
}

const Response::ResponseHeaders& Response::getResponseHeaders() const
{
	return mResponseHeaders;
}

int64_t Response::getRetryAfterInMilliseconds() const
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
