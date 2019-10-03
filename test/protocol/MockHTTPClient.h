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

#ifndef _TEST_PROTOCOL_MOCKHTTPCLIENT_H
#define _TEST_PROTOCOL_MOCKHTTPCLIENT_H

#include "Types.h"
#include "../core/Types.h"
#include "../core/configuration/Types.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

namespace test {
	class MockHTTPClient : public types::IHttpClient_t
	{
	public:
		MockHTTPClient(types::HttpClientConfiguration_sp configuration)
			: mHTTPClientConfiguration(configuration)
		{
		}

		virtual types::StatusResponse_sp sendStatusRequest()
		{
			return types::StatusResponse_sp(sendStatusRequestRawPtrProxy());
		}

		virtual types::StatusResponse_sp sendBeaconRequest(
				const types::Utf8String_t& clientIPAddress,
				const types::Utf8String_t& beaconData
			)
		{
			return types::StatusResponse_sp(sendBeaconRequestRawPtrProxy(clientIPAddress, beaconData));
		}

		virtual types::StatusResponse_sp sendNewSessionRequest()
		{
			return types::StatusResponse_sp(sendNewSessionRequestRawPtrProxy());
		}

		virtual ~MockHTTPClient() {}

		MOCK_METHOD0(sendStatusRequestRawPtrProxy, types::StatusResponse_sp());

		MOCK_METHOD2(sendBeaconRequestRawPtrProxy,
			types::StatusResponse_sp(
				const types::Utf8String_t&,
				const types::Utf8String_t&
			)
		);

		MOCK_METHOD0(sendNewSessionRequestRawPtrProxy, types::StatusResponse_sp());
	private:
		types::HttpClientConfiguration_sp mHTTPClientConfiguration;
	};
}
#endif
