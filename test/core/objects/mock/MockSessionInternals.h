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

		MOCK_METHOD(
			std::shared_ptr<openkit::IRootAction>,
			enterAction,
			(
				const char* /* actionName */
			),
			(override)
		);

		MOCK_METHOD(void, identifyUser, (const char*), (override));

		MOCK_METHOD(
			void,
			sendEvent,
			(
				const char*, /*eventName*/
				(const openkit::json::JsonObjectValue::JsonObjectMapPtr attributes) /*attributes*/
				),
			(override)
		);

		MOCK_METHOD(
			void,
			reportCrash,
			(
				const char*, /* errorName */
				const char*, /* reason */
				const char* /* stacktrace */
			)
		);

		MOCK_METHOD(
			std::shared_ptr<openkit::IWebRequestTracer>,
			traceWebRequest,
			(
				const char* /*url */
			),
			(override)
		);

		MOCK_METHOD(void, end, (), (override));

		MOCK_METHOD(void, startSession, (), (override));

		MOCK_METHOD(
			std::shared_ptr<protocol::IStatusResponse>,
			sendBeacon,
			(
				std::shared_ptr<providers::IHTTPClientProvider>,
				const protocol::IAdditionalQueryParameters&
			),
			(override)
		);

		MOCK_METHOD(bool, isEmpty, (), (const, override));

		MOCK_METHOD(void, clearCapturedData, (), (override));

		MOCK_METHOD(void, end, (bool /* sendSessionEndEvent */), (override));

		MOCK_METHOD(bool, tryEnd, (), (override));

		MOCK_METHOD(int64_t, getSplitByEventsGracePeriodEndTimeInMillis, (), (override));

		MOCK_METHOD(
			void,
			setSplitByEventsGracePeriodEndTimeInMillis,
			(
				int64_t /* splitByEventsGracePeriodEndTimeInMillis */
			),
			(override)
		);

		MOCK_METHOD(
			void,
			initializeServerConfiguration, 
			(
				std::shared_ptr<core::configuration::IServerConfiguration> /* initialServerConfig */ 
			),
			(override)
		);

		MOCK_METHOD(
			void,
			updateServerConfiguration,
			(
				std::shared_ptr<core::configuration::IServerConfiguration> /* serverConfig */
			),
			(override)
		);

		MOCK_METHOD(
			void,
			onChildClosed,
			(
				std::shared_ptr<core::objects::IOpenKitObject>
			),
			(override)
		);

		MOCK_METHOD(void, close, (), (override));

		MOCK_METHOD(bool, isDataSendingAllowed, (), (override));

		MOCK_METHOD(void, enableCapture, (), (override));

		MOCK_METHOD(void, disableCapture, (), (override));

		MOCK_METHOD(bool, canSendNewSessionRequest, (), (const, override));

		MOCK_METHOD(void, decreaseNumRemainingSessionRequests, (), (override));

		MOCK_METHOD(std::shared_ptr<protocol::IBeacon>, getBeacon, (), (override));

		MOCK_METHOD(bool, isConfigured, (), (override));

		MOCK_METHOD(bool, isConfiguredAndFinished, (), (override));

		MOCK_METHOD(bool, isConfiguredAndOpen, (), (override));

		MOCK_METHOD(bool, isFinished, (), (override));

		MOCK_METHOD(bool, wasTriedForEnding, (), (override));
	};
}

#endif