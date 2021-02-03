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

#ifndef _TEST_CORE_OBJECTS_MOCKSESSIONINTERNALS_H
#define _TEST_CORE_OBJECTS_MOCKSESSIONINTERNALS_H

#include "OpenKit/IRootAction.h"
#include "OpenKit/IWebRequestTracer.h"
#include "core/configuration/IBeaconConfiguration.h"
#include "core/objects/IOpenKitObject.h"
#include "core/objects/SessionInternals.h"
#include "protocol/IAdditionalQueryParameters.h"
#include "protocol/IStatusResponse.h"
#include "providers/IHTTPClientProvider.h"

#include "gmock/gmock.h"

#include <memory>

namespace test
{
	class MockSessionInternals
		: public core::objects::SessionInternals
	{
	public:

		MockSessionInternals()
		{
			ON_CALL(*this, enterAction(testing::_)).WillByDefault(testing::ReturnNull());
			ON_CALL(*this, traceWebRequest(testing::_)).WillByDefault(testing::ReturnNull());
			ON_CALL(*this, sendBeacon(testing::_, testing::_)).WillByDefault(testing::ReturnNull());
		}

		~MockSessionInternals() override = default;

		static std::shared_ptr<testing::NiceMock<MockSessionInternals>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockSessionInternals>>();
		}

		static std::shared_ptr<testing::StrictMock<MockSessionInternals>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockSessionInternals>>();
		}

		MOCK_METHOD1(enterAction,
			std::shared_ptr<openkit::IRootAction>(
				const char* /* actionName */
			)
		);

		MOCK_METHOD1(identifyUser,
			void(
				const char* /* userTag */
			)
		);

		MOCK_METHOD3(reportCrash,
			void(
				const char*, /* errorName */
				const char*, /* reason */
				const char* /* stacktrace */
			)
		);

		MOCK_METHOD1(traceWebRequest,
			std::shared_ptr<openkit::IWebRequestTracer>(
				const char* /*url */
			)
		);

		MOCK_METHOD0(end, void());

		MOCK_METHOD0(startSession, void());

		MOCK_METHOD2(sendBeacon,
			std::shared_ptr<protocol::IStatusResponse>(
				std::shared_ptr<providers::IHTTPClientProvider>,
				const protocol::IAdditionalQueryParameters&
			)
		);

		MOCK_CONST_METHOD0(isEmpty, bool());

		MOCK_METHOD0(clearCapturedData, void());

		MOCK_CONST_METHOD0(isSessionEnded, bool());

		MOCK_METHOD1(end, void(bool /* sendSessionEndEvent */));

		MOCK_METHOD0(tryEnd, bool());

		MOCK_METHOD0(getSplitByEventsGracePeriodEndTimeInMillis, int64_t());

		MOCK_METHOD1(setSplitByEventsGracePeriodEndTimeInMillis,
			void(
				int64_t /* splitByEventsGracePeriodEndTimeInMillis */
			)
		);

		MOCK_METHOD1(initializeServerConfiguration, 
			void(
				std::shared_ptr<core::configuration::IServerConfiguration> /* initialServerConfig */ 
			)
		);

		MOCK_METHOD1(updateServerConfiguration,
			void(
				std::shared_ptr<core::configuration::IServerConfiguration> /* serverConfig */
			)
		);

		MOCK_METHOD1(onChildClosed,
			void(
				std::shared_ptr<core::objects::IOpenKitObject>
			)
		);

		MOCK_METHOD0(close, void());

		MOCK_METHOD0(isDataSendingAllowed, bool());

		MOCK_METHOD0(enableCapture, void());

		MOCK_METHOD0(disableCapture, void());

		MOCK_CONST_METHOD0(canSendNewSessionRequest, bool());

		MOCK_METHOD0(decreaseNumRemainingSessionRequests, void());

		MOCK_METHOD0(getBeacon, std::shared_ptr<protocol::IBeacon>());

		MOCK_METHOD0(isConfigured, bool());

		MOCK_METHOD0(isConfiguredAndFinished, bool());

		MOCK_METHOD0(isConfiguredAndOpen, bool());

		MOCK_METHOD0(isFinished, bool());

		MOCK_METHOD0(wasTriedForEnding, bool());
	};
}

#endif