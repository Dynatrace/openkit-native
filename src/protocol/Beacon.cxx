/**
* Copyright 2018-2019 Dynatrace LLC
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
#include "OpenKit/DataCollectionLevel.h"
#include "OpenKit/CrashReportingLevel.h"
#include "ProtocolConstants.h"
#include "BeaconProtocolConstants.h"
#include "core/util/URLEncoding.h"
#include "core/util/InetAddressValidator.h"
#include "providers/DefaultPRNGenerator.h"

#include <random>
#include <sstream>
#include <iomanip>
#include <locale>
#include <type_traits>

using namespace protocol;

namespace details
{
	constexpr std::streamsize DEFAULT_FP_PRECISION = 6;

	template<typename Type>
	static std::string locale_independent_to_string(const Type& value)
	{
		std::ostringstream oss;
		oss.imbue(std::locale::classic());
		if (std::is_floating_point<Type>::value) {
			oss << std::fixed << std::setprecision(DEFAULT_FP_PRECISION);
		}
		oss << value;

		return oss.str();
	}
}

Beacon::Beacon(std::shared_ptr<openkit::ILogger> logger, std::shared_ptr<caching::IBeaconCache> beaconCache, std::shared_ptr<configuration::Configuration> configuration, const char* clientIPAddress, std::shared_ptr<providers::IThreadIDProvider> threadIDProvider, std::shared_ptr<providers::ITimingProvider> timingProvider)
	: Beacon(logger, beaconCache, configuration, clientIPAddress, threadIDProvider, timingProvider, std::make_shared<providers::DefaultPRNGenerator>())
{
}

Beacon::Beacon(std::shared_ptr<openkit::ILogger> logger, std::shared_ptr<caching::IBeaconCache> beaconCache, std::shared_ptr<configuration::Configuration> configuration, const char* clientIPAddress, std::shared_ptr<providers::IThreadIDProvider> threadIDProvider, std::shared_ptr<providers::ITimingProvider> timingProvider, std::shared_ptr<providers::IPRNGenerator> randomGenerator)
	: mLogger(logger)
	, mConfiguration(configuration)
	, mClientIPAddress(core::UTF8String(""))
	, mTimingProvider(timingProvider)
	, mThreadIDProvider(threadIDProvider)
	, mSequenceNumber(0)
	, mID(0)
	, mSessionNumber()
	, mSessionStartTime(timingProvider->provideTimestampInMilliseconds())
	, mImmutableBasicBeaconData()
	, mBeaconCache(beaconCache)
	, mHTTPClientConfiguration(configuration->getHTTPClientConfiguration())
	, mBeaconConfiguration(configuration->getBeaconConfiguration())
	, mDeviceID()
	, mRandomGenerator(randomGenerator)
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

	if (mBeaconConfiguration->getDataCollectionLevel() == openkit::DataCollectionLevel::USER_BEHAVIOR)
	{
		mDeviceID = mConfiguration->getDeviceID();
		mSessionNumber = configuration->createSessionNumber();
	}
	else
	{
		mDeviceID = mRandomGenerator->nextInt64(std::numeric_limits<int64_t>::max());
		mSessionNumber = 1;
	}

	mImmutableBasicBeaconData = createImmutableBeaconData();
}

core::UTF8String Beacon::createImmutableBeaconData()
{
	core::UTF8String basicBeaconData;

	//version and application information
	addKeyValuePair(basicBeaconData, protocol::BEACON_KEY_PROTOCOL_VERSION, protocol::PROTOCOL_VERSION);
	addKeyValuePair(basicBeaconData, protocol::BEACON_KEY_OPENKIT_VERSION, protocol::OPENKIT_VERSION);
	addKeyValuePair(basicBeaconData, protocol::BEACON_KEY_APPLICATION_ID, mConfiguration->getApplicationID());
	addKeyValuePair(basicBeaconData, protocol::BEACON_KEY_APPLICATION_NAME, mConfiguration->getApplicationName());
	auto applicationVersion = mConfiguration->getApplicationVersion();
	if (!applicationVersion.empty())
	{
		addKeyValuePair(basicBeaconData, protocol::BEACON_KEY_APPLICATION_VERSION, applicationVersion);
	}
	addKeyValuePair(basicBeaconData, protocol::BEACON_KEY_PLATFORM_TYPE, PLATFORM_TYPE_OPENKIT);
	addKeyValuePair(basicBeaconData, protocol::BEACON_KEY_AGENT_TECHNOLOGY_TYPE, AGENT_TECHNOLOGY_TYPE);

	// device/visitor ID, session number and IP address
	addKeyValuePair(basicBeaconData, protocol::BEACON_KEY_VISITOR_ID, getDeviceID());
	addKeyValuePair(basicBeaconData, protocol::BEACON_KEY_SESSION_NUMBER, getSessionNumber());
	addKeyValuePair(basicBeaconData, protocol::BEACON_KEY_CLIENT_IP_ADDRESS, mClientIPAddress);

	// platform information
	auto deviceOS = mConfiguration->getDevice()->getOperatingSystem();
	if (!deviceOS.empty())
	{
		addKeyValuePair(basicBeaconData, BEACON_KEY_DEVICE_OS, deviceOS);
	}
	auto deviceManufacturer = mConfiguration->getDevice()->getManufacturer();
	if (!deviceManufacturer.empty())
	{
		addKeyValuePair(basicBeaconData, BEACON_KEY_DEVICE_MANUFACTURER, deviceManufacturer);
	}
	auto deviceModel = mConfiguration->getDevice()->getModelID();
	if (!deviceModel.empty())
	{
		addKeyValuePair(basicBeaconData, BEACON_KEY_DEVICE_MODEL, deviceModel);
	}

	auto beaconConfiguration = mConfiguration->getBeaconConfiguration();
	addKeyValuePair(basicBeaconData, BEACON_KEY_DATA_COLLECTION_LEVEL, (int32_t)beaconConfiguration->getDataCollectionLevel());
	addKeyValuePair(basicBeaconData, BEACON_KEY_CRASH_REPORTING_LEVEL, (int32_t)beaconConfiguration->getCrashReportingLevel());

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
	addKeyValuePair(timestampData, BEACON_KEY_SESSION_START_TIME, mTimingProvider->convertToClusterTime(mSessionStartTime));
	addKeyValuePair(timestampData, BEACON_KEY_TIMESYNC_TIME, mTimingProvider->convertToClusterTime(mSessionStartTime));
	if (!mTimingProvider->isTimeSyncSupported())
	{
		addKeyValuePair(timestampData, BEACON_KEY_TRANSMISSION_TIME, mTimingProvider->provideTimestampInMilliseconds());
	}
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

void Beacon::addKeyValuePair(core::UTF8String& s, const core::UTF8String& key, int32_t value)
{
	auto strValue = details::locale_independent_to_string(value);
	addKeyValuePair(s, key, strValue);
}

void Beacon::addKeyValuePair(core::UTF8String& s, const core::UTF8String& key, int64_t value)
{
	auto strValue = details::locale_independent_to_string(value);
	addKeyValuePair(s, key, strValue);
}

void Beacon::addKeyValuePair(core::UTF8String& s, const core::UTF8String& key, double value)
{
	auto strValue = details::locale_independent_to_string(value);
	addKeyValuePair(s, key, strValue);
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
	if (std::atomic_load(&mBeaconConfiguration)->getDataCollectionLevel() == openkit::DataCollectionLevel::OFF)
	{
		return core::UTF8String("");
	}

	std::ostringstream oss;
	oss.imbue(std::locale::classic());

	oss << TAG_PREFIX << "_"
		<< PROTOCOL_VERSION << "_"
		<< mHTTPClientConfiguration->getServerID() << "_"
		<< getDeviceID() << "_"
		<< mSessionNumber << "_"
		<< mConfiguration->getApplicationIDPercentEncoded().getStringData() << "_"
		<< parentActionID << "_"
		<< mThreadIDProvider->getThreadID() << "_"
		<< sequenceNumber;

	return core::UTF8String(oss.str());
}

void Beacon::addAction(std::shared_ptr<core::Action> action)
{
	if (std::atomic_load(&mBeaconConfiguration)->getDataCollectionLevel() == openkit::DataCollectionLevel::OFF)
	{
		return;
	}

	core::UTF8String actionData = createBasicEventData(EventType::ACTION, action->getName());

	addKeyValuePair(actionData, BEACON_KEY_ACTION_ID, action->getID());
	addKeyValuePair(actionData, BEACON_KEY_PARENT_ACTION_ID, action->getParentID());
	addKeyValuePair(actionData, BEACON_KEY_START_SEQUENCE_NUMBER, action->getStartSequenceNo());
	addKeyValuePair(actionData, BEACON_KEY_TIME_0, getTimeSinceSessionStartTime(action->getStartTime()));
	addKeyValuePair(actionData, BEACON_KEY_END_SEQUENCE_NUMBER, action->getEndSequenceNo());
	addKeyValuePair(actionData, BEACON_KEY_TIME_1, action->getEndTime() - action->getStartTime());

	addActionData(action->getStartTime(), actionData);
}

void Beacon::addAction(std::shared_ptr<core::RootAction> action)
{
	if (std::atomic_load(&mBeaconConfiguration)->getDataCollectionLevel() == openkit::DataCollectionLevel::OFF)
	{
		return;
	}

	core::UTF8String actionData = createBasicEventData(EventType::ACTION, action->getName());

	addKeyValuePair(actionData, BEACON_KEY_ACTION_ID, action->getID());
	addKeyValuePair(actionData, BEACON_KEY_PARENT_ACTION_ID, 0);
	addKeyValuePair(actionData, BEACON_KEY_START_SEQUENCE_NUMBER, action->getStartSequenceNo());
	addKeyValuePair(actionData, BEACON_KEY_TIME_0, getTimeSinceSessionStartTime(action->getStartTime()));
	addKeyValuePair(actionData, BEACON_KEY_END_SEQUENCE_NUMBER, action->getEndSequenceNo());
	addKeyValuePair(actionData, BEACON_KEY_TIME_1, action->getEndTime() - action->getStartTime());

	addActionData(action->getStartTime(), actionData);
}

void Beacon::addActionData(int64_t timestamp, const core::UTF8String& actionData)
{
	if (mConfiguration->isCapture())
	{
		mBeaconCache->addActionData(mSessionNumber, timestamp, actionData);
	}
}

void Beacon::startSession()
{
	core::UTF8String eventData = createBasicEventData(EventType::SESSION_START, nullptr);

	addKeyValuePair(eventData, BEACON_KEY_PARENT_ACTION_ID, 0);
	addKeyValuePair(eventData, BEACON_KEY_START_SEQUENCE_NUMBER, createSequenceNumber());
	addKeyValuePair(eventData, BEACON_KEY_TIME_0, int64_t(0));

	addEventData(mSessionStartTime, eventData);
}

void Beacon::endSession(std::shared_ptr<core::Session> session)
{
	if (std::atomic_load(&mBeaconConfiguration)->getDataCollectionLevel() == openkit::DataCollectionLevel::OFF)
	{
		return;
	}

	core::UTF8String eventData = createBasicEventData(EventType::SESSION_END, nullptr);

	addKeyValuePair(eventData, BEACON_KEY_PARENT_ACTION_ID, 0);
	addKeyValuePair(eventData, BEACON_KEY_START_SEQUENCE_NUMBER, createSequenceNumber());
	addKeyValuePair(eventData, BEACON_KEY_TIME_0, getTimeSinceSessionStartTime(session->getEndTime()));

	addEventData(session->getEndTime(), eventData);
}

void Beacon::reportValue(int32_t actionID, const core::UTF8String& valueName, int32_t value)
{
	if (std::atomic_load(&mBeaconConfiguration)->getDataCollectionLevel() != openkit::DataCollectionLevel::USER_BEHAVIOR)
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
	if (std::atomic_load(&mBeaconConfiguration)->getDataCollectionLevel() != openkit::DataCollectionLevel::USER_BEHAVIOR)
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
	if (std::atomic_load(&mBeaconConfiguration)->getDataCollectionLevel() != openkit::DataCollectionLevel::USER_BEHAVIOR)
	{
		return;
	}

	uint64_t eventTimestamp;
	core::UTF8String eventData = buildEvent(EventType::VALUE_STRING, valueName, actionID, eventTimestamp);

	addKeyValuePair(eventData, BEACON_KEY_VALUE, value);

	addEventData(eventTimestamp, eventData);
}

void Beacon::reportEvent(int32_t actionID, const core::UTF8String& eventName)
{
	if (std::atomic_load(&mBeaconConfiguration)->getDataCollectionLevel() != openkit::DataCollectionLevel::USER_BEHAVIOR)
	{
		return;
	}

	uint64_t eventTimestamp;
	core::UTF8String eventData = buildEvent(EventType::NAMED_EVENT, eventName, actionID, eventTimestamp);

	addEventData(eventTimestamp, eventData);
}

void Beacon::reportError(int32_t actionID, const core::UTF8String& errorName, int32_t errorCode, const core::UTF8String& reason)
{
	if (!mConfiguration->isCaptureErrors())
	{
		return;
	}

	if (std::atomic_load(&mBeaconConfiguration)->getDataCollectionLevel() == openkit::DataCollectionLevel::OFF)
	{
		return;
	}

	core::UTF8String eventData = createBasicEventData(EventType::FAILURE_ERROR, errorName);
	uint64_t timestamp = mTimingProvider->provideTimestampInMilliseconds();
	addKeyValuePair(eventData, BEACON_KEY_PARENT_ACTION_ID, actionID);
	addKeyValuePair(eventData, BEACON_KEY_START_SEQUENCE_NUMBER, createSequenceNumber());
	addKeyValuePair(eventData, BEACON_KEY_TIME_0, getTimeSinceSessionStartTime(timestamp));
	addKeyValuePair(eventData, BEACON_KEY_ERROR_CODE, errorCode);
	if (reason != nullptr)
	{
		addKeyValuePair(eventData, BEACON_KEY_ERROR_REASON, reason);
	}
	addKeyValuePair(eventData, BEACON_KEY_ERROR_TECHNOLOGY_TYPE, ERROR_TECHNOLOGY_TYPE);

	addEventData(timestamp, eventData);
}

void Beacon::reportCrash(const core::UTF8String& errorName, const core::UTF8String& reason, const core::UTF8String& stacktrace)
{
	if (!mConfiguration->isCaptureCrashes())
	{
		return;
	}

	if (std::atomic_load(&mBeaconConfiguration)->getCrashReportingLevel() != openkit::CrashReportingLevel::OPT_IN_CRASHES)
	{
		return;
	}

	core::UTF8String eventData = createBasicEventData(EventType::FAILURE_CRASH, errorName);

	auto timestamp = mTimingProvider->provideTimestampInMilliseconds();

	addKeyValuePair(eventData, BEACON_KEY_PARENT_ACTION_ID, 0);                                  // no parent action
	addKeyValuePair(eventData, BEACON_KEY_START_SEQUENCE_NUMBER, createSequenceNumber());
	addKeyValuePair(eventData, BEACON_KEY_TIME_0, getTimeSinceSessionStartTime(timestamp));
	addKeyValuePair(eventData, BEACON_KEY_ERROR_REASON, reason);
	addKeyValuePair(eventData, BEACON_KEY_ERROR_STACKTRACE, stacktrace);
	addKeyValuePair(eventData, BEACON_KEY_ERROR_TECHNOLOGY_TYPE, ERROR_TECHNOLOGY_TYPE);

	addEventData(timestamp, eventData);
}

void Beacon::addWebRequest(int32_t parentActionID, std::shared_ptr<core::WebRequestTracerBase> webRequestTracer)
{
	if (std::atomic_load(&mBeaconConfiguration)->getDataCollectionLevel() == openkit::DataCollectionLevel::OFF)
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
	if (std::atomic_load(&mBeaconConfiguration)->getDataCollectionLevel() != openkit::DataCollectionLevel::USER_BEHAVIOR)
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
	core::UTF8String multiplicityData;
	addKeyValuePair(multiplicityData, BEACON_KEY_MULTIPLICITY, mBeaconConfiguration->getMultiplicity());
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

std::shared_ptr<protocol::StatusResponse> Beacon::send(std::shared_ptr<providers::IHTTPClientProvider> clientProvider)
{
	std::shared_ptr<protocol::IHTTPClient> httpClient = clientProvider->createClient(mLogger, mHTTPClientConfiguration);

	std::shared_ptr<protocol::StatusResponse> response = nullptr;

	while (true)
	{
		// prefix for this chunk - must be built up newly, due to changing timestamps
		core::UTF8String prefix = mImmutableBasicBeaconData;
		prefix.concatenate( getMutableBeaconData());

		core::UTF8String chunk = mBeaconCache->getNextBeaconChunk(mSessionNumber, prefix, mConfiguration->getMaxBeaconSize() - 1024, BEACON_DATA_DELIMITER);
		if (chunk == nullptr || chunk.empty())
		{
			return response;
		}

		// send the request
		response = httpClient->sendBeaconRequest(mClientIPAddress, chunk);
		if (response == nullptr || response->isErroneousResponse())
		{
			// error happened - but don't know what exactly
			// reset the previously retrieved chunk (restore it in internal cache) & retry another time
			mBeaconCache->resetChunkedData(mSessionNumber);
			break;
		}
		else
		{
			// worked -> remove previously retrieved chunk from cache
			mBeaconCache->removeChunkedData(mSessionNumber);
		}
	}

	return response;
}

void Beacon::addEventData(int64_t timestamp, const core::UTF8String& eventData)
{
	if (mConfiguration->isCapture())
	{
		mBeaconCache->addEventData(mSessionNumber, timestamp, eventData);
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
	return mBeaconCache->isEmpty(mSessionNumber);
}

void Beacon::clearData()
{
	// remove all cached data for this Beacon from the cache
	mBeaconCache->deleteCacheEntry(mSessionNumber);
}

int32_t Beacon::getSessionNumber() const
{
	return mSessionNumber;
}

int64_t Beacon::getDeviceID() const
{
	return mDeviceID;
}

void Beacon::setBeaconConfiguration(std::shared_ptr<configuration::BeaconConfiguration> beaconConfiguration)
{
	std::atomic_store(&mBeaconConfiguration, beaconConfiguration);
}

std::shared_ptr<configuration::BeaconConfiguration> Beacon::getBeaconConfiguration() const
{
	return std::atomic_load(&mBeaconConfiguration);
}

const core::UTF8String& Beacon::getClientIPAddress() const
{
	return mClientIPAddress;
}
