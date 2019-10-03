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

#ifndef _TEST_PROTOCOL_MOCKTYPES_H
#define _TEST_PROTOCOL_MOCKTYPES_H

#include "MockBeacon.h"
#include "MockHTTPClient.h"
#include "MockStatusResponse.h"

#include "gmock/gmock.h"

namespace test
{
	namespace types
	{
		using MockBeacon_t = MockBeacon;
		using MockNiceBeacon_t = testing::NiceMock<MockBeacon_t>;
		using MockNiceBeacon_sp = std::shared_ptr<MockNiceBeacon_t>;
		using MockStrictBeacon_t = testing::StrictMock<MockBeacon_t>;
		using MockStrictBeacon_sp = std::shared_ptr<MockStrictBeacon_t>;

		using MockHttpClient_t = MockHTTPClient;
		using MockNiceHttpClient_t = testing::NiceMock<MockHttpClient_t>;
		using MockNiceHttpClient_sp = std::shared_ptr<MockNiceHttpClient_t>;
		using MockStrictHttpClient_t = testing::StrictMock<MockHttpClient_t>;
		using MockStrictHttpClient_sp = std::shared_ptr<MockStrictHttpClient_t>;

		using MockStatusResponse_t = MockStatusResponse;
		using MockNiceStatusResponse_t = testing::NiceMock<MockStatusResponse_t>;
		using MockNiceStatusResponse_sp = std::shared_ptr<MockNiceStatusResponse_t>;
	}
}

#endif
