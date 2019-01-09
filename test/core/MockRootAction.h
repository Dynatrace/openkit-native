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

#ifndef _TEST_CORE_MOCKROOTACTION_H
#define _TEST_CORE_MOCKROOTACTION_H

#include "core/RootAction.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <memory>

using namespace core;
namespace test
{
	class MockRootAction : public RootAction
	{
	public:
		MockRootAction(std::shared_ptr<openkit::ILogger> logger, std::shared_ptr<protocol::Beacon> beacon, std::shared_ptr<core::Session> session)
			: RootAction(logger, beacon, core::UTF8String("Mock Action"), session)
		{
		}

		virtual ~MockRootAction() {}

		MOCK_CONST_METHOD0(getID, int32_t());
	};
}

#endif