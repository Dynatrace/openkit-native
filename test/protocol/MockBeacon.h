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

#ifndef _TEST_PROTOCOL_MOCKBEACON_H
#define _TEST_PROTOCOL_MOCKBEACON_H

#include "protocol/Beacon.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <memory>

#include "core/util/DefaultLogger.h"
#include "caching/BeaconCache.h"
#include "configuration/Configuration.h"
#include "core/UTF8String.h"
#include "providers/ITimingProvider.h"
#include "providers/IThreadIDProvider.h"

namespace test {
	class MockBeacon : public protocol::Beacon
	{
	public:
		MockBeacon(std::shared_ptr<openkit::ILogger> logger, std::shared_ptr<caching::BeaconCache> beaconCache, std::shared_ptr<configuration::Configuration> configuration, const char* clientIPAddress, std::shared_ptr<providers::IThreadIDProvider> threadIDProvider, std::shared_ptr<providers::ITimingProvider> timingProvider)
			: Beacon(logger, beaconCache, configuration, clientIPAddress, threadIDProvider, timingProvider)
		{

		}

		std::shared_ptr<protocol::StatusResponse> RealSend(std::shared_ptr<providers::IHTTPClientProvider> httpClientProvider)
		{
			return protocol::Beacon::send(httpClientProvider);
		}

		void reportValue(int32_t actionID, const core::UTF8String& valueName, int32_t value) override
		{
			reportValueInt32(actionID, valueName, value);
		}

		void reportValue(int32_t actionID, const core::UTF8String& valueName, double value) override
		{
			reportValueDouble(actionID, valueName, value);
		}

		void reportValue(int32_t actionID, const core::UTF8String& valueName, const core::UTF8String& value) override
		{
			reportValueString(actionID, valueName, value);
		}

		virtual ~MockBeacon() {}

		MOCK_METHOD1(identifyUser, void(const core::UTF8String& userTag));
		MOCK_METHOD2(reportEvent, void(int32_t, const core::UTF8String&));
		MOCK_METHOD3(reportValueInt32, void(int32_t, const core::UTF8String&, int32_t));
		MOCK_METHOD3(reportValueDouble, void(int32_t, const core::UTF8String&, double));
		MOCK_METHOD3(reportValueString, void(int32_t, const core::UTF8String&, const core::UTF8String&));
		MOCK_METHOD4(reportError, void(int32_t, const core::UTF8String&, int32_t, const core::UTF8String&));
		MOCK_METHOD3(reportCrash, void(const core::UTF8String&, const core::UTF8String&, const core::UTF8String&));
		MOCK_METHOD2(addWebRequest, void(int32_t, std::shared_ptr<core::WebRequestTracer>));
		MOCK_METHOD0(startSession, void(void));
		MOCK_METHOD1(endSession, void(std::shared_ptr<core::Session>));
		MOCK_CONST_METHOD0(getCurrentTimestamp, int64_t(void));
		MOCK_METHOD1(send, std::shared_ptr<protocol::StatusResponse>(std::shared_ptr<providers::IHTTPClientProvider>));
		MOCK_METHOD2(createTag, core::UTF8String(int32_t, int32_t));
		MOCK_METHOD0(createSequenceNumber, int32_t());
	};
}
#endif
