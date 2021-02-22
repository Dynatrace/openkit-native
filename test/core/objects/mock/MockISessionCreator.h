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

#ifndef _TEST_CORE_OBJECTS_MOCK_MOCKISESSIONCREATORINPUT_H
#define _TEST_CORE_OBJECTS_MOCK_MOCKISESSIONCREATORINPUT_H

#include "core/objects/ISessionCreator.h"

#include "gmock/gmock.h"

#include <memory>

namespace test
{
	class MockISessionCreator
		: public core::objects::ISessionCreator
	{
	public:

		~MockISessionCreator() override = default;

		static std::shared_ptr<testing::NiceMock<MockISessionCreator>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockISessionCreator>>();
		}

		static std::shared_ptr<testing::StrictMock<MockISessionCreator>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockISessionCreator>>();
		}


		MOCK_METHOD(
			std::shared_ptr<core::objects::SessionInternals>,
			createSession,
			(std::shared_ptr<core::objects::IOpenKitComposite> parent),
			(override)
		);

		MOCK_METHOD(void, reset, (), (override));
	};
}

#endif
