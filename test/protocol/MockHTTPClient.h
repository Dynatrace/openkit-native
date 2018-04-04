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

#ifndef _TEST_PROTOCOL_MOCKHTTPCLIENT_H
#define _TEST_PROTOCOL_MOCKHTTPCLIENT_H

#include "protocol/IHTTPClient.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

namespace test {
	class MockHTTPClient : public protocol::IHTTPClient
	{
	public:
		MockHTTPClient(std::shared_ptr<configuration::HTTPClientConfiguration> configuration)
			: mHTTPClientConfiguration(configuration)
		{
		}

		virtual std::unique_ptr<protocol::StatusResponse> sendStatusRequest()
		{
			return std::unique_ptr<protocol::StatusResponse>(sendStatusRequestRawPtrProxy());
		}

		virtual std::unique_ptr<protocol::TimeSyncResponse> sendTimeSyncRequest()
		{
			return std::unique_ptr<protocol::TimeSyncResponse>(sendTimeSyncRequestRawPtrProxy());
		}

		virtual std::unique_ptr<protocol::StatusResponse> sendBeaconRequest(const core::UTF8String& clientIPAddress, const core::UTF8String& beaconData)
		{
			return std::unique_ptr<protocol::StatusResponse>(sendBeaconRequestRawPtrProxy(clientIPAddress, beaconData));
		}

		virtual ~MockHTTPClient() {};

		MOCK_METHOD0(sendStatusRequestRawPtrProxy, protocol::StatusResponse*());

		MOCK_METHOD2(sendBeaconRequestRawPtrProxy, protocol::StatusResponse*(const core::UTF8String&, const core::UTF8String&));

		MOCK_METHOD0(sendTimeSyncRequestRawPtrProxy, protocol::TimeSyncResponse*());
	private:
		std::shared_ptr<configuration::HTTPClientConfiguration> mHTTPClientConfiguration;
	};
}
#endif
