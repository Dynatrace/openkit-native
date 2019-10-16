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

#ifndef _TEST_CORE_OBJECTS_MOCKOPENKITCOMPOSITE_H
#define _TEST_CORE_OBJECTS_MOCKOPENKITCOMPOSITE_H

#include "core/objects/IOpenKitComposite.h"
#include "core/objects/IOpenKitObject.h"

#include "gmock/gmock.h"

#include <list>
#include <memory>

namespace test
{
class MockIOpenKitComposite : public core::objects::IOpenKitComposite
	{
	public:
		MockIOpenKitComposite()
		{
		}

		~MockIOpenKitComposite()
		{
		}

		static std::shared_ptr<testing::NiceMock<MockIOpenKitComposite>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockIOpenKitComposite>>();
		}

		static std::shared_ptr<testing::StrictMock<MockIOpenKitComposite>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockIOpenKitComposite>>();
		}

		MOCK_METHOD1(storeChildInList,
			void(
				std::shared_ptr<core::objects::IOpenKitObject>
			)
		);

		MOCK_METHOD1(removeChildFromList,
			void(
				std::shared_ptr<core::objects::IOpenKitObject>
			)
		);

		MOCK_METHOD0(getCopyOfChildObjects, std::list<std::shared_ptr<core::objects::IOpenKitObject>>());

		MOCK_METHOD1(onChildClosed,
			void(
				const std::shared_ptr<core::objects::IOpenKitObject>
			)
		);

		MOCK_CONST_METHOD0(getActionId, int32_t());

		MOCK_METHOD0(close, void());
	};
}

#endif
