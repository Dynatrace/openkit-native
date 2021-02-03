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

#ifndef _PROTOCOL_KEYVALUERESPONSEPARSER_H
#define _PROTOCOL_KEYVALUERESPONSEPARSER_H

#include "IResponseAttributes.h"
#include "ResponseAttributes.h"
#include "core/UTF8String.h"

#include <memory>
#include <unordered_map>

namespace protocol
{
	class KeyValueResponseParser
	{
	public:

		static constexpr const char* RESPONSE_KEY_MAX_BEACON_SIZE_IN_KB = "bl";
		static constexpr const char* RESPONSE_KEY_SEND_INTERVAL_IN_SEC = "si";

		static constexpr const char* RESPONSE_KEY_CAPTURE = "cp";
		static constexpr const char* RESPONSE_KEY_REPORT_CRASHES = "cr";
		static constexpr const char* RESPONSE_KEY_REPORT_ERRORS = "er";

		static constexpr const char* RESPONSE_KEY_SERVER_ID = "id";
		static constexpr const char* RESPONSE_KEY_MULTIPLICITY = "mp";

		static std::shared_ptr<IResponseAttributes> parse(const core::UTF8String& keyValuePairResponse);

	private:

		KeyValueResponseParser() {}

		static void applyBeaconSizeInKb(
			protocol::ResponseAttributes::Builder& builder,
			std::unordered_map<std::string, std::string>& keyValuePairs
		);

		static void applySendIntervalInSec(
			protocol::ResponseAttributes::Builder& builder,
			std::unordered_map<std::string, std::string>& keyValuePairs
		);

		static void applyCapture(
			protocol::ResponseAttributes::Builder& builder,
			std::unordered_map<std::string, std::string>& keyValuePairs
		);

		static void applyReportCrashes(
			protocol::ResponseAttributes::Builder& builder,
			std::unordered_map<std::string, std::string>& keyValuePairs
		);

		static void applyReportErrors(
			protocol::ResponseAttributes::Builder& builder,
			std::unordered_map<std::string, std::string>& keyValuePairs
		);

		static void applyServerId(
			protocol::ResponseAttributes::Builder& builder,
			std::unordered_map<std::string, std::string>& keyValuePairs
		);

		static void applyMultiplicity(
			protocol::ResponseAttributes::Builder& builder,
			std::unordered_map<std::string, std::string>& keyValuePairs
		);
	};
}

#endif
