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

#ifndef _TEST_CORE_OBJECTS_MOCKACTION_H
#define _TEST_CORE_OBJECTS_MOCKACTION_H

#include "Types.h"
#include "../../api/Types.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <memory>

namespace test
{
	class MockLeafAction : public types::LeafAction_t
	{
	public:
		MockLeafAction
		(
			types::IActionCommon_sp logger,
			types::IRootAction_sp parentAction
		)
		: types::LeafAction_t
		(
			logger,
			parentAction
		)
		{
		}

		virtual ~MockLeafAction() {}

		MOCK_CONST_METHOD0(getID, int32_t());
	};
}

#endif