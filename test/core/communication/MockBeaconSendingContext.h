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

#ifndef _TEST_CORE_COMMUNICATION_MOCKBEACONSENDINGCONTEXT_H
#define _TEST_CORE_COMMUNICATION_MOCKBEACONSENDINGCONTEXT_H

#include "../../providers/mock/MockIHTTPClientProvider.h"
#include "../../providers/mock/MockITimingProvider.h"

#include "Types.h"
#include "../Types.h"
#include "../configuration/Types.h"
#include "../objects/Types.h"
#include "../util/Types.h"
#include "../../api/Types.h"
#include "../../protocol/Types.h"
#include "../../providers/Types.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <memory>

namespace test
{
class MockBeaconSendingContext : public types::BeaconSendingContext_t
	{
	public:
		MockBeaconSendingContext(
			types::ILogger_sp logger
		)
		: types::BeaconSendingContext_t
		(
			logger,
			MockIHTTPClientProvider::createNice(),
			MockITimingProvider::createNice(),
			std::make_shared<types::Configuration_t>
			(
				std::make_shared<types::Device_t>("", "", ""),
				types::OpenKitType_t::Type::DYNATRACE,
				types::Utf8String_t(""),
				types::Utf8String_t(""),
				types::Utf8String_t(""),
				1,
				"1",
				types::Utf8String_t(""),
				std::make_shared<types::DefaultSessionIdProvider_t>(),
				std::make_shared<types::SslStrictTrustManager_t>(),
				std::make_shared<types::BeaconCacheConfiguration_t>(-1, -1, -1),
				std::make_shared<types::BeaconConfiguration_t>()
			)
		)
		{
		}

		MOCK_METHOD0(requestShutdown, void());
		MOCK_CONST_METHOD0(isShutdownRequested, bool());
		MOCK_METHOD1(setInitCompleted, void(bool));
		MOCK_CONST_METHOD0(isInTerminalState, bool());
		MOCK_CONST_METHOD0(isCaptureOn, bool());
		MOCK_METHOD1(setNextState, void(types::AbstractBeaconSendingState_sp nextState));
		MOCK_METHOD0(getHTTPClientProvider, types::IHttpClientProvider_sp());
		MOCK_METHOD0(getHTTPClient, types::IHttpClient_sp());
		MOCK_CONST_METHOD0(getCurrentTimestamp, int64_t());
		MOCK_METHOD0(sleep, void());
		MOCK_METHOD1(sleep, void(int64_t));
		MOCK_METHOD1(setLastOpenSessionBeaconSendTime, void(int64_t));
		MOCK_CONST_METHOD0(getLastOpenSessionBeaconSendTime, int64_t());
		MOCK_METHOD1(setLastStatusCheckTime, void(int64_t));
		MOCK_CONST_METHOD0(getSendInterval, int64_t());
		MOCK_METHOD0(getAllNewSessions, std::vector<types::SessionWrapper_sp>());
		MOCK_METHOD0(getAllOpenAndConfiguredSessions, std::vector<types::SessionWrapper_sp>());
		MOCK_METHOD0(getAllFinishedAndConfiguredSessions, std::vector<types::SessionWrapper_sp>());
		MOCK_METHOD0(disableCapture, void());
		MOCK_METHOD1(finishSession, void(types::Session_sp));
		MOCK_METHOD1(removeSession, bool(types::SessionWrapper_sp));
		MOCK_METHOD1(pushBackFinishedSession, void(types::Session_sp));

		void RealSetNextState(types::AbstractBeaconSendingState_sp nextState)
		{
			return types::BeaconSendingContext_t::setNextState(nextState);
		}

		void RealFinishSession(types::Session_sp session)
		{
			return types::BeaconSendingContext_t::finishSession(session);
		}

		types::AbstractBeaconSendingState_sp RealGetNextState()
		{
			return types::BeaconSendingContext_t::getNextState();
		}

		std::vector<types::SessionWrapper_sp> RealGetAllNewSessions()
		{
			return types::BeaconSendingContext_t::getAllNewSessions();
		}

		std::vector<types::SessionWrapper_sp> RealGetAllOpenAndConfiguredSessions()
		{
			return types::BeaconSendingContext_t::getAllOpenAndConfiguredSessions();
		}

		std::vector<types::SessionWrapper_sp> RealGetAllFinishedAndConfiguredSessions()
		{
			return types::BeaconSendingContext_t::getAllFinishedAndConfiguredSessions();
		}

		bool RealIsInTerminalState()
		{
			return types::BeaconSendingContext_t::isInTerminalState();
		}

		virtual ~MockBeaconSendingContext() {}
	};
}
#endif
