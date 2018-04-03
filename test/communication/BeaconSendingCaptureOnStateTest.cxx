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

#include "communication/BeaconSendingCaptureOnState.h"
#include "communication/AbstractBeaconSendingState.h"

#include "MockBeaconSendingContext.h"
//#include "../protocol/MockHTTPClient.h"
#include "../protocol/MockStatusResponse.h"
#include "../communication/CustomMatchers.h"

class BeaconSendingCaptureOnStateTest : public testing::Test
{
public:

	BeaconSendingCaptureOnStateTest()
		: target(nullptr)
	{
	}

	void SetUp()
	{
		target = std::shared_ptr<communication::AbstractBeaconSendingState>(new communication::BeaconSendingCaptureOnState());
	}

	void TearDown()
	{
		target = nullptr;
	}

	std::shared_ptr<communication::AbstractBeaconSendingState> target;
};

TEST_F(BeaconSendingCaptureOnStateTest, aBeaconSendingCaptureOnStateIsNotATerminalState)
{
	// verify that BeaconSendingCaptureOnState is not a terminal state
	EXPECT_FALSE(target->isTerminalState());
}

// TODO: Add tests