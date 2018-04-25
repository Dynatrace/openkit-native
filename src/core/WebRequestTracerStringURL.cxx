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

#include "WebRequestTracerStringURL.h"

#include "protocol/Beacon.h"
#include "core/RootAction.h"
#include "core/Action.h"

namespace core
{
	WebRequestTracerStringURL::WebRequestTracerStringURL(std::shared_ptr<openkit::ILogger> logger, std::shared_ptr<protocol::Beacon> beacon, int32_t parentActionID, const UTF8String& url)
		: WebRequestTracerBase(logger, beacon, parentActionID)
	{
		uint32_t indexOfQuestionMark = url.getIndexOf("?");

		if (indexOfQuestionMark > 0 && indexOfQuestionMark != std::string::npos)
		{
			WebRequestTracerBase::mURL = url.substring(0, indexOfQuestionMark);
		}
		else
		{
			WebRequestTracerBase::mURL = url;
		}
	}

}