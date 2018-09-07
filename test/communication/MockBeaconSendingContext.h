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

#include "core/util/DefaultLogger.h"
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
#include "core/SessionWrapper.h"

#include "../providers/MockTimingProvider.h"
#include "TestBeaconSendingState.h"
#include "../protocol/MockHTTPClient.h"
#include "../providers/MockHTTPClientProvider.h"

using namespace communication;
namespace test
{
	class MockBeaconSendingContext : public BeaconSendingContext
	{
	public:
		MockBeaconSendingContext(std::shared_ptr<openkit::ILogger> logger)
			: BeaconSendingContext(logger, 
				std::make_shared<test::MockHTTPClientProvider>(),
				std::make_shared<test::MockTimingProvider>(),
				std::make_shared<configuration::Configuration>( std::shared_ptr<configuration::Device>(new configuration::Device("", "", "")), configuration::OpenKitType::Type::DYNATRACE, core::UTF8String(""), core::UTF8String(""), core::UTF8String(""), core::UTF8String("1"),  core::UTF8String(""),
																std::make_shared<providers::DefaultSessionIDProvider>(),
																std::make_shared<protocol::SSLStrictTrustManager>(),
																std::make_shared<configuration::BeaconCacheConfiguration>(-1, -1, -1),
																std::make_shared<configuration::BeaconConfiguration>()))
		{
		}

		MOCK_METHOD0(requestShutdown, void());
		MOCK_CONST_METHOD0(isShutdownRequested, bool());
		MOCK_METHOD1(setInitCompleted, void(bool));
		MOCK_CONST_METHOD0(isInTerminalState, bool());
		MOCK_CONST_METHOD0(isCaptureOn, bool());
		MOCK_METHOD2(initializeTimeSync, void(int64_t, bool));
		MOCK_CONST_METHOD0(isTimeSyncSupported, bool());
		MOCK_METHOD0(disableTimeSyncSupport, void());
		MOCK_CONST_METHOD0(isTimeSynced, bool());
		MOCK_METHOD1(setNextState, void(std::shared_ptr<AbstractBeaconSendingState> nextState));
		MOCK_METHOD0(getHTTPClientProvider, std::shared_ptr<providers::IHTTPClientProvider>());
		MOCK_METHOD0(getHTTPClient, std::shared_ptr<protocol::IHTTPClient>());
		MOCK_CONST_METHOD0(getCurrentTimestamp, int64_t());
		MOCK_METHOD0(sleep, void());
		MOCK_METHOD1(sleep, void(int64_t));
		MOCK_METHOD1(setLastOpenSessionBeaconSendTime, void(int64_t));
		MOCK_CONST_METHOD0(getLastOpenSessionBeaconSendTime, int64_t());
		MOCK_METHOD1(setLastStatusCheckTime, void(int64_t));
		MOCK_CONST_METHOD0(getSendInterval, int64_t());
		MOCK_METHOD0(getAllNewSessions, std::vector<std::shared_ptr<core::SessionWrapper>>());
		MOCK_METHOD0(getAllOpenAndConfiguredSessions, std::vector<std::shared_ptr<core::SessionWrapper>>());
		MOCK_METHOD0(getAllFinishedAndConfiguredSessions, std::vector<std::shared_ptr<core::SessionWrapper>>());
		MOCK_METHOD0(disableCapture, void());
		MOCK_CONST_METHOD0(getLastTimeSyncTime, int64_t());
		MOCK_METHOD1(setLastTimeSyncTime, void(int64_t));
		MOCK_METHOD1(finishSession, void(std::shared_ptr<core::Session>));
		MOCK_METHOD1(removeSession, bool(std::shared_ptr<core::SessionWrapper>));
		MOCK_METHOD1(pushBackFinishedSession, void(std::shared_ptr<core::Session>));

		void RealSetNextState(std::shared_ptr<AbstractBeaconSendingState> nextState) 
		{ 
			return BeaconSendingContext::setNextState(nextState); 
		}

		void RealFinishSession(std::shared_ptr<core::Session> session)
		{
			return BeaconSendingContext::finishSession(session);
		}

		std::shared_ptr<AbstractBeaconSendingState> RealGetNextState()
		{
			return BeaconSendingContext::getNextState();
		}

		std::vector<std::shared_ptr<core::SessionWrapper>> RealGetAllNewSessions()
		{
			return BeaconSendingContext::getAllNewSessions();
		}

		std::vector<std::shared_ptr<core::SessionWrapper>> RealGetAllOpenAndConfiguredSessions()
		{
			return BeaconSendingContext::getAllOpenAndConfiguredSessions();
		}

		std::vector<std::shared_ptr<core::SessionWrapper>> RealGetAllFinishedAndConfiguredSessions()
		{
			return BeaconSendingContext::getAllFinishedAndConfiguredSessions();
		}

		bool RealIsInTerminalState() 
		{
			return BeaconSendingContext::isInTerminalState();
		}	

		virtual ~MockBeaconSendingContext() {}
	};
}
#endif
