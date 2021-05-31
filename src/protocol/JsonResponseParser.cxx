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

#include "JsonResponseParser.h"
#include "util/json/JsonParser.h"
#include "util/json/objects/JsonNumberValue.h"
#include "util/json/objects/JsonObjectValue.h"
#include "util/json/objects/JsonStringValue.h"

using namespace protocol;


std::shared_ptr<protocol::IResponseAttributes> JsonResponseParser::parse(const core::UTF8String& jsonResponse)
{
	auto jsonParser = util::json::JsonParser(jsonResponse.getStringData());

	auto parsedValue = jsonParser.parse();
	auto rootObject  = std::dynamic_pointer_cast<util::json::objects::JsonObjectValue>(parsedValue);

	auto builder = ResponseAttributes::withJsonDefaults();

	applyAgentConfiguration(builder, rootObject);
	applyApplicationConfiguration(builder, rootObject);
	applyDynamicConfiguration(builder, rootObject);
	applyRootAttributes(builder, rootObject);

	return builder.build();
}

std::shared_ptr<util::json::objects::JsonObjectValue> JsonResponseParser::getJsonObjectFrom(
	std::shared_ptr<util::json::objects::JsonObjectValue> jsonObject,
	const std::string& key
)
{
	auto entry = jsonObject->find(key);
	if (entry == jsonObject->end())
	{
		return nullptr;
	}

	return std::dynamic_pointer_cast<util::json::objects::JsonObjectValue>(entry->second);
}

std::shared_ptr<util::json::objects::JsonStringValue> JsonResponseParser::getJsonStringFrom(
	std::shared_ptr<util::json::objects::JsonObjectValue> jsonObject,
	const std::string& key
)
{
	auto entry = jsonObject->find(key);
	if (entry == jsonObject->end())
	{
		return nullptr;
	}

	return std::dynamic_pointer_cast<util::json::objects::JsonStringValue>(entry->second);
}

std::shared_ptr<util::json::objects::JsonNumberValue> JsonResponseParser::getJsonNumberFrom(
	std::shared_ptr<util::json::objects::JsonObjectValue> jsonObject,
	const std::string& key
)
{
	auto entry = jsonObject->find(key);
	if (entry == jsonObject->end())
	{
		return nullptr;
	}

	return std::dynamic_pointer_cast<util::json::objects::JsonNumberValue>(entry->second);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Agent configuration
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void JsonResponseParser::applyAgentConfiguration(
	protocol::ResponseAttributes::Builder& builder,
	std::shared_ptr<util::json::objects::JsonObjectValue> rootObject
)
{
	auto agentConfigObject = getJsonObjectFrom(rootObject, JsonResponseParser::RESPONSE_KEY_AGENT_CONFIG);
	if (agentConfigObject == nullptr)
	{
		return;
	}

	applyBeaconSizeInKb(builder, agentConfigObject);
	applyMaxSessionDurationInMin(builder, agentConfigObject);
	applyMaxEventsPerSession(builder, agentConfigObject);
	applySessionTimeoutInSec(builder, agentConfigObject);
	applySendIntervalInSec(builder, agentConfigObject);
	applyVisitStoreVersion(builder, agentConfigObject);
}

void JsonResponseParser::applyBeaconSizeInKb(
	protocol::ResponseAttributes::Builder& builder,
	std::shared_ptr<util::json::objects::JsonObjectValue> agentConfigObject
)
{
	auto numberValue = getJsonNumberFrom(agentConfigObject, JsonResponseParser::RESPONSE_KEY_MAX_BEACON_SIZE_IN_KB);
	if (numberValue == nullptr)
	{
		return;
	}

	auto beaconSizeInKb = numberValue->getInt32Value();
	builder.withMaxBeaconSizeInBytes(beaconSizeInKb * 1024);
}

void JsonResponseParser::applyMaxSessionDurationInMin(
	protocol::ResponseAttributes::Builder& builder,
	std::shared_ptr<util::json::objects::JsonObjectValue> agentConfigObject
)
{
	auto numberValue = getJsonNumberFrom(agentConfigObject, JsonResponseParser::RESPONSE_KEY_MAX_SESSION_DURATION_IN_MIN);
	if (numberValue == nullptr)
	{
		return;
	}

	auto sessionDurationMillis = numberValue->getInt32Value() * 60 * 1000;
	builder.withMaxSessionDurationInMilliseconds(sessionDurationMillis);
}

void JsonResponseParser::applyMaxEventsPerSession(
	protocol::ResponseAttributes::Builder& builder,
	std::shared_ptr<util::json::objects::JsonObjectValue> agentConfigObject
)
{
	auto numberValue = getJsonNumberFrom(agentConfigObject, JsonResponseParser::RESPONSE_KEY_MAX_EVENTS_PER_SESSION);
	if (numberValue == nullptr)
	{
		return;
	}

	auto maxEvents = numberValue->getInt32Value();
	builder.withMaxEventsPerSession(maxEvents);
}

void JsonResponseParser::applySessionTimeoutInSec(
	protocol::ResponseAttributes::Builder& builder,
	std::shared_ptr<util::json::objects::JsonObjectValue> agentConfigObject
)
{
	auto numberValue = getJsonNumberFrom(agentConfigObject, JsonResponseParser::RESPONSE_KEY_SESSION_TIMEOUT_IN_SEC);
	if (numberValue == nullptr)
	{
		return;
	}

	auto timeoutInMillis = numberValue->getInt32Value() * 1000;
	builder.withSessionTimeoutInMilliseconds(timeoutInMillis);
}

void JsonResponseParser::applySendIntervalInSec(
	protocol::ResponseAttributes::Builder& builder,
	std::shared_ptr<util::json::objects::JsonObjectValue> agentConfigObject
)
{
	auto numberValue = getJsonNumberFrom(agentConfigObject, JsonResponseParser::RESPONSE_KEY_SEND_INTERVAL_IN_SEC);
	if (numberValue == nullptr)
	{
		return;
	}

	auto intervalInMillis = numberValue->getInt32Value() * 1000;
	builder.withSendIntervalInMilliseconds(intervalInMillis);
}

void JsonResponseParser::applyVisitStoreVersion(
	protocol::ResponseAttributes::Builder& builder,
	std::shared_ptr<util::json::objects::JsonObjectValue> agentConfigObject
)
{
	auto numberValue = getJsonNumberFrom(agentConfigObject, JsonResponseParser::RESPONSE_KEY_VISIT_STORE_VERSION);
	if (numberValue == nullptr)
	{
		return;
	}

	auto visitStoreVersion = numberValue->getInt32Value();
	builder.withVisitStoreVersion(visitStoreVersion);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Application configuration
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void JsonResponseParser::applyApplicationConfiguration(
	protocol::ResponseAttributes::Builder& builder,
	std::shared_ptr<util::json::objects::JsonObjectValue> rootObject
)
{
	auto appConfigObject = getJsonObjectFrom(rootObject, JsonResponseParser::RESPONSE_KEY_APP_CONFIG);
	if (appConfigObject == nullptr)
	{
		return;
	}

	applyCapture(builder, appConfigObject);
	applyReportCrashes(builder, appConfigObject);
	applyReportErrors(builder, appConfigObject);
	applyTrafficControlPercentage(builder, appConfigObject);
	applyApplicationId(builder, appConfigObject);
}

void JsonResponseParser::applyCapture(
	protocol::ResponseAttributes::Builder& builder,
	std::shared_ptr<util::json::objects::JsonObjectValue> appConfigObject
)
{
	auto numberValue = getJsonNumberFrom(appConfigObject, JsonResponseParser::RESPONSE_KEY_CAPTURE);
	if (numberValue == nullptr)
	{
		return;
	}

	auto capture = numberValue->getInt32Value();
	builder.withCapture(capture == 1);
}

void JsonResponseParser::applyReportCrashes(
	protocol::ResponseAttributes::Builder& builder,
	std::shared_ptr<util::json::objects::JsonObjectValue> appConfigObject
)
{
	auto numberValue = getJsonNumberFrom(appConfigObject, JsonResponseParser::RESPONSE_KEY_REPORT_CRASHES);
	if (numberValue == nullptr)
	{
		return;
	}

	auto reportCrashes = numberValue->getInt32Value();
	builder.withCaptureCrashes(reportCrashes != 0);
}

void JsonResponseParser::applyReportErrors(
	protocol::ResponseAttributes::Builder& builder,
	std::shared_ptr<util::json::objects::JsonObjectValue> appConfigObject
)
{
	auto numberValue = getJsonNumberFrom(appConfigObject, JsonResponseParser::RESPONSE_KEY_REPORT_ERRORS);
	if (numberValue == nullptr)
	{
		return;
	}

	auto reportErrors = numberValue->getInt32Value();
	builder.withCaptureErrors(reportErrors != 0);
}

void JsonResponseParser::applyTrafficControlPercentage(
	protocol::ResponseAttributes::Builder& builder,
	std::shared_ptr<util::json::objects::JsonObjectValue> appConfigObject
)
{
	auto numberValue = getJsonNumberFrom(appConfigObject, JsonResponseParser::RESPONSE_KEY_TRAFFIC_CONTROL_PERCENTAGE);
	if (numberValue == nullptr)
	{
		return;
	}

	auto trafficControlPercentage = numberValue->getInt32Value();
	builder.withTrafficControlPercentage(trafficControlPercentage);
}

void JsonResponseParser::applyApplicationId(
	protocol::ResponseAttributes::Builder& builder,
	std::shared_ptr<util::json::objects::JsonObjectValue> appConfigObject
)
{
	const auto applicationId = getJsonStringFrom(appConfigObject, JsonResponseParser::RESPONSE_KEY_APPLICATION_ID);
	if (applicationId)
	{
		builder.withApplicationId(applicationId->getValue());
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Dynamic configuration
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void JsonResponseParser::applyDynamicConfiguration(
	protocol::ResponseAttributes::Builder& builder,
	std::shared_ptr<util::json::objects::JsonObjectValue> rootObject
)
{
	auto dynConfigObject = getJsonObjectFrom(rootObject, JsonResponseParser::RESPONSE_KEY_DYNAMIC_CONFIG);
	if (dynConfigObject == nullptr)
	{
		return;
	}

	applyMultiplicity(builder, dynConfigObject);
	applyServerId(builder, dynConfigObject);
	applyStatus(builder, dynConfigObject);
}

void JsonResponseParser::applyMultiplicity(
	protocol::ResponseAttributes::Builder& builder,
	std::shared_ptr<util::json::objects::JsonObjectValue> dynConfigObject
)
{
	auto numberValue = getJsonNumberFrom(dynConfigObject, JsonResponseParser::RESPONSE_KEY_MULTIPLICITY);
	if (numberValue == nullptr)
	{
		return;
	}

	auto multiplicity = numberValue->getInt32Value();
	builder.withMultiplicity(multiplicity);
}

void JsonResponseParser::applyServerId(
	protocol::ResponseAttributes::Builder& builder,
	std::shared_ptr<util::json::objects::JsonObjectValue> dynConfigObject
)
{
	auto numberValue = getJsonNumberFrom(dynConfigObject, JsonResponseParser::RESPONSE_KEY_SERVER_ID);
	if (numberValue == nullptr)
	{
		return;
	}

	auto serverId = numberValue->getInt32Value();
	builder.withServerId(serverId);
}

void JsonResponseParser::applyStatus(
	protocol::ResponseAttributes::Builder& builder,
	std::shared_ptr<util::json::objects::JsonObjectValue> dynConfigObject
)
{
	auto status = getJsonStringFrom(dynConfigObject, JsonResponseParser::RESPONSE_KEY_STATUS);
	if (status)
	{
		builder.withStatus(status->getValue());
	}	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Root object configuration
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void JsonResponseParser::applyRootAttributes(
	protocol::ResponseAttributes::Builder& builder,
	std::shared_ptr<util::json::objects::JsonObjectValue> rootObject
)
{
	auto numberValue = getJsonNumberFrom(rootObject, JsonResponseParser::RESPONSE_KEY_TIMESTAMP_IN_MILLIS);
	if (numberValue == nullptr)
	{
		return;
	}

	auto timestamp = numberValue->getLongValue();
	builder.withTimestampInMilliseconds(timestamp);
}