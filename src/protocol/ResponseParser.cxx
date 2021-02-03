/**
 * Copyright 2018-2021 Dynatrace LLC
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

#include "ResponseParser.h"
#include "JsonResponseParser.h"
#include "KeyValueResponseParser.h"

using namespace protocol;

std::shared_ptr<IResponseAttributes> ResponseParser::parseResponse(const core::UTF8String& responseString)
{
	if (isKeyValuePairResponse(responseString))
	{
		return KeyValueResponseParser::parse(responseString);
	}

	return JsonResponseParser::parse(responseString);
}

bool ResponseParser::isKeyValuePairResponse(const core::UTF8String& responseString)
{
	auto response = responseString.getStringData();
	return response == KEY_VALUE_RESPONSE_TYPE_MOBILE
		|| response.find(KEY_VALUE_RESPONSE_TYPE_MOBILE_WITH_SEPARATOR) == 0;
}