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

#ifndef _TEST_COMMUNICATION_MOCKTYPES_H
#define _TEST_COMMUNICATION_MOCKTYPES_H

#include "MockAbstractBeaconSendingState.h"
#include "MockBeaconSendingContext.h"

#include "gmock/gmock.h"

#include <memory>

namespace test
{
	namespace types
	{
		using MockAbstractBeaconSendingState_t = MockAbstractBeaconSendingState;
		using MockNiceAbstractBeaconSendingState_t = testing::NiceMock<MockAbstractBeaconSendingState_t>;
		using MockNiceAbstractBeaconSendingState_sp = std::shared_ptr<MockNiceAbstractBeaconSendingState_t>;
		using MockStrictAbstractBeaconSendingState_t = testing::StrictMock<MockAbstractBeaconSendingState_t>;
		using MockStrictAbstractBeaconSendingState_sp = std::shared_ptr<MockStrictAbstractBeaconSendingState_t>;

		using MockBeaconSendingContext_t = MockBeaconSendingContext;
		using MockNiceBeaconSendingContext_t = testing::NiceMock<MockBeaconSendingContext_t>;
		using MockNiceBeaconSendingContext_sp = std::shared_ptr<MockNiceBeaconSendingContext_t>;
		using MockStrictBeaconSendingContext_t = testing::StrictMock<MockBeaconSendingContext_t>;
		using MockStrictBeaconSendingContext_sp = std::shared_ptr<MockStrictBeaconSendingContext_t>;
	}
}

#endif
