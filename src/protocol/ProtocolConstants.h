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

#ifndef _PROTOCOL_CONSTANTS_H
#define _PROTOCOL_CONSTANTS_H

namespace protocol
{
	// request type constants
	constexpr const char* REQUEST_TYPE_MOBILE = "type=m";

	// query parameter constants
	constexpr const char* QUERY_KEY_SERVER_ID = "srvid";
	constexpr const char* QUERY_KEY_APPLICATION = "app";
	constexpr const char* QUERY_KEY_VERSION = "va";
	constexpr const char* QUERY_KEY_PLATFORM_TYPE = "pt";
	constexpr const char* QUERY_KEY_AGENT_TECHNOLOGY_TYPE = "tt";
	constexpr const char* QUERY_KEY_RESPONSE_TYPE = "resp";
	constexpr const char* QUERY_KEY_CONFIG_TIMESTAMP = "cts";
	constexpr const char* QUERY_KEY_NEW_SESSION = "ns";
	constexpr const char* QUERY_KEY_SESSION_IDENTIFIER = "si";

	// constant query parameter values
	constexpr const char* OPENKIT_VERSION = "8.281.30300";
	constexpr const char* PLATFORM_TYPE_OPENKIT = "1";
	constexpr const char* AGENT_TECHNOLOGY_TYPE = "okc";
	constexpr int32_t PROTOCOL_VERSION = 3;
	constexpr const char* ERROR_TECHNOLOGY_TYPE = "c";
	constexpr const char* RESPONSE_TYPE_JSON = "json";

	//maximum supported name length
	constexpr int32_t MAX_NAME_LEN = 250;
	constexpr int32_t MAX_REASON_LEN = 1000;
	constexpr int32_t MAX_STACKTRACE_LEN = 128000;
}

#endif /* _PROTOCOL_CONSTANTS_H */
