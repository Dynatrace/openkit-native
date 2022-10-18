/**
 * Copyright 2018-2022 Dynatrace LLC
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

#ifndef _TEST_CORE_OBJECTS_MOCKISUPPLEMENTARYBASICDATA_H
#define _TEST_CORE_OBJECTS_MOCKISUPPLEMENTARYBASICDATA_H

#include "core/objects/ISupplementaryBasicData.h"

#include "gmock/gmock.h"

namespace test
{

	class MockISupplementaryBasicData
		: public core::objects::ISupplementaryBasicData
	{
	public:

		///
		/// Default constructor
		///
		MockISupplementaryBasicData() = default;

		static std::shared_ptr<testing::NiceMock<MockISupplementaryBasicData>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockISupplementaryBasicData>>();
		}

		static std::shared_ptr<testing::StrictMock<MockISupplementaryBasicData>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockISupplementaryBasicData>>();
		}

		MOCK_METHOD(
			void,
			setCarrier,
			(
				const core::UTF8String& /*carrier*/
			),
			(override)
		);

		MOCK_METHOD(
			const core::UTF8String&,
			getCarrier,
			(),
			(override)
		);

		MOCK_METHOD(
			const bool,
			isCarrierAvailable,
			(),
			(override)
		);

		MOCK_METHOD(
			void,
			resetCarrier,
			(),
			(override)
		);

		MOCK_METHOD(
			void,
			setNetworkTechnology,
			(
				const core::UTF8String& /*technology*/
			),
			(override)
		);

		MOCK_METHOD(
			const core::UTF8String&,
			getNetworkTechnology,
			(),
			(override)
		);

		MOCK_METHOD(
			const bool,
			isNetworkTechnologyAvailable,
			(),
			(override)
		);

		MOCK_METHOD(
			void,
			resetNetworkTechnology,
			(),
			(override)
		);

		MOCK_METHOD(
			void,
			setConnectionType,
			(
				const openkit::ConnectionType /*connectionType*/
			),
			(override)
		);

		MOCK_METHOD(
			const openkit::ConnectionType,
			getConnectionType,
			(),
			(override)
		);

		MOCK_METHOD(
			const bool,
			isConnectionTypeAvailable,
			(),
			(override)
		);

		MOCK_METHOD(
			void,
			resetConnectionType,
			(),
			(override)
		);
	};

}

#endif