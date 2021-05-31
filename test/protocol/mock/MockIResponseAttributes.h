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

#ifndef _PROTOCOL_MOCK_MOCKIRESPONSEATTRIBUTES_H
#define _PROTOCOL_MOCK_MOCKIRESPONSEATTRIBUTES_H

#include "protocol/IResponseAttributes.h"
#include "../../DefaultValues.h"
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
			ON_CALL(*this, getApplicationId())
				.WillByDefault(testing::ReturnRef(DefaultValues::UTF8_EMPTY_STRING));
			ON_CALL(*this, getStatus())
				.WillByDefault(testing::ReturnRef(DefaultValues::UTF8_EMPTY_STRING));
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

		MOCK_METHOD(int32_t, getMaxBeaconSizeInBytes, (), (const, override));

		MOCK_METHOD(int32_t, getMaxSessionDurationInMilliseconds, (), (const, override));

		MOCK_METHOD(int32_t, getMaxEventsPerSession, (), (const, override));

		MOCK_METHOD(int32_t, getSessionTimeoutInMilliseconds, (), (const, override));

		MOCK_METHOD(int32_t, getSendIntervalInMilliseconds, (), (const, override));

		MOCK_METHOD(int32_t, getVisitStoreVersion, (), (const, override));

		MOCK_METHOD(bool, isCapture, (), (const, override));

		MOCK_METHOD(bool, isCaptureCrashes, (), (const, override));

		MOCK_METHOD(bool, isCaptureErrors, (), (const, override));

		MOCK_METHOD(int32_t, getTrafficControlPercentage, (), (const, override));

		MOCK_METHOD(const core::UTF8String& , getApplicationId, (), (const, override));
		
		MOCK_METHOD(int32_t, getMultiplicity, (), (const, override));

		MOCK_METHOD(int32_t, getServerId, (), (const, override));

		MOCK_METHOD(const core::UTF8String&, getStatus, (), (const, override));

		MOCK_METHOD(int64_t, getTimestampInMilliseconds, (), (const, override));

		MOCK_METHOD(
			bool,
			isAttributeSet,
			(
				protocol::ResponseAttribute /* attribute */
			), 
			(const, override)
		);

		MOCK_METHOD(
			std::shared_ptr<protocol::IResponseAttributes>,
			merge,
			(
				std::shared_ptr<protocol::IResponseAttributes> /* attributes */
			), 
			(const, override)
		);
	};
}

#endif
