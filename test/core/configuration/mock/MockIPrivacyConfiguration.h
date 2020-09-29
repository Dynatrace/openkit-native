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

#ifndef _TEST_CORE_CONFIGURATION_MOCK_MOCKIPRIVACYCONFIGURATION_H
#define _TEST_CORE_CONFIGURATION_MOCK_MOCKIPRIVACYCONFIGURATION_H

#include "OpenKit/CrashReportingLevel.h"
#include "OpenKit/DataCollectionLevel.h"
#include "core/configuration/ConfigurationDefaults.h"
#include "core/configuration/IPrivacyConfiguration.h"

#include "gmock/gmock.h"

#include <memory>

namespace test
{
	class MockIPrivacyConfiguration
		: public core::configuration::IPrivacyConfiguration
	{
	public:

		MockIPrivacyConfiguration()
		{
			ON_CALL(*this, getDataCollectionLevel())
				.WillByDefault(testing::Return(core::configuration::DEFAULT_DATA_COLLECTION_LEVEL));
			ON_CALL(*this, getCrashReportingLevel())
				.WillByDefault(testing::Return(core::configuration::DEFAULT_CRASH_REPORTING_LEVEL));

			ON_CALL(*this, isDeviceIdSendingAllowed()).WillByDefault(testing::Return(true));
			ON_CALL(*this, isSessionNumberReportingAllowed()).WillByDefault(testing::Return(true));
			ON_CALL(*this, isWebRequestTracingAllowed()).WillByDefault(testing::Return(true));
			ON_CALL(*this, isSessionReportingAllowed()).WillByDefault(testing::Return(true));
			ON_CALL(*this, isActionReportingAllowed()).WillByDefault(testing::Return(true));
			ON_CALL(*this, isValueReportingAllowed()).WillByDefault(testing::Return(true));
			ON_CALL(*this, isEventReportingAllowed()).WillByDefault(testing::Return(true));
			ON_CALL(*this, isErrorReportingAllowed()).WillByDefault(testing::Return(true));
			ON_CALL(*this, isCrashReportingAllowed()).WillByDefault(testing::Return(true));
			ON_CALL(*this, isUserIdentificationAllowed()).WillByDefault(testing::Return(true));
		}

		~MockIPrivacyConfiguration() override = default;

		static std::shared_ptr<testing::NiceMock<MockIPrivacyConfiguration>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockIPrivacyConfiguration>>();
		}

		static std::shared_ptr<testing::StrictMock<MockIPrivacyConfiguration>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockIPrivacyConfiguration>>();
		}

		MOCK_CONST_METHOD0(getDataCollectionLevel, openkit::DataCollectionLevel());

		MOCK_CONST_METHOD0(getCrashReportingLevel, openkit::CrashReportingLevel());

		MOCK_CONST_METHOD0(isDeviceIdSendingAllowed, bool());

		MOCK_CONST_METHOD0(isSessionNumberReportingAllowed, bool());

		MOCK_CONST_METHOD0(isWebRequestTracingAllowed, bool());

		MOCK_CONST_METHOD0(isSessionReportingAllowed, bool());

		MOCK_CONST_METHOD0(isActionReportingAllowed, bool());

		MOCK_CONST_METHOD0(isValueReportingAllowed, bool());

		MOCK_CONST_METHOD0(isEventReportingAllowed, bool());

		MOCK_CONST_METHOD0(isErrorReportingAllowed, bool());

		MOCK_CONST_METHOD0(isCrashReportingAllowed, bool());

		MOCK_CONST_METHOD0(isUserIdentificationAllowed, bool());
	};
}

#endif
