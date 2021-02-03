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

#ifndef _TEST_PROTOCOL_MOCK_MOCKIHTTPCLIENT_H
#define _TEST_PROTOCOL_MOCK_MOCKIHTTPCLIENT_H

#include "protocol/IAdditionalQueryParameters.h"
#include "protocol/IHTTPClient.h"
#include "protocol/IStatusResponse.h"

#include "gmock/gmock.h"

#include <memory>

namespace test
{
	class MockIHTTPClient
		: public protocol::IHTTPClient
	{
	public:

		MockIHTTPClient()
		{
			ON_CALL(*this, sendStatusRequest(testing::_))
				.WillByDefault(testing::ReturnNull());
			ON_CALL(*this, sendBeaconRequest(testing::_, testing::_, testing::_))
				.WillByDefault(testing::ReturnNull());
			ON_CALL(*this, sendNewSessionRequest(testing::_))
				.WillByDefault(testing::ReturnNull());
		}

		///
		/// Destructor
		///
		~MockIHTTPClient() override = default;

		static std::shared_ptr<testing::NiceMock<MockIHTTPClient>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockIHTTPClient>>();
		}

		static std::shared_ptr<testing::StrictMock<MockIHTTPClient>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockIHTTPClient>>();
		}

		MOCK_METHOD1(sendStatusRequest,
			std::shared_ptr<protocol::IStatusResponse>(
				const protocol::IAdditionalQueryParameters& /* additionalParameters */
			)
		);

		MOCK_METHOD3(sendBeaconRequest,
			std::shared_ptr<protocol::IStatusResponse>(
				const core::UTF8String&, /* clientIPAddress */
				const core::UTF8String&, /* beaconData */
				const protocol::IAdditionalQueryParameters& /* additionalParameters */
			)
		);

		MOCK_METHOD1(sendNewSessionRequest,
			std::shared_ptr<protocol::IStatusResponse>(
				const protocol::IAdditionalQueryParameters& /* additionalParameters */
			)
		);
	};
}

#endif
