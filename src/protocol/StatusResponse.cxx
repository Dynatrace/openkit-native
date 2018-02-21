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

#include "StatusResponse.h"

#include <stdexcept>
#include <sstream>

using namespace protocol;

StatusResponse::StatusResponse()
	: Response(0)
	, mCapture(true)
	, mSendInterval(-1)
	, mMonitorName()
	, mServerID(-1)
	, mMaxBeaconSize(-1)
	, mCaptureErrors(true)
	, mCaptureCrashes(true)
{
}

StatusResponse::StatusResponse(const core::UTF8String& response, uint32_t responseCode)
	: Response(responseCode)
	, mCapture(true)
	, mSendInterval(-1)
	, mMonitorName()
	, mServerID(-1)
	, mMaxBeaconSize(-1)
	, mCaptureErrors(true)
	, mCaptureCrashes(true)
{
	parseResponse(response);
}

void StatusResponse::parseResponse(const core::UTF8String& response)
{
	std::stringstream ss(response.getStringData());
	std::string item;
	while (std::getline(ss, item, '&'))
	{
		auto found = item.find('=');
		if (found != std::string::npos)
		{
			auto key = item.substr(0, found);
			auto value = item.substr(found + 1);

			if (!key.empty() && !value.empty())
			{
				if (key.compare(RESPONSE_KEY_CAPTURE) == 0)
				{
					mCapture = std::stoi(value) == 1;
				}
				else if (key.compare(RESPONSE_KEY_SEND_INTERVAL) == 0)
				{
					mSendInterval = std::stoi(value) * 1000;
				}
				else if (key.compare(RESPONSE_KEY_MONITOR_NAME) == 0)
				{
					mMonitorName = core::UTF8String(value.c_str());
				}
				else if (key.compare(RESPONSE_KEY_SERVER_ID) == 0)
				{
					mServerID = std::stoi(value);
				}
				else if (key.compare(RESPONSE_KEY_MAX_BEACON_SIZE) == 0)
				{
					mMaxBeaconSize = std::stoi(value);
				}
				else if (key.compare(RESPONSE_KEY_CAPTURE_ERRORS) == 0)
				{
					/* 1 (always on) and 2 (only on WiFi) are treated the same */
					mCaptureErrors = std::stoi(value) != 0;
				}
				else if (key.compare(RESPONSE_KEY_CAPTURE_CRASHES) == 0)
				{
					/* 1 (always on) and 2 (only on WiFi) are treated the same */
					mCaptureCrashes = std::stoi(value) != 0;
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
