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
#include "configuration/OpenKitType.h"
#include "protocol/StatusResponse.h"
#include "protocol/ssl/SSLStrictTrustManager.h"
#include "providers/DefaultHTTPClientProvider.h"
#include "providers/DefaultSessionIDProvider.h"

#include "../providers/TestTimingProvider.h"
#include "TestBeaconSendingState.h"
#include "../protocol/MockHTTPClient.h"

using namespace communication;
namespace test
{
	class MockBeaconSendingContext : public BeaconSendingContext
	{
	public:
		MockBeaconSendingContext()
			: BeaconSendingContext(std::make_shared<providers::DefaultHTTPClientProvider>(),
				std::make_shared<test::TestTimingProvider>(),
				std::make_shared<configuration::Configuration>( std::shared_ptr<configuration::Device>(new configuration::Device("", "", "")),configuration::OpenKitType::DYNATRACE, core::UTF8String(""), core::UTF8String(""), 1,  core::UTF8String(""),
																std::make_shared<providers::DefaultSessionIDProvider>(),
																std::make_shared<protocol::SSLStrictTrustManager>()))
		{
		}

		MOCK_CONST_METHOD0(isShutdownRequested, bool());
		MOCK_METHOD0(requestShutdown, void());
		MOCK_METHOD1(setLastOpenSessionBeaconSendTime, void(int64_t));
		MOCK_CONST_METHOD0(getCurrentTimestamp, int64_t());
		MOCK_METHOD1(setLastStatusCheckTime, void(int64_t));
		MOCK_METHOD1(setInitCompleted, void(bool));
		MOCK_METHOD1(setNextState, void(std::shared_ptr<AbstractBeaconSendingState> nextState));
		MOCK_CONST_METHOD0(isInTerminalState, bool());
		MOCK_METHOD1(sleep, void(int64_t));
		MOCK_METHOD0(getHTTPClient, std::shared_ptr<protocol::IHTTPClient>());
		MOCK_CONST_METHOD0(isTimeSyncSupported, bool());
		MOCK_CONST_METHOD0(getLastTimeSyncTime, int64_t());
		MOCK_CONST_METHOD0(isCaptureOn, bool());
		MOCK_METHOD2(initializeTimeSync, void (int64_t , bool ));
		MOCK_METHOD1(setLastTimeSyncTime, void(int64_t));
		MOCK_METHOD0(disableTimeSyncSupport, void());

		void RealSetNextState(std::shared_ptr<AbstractBeaconSendingState> nextState) 
		{ 
			return BeaconSendingContext::setNextState(nextState); 
		}

		bool RealIsInTerminalState() 
		{
			return BeaconSendingContext::isInTerminalState();
		}

		void RealSleep(uint64_t ms)
		{
			return BeaconSendingContext::sleep(ms);
		}	

		virtual ~MockBeaconSendingContext() {}
	};
}
#endif
