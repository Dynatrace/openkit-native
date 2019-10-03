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

#ifndef _TEST_CORE_MOCKTYPES_H
#define _TEST_CORE_MOCKTYPES_H

#include "MockBeaconSender.h"

#include "gmock/gmock.h"

#include <memory>

namespace test
{
	namespace types
	{
		using MockBeaconSender_t = test::MockBeaconSender;
		using MockStrictBeaconSender_t = testing::StrictMock<MockBeaconSender_t>;
		using MockStrictBeaconSender_sp = std::shared_ptr<MockStrictBeaconSender_t>;
	}
}

#endif
