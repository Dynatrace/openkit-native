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

#include "Beacon.h"
#include "ProtocolConstants.h"
#include "BeaconProtocolConstants.h"
#include "core/util/URLEncoding.h"
#include "core/util/InetAddressValidator.h"
#include "core/util/StringUtil.h"
#include "core/util/ConnectionTypeUtil.h"
#include "providers/DefaultPRNGenerator.h"
#include "OpenKit/json/JsonObjectValue.h"
#include "OpenKit/json/JsonStringValue.h"
#include "OpenKit/json/JsonNumberValue.h"
#include "OpenKitVersion.h"
#include <random>
#include <core/objects/EventPayloadAttributes.h>
#include <regex>

using namespace protocol;

Beacon::Beacon(const protocol::IBeaconInitializer& initializer, const std::shared_ptr<core::configuration::IBeaconConfiguration> configuration)
	: mLogger(initializer.getLogger())
	, mBeaconCache(initializer.getBeaconCache())
	, mBeaconConfiguration(configuration)
	, mUseClientIpAddress(initializer.useClientIpAddress())
	, mClientIPAddress(initializer.getClientIpAddress())
	, mThreadIDProvider(initializer.getThreadIdProvider())
	, mTimingProvider(initializer.getTiminigProvider())
	, mRandomGenerator(initializer.getRandomNumberGenerator())
	, mDeviceID()
	, mSequenceNumber(0)
	, mID(0)
	, mBeaconKey(initializer.getSessionIdProvider()->getNextSessionID(), initializer.getSessionSequenceNumber())
	, mTrafficControlValue(initializer.getRandomNumberGenerator()->nextPercentageValue())
	, mSessionNumber(0)
	, mSessionSequenceNumber(initializer.getSessionSequenceNumber())
	, mSessionStartTime(initializer.getTiminigProvider()->provideTimestampInMilliseconds())
	, mImmutableBasicBeaconData()
	, mSupplementaryBasicData(initializer.getSupplementaryBasicData())

{
	if (mUseClientIpAddress && !core::util::InetAddressValidator::IsValidIP(mClientIPAddress))
	{
		if (mLogger->isWarningEnabled())
		{
			mLogger->warning("Beacon() - Client IP address validation failed: %s", mClientIPAddress.getStringData().c_str());
		}
		
		mUseClientIpAddress = false; // determined on server side
	}

	auto privacyConfig = configuration->getPrivacyConfiguration();
	auto openKitConfig = configuration->getOpenKitConfiguration();
	mDeviceID = privacyConfig->isDeviceIdSendingAllowed()
		? openKitConfig->getDeviceId()
		: mRandomGenerator->nextPositiveInt64();
	mSessionNumber = privacyConfig->isSessionNumberReportingAllowed()
		? mBeaconKey.getBeaconId()
		: 1;

	mImmutableBasicBeaconData = createImmutableBeaconData();
}

core::UTF8String Beacon::createImmutableBeaconData()
{
	core::UTF8String basicBeaconData;

	auto openKitConfig = mBeaconConfiguration->getOpenKitConfiguration();

	//version and application information
	addKeyValuePair(basicBeaconData, protocol::BEACON_KEY_PROTOCOL_VERSION, protocol::PROTOCOL_VERSION);
	addKeyValuePair(basicBeaconData, protocol::BEACON_KEY_OPENKIT_VERSION, protocol::OPENKIT_VERSION);
	addKeyValuePair(basicBeaconData, protocol::BEACON_KEY_APPLICATION_ID, openKitConfig->getApplicationId());
	addKeyValuePairIfNotEmpty(basicBeaconData, protocol::BEACON_KEY_APPLICATION_VERSION, openKitConfig->getApplicationVersion());

	addKeyValuePair(basicBeaconData, protocol::BEACON_KEY_PLATFORM_TYPE, PLATFORM_TYPE_OPENKIT);
	addKeyValuePair(basicBeaconData, protocol::BEACON_KEY_AGENT_TECHNOLOGY_TYPE, AGENT_TECHNOLOGY_TYPE);

	// device/visitor ID, session number and IP address
	addKeyValuePair(basicBeaconData, protocol::BEACON_KEY_VISITOR_ID, getDeviceID());
	addKeyValuePair(basicBeaconData, protocol::BEACON_KEY_SESSION_NUMBER, getSessionNumber());
	
	if (mUseClientIpAddress)
	{
		addKeyValuePair(basicBeaconData, protocol::BEACON_KEY_CLIENT_IP_ADDRESS, mClientIPAddress);
	}

	// platform information
	addKeyValuePairIfNotEmpty(basicBeaconData, BEACON_KEY_DEVICE_OS, openKitConfig->getOperatingSystem());
	addKeyValuePairIfNotEmpty(basicBeaconData, BEACON_KEY_DEVICE_MANUFACTURER, openKitConfig->getManufacturer());
	addKeyValuePairIfNotEmpty(basicBeaconData, BEACON_KEY_DEVICE_MODEL, openKitConfig->getModelId());

	auto privacyConfig = mBeaconConfiguration->getPrivacyConfiguration();
	addKeyValuePair(basicBeaconData, BEACON_KEY_DATA_COLLECTION_LEVEL, (int32_t)privacyConfig->getDataCollectionLevel());
	addKeyValuePair(basicBeaconData, BEACON_KEY_CRASH_REPORTING_LEVEL, (int32_t)privacyConfig->getCrashReportingLevel());

	return basicBeaconData;
}

core::UTF8String Beacon::createBasicEventData(protocol::EventType eventType, const core::UTF8String& eventName)
{
	core::UTF8String eventData = createBasicEventDataWithoutName(eventType);
	if (!eventName.empty())
	{
		addKeyValuePair(eventData, BEACON_KEY_NAME, truncate(eventName));
	}
	
	return eventData;
}

core::UTF8String Beacon::createBasicEventDataWithoutName(protocol::EventType eventType)
{
	core::UTF8String eventData;
	addKeyValuePair(eventData, BEACON_KEY_EVENT_TYPE, static_cast<int32_t>(eventType));
	addKeyValuePair(eventData, BEACON_KEY_THREAD_ID, mThreadIDProvider->getThreadID());

	return eventData;
}

core::UTF8String Beacon::createTimestampData()
{
	core::UTF8String timestampData;
	addKeyValuePair(timestampData, BEACON_KEY_TRANSMISSION_TIME, mTimingProvider->provideTimestampInMilliseconds());
	addKeyValuePair(timestampData, BEACON_KEY_SESSION_START_TIME, mSessionStartTime);

	return timestampData;
}

core::UTF8String Beacon::buildEvent(EventType eventType, const core::UTF8String& name, int32_t parentActionID, uint64_t& eventTimestamp)
{
	core::UTF8String eventData = createBasicEventData(eventType, name);

	eventTimestamp = mTimingProvider->provideTimestampInMilliseconds();
	addKeyValuePair(eventData, BEACON_KEY_PARENT_ACTION_ID, parentActionID);
	addKeyValuePair(eventData, BEACON_KEY_START_SEQUENCE_NUMBER, createSequenceNumber());
	addKeyValuePair(eventData, BEACON_KEY_TIME_0, getTimeSinceSessionStartTime(eventTimestamp));

	return eventData;
}

void Beacon::appendKey(core::UTF8String& s, const core::UTF8String& key)
{
	if (!s.empty())
	{
		s.concatenate(BEACON_DATA_DELIMITER);
	}

	s.concatenate(key);
	s.concatenate("=");
}

void Beacon::addKeyValuePair(core::UTF8String& s, const core::UTF8String& key, const core::UTF8String& value)
{
	appendKey(s, key);
	s.concatenate(core::util::URLEncoding::urlencode(value, { '_' }));
}

void Beacon::addKeyValuePairIfNotEmpty(core::UTF8String& s, const core::UTF8String& key, const core::UTF8String& value)
{
	if(value.getStringLength() > 0)
	{
		addKeyValuePair(s, key, value);
	}
}

void Beacon::addKeyValuePair(core::UTF8String& s, const core::UTF8String& key, int32_t value)
{
	addKeyValuePair(s, key, core::util::StringUtil::toInvariantString(value));
}

void Beacon::addKeyValuePair(core::UTF8String& s, const core::UTF8String& key, int64_t value)
{
	addKeyValuePair(s, key, core::util::StringUtil::toInvariantString(value));
}

void Beacon::addKeyValuePair(core::UTF8String& s, const core::UTF8String& key, double value)
{
	addKeyValuePair(s, key, core::util::StringUtil::toInvariantString(value));
}

int32_t Beacon::createSequenceNumber()
{
	return ++mSequenceNumber;
}

int64_t Beacon::getCurrentTimestamp() const
{
	return mTimingProvider->provideTimestampInMilliseconds();
}

int64_t Beacon::getSessionStartTime() const
{
	return mSessionStartTime;
}

int32_t Beacon::createID()
{
	return ++mID;
}

core::UTF8String Beacon::createTag(int32_t parentActionID, int32_t sequenceNumber)
{
	if (!mBeaconConfiguration->getPrivacyConfiguration()->isWebRequestTracingAllowed())
	{
		return core::UTF8String();
	}

	auto serverId = mBeaconConfiguration->getServerConfiguration()->getServerId();

	core::UTF8String webRequestTag(TAG_PREFIX);

	webRequestTag.concatenate("_");
	webRequestTag.concatenate(core::util::StringUtil::toInvariantString(PROTOCOL_VERSION));
	webRequestTag.concatenate("_");
	webRequestTag.concatenate(core::util::StringUtil::toInvariantString(serverId));
	webRequestTag.concatenate("_");
	webRequestTag.concatenate(core::util::StringUtil::toInvariantString(getDeviceID()));
	webRequestTag.concatenate("_");
	webRequestTag.concatenate(core::util::StringUtil::toInvariantString(mSessionNumber));
	if (getVisitStoreVersion() > 1)
	{
		webRequestTag.concatenate("-");
		webRequestTag.concatenate(core::util::StringUtil::toInvariantString(mSessionSequenceNumber));
	}
	webRequestTag.concatenate("_");
	webRequestTag.concatenate(mBeaconConfiguration->getOpenKitConfiguration()->getApplicationIdPercentEncoded());
	webRequestTag.concatenate("_");
	webRequestTag.concatenate(core::util::StringUtil::toInvariantString(parentActionID));
	webRequestTag.concatenate("_");
	webRequestTag.concatenate(core::util::StringUtil::toInvariantString(mThreadIDProvider->getThreadID()));
	webRequestTag.concatenate("_");
	webRequestTag.concatenate(core::util::StringUtil::toInvariantString(sequenceNumber));

	return webRequestTag;
}

void Beacon::addAction(std::shared_ptr<core::objects::IActionCommon> action)
{
	if (action == nullptr || action->getName().empty())
	{
		throw std::invalid_argument("action is nullptr or action->getName().empty() is true");
	}

	if (!mBeaconConfiguration->getPrivacyConfiguration()->isActionReportingAllowed())
	{
		return;
	}

	if (!isDataCapturingEnabled())
	{
		return;
	}

	core::UTF8String actionData = createBasicEventData(EventType::ACTION, action->getName());

	addKeyValuePair(actionData, BEACON_KEY_ACTION_ID, action->getID());
	addKeyValuePair(actionData, BEACON_KEY_PARENT_ACTION_ID, action->getParentID());
	addKeyValuePair(actionData, BEACON_KEY_START_SEQUENCE_NUMBER, action->getStartSequenceNumber());
	addKeyValuePair(actionData, BEACON_KEY_TIME_0, getTimeSinceSessionStartTime(action->getStartTime()));
	addKeyValuePair(actionData, BEACON_KEY_END_SEQUENCE_NUMBER, action->getEndSequenceNumber());
	addKeyValuePair(actionData, BEACON_KEY_TIME_1, action->getEndTime() - action->getStartTime());

	addActionData(action->getStartTime(), actionData);
}

void Beacon::addActionData(int64_t timestamp, const core::UTF8String& actionData)
{
	if (isDataCapturingEnabled())
	{
		mBeaconCache->addActionData(mBeaconKey, timestamp, actionData);
	}
}

void Beacon::startSession()
{
	if (!isDataCapturingEnabled())
	{
		return;
	}

	core::UTF8String eventData = createBasicEventDataWithoutName(EventType::SESSION_START);

	addKeyValuePair(eventData, BEACON_KEY_PARENT_ACTION_ID, 0);
	addKeyValuePair(eventData, BEACON_KEY_START_SEQUENCE_NUMBER, createSequenceNumber());
	addKeyValuePair(eventData, BEACON_KEY_TIME_0, int64_t(0));

	addEventData(mSessionStartTime, eventData);
}

void Beacon::endSession()
{
	if (!mBeaconConfiguration->getPrivacyConfiguration()->isSessionReportingAllowed())
	{
		return;
	}

	if (!isDataCapturingEnabled())
	{
		return;
	}

	core::UTF8String eventData = createBasicEventDataWithoutName(EventType::SESSION_END);

	auto endTime = getCurrentTimestamp();
	addKeyValuePair(eventData, BEACON_KEY_PARENT_ACTION_ID, 0);
	addKeyValuePair(eventData, BEACON_KEY_START_SEQUENCE_NUMBER, createSequenceNumber());
	addKeyValuePair(eventData, BEACON_KEY_TIME_0, getTimeSinceSessionStartTime(endTime));

	addEventData(endTime, eventData);
}

void Beacon::reportValue(int32_t actionID, const core::UTF8String& valueName, int32_t value)
{
	reportValue(actionID, valueName, static_cast<int64_t>(value));
}

void Beacon::reportValue(int32_t actionID, const core::UTF8String& valueName, int64_t value)
{
	if (valueName.empty())
	{
		throw std::invalid_argument("valueName.empty() is true");
	}

	if (!mBeaconConfiguration->getPrivacyConfiguration()->isValueReportingAllowed())
	{
		return;
	}

	if (!isDataCapturingEnabled())
	{
		return;
	}

	uint64_t eventTimestamp;
	core::UTF8String eventData = buildEvent(EventType::VALUE_INT, valueName, actionID, eventTimestamp);
	addKeyValuePair(eventData, BEACON_KEY_VALUE, value);

	addEventData(eventTimestamp, eventData);
}

void Beacon::reportValue(int32_t actionID, const core::UTF8String& valueName, double value)
{
	if (valueName.empty())
	{
		throw std::invalid_argument("valueName.empty() is true");
	}

	if (!mBeaconConfiguration->getPrivacyConfiguration()->isValueReportingAllowed())
	{
		return;
	}

	if (!isDataCapturingEnabled())
	{
		return;
	}

	uint64_t eventTimestamp;
	core::UTF8String eventData = buildEvent(EventType::VALUE_DOUBLE, valueName, actionID, eventTimestamp);

	addKeyValuePair(eventData, BEACON_KEY_VALUE, value);

	addEventData(eventTimestamp, eventData);
}

void Beacon::reportValue(int32_t actionID, const core::UTF8String& valueName, const core::UTF8String& value)
{
	if (valueName.empty())
	{
		throw std::invalid_argument("valueName.empty() is true");
	}

	if (!mBeaconConfiguration->getPrivacyConfiguration()->isValueReportingAllowed())
	{
		return;
	}

	if (!isDataCapturingEnabled())
	{
		return;
	}

	uint64_t eventTimestamp;
	core::UTF8String eventData = buildEvent(EventType::VALUE_STRING, valueName, actionID, eventTimestamp);

	addKeyValuePairIfNotEmpty(eventData, BEACON_KEY_VALUE, value);

	addEventData(eventTimestamp, eventData);
}

void Beacon::reportEvent(int32_t actionID, const core::UTF8String& eventName)
{
	if (eventName.empty())
	{
		throw std::invalid_argument("eventName.empty() is true");
	}

	if (!mBeaconConfiguration->getPrivacyConfiguration()->isEventReportingAllowed())
	{
		return;
	}

	if (!isDataCapturingEnabled())
	{
		return;
	}

	uint64_t eventTimestamp;
	core::UTF8String eventData = buildEvent(EventType::NAMED_EVENT, eventName, actionID, eventTimestamp);

	addEventData(eventTimestamp, eventData);
}

void Beacon::reportError(int32_t actionID, const core::UTF8String& errorName, int32_t errorCode)
{
	if (errorName.empty())
	{
		throw std::invalid_argument("errorName.empty() is true");
	}

	if (!mBeaconConfiguration->getPrivacyConfiguration()->isErrorReportingAllowed())
	{
		return;
	}

	if (!isErrorCapturingEnabled())
	{
		return;
	}

	core::UTF8String eventData = createBasicEventData(EventType::FAILURE_ERROR, errorName);
	uint64_t timestamp = mTimingProvider->provideTimestampInMilliseconds();
	addKeyValuePair(eventData, BEACON_KEY_PARENT_ACTION_ID, actionID);
	addKeyValuePair(eventData, BEACON_KEY_START_SEQUENCE_NUMBER, createSequenceNumber());
	addKeyValuePair(eventData, BEACON_KEY_TIME_0, getTimeSinceSessionStartTime(timestamp));
	addKeyValuePair(eventData, BEACON_KEY_ERROR_VALUE, errorCode);
	addKeyValuePair(eventData, BEACON_KEY_ERROR_TECHNOLOGY_TYPE, ERROR_TECHNOLOGY_TYPE);

	addEventData(timestamp, eventData);
}

void Beacon::reportError(
	int32_t actionID,
	const core::UTF8String& errorName,
	const core::UTF8String& causeName,
	const core::UTF8String& causeDescription,
	const core::UTF8String& causeStackTrace)
{
	if (errorName.empty())
	{
		throw std::invalid_argument("errorName.empty() is true");
	}

	if (!mBeaconConfiguration->getPrivacyConfiguration()->isErrorReportingAllowed())
	{
		return;
	}

	if (!isErrorCapturingEnabled())
	{
		return;
	}

	size_t maxStackTraceLength = protocol::MAX_STACKTRACE_LEN;

	// Truncating stacktrace at last line break
	if (causeStackTrace.getStringLength() > protocol::MAX_STACKTRACE_LEN)
	{
		int lastLineBreakIndex = causeStackTrace.getStringData().find_last_of('\n', protocol::MAX_STACKTRACE_LEN);
		if (lastLineBreakIndex != -1)
		{
			maxStackTraceLength = lastLineBreakIndex;
		}
	}

	core::UTF8String eventData = createBasicEventData(EventType::FAILURE_EXCEPTION, errorName);
	uint64_t timestamp = mTimingProvider->provideTimestampInMilliseconds();
	addKeyValuePair(eventData, BEACON_KEY_PARENT_ACTION_ID, actionID);
	addKeyValuePair(eventData, BEACON_KEY_START_SEQUENCE_NUMBER, createSequenceNumber());
	addKeyValuePair(eventData, BEACON_KEY_TIME_0, getTimeSinceSessionStartTime(timestamp));
	addKeyValuePairIfNotEmpty(eventData, BEACON_KEY_ERROR_VALUE, causeName);
	addKeyValuePairIfNotEmpty(eventData, BEACON_KEY_ERROR_REASON, truncate(causeDescription, protocol::MAX_REASON_LEN));
	addKeyValuePairIfNotEmpty(eventData, BEACON_KEY_ERROR_STACKTRACE, truncate(causeStackTrace, maxStackTraceLength));
	addKeyValuePair(eventData, BEACON_KEY_ERROR_TECHNOLOGY_TYPE, ERROR_TECHNOLOGY_TYPE);

	addEventData(timestamp, eventData);
}

void Beacon::reportCrash(const core::UTF8String& errorName, const core::UTF8String& reason, const core::UTF8String& stacktrace)
{
	if (errorName.empty())
	{
		throw std::invalid_argument("errorName.empty() is true");
	}

	if (!mBeaconConfiguration->getPrivacyConfiguration()->isCrashReportingAllowed())
	{
		return;
	}

	if (!isCrashCapturingEnabled())
	{
		return;
	}

	size_t maxStackTraceLength = protocol::MAX_STACKTRACE_LEN;

	// Truncating stacktrace at last line break
	if (stacktrace.getStringLength() > protocol::MAX_STACKTRACE_LEN)
	{
		int lastLineBreakIndex = stacktrace.getStringData().find_last_of('\n', protocol::MAX_STACKTRACE_LEN);
		if (lastLineBreakIndex != -1)
		{
			maxStackTraceLength = lastLineBreakIndex;
		}
	}

	core::UTF8String eventData = createBasicEventData(EventType::FAILURE_CRASH, errorName);

	auto timestamp = mTimingProvider->provideTimestampInMilliseconds();

	addKeyValuePair(eventData, BEACON_KEY_PARENT_ACTION_ID, 0);                                  // no parent action
	addKeyValuePair(eventData, BEACON_KEY_START_SEQUENCE_NUMBER, createSequenceNumber());
	addKeyValuePair(eventData, BEACON_KEY_TIME_0, getTimeSinceSessionStartTime(timestamp));
	addKeyValuePairIfNotEmpty(eventData, BEACON_KEY_ERROR_REASON, truncate(reason, protocol::MAX_REASON_LEN));
	addKeyValuePairIfNotEmpty(eventData, BEACON_KEY_ERROR_STACKTRACE, truncate(stacktrace, maxStackTraceLength));
	addKeyValuePair(eventData, BEACON_KEY_ERROR_TECHNOLOGY_TYPE, ERROR_TECHNOLOGY_TYPE);

	addEventData(timestamp, eventData);
}

void Beacon::addWebRequest(
	int32_t parentActionID,
	std::shared_ptr<core::objects::IWebRequestTracerInternals> webRequestTracer
)
{
	if (webRequestTracer == nullptr || webRequestTracer->getURL().empty())
	{
		throw std::invalid_argument("webRequestTracer == nullptr || webRequestTracer->getURL().empty() is true");
	}

	if (!mBeaconConfiguration->getPrivacyConfiguration()->isWebRequestTracingAllowed())
	{
		return;
	}

	if (!isDataCapturingEnabled())
	{
		return;
	}

	core::UTF8String eventData = createBasicEventData(EventType::WEBREQUEST, webRequestTracer->getURL());

	addKeyValuePair(eventData, BEACON_KEY_PARENT_ACTION_ID, parentActionID);
	addKeyValuePair(eventData, BEACON_KEY_START_SEQUENCE_NUMBER, webRequestTracer->getStartSequenceNo());
	addKeyValuePair(eventData, BEACON_KEY_TIME_0, getTimeSinceSessionStartTime(webRequestTracer->getStartTime()));
	addKeyValuePair(eventData, BEACON_KEY_END_SEQUENCE_NUMBER, webRequestTracer->getEndSequenceNo());
	addKeyValuePair(eventData, BEACON_KEY_TIME_1, webRequestTracer->getEndTime() - webRequestTracer->getStartTime());

	int32_t bytesSent = webRequestTracer->getBytesSent();
	if (bytesSent > -1)
	{
		addKeyValuePair(eventData, BEACON_KEY_WEBREQUEST_BYTES_SENT, bytesSent);
	}

	int32_t bytesReceived = webRequestTracer->getBytesReceived();
	if (bytesReceived > -1)
	{
		addKeyValuePair(eventData, BEACON_KEY_WEBREQUEST_BYTES_RECEIVED, bytesReceived);
	}

	int32_t responseCode = webRequestTracer->getResponseCode();
	if (responseCode > -1)
	{
		addKeyValuePair(eventData, BEACON_KEY_WEBREQUEST_RESPONSE_CODE, responseCode);
	}

	addEventData(webRequestTracer->getStartTime(), eventData);
}

void Beacon::identifyUser(const core::UTF8String& userTag)
{
	if (!mBeaconConfiguration->getPrivacyConfiguration()->isUserIdentificationAllowed())
	{
		return;
	}

	if (!isDataCapturingEnabled())
	{
		return;
	}

	core::UTF8String eventData = userTag.empty()
		? createBasicEventDataWithoutName(EventType::IDENTIFY_USER)
		: createBasicEventData(EventType::IDENTIFY_USER, userTag);

	auto timestamp = mTimingProvider->provideTimestampInMilliseconds();

	addKeyValuePair(eventData, BEACON_KEY_PARENT_ACTION_ID, 0);
	addKeyValuePair(eventData, BEACON_KEY_START_SEQUENCE_NUMBER, createSequenceNumber());
	addKeyValuePair(eventData, BEACON_KEY_TIME_0, getTimeSinceSessionStartTime(timestamp));

	addEventData(timestamp, eventData);
}

void Beacon::sendBizEvent(const core::UTF8String& type, const openkit::json::JsonObjectValue::JsonObjectMapPtr attributes)
{
	if (type.empty())
	{
		throw std::invalid_argument("type.empty() is true");
	}

	if (!mBeaconConfiguration->getPrivacyConfiguration()->isEventReportingAllowed())
	{
		return;
	}

	if (!isDataCapturingEnabled())
	{
		return;
	}

	auto builder = std::make_shared<core::objects::EventPayloadBuilder>(attributes, mLogger);
	builder->addNonOverridableAttribute("event.type", openkit::json::JsonStringValue::fromString(type.getStringData()));
	
	core::UTF8String inputPayload(builder->build());
	
	builder->cleanReservedInternalAttributes()
		.addNonOverridableAttribute("dt.rum.custom_attributes_size", openkit::json::JsonNumberValue::fromLong(inputPayload.getStringLength()));
	
	generateEventPayload(builder);
	builder->addNonOverridableAttribute(core::objects::EVENT_KIND, openkit::json::JsonStringValue::fromString(core::objects::EVENT_KIND_BIZ));

	if (attributes == nullptr || attributes->find("event.name") == attributes->end())
	{
		builder->addNonOverridableAttribute("event.name", openkit::json::JsonStringValue::fromString(type.getStringData()));
	}

	sendEventPayload(*builder);
}

void Beacon::sendEvent(const core::UTF8String& name, const openkit::json::JsonObjectValue::JsonObjectMapPtr attributes)
{
	if (name.empty())
	{
		throw std::invalid_argument("name.empty() is true");
	}

	if (!mBeaconConfiguration->getPrivacyConfiguration()->isEventReportingAllowed())
	{
		return;
	}

	if (!isDataCapturingEnabled())
	{
		return;
	}

	auto builder = std::make_shared<core::objects::EventPayloadBuilder>(attributes, mLogger);
	generateEventPayload(builder);
	builder->addNonOverridableAttribute("event.name", openkit::json::JsonStringValue::fromString(name.getStringData()))
		.addOverridableAttribute(core::objects::EVENT_KIND, openkit::json::JsonStringValue::fromString(core::objects::EVENT_KIND_RUM));

	sendEventPayload(*builder);
}

void Beacon::sendEventPayload(core::objects::EventPayloadBuilder& builder)
{
	auto jsonPayload = builder.build();

	if (jsonPayload.length() > EVENT_PAYLOAD_BYTES_LENGTH) {
		throw std::invalid_argument("Event payload is exceeding " + std::to_string(EVENT_PAYLOAD_BYTES_LENGTH) + " bytes!");
	}

	auto timestamp = mTimingProvider->provideTimestampInMilliseconds();

	core::UTF8String eventData;
	addKeyValuePair(eventData, BEACON_KEY_EVENT_TYPE, static_cast<int32_t>(EventType::EVENT));
	addKeyValuePair(eventData, BEACON_KEY_EVENT_PAYLOAD, jsonPayload);

	addEventData(timestamp, eventData);
}

void Beacon::generateEventPayload(std::shared_ptr<core::objects::EventPayloadBuilder> builder)
{
	auto openKitConfig = mBeaconConfiguration->getOpenKitConfiguration();

	builder->addOverridableAttribute(core::objects::TIMESTAMP, openkit::json::JsonNumberValue::fromLong(mTimingProvider->provideTimestampInNanoseconds()));
	builder->addNonOverridableAttribute(EVENT_PAYLOAD_APPLICATION_ID, openkit::json::JsonStringValue::fromString(openKitConfig->getApplicationId().getStringData()));
	builder->addNonOverridableAttribute(EVENT_PAYLOAD_INSTANCE_ID, openkit::json::JsonNumberValue::fromLong(getDeviceID()));
	builder->addNonOverridableAttribute(EVENT_PAYLOAD_SESSION_ID, openkit::json::JsonNumberValue::fromLong(getSessionNumber()));
	builder->addNonOverridableAttribute(EVENT_SCHEMA_VERSION, openkit::json::JsonStringValue::fromString("1.0"));
	builder->addOverridableAttribute(core::objects::APP_VERSION, openkit::json::JsonStringValue::fromString(openKitConfig->getApplicationVersion().getStringData()));
	builder->addOverridableAttribute(core::objects::OS_NAME, openkit::json::JsonStringValue::fromString(openKitConfig->getOperatingSystem().getStringData()));
	builder->addOverridableAttribute(core::objects::DEVICE_MANUFACTURER, openkit::json::JsonStringValue::fromString(openKitConfig->getManufacturer().getStringData()));
	builder->addOverridableAttribute(core::objects::DEVICE_MODEL_IDENTIFIER, openkit::json::JsonStringValue::fromString(openKitConfig->getModelId().getStringData()));
	builder->addOverridableAttribute(core::objects::EVENT_PROVIDER, openkit::json::JsonStringValue::fromString(openKitConfig->getApplicationId().getStringData()));
}

core::UTF8String Beacon::createMultiplicityData()
{
	auto multiplicity = mBeaconConfiguration->getServerConfiguration()->getMultiplicity();
	core::UTF8String multiplicityData;
	addKeyValuePair(multiplicityData, BEACON_KEY_MULTIPLICITY, multiplicity);
	return multiplicityData;
}

core::UTF8String Beacon::getMutableBeaconData()
{
	const auto delimiter = core::UTF8String(BEACON_DATA_DELIMITER);

	core::UTF8String mutableBeaconData;

	const auto visitStoreVersion = getVisitStoreVersion();
	addKeyValuePair(mutableBeaconData, BEACON_KEY_VISIT_STORE_VERSION, visitStoreVersion);

	if(visitStoreVersion > 1) {
		addKeyValuePair(mutableBeaconData, BEACON_KEY_SESSION_SEQUENCE, mSessionSequenceNumber);
	}

	mutableBeaconData.concatenate(delimiter);
	mutableBeaconData.concatenate(createTimestampData());
	mutableBeaconData.concatenate(delimiter);
	mutableBeaconData.concatenate(createMultiplicityData());

	if (mSupplementaryBasicData->isNetworkTechnologyAvailable())
	{
		addKeyValuePair(mutableBeaconData, BEACON_KEY_NETWORK_TECHNOLOGY, mSupplementaryBasicData->getNetworkTechnology());
	}

	if (mSupplementaryBasicData->isCarrierAvailable())
	{
		addKeyValuePair(mutableBeaconData, BEACON_KEY_CARRIER, truncate(mSupplementaryBasicData->getCarrier()));
	}

	if (mSupplementaryBasicData->isConnectionTypeAvailable() && mSupplementaryBasicData->getConnectionType() != openkit::ConnectionType::UNSET)
	{
		addKeyValuePair(mutableBeaconData, BEACON_KEY_CONNECTION_TYPE, core::util::ConnectionTypeToString(mSupplementaryBasicData->getConnectionType()));
	}

	return mutableBeaconData;
}

std::shared_ptr<protocol::IStatusResponse> Beacon::send(std::shared_ptr<providers::IHTTPClientProvider> clientProvider,
	const protocol::IAdditionalQueryParameters& additionalParameters)
{
	auto httpClient = clientProvider->createClient(mBeaconConfiguration->getHTTPClientConfiguration());

	std::shared_ptr<protocol::IStatusResponse> response = nullptr;

	mBeaconCache->prepareDataForSending(mBeaconKey);
	while (mBeaconCache->hasDataForSending(mBeaconKey))
	{
		// prefix for this chunk - must be built up newly, due to changing timestamps
		auto prefix = mImmutableBasicBeaconData;
		prefix.concatenate( BEACON_DATA_DELIMITER );
		prefix.concatenate( getMutableBeaconData());

		auto chunk = mBeaconCache->getNextBeaconChunk(
			mBeaconKey,
			prefix,
			mBeaconConfiguration->getServerConfiguration()->getBeaconSizeInBytes() - 1024,
			BEACON_DATA_DELIMITER
		);
		if (chunk == nullptr || chunk.empty())
		{
			return response;
		}

		// send the request
		response = httpClient->sendBeaconRequest(mClientIPAddress, chunk, additionalParameters);
		if (response == nullptr || response->isErroneousResponse())
		{
			// error happened - but don't know what exactly
			// reset the previously retrieved chunk (restore it in internal cache) & retry another time
			mBeaconCache->resetChunkedData(mBeaconKey);
			break;
		}
		else
		{
			// worked -> remove previously retrieved chunk from cache
			mBeaconCache->removeChunkedData(mBeaconKey);
		}
	}

	return response;
}

void Beacon::addEventData(int64_t timestamp, const core::UTF8String& eventData)
{
	if (isDataCapturingEnabled())
	{
		mBeaconCache->addEventData(mBeaconKey, timestamp, eventData);
	}
}

core::UTF8String Beacon::truncate(const core::UTF8String& string, size_t length)
{
	if (string.getStringLength() > length)
	{
		return string.substring(0, length);
	}
	return string;
}

core::UTF8String Beacon::truncate(const core::UTF8String& string)
{
	return truncate(string, protocol::MAX_NAME_LEN);
}

int64_t Beacon::getTimeSinceSessionStartTime(int64_t timestamp)
{
	return timestamp - mSessionStartTime;
}

bool Beacon::isEmpty() const
{
	return mBeaconCache->isEmpty(mBeaconKey);
}

void Beacon::clearData()
{
	// remove all cached data for this Beacon from the cache
	mBeaconCache->deleteCacheEntry(mBeaconKey);
}

int32_t Beacon::getSessionNumber() const
{
	return mSessionNumber;
}

int32_t Beacon::getSessionSequenceNumber() const
{
	return mSessionSequenceNumber;
}

int64_t Beacon::getDeviceID() const
{
	return mDeviceID;
}

bool Beacon::useClientIPAddress() const
{
	return mUseClientIpAddress;
}

const core::UTF8String& Beacon::getClientIPAddress() const
{
	return mClientIPAddress;
}

void Beacon::initializeServerConfiguration(std::shared_ptr<core::configuration::IServerConfiguration> serverConfiguration)
{
	mBeaconConfiguration->initializeServerConfiguration(serverConfiguration);
}

void Beacon::updateServerConfiguration(std::shared_ptr<core::configuration::IServerConfiguration> serverConfig)
{
	mBeaconConfiguration->updateServerConfiguration(serverConfig);
}

bool Beacon::isServerConfigurationSet()
{
	return mBeaconConfiguration->isServerConfigurationSet();
}

bool Beacon::isActionReportingAllowedByPrivacySettings()
{
	return mBeaconConfiguration->getPrivacyConfiguration()->isActionReportingAllowed();
}

bool Beacon::isDataCapturingEnabled()
{
	auto serverConfiguration = mBeaconConfiguration->getServerConfiguration();

	return serverConfiguration->isSendingDataAllowed()
		&& mTrafficControlValue < serverConfiguration->getTrafficControlPercentage();
}

bool Beacon::isErrorCapturingEnabled()
{
	auto serverConfiguration = mBeaconConfiguration->getServerConfiguration();

	return serverConfiguration->isSendingErrorsAllowed()
		&& mTrafficControlValue < serverConfiguration->getTrafficControlPercentage();
}

bool Beacon::isCrashCapturingEnabled()
{
	auto serverConfiguration = mBeaconConfiguration->getServerConfiguration();

	return serverConfiguration->isSendingCrashesAllowed()
		&& mTrafficControlValue < serverConfiguration->getTrafficControlPercentage();
}

void Beacon::enableCapture()
{
	return mBeaconConfiguration->enableCapture();
}

void Beacon::disableCapture()
{
	return mBeaconConfiguration->disableCapture();
}

void Beacon::setServerConfigurationUpdateCallback(core::configuration::ServerConfigurationUpdateCallback serverConfigurationUpdateCallback)
{
	mBeaconConfiguration->setServerConfigurationUpdateCallback(serverConfigurationUpdateCallback);
}

int32_t Beacon::getVisitStoreVersion()
{
	return mBeaconConfiguration->getServerConfiguration()->getVisitStoreVersion();
}