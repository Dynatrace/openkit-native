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

#ifndef _TEST_CORE_OBJECTS_MOCK_MOCKISESSIONPROXY_H
#define _TEST_CORE_OBJECTS_MOCK_MOCKISESSIONPROXY_H

#include "core/objects/ISessionProxy.h"

#include <memory>

#include "gmock/gmock.h"

namespace test
{
	class MockISessionProxy : public core::objects::ISessionProxy
	{
	public:

		~MockISessionProxy() override = default;

		static std::shared_ptr<testing::NiceMock<MockISessionProxy>> createNice()
		{
			return std::make_shared<testing::NiceMock<MockISessionProxy>>();
		}

		static std::shared_ptr<testing::StrictMock<MockISessionProxy>> createStrict()
		{
			return std::make_shared<testing::StrictMock<MockISessionProxy>>();
		}

		MOCK_METHOD(
			std::shared_ptr<openkit::IRootAction>,
			enterAction,
			(
				const char* // actionName
			),
			(override)
		);

		MOCK_METHOD(
			void,
			identifyUser,
			(
				const char* //userTag
			),
			(override)
		);

		MOCK_METHOD(
			void,
			sendBizEvent,
			(
				const char*, // type
				(const openkit::json::JsonObjectValue::JsonObjectMapPtr) /*attributes*/
				),
			(override)
		);

		MOCK_METHOD(
			void,
			sendEvent,
			(
				const char*, // name
				(const openkit::json::JsonObjectValue::JsonObjectMapPtr) /*attributes*/
			),
			(override)
		);

		MOCK_METHOD(
			void,
			reportCrash,
			(
				const char*, // errorName
				const char*, // reason
				const char*  //stacktrace
			),
			(override)
		);

		MOCK_METHOD(
			std::shared_ptr<openkit::IWebRequestTracer>,
			traceWebRequest,
			(
				const char* // url
			),
			(override)
		);

		MOCK_METHOD(void, end, (), (override));

		MOCK_METHOD(bool, isFinished, (), (override));

		MOCK_METHOD(
			void,
			onServerConfigurationUpdate,
			(
				std::shared_ptr<core::configuration::IServerConfiguration> //serverConfig
			),
			(override)
		);

		MOCK_METHOD(int64_t, splitSessionByTime, (), (override));
	};
}

#endif
