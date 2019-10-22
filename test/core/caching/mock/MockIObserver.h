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

#ifndef _TEST_CORE_CACHING_MOCK_MOCKOBSERVER_H
#define _TEST_CORE_CACHING_MOCK_MOCKOBSERVER_H

#include "core/caching/IObserver.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <memory>

namespace test
{
	class MockIObserver
		: public core::caching::IObserver
	{
	public:
		MockIObserver()
		{
		}

		virtual ~MockIObserver() {}

		static std::shared_ptr<testing::NiceMock<MockIObserver>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockIObserver>>();
		}

		static std::shared_ptr<testing::StrictMock<MockIObserver>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockIObserver>>();
		}

		MOCK_METHOD0(update, void());
	};
}
#endif
