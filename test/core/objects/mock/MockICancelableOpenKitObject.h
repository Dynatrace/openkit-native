/**
 * Copyright 2018-2021 Dynatrace LLC
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

#ifndef _TEST_CORE_OBJECTS_MOCKICANCELABLEOPENKITOBJECT_H
#define _TEST_CORE_OBJECTS_MOCKICANCELABLEOPENKITOBJECT_H

#include "core/objects/ICancelableOpenKitObject.h"

#include <gmock/gmock.h>

#include <memory>

namespace test
{

	class MockICancelableOpenKitObject
		: public core::objects::ICancelableOpenKitObject
	{
	public:

		~MockICancelableOpenKitObject() override = default;

		static std::shared_ptr<testing::NiceMock<MockICancelableOpenKitObject>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockICancelableOpenKitObject>>();
		}

		static std::shared_ptr<testing::StrictMock<MockICancelableOpenKitObject>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockICancelableOpenKitObject>>();
		}

		MOCK_METHOD(void, close, (), (override));

		MOCK_METHOD(void, cancel, (), (override));
	};

}

#endif
