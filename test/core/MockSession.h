/**
* Copyright 2018 Dynatrace LLC
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

#ifndef _TEST_CORE_MOCKSESSION_H
#define _TEST_CORE_MOCKSESSION_H

#include "core/Session.h"
#include "core/util/DefaultLogger.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <memory>

using namespace core;
namespace test
{
	class MockSession : public Session
	{
	public:
		MockSession()
			: Session(
				std::shared_ptr<api::ILogger>(new core::util::DefaultLogger(devNull, true)),
				std::shared_ptr<BeaconSender>(),
				std::shared_ptr<protocol::Beacon>())
		{
		}

		virtual std::unique_ptr<protocol::StatusResponse> sendBeacon(std::shared_ptr<providers::IHTTPClientProvider> clientProvider)
		{
			return std::unique_ptr<protocol::StatusResponse>(sendBeaconRawPtrProxy(clientProvider));
		}

		MOCK_METHOD1(enterAction, std::shared_ptr<api::IRootAction>(const char*));
		MOCK_METHOD0(end, void());
		MOCK_METHOD1(sendBeaconRawPtrProxy, protocol::StatusResponse*(std::shared_ptr<providers::IHTTPClientProvider>));
		MOCK_CONST_METHOD0(isEmpty, bool());
		MOCK_METHOD0(clearCapturedData, void());

	private:
		std::ostringstream devNull;
	};
}

#endif