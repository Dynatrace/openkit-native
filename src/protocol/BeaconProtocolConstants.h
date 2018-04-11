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

#ifndef _PROTOCOL_BEACONPROTOCOLCONSTANTS_H
#define _PROTOCOL_BEACONPROTOCOLCONSTANTS_H

namespace protocol
{
	//delimiter
	constexpr char BEACON_DATA_DELIMITER = '&';

	//web request tag prefix constant
	constexpr char TAG_PREFIX[] = "MT";

	// basic data constants
	constexpr char BEACON_KEY_PROTOCOL_VERSION[] = "vv";
	constexpr char BEACON_KEY_OPENKIT_VERSION[] = "va";
	constexpr char BEACON_KEY_APPLICATION_ID[] = "ap";
	constexpr char BEACON_KEY_APPLICATION_NAME[] = "an";
	constexpr char BEACON_KEY_APPLICATION_VERSION[] = "vn";
	constexpr char BEACON_KEY_PLATFORM_TYPE[] = "pt";
	constexpr char BEACON_KEY_AGENT_TECHNOLOGY_TYPE[] = "tt";
	constexpr char BEACON_KEY_VISITOR_ID[] = "vi";
	constexpr char BEACON_KEY_SESSION_NUMBER[] = "sn";
	constexpr char BEACON_KEY_CLIENT_IP_ADDRESS[] = "ip";

	//device data constants
	constexpr char BEACON_KEY_DEVICE_OS[] = "os";
	constexpr char BEACON_KEY_DEVICE_MANUFACTURER[] = "mf";
	constexpr char BEACON_KEY_DEVICE_MODEL[] = "md";

	// timestamp constants
	constexpr char BEACON_KEY_SESSION_START_TIME[] = "tv";
	constexpr char BEACON_KEY_TIMESYNC_TIME[] = "ts";
	constexpr char BEACON_KEY_TRANSMISSION_TIME[] = "tx";

	//action related constants
	constexpr char BEACON_KEY_EVENT_TYPE[] = "et";
	constexpr char BEACON_KEY_NAME[] = "na";
	constexpr char BEACON_KEY_THREAD_ID[] = "it";
	constexpr char BEACON_KEY_ACTION_ID[] = "ca";
	constexpr char BEACON_KEY_PARENT_ACTION_ID[] = "pa";
	constexpr char BEACON_KEY_START_SEQUENCE_NUMBER[] = "s0";
	constexpr char BEACON_KEY_TIME_0[] = "t0";
	constexpr char BEACON_KEY_END_SEQUENCE_NUMBER[] = "s1";
	constexpr char BEACON_KEY_TIME_1[] = "t1";

	// data, error & crash capture constants
	constexpr char BEACON_KEY_VALUE[] = "vl";
	constexpr char BEACON_KEY_ERROR_CODE[] = "ev";
	constexpr char BEACON_KEY_ERROR_REASON[] = "rs";
	constexpr char BEACON_KEY_ERROR_STACKTRACE[] = "st";
	constexpr char BEACON_KEY_WEBREQUEST_RESPONSE_CODE[] = "rc";
	constexpr char BEACON_KEY_WEBREQUEST_BYTES_SENT[] = "bs";
	constexpr char BEACON_KEY_WEBREQUEST_BYTES_RECEIVED[] = "br";
}

#endif