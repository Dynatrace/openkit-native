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

#ifndef _TEST_PROVIDERS_MOCK_MOCKIHTTPCLIENTPROVIDER_H
#define _TEST_PROVIDERS_MOCK_MOCKIHTTPCLIENTPROVIDER_H

#include "OpenKit/ILogger.h"
#include "core/configuration/IHTTPClientConfiguration.h"
#include "providers/IHTTPClientProvider.h"
#include "protocol/IHTTPClient.h"

#include "gmock/gmock.h"

#include <memory>

namespace test {
	class MockIHTTPClientProvider
		: public providers::IHTTPClientProvider
	{
	public:
		///
		/// Default constructor
		///
		MockIHTTPClientProvider()
		{
			ON_CALL(*this, createClient(testing::_, testing::_))
				.WillByDefault(testing::Return(nullptr));
		}

		~MockIHTTPClientProvider() override = default;

		static std::shared_ptr<testing::NiceMock<MockIHTTPClientProvider>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockIHTTPClientProvider>>();
		}

		static std::shared_ptr<testing::StrictMock<MockIHTTPClientProvider>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockIHTTPClientProvider>>();
		}

		MOCK_METHOD2(createClient,
			std::shared_ptr<protocol::IHTTPClient>(
				std::shared_ptr<openkit::ILogger>,
				std::shared_ptr<core::configuration::IHTTPClientConfiguration>
			)
		);
	};
}

#endif

