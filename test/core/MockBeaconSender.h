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

#ifndef _TEST_CORE_MOCKBEACONSENDER_H
#define _TEST_CORE_MOCKBEACONSENDER_H

#include "Types.h"
#include "configuration/Types.h"
#include "objects/Types.h"
#include "util/Types.h"
#include "../api/Types.h"
#include "../providers/Types.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <memory>

namespace test {
class MockBeaconSender : public types::BeaconSender_t
	{
	public:
		MockBeaconSender
		(
			types::ILogger_sp logger,
			types::Configuration_sp configuration,
			types::IHttpClientProvider_sp httpClientProvider,
			types::ITimingProvider_sp timingProvider
		)
		: BeaconSender
		(
			logger,
			configuration,
			httpClientProvider,
			timingProvider
		)
		{
		}

		virtual ~MockBeaconSender() {}

		MOCK_METHOD1(startSession, void(types::Session_sp));
		MOCK_METHOD1(finishSession, void(types::Session_sp));
	};
}
#endif
