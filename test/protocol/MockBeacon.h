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

		std::unique_ptr<protocol::StatusResponse> RealSend(std::shared_ptr<providers::IHTTPClientProvider> httpClientProvider)
		{
			return protocol::Beacon::send(httpClientProvider);
		}

		/// for actions

		void reportEvent(std::shared_ptr<core::Action> parentAction, const core::UTF8String& eventName)
		{
			reportEventGivenAction(parentAction, eventName);
		}

		void reportValue(std::shared_ptr<core::Action> parentAction, const core::UTF8String& valueName, int32_t value) override
		{
			reportValueInt32GivenAction(parentAction, valueName, value);
		}

		void reportValue(std::shared_ptr<core::Action> parentAction, const core::UTF8String& valueName, double value) override
		{
			reportValueDoubleGivenAction(parentAction, valueName, value);
		}

		void reportValue(std::shared_ptr<core::Action> parentAction, const core::UTF8String& valueName, const core::UTF8String& value) override
		{
			reportValueStringGivenAction(parentAction, valueName, value);
		}

		void reportError(std::shared_ptr<core::Action> parentAction, const core::UTF8String& errorName, int32_t reason, const core::UTF8String& stacktrace)
		{
			reportErrorGivenAction(parentAction, errorName, reason, stacktrace);
		}

		// for root actions

		void reportEvent(std::shared_ptr<core::RootAction> parentAction, const core::UTF8String& eventName)
		{
			reportEventGivenRootAction(parentAction, eventName);
		}

		void reportValue(std::shared_ptr<core::RootAction> parentAction, const core::UTF8String& valueName, int32_t value) override
		{
			reportValueInt32GivenRootAction(parentAction, valueName, value);
		}

		void reportValue(std::shared_ptr<core::RootAction> parentAction, const core::UTF8String& valueName, double value) override
		{
			reportValueDoubleGivenRootAction(parentAction, valueName, value);
		}

		void reportValue(std::shared_ptr<core::RootAction> parentAction, const core::UTF8String& valueName, const core::UTF8String& value) override
		{
			reportValueStringGivenRootAction(parentAction, valueName, value);
		}

		void reportError(std::shared_ptr<core::RootAction> parentAction, const core::UTF8String& errorName, int32_t reason, const core::UTF8String& stacktrace)
		{
			reportErrorGivenRootAction(parentAction, errorName, reason, stacktrace);
		}

		virtual ~MockBeacon() {}

		MOCK_METHOD1(identifyUser, void(const core::UTF8String& userTag));
		MOCK_METHOD2(reportEventGivenAction, void(std::shared_ptr<core::Action>, const core::UTF8String&));
		MOCK_METHOD3(reportValueInt32GivenAction, void(std::shared_ptr<core::Action>, const core::UTF8String&, int32_t));
		MOCK_METHOD3(reportValueDoubleGivenAction, void(std::shared_ptr<core::Action>, const core::UTF8String&, double));
		MOCK_METHOD3(reportValueStringGivenAction, void(std::shared_ptr<core::Action>, const core::UTF8String&, const core::UTF8String&));
		MOCK_METHOD4(reportErrorGivenAction, void(std::shared_ptr<core::Action>, const core::UTF8String&, int32_t, const core::UTF8String&));
		MOCK_METHOD2(reportEventGivenRootAction, void(std::shared_ptr<core::RootAction>, const core::UTF8String&));
		MOCK_METHOD3(reportValueInt32GivenRootAction, void(std::shared_ptr<core::RootAction>, const core::UTF8String&, int32_t));
		MOCK_METHOD3(reportValueDoubleGivenRootAction, void(std::shared_ptr<core::RootAction>, const core::UTF8String&, double));
		MOCK_METHOD3(reportValueStringGivenRootAction, void(std::shared_ptr<core::RootAction>, const core::UTF8String&, const core::UTF8String&));
		MOCK_METHOD4(reportErrorGivenRootAction, void(std::shared_ptr<core::RootAction>, const core::UTF8String&, int32_t, const core::UTF8String&));
		MOCK_METHOD3(reportCrash, void(const core::UTF8String&, const core::UTF8String&, const core::UTF8String&));
		MOCK_METHOD1(endSession, void(std::shared_ptr<core::Session>));
		MOCK_CONST_METHOD0(getCurrentTimestamp, int64_t(void));
		MOCK_METHOD1(send, std::unique_ptr<protocol::StatusResponse>(std::shared_ptr<providers::IHTTPClientProvider>));
	};
}
#endif
