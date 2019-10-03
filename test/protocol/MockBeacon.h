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

#include "Types.h"
#include "../api/Types.h"
#include "../core/Types.h"
#include "../core/objects/Types.h"
#include "../core/util/Types.h"
#include "../core/caching/Types.h"
#include "../core/configuration/Types.h"
#include "../protocol/Types.h"
#include "../providers/Types.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

namespace test {
	class MockBeacon : public types::Beacon_t
	{
	public:
		MockBeacon
		(
			types::ILogger_sp logger,
			types::BeaconCache_sp beaconCache,
			types::Configuration_sp configuration,
			const char* clientIPAddress,
			types::IThreadIdProvider_sp threadIDProvider,
			types::ITimingProvider_sp timingProvider
		)
		: types::Beacon_t
		(
			logger,
			beaconCache,
			configuration,
			clientIPAddress,
			threadIDProvider,
			timingProvider
		)
		{
		}

		types::StatusResponse_sp RealSend(types::IHttpClientProvider_sp httpClientProvider)
		{
			return types::Beacon_t::send(httpClientProvider);
		}

		void reportValue(int32_t actionID, const types::Utf8String_t& valueName, int32_t value) override
		{
			reportValueInt32(actionID, valueName, value);
		}

		void reportValue(int32_t actionID, const types::Utf8String_t& valueName, double value) override
		{
			reportValueDouble(actionID, valueName, value);
		}

		void reportValue(
			int32_t actionID,
			const types::Utf8String_t& valueName,
			const types::Utf8String_t& value
		) override
		{
			reportValueString(actionID, valueName, value);
		}

		virtual ~MockBeacon() {}

		MOCK_METHOD1(identifyUser,
			void(
				const types::Utf8String_t& userTag
			)
		);

		MOCK_METHOD2(reportEvent,
			void(
				int32_t,
				const types::Utf8String_t&
			)
		);

		MOCK_METHOD3(reportValueInt32,
			void(
				int32_t,
				const types::Utf8String_t&,
				int32_t
			)
		);

		MOCK_METHOD3(reportValueDouble,
			void(
				int32_t,
				const types::Utf8String_t&,
				double
			)
		);

		MOCK_METHOD3(reportValueString,
			void(
				int32_t,
				const types::Utf8String_t&,
				const types::Utf8String_t&
			)
		);

		MOCK_METHOD4(reportError,
			void(
				int32_t,
				const types::Utf8String_t&,
				int32_t,
				const types::Utf8String_t&
			)
		);

		MOCK_METHOD3(reportCrash,
			void(
				const types::Utf8String_t&,
				const types::Utf8String_t&,
				const types::Utf8String_t&
			)
		);

		MOCK_METHOD2(addWebRequest,
			void(
				int32_t,
				types::WebRequestTracer_sp
			)
		);

		MOCK_METHOD0(startSession, void(void));

		MOCK_METHOD1(endSession,
			void(
				types::Session_sp
			)
		);

		MOCK_CONST_METHOD0(getCurrentTimestamp, int64_t(void));

		MOCK_METHOD1(send,
			types::StatusResponse_sp(
				types::IHttpClientProvider_sp
			)
		);

		MOCK_METHOD2(createTag,
			types::Utf8String_t(
				int32_t,
				int32_t
			)
		);

		MOCK_METHOD0(createSequenceNumber, int32_t());
	};
}
#endif
