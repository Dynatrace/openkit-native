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

#ifndef _PROTOCOL_CONSTANTS_H
#define _PROTOCOL_CONSTANTS_H
namespace protocol
{
	// request type constants
	constexpr char REQUEST_TYPE_MOBILE[] = "type=m";

	// query parameter constants
	constexpr char QUERY_KEY_SERVER_ID[] = "srvid";
	constexpr char QUERY_KEY_APPLICATION[] = "app";
	constexpr char QUERY_KEY_VERSION[] = "va";
	constexpr char QUERY_KEY_PLATFORM_TYPE[] = "pt";
	constexpr char QUERY_KEY_AGENT_TECHNOLOGY_TYPE[] = "tt";
	constexpr char QUERY_KEY_NEW_SESSION[] = "ns";

	// constant query parameter values
	constexpr char OPENKIT_VERSION[] = "7.0.0000";
	constexpr char PLATFORM_TYPE_OPENKIT[] = "1";
	constexpr char AGENT_TECHNOLOGY_TYPE[] = "okc";
	constexpr int32_t PROTOCOL_VERSION = 3;
	constexpr char ERROR_TECHNOLOGY_TYPE[] = "c";

	//maximum supported name length
	constexpr int32_t MAX_NAME_LEN = 250;
}
#endif /* _PROTOCOL_CONSTANTS_H */
