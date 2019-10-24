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

#ifndef _TEST_CORE_MOCK_MOCKSESSIONWRAPPER_H
#define _TEST_CORE_MOCK_MOCKSESSIONWRAPPER_H

#include "core/SessionWrapper.h"

#include "gmock/gmock.h"

#include <memory>

namespace test
{
	class MockSessionWrapper
		: public core::SessionWrapper
	{
	public:

		MockSessionWrapper(std::shared_ptr<core::objects::Session> session)
			: SessionWrapper(session)
		{
		}

		static std::shared_ptr<testing::NiceMock<MockSessionWrapper>> createNice(
			std::shared_ptr<core::objects::Session> session
		)
		{
			return std::make_shared<testing::NiceMock<MockSessionWrapper>>(session);
		}

		static std::shared_ptr<testing::StrictMock<MockSessionWrapper>> createStrict(
			std::shared_ptr<core::objects::Session> session
		)
		{
			return std::make_shared<testing::StrictMock<MockSessionWrapper>>(session);
		}

		MOCK_CONST_METHOD0(isDataSendingAllowed, bool());

		MOCK_CONST_METHOD0(canSendNewSessionRequest, bool());

		MOCK_METHOD1(updateBeaconConfiguration, void(std::shared_ptr<core::configuration::IBeaconConfiguration>));

		MOCK_METHOD0(decreaseNumberOfNewSessionRequests, void());

		MOCK_CONST_METHOD0(getBeaconConfiguration, std::shared_ptr<core::configuration::IBeaconConfiguration>());

		MOCK_METHOD1(sendBeacon,
			std::shared_ptr<protocol::IStatusResponse>(
				std::shared_ptr<providers::IHTTPClientProvider>
			)
		);

		MOCK_CONST_METHOD0(isEmpty, bool());

		MOCK_METHOD0(clearCapturedData, void());
	};
}

#endif
