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

#ifndef _TEST_PROTOCOL_MOCKBEACON_H
#define _TEST_PROTOCOL_MOCKBEACON_H

#include "protocol/Beacon.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <memory>

#include "caching/BeaconCache.h"
#include "configuration/Configuration.h"
#include "core/UTF8String.h"
#include "providers/ITimingProvider.h"
#include "providers/IThreadIDProvider.h"

namespace test {
	class MockBeacon : public protocol::Beacon
	{
	public:
		MockBeacon(std::shared_ptr<caching::BeaconCache> beaconCache, std::shared_ptr<configuration::Configuration> configuration, const core::UTF8String clientIPAddress, std::shared_ptr<providers::IThreadIDProvider> threadIDProvider, std::shared_ptr<providers::ITimingProvider> timingProvider)
			: Beacon(beaconCache, configuration, clientIPAddress, threadIDProvider, timingProvider)
		{

		}

		virtual ~MockBeacon() {};

		MOCK_METHOD1(identifyUser, void(const core::UTF8String& userTag));
		MOCK_METHOD3(reportCrash, void(const core::UTF8String&, const core::UTF8String&, const core::UTF8String&));
		MOCK_METHOD1(endSession, void(std::shared_ptr<core::Session>));
		MOCK_CONST_METHOD0(getCurrentTimestamp, int64_t(void));
	};
}
#endif
