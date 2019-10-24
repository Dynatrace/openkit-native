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

#include "mock/MockIOpenKitComposite.h"
#include "../../api/mock/MockILogger.h"
#include "../../protocol/mock/MockIBeacon.h"

#include "core/objects/WebRequestTracer.h"
#include "core/UTF8String.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>

using namespace test;

using MockNiceIBeacon_sp = std::shared_ptr<testing::NiceMock<MockIBeacon>>;
using MockNiceILogger_sp = std::shared_ptr<testing::NiceMock<MockILogger>>;
using MockNiceIOpenKitComposite_sp = std::shared_ptr<testing::NiceMock<MockIOpenKitComposite>>;
using MockStrictIBeacon_sp = std::shared_ptr<testing::StrictMock<MockIBeacon>>;
using WebRequestTracer_t = core::objects::WebRequestTracer;
using WebRequestTracer_sp = std::shared_ptr<WebRequestTracer_t>;
using Utf8String_t = core::UTF8String;

static const char APP_ID[] = "appID";
static const char APP_NAME[] = "appName";
static const char TAG[] = "THE_TAG";

class WebRequestTracerTest : public testing::Test
{
protected:

	MockNiceILogger_sp mockLogger;
	MockNiceIBeacon_sp mockBeaconNice;
	MockNiceIOpenKitComposite_sp mockParentNice;

	void SetUp()
	{
		mockLogger = MockILogger::createNice();

		mockBeaconNice = MockIBeacon::createNice();

		mockParentNice = MockIOpenKitComposite::createNice();
	}

	void TearDown()
	{
	}

	WebRequestTracer_sp createTracer()
	{
		return createTracer(mockBeaconNice);
	}

	WebRequestTracer_sp createTracer(std::shared_ptr<MockIBeacon> beacon)
	{
		return std::make_shared<WebRequestTracer_t>(
			mockLogger,
			mockParentNice,
			beacon,
			""
		);
	}
};



TEST_F(WebRequestTracerTest, defaultValues)
{
	//given
	ON_CALL(*mockBeaconNice, createSequenceNumber())
		.WillByDefault(testing::Return(42));
	ON_CALL(*mockBeaconNice, getCurrentTimestamp())
		.WillByDefault(testing::Return(123456789L));

	// test the constructor call
	auto testWebRequestTracer = createTracer();

	// verify default values
	auto tracerURL = testWebRequestTracer->getURL();
	ASSERT_TRUE(Utf8String_t(tracerURL).equals(Utf8String_t("<unknown>")));
	ASSERT_EQ(testWebRequestTracer->getResponseCode(), -1);
	ASSERT_EQ(testWebRequestTracer->getStartTime(), 123456789L);
	ASSERT_EQ(testWebRequestTracer->getEndTime(), -1);
	ASSERT_EQ(testWebRequestTracer->getStartSequenceNo(), 42);
	ASSERT_EQ(testWebRequestTracer->getEndSequenceNo(), -1);
	ASSERT_EQ(testWebRequestTracer->getBytesSent(), -1);
	ASSERT_EQ(testWebRequestTracer->getBytesReceived(), -1);
}

TEST_F(WebRequestTracerTest, getTag)
{
	// given
	Utf8String_t theTag(TAG);
	ON_CALL(*mockBeaconNice, createTag(testing::_, testing::_))
		.WillByDefault(testing::Return(theTag));
	auto testWebRequestTracer = createTracer();

	//verify
	const char* actualValue = testWebRequestTracer->getTag();
	auto test = theTag.equals(actualValue);
	ASSERT_TRUE(test);
}

TEST_F(WebRequestTracerTest, aNewlyCreatedWebRequestTracerIsNotStopped)
{
	// given
	auto testWebRequestTracer = createTracer();

	//verify
	ASSERT_FALSE(testWebRequestTracer->isStopped());
}

TEST_F(WebRequestTracerTest, aWebRequestTracerIsStoppedAfterStopHasBeenCalled)
{
	// given
	auto testWebRequestTracer = createTracer();

	// when
	testWebRequestTracer->stop();

	// verify
	ASSERT_TRUE(testWebRequestTracer->isStopped());
}

TEST_F(WebRequestTracerTest, aWebRequestTracerIsStoppedAfterStopWithResponseCodeHasBeenCalled)
{
	// given
	auto testWebRequestTracer = createTracer();

	// when
	testWebRequestTracer->stop(200);

	// verify
	ASSERT_TRUE(testWebRequestTracer->isStopped());
}

TEST_F(WebRequestTracerTest, setResponseCodeSetsTheResponseCode)
{
	// given
	auto testWebRequestTracer = createTracer();

	// when
	auto obtained = testWebRequestTracer->setResponseCode(418);

	// verify
	ASSERT_EQ(obtained, testWebRequestTracer);
	ASSERT_EQ(testWebRequestTracer->getResponseCode(), 418);
}

TEST_F(WebRequestTracerTest, stopWithResponseCodeSetsTheResponseCode)
{
	// given
	auto testWebRequestTracer = createTracer();

	// when
	testWebRequestTracer->stop(418);

	// verify
	ASSERT_EQ(testWebRequestTracer->getResponseCode(), 418);
}

TEST_F(WebRequestTracerTest, setResponseCodeDoesNotSetTheResponseCodeIfStopped)
{
	// given
	auto testWebRequestTracer = createTracer();

	// when
	testWebRequestTracer->stop();
	auto obtained = testWebRequestTracer->setResponseCode(418);

	// verify
	ASSERT_EQ(obtained, testWebRequestTracer);
	ASSERT_EQ(testWebRequestTracer->getResponseCode(), -1);
}

TEST_F(WebRequestTracerTest, stopWithResponseCodeDoesNotSetTheResponseCodeIfStopped)
{
	// given
	auto testWebRequestTracer = createTracer();
	testWebRequestTracer->stop(200);

	// when
	testWebRequestTracer->stop(404);

	// verify
	ASSERT_EQ(testWebRequestTracer->getResponseCode(), 200);
}

TEST_F(WebRequestTracerTest, setBytesSentSetsTheNumberOfSentBytes)
{
	// given
	auto testWebRequestTracer = createTracer();

	// when
	auto obtained = testWebRequestTracer->setBytesSent(1234);

	// verify
	ASSERT_EQ(obtained, testWebRequestTracer);
	ASSERT_EQ(testWebRequestTracer->getBytesSent(), 1234);
}

TEST_F(WebRequestTracerTest, setBytesSentDoesNotSetAnythingIfStopped)
{
	// given
	auto testWebRequestTracer = createTracer();

	// when
	testWebRequestTracer->stop();
	auto obtained = testWebRequestTracer->setBytesSent(1234 );

	// verify
	ASSERT_EQ(obtained, testWebRequestTracer);
	ASSERT_EQ(testWebRequestTracer->getBytesSent(), -1);
}

TEST_F(WebRequestTracerTest, setBytesSentDoesNotSetAnythingIfStoppedWithResponseCode)
{
	// given
	auto testWebRequestTracer = createTracer();

	// when
	testWebRequestTracer->stop(200);
	auto obtained = testWebRequestTracer->setBytesSent(1234 );

	// verify
	ASSERT_EQ(obtained, testWebRequestTracer);
	ASSERT_EQ(testWebRequestTracer->getBytesSent(), -1);
}

TEST_F(WebRequestTracerTest, setBytesReceivedSetsTheNumberOfReceivedBytes)
{
	// given
	auto testWebRequestTracer = createTracer();

	// when
	auto obtained = testWebRequestTracer->setBytesReceived(1234);

	// verify
	ASSERT_EQ(obtained, testWebRequestTracer);
	ASSERT_EQ(testWebRequestTracer->getBytesReceived(), 1234);
}

TEST_F(WebRequestTracerTest, setBytesReceivedDoesNotSetAnythingIfStopped)
{
	// given
	auto testWebRequestTracer = createTracer();

	// when
	testWebRequestTracer->stop();
	auto obtained = testWebRequestTracer->setBytesReceived(1234);

	// verify
	ASSERT_EQ(obtained, testWebRequestTracer);
	ASSERT_EQ(testWebRequestTracer->getBytesReceived(), -1);
}

TEST_F(WebRequestTracerTest, setBytesReceivedDoesNotSetAnythingIfStoppedWithResponseCode)
{
	// given
	auto testWebRequestTracer = createTracer();

	// when
	testWebRequestTracer->stop(200);
	auto obtained = testWebRequestTracer->setBytesReceived(1234);

	// verify
	ASSERT_EQ(obtained, testWebRequestTracer);
	ASSERT_EQ(testWebRequestTracer->getBytesReceived(), -1);
}

TEST_F(WebRequestTracerTest, startSetsTheStartTime)
{
	// given
	ON_CALL(*mockBeaconNice, getCurrentTimestamp())
		.WillByDefault(testing::Return(123456789L));
	auto testWebRequestTracer = createTracer();

	// when
	auto obtained = testWebRequestTracer->start();

	// verify
	ASSERT_EQ(obtained, testWebRequestTracer);
	ASSERT_EQ(testWebRequestTracer->getStartTime(), 123456789L);
}

TEST_F(WebRequestTracerTest, startDoesNothingIfAlreadyStopped)
{
	// given
	ON_CALL(*mockBeaconNice, getCurrentTimestamp())
		.WillByDefault(testing::Return(123456789L));
	auto testWebRequestTracer = createTracer();

	// when
	testWebRequestTracer->stop();
	auto obtained = testWebRequestTracer->start();

	// verify
	ASSERT_EQ(obtained, testWebRequestTracer);
	ASSERT_EQ(testWebRequestTracer->getStartTime(), 123456789L);
}

TEST_F(WebRequestTracerTest, startDoesNothingIfAlreadyStoppedWithResponseCode)
{
	// given
	ON_CALL(*mockBeaconNice, getCurrentTimestamp())
		.WillByDefault(testing::Return(123456789L));
	auto testWebRequestTracer = createTracer();

	// when
	testWebRequestTracer->stop(200);
	auto obtained = testWebRequestTracer->start();

	// verify
	ASSERT_EQ(obtained, testWebRequestTracer);
	ASSERT_EQ(testWebRequestTracer->getStartTime(), 123456789L);
}

TEST_F(WebRequestTracerTest, stopCanOnlyBeExecutedOnce)
{
	// given
	ON_CALL(*mockBeaconNice, createSequenceNumber())
		.WillByDefault(testing::Return(42));

	// when
	EXPECT_CALL(*mockBeaconNice, createSequenceNumber())
		.Times(testing::Exactly(2));
	EXPECT_CALL(*mockBeaconNice, addWebRequest(testing::_, testing::_))
		.Times(testing::Exactly(1));

	auto testWebRequestTracer = createTracer();

	testWebRequestTracer->stop();
	ASSERT_EQ(testWebRequestTracer->getEndSequenceNo(), 42);

	// when called another time
	testing::Mock::VerifyAndClearExpectations(&mockBeaconNice);

	EXPECT_CALL(*mockBeaconNice, createSequenceNumber())
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mockBeaconNice, addWebRequest(testing::_, testing::_))
		.Times(testing::Exactly(0));

	testWebRequestTracer->stop();
}

TEST_F(WebRequestTracerTest, stopWithResponseCodeCanOnlyBeExecutedOnce)
{
	// given
	ON_CALL(*mockBeaconNice, createSequenceNumber())
		.WillByDefault(testing::Return(42));

	// when
	EXPECT_CALL(*mockBeaconNice, createSequenceNumber())
		.Times(testing::Exactly(2));
	EXPECT_CALL(*mockBeaconNice, addWebRequest(testing::_, testing::_))
		.Times(testing::Exactly(1));

	auto testWebRequestTracer = createTracer();

	testWebRequestTracer->stop(200);
	ASSERT_EQ(testWebRequestTracer->getEndSequenceNo(), 42);

	// when called another time
	testing::Mock::VerifyAndClearExpectations(&mockBeaconNice);

	EXPECT_CALL(*mockBeaconNice, createSequenceNumber())
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mockBeaconNice, addWebRequest(testing::_, testing::_))
		.Times(testing::Exactly(0));

	testWebRequestTracer->stop(404);
}

TEST_F(WebRequestTracerTest, aNewlyCreatedWebRequestTracerDoesNotAttachToTheParent)
{
	// expect
	EXPECT_CALL(*mockParentNice, getActionId())
		.Times(testing::Exactly(2));

	// given, when
	auto target = createTracer();

	// then
	auto obtained = target->getParent();
	ASSERT_THAT(mockParentNice, testing::Eq(obtained));

}