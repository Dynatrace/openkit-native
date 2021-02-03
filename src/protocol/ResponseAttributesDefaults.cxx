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

#include "ResponseAttributesDefaults.h"

#include "core/UTF8String.h"

using namespace protocol;

const core::UTF8String ResponseAttributesDefaults::emptyString;

const std::shared_ptr<IResponseAttributes> ResponseAttributesDefaults::jsonResponse()
{
	static const auto jsonResponseDefaults = std::make_shared<ResponseAttributesDefaults::JsonResponseDefaults>();

	return jsonResponseDefaults;
}

const std::shared_ptr<IResponseAttributes> ResponseAttributesDefaults::keyValueResponse()
{
	static const auto keyValueResponseDefaults = std::make_shared<ResponseAttributesDefaults::KeyValueResponseDefaults>();

	return keyValueResponseDefaults;
}

const std::shared_ptr<IResponseAttributes> ResponseAttributesDefaults::undefined()
{
	static const auto undefinedDefaults = std::make_shared<ResponseAttributesDefaults::UndefinedDefaults>();

	return undefinedDefaults;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// AbstractResponseDefaults
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int32_t ResponseAttributesDefaults::AbstractResponseDefaults::getSendIntervalInMilliseconds() const
{
	return 120 * 1000; // 120 sec in millis
}

int32_t ResponseAttributesDefaults::AbstractResponseDefaults::getVisitStoreVersion() const
{
	return 1;
}

bool ResponseAttributesDefaults::AbstractResponseDefaults::isCapture() const
{
	return true;
}

bool ResponseAttributesDefaults::AbstractResponseDefaults::isCaptureCrashes() const
{
	return true;
}

bool ResponseAttributesDefaults::AbstractResponseDefaults::isCaptureErrors() const
{
	return true;
}

const core::UTF8String& ResponseAttributesDefaults::AbstractResponseDefaults::getApplicationId() const
{
	return emptyString;
}

int32_t ResponseAttributesDefaults::AbstractResponseDefaults::getMultiplicity() const
{
	return 1;
}

int32_t ResponseAttributesDefaults::AbstractResponseDefaults::getServerId() const
{
	return 1;
}

const core::UTF8String& ResponseAttributesDefaults::AbstractResponseDefaults::getStatus() const
{
	return emptyString;
}

int64_t ResponseAttributesDefaults::AbstractResponseDefaults::getTimestampInMilliseconds() const
{
	return 0;
}

bool ResponseAttributesDefaults::AbstractResponseDefaults::isAttributeSet(protocol::ResponseAttribute) const
{
	return false;
}

std::shared_ptr<IResponseAttributes> ResponseAttributesDefaults::AbstractResponseDefaults::merge(
	std::shared_ptr<protocol::IResponseAttributes> attributes
) const
{
	return attributes;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// JsonResponseDefaults
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int32_t ResponseAttributesDefaults::JsonResponseDefaults::getMaxBeaconSizeInBytes() const
{
	return 150 * 1024; // 150 kB in bytes
}

int32_t ResponseAttributesDefaults::JsonResponseDefaults::getMaxSessionDurationInMilliseconds() const
{
	return 360 * 60 * 1000; // 360 min in millis
}

int32_t ResponseAttributesDefaults::JsonResponseDefaults::getMaxEventsPerSession() const
{
	return 200;
}

int32_t ResponseAttributesDefaults::JsonResponseDefaults::getSessionTimeoutInMilliseconds() const
{
	return 600 * 1000; // 600 sec in millis
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// KeyValueResponseDefaults
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int32_t ResponseAttributesDefaults::KeyValueResponseDefaults::getMaxBeaconSizeInBytes() const
{
	return 30 * 1024; // 30 kB in bytes
}

int32_t ResponseAttributesDefaults::KeyValueResponseDefaults::getMaxSessionDurationInMilliseconds() const
{
	return -1;
}

int32_t ResponseAttributesDefaults::KeyValueResponseDefaults::getMaxEventsPerSession() const
{
	return -1;
}

int32_t ResponseAttributesDefaults::KeyValueResponseDefaults::getSessionTimeoutInMilliseconds() const
{
	return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UndefinedDefaults
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int32_t ResponseAttributesDefaults::UndefinedDefaults::getMaxBeaconSizeInBytes() const
{
	return 30 * 1024; // 30 kB in bytes
}

int32_t ResponseAttributesDefaults::UndefinedDefaults::getMaxSessionDurationInMilliseconds() const
{
	return -1;
}

int32_t ResponseAttributesDefaults::UndefinedDefaults::getMaxEventsPerSession() const
{
	return -1;
}

int32_t ResponseAttributesDefaults::UndefinedDefaults::getSessionTimeoutInMilliseconds() const
{
	return -1;
}

int32_t ResponseAttributesDefaults::UndefinedDefaults::getServerId() const
{
	return -1;
}