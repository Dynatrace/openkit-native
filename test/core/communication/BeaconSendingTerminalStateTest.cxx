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

#include "core/communication/BeaconSendingTerminalState.h"

#include "mock/MockIBeaconSendingContext.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace test;

using BeaconSendingTerminalState_t = core::communication::BeaconSendingTerminalState;

class BeaconSendingTerminalStateTest : public testing::Test
{
};

TEST_F(BeaconSendingTerminalStateTest, isTerminalStateIsTrueForTheTerminalState)
{
	// given
	BeaconSendingTerminalState_t target;

	// when
	auto obtained = target.isTerminalState();

	// then
	ASSERT_THAT(obtained, testing::Eq(true));
}

TEST_F(BeaconSendingTerminalStateTest, theShutdownStateIsNeverTheSameReference)
{
	// given
	auto target = std::make_shared<BeaconSendingTerminalState_t>();

	//
	auto obtained = target->getShutdownState();

	// then
	ASSERT_THAT(obtained, testing::NotNull());
	ASSERT_THAT(obtained.get(), testing::Ne(target.get()));
}


TEST_F(BeaconSendingTerminalStateTest, getStateNameReturnsTheStateName)
{
	// given
	BeaconSendingTerminalState_t target;

	// when
	auto obtained = target.getStateName();

	// then
	ASSERT_THAT(obtained, testing::StrEq("Terminal"));
}

TEST_F(BeaconSendingTerminalStateTest, executeRequestsShutdown)
{
	// with
	auto mockContext = MockIBeaconSendingContext::createStrict();

	// expect
	EXPECT_CALL(*mockContext, requestShutdown())
		.Times(::testing::Exactly(1));

	EXPECT_CALL(*mockContext, isShutdownRequested())
		.Times(::testing::Exactly(1));

	// given
	BeaconSendingTerminalState_t target;

	// when
	target.execute(*mockContext);
}
