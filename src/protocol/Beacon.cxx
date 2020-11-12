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

#include "Beacon.h"
#include "ProtocolConstants.h"
#include "BeaconProtocolConstants.h"
#include "core/util/URLEncoding.h"
#include "core/util/InetAddressValidator.h"
#include "core/util/StringUtil.h"
#include "providers/DefaultPRNGenerator.h"

#include <random>

using namespace protocol;

Beacon::Beacon(
	std::shared_ptr<openkit::ILogger> logger,
	std::shared_ptr<core::caching::IBeaconCache> beaconCache,
	std::shared_ptr<core::configuration::IBeaconConfiguration> configuration,
	const char* clientIPAddress,
	std::shared_ptr<providers::ISessionIDProvider> sessionIDProvider,
	std::shared_ptr<providers::IThreadIDProvider> threadIDProvider,
	std::shared_ptr<providers::ITimingProvider> timingProvider
)
: Beacon(
	logger,
	beaconCache,
	configuration,
	clientIPAddress,
	sessionIDProvider,
	threadIDProvider,
	timingProvider,
	std::make_shared<providers::DefaultPRNGenerator>()
)
{
}

Beacon::Beacon(
	std::shared_ptr<openkit::ILogger> logger,
	std::shared_ptr<core::caching::IBeaconCache> beaconCache,
	std::shared_ptr<core::configuration::IBeaconConfiguration> configuration,
	const char* clientIPAddress,
	std::shared_ptr<providers::ISessionIDProvider> sessionIDProvider,
	std::shared_ptr<providers::IThreadIDProvider> threadIDProvider,
	std::shared_ptr<providers::ITimingProvider> timingProvider,
	std::shared_ptr<providers::IPRNGenerator> randomGenerator
)
	: mLogger(logger)
	, mBeaconCache(beaconCache)
	, mBeaconConfiguration(configuration)
	, mClientIPAddress(core::UTF8String())
	, mThreadIDProvider(threadIDProvider)
	, mTimingProvider(timingProvider)
	, mRandomGenerator(randomGenerator)
	, mDeviceID()
	, mSequenceNumber(0)
	, mID(0)
	, mBeaconId(sessionIDProvider->getNextSessionID())
	, mSessionNumber()
	, mSessionStartTime(timingProvider->provideTimestampInMilliseconds())
	, mImmutableBasicBeaconData()

{
	core::UTF8String internalClientIPAddress(clientIPAddress);
	if (clientIPAddress == nullptr)
	{
		// A client IP address, which is a nullptr, is valid.
		// The real IP address is determined on the server side.
	}
	else if (core::util::InetAddressValidator::IsValidIP(internalClientIPAddress))
	{
		mClientIPAddress = internalClientIPAddress;
	}
	else
	{
		if (logger->isWarningEnabled())
		{
			logger->warning("Beacon() - Client IP address validation failed: %s", internalClientIPAddress.getStringData().c_str());
		}
	}

	auto privacyConfig = configuration->getPrivacyConfiguration();
	auto openKitConfig = configuration->getOpenKitConfiguration();
	mDeviceID = privacyConfig->isDeviceIdSendingAllowed()
		? openKitConfig->getDeviceId()
		: mRandomGenerator->nextPositiveInt64();
	mSessionNumber = privacyConfig->isSessionNumberReportingAllowed()
		? mBeaconId
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
	addKeyValuePair(basicBeaconData, protocol::BEACON_KEY_APPLICATION_NAME, openKitConfig->getApplicationName());
	addKeyValuePairIfNotEmpty(basicBeaconData, protocol::BEACON_KEY_APPLICATION_VERSION, openKitConfig->getApplicationVersion());

	addKeyValuePair(basicBeaconData, protocol::BEACON_KEY_PLATFORM_TYPE, PLATFORM_TYPE_OPENKIT);
	addKeyValuePair(basicBeaconData, protocol::BEACON_KEY_AGENT_TECHNOLOGY_TYPE, AGENT_TECHNOLOGY_TYPE);

	// device/visitor ID, session number and IP address
	addKeyValuePair(basicBeaconData, protocol::BEACON_KEY_VISITOR_ID, getDeviceID());
	addKeyValuePair(basicBeaconData, protocol::BEACON_KEY_SESSION_NUMBER, getSessionNumber());
	if (!mClientIPAddress.empty())
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
	core::UTF8String eventData;
	addKeyValuePair(eventData, BEACON_KEY_EVENT_TYPE, static_cast<int32_t>(eventType));

	if (!eventName.empty())
	{
		addKeyValuePair(eventData, BEACON_KEY_NAME, truncate(eventName));
	}
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
		s.concatenate("&");
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
	if (!mBeaconConfiguration->getPrivacyConfiguration()->isActionReportingAllowed())
	{
		return;
	}

	if (!mBeaconConfiguration->getServerConfiguration()->isSendingDataAllowed())
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
	if (isCaptureEnabled())
	{
		mBeaconCache->addActionData(mBeaconId, timestamp, actionData);
	}
}

void Beacon::startSession()
{
	if (!isCaptureEnabled())
	{
		return;
	}

	core::UTF8String eventData = createBasicEventData(EventType::SESSION_START, nullptr);

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

	if (!mBeaconConfiguration->getServerConfiguration()->isSendingDataAllowed())
	{
		return;
	}

	core::UTF8String eventData = createBasicEventData(EventType::SESSION_END, nullptr);

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
	if (!mBeaconConfiguration->getPrivacyConfiguration()->isValueReportingAllowed())
	{
		return;
	}

	if (!mBeaconConfiguration->getServerConfiguration()->isSendingDataAllowed())
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
	if (!mBeaconConfiguration->getPrivacyConfiguration()->isValueReportingAllowed())
	{
		return;
	}

	if (!mBeaconConfiguration->getServerConfiguration()->isSendingDataAllowed())
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
	if (!mBeaconConfiguration->getPrivacyConfiguration()->isValueReportingAllowed())
	{
		return;
	}

	if (!mBeaconConfiguration->getServerConfiguration()->isSendingDataAllowed())
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
	if (!mBeaconConfiguration->getPrivacyConfiguration()->isEventReportingAllowed())
	{
		return;
	}

	if (!mBeaconConfiguration->getServerConfiguration()->isSendingDataAllowed())
	{
		return;
	}

	uint64_t eventTimestamp;
	core::UTF8String eventData = buildEvent(EventType::NAMED_EVENT, eventName, actionID, eventTimestamp);

	addEventData(eventTimestamp, eventData);
}

void Beacon::reportError(int32_t actionID, const core::UTF8String& errorName, int32_t errorCode, const core::UTF8String& reason)
{
	if (!mBeaconConfiguration->getPrivacyConfiguration()->isErrorReportingAllowed())
	{
		return;
	}

	if (!mBeaconConfiguration->getServerConfiguration()->isSendingErrorsAllowed())
	{
		return;
	}

	core::UTF8String eventData = createBasicEventData(EventType::FAILURE_ERROR, errorName);
	uint64_t timestamp = mTimingProvider->provideTimestampInMilliseconds();
	addKeyValuePair(eventData, BEACON_KEY_PARENT_ACTION_ID, actionID);
	addKeyValuePair(eventData, BEACON_KEY_START_SEQUENCE_NUMBER, createSequenceNumber());
	addKeyValuePair(eventData, BEACON_KEY_TIME_0, getTimeSinceSessionStartTime(timestamp));
	addKeyValuePair(eventData, BEACON_KEY_ERROR_CODE, errorCode);
	addKeyValuePairIfNotEmpty(eventData, BEACON_KEY_ERROR_REASON, reason);
	addKeyValuePair(eventData, BEACON_KEY_ERROR_TECHNOLOGY_TYPE, ERROR_TECHNOLOGY_TYPE);

	addEventData(timestamp, eventData);
}

void Beacon::reportCrash(const core::UTF8String& errorName, const core::UTF8String& reason, const core::UTF8String& stacktrace)
{
	if (!mBeaconConfiguration->getPrivacyConfiguration()->isCrashReportingAllowed())
	{
		return;
	}

	if (!mBeaconConfiguration->getServerConfiguration()->isSendingCrashesAllowed())
	{
		return;
	}

	core::UTF8String eventData = createBasicEventData(EventType::FAILURE_CRASH, errorName);

	auto timestamp = mTimingProvider->provideTimestampInMilliseconds();

	addKeyValuePair(eventData, BEACON_KEY_PARENT_ACTION_ID, 0);                                  // no parent action
	addKeyValuePair(eventData, BEACON_KEY_START_SEQUENCE_NUMBER, createSequenceNumber());
	addKeyValuePair(eventData, BEACON_KEY_TIME_0, getTimeSinceSessionStartTime(timestamp));
	addKeyValuePairIfNotEmpty(eventData, BEACON_KEY_ERROR_REASON, reason);
	addKeyValuePairIfNotEmpty(eventData, BEACON_KEY_ERROR_STACKTRACE, stacktrace);
	addKeyValuePair(eventData, BEACON_KEY_ERROR_TECHNOLOGY_TYPE, ERROR_TECHNOLOGY_TYPE);

	addEventData(timestamp, eventData);
}

void Beacon::addWebRequest(
	int32_t parentActionID,
	std::shared_ptr<core::objects::IWebRequestTracerInternals> webRequestTracer
)
{
	if (!mBeaconConfiguration->getPrivacyConfiguration()->isWebRequestTracingAllowed())
	{
		return;
	}

	if (!isCaptureEnabled())
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

	if (!isCaptureEnabled())
	{
		return;
	}

	core::UTF8String eventData = createBasicEventData(EventType::IDENTIFY_USER, userTag);

	auto timestamp = mTimingProvider->provideTimestampInMilliseconds();

	addKeyValuePair(eventData, BEACON_KEY_PARENT_ACTION_ID, 0);
	addKeyValuePair(eventData, BEACON_KEY_START_SEQUENCE_NUMBER, createSequenceNumber());
	addKeyValuePair(eventData, BEACON_KEY_TIME_0, getTimeSinceSessionStartTime(timestamp));

	addEventData(timestamp, eventData);
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
	core::UTF8String delimiter = core::UTF8String(BEACON_DATA_DELIMITER);

	core::UTF8String mutableBeaconData;

	mutableBeaconData.concatenate(delimiter);
	mutableBeaconData.concatenate(createTimestampData());
	mutableBeaconData.concatenate(delimiter);
	mutableBeaconData.concatenate(createMultiplicityData());

	return mutableBeaconData;
}

std::shared_ptr<protocol::IStatusResponse> Beacon::send(std::shared_ptr<providers::IHTTPClientProvider> clientProvider,
	const protocol::IAdditionalQueryParameters& additionalParameters)
{
	auto httpClient = clientProvider->createClient(mLogger, mBeaconConfiguration->getHTTPClientConfiguration());

	std::shared_ptr<protocol::IStatusResponse> response = nullptr;

	while (true)
	{
		// prefix for this chunk - must be built up newly, due to changing timestamps
		auto prefix = mImmutableBasicBeaconData;
		prefix.concatenate( getMutableBeaconData());

		auto chunk = mBeaconCache->getNextBeaconChunk(
			mBeaconId,
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
			mBeaconCache->resetChunkedData(mBeaconId);
			break;
		}
		else
		{
			// worked -> remove previously retrieved chunk from cache
			mBeaconCache->removeChunkedData(mBeaconId);
		}
	}

	return response;
}

void Beacon::addEventData(int64_t timestamp, const core::UTF8String& eventData)
{
	if (isCaptureEnabled())
	{
		mBeaconCache->addEventData(mBeaconId, timestamp, eventData);
	}
}

core::UTF8String Beacon::truncate(const core::UTF8String& string)
{
	if (string.getStringLength() > protocol::MAX_NAME_LEN)
	{
		return string.substring(0, protocol::MAX_NAME_LEN);
	}
	return string;
}

int64_t Beacon::getTimeSinceSessionStartTime(int64_t timestamp)
{
	return timestamp - mSessionStartTime;
}

bool Beacon::isEmpty() const
{
	return mBeaconCache->isEmpty(mBeaconId);
}

void Beacon::clearData()
{
	// remove all cached data for this Beacon from the cache
	mBeaconCache->deleteCacheEntry(mBeaconId);
}

int32_t Beacon::getSessionNumber() const
{
	return mSessionNumber;
}

int64_t Beacon::getDeviceID() const
{
	return mDeviceID;
}

const core::UTF8String& Beacon::getClientIPAddress() const
{
	return mClientIPAddress;
}

void Beacon::updateServerConfiguration(std::shared_ptr<core::configuration::IServerConfiguration> serverConfig)
{
	mBeaconConfiguration->updateServerConfiguration(serverConfig);
}

bool Beacon::isServerConfigurationSet()
{
	return mBeaconConfiguration->isServerConfigurationSet();
}

bool Beacon::isCaptureEnabled()
{
	return mBeaconConfiguration->getServerConfiguration()->isCaptureEnabled();
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