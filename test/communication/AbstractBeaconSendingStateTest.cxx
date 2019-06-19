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

#include "MockAbstractBeaconSendingState.h"
#include "MockBeaconSendingContext.h"
#include "configuration/HTTPClientConfiguration.h"
#include "configuration/Configuration.h"
#include "providers/DefaultHTTPClientProvider.h"
#include "providers/DefaultTimingProvider.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

class AbstractBeaconSendingStateTest : public testing::Test
{
protected:
	AbstractBeaconSendingStateTest()
		: mLogger(nullptr)
	{
	}

	void SetUp()
	{
		mLogger = std::make_shared<core::util::DefaultLogger>(devNull, openkit::LogLevel::LOG_LEVEL_DEBUG);
	}

	void TearDown()
	{
		mLogger = nullptr;
	}

	std::ostringstream devNull;
	std::shared_ptr<openkit::ILogger> mLogger;
	
};

TEST_F(AbstractBeaconSendingStateTest,  aTestBeaconSendingStateExecutes)
{
	testing::NiceMock<test::MockAbstractBeaconSendingState> mockState;
	testing::StrictMock<test::MockBeaconSendingContext> mockContext(mLogger);//StrictMock ensure that  all additional calls on context result in failure

	ON_CALL(mockState, execute(testing::_))
		.WillByDefault(testing::WithArgs<0>(testing::Invoke(&mockState, &test::MockAbstractBeaconSendingState::RealExecute)));

	// verify doExecute was called
	EXPECT_CALL(mockState, doExecute(testing::_))
		.Times(::testing::AtLeast(1));
	// also verify that shutdown requested is queried, but nothing else
	EXPECT_CALL(mockContext, isShutdownRequested())
		.Times(::testing::Exactly(1));

	// when calling execute
	mockState.execute(mockContext);
}
	


