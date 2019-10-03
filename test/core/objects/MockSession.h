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

#ifndef _TEST_CORE_OBJECTS_MOCKSESSION_H
#define _TEST_CORE_OBJECTS_MOCKSESSION_H

#include "Types.h"
#include "../Types.h"
#include "../configuration/Types.h"
#include "../../api/Types.h"
#include "../../protocol/Types.h"
#include "../../providers/Types.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <memory>

namespace test
{
	class MockSession : public types::Session_t
	{
	public:
		MockSession
		(
			types::ILogger_sp logger
		)
		: Session
		(
			logger,
			types::BeaconSender_sp(),
			types::Beacon_sp()
		)
		{
		}

		virtual types::StatusResponse_sp sendBeacon(types::IHttpClientProvider_sp clientProvider)
		{
			return types::StatusResponse_sp(sendBeaconRawPtrProxy(clientProvider));
		}

		MOCK_METHOD1(enterAction,
			types::IRootAction_sp(
					const char*
			)
		);

		MOCK_METHOD0(end, void());

		MOCK_METHOD1(sendBeaconRawPtrProxy,
			types::StatusResponse_sp(
				types::IHttpClientProvider_sp
			)
		);

		MOCK_CONST_METHOD0(isEmpty, bool());

		MOCK_METHOD0(clearCapturedData, void());

		MOCK_CONST_METHOD0(getEndTime, int64_t());

		MOCK_METHOD1(setBeaconConfiguration,
			void(
				types::BeaconConfiguration_sp
			)
		);

		MOCK_CONST_METHOD0(getBeaconConfiguration, types::BeaconConfiguration_sp());
	};
}

#endif