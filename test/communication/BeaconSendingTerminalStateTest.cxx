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

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "communication/BeaconSendingTerminalState.h"
#include "communication/AbstractBeaconSendingState.h"
#include "../protocol/NullLogger.h"

#include "MockBeaconSendingContext.h"

class BeaconSendingTerminalStateTest : public testing::Test
{
public:

	BeaconSendingTerminalStateTest()
		: mLogger(nullptr)
		, mTarget(nullptr)
	{
	}

	void SetUp()
	{
		mLogger = std::make_shared<NullLogger>();
		mTarget = std::make_shared<BeaconSendingTerminalState>();
	}

	void TearDown()
	{
		mLogger = nullptr;
		mTarget = nullptr;
	}

	std::shared_ptr<openkit::ILogger> mLogger;
	std::shared_ptr<BeaconSendingTerminalState> mTarget;
};

TEST_F(BeaconSendingTerminalStateTest, isTerminalStateIsTrueForTheTerminalState)
{
	EXPECT_TRUE(mTarget->isTerminalState());
}

TEST_F(BeaconSendingTerminalStateTest, theShutdownStateIsNeverTheSameReference)
{
	std::shared_ptr<AbstractBeaconSendingState> obtained = mTarget->getShutdownState();

	ASSERT_NE(obtained, nullptr);
	EXPECT_NE(mTarget.get(), obtained.get());
}

TEST_F(BeaconSendingTerminalStateTest, executeRequestsShutdown)
{
	testing::StrictMock<test::MockBeaconSendingContext> mockContext(mLogger);//StrictMock ensure that  all additional calls on context result in failure

	EXPECT_CALL(mockContext, requestShutdown())
		.Times(::testing::Exactly(1));

	EXPECT_CALL(mockContext, isShutdownRequested())
		.Times(::testing::Exactly(1));

	mTarget->execute(mockContext);
}

TEST_F(BeaconSendingTerminalStateTest, ToStringReturnsCorrectStateName)
{
	// given
	communication::BeaconSendingTerminalState target;

	// when
	auto stateName = target.getStateName();

	// then
	ASSERT_STREQ(stateName, "Terminal");
}