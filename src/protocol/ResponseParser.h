/**
 * Copyright 2018-2020 Dynatrace LLC
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

#ifndef _PROTOCOL_RESPONSEPARSER_H
#define _PROTOCOL_RESPONSEPARSER_H

#include "IResponseAttributes.h"
#include "core/UTF8String.h"

#include <string>
#include <memory>

namespace protocol
{

	///
	/// Evaluates a response string and transforms it into corresponding @ref IResponseAttributes
	///
	class ResponseParser
	{
	public:

		static const std::string KEY_VALUE_RESPONSE_TYPE_MOBILE;
		static const std::string KEY_VALUE_RESPONSE_TYPE_MOBILE_WITH_SEPARATOR;

		static std::shared_ptr<IResponseAttributes> parseResponse(const core::UTF8String& responseString);

	private:

		static bool isKeyValuePairResponse(const core::UTF8String& responseString);
	};
}

#endif
