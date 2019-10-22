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

#include "Types.h"
#include "MockTypes.h"
#include "../../api/Types.h"
#include "../../api/mock/MockILogger.h"
#include "../../protocol/Types.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace test;
using namespace test::types;

class BeaconSendingTerminalStateTest : public testing::Test
{
protected:

	BeaconSendingTerminalStateTest()
		: mLogger(nullptr)
		, mTarget(nullptr)
	{
	}

	void SetUp()
	{
		mLogger = MockILogger::createNice();
		mTarget = std::make_shared<BeaconSendingTerminalState_t>();
	}

	void TearDown()
	{
		mLogger = nullptr;
		mTarget = nullptr;
	}

	ILogger_sp mLogger;
	BeaconSendingTerminalState_sp mTarget;
};

TEST_F(BeaconSendingTerminalStateTest, isTerminalStateIsTrueForTheTerminalState)
{
	EXPECT_TRUE(mTarget->isTerminalState());
}

TEST_F(BeaconSendingTerminalStateTest, theShutdownStateIsNeverTheSameReference)
{
	AbstractBeaconSendingState_sp obtained = mTarget->getShutdownState();

	ASSERT_NE(obtained, nullptr);
	EXPECT_NE(mTarget.get(), obtained.get());
}

TEST_F(BeaconSendingTerminalStateTest, executeRequestsShutdown)
{
	MockStrictBeaconSendingContext_t mockContext(mLogger);//StrictMock ensure that  all additional calls on context result in failure

	EXPECT_CALL(mockContext, requestShutdown())
		.Times(::testing::Exactly(1));

	EXPECT_CALL(mockContext, isShutdownRequested())
		.Times(::testing::Exactly(1));

	mTarget->execute(mockContext);
}

TEST_F(BeaconSendingTerminalStateTest, ToStringReturnsCorrectStateName)
{
	// given
	BeaconSendingTerminalState_t target;

	// when
	auto stateName = target.getStateName();

	// then
	ASSERT_STREQ(stateName, "Terminal");
}