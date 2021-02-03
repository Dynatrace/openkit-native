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

#include "mock/MockAbstractBeaconSendingState.h"
#include "mock/MockIBeaconSendingContext.h"
#include "mock/MockIBeaconSendingState.h"

#include "core/communication/AbstractBeaconSendingState.h"
#include "core/communication/IBeaconSendingContext.h"
#include "core/communication/IBeaconSendingState.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace test;


class AbstractBeaconSendingStateTest : public testing::Test
{
};

TEST_F(AbstractBeaconSendingStateTest, aTestBeaconSendingStateExecutes)
{
	// with
	auto mockContext = MockIBeaconSendingContext::createStrict();

	// expect
	EXPECT_CALL(*mockContext, isShutdownRequested())
		.WillRepeatedly(testing::Return(false));

	// given
	auto target = MockAbstractBeaconSendingState::createStrict();

	// expect
	EXPECT_CALL(*target, doExecute(testing::Ref(*mockContext)))
		.Times(1);

	// when
	target->execute(*mockContext);

}

TEST_F(AbstractBeaconSendingStateTest, aTestBeaconSendingSetsNextStateAsShutdownState)
{
	// with
	auto mockShutdownState = MockIBeaconSendingState::createNice();
	auto mockContext = MockIBeaconSendingContext::createStrict();

	// expect
	EXPECT_CALL(*mockContext, isShutdownRequested())
		.Times(testing::Exactly(1))
		.WillOnce(testing::Return(true));
	EXPECT_CALL(*mockContext, setNextState(testing::Eq(mockShutdownState)))
		.Times(1);

	// given
	auto target = MockAbstractBeaconSendingState::createStrict();

	// expect
	EXPECT_CALL(*target, getShutdownState())
		.Times(testing::Exactly(1))
		.WillOnce(testing::Return(mockShutdownState));
	EXPECT_CALL(*target, doExecute(testing::_))
		.Times(1);

	// when
	target->execute(*mockContext);
}


