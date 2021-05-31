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

#include "KeyValueResponseParser.h"

using namespace protocol;

std::shared_ptr<IResponseAttributes> KeyValueResponseParser::parse(const core::UTF8String& keyValuePairResponse)
{
	auto keyValuePairs = std::unordered_map<std::string, std::string>();
	for (auto const& part : keyValuePairResponse.split('&'))
	{
		auto found = part.getIndexOf("=");
		if (found == std::string::npos)
		{
			continue;
		}

		auto key = part.substring(0, found);
		auto value = part.substring(found + 1);

		if (key.empty() || value.empty())
		{
			continue;
		}

		keyValuePairs.insert({key.getStringData(), value.getStringData()});
	}

	auto builder = ResponseAttributes::withKeyValueDefaults();
	applyBeaconSizeInKb(builder, keyValuePairs);
	applySendIntervalInSec(builder, keyValuePairs);
	applyCapture(builder, keyValuePairs);
	applyReportCrashes(builder, keyValuePairs);
	applyReportErrors(builder, keyValuePairs);
	applyTrafficControlPercentage(builder, keyValuePairs);
	applyServerId(builder, keyValuePairs);
	applyMultiplicity(builder, keyValuePairs);

	return builder.build();
}

void KeyValueResponseParser::applyBeaconSizeInKb(
	protocol::ResponseAttributes::Builder& builder,
	std::unordered_map <std::string, std::string>& keyValuePairs
)
{
	auto entry = keyValuePairs.find(KeyValueResponseParser::RESPONSE_KEY_MAX_BEACON_SIZE_IN_KB);
	if (entry == keyValuePairs.end())
	{
		return;
	}

	auto beaconSizeInKb = std::stoi(entry->second);
	builder.withMaxBeaconSizeInBytes(beaconSizeInKb * 1024);
}

void KeyValueResponseParser::applySendIntervalInSec(
	protocol::ResponseAttributes::Builder& builder,
	std::unordered_map <std::string, std::string>& keyValuePairs
)
{
	auto entry = keyValuePairs.find(KeyValueResponseParser::RESPONSE_KEY_SEND_INTERVAL_IN_SEC);
	if (entry == keyValuePairs.end())
	{
		return;
	}

	auto sendIntervalInSec = std::stoi(entry->second);
	builder.withSendIntervalInMilliseconds(sendIntervalInSec * 1000);
}

void KeyValueResponseParser::applyCapture(
	protocol::ResponseAttributes::Builder& builder,
	std::unordered_map <std::string, std::string>& keyValuePairs
)
{
	auto entry = keyValuePairs.find(KeyValueResponseParser::RESPONSE_KEY_CAPTURE);
	if (entry == keyValuePairs.end())
	{
		return;
	}

	auto capture = std::stoi(entry->second);
	builder.withCapture(capture == 1);
}

void KeyValueResponseParser::applyReportCrashes(
		protocol::ResponseAttributes::Builder& builder,
	std::unordered_map <std::string, std::string>& keyValuePairs
)
{
	auto entry = keyValuePairs.find(KeyValueResponseParser::RESPONSE_KEY_REPORT_CRASHES);
	if (entry == keyValuePairs.end())
	{
		return;
	}

	auto reportCrashes = std::stoi(entry->second);
	builder.withCaptureCrashes(reportCrashes != 0);
}

void KeyValueResponseParser::applyReportErrors(
	protocol::ResponseAttributes::Builder& builder,
	std::unordered_map <std::string, std::string>& keyValuePairs
)
{
	auto entry = keyValuePairs.find(KeyValueResponseParser::RESPONSE_KEY_REPORT_ERRORS);
	if (entry == keyValuePairs.end())
	{
		return;
	}

	auto reportErrors = std::stoi(entry->second);
	builder.withCaptureErrors(reportErrors != 0);
}

void KeyValueResponseParser::applyTrafficControlPercentage(
	protocol::ResponseAttributes::Builder& builder,
	std::unordered_map <std::string, std::string>& keyValuePairs
)
{
	auto entry = keyValuePairs.find(KeyValueResponseParser::RESPONSE_KEY_TRAFFIC_CONTROL_PERCENTAGE);
	if (entry == keyValuePairs.end())
	{
		return;
	}

	auto trafficControlPercentage = std::stoi(entry->second);
	builder.withTrafficControlPercentage(trafficControlPercentage);
}

void KeyValueResponseParser::applyServerId(
	protocol::ResponseAttributes::Builder& builder,
	std::unordered_map <std::string, std::string>& keyValuePairs
)
{
	auto entry = keyValuePairs.find(KeyValueResponseParser::RESPONSE_KEY_SERVER_ID);
	if (entry == keyValuePairs.end())
	{
		return;
	}

	auto serverId = std::stoi(entry->second);
	builder.withServerId(serverId);
}

void KeyValueResponseParser::applyMultiplicity(
	protocol::ResponseAttributes::Builder& builder,
	std::unordered_map <std::string, std::string>& keyValuePairs
)
{
	auto entry = keyValuePairs.find(KeyValueResponseParser::RESPONSE_KEY_MULTIPLICITY);
	if (entry == keyValuePairs.end())
	{
		return;
	}

	auto multiplicity = std::stoi(entry->second);
	builder.withMultiplicity(multiplicity);
}