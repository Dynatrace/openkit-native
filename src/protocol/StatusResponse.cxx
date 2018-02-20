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

StatusResponse::StatusResponse(core::UTF8String response, uint32_t responseCode)
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

void StatusResponse::parseResponse(const core::UTF8String response)
{

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
