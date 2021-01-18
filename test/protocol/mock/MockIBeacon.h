/**
 * Copyright 2018-2020 Dynatrace LLC
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

		MOCK_METHOD0(createSequenceNumber, int32_t());

		MOCK_CONST_METHOD0(getCurrentTimestamp, int64_t());

		MOCK_CONST_METHOD0(getSessionStartTime, int64_t());

		MOCK_METHOD0(createID, int32_t());

		MOCK_METHOD2(createTag,
			core::UTF8String(
				int32_t, /* actionID */
				int32_t /* sequenceNumber */
			)
		);

		MOCK_METHOD1(addAction,
			void(
				std::shared_ptr<core::objects::IActionCommon>
			)
		);

		MOCK_METHOD0(startSession, void());

		MOCK_METHOD0(endSession, void());

		MOCK_METHOD3(reportValue,
			void(
				int32_t, /* actionID */
				const core::UTF8String&, /* valueName */
				int32_t /* value */
			)
		);

		MOCK_METHOD3(reportValue,
			void(
				int32_t, /* actionID */
				const core::UTF8String&, /* valueName */
				int64_t /* value */
			)
		);

		MOCK_METHOD3(reportValue,
			void(
				int32_t, /* actionID */
				const core::UTF8String&, /* valueName */
				double /* value */
			)
		);

		MOCK_METHOD3(reportValue,
			void(
				int32_t, /* actionID */
				const core::UTF8String&, /* valueName */
				const core::UTF8String& /* value */
			)
		);

		MOCK_METHOD2(reportEvent,
			void(
				int32_t, /* actionID */
				const core::UTF8String& /* eventName */
			)
		);

		MOCK_METHOD4(reportError,
			void(
				int32_t, /* actionID */
				const core::UTF8String&, /* errorName */
				int32_t, /* errorCode */
				const core::UTF8String& /* reason */
			)
		);

		MOCK_METHOD3(reportCrash,
			void(
				const core::UTF8String&, /* errorName */
				const core::UTF8String&, /* reason */
				const core::UTF8String& /* stacktrace */
			)
		);

		MOCK_METHOD2(addWebRequest,
			void(
				int32_t, /* parentActionID */
				std::shared_ptr<core::objects::IWebRequestTracerInternals> /* webRequestTracer */
			)
		);

		MOCK_METHOD1(identifyUser,
			void(
				const core::UTF8String& /* userTag */
			)
		);

		MOCK_METHOD2(send,
			std::shared_ptr<protocol::IStatusResponse>(
				std::shared_ptr<providers::IHTTPClientProvider>, /* clientProvider */
				const protocol::IAdditionalQueryParameters& /* additionalParameters */
			)
		);

		MOCK_CONST_METHOD0(isEmpty, bool());

		MOCK_METHOD0(clearData, void());

		MOCK_CONST_METHOD0(getSessionNumber, int32_t());

		MOCK_CONST_METHOD0(getSessionSequenceNumber, int32_t());

		MOCK_CONST_METHOD0(getDeviceID, int64_t());

		MOCK_CONST_METHOD0(useClientIPAddress, bool());

		MOCK_CONST_METHOD0(getClientIPAddress, core::UTF8String&());

		MOCK_METHOD1(initializeServerConfiguration,
			void(
				std::shared_ptr<core::configuration::IServerConfiguration> /* serverConfiguration */
			)
		);

		MOCK_METHOD1(updateServerConfiguration,
			void(
				std::shared_ptr<core::configuration::IServerConfiguration> /* serverConfig */
			)
		);

		MOCK_METHOD0(isServerConfigurationSet, bool());

		MOCK_METHOD0(isActionReportingAllowedByPrivacySettings, bool());

		MOCK_METHOD0(isDataCapturingEnabled, bool());

		MOCK_METHOD0(enableCapture, void());

		MOCK_METHOD0(disableCapture, void());

		MOCK_METHOD1(setServerConfigurationUpdateCallback, void(core::configuration::ServerConfigurationUpdateCallback));
	};
}
#endif
