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

constexpr char RESPONSE_KEY_REQUEST_RECEIVE_TIME[] = "t1";
constexpr char RESPONSE_KEY_RESPONSE_SEND_TIME[] = "t2";

#include "TimeSyncResponse.h"

#include <sstream>
#include <stdexcept>

using namespace protocol;

TimeSyncResponse::TimeSyncResponse()
	: Response(0)
	, mRequestReceiveTime(-1)
	, mResponseSendTime(-1)
{
}

TimeSyncResponse::TimeSyncResponse(const core::UTF8String& response, uint32_t responseCode)
	: Response(responseCode)
	, mRequestReceiveTime(-1)
	, mResponseSendTime(-1)
{
	parseResponse(response);
}

void TimeSyncResponse::parseResponse(const core::UTF8String& response)
{
	auto parts = response.split('&');
	for (auto const& part : parts) {
		auto found = part.getIndexOf("=");
		if (found != std::string::npos)
		{
			auto key = part.substring(0, found);
			auto value = part.substring(found + 1);

			if (!key.empty() && !value.empty())
			{
				if (key.equals(RESPONSE_KEY_REQUEST_RECEIVE_TIME))
				{
					mRequestReceiveTime = std::stoll(value.getStringData());
				}
				else if (key.equals(RESPONSE_KEY_RESPONSE_SEND_TIME))
				{
					mResponseSendTime = std::stoll(value.getStringData());
				}
			}
		}
	}
}

int64_t TimeSyncResponse::getRequestReceiveTime() const
{
	return mRequestReceiveTime;
}

int64_t TimeSyncResponse::getResponseSendTime() const
{
	return mResponseSendTime;
}
