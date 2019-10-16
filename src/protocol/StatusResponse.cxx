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

#include <stdexcept>
#include <sstream>
#include <inttypes.h>

using namespace protocol;

static constexpr int32_t HTTP_BAD_REQUEST = 400;
static constexpr int32_t HTTP_TOO_MANY_REQUESTS = 429;

static constexpr char RESPONSE_KEY_RETRY_AFTER[] = "retry-after";
static constexpr int64_t DEFAULT_RETRY_AFTER_IN_MILLISECONDS = 10L * 60L * 1000L; // 10 minutes in milliseconds

constexpr char RESPONSE_KEY_CAPTURE[] = "cp";
constexpr char RESPONSE_KEY_SEND_INTERVAL[] = "si";
constexpr char RESPONSE_KEY_MONITOR_NAME[] = "bn";
constexpr char RESPONSE_KEY_SERVER_ID[] = "id";
constexpr char RESPONSE_KEY_MAX_BEACON_SIZE[] = "bl";
constexpr char RESPONSE_KEY_CAPTURE_ERRORS[] = "er";
constexpr char RESPONSE_KEY_CAPTURE_CRASHES[] = "cr";
constexpr char RESPONSE_KEY_MULTIPLICITY[] = "mp";

StatusResponse::StatusResponse
(
	std::shared_ptr<openkit::ILogger> logger,
	const core::UTF8String& response,
	int32_t responseCode,
	const ResponseHeaders& responseHeaders
)
	: mLogger(logger)
	, mResponseCode(responseCode)
	, mResponseHeaders(responseHeaders)
	, mCapture(true)
	, mSendInterval(-1)
	, mMonitorName()
	, mServerID(-1)
	, mMaxBeaconSize(-1)
	, mCaptureErrors(true)
	, mCaptureCrashes(true)
	, mMultiplicity(1)
{
	parseResponse(response);
}

bool StatusResponse::isSuccessfulResponse() const
{
	return !isErroneousResponse();
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

void StatusResponse::parseResponse(const core::UTF8String& response)
{
	auto parts = response.split('&');
	for (auto const& part : parts)
	{
		auto found = part.getIndexOf("=");
		if (found != std::string::npos)
		{
			auto key = part.substring(0, found);
			auto value = part.substring(found + 1);

			if (!key.empty() && !value.empty())
			{
				if (key.equals(RESPONSE_KEY_CAPTURE))
				{
					mCapture = std::stoi(value.getStringData()) == 1;
				}
				else if (key.equals(RESPONSE_KEY_SEND_INTERVAL))
				{
					mSendInterval = std::stoi(value.getStringData()) * 1000;
				}
				else if (key.equals(RESPONSE_KEY_MONITOR_NAME))
				{
					mMonitorName = core::UTF8String(value);
				}
				else if (key.equals(RESPONSE_KEY_SERVER_ID))
				{
					mServerID = std::stoi(value.getStringData());
				}
				else if (key.equals(RESPONSE_KEY_MAX_BEACON_SIZE))
				{
					mMaxBeaconSize = std::stoi(value.getStringData());
				}
				else if (key.equals(RESPONSE_KEY_CAPTURE_ERRORS))
				{
					/* 1 (always on) and 2 (only on WiFi) are treated the same */
					mCaptureErrors = std::stoi(value.getStringData()) != 0;
				}
				else if (key.equals(RESPONSE_KEY_CAPTURE_CRASHES))
				{
					/* 1 (always on) and 2 (only on WiFi) are treated the same */
					mCaptureCrashes = std::stoi(value.getStringData()) != 0;
				}
				else if (key.equals(RESPONSE_KEY_MULTIPLICITY))
				{
					mMultiplicity = std::stoi(value.getStringData());
				}
			}
		}
	}
}

bool StatusResponse::isCapture() const
{
	return mCapture;
}

int32_t StatusResponse::getSendInterval() const
{
	return mSendInterval;
}

const core::UTF8String& StatusResponse::getMonitorName() const
{
	return mMonitorName;
}

int32_t StatusResponse::getServerID() const
{
	return mServerID;
}

int32_t StatusResponse::getMaxBeaconSize() const
{
	return mMaxBeaconSize;
}

bool StatusResponse::isCaptureErrors() const
{
	return mCaptureErrors;
}

bool StatusResponse::isCaptureCrashes() const
{
	return mCaptureCrashes;
}

int32_t StatusResponse::getMultiplicity() const
{
	return mMultiplicity;
}
