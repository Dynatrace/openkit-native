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

#ifndef _PROTOCOL_JSONRESPONSEPARSER_H
#define _PROTOCOL_JSONRESPONSEPARSER_H


#include "IResponseAttributes.h"
#include "ResponseAttributes.h"
#include "core/UTF8String.h"
#include "util/json/objects/JsonObjectValue.h"
#include "util/json/objects/JsonNumberValue.h"
#include "util/json/objects/JsonStringValue.h"

#include <memory>

namespace protocol
{
	class JsonResponseParser
	{
	public:

		static constexpr const char* RESPONSE_KEY_AGENT_CONFIG = "mobileAgentConfig";
		static constexpr const char* RESPONSE_KEY_MAX_BEACON_SIZE_IN_KB = "maxBeaconSizeKb";
		static constexpr const char* RESPONSE_KEY_MAX_SESSION_DURATION_IN_MIN = "maxSessionDurationMins";
		static constexpr const char* RESPONSE_KEY_MAX_EVENTS_PER_SESSION = "maxEventsPerSession";
		static constexpr const char* RESPONSE_KEY_SESSION_TIMEOUT_IN_SEC = "sessionTimeoutSec";
		static constexpr const char* RESPONSE_KEY_SEND_INTERVAL_IN_SEC = "sendIntervalSec";
		static constexpr const char* RESPONSE_KEY_VISIT_STORE_VERSION = "visitStoreVersion";

		static constexpr const char* RESPONSE_KEY_APP_CONFIG = "appConfig";
		static constexpr const char* RESPONSE_KEY_CAPTURE = "capture";
		static constexpr const char* RESPONSE_KEY_REPORT_CRASHES = "reportCrashes";
		static constexpr const char* RESPONSE_KEY_REPORT_ERRORS = "reportErrors";

		static constexpr const char* RESPONSE_KEY_DYNAMIC_CONFIG = "dynamicConfig";
		static constexpr const char* RESPONSE_KEY_APPLICATION_ID = "applicationId";
		static constexpr const char* RESPONSE_KEY_MULTIPLICITY = "multiplicity";
		static constexpr const char* RESPONSE_KEY_SERVER_ID = "serverId";

		static constexpr const char* RESPONSE_KEY_STATUS = "status";

		static constexpr const char* RESPONSE_KEY_TIMESTAMP_IN_MILLIS = "timestamp";

		static std::shared_ptr<protocol::IResponseAttributes> parse(const core::UTF8String& jsonResponse);

	private:

		JsonResponseParser() {}

		static std::shared_ptr<util::json::objects::JsonObjectValue> getJsonObjectFrom(
			std::shared_ptr<util::json::objects::JsonObjectValue> jsonObject,
			const std::string& key
		);

		static std::shared_ptr<util::json::objects::JsonStringValue> getJsonStringFrom(
			std::shared_ptr<util::json::objects::JsonObjectValue> jsonObject,
			const std::string& key
		);

		static std::shared_ptr<util::json::objects::JsonNumberValue> getJsonNumberFrom(
			std::shared_ptr<util::json::objects::JsonObjectValue> jsonObject,
			const std::string& key
		);

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// Agent configuration
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		static void applyAgentConfiguration(
			protocol::ResponseAttributes::Builder& builder,
			std::shared_ptr<util::json::objects::JsonObjectValue> rootObject
		);

		static void applyBeaconSizeInKb(
			protocol::ResponseAttributes::Builder& builder,
			std::shared_ptr<util::json::objects::JsonObjectValue> agentConfigObject
		);

		static void applyMaxSessionDurationInMin(
			protocol::ResponseAttributes::Builder& builder,
			std::shared_ptr<util::json::objects::JsonObjectValue> agentConfigObject
		);

		static void applyMaxEventsPerSession(
			protocol::ResponseAttributes::Builder& builder,
			std::shared_ptr<util::json::objects::JsonObjectValue> agentConfigObject
		);

		static void applySessionTimeoutInSec(
			protocol::ResponseAttributes::Builder& builder,
			std::shared_ptr<util::json::objects::JsonObjectValue> agentConfigObject
		);

		static void applySendIntervalInSec(
			protocol::ResponseAttributes::Builder& builder,
			std::shared_ptr<util::json::objects::JsonObjectValue> agentConfigObject
		);

		static void applyVisitStoreVersion(
			protocol::ResponseAttributes::Builder& builder,
			std::shared_ptr<util::json::objects::JsonObjectValue> agentConfigObject
		);

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// Application configuration
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		static void applyApplicationConfiguration(
			protocol::ResponseAttributes::Builder& builder,
			std::shared_ptr<util::json::objects::JsonObjectValue> rootObject
		);

		static void applyCapture(
			protocol::ResponseAttributes::Builder& builder,
			std::shared_ptr<util::json::objects::JsonObjectValue> appConfigObject
		);

		static void applyReportCrashes(
			protocol::ResponseAttributes::Builder& builder,
			std::shared_ptr<util::json::objects::JsonObjectValue> appConfigObject
		);

		static void applyReportErrors(
			protocol::ResponseAttributes::Builder& builder,
			std::shared_ptr<util::json::objects::JsonObjectValue> appConfigObject
		);

		static void applyApplicationId(
			protocol::ResponseAttributes::Builder& builder,
			std::shared_ptr<util::json::objects::JsonObjectValue> dynConfigObject
		);

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// Dynamic configuration
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		static void applyDynamicConfiguration(
			protocol::ResponseAttributes::Builder& builder,
			std::shared_ptr<util::json::objects::JsonObjectValue> rootObject
		);

		static void applyMultiplicity(
			protocol::ResponseAttributes::Builder& builder,
			std::shared_ptr<util::json::objects::JsonObjectValue> dynConfigObject
		);

		static void applyServerId(
			protocol::ResponseAttributes::Builder& builder,
			std::shared_ptr<util::json::objects::JsonObjectValue> dynConfigObject
		);

		static void applyStatus(
			protocol::ResponseAttributes::Builder& builder,
			std::shared_ptr<util::json::objects::JsonObjectValue> dynConfigObject
		);

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// Root object configuration
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		static void applyRootAttributes(
			protocol::ResponseAttributes::Builder& builder,
			std::shared_ptr<util::json::objects::JsonObjectValue> rootObject
		);
	};
}


#endif
