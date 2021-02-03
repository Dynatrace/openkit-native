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

#ifndef _TEST_API_MOCK_MOCKISSLTRUSTMANAGER_H
#define _TEST_API_MOCK_MOCKISSLTRUSTMANAGER_H

#include "OpenKit/ISSLTrustManager.h"

#include "curl/curl.h"
#include "gmock/gmock.h"

#include <memory>

namespace test
{
	class MockISslTrustManager
		: public openkit::ISSLTrustManager
	{
	public:

		~MockISslTrustManager() override = default;

		static std::shared_ptr<testing::NiceMock<MockISslTrustManager>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockISslTrustManager>>();
		}

		static std::shared_ptr<testing::StrictMock<MockISslTrustManager>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockISslTrustManager>>();
		}

		MOCK_METHOD1(applyTrustManager, void(CURL*));
	};
}

#endif
