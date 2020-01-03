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

#ifndef _PROTOCOL_MOCK__MOCKIRESPONSEATTRIBUTES_H
#define _PROTOCOL_MOCK_MOCKIRESPONSEATTRIBUTES_H

#include "protocol/IResponseAttributes.h"

#include "gmock/gmock.h"

#include <memory>

namespace test
{
	class MockIResponseAttributes
		: public protocol::IResponseAttributes

	{
	public:

		MockIResponseAttributes()
		{
			ON_CALL(*this, merge(testing::_)).WillByDefault(testing::Return(nullptr));
		}

		~MockIResponseAttributes() override = default;

		static std::shared_ptr<testing::NiceMock<MockIResponseAttributes>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockIResponseAttributes>>();
		}

		static std::shared_ptr<testing::StrictMock<MockIResponseAttributes>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockIResponseAttributes>>();
		}

		MOCK_CONST_METHOD0(getMaxBeaconSizeInBytes, int32_t());

		MOCK_CONST_METHOD0(getMaxSessionDurationInMilliseconds, int32_t());

		MOCK_CONST_METHOD0(getMaxEventsPerSession, int32_t());

		MOCK_CONST_METHOD0(getSessionTimeoutInMilliseconds, int32_t());

		MOCK_CONST_METHOD0(getSendIntervalInMilliseconds, int32_t());

		MOCK_CONST_METHOD0(getVisitStoreVersion, int32_t());

		MOCK_CONST_METHOD0(isCapture, bool());

		MOCK_CONST_METHOD0(isCaptureCrashes, bool());

		MOCK_CONST_METHOD0(isCaptureErrors, bool());

		MOCK_CONST_METHOD0(getMultiplicity, int32_t());

		MOCK_CONST_METHOD0(getServerId, int32_t());

		MOCK_CONST_METHOD0(getTimestampInMilliseconds, int64_t());

		MOCK_CONST_METHOD1(isAttributeSet,
			bool (
				protocol::ResponseAttribute /* attribute */
			)
		);

		MOCK_CONST_METHOD1(merge,
			std::shared_ptr<protocol::IResponseAttributes>(
				std::shared_ptr<protocol::IResponseAttributes> /* attributes */
			)
		);
	};
}

#endif
