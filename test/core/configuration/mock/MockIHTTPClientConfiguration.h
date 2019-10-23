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

#ifndef _TEST_CORE_CONFIGURATION_MOCK_MOCKIHTTPCLIENTCONFIGURATION_H
#define _TEST_CORE_CONFIGURATION_MOCK_MOCKIHTTPCLIENTCONFIGURATION_H

#include "../../../DefaultValues.h"

#include "OpenKit/ISSLTrustManager.h"
#include "core/UTF8String.h"
#include "core/configuration/IHTTPClientConfiguration.h"

#include "gmock/gmock.h"

#include <cstdint>
#include <memory>

namespace test
{
	class MockIHTTPClientConfiguration
		: public core::configuration::IHTTPClientConfiguration
	{
	public:

		MockIHTTPClientConfiguration()
		{
			ON_CALL(*this, getBaseURL())
				.WillByDefault(testing::Return(DefaultValues::UTF8_EMPTY_STRING));
			ON_CALL(*this, getApplicationID())
				.WillByDefault(testing::Return(DefaultValues::UTF8_EMPTY_STRING));
			ON_CALL(*this, getSSLTrustManager())
				.WillByDefault(testing::Return(nullptr));
		}

		virtual ~MockIHTTPClientConfiguration() {}

		MOCK_CONST_METHOD0(getBaseURL, const core::UTF8String&());

		MOCK_CONST_METHOD0(getServerID, int32_t());

		MOCK_CONST_METHOD0(getApplicationID, const core::UTF8String&());

		MOCK_CONST_METHOD0(getSSLTrustManager, std::shared_ptr<openkit::ISSLTrustManager>());
	};
}

#endif
