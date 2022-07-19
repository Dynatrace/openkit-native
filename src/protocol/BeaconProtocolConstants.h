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

#ifndef _PROTOCOL_BEACONPROTOCOLCONSTANTS_H
#define _PROTOCOL_BEACONPROTOCOLCONSTANTS_H

namespace protocol
{
	//delimiter
	constexpr const char* BEACON_DATA_DELIMITER = "&";

	//web request tag prefix constant
	constexpr const char* TAG_PREFIX = "MT";

	// basic data constants
	constexpr const char* BEACON_KEY_PROTOCOL_VERSION = "vv";
	constexpr const char* BEACON_KEY_OPENKIT_VERSION = "va";
	constexpr const char* BEACON_KEY_APPLICATION_ID = "ap";
	constexpr const char* BEACON_KEY_APPLICATION_NAME = "an";
	constexpr const char* BEACON_KEY_APPLICATION_VERSION = "vn";
	constexpr const char* BEACON_KEY_PLATFORM_TYPE = "pt";
	constexpr const char* BEACON_KEY_AGENT_TECHNOLOGY_TYPE = "tt";
	constexpr const char* BEACON_KEY_VISITOR_ID = "vi";
	constexpr const char* BEACON_KEY_SESSION_NUMBER = "sn";
	constexpr const char* BEACON_KEY_SESSION_SEQUENCE = "ss";
	constexpr const char* BEACON_KEY_CLIENT_IP_ADDRESS = "ip";
	constexpr const char* BEACON_KEY_MULTIPLICITY = "mp";
	constexpr const char* BEACON_KEY_DATA_COLLECTION_LEVEL = "dl";
	constexpr const char* BEACON_KEY_CRASH_REPORTING_LEVEL = "cl";
	constexpr const char* BEACON_KEY_VISIT_STORE_VERSION = "vs";

	//device data constants
	constexpr const char BEACON_KEY_DEVICE_OS[] = "os";
	constexpr const char BEACON_KEY_DEVICE_MANUFACTURER[] = "mf";
	constexpr const char BEACON_KEY_DEVICE_MODEL[] = "md";

	// timestamp constants
	constexpr const char BEACON_KEY_SESSION_START_TIME[] = "tv";
	constexpr const char BEACON_KEY_TRANSMISSION_TIME[] = "tx";

	//action related constants
	constexpr const char BEACON_KEY_EVENT_TYPE[] = "et";
	constexpr const char BEACON_KEY_NAME[] = "na";
	constexpr const char BEACON_KEY_THREAD_ID[] = "it";
	constexpr const char BEACON_KEY_ACTION_ID[] = "ca";
	constexpr const char BEACON_KEY_PARENT_ACTION_ID[] = "pa";
	constexpr const char BEACON_KEY_START_SEQUENCE_NUMBER[] = "s0";
	constexpr const char BEACON_KEY_TIME_0[] = "t0";
	constexpr const char BEACON_KEY_END_SEQUENCE_NUMBER[] = "s1";
	constexpr const char BEACON_KEY_TIME_1[] = "t1";

	// data, error & crash capture constants
	constexpr const char BEACON_KEY_VALUE[] = "vl";
	constexpr const char BEACON_KEY_ERROR_VALUE[] = "ev";
	constexpr const char BEACON_KEY_ERROR_REASON[] = "rs";
	constexpr const char BEACON_KEY_ERROR_STACKTRACE[] = "st";
	constexpr const char BEACON_KEY_ERROR_TECHNOLOGY_TYPE[] = "tt";

	// web request constants
	constexpr const char BEACON_KEY_WEBREQUEST_RESPONSE_CODE[] = "rc";
	constexpr const char BEACON_KEY_WEBREQUEST_BYTES_SENT[] = "bs";
	constexpr const char BEACON_KEY_WEBREQUEST_BYTES_RECEIVED[] = "br";

	// events api
	constexpr const char BEACON_KEY_EVENT_PAYLOAD[] = "pl";
	constexpr const int EVENT_PAYLOAD_BYTES_LENGTH = 16 * 1024;
	constexpr const char* EVENT_PAYLOAD_APPLICATION_ID = "dt.application_id";
	constexpr const char* EVENT_PAYLOAD_INSTANCE_ID = "dt.instance_id";
	constexpr const char* EVENT_PAYLOAD_SESSION_ID = "dt.sid";
	constexpr const char* EVENT_PAYLOAD_SEND_TIMESTAMP = "dt.send_timestamp";
	constexpr const char* SEND_TIMESTAMP_PLACEHOLER = "DT_SEND_TIMESTAMP_PLACEHOLDER";

}

#endif