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

constexpr char RESPONSE_KEY_CAPTURE[] = "cp";
constexpr char RESPONSE_KEY_SEND_INTERVAL[] = "si";
constexpr char RESPONSE_KEY_MONITOR_NAME[] = "bn";
constexpr char RESPONSE_KEY_SERVER_ID[] = "id";
constexpr char RESPONSE_KEY_MAX_BEACON_SIZE[] = "bl";
constexpr char RESPONSE_KEY_CAPTURE_ERRORS[] = "er";
constexpr char RESPONSE_KEY_CAPTURE_CRASHES[] = "cr";
constexpr char RESPONSE_KEY_MULTIPLICITY[] = "mp";

#include "StatusResponse.h"

#include <stdexcept>
#include <sstream>

using namespace protocol;

StatusResponse::StatusResponse(std::shared_ptr<openkit::ILogger> logger, const core::UTF8String& response, int32_t responseCode, const Response::ResponseHeaders& responseHeaders)
	: Response(logger, responseCode, responseHeaders)
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
