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

#ifndef _TEST_PROTOCOL_TYPES_H
#define _TEST_PROTOCOL_TYPES_H

#include "protocol/ssl/SSLBlindTrustManager.h"
#include "protocol/ssl/SSLStrictTrustManager.h"
#include "protocol/Beacon.h"
#include "protocol/EventType.h"
#include "protocol/HTTPClient.h"
#include "protocol/HTTPResponseParser.h"
#include "protocol/IStatusResponse.h"
#include "protocol/StatusResponse.h"

namespace test
{
	namespace types
	{
		using SslBlindTrustManager_t = protocol::SSLBlindTrustManager;
		using SslBlindTrustManager_sp = std::shared_ptr<SslBlindTrustManager_t>;

		using SslStrictTrustManager_t = protocol::SSLStrictTrustManager;
		using SslStrictTrustManager_sp = std::shared_ptr<SslStrictTrustManager_t>;

		using Beacon_t = protocol::Beacon;
		using Beacon_sp = std::shared_ptr<Beacon_t>;

		using EventType_t = protocol::EventType;

		using HttpClient_t = protocol::HTTPClient;
		using HttpClient_sp = std::shared_ptr<HttpClient_t>;

		using HttpResponseParser_t = protocol::HTTPResponseParser;

		using IBeacon_t = protocol::IBeacon;
		using IBeacon_sp = std::shared_ptr<IBeacon_t>;

		using IHttpClient_t = protocol::IHTTPClient;
		using IHttpClient_sp = std::shared_ptr<IHttpClient_t>;

		using IStatusResponse_t = protocol::IStatusResponse;
		using IStatusResponse_sp = std::shared_ptr<IStatusResponse_t>;

		using StatusResponse_t = protocol::StatusResponse;
		using StatusResponse_sp = std::shared_ptr<StatusResponse_t>;
	}
}

#endif
