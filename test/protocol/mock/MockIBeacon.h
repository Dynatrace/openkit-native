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

#ifndef _TEST_PROTOCOL_MOCK_MOCKIBEACON_H
#define _TEST_PROTOCOL_MOCK_MOCKIBEACON_H

#include "../../DefaultValues.h"
#include "protocol/IBeacon.h"
#include "protocol/IStatusResponse.h"
#include "core/configuration/IBeaconConfiguration.h"
#include "core/objects/IActionCommon.h"
#include "core/objects/IWebRequestTracerInternals.h"
#include "providers/IHTTPClientProvider.h"

#include "gmock/gmock.h"

#include <memory>

namespace test
{
	class MockIBeacon
		: public protocol::IBeacon
	{
	public:

		MockIBeacon()
		{
			ON_CALL(*this, createTag(testing::_, testing::_))
				.WillByDefault(testing::Return(DefaultValues::UTF8_EMPTY_STRING));
			ON_CALL(*this, send(testing::_, testing::_))
				.WillByDefault(testing::ReturnNull());
			ON_CALL(*this, useClientIPAddress())
				.WillByDefault(testing::Return(false));
			ON_CALL(*this, getClientIPAddress())
				.WillByDefault(testing::ReturnRefOfCopy(DefaultValues::UTF8_EMPTY_STRING));
		}

		~MockIBeacon() override = default;

		static std::shared_ptr<testing::NiceMock<MockIBeacon>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockIBeacon>>();
		}

		static std::shared_ptr<testing::StrictMock<MockIBeacon>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockIBeacon>>();
		}

		MOCK_METHOD(int32_t, createSequenceNumber, (), (override));

		MOCK_METHOD(int64_t, getCurrentTimestamp, (), (const, override));

		MOCK_METHOD(int64_t, getSessionStartTime, (), (const, override));

		MOCK_METHOD(int32_t, createID, (), (override));

		MOCK_METHOD(
			core::UTF8String,
			createTag,
			(
				int32_t, /* actionID */
				int32_t /* sequenceNumber */
			),
			(override)
		);

		MOCK_METHOD(void, addAction, (std::shared_ptr<core::objects::IActionCommon>), (override));

		MOCK_METHOD(void, startSession, (), (override));

		MOCK_METHOD(void, endSession, (), (override));

		MOCK_METHOD(
			void,
			reportValue,
			(
				int32_t, /* actionID */
				const core::UTF8String&, /* valueName */
				int32_t /* value */
			),
			(override)
		);

		MOCK_METHOD(
			void,
			reportValue,
			(
				int32_t, /* actionID */
				const core::UTF8String&, /* valueName */
				int64_t /* value */
			),
			(override)
		);

		MOCK_METHOD(
			void,
			reportValue,
			(
				int32_t, /* actionID */
				const core::UTF8String&, /* valueName */
				double /* value */
			),
			(override)
		);

		MOCK_METHOD(
			void,
			reportValue,
			(
				int32_t, /* actionID */
				const core::UTF8String&, /* valueName */
				const core::UTF8String& /* value */
			),
			(override)
		);

		MOCK_METHOD(
			void,
			reportEvent,
			(
				int32_t, /* actionID */
				const core::UTF8String& /* eventName */
			),
			(override)
		);

		MOCK_METHOD(
			void,
			reportError,
			(
				int32_t, /* actionID */
				const core::UTF8String&, /* errorName */
				int32_t /* errorCode */
			),
			(override)
		);

		MOCK_METHOD(
			void,
			reportError,
			(
				int32_t, /* actionID */
				const core::UTF8String&, /* errorName */
				const core::UTF8String&, /* causeName */
				const core::UTF8String&, /* causeDescription */
				const core::UTF8String&  /* causeStackTrace */
				),
			(override)
		);

		MOCK_METHOD(
			void,
			reportCrash,
			(
				const core::UTF8String&, /* errorName */
				const core::UTF8String&, /* reason */
				const core::UTF8String& /* stacktrace */
			),
			(override)
		);

		MOCK_METHOD(
			void,
			addWebRequest,
			(
				int32_t, /* parentActionID */
				std::shared_ptr<core::objects::IWebRequestTracerInternals> /* webRequestTracer */
			),
			(override)
		);

		MOCK_METHOD(void, identifyUser, (const core::UTF8String& /* userTag */), (override));

		MOCK_METHOD(
			void,
			sendEvent,
			(
				const core::UTF8String&, /*name*/
				(const openkit::json::JsonObjectValue::JsonObjectMapPtr) /*attributes*/
			),
			(override)
		);

		MOCK_METHOD(
			void,
			sendBizEvent,
			(
				const core::UTF8String&, /*type*/
				(const openkit::json::JsonObjectValue::JsonObjectMapPtr) /*attributes*/
				),
			(override)
		);

		MOCK_METHOD(
			std::shared_ptr<protocol::IStatusResponse>,
			send,
			(
				std::shared_ptr<providers::IHTTPClientProvider>, /* clientProvider */
				const protocol::IAdditionalQueryParameters& /* additionalParameters */
			),
			(override)
		);

		MOCK_METHOD(bool, isEmpty, (), (const, override));

		MOCK_METHOD(void, clearData, (), (override));

		MOCK_METHOD(int32_t, getSessionNumber, (), (const, override));

		MOCK_METHOD(int32_t, getSessionSequenceNumber, (), (const, override));

		MOCK_METHOD(int64_t, getDeviceID, (), (const, override));

		MOCK_METHOD(bool, useClientIPAddress, (), (const, override));

		MOCK_METHOD(core::UTF8String&, getClientIPAddress, (), (const, override));
		
		MOCK_METHOD(
			void,
			initializeServerConfiguration,
			(
				std::shared_ptr<core::configuration::IServerConfiguration> /* serverConfiguration */
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

		MOCK_METHOD(bool, isServerConfigurationSet, (), (override));

		MOCK_METHOD(bool, isActionReportingAllowedByPrivacySettings, (), (override));

		MOCK_METHOD(bool, isDataCapturingEnabled, (), (override));

		MOCK_METHOD(bool, isErrorCapturingEnabled, (), (override));

		MOCK_METHOD(bool, isCrashCapturingEnabled, (), (override));

		MOCK_METHOD(void, enableCapture, (), (override));

		MOCK_METHOD(void, disableCapture, (), (override));

		MOCK_METHOD(void, setServerConfigurationUpdateCallback, (core::configuration::ServerConfigurationUpdateCallback), (override));
	};
}
#endif
