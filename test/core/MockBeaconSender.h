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

#ifndef _TEST_CORE_MOCKBEACONSENDER_H
#define _TEST_CORE_MOCKBEACONSENDER_H

#include "core/BeaconSender.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <memory>

#include "core/util/DefaultLogger.h"
#include "core/Session.h"
#include "providers/IHTTPClientProvider.h"
#include "providers/ITimingProvider.h"
#include "configuration/Configuration.h"

namespace test {
	class MockBeaconSender : public core::BeaconSender
	{
	public:
		MockBeaconSender(std::shared_ptr<configuration::Configuration> configuration,
			std::shared_ptr<providers::IHTTPClientProvider> httpClientProvider,
			std::shared_ptr<providers::ITimingProvider> timingProvider)
			: BeaconSender(std::shared_ptr<api::ILogger>(new core::util::DefaultLogger(devNull, true)), configuration,  httpClientProvider, timingProvider)
		{

		}

		virtual ~MockBeaconSender() {}

		MOCK_METHOD1(startSession, void(std::shared_ptr<core::Session>));
		MOCK_METHOD1(finishSession, void(std::shared_ptr<core::Session>));

	private:
		std::ostringstream devNull;
	};
}
#endif
