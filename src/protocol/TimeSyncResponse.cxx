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
	std::stringstream ss(response.getStringData());
	std::string item;
	while (std::getline(ss, item, '&'))
	{
		size_t found = item.find('=');
		if (found != std::string::npos)
		{
			std::string key = item.substr(0, found);
			std::string value = item.substr(found + 1);

			if (!key.empty() && !value.empty())
			{
				if (key.compare(RESPONSE_KEY_REQUEST_RECEIVE_TIME) == 0)
				{
					mRequestReceiveTime = std::stol(value);
				}
				else if (key.compare(RESPONSE_KEY_RESPONSE_SEND_TIME) == 0)
				{
					mResponseSendTime = std::stol(value);
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
