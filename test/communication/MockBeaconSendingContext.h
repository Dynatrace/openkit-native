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

#ifndef _TEST_COMMUNICATION_MOCKBEACONSENDINGCONTEXT_H
#define _TEST_COMMUNICATION_MOCKBEACONSENDINGCONTEXT_H

#include <memory>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "communication/AbstractBeaconSendingState.h"
#include "communication/BeaconSendingContext.h"
#include "configuration/HTTPClientConfiguration.h"
#include "core/UTF8String.h"
#include "configuration/Configuration.h"
#include "protocol/StatusResponse.h"

#include "../providers/TestTimingProvider.h"
#include "TestBeaconSendingState.h"
#include "../providers/TestHTTPClientProvider.h"

using namespace communication;
namespace test
{
	class MockBeaconSendingContext : public BeaconSendingContext
	{
	public:
		MockBeaconSendingContext()
			: MockBeaconSendingContext(std::make_shared<configuration::HTTPClientConfiguration>("", 0, ""))
		{
		}

		MockBeaconSendingContext(std::shared_ptr<configuration::HTTPClientConfiguration> httpClientConfiguration)
			: BeaconSendingContext(std::make_shared<test::TestHTTPClientProvider>(),
				std::make_shared<test::TestTimingProvider>(),
				std::make_shared<configuration::Configuration>(httpClientConfiguration))
			, mHttpClientProvider()
		{
		}

		MOCK_CONST_METHOD0(isShutdownRequested, bool());
		MOCK_METHOD0(requestShutdown, void());
		MOCK_METHOD1(setLastOpenSessionBeaconSendTime, void(uint64_t));
		MOCK_CONST_METHOD0(getCurrentTimestamp, uint64_t());
		MOCK_METHOD1(setLastStatusCheckTime, void(uint64_t));
		MOCK_METHOD1(setInitCompleted, void(bool));
		MOCK_METHOD1(setNextState, void(std::shared_ptr<AbstractBeaconSendingState> nextState));
		MOCK_CONST_METHOD0(isInTerminalState, bool());
		MOCK_METHOD1(sleep, void(uint64_t));
		MOCK_METHOD0(getHTTPClient, std::unique_ptr<protocol::HTTPClient>());

		void RealSetNextState(std::shared_ptr<AbstractBeaconSendingState> nextState) 
		{ 
			return BeaconSendingContext::setNextState(nextState); 
		}

		bool RealIsInTerminalState() 
		{
			return BeaconSendingContext::isInTerminalState();
		}

		std::unique_ptr<protocol::HTTPClient> RealGetHTTPClient()
		{ 
			auto httpClientConfiguration = std::make_shared<configuration::HTTPClientConfiguration>(core::UTF8String(""), 0, core::UTF8String(""));
			return mHttpClientProvider.createClient(httpClientConfiguration); 
		}

		std::unique_ptr<protocol::HTTPClient> TestEmptyGetHTTPClient()
		{
			return BeaconSendingContext::getHTTPClient();
		}

		void RealSleep(uint64_t ms)
		{
			return BeaconSendingContext::sleep(ms);
		}

		providers::DefaultHTTPClientProvider mHttpClientProvider;

		virtual ~MockBeaconSendingContext() {}
	};
}
#endif
