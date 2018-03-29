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

#include "Beacon.h"
#include "ProtocolConstants.h"
#include "BeaconProtocolConstants.h"
#include "core/util/URLEncoding.h"
#include "core/util/InetAddressValidator.h"

using namespace protocol;

Beacon::Beacon(std::shared_ptr<caching::BeaconCache> beaconCache, std::shared_ptr<configuration::Configuration> configuration, const core::UTF8String clientIPAddress, std::shared_ptr<providers::IThreadIDProvider> threadIDProvider, std::shared_ptr<providers::ITimingProvider> timingProvider)
	: mConfiguration(configuration)
	, mClientIPAddress(core::UTF8String(""))
	, mTimingProvider(timingProvider)
	, mThreadIDProvider(threadIDProvider)
	, mSequenceNumber(0)
	, mID(0)
	, mSessionNumber(configuration->createSessionNumber())
	, mSessionStartTime(timingProvider->provideTimestampInMilliseconds())
	, mBasicBeaconData()
	, mBeaconCache(beaconCache)
{
	if (core::util::InetAddressValidator::IsValidIP(clientIPAddress))
	{
		mClientIPAddress = clientIPAddress;
	}
}

core::UTF8String Beacon::createBasicBeaconData()
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
	addKeyValuePair(basicBeaconData, protocol::BEACON_KEY_VISITOR_ID, mConfiguration->getDeviceID());
	addKeyValuePair(basicBeaconData, protocol::BEACON_KEY_SESSION_NUMBER, static_cast<int32_t>(mSessionNumber));
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
	
	return basicBeaconData;
}

core::UTF8String Beacon::createBasicEventData(protocol::EventType eventType, const core::UTF8String& eventName)
{
	core::UTF8String eventData;
	addKeyValuePair(eventData, BEACON_KEY_EVENT_TYPE, static_cast<int32_t>(eventType));

	if (!eventName.empty())
	{
		addKeyValuePair(eventData, BEACON_KEY_NAME, trunctate(eventName));
	}
	addKeyValuePair(eventData, BEACON_KEY_THREAD_ID, mThreadIDProvider->getThreadID());
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
	s.concatenate(core::util::URLEncoding::urlencode(value));
}

void Beacon::addKeyValuePair(core::UTF8String& s, const core::UTF8String& key, int32_t value)
{
	appendKey(s, key);
	s.concatenate(std::to_string(value));
}

void Beacon::addKeyValuePair(core::UTF8String& s, const core::UTF8String& key, int64_t value)
{
	appendKey(s, key);
	s.concatenate(std::to_string(value));
}

void Beacon::addKeyValuePair(core::UTF8String& s, const core::UTF8String& key, double value)
{
	appendKey(s, key);
	s.concatenate(std::to_string(value));
}

int32_t Beacon::createSequenceNumber()
{
	return mSequenceNumber++;
}

int64_t Beacon::getCurrentTimestamp() const
{
	return mTimingProvider->provideTimestampInMilliseconds();
}

int32_t Beacon::createID()
{
	return mID++;
}

void Beacon::addAction(std::shared_ptr<core::Action> action)
{
	core::UTF8String actionData = createBasicEventData(EventType::ACTION, action->getName());

	addKeyValuePair(actionData, BEACON_KEY_ACTION_ID, action->getID());
	addKeyValuePair(actionData, BEACON_KEY_PARENT_ACTION_ID, action->getParentID());
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

void Beacon::endSession(std::shared_ptr<core::Session> session)
{
	core::UTF8String eventData = createBasicEventData(EventType::SESSION_END, nullptr);

	addKeyValuePair(eventData, BEACON_KEY_PARENT_ACTION_ID, 0);
	addKeyValuePair(eventData, BEACON_KEY_START_SEQUENCE_NUMBER, createSequenceNumber());
	addKeyValuePair(eventData, BEACON_KEY_TIME_0, getTimeSinceSessionStartTime(session->getEndTime()));

	addEventData(session->getEndTime(), eventData);
}

void Beacon::reportCrash(const core::UTF8String& errorName, const core::UTF8String& reason, const core::UTF8String& stacktrace)
{
	if (!mConfiguration->isCaptureCrashes()) {
		return;
	}

	core::UTF8String eventData = createBasicEventData(EventType::FAILURE_CRASH, errorName);

	auto timestamp = mTimingProvider->provideTimestampInMilliseconds();

	addKeyValuePair(eventData, BEACON_KEY_PARENT_ACTION_ID, 0);                                  // no parent action
	addKeyValuePair(eventData, BEACON_KEY_START_SEQUENCE_NUMBER, createSequenceNumber());
	addKeyValuePair(eventData, BEACON_KEY_TIME_0, getTimeSinceSessionStartTime(timestamp));
	addKeyValuePair(eventData, BEACON_KEY_ERROR_REASON, reason);
	addKeyValuePair(eventData, BEACON_KEY_ERROR_STACKTRACE, stacktrace);

	addEventData(timestamp, eventData);
}

void Beacon::identifyUser(const core::UTF8String& userTag)
{
	core::UTF8String eventData = createBasicEventData(EventType::IDENTIFY_USER, userTag);

	auto timestamp = mTimingProvider->provideTimestampInMilliseconds();

	addKeyValuePair(eventData, BEACON_KEY_PARENT_ACTION_ID, 0);
	addKeyValuePair(eventData, BEACON_KEY_START_SEQUENCE_NUMBER, createSequenceNumber());
	addKeyValuePair(eventData, BEACON_KEY_TIME_0, getTimeSinceSessionStartTime(timestamp));

	addEventData(timestamp, eventData);
}

void Beacon::addEventData(int64_t timestamp, const core::UTF8String& eventData)
{
	if (mConfiguration->isCapture())
	{
		mBeaconCache->addEventData(mSessionNumber, timestamp, eventData);
	}
}

core::UTF8String Beacon::trunctate(const core::UTF8String& string)
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